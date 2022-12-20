/* This file is part of the Project Athena Zephyr Notification System.
 * It contains definitions for the ACL library
 *
 *	Created by:	John T. Kohl
 *
 *	$Id: 84c5a70628e77da5d5091ab87b17d5c4c582d765 $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */

#include <zephyr/mit-copyright.h>

#ifndef	__ACL__
#define	__ACL__

int acl_add(char *, char *);
int acl_check(char *, char *, struct sockaddr_in *);
int acl_delete(char *, char *);
int acl_initialize(char *, int);
void acl_cache_reset(void);

#endif /* __ACL__ */

