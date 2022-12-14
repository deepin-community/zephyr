/* This file is part of the Project Athena Zephyr Notification System.
 * It is one of the source files comprising zwgc, the Zephyr WindowGram
 * client.
 *
 *      Created by:     Marc Horowitz <marc@athena.mit.edu>
 *
 *      $Id: 41716cb42bccaaed8142decbcd3c4a64cd2cb169 $
 *
 *      Copyright (c) 1989 by the Massachusetts Institute of Technology.
 *      For copying and distribution information, see the file
 *      "mit-copyright.h".
 */

#include <sysdep.h>

#if (!defined(lint) && !defined(SABER))
static const char rcsid_character_class_c[] = "$Id: 41716cb42bccaaed8142decbcd3c4a64cd2cb169 $";
#endif

#include <zephyr/mit-copyright.h>
#include <zephyr/zephyr.h>
#include "character_class.h"

/* 
 * It may look like we are passing the cache by value, but since it's
 * really an array we are passing by reference.  C strikes again....
 */

static character_class cache;

/* character_class */
char *
string_to_character_class(string str)
{
    int i, l;

    (void) memset(cache, 0, sizeof(cache));

    l = strlen(str);

    for (i = 0; i < l; i++)
	cache[(unsigned char)str[i]] = 1;

    return(cache);
}
