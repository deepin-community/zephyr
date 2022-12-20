/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZCompareUIDPred function.
 *
 *	Created by:	Robert French
 *
 *	$Id: abb05a40b5c1f680da2b3d0a9ff5a7d9fe8053f2 $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */

#ifndef lint
static const char rcsid_ZCompareUIDPred_c[] = "$Id: abb05a40b5c1f680da2b3d0a9ff5a7d9fe8053f2 $";
#endif

#include <internal.h>

int
ZCompareUIDPred(ZNotice_t *notice,
		void *uid)
{
    return (ZCompareUID(&notice->z_uid, (ZUnique_Id_t *) uid));
}

int
ZCompareMultiUIDPred(ZNotice_t *notice,
		     void *uid)
{
    return (ZCompareUID(&notice->z_multiuid, (ZUnique_Id_t *) uid));
}
