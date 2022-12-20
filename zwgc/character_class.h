/* This file is part of the Project Athena Zephyr Notification System.
 * It is one of the source files comprising zwgc, the Zephyr WindowGram
 * client.
 *
 *      Created by:     Marc Horowitz <marc@athena.mit.edu>
 *
 *      $Id: 050b282e0ffb8873419ba5a87a4e6a4798cb6e69 $
 *
 *      Copyright (c) 1989 by the Massachusetts Institute of Technology.
 *      For copying and distribution information, see the file
 *      "mit-copyright.h".
 */


#include <zephyr/mit-copyright.h>

#ifndef character_class_TYPE
#define character_class_TYPE

#include "new_string.h"

#define  NUMBER_OF_CHARACTERS   256

typedef char character_class[NUMBER_OF_CHARACTERS];

extern /* character_class */ char * string_to_character_class(string);

#endif
