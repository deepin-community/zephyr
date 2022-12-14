/* This file is part of the Project Athena Zephyr Notification System.
 * It is one of the source files comprising zwgc, the Zephyr WindowGram
 * client.
 *
 *      Created by:     Marc Horowitz <marc@athena.mit.edu>
 *
 *      $Id: 16d9047a7a02a046e0399260d38d1fc94cf1a03b $
 *
 *      Copyright (c) 1989 by the Massachusetts Institute of Technology.
 *      For copying and distribution information, see the file
 *      "mit-copyright.h".
 */

#include <sysdep.h>

#if (!defined(lint) && !defined(SABER))
static const char rcsid_text_operations_c[] = "$Id: 16d9047a7a02a046e0399260d38d1fc94cf1a03b $";
#endif

#include <zephyr/mit-copyright.h>

#include "new_memory.h"
#include "text_operations.h"
#include "char_stack.h"

string
lany(string *text_ptr,
     string str)
{
    string result, whats_left;
    char *p = *text_ptr;

    while (*p && *str) p++, str++;

    result = string_CreateFromData(*text_ptr, p - *text_ptr);
    whats_left = string_Copy(p);
    free(*text_ptr);
    *text_ptr = whats_left;

    return(result);
}

string
lbreak(string *text_ptr,
       const character_class set)
{
    string result, whats_left;
    char *p = *text_ptr;

    while (*p && !set[(int)*p]) p++;

    result = string_CreateFromData(*text_ptr, p - *text_ptr);
    whats_left = string_Copy(p);
    free(*text_ptr);
    *text_ptr = whats_left;

    return(result);
}

string
lspan(string *text_ptr,
      character_class set)
{
    string result, whats_left;
    char *p = *text_ptr;

    while (*p && set[(int)*p]) p++;

    result = string_CreateFromData(*text_ptr, p - *text_ptr);
    whats_left = string_Copy(p);
    free(*text_ptr);
    *text_ptr = whats_left;

    return(result);
}

string
rany(string *text_ptr,
     string str)
{
    string result, whats_left;
    string text = *text_ptr;
    char *p = text + strlen(text);

    while (text<p && *str) p--, str++;

    result = string_Copy(p);
    whats_left = string_CreateFromData(text, p - text);
    free(text);
    *text_ptr = whats_left;

    return(result);
}

string
rbreak(string *text_ptr,
       character_class set)
{
    string result, whats_left;
    string text = *text_ptr;
    char *p = text + strlen(text);

    while (text<p && !set[(int)p[-1]]) p--;

    result = string_Copy(p);
    whats_left = string_CreateFromData(text, p - text);
    free(text);
    *text_ptr = whats_left;

    return(result);
}

string
rspan(string *text_ptr,
      character_class set)
{
    string result, whats_left;
    string text = *text_ptr;
    char *p = text + strlen(text);

    while (text<p && set[(int)p[-1]]) p--;

    result = string_Copy(p);
    whats_left = string_CreateFromData(text, p - text);
    free(text);
    *text_ptr = whats_left;

    return(result);
}
