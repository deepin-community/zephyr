/* This file is part of the Project Athena Zephyr Notification System.
 * It contains source for the ZGetWGPort function.
 *
 *	Created by:	Robert French
 *
 *	$Id: b3ef17ceb40c40e5ea563e46861894fc150407bf $
 *
 *	Copyright (c) 1987 by the Massachusetts Institute of Technology.
 *	For copying and distribution information, see the file
 *	"mit-copyright.h". 
 */

#ifndef lint
static const char rcsid_ZGetWGPort_c[] = "$Id: b3ef17ceb40c40e5ea563e46861894fc150407bf $";
#endif

#include <internal.h>

int
ZGetWGPort(void)
{
    char *envptr, name[128];
    FILE *fp;
    int wgport;
	
    envptr = getenv("WGFILE");
    if (!envptr) {
	(void) sprintf(name, "/tmp/wg.%d", getuid());
	envptr = name;
    } 
    if (!(fp = fopen(envptr, "r")))
	return (-1);

    /* if fscanf fails, return -1 via wgport */
    if (fscanf(fp, "%d", &wgport) != 1)
	    wgport = -1;

    (void) fclose(fp);

    return (wgport);
}
