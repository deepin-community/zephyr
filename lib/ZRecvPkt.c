/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for ZReceivePacket function.
 *
 *	Created by:	Robert French
 *
 *	$Id: 3f36ddcb8373ec1e15a8e3fde0f4cb8a8e74c4aa $
 *
 *	Copyright (c) 1987,1988 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */

#ifndef lint
static const char rcsid_ZReceivePacket_c[] = "$Id: 3f36ddcb8373ec1e15a8e3fde0f4cb8a8e74c4aa $";
#endif

#include <internal.h>

Code_t
ZReceivePacket(ZPacket_t buffer,
	       int *ret_len,
	       struct sockaddr_in *from)
{
    Code_t retval;
    struct _Z_InputQ *nextq;
    
    if ((retval = Z_WaitForComplete()) != ZERR_NONE)
	return (retval);

    nextq = Z_GetFirstComplete();

    *ret_len = nextq->packet_len;
    if (*ret_len > Z_MAXPKTLEN)
	return (ZERR_PKTLEN);
    
    (void) memcpy(buffer, nextq->packet, *ret_len);

    if (from)
	*from = nextq->from;
	
    Z_RemQueue(nextq);

    return (ZERR_NONE);
}
