/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZGetSubscriptions function.
 *
 *	Created by:	Robert French
 *
 *	$Id: 7f8947ff4246ccbf217be749e7ec73861a60ca90 $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */

#ifndef lint
static const char rcsid_ZGetSubscriptions_c[] = "$Id: 7f8947ff4246ccbf217be749e7ec73861a60ca90 $";
#endif

#include <internal.h>

#ifndef MIN
#define MIN(x, y)               ((x) < (y) ? (x) : (y))
#endif

Code_t
ZGetSubscriptions(ZSubscription_t *subscription,
		  int *numsubs)
{
    int i;
	
    if (!__subscriptions_list)
	return (ZERR_NOSUBSCRIPTIONS);

    if (__subscriptions_next == __subscriptions_num)
	return (ZERR_NOMORESUBSCRIPTIONS);
	
    for (i = 0; i < MIN(*numsubs, __subscriptions_num-__subscriptions_next); i++)
	subscription[i] = __subscriptions_list[i+__subscriptions_next];

    if (__subscriptions_num-__subscriptions_next < *numsubs)
	*numsubs = __subscriptions_num-__subscriptions_next;

    __subscriptions_next += *numsubs;
	
    return (ZERR_NONE);
}
