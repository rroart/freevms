#ifndef __FILE_LOADED
#define __FILE_LOADED   1

/*  FILE - V3.0 */

/*
    Manifest constants used by BSD 4.2 OPEN function
 */

# define O_RDONLY   000
# define O_WRONLY   001
# define O_RDWR     002
# define O_NDELAY   004
# define O_NOWAIT   004
# define O_APPEND   010
# define O_CREAT    01000
# define O_TRUNC    02000
# define O_EXCL     04000

/*
 * Flock call.
 */
#define LOCK_SH     1   /* shared lock */
#define LOCK_EX     2   /* exclusive lock */
#define LOCK_NB     4   /* don't block when locking */
#define LOCK_UN     8   /* unlock */

/*
 * flags- also for fcntl call.
 */
#define FOPEN       (-1)
#define FREAD       00001       /* descriptor read/receive'able */
#define FWRITE      00002       /* descriptor write/send'able */
#ifndef F_DUPFD
#define FNDELAY     00004       /* no delay */
#define FAPPEND     00010       /* append on each write */
#endif
#define FMARK       00020       /* mark during gc() */
#define FDEFER      00040       /* defer for next gc pass */
#ifndef F_DUPFD
#define FASYNC      00100       /* signal pgrp when data ready */
#endif
#define FSHLOCK     00200       /* shared lock present */
#define FEXLOCK     00400       /* exclusive lock present */

/* bits to save after open */
#define FMASK       00113
#define FCNTLCANT   (FREAD|FWRITE|FMARK|FDEFER|FSHLOCK|FEXLOCK)

/* open only modes */
#define FCREAT      01000       /* create if nonexistant */
#define FTRUNC      02000       /* truncate to zero length */
#define FEXCL       04000       /* error if already created */

#ifndef F_DUPFD
/* fcntl(2) requests--from <fcntl.h> */
#define F_DUPFD 0   /* Duplicate fildes */
#define F_GETFD 1   /* Get fildes flags */
#define F_SETFD 2   /* Set fildes flags */
#define F_GETFL 3   /* Get file flags */
#define F_SETFL 4   /* Set file flags */
#define F_GETOWN 5  /* Get owner */
#define F_SETOWN 6  /* Set owner */
#endif

#endif                  /* __FILE_LOADED */
