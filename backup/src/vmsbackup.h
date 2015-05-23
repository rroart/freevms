/* Variables and functions exported from vmsbackup.c.  See vmsbackup.c
 for comments on each variable or function.  */

#ifndef VMSBACKUP_H
#define VMSBACKUP_H

extern int cflag, dflag, eflag, sflag, tflag, vflag, wflag, xflag;
extern int flag_binary;
extern int flag_full;
extern char *tapefile;
extern int selset;
extern int blocksize;

extern void vmsbackup(void);

extern char **gargv;
extern int goptind, gargc;

#endif /* VMSBACKUP_H */
