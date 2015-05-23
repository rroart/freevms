#ifndef SYSDEP_H
#define SYSDEP_H

/* Variables and functions exported from dclmain.c and getoptmain.c.  */

void usage(char *progname);

int time_vms_to_asc(short *asclength, char *ascbuffer, void *srctime);

#endif /* SYSDEP_H */
