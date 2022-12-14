/* This file is part of the Project Athena Zephyr Notification System.
 * It is one of the source files comprising zwgc, the Zephyr WindowGram
 * client.
 *
 *      Created by:     Marc Horowitz <marc@athena.mit.edu>
 *
 *      $Id: db3f2ea28cdd4dff1c0d13f8c716aeac5ed85aa7 $
 *
 *      Copyright (c) 1989 by the Massachusetts Institute of Technology.
 *      For copying and distribution information, see the file
 *      "mit-copyright.h".
 */


#include <zephyr/mit-copyright.h>

#ifndef buffer_MODULE
#define buffer_MODULE

#include "new_string.h"

extern string buffer_to_string(void);
extern void clear_buffer(void);
extern void append_buffer(char *);

#endif
