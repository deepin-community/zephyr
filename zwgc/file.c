/* This file is part of the Project Athena Zephyr Notification System.
 * It is one of the source files comprising zwgc, the Zephyr WindowGram
 * client.
 *
 *      Created by:     Marc Horowitz <marc@athena.mit.edu>
 *
 *      $Id: 8d5bdd0fe84d687e6966f3b609e6eae8a1114ad4 $
 *
 *      Copyright (c) 1989 by the Massachusetts Institute of Technology.
 *      For copying and distribution information, see the file
 *      "mit-copyright.h".
 */

#include <sysdep.h>

#if (!defined(lint) && !defined(SABER))
static const char rcsid_file_c[] = "$Id: 8d5bdd0fe84d687e6966f3b609e6eae8a1114ad4 $";
#endif

#include <zephyr/mit-copyright.h>

#include <pwd.h>
#include "new_memory.h"
#include "new_string.h"
#include "error.h"
#include "file.h"

/*
 *    char *get_home_directory()
 *
 *        Effects: Attempts to locate the user's (by user, the owner of this
 *                 process is meant) home directory & return its pathname.
 *                 Returns NULL if unable to do so.  Does so by first checking
 *                 the environment variable HOME.  If it is unset, falls back
 *                 on the user's password entry.
 *         Note: The returned pointer may point to a static buffer & hence
 *               go away on further calls to getenv, get_home_directory,
 *               getpwuid, or related calls.  The caller should copy it
 *               if necessary.
 */

static char
*get_home_directory(void)
{
    char *result;
    struct passwd *passwd_entry;

    result = getenv("HOME");
    if (result)
      return(result);

    if (!(passwd_entry = getpwuid(getuid())))
      return(NULL);

    return(passwd_entry->pw_dir);
}

/*
 *
 */

FILE *locate_file(char *override_filename,
		  char *home_dir_filename,
		  char *fallback_filename)
{
    char *filename;
    FILE *result;

    errno = 0;

    if (override_filename) {
	if (string_Eq(override_filename, "-"))
	  return(stdin);
	
	result = fopen(override_filename, "r");
	if (!(result = fopen(override_filename, "r"))) {
	    /* <<<>>> */
	    fprintf(stderr, "zwgc: error while opening %s for reading: ",
		   override_filename);
	    perror("");
	}
	return(result);
    }

    if (home_dir_filename) {
	filename = get_home_directory();
	if (filename) {
	    filename = string_Concat(filename, "/");
	    filename = string_Concat2(filename, home_dir_filename);
	    result = fopen(filename, "r");
	    if (result) {
		free(filename);
		return(result);
	    }
	    if (errno != ENOENT) {
		/* <<<>>> */
		fprintf(stderr, "zwgc: error while opening %s for reading: ",
			filename);
		perror("");
		free(filename);
		return(result);
	    }
	    free(filename);
	} else
	  ERROR("unable to find your home directory.\n");
    }

    if (fallback_filename) {
	if (!(result = fopen(fallback_filename, "r"))) {
	    /* <<<>>> */
	    fprintf(stderr, "zwgc: error while opening %s for reading: ",
		   fallback_filename);
	    perror("");
	}
	return(result);
    }

    return(NULL);
}
