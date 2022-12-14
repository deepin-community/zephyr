#ifndef __HM_H__
#define __HM_H__
/* This file is part of the Project Athena Zephyr Notification System.
 * It contains the hostmanager header file.
 *
 *      Created by:     David C. Jedlinsky
 *
 *      $Id: 40cd34e04bf377f3f2de8f4fdf4beda0420130f2 $
 *
 *      Copyright (c) 1987, 1991 by the Massachusetts Institute of Technology.
 *      For copying and distribution information, see the file
 *      "mit-copyright.h". 
 */

#include <zephyr/mit-copyright.h>
#include <internal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "timer.h"

/* These macros are for insertion into and deletion from a singly-linked list
 * with back pointers to the previous element's next pointer.  In order to
 * make these macros act like expressions, they use the comma operator for
 * sequenced evaluations of assignment, and "a && b" for "evaluate assignment
 * b if expression a is true". */
#define LIST_INSERT(head, elem) \
	((elem)->next = *(head), \
	 (*head) && ((*(head))->prev_p = &(elem)->next), \
	 (*head) = (elem), (elem)->prev_p = (head))
#define LIST_DELETE(elem) \
	(*(elem)->prev_p = (elem)->next, \
	 (elem)->next && ((elem)->next->prev_p = (elem)->prev_p))

#ifdef DEBUG
#define DPR(a) fprintf(stderr, a); fflush(stderr)
#define DPR2(a,b) fprintf(stderr, a, b); fflush(stderr)
#define Zperr(e) fprintf(stderr, "Error = %d\n", e)
#else
#define DPR(a)
#define DPR2(a,b)
#define Zperr(e)
#endif

#define BOOTING 1
#define NOTICES 2

/* main.c */
void die_gracefully(void);

/* zhm_client.c */
void transmission_tower(ZNotice_t *, char *, int);
Code_t send_outgoing(ZNotice_t *);

/* queue.c */
void init_queue(void);
Code_t add_notice_to_queue(ZNotice_t *, char *, struct sockaddr_in *,
				int);
Code_t remove_notice_from_queue(ZNotice_t *, ZNotice_Kind_t *,
				     struct sockaddr_in *);
void retransmit_queue(struct sockaddr_in *);
void disable_queue_retransmits(void);
int queue_len(void);

/* zhm.c */
extern void new_server(char *sugg_serv);
extern void send_boot_notice(char *);
extern void send_flush_notice(char *);

/* zhm_server.c */
extern void find_next_server(char *);
extern void server_manager(ZNotice_t *);

extern u_short cli_port;
extern char **serv_list;
extern char cur_serv[], prim_serv[];
extern struct sockaddr_in cli_sin, serv_sin, from;
extern int no_server, deactivated, noflushflag, rebootflag;
extern int timeout_type, hmdebug, nservchang, booting, nclt, nserv, numserv;
extern int rexmit_times[];

#ifdef HAVE_ETEXT
extern int etext;
#define adjust_size(size)	size -= (unsigned long) &etext;
#else
/* Pick a var that tends to be near the start of data section.  */
extern char **environ;
#define adjust_size(size)	size -= (uintptr_t) &environ
#endif

#endif
