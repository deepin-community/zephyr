/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZCompareUID function.
 *
 *	Created by:	Robert French
 *
 *	$Id: a448deb11f2b554a98bf53242f578626e0eb3da5 $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */

#ifndef lint
static const char rcsid_ZCompareUID_c[] = "$Id: a448deb11f2b554a98bf53242f578626e0eb3da5 $";
#endif

#include <internal.h>

int
ZCompareUID(ZUnique_Id_t *uid1,
	    ZUnique_Id_t *uid2)
{
    return (!memcmp((char *)uid1, (char *)uid2, sizeof (*uid1)));
}
