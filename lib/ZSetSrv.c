/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZSetServerState function.
 *
 *	Created by:	Robert French
 *
 *	$Id: a3c67c0bcae0667a3e11c010d5f69e52490a5bd9 $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */

#ifndef lint
static const char rcsid_ZSetServerState_c[] = "$Id: a3c67c0bcae0667a3e11c010d5f69e52490a5bd9 $";
#endif

#include <internal.h>

Code_t
ZSetServerState(int state)
{
	__Zephyr_server = state;
	
	return (ZERR_NONE);
}
