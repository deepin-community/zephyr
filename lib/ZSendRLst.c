/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZSendRawNotice function.
 *
 *	Created by:	John T. Kohl
 *
 *	$Id: 3cb4c636f03d98622abc436265ba1a8a7742b24f $
 *
 *	Copyright (c) 1988 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */

#ifndef lint
static const char rcsid_ZSendRawList_c[] = "$Id: 3cb4c636f03d98622abc436265ba1a8a7742b24f $";
#endif

#include <internal.h>

Code_t ZSendRawList(ZNotice_t *notice,
		    char *list[],
		    int nitems)
{
    return(ZSrvSendRawList(notice, list, nitems, Z_XmitFragment));
}

Code_t ZSrvSendRawList(ZNotice_t *notice,
		       char *list[],
		       int nitems,
		       Code_t (*send_routine)(ZNotice_t *, char *, int, int))
{
    Code_t retval;
    ZNotice_t newnotice;
    char *buffer;
    int len;

    if ((retval = ZFormatRawNoticeList(notice, list, nitems, &buffer, 
				       &len)) != ZERR_NONE)
	return (retval);

    if ((retval = ZParseNotice(buffer, len, &newnotice)) != ZERR_NONE)
	return (retval);
    
    retval = Z_SendFragmentedNotice(&newnotice, len, NULL, send_routine);

    free(buffer);

    return (retval);
}
