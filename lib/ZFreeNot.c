/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZFreeNotice function.
 *
 *	Created by:	Robert French
 *
 *	$Id: 663dbf8fbc5b3a1ced1232af935fd95e82c92c6d $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */

#ifndef lint
static const char rcsid_ZFreeNotice_c[] = "$Id: 663dbf8fbc5b3a1ced1232af935fd95e82c92c6d $";
#endif

#include <internal.h>

Code_t
ZFreeNotice(ZNotice_t *notice)
{
    free(notice->z_packet);
    return 0;
}
