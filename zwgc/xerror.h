/* This file is part of the Project Athena Zephyr Notification System.
 * It is one of the source files comprising zwgc, the Zephyr WindowGram
 * client.
 *
 *      Created by:     Marc Horowitz <marc@athena.mit.edu>
 *
 *      $Id: ea6702ede70f492d9945e345a8e6ea829db82e9e $
 *
 *      Copyright (c) 1989 by the Massachusetts Institute of Technology.
 *      For copying and distribution information, see the file
 *      "mit-copyright.h".
 */

#ifndef _XERROR_H_
#define _XERROR_H_

#if (!defined(lint) && !defined(SABER))
static const char rcsid_xerror_h[] = "$Id: ea6702ede70f492d9945e345a8e6ea829db82e9e $";
#endif

#include <zephyr/mit-copyright.h>

extern int xerror_happened;

void begin_xerror_trap(Display *);
void end_xerror_trap(Display *);

#endif
