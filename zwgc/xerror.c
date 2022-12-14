/* This file is part of the Project Athena Zephyr Notification System.
 * It is one of the source files comprising zwgc, the Zephyr WindowGram
 * client.
 *
 *      Created by:     Marc Horowitz <marc@athena.mit.edu>
 *
 *      $Id: cb934b521aad73826ca417779e418d8f86953968 $
 *
 *      Copyright (c) 1989 by the Massachusetts Institute of Technology.
 *      For copying and distribution information, see the file
 *      "mit-copyright.h".
 */

#include <sysdep.h>

#if (!defined(lint) && !defined(SABER))
static const char rcsid_xerror_c[] = "$Id: cb934b521aad73826ca417779e418d8f86953968 $";
#endif

#include <zephyr/mit-copyright.h>

#ifndef X_DISPLAY_MISSING

#include <X11/Xlib.h>
#include "mux.h"
#include "xerror.h"

int xerror_happened;

/*ARGSUSED*/
static int
xerrortrap(Display *dpy,
	   XErrorEvent *xerrev)
{
   xerror_happened = 1;
   return 0;
}

/*ARGSUSED*/
void
begin_xerror_trap(Display *dpy)
{
   xerror_happened = 0;
   XSetErrorHandler(xerrortrap);
}

void
end_xerror_trap(Display *dpy)
{
   XSync(dpy,False);
   XSetErrorHandler(NULL);
}

#endif

