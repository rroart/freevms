/* Variables and functions exported from vmsbackup.c.  See vmsbackup.c
   for comments on each variable or function.  */

extern int cflag, dflag, eflag, sflag, tflag, vflag, wflag, xflag;
extern int flag_binary;
extern int flag_full;
extern char *tapefile;
extern int selset;
extern int blocksize;

extern void vmsbackup ();

extern char **gargv;
extern int goptind, gargc;
