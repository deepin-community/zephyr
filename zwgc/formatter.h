/* This file is part of the Project Athena Zephyr Notification System.
 * It is one of the source files comprising zwgc, the Zephyr WindowGram
 * client.
 *
 *      Created by:     Marc Horowitz <marc@athena.mit.edu>
 *
 *      $Id: d2cf0f061eb6c4b0e52aa968e59b5fafd9c439e3 $
 *
 *      Copyright (c) 1989 by the Massachusetts Institute of Technology.
 *      For copying and distribution information, see the file
 *      "mit-copyright.h".
 */


#include <zephyr/mit-copyright.h>

#include "new_string.h"

#ifndef formatter_MODULE
#define formatter_MODULE

typedef struct _desctype {
    struct _desctype *next;

    short int code;
#define DT_EOF	0	/* End of message.	*/
#define DT_ENV	1	/* Open environment.	*/
#define DT_STR	2	/* Display string.	*/
#define DT_END	3	/* Close environment.	*/
#define DT_NL	4	/* Newline.		*/
    
    char *str;		/* Name of environment, string to be displayed.	*/
    int len;		/* Length of string/environment name for
			   ENV, STR, END.  Undefined for EOF */
} desctype;

extern desctype *disp_get_cmds(char *, int *, int *);
extern void free_desc(desctype *);

extern string protect(string);
extern string verbatim(string, int);
extern string stylestrip(string);
#endif
