#ifndef x_driver_MODULE
#define x_driver_MODULE

/* This file is part of the Project Athena Zephyr Notification System.
 * It is one of the source files comprising zwgc, the Zephyr WindowGram
 * client.
 *
 *      Created by:     Marc Horowitz <marc@athena.mit.edu>
 *
 *      $Id: 7a77b91406a8cfe86e0aa0c0738c099fa154079b $
 *
 *      Copyright (c) 1989 by the Massachusetts Institute of Technology.
 *      For copying and distribution information, see the file
 *      "mit-copyright.h".
 */


#include <zephyr/mit-copyright.h>

#include <X11/Xlib.h>

#include "new_string.h"

extern Display *x_dpy;

extern char *X_driver(string);
extern int X_driver_init(char *, char, int *, char **);
extern char *get_string_resource(string, string);
extern int get_bool_resource(string, string, int);
extern unsigned long x_string_to_color(char *, unsigned long);

#endif
