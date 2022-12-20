/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZFormatAuthenticNotice function.
 *
 *	Created by:	Robert French
 *
 *	$Id: f658f7c339be8a94789a641b5a274794077bb3c4 $
 *
 *	Copyright (c) 1987,1988 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h".
 */

#ifndef lint
static const char rcsid_ZFormatAuthenticNotice_c[] = "$Id: f658f7c339be8a94789a641b5a274794077bb3c4 $";
#endif

#include <internal.h>

#ifdef HAVE_KRB4
Code_t
ZFormatAuthenticNotice(ZNotice_t *notice,
		       char *buffer,
		       int buffer_len,
		       int *len,
		       C_Block session)
{
    ZNotice_t newnotice;
    char *ptr;
    int retval, hdrlen;

    newnotice = *notice;
    newnotice.z_auth = 1;
    newnotice.z_authent_len = 0;
    newnotice.z_ascii_authent = "";

    if ((retval = Z_FormatRawHeader(&newnotice, buffer, buffer_len,
				    &hdrlen, &ptr, NULL)) != ZERR_NONE)
	return (retval);

    newnotice.z_checksum =
	(ZChecksum_t)des_quad_cksum((void *)buffer, NULL, ptr - buffer, 0, (C_Block *)session);

    if ((retval = Z_FormatRawHeader(&newnotice, buffer, buffer_len,
				    &hdrlen, NULL, NULL)) != ZERR_NONE)
	return (retval);

    ptr = buffer+hdrlen;

    if (newnotice.z_message_len+hdrlen > buffer_len)
	return (ZERR_PKTLEN);

    (void) memcpy(ptr, newnotice.z_message, newnotice.z_message_len);

    *len = hdrlen+newnotice.z_message_len;

    if (*len > Z_MAXPKTLEN)
	return (ZERR_PKTLEN);

    return (ZERR_NONE);
}
#endif

#ifdef HAVE_KRB5
Code_t
ZFormatAuthenticNoticeV5(ZNotice_t *notice,
			 register char *buffer,
			 register int buffer_len,
			 int *len,
			 krb5_keyblock *keyblock)
{
    ZNotice_t newnotice;
    char *ptr;
    int retval, hdrlen, hdr_adj;
    krb5_enctype enctype;
    krb5_cksumtype cksumtype;
#ifdef HAVE_KRB4
    int key_len;
#endif
    char *cksum_start, *cstart, *cend;
    int cksum_len;

#ifdef HAVE_KRB4
    key_len = Z_keylen(keyblock);
#endif
    retval = Z_ExtractEncCksum(keyblock, &enctype, &cksumtype);
    if (retval)
	return (ZAUTH_FAILED);

#ifdef HAVE_KRB4
    if (key_len == 8 && (enctype == (krb5_enctype)ENCTYPE_DES_CBC_CRC ||
                         enctype == (krb5_enctype)ENCTYPE_DES_CBC_MD4 ||
                         enctype == (krb5_enctype)ENCTYPE_DES_CBC_MD5)) {
         C_Block tmp;
         memcpy(&tmp, Z_keydata(keyblock), key_len);
         return ZFormatAuthenticNotice(notice, buffer, buffer_len, len,
                                       tmp);
    }
#endif

    newnotice = *notice;
    newnotice.z_auth = 1;
    newnotice.z_authent_len = 0;
    newnotice.z_ascii_authent = "";

    if ((retval = Z_NewFormatRawHeader(&newnotice, buffer, buffer_len,
                                       &hdrlen,
                                       &cksum_start, &cksum_len, &cstart,
                                       &cend)) != ZERR_NONE)
	return (retval);

    /* we know this is only called by the server */
    retval = Z_InsertZcodeChecksum(keyblock, &newnotice, buffer,
                                   cksum_start, cksum_len, cstart, cend,
                                   buffer_len, &hdr_adj, 1);
    if (retval)
	return retval;

    hdrlen += hdr_adj;

    ptr = buffer+hdrlen;

    if (newnotice.z_message_len+hdrlen > buffer_len)
	 return (ZERR_PKTLEN);

    (void) memcpy(ptr, newnotice.z_message, newnotice.z_message_len);

    *len = hdrlen+newnotice.z_message_len;

    if (*len > Z_MAXPKTLEN)
	return (ZERR_PKTLEN);

    return (ZERR_NONE);
}
#endif
