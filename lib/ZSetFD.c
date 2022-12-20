/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZSetFD function.
 *
 *	Created by:	Robert French
 *
 *	$Id: b0f3df71ad5b8ac3db224ebc66a7a8dde0c01a8e $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */

#ifndef lint
static const char rcsid_ZSetFD_c[] = "$Id: b0f3df71ad5b8ac3db224ebc66a7a8dde0c01a8e $";
#endif

#include <internal.h>

Code_t
ZSetFD(int fd)
{
	(void) ZClosePort();

	__Zephyr_fd = fd;
	__Zephyr_open = 0;
	
	return (ZERR_NONE);
}
