/* This file is part of the Project Athena Zephyr Notification System.
 * It contains functions for the User Locator service.
 *
 *	Created by:	John T. Kohl
 *
 *	$Id: ab936b831686dc54916c0f9b27aebe8b02e3f553 $
 *
 *	Copyright (c) 1987,1988 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h".
 */

#include <zephyr/mit-copyright.h>
#include "zserver.h"
#include <sys/socket.h>

#ifndef lint
#ifndef SABER
static const char rcsid_uloc_c[] =
"$Id: ab936b831686dc54916c0f9b27aebe8b02e3f553 $";
#endif /* SABER */
#endif /* lint */

/*
 * The user locator functions.
 *
 * External functions:
 *
 * void ulocate_dispatch(notice, auth, who, server)
 *	ZNotice_t *notice;
 *	int auth;
 *	struct sockaddr_in *who;
 *	Server *server;
 *
 * void ulogin_dispatch(notice, auth, who, server)
 *	ZNotice_t *notice;
 *	int auth;
 *	struct sockaddr_in *who;
 *	Server *server;
 *
 * void uloc_hflush(addr)
 *	struct in_addr *addr;
 *
 * void uloc_flush_client(sin)
 *	struct sockaddr_in *sin;
 *
 * Code_t uloc_send_locations()
 *
 * void uloc_dump_locs(fp)
 *	FILE *fp;
 */

/*
 * The user locator.
 * We maintain an array of Location sorted by user (so we can do
 * binary searches), growing and shrinking it as necessary.
 */

/* WARNING: make sure this is the same as the number of strings you */
/* plan to hand back to the user in response to a locate request, */
/* else you will lose.  See ulogin_locate() and uloc_send_locations() */
#define	NUM_FIELDS	3

#define NOLOC		1
#define QUIET		-1
#define UNAUTH		-2

static void ulogin_locate(ZNotice_t *notice, struct sockaddr_in *who,
			  int auth);
void ulogin_flush_user(ZNotice_t *notice);
int ulogin_find(char *user, struct in_addr *host,
                      unsigned int port);
int ulogin_find_user(char *user);
static int ulogin_setup(ZNotice_t *notice, Location *locs,
			Exposure_type exposure, struct sockaddr_in *who);
int ulogin_add_user(ZNotice_t *notice, Exposure_type exposure,
			   struct sockaddr_in *who);
static int ulogin_parse(ZNotice_t *notice, Location *locs);
Exposure_type ulogin_remove_user(ZNotice_t *notice,
                                 struct sockaddr_in *who,
                                 int *err_return);
static void login_sendit(ZNotice_t *notice, int auth,
			 struct sockaddr_in *who, int external);
static char **ulogin_marshal_locs(ZNotice_t *notice, int *found, int auth);

static void free_loc(Location *loc);
static void ulogin_locate_forward(ZNotice_t *notice, struct sockaddr_in *who,
				  ZRealm *realm);
static int reallocate_locations(int new_num);

Location *locations = NULL; /* ptr to first in array */
static int num_locs = 0;	/* number in array */

/*
 * Dispatch a LOGIN notice.
 */

Code_t
ulogin_dispatch(ZNotice_t *notice,
		int auth,
		struct sockaddr_in *who,
		Server *server)
{
    Exposure_type retval;
    int err_ret;

    if (strcmp(notice->z_opcode, LOGIN_USER_LOGOUT) == 0) {
	retval = ulogin_remove_user(notice, who, &err_ret);
	switch (retval) {
	  case NONE:
	    if (err_ret == UNAUTH) {
		if (server == me_server)
		    clt_ack(notice, who, AUTH_FAILED);
		return ZERR_NONE;
	    } else if (err_ret == NOLOC) {
		if (server == me_server)
		    clt_ack(notice, who, NOT_FOUND);
		return ZERR_NONE;
	    }
	    syslog(LOG_ERR,"bogus location exposure NONE, %s",
		   notice->z_sender);
	    break;
	  case OPSTAFF_VIS:
	  case REALM_VIS:
	    /* he is not announced to people.  Silently ack */
	    if (server == me_server)
		ack(notice, who);
	    break;
	  case REALM_ANN:
	  case NET_VIS:
	    if (server == me_server)
		sendit(notice, 1, who, 0);
	    break;
	  case NET_ANN:
	    /* currently no distinction between these.
	       just announce */
	    /* we assume that if this user is at a certain
	       IP address, we can trust the logout to be
	       authentic.  ulogin_remove_user checks the
	       ip addrs */
	    if (server == me_server)
		sendit(notice, 1, who, 1);
	    break;
	  default:
	    syslog(LOG_ERR,"bogus location exposure %d/%s",
		   (int) retval, notice->z_sender);
	    break;
	}
	if (server == me_server) /* tell the other servers */
	    server_forward(notice, auth, who);
	return ZERR_NONE;
    }
    if (!bdumping &&
	(!auth || strcmp(notice->z_sender, notice->z_class_inst) != 0) &&
	(!auth || !opstaff_check(notice->z_sender)))  {
	zdbug((LOG_DEBUG,"unauthentic ulogin: %d %s %s", auth,
	       notice->z_sender, notice->z_class_inst));
	if (server == me_server)
	    clt_ack(notice, who, AUTH_FAILED);
	return ZERR_NONE;
    }
    if (strcmp(notice->z_opcode, LOGIN_USER_FLUSH) == 0) {
	ulogin_flush_user(notice);
	if (server == me_server)
	    ack(notice, who);
    } else if (strcmp(notice->z_opcode, EXPOSE_NONE) == 0) {
	ulogin_remove_user(notice, who, &err_ret);
	if (err_ret == UNAUTH) {
	    if (server == me_server)
		clt_ack(notice, who, AUTH_FAILED);
	    return ZERR_NONE;
	} else if (err_ret == NOLOC) {
	    if (server == me_server)
		clt_ack(notice, who, NOT_FOUND);
	    return ZERR_NONE;
	}
	if (server == me_server) {
	    ack(notice, who);
	    server_forward(notice, auth, who);
	}
	return ZERR_NONE;
    } else if (strcmp(notice->z_opcode, EXPOSE_OPSTAFF) == 0) {
	err_ret = ulogin_add_user(notice, OPSTAFF_VIS, who);
	if (server == me_server) {
	    if (err_ret)
		nack(notice, who);
	    else
		ack(notice, who);
	}
    } else if (strcmp(notice->z_opcode, EXPOSE_REALMVIS) == 0) {
	err_ret = ulogin_add_user(notice, REALM_VIS, who);
	if (server == me_server) { /* realm vis is not broadcast,
				      so we ack it here */
	    if (err_ret)
		nack(notice, who);
	    else
		ack(notice, who);
	}
    } else if (!strcmp(notice->z_opcode, EXPOSE_REALMANN)) {
	err_ret = ulogin_add_user(notice, REALM_ANN, who);
	if (server == me_server) { /* announce to the realm */
	    if (err_ret)
		nack(notice, who);
	    else
		login_sendit(notice, auth, who, 0);
	}
    } else if (!strcmp(notice->z_opcode, EXPOSE_NETVIS)) {
	err_ret = ulogin_add_user(notice, NET_VIS, who);
	if (server == me_server) { /* announce to the realm */
	    if (err_ret)
		nack(notice, who);
	    else
		login_sendit(notice, auth, who, 0);
	}
    } else if (!strcmp(notice->z_opcode, EXPOSE_NETANN)) {
	err_ret = ulogin_add_user(notice, NET_ANN, who);
	if (server == me_server) { /* tell the world */
	    if (err_ret)
		nack(notice, who);
	    else
		login_sendit(notice, auth, who, 1);
	}
    } else {
	if (!strcmp(notice->z_opcode, LOGIN_USER_LOGIN)) {
	    zdbug((LOG_DEBUG, "ulog opcode from unknown foreign realm %s",
		   notice->z_opcode));
	} else {
	    syslog(LOG_ERR, "unknown ulog opcode %s", notice->z_opcode);
	}
	if (server == me_server)
	    nack(notice, who);
	return ZERR_NONE;
    }
    if (server == me_server)
	server_forward(notice, auth, who);
    return ZERR_NONE;
}

static void
login_sendit(ZNotice_t *notice,
	     int auth,
	     struct sockaddr_in *who,
	     int external)
{
    ZNotice_t log_notice;

    /* we must copy the notice struct here because we need the original
       for forwarding.  We needn't copy the private data of the notice,
       since that isn't modified by sendit and its subroutines. */

    log_notice = *notice;

    log_notice.z_opcode = LOGIN_USER_LOGIN;
    sendit(&log_notice, auth, who, external);
}


/*
 * Dispatch a LOCATE notice.
 */
Code_t
ulocate_dispatch(ZNotice_t *notice,
		 int auth,
		 struct sockaddr_in *who,
		 Server *server)
{
    char *cp;
    ZRealm *realm;

    if (!strcmp(notice->z_opcode, LOCATE_LOCATE)) {
	/* we are talking to a current-rev client; send an ack */
	ack(notice, who);
	cp = strchr(notice->z_class_inst, '@');
	if (cp && (realm = realm_get_realm_by_name(cp + 1)))
	    ulogin_locate_forward(notice, who, realm);
	else
	    ulogin_locate(notice, who, auth);
	return ZERR_NONE;
    } else {
        syslog(LOG_ERR, "unknown uloc opcode %s", notice->z_opcode);
	if (server == me_server)
	    nack(notice, who);
	return ZERR_NONE;
    }
}

/*
 * reallocate locations
 */
static int
reallocate_locations(int new_num) {
    Location *new_loc;

    if (new_num) {
        new_loc = realloc(locations, new_num * sizeof(Location));
        if (new_loc == NULL) {
            syslog(LOG_ERR, "failed to resize uloc table: %m");
            if (new_num < num_locs)
                num_locs = new_num;
            return errno;
        }
        locations = new_loc;
    } else {
	free(locations);
	locations = NULL;
    }

    num_locs = new_num;

    return 0;
}

/*
 * Flush all locations at the address.
 */

void
uloc_hflush(struct in_addr *addr)
{
    int i, new_num = 0;

    if (num_locs == 0)
	return;			/* none to flush */

    for (i = 0; i < num_locs; i++)
	if (locations[i].addr.sin_addr.s_addr != addr->s_addr)
	    locations[new_num++] = locations[i];
	else
	    free_loc(&locations[i]);

    reallocate_locations(new_num);
}

void
uloc_flush_client(struct sockaddr_in *sin)
{
    int i, new_num = 0;

    if (num_locs == 0)
	return;			/* none to flush */

    for (i = 0; i < num_locs; i++)
	if ((locations[i].addr.sin_addr.s_addr != sin->sin_addr.s_addr)
	    || (locations[i].addr.sin_port != sin->sin_port))
	    locations[new_num++] = locations[i];
	else
	    free_loc(&locations[i]);

    num_locs = new_num;

    reallocate_locations(new_num);
}

/*
 * Send the locations for host for a brain dump
 */

/*ARGSUSED*/
Code_t
uloc_send_locations(void)
{
    Location *loc;
    int i;
    char *lyst[NUM_FIELDS];
    char *exposure_level;
    Code_t retval;

    for (i = 0, loc = locations; i < num_locs; i++, loc++) {
	lyst[0] = (char *) loc->machine->string;
	lyst[1] = (char *) loc->time;
	lyst[2] = (char *) loc->tty->string;

	switch (loc->exposure) {
	  case OPSTAFF_VIS:
	    exposure_level = EXPOSE_OPSTAFF;
	    break;
	  case REALM_VIS:
	    exposure_level = EXPOSE_REALMVIS;
	    break;
	  case REALM_ANN:
	    exposure_level = EXPOSE_REALMANN;
	    break;
	  case NET_VIS:
	    exposure_level = EXPOSE_NETVIS;
	    break;
	  case NET_ANN:
	    exposure_level = EXPOSE_NETANN;
	    break;
	  default:
	    syslog(LOG_ERR,"broken location state %s/%d",
		   loc->user->string, (int) loc->exposure);
	    exposure_level = EXPOSE_OPSTAFF;
	    break;
	}
	retval = bdump_send_list_tcp(ACKED, &loc->addr, LOGIN_CLASS,
				     loc->user->string, exposure_level, myname,
				     "", lyst, NUM_FIELDS);
	if (retval != ZERR_NONE) {
	    syslog(LOG_ERR, "uloc_send_locs: %s", error_message(retval));
	    return(retval);
	}
    }
    return ZERR_NONE;
}

/*
 * Add the user to the internal table of locations.
 */

int
ulogin_add_user(ZNotice_t *notice,
		Exposure_type exposure,
		struct sockaddr_in *who)
{
    Location newloc;
    int i, here;

    here = ulogin_find(notice->z_class_inst, &who->sin_addr, notice->z_port);
    if (here >= 0) {
	/* Update the time, tty, and exposure on the existing location. */
	locations[here].exposure = exposure;
	if (ulogin_parse(notice, &newloc) == 0) {
	    free_string(locations[here].tty);
	    locations[here].tty = dup_string(newloc.tty);
	    free(locations[here].time);
	    locations[here].time = strsave(newloc.time);
	    free_loc(&newloc);
	}
	return 0;
    }

    if (ulogin_setup(notice, &newloc, exposure, who))
        return 1;

    if(reallocate_locations(num_locs + 1))
        return 1;

    /* skip old locs */
    for (i = 0;
         i < num_locs-1 && comp_string(locations[i].user, newloc.user) < 0;
         i++)
        ;

    here = i;

    /* copy the rest */
    for(i = num_locs - 1; i > here; i--)
        locations[i] = locations[i - 1];

    locations[here] = newloc;

    return 0;
}

/*
 * Set up the location locs with the information in the notice.
 */

static int
ulogin_setup(ZNotice_t *notice,
	     Location *locs,
	     Exposure_type exposure,
	     struct sockaddr_in *who)
{
    if (ulogin_parse(notice, locs))
	return 1;

    locs->exposure = exposure;
    locs->addr.sin_family = AF_INET;
    locs->addr.sin_addr.s_addr = who->sin_addr.s_addr;
    locs->addr.sin_port = notice->z_port;
    return(0);
}

/*
 * Parse the location information in the notice, and fill it into *locs
 */

static int
ulogin_parse(ZNotice_t *notice,
	     Location *locs)
{
    char *cp, *base;
    int nulls = 0;

    if (!notice->z_message_len) {
	syslog(LOG_ERR, "short ulogin");
	return 1;
    }

    base = notice->z_message;
    for (cp = base; cp < base + notice->z_message_len; cp++) {
	if (!*cp)
	    nulls++;
    }
    if (nulls < 3) {
	syslog(LOG_ERR, "zloc bad format from user %s (only %d fields)",
	       notice->z_sender, nulls);
	return 1;
    }

    locs->user = make_string(notice->z_class_inst,0);

    cp = base;
    locs->machine = make_string(cp,0);

    cp += (strlen(cp) + 1);
    locs->time = strsave(cp);

    /* This field might not be null-terminated */
    cp += (strlen(cp) + 1);
    locs->tty = make_string(cp, 0);

    return 0;
}

int
ulogin_find(char *user,
	    struct in_addr *host,
	    unsigned int port)
{
    int i;
    String *str;

    /* Find the first location for this user. */
    i = ulogin_find_user(user);
    if (i == -1)
	return -1;

    /* Look for a location which matches the host and port. */
    str = make_string(user, 0);
    while (i < num_locs && locations[i].user == str) {
	if (locations[i].addr.sin_addr.s_addr == host->s_addr
	    && locations[i].addr.sin_port == port) {
	    free_string(str);
	    return i;
	}
	i++;
    }

    free_string(str);
    return -1;
}

/*
 * Return the table index of the first instance of this user@realm in the
 * table.
 */
int
ulogin_find_user(char *user)
{
    int i, rlo, rhi;
    int compar;
    String *str;

    if (!locations)
	return -1;

    str = make_string(user, 0);

    /* i is the current midpoint location, rlo is the lowest we will
     * still check, and rhi is the highest we will still check. */

    i = num_locs / 2;
    rlo = 0;
    rhi = num_locs - 1;

    while ((compar = comp_string(locations[i].user, str)) != 0) {
	if (compar < 0)
	    rlo = i + 1;
	else
	    rhi = i - 1;
	if (rhi - rlo < 0) {
	    free_string(str);
	    return -1;
	}
	i = (rhi + rlo) / 2;
    }

    /* Back up to the first location for this user. */
    while (i > 0 && locations[i - 1].user == str)
	i--;
    free_string(str);
    return i;
}

/*
 * remove the user specified in notice from the internal table
 */

Exposure_type
ulogin_remove_user(ZNotice_t *notice,
		   struct sockaddr_in *who,
		   int *err_return)
{
    int here, i;
    Exposure_type quiet;

    *err_return = 0;
    here = ulogin_find(notice->z_class_inst, &who->sin_addr, notice->z_port);
    if (here == -1) {
	*err_return = NOLOC;
	return NONE;
    }

    quiet = locations[here].exposure;

    /* free up this one */
    free_loc(&locations[here]);

    for(i = here; i < num_locs - 1; i++)
        locations[i] = locations[i + 1];

    reallocate_locations(num_locs - 1);

    /* all done */
    return quiet;
}

/*
 * remove all locs of the user specified in notice from the internal table
 */

void
ulogin_flush_user(ZNotice_t *notice)
{
    int here, i, j, num_match, num_left;

    i = num_match = num_left = 0;

    here = ulogin_find_user(notice->z_class_inst);
    if (here == -1)
	return;

    num_left = num_locs - here;

    while (num_left &&
	   !strcasecmp(locations[here + num_match].user->string,
		       notice->z_class_inst)) {
	/* as long as we keep matching, march up the list */
	num_match++;
	num_left--;
    }

    for (j = 0; j < num_match; j++)
        free_loc(&locations[here + j]);

    /* copy the rest */
    for (i = here; i < num_locs - num_match; i++)
        locations[i] = locations[i + 1];

    reallocate_locations(num_locs - num_match);
}


static void
ulogin_locate(ZNotice_t *notice,
	      struct sockaddr_in *who,
	      int auth)
{
    char **answer;
    int found;
    Code_t retval;
    struct sockaddr_in send_to_who;

    answer = ulogin_marshal_locs(notice, &found, auth);

    send_to_who = *who;
    send_to_who.sin_port = notice->z_port;

    retval = ZSetDestAddr(&send_to_who);
    if (retval != ZERR_NONE) {
	syslog(LOG_WARNING, "ulogin_locate set addr: %s",
	       error_message(retval));
	if (answer)
	    free(answer);
	return;
    }

    notice->z_kind = ACKED;

    /* use xmit_frag() to send each piece of the notice */

    retval = ZSrvSendRawList(notice, answer, found * NUM_FIELDS, xmit_frag);
    if (retval != ZERR_NONE)
	syslog(LOG_WARNING, "ulog_locate xmit: %s", error_message(retval));
    if (answer)
	free(answer);
}

/*
 * Locate the user and collect the locations into an array.  Return the # of
 * locations in *found.
 */

static char **
ulogin_marshal_locs(ZNotice_t *notice,
		    int *found,
		    int auth)
{
    Location **matches = (Location **) 0;
    char **answer;
    int i = 0;
    String *inst;
    int local = (auth && realm_sender_in_realm(ZGetRealm(), notice->z_sender));
    int opstaff = (auth && opstaff_check(notice->z_sender));

    *found = 0;			/* # of matches */

    i = ulogin_find_user(notice->z_class_inst);
    if (i == -1)
	return NULL;

    inst = make_string(notice->z_class_inst,0);
    while (i < num_locs && (inst == locations[i].user)) {
	/* these locations match */
	switch (locations[i].exposure) {
	  case OPSTAFF_VIS:
	    if (!opstaff) {
		i++;
		continue;
	    }
	  case REALM_VIS:
	  case REALM_ANN:
	    if (!local) {
		i++;
		continue;
	    }
	  case NET_VIS:
	  case NET_ANN:
	  default:
	    break;
	}
	if (!*found) {
	    matches = (Location **) malloc(sizeof(Location *));
	    if (!matches) {
		syslog(LOG_ERR, "ulog_loc: no mem");
		break;	/* from the while */
	    }
	    matches[0] = &locations[i];
	    (*found)++;
	} else {
	    matches = (Location **) realloc(matches,
					    ++(*found) * sizeof(Location *));
	    if (!matches) {
		syslog(LOG_ERR, "ulog_loc: realloc no mem");
		*found = 0;
		break;	/* from the while */
	    }
	    matches[*found - 1] = &locations[i];
	}
	i++;
    }
    free_string(inst);

    /* OK, now we have a list of user@host's to return to the client
       in matches */


#ifdef DEBUG
    if (zdebug) {
	for (i = 0; i < *found ; i++)
	    zdbug((LOG_DEBUG,"found %s",
		   matches[i]->user->string));
    }
#endif

    /* coalesce the location information into a list of char *'s */
    answer = (char **) malloc((*found) * NUM_FIELDS * sizeof(char *));
    if (!answer) {
	syslog(LOG_ERR, "zloc no mem(answer)");
	*found = 0;
    } else
	for (i = 0; i < *found ; i++) {
	    answer[i * NUM_FIELDS] = matches[i]->machine->string;
	    answer[i * NUM_FIELDS + 1] = matches[i]->time;
	    answer[i * NUM_FIELDS + 2] = matches[i]->tty->string;
	}

    if (matches)
	free(matches);
    return answer;
}

void
uloc_dump_locs(FILE *fp)
{
    int i;

    for (i = 0; i < num_locs; i++) {
	fputs("'", fp);
	dump_quote(locations[i].user->string, fp);
	fputs("' '", fp);
	dump_quote(locations[i].machine->string, fp);
	fputs("' '", fp);
	dump_quote(locations[i].time, fp);
	fputs("' '", fp);
	dump_quote(locations[i].tty->string, fp);
	fputs("' ", fp);
	switch (locations[i].exposure) {
	  case OPSTAFF_VIS:
	    fputs("OPSTAFF", fp);
	    break;
	  case REALM_VIS:
	    fputs("RLM_VIS", fp);
	    break;
	  case REALM_ANN:
	    fputs("RLM_ANN", fp);
	    break;
	  case NET_VIS:
	    fputs("NET_VIS", fp);
	    break;
	  case NET_ANN:
	    fputs("NET_ANN", fp);
	    break;
	  default:
	    fprintf(fp, "? %d ?", locations[i].exposure);
	    break;
	}
	fprintf(fp, " %s/%d\n", inet_ntoa(locations[i].addr.sin_addr),
		ntohs(locations[i].addr.sin_port));
    }
}

static void
free_loc(Location *loc)
{
    free_string(loc->user);
    free_string(loc->machine);
    free_string(loc->tty);
    free(loc->time);
    return;
}

static void
ulogin_locate_forward(ZNotice_t *notice,
		      struct sockaddr_in *who,
		      ZRealm *realm)
{
    ZNotice_t lnotice;

    lnotice = *notice;
    lnotice.z_opcode = REALM_REQ_LOCATE;

    realm_handoff(&lnotice, 1, who, realm, 0);
}

void
ulogin_realm_locate(ZNotice_t *notice,
		    struct sockaddr_in *who,
		    ZRealm *realm)
{
  char **answer;
  int found;
  Code_t retval;
  ZNotice_t lnotice;
  char *pack;
  int packlen;

#ifdef DEBUG
  if (zdebug)
    zdbug((LOG_DEBUG, "ulogin_realm_locate"));
#endif

  answer = ulogin_marshal_locs(notice, &found, 0/*AUTH*/);

  lnotice = *notice;
  lnotice.z_opcode = REALM_ANS_LOCATE;

  if ((retval = ZFormatRawNoticeList(&lnotice, answer, found * NUM_FIELDS, &pack, &packlen)) != ZERR_NONE) {
    syslog(LOG_WARNING, "ulog_rlm_loc format: %s",
           error_message(retval));

    if (answer)
      free(answer);
    return;
  }
  if (answer)
    free(answer);

  if ((retval = ZParseNotice(pack, packlen, &lnotice)) != ZERR_NONE) {
    syslog(LOG_WARNING, "subscr_rlm_sendit parse: %s",
           error_message(retval));
    free(pack);
    return;
  }

  realm_handoff(&lnotice, 1, who, realm, 0);
  free(pack);

  return;
}

void
ulogin_relay_locate(ZNotice_t *notice,
		    struct sockaddr_in *who)
{
  ZNotice_t lnotice;
  Code_t retval;
  struct sockaddr_in newwho;
  char *pack;
  int packlen;

  notice_extract_address(notice, &newwho);

  if ((retval = ZSetDestAddr(&newwho)) != ZERR_NONE) {
    syslog(LOG_WARNING, "uloc_relay_loc set addr: %s",
           error_message(retval));
    return;
  }

  lnotice = *notice;
  lnotice.z_opcode = LOCATE_LOCATE;
  lnotice.z_kind = ACKED;
  lnotice.z_auth = 0;
  lnotice.z_authent_len = 0;
  lnotice.z_ascii_authent = "";
  lnotice.z_checksum = 0;
  lnotice.z_ascii_checksum = "";

  if ((retval = ZFormatRawNotice(&lnotice, &pack, &packlen)) != ZERR_NONE) {
    syslog(LOG_WARNING, "ulog_relay_loc format: %s",
           error_message(retval));
    return;
  }

  if ((retval = ZSendPacket(pack, packlen, 0)) != ZERR_NONE) {
    syslog(LOG_WARNING, "ulog_relay_loc xmit: %s",
           error_message(retval));
  }
  free(pack);
}
