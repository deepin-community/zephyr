/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZFormatSmallRawNotice function.
 *
 *	Created by:	Robert French
 *
 *	$Id: 3de04ad8ce47b3b2002c637ff975b2184bcecac0 $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */

#ifndef lint
static const char rcsid_ZFormatRawNotice_c[] = "$Id: 3de04ad8ce47b3b2002c637ff975b2184bcecac0 $";
#endif

#include <internal.h>

Code_t
ZFormatSmallRawNotice(ZNotice_t *notice,
		      ZPacket_t buffer,
		      int *ret_len)
{
    Code_t retval;
    int hdrlen;
    
    if ((retval = Z_FormatRawHeader(notice, buffer, Z_MAXHEADERLEN,
				    &hdrlen, NULL, NULL)) != ZERR_NONE)
	return (retval);

    *ret_len = hdrlen+notice->z_message_len;

    if (*ret_len > Z_MAXPKTLEN)
	return (ZERR_PKTLEN);

    (void) memcpy(buffer+hdrlen, notice->z_message, notice->z_message_len);

    return (ZERR_NONE);
}

Code_t
ZNewFormatSmallRawNotice(ZNotice_t *notice,
			 ZPacket_t buffer,
			 int *ret_len)
{
  Code_t retval;
  int hdrlen;
  
  if ((retval = Z_AsciiFormatRawHeader(notice, buffer, Z_MAXHEADERLEN,
                                       &hdrlen, NULL, NULL, NULL, NULL)) 
      != ZERR_NONE)
    return (retval);
  
  *ret_len = hdrlen+notice->z_message_len;
  
  if (*ret_len > Z_MAXPKTLEN)
    return (ZERR_PKTLEN);
  
  (void) memcpy(buffer+hdrlen, notice->z_message, notice->z_message_len);
  
  return (ZERR_NONE);
}
