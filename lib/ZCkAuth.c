/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZCheckAuthentication function.
 *
 *	Created by:	Robert French
 *
 *	$Id: ea024a90d7d2417833aef9abd6e7eefccd900402 $
 *
 *	Copyright (c) 1987,1991 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */

#ifndef lint
static const char rcsid_ZCheckAuthentication_c[] =
    "$Id: ea024a90d7d2417833aef9abd6e7eefccd900402 $";
#endif

#include <internal.h>

#if defined(HAVE_KRB5) && !HAVE_KRB5_FREE_DATA
#define krb5_free_data(ctx, dat) free((dat)->data)
#endif

/* Check authentication of the notice.
   If it looks authentic but fails the Kerberos check, return -1.
   If it looks authentic and passes the Kerberos check, return 1.
   If it doesn't look authentic, return 0

   When not using Kerberos, return true if the notice claims to be authentic.
   Only used by clients; the server uses its own routine.
 */
Code_t
ZCheckAuthentication(ZNotice_t *notice,
		     struct sockaddr_in *from)
{
#if defined(HAVE_KRB4) && !defined(HAVE_KRB5)
    int result;
    ZChecksum_t our_checksum;
    C_Block *session;
    CREDENTIALS cred;

    /* If the value is already known, return it. */
    if (notice->z_checked_auth != ZAUTH_UNSET)
	return (notice->z_checked_auth);

    if (!notice->z_auth)
	return (ZAUTH_NO);

    if ((result = krb_get_cred(SERVER_SERVICE, SERVER_INSTANCE, 
			       __Zephyr_realm, &cred)) != 0)
	return (ZAUTH_NO);

    session = (C_Block *)cred.session;

    our_checksum = des_quad_cksum((unsigned char *)notice->z_packet,
				  NULL, 
				  notice->z_default_format+
				  strlen(notice->z_default_format) + 1
				  - notice->z_packet,
				  0, session);

    /* if mismatched checksum, then the packet was corrupted */
    return ((our_checksum == notice->z_checksum) ? ZAUTH_YES : ZAUTH_FAILED);
#else
    return ZCheckZcodeAuthentication(notice, from);
#endif
} 
