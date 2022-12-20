/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for ZPeekNotice function.
 *
 *	Created by:	Robert French
 *
 *	$Id: 18c2026e2f62452e1170a232d15f16b3e1630620 $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */

#ifndef lint
static const char rcsid_ZPeekNotice_c[] = "$Id: 18c2026e2f62452e1170a232d15f16b3e1630620 $";
#endif

#include <internal.h>

Code_t
ZPeekNotice(ZNotice_t *notice,
	    struct sockaddr_in *from)
{
    char *buffer;
    int len;
    Code_t retval;
	
    if ((retval = ZPeekPacket(&buffer, &len, from)) != ZERR_NONE)
	return (retval);

    return (ZParseNotice(buffer, len, notice));
}
