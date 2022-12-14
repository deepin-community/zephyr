/*
 * Copyright (C) 1991 by the Massachusetts Institute of Technology.
 * For copying and distribution information, see the file "mit-copyright.h".
 *
 *	$Id: 127c95bf84d25c11af0f93d084d2952ee7306ab7 $
 */

#include <zephyr/mit-copyright.h>

#ifndef __zstring_h
#define __zstring_h __FILE__

#define STRING_HASH_TABLE_SIZE	1024

#include <stdio.h>

typedef struct _String
{
  char *string;				/* the string itself */
  int ref_count;			/* for gc */
  unsigned long hash_val;		/* hash value for this string */
  struct _String *next, *prev;		/* for linking in hash table */
} String;

String *make_string(char *s, int downcase);
void free_string(String *z);
String *find_string(char *s, int downcase);
String *dup_string(String *z);
int comp_string(String *a, String *b);
void print_string_table(FILE *f);

#endif /* __zstring_h */

