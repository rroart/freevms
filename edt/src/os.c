//+++2004-01-03
//    Copyright (C) 2001,2002,2003  Mike Rieker, Beverly, MA USA
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; version 2 of the License.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---2004-01-03

/************************************************************************/
/*									*/
/*  OS dependent routines						*/
/*									*/
/*  Logical name EDT_TTYNAME can be used to point to alternate terminal	*/
/*									*/
/************************************************************************/

#include "edt.h"

#define JOURNAL_FLUSH_INTERVAL (15)

static int screenmode = 0;
char *help_name = NULL;

#if defined (VMS)

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include dcdef
#include iodef
#include ssdef
#include tt2def

static int jnlflushenable = 0;		/* 0: not in read routine, ast should not do anything */
					/* 1: in read routine, it's ok for ast routine to flush */
static int jnlflushtimerip = 0;		/* 0: no timer is queued */
					/* 1: timer request queued */
static int ttsaveof = 0;		/* 0: last read ended normally */
					/* 1: last read ended in eof */
static uByte originalmodes[12];		/* original terminal modes */
static uQuad nextjournalflush = 0;	/* 0: the journal was freshly flushed and a read is */
					/*    in progress, so no journal flushing need be done */
					/* else: datetime of next journal flush */
static uWord ttchan = 0;		/* 0: channel has not been assigned yet */
					/* else: i/o channel to the terminal */

void lib$stop ();
uLong sys$assign ();
uLong sys$exit ();
uLong sys$gettim ();
uLong sys$qio ();
uLong sys$qiow ();
uLong sys$setast ();
uLong sys$setimr ();
uLong sys$synch ();

static void setjnlflush (int on);
static void jnlflushast (void);

#elif defined (_OZONE)

#include "ozone.h"
#include "oz_knl_status.h"
#include "oz_io_console.h"
#include "oz_io_fs.h"
#include "oz_io_timer.h"
#include "oz_sys_dateconv.h"
#include "oz_sys_io.h"
#include "oz_sys_io_fs.h"
#include "oz_sys_logname.h"
#include "oz_sys_thread.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static OZ_Console_modebuff modebuff;
static OZ_Handle h_timer;
static OZ_Handle h_tty;
static int jnlflushenable = 0;		/* 0: not in read routine, ast should not do anything */
					/* 1: in read routine, it's ok for ast routine to flush */
static int jnlflushtimerip = 0;		/* 0: no timer is queued */
					/* 1: timer request queued */
static int ttsaveof = 0;		/* 0: last read ended normally */
					/* 1: last read ended in eof */
static OZ_Datebin nextjournalflush = 0;	/* 0: the journal was freshly flushed and a read is */
					/*    in progress, so no journal flushing need be done */
					/* else: datetime of next journal flush */

static void setjnlflush (int on);
static void jnlflushast (void *dummy, uLong status, OZ_Mchargs *mchargs);

#else

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fcntl.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef IUCLC
#define IUCLC 0
#endif
#ifndef OLCUC
#define OLCUC 0
#endif
#ifndef OFILL
#define OFILL 0
#endif

static int ttsaveof = 0;
static int ttyfd = -1;
static int tty_length = 0;
static int tty_width = 0;
static struct termios initial_settings;
static time_t nextjournalflush = 0;

static int timedread (char *buf, int siz);

#endif

/************************************************************************/
/*									*/
/*  Initialization routine						*/
/*									*/
/************************************************************************/

void os_initialization (void)

{
#if defined (VMS)

  char *p, *ttname;
  struct { uLong size;
           char *buff;
         } ttdesc;
  uLong sts;

  struct { uWord sts, len;
           char trm[4];
         } iosb;

  /* Assign I/O channel to terminal */

  ttname = getenv ("EDT_TTYNAME");
  if (ttname == NULL) ttname = "TT";
  ttdesc.size = strlen (ttname);
  ttdesc.buff = ttname;
  sts = sys$assign (&ttdesc, &ttchan, 0, 0);
  if (!(sts & 1)) {
    fprintf (stderr, "error 0x%x assigning channel to terminal %s\n", sts, ttname);
    sys$exit (sts);
  }

  /* Sense mode - get original modes and make sure it is a terminal */

  sts = sys$qiow (1, ttchan, IO$_SENSEMODE, &iosb, 0, 0, originalmodes, sizeof originalmodes, 0, 0, 0, 0);
  if (sts & 1) sts = iosb.sts;
  if (!(sts & 1)) {
    fprintf (stderr, "error 0x%x sensing terminal %s modes\n", sts, ttname);
    sys$exit (sts);
  }
  if (originalmodes[0] != DC$_TERM) {
    fprintf (stderr, "device %s is not a terminal\n", ttname);
    sys$exit (SS$_IVDEVNAM);
  }

  /* Help file is same name as executable but with .HLP instead of .EXE */

  help_name = strdup (pn);
  p = strchr (help_name, ';');
  if (p == NULL) p = help_name + strlen (help_name);
  strcpy (p - 3, "hlp");

#elif defined (_OZONE)

  char *ttname;
  OZ_IO_console_getmode console_getmode;
  OZ_IO_fs_open fs_open;
  uLong sts;

  /* Assign I/O channel to terminal */

  ttname = "EDT_TTYNAME";
  if (getenv (ttname) == NULL) ttname = "OZ_CONSOLE";
  memset (&fs_open, 0, sizeof fs_open);
  fs_open.name = ttname;
  fs_open.lockmode = OZ_LOCKMODE_EX;
  sts = oz_sys_io_fs_open (sizeof fs_open, &fs_open, 0, &h_tty);
  if (sts != OZ_SUCCESS) {
    fprintf (stderr, "error %u opening %s\n", sts, ttname);
    oz_sys_thread_exit (sts);
  }

  /* Get width and columns */

  memset (&modebuff, 0, sizeof modebuff);
  memset (&console_getmode, 0, sizeof console_getmode);
  console_getmode.size = sizeof modebuff;
  console_getmode.buff = &modebuff;

  sts = oz_sys_io (OZ_PROCMODE_KNL, h_tty, 0, OZ_IO_CONSOLE_GETMODE, sizeof console_getmode, &console_getmode);
  if (sts != OZ_SUCCESS) {
    fprintf (stderr, "error %u getting console mode\n", sts);
    oz_sys_thread_exit (sts);
  }

  /* Assign I/O channel to timer */

  sts = oz_sys_io_assign (OZ_PROCMODE_KNL, &h_timer, "timer", OZ_LOCKMODE_CW);
  if (sts != OZ_SUCCESS) {
    fprintf (stderr, "error %u assigning channel to timer\n", sts);
    oz_sys_thread_exit (sts);
  }

  /* Set up help file name */

  help_name = "edt.hlp";

#else

  char linkbuf[256], *p, *q, *ttname;
  const char *pp, *qq;
  FILE *sizepipe;
  int rc;
  String *pathstring;
  struct stat statbuf;

  ttname = getenv ("EDT_TTYNAME");
  if ((ttname == NULL) || (ttname[0] == 0)) ttname = "/dev/tty";

  ttyfd = open (ttname, O_RDWR);
  if (ttyfd < 0) {
    fprintf (stderr, "error opening terminal %s: %s\n", ttname, strerror (errno));
    exit (-1);
  }

  if (tcgetattr (ttyfd, &initial_settings) < 0) {
    fprintf (stderr, "error getting terminal %s settings: %s\n", ttname, strerror (errno));
    abort ();
  }

  sprintf (linkbuf, "stty size < %s", ttname);
  sizepipe = popen (linkbuf, "r");
  if (sizepipe == NULL) {
    fprintf (stderr, "error getting terminal %s sizes: %s\n", ttname, strerror (errno));
    tty_width  = 80;
    tty_length = 24;
  } else {
    if (fscanf (sizepipe, "%d %d", &tty_length, &tty_width) != 2) {
      fprintf (stderr, "unable to decode output from 'stty size < %s' command\n", ttname);
      tty_width  = 80;
      tty_length = 24;
    }
    pclose (sizepipe);
  }

  /* Try to find help file - same directory as executable */

  pathstring = NULL;						/* haven't allocated a string yet */
  pp = pn;							/* assume we just use 'program name' from argv[0] */
  if (strchr (pn, '/') == NULL) {				/* use it if it contains a slash */
    pathstring = string_create (0, NULL);			/* no slash, create an empty string for the path */
    for (p = getenv ("PATH"); p != NULL; p = q) {		/* look through PATH directories for executable */
      q = strchr (p, ':');					/* get next directory name */
      if (q == NULL) q = p + strlen (p);			/* point to the end of it */
      string_setval (pathstring, q - p, p);			/* copy directory name to pathstring */
      if (q[-1] != '/') string_concat (pathstring, 1, "/");	/* make sure it has a slash on the end */
      string_concat (pathstring, strlen (pn), pn);		/* then put program name after that */
      pp = string_getval (pathstring);				/* point to composite pathname string */
      while ((rc = lstat (pp, &statbuf)) >= 0) {		/* try to stat the executable or the softlink */
        if (!S_ISLNK (statbuf.st_mode)) break;			/* if it's an actual file, we found it */
        rc = readlink (pp, linkbuf, sizeof linkbuf - 1);	/* it's a softlink, read the softlink value */
        if (rc <= 0) break;					/* stop trying if softlink broken */
        if (linkbuf[0] == '/') string_setval (pathstring, rc, linkbuf); /* if absolute softlink, replace the old pathstring */
        else {
          qq = strrchr (pp, '/') + 1;				/* relative, add to end of old pathstring after directory */
          string_remove (pathstring, string_getlen (pathstring) - (qq - pp), qq - pp);
          string_concat (pathstring, rc, linkbuf);
        }
        pp = string_getval (pathstring);
      }
      if (rc >= 0) break;					/* if executable found, we're done looking thru PATH directories */
      if (*q != 0) q ++;
      else q = NULL;
    }
    if (p == NULL) pp = pn;					/* if it wasn't found in PATH directories, just use pn */
    else pp = string_getval (pathstring);			/* otherwise, use executable's path */
  }

  help_name = malloc (strlen (pp) + 5);				/* allocate a buffer for help filename */
  strcpy (help_name, pp);					/* copy in the executable directory and name */
  strcat (help_name, ".hlp");					/* append .hlp for the help file name */

  if (pathstring != NULL) string_delete (pathstring);		/* free off string if we allocated one */

#endif

  /* Make sure C-library output is flushed */

  fflush (stderr);
  fflush (stdout);
}

/************************************************************************/
/*									*/
/*  Set screen mode							*/
/*									*/
/*    Input:								*/
/*									*/
/*	on = 0 : off (used for line mode)				*/
/*	     1 : on (used for screen (change) mode)			*/
/*									*/
/************************************************************************/

void os_screenmode (int on)

{
#if defined (VMS)

  struct { uWord sts, len;
           char trm[4];
         } iosb;
  uLong sensebuf[3], sts;

  if (screenmode != on) {
    memcpy (sensebuf, originalmodes, sizeof sensebuf);

    if (on) sensebuf[2] |= TT2$M_PASTHRU;
    else sensebuf[2] &= ~TT2$M_PASTHRU;

    sts = sys$qiow (1, ttchan, IO$_SETMODE, &iosb, 0, 0, sensebuf, sizeof sensebuf, 0, 0, 0, 0);
    if (sts & 1) sts = iosb.sts;
    if (!(sts & 1)) lib$stop (sts);

    screenmode = on;
  }

#elif defined (_OZONE)

    screenmode = on;

#else

  struct termios settings;

  if (screenmode != on) {					/* see if requested differs from current state */
    settings = initial_settings;
    if (on) {
      settings.c_iflag &= ~(INLCR | IGNCR | ICRNL | IUCLC);	/* really screw it up by turning everything off */
      settings.c_oflag &= ~(OPOST | OLCUC | ONLCR | OCRNL | ONOCR | ONLRET | OFILL);
      settings.c_lflag &= ~(ISIG | ICANON | ECHO);
    }
    if (tcsetattr (ttyfd, TCSANOW, &settings) < 0) {		/* set it the way we want it */
      perror ("error setting terminal settings");
      abort ();
    }
    screenmode = on;						/* now it is set to the new state */
  }

#endif
}

/************************************************************************/
/*									*/
/*  Read from terminal (in line mode) with prompt			*/
/*									*/
/*    Input:								*/
/*									*/
/*	prompt = prompt string pointer					*/
/*									*/
/*    Output:								*/
/*									*/
/*	os_readprompt = NULL : eof					*/
/*	                else : string pointer				*/
/*									*/
/*    Note:								*/
/*									*/
/*	This routine should only be called from jnl_readprompt.  	*/
/*	Others should call jnl_readprompt instead.			*/
/*									*/
/*	Screenmode should be OFF					*/
/*									*/
/************************************************************************/

String *os_readprompt (const char *prompt)

{
#if defined (VMS)

  char buf[256];
  String *string;
  struct { uWord sts, len;
           char trm[4];
         } iosb;
  uLong sts;

  /* Enable journal flushing (via ast) */

  setjnlflush (1);

  /* Stop if we got an eof last time along with partial data */

  if (ttsaveof) goto eof;

  /* Start with no string at all */

  string = NULL;

  /* Read with prompt (no prompt if continuation of previous read) */

get:
  if (string != NULL) sts = sys$qiow (1, ttchan, IO$_READVBLK, &iosb, 0, 0, buf, sizeof buf, 0, 0, 0, 0);
  else sts = sys$qiow (1, ttchan, IO$_READPROMPT, &iosb, 0, 0, buf, sizeof buf, 0, 0, prompt, strlen (prompt));

  /* Check termination status, treat any error like an eof */

  if (sts & 1) sts = iosb.sts;
  if (!(sts & 1)) ttsaveof = 1;
  if (iosb.trm[0] == 26) ttsaveof = 1;

  /* If hard eof, exit now */

  if (ttsaveof && (string == NULL) && (iosb.len == 0)) goto eof;

  /* Concat any fetched data to string */

  if (string != NULL) string_concat (string, iosb.len, buf);
  else string = string_create (iosb.len, buf);

  /* Repeat if we got a full buffer and its wasn't eof */

  if (!ttsaveof && (iosb.len == sizeof buf)) goto get;

  /* Less than a full buffer or eof, return string pointer */

  setjnlflush (0);
  return (string);

  /* Eof, return NULL pointer */

eof:
  ttsaveof = 0;
  setjnlflush (0);
  return (NULL);

#elif defined (_OZONE)

  char buf[256];
  OZ_IO_console_read console_read;
  String *string;
  uLong len, sts;

  /* Enable journal flushing (via ast) */

  setjnlflush (1);

  /* Stop if we got an eof last time along with partial data */

  if (ttsaveof) goto eof;

  /* Start with no string at all */

  string = NULL;

  /* Read with prompt (no prompt if continuation of previous read) */

get:
  memset (&console_read, 0, sizeof console_read);
  console_read.size = sizeof buf;
  console_read.buff = buf;
  console_read.rlen = &len;
  if (string == NULL) {
    console_read.pmtsize = strlen (prompt) - 2;
    console_read.pmtbuff = prompt + 2;
  }
  sts = oz_sys_io (OZ_PROCMODE_KNL, h_tty, 0, OZ_IO_CONSOLE_READ, sizeof console_read, &console_read);

  /* Check termination status, treat any error like an eof */

  if ((sts != OZ_SUCCESS) && (sts != OZ_NOTERMINATOR)) ttsaveof = 1;

  /* If hard eof, exit now */

  if (ttsaveof && (string == NULL) && (len == 0)) goto eof;

  /* Concat any fetched data to string */

  if (string != NULL) string_concat (string, len, buf);
  else string = string_create (len, buf);

  /* Repeat if we did not get a terminator */

  if (sts == OZ_NOTERMINATOR) goto get;

  /* We got a terminator, return string pointer */

  setjnlflush (0);
  return (string);

  /* Eof, return NULL pointer */

eof:
  ttsaveof = 0;
  setjnlflush (0);
  return (NULL);

#else /* Generic CRTL version */

  char buf[256];
  int eol, len;
  String *string;

  string = NULL;

  /* Check for eof on prior read */

  if (ttsaveof) goto eof;

  /* Output prompt, if any */

  os_writebuffer (strlen (prompt), prompt);

  /* Read a much as we can from terminal */

get:
  len = timedread (buf, sizeof buf);

  /* If eof and we have nothing so far, return eof pointer, else return what we have */

  if (len <= 0) {
    ttsaveof = 1;
    if (string == NULL) goto eof;
    return (string);
  }

  /* Not eof, concat any fetched data to string */

  eol  = (buf[len-1] == '\n');
  len -= eol;
  if (string != NULL) string_concat (string, len, buf);
  else string = string_create (len, buf);

  /* Repeat until we get an \n (or eof) */

  if (!eol) goto get;

  /* Got \n, so return string pointer */

  return (string);

  /* Got eof with no data, return NULL pointer */

eof:
  ttsaveof = 0;
  return (NULL);
#endif
}

/************************************************************************/
/*									*/
/*  Read keypad sequence from terminal without echoing			*/
/*									*/
/*    Output:								*/
/*									*/
/*	os_readkeyseq = NULL : eof					*/
/*	                else : string pointer				*/
/*									*/
/*    Note:								*/
/*									*/
/*	This routine should only be called from jnl_readkeyseq.  	*/
/*	Others should call jnl_readkeyseq instead.			*/
/*									*/
/*	Screenmode should be ON						*/
/*									*/
/************************************************************************/

int os_readkeyseq (String *keystring)

{
#if defined (VMS)

  char buf[256];
  struct { uWord sts, len;
           char termchr;
           char fill1;
           char termlen;
           char fill2;
         } iosb;
  uLong sts;

  /* Enable journal flushing */

  setjnlflush (1);

  /* Read without echoing, wait for one character */

  sts = sys$qiow (1, ttchan, IO$_READVBLK | IO$M_NOECHO, &iosb, 0, 0, buf, 1, 0, 0, 0, 0);

  /* Check termination status, treat any error like an eof */

  if (sts & 1) sts = iosb.sts;
  if (!(sts & 1)) return (0);

  /* Disable journal flushing */

  setjnlflush (0);

  /* Concat any fetched data to string */

  string_concat (keystring, iosb.len, buf);
  string_concat (keystring, iosb.termlen, &iosb.termchr);

  /* Read again, but just get whatever happens to be in read-ahead, don't wait */

  sts = sys$qiow (1, ttchan, IO$_READVBLK | IO$M_NOECHO | IO$M_TIMED, &iosb, 0, 0, buf, sizeof buf, 0, 0, 0, 0);

  /* Check termination status, treat any error like an eof */

  if (sts & 1) sts = iosb.sts;
  if (sts == SS$_TIMEOUT) sts |= 1;
  if (!(sts & 1)) return (0);

  /* Concat any fetched data to string */

  string_concat (keystring, iosb.len, buf);
  string_concat (keystring, iosb.termlen, &iosb.termchr);

  return (1);

#elif defined (_OZONE)

  char buf[256];
  OZ_IO_console_getdat console_getdat;
  uLong len, sts;

  /* Enable journal flushing */

  setjnlflush (1);

  /* Read whatever is waiting for us, but wait for at least one char */

  memset (&console_getdat, 0, sizeof console_getdat);
  console_getdat.size = sizeof buf;
  console_getdat.buff = buf;
  console_getdat.rlen = &len;
  sts = oz_sys_io (OZ_PROCMODE_KNL, h_tty, 0, OZ_IO_CONSOLE_GETDAT, sizeof console_getdat, &console_getdat);

  /* Check termination status, treat any error like an eof */

  if (sts != OZ_SUCCESS) return (0);

  /* Disable journal flushing */

  setjnlflush (0);

  /* Concat any fetched data to string */

  string_concat (keystring, len, buf);

  return (1);

#else /* Generic CRTL version */

  char buf[256];
  int rc;

  /* Wait for whatever is available */

  rc = timedread (buf, sizeof buf);
  if (rc <= 0) return (0);
  string_concat (keystring, rc, buf);
  return (1);

#endif
}

/************************************************************************/
/*									*/
/*  Write a buffer to terminal screen					*/
/*									*/
/*    Input:								*/
/*									*/
/*	size = size of data to be written				*/
/*	buff = address of data						*/
/*									*/
/*    Note:								*/
/*									*/
/*	Screenmode is ON : no editing or formatting occurs		*/
/*	             OFF : normal editing and formatting occurs		*/
/*									*/
/************************************************************************/

int os_writebuffer (int size, const char *buff)

{
#if defined (VMS)

  const char *p;
  struct { uWord sts, len;
           char termchr;
           char fill1;
           char termlen;
           char fill2;
         } iosb1, iosb2;
  uLong sts;

  /* Write without formatting */

  if (screenmode) {
    sts = sys$qiow (1, ttchan, IO$_WRITEVBLK | IO$M_NOFORMAT, &iosb1, 0, 0, buff, size, 0, 0, 0, 0);
    if (sts & 1) sts = iosb1.sts;
    return (sts & 1);
  }

  /* Needs to have <CR>'s inserted before the <LF>'s */

  iosb1.sts = 1;
  iosb2.sts = 1;
  while (size > 0) {
    p = memchr (buff, '\n', size);		/* find an <LF> */
    if (p == NULL) p = buff + size;		/* if none left, point to end of string */
    sts = sys$synch (1, &iosb1);		/* make sure we can use this iosb again */
    if (sts & 1) sts = iosb1.sts;
    if (!(sts & 1)) return (0);
    sts = sys$qio (1, ttchan, IO$_WRITEVBLK | IO$M_NOFORMAT, &iosb1, 0, 0, buff, p - buff, 0, 0, 0, 0); /* start all up to <LF> */
    if (!(sts & 1)) return (0);
    size -= p - buff;				/* see how much is left, including the <LF> */
    buff  = p;
    if (size == 0) break;			/* if nothing (we hit the end above), we're all done */
    sts = sys$synch (1, &iosb2);		/* make sure we can use this iosb again */
    if (sts & 1) sts = iosb2.sts;
    if (!(sts & 1)) return (0);
    sts = sys$qio (1, ttchan, IO$_WRITEVBLK | IO$M_NOFORMAT, &iosb2, 0, 0, "\r\n", 2, 0, 0, 0, 0); /* start outputting <CR><LF> */
    if (!(sts & 1)) return (0);
    size --;					/* increment over the <LF> */
    buff ++;
  }
  sts = sys$synch (1, &iosb1);			/* end of buffer, wait for writes to complete */
  if (sts & 1) sts = iosb1.sts;
  if (sts & 1) sts = sys$synch (1, &iosb2);
  if (sts & 1) sts = iosb2.sts;
  return (sts & 1);

#elif defined (_OZONE)

  OZ_IO_console_putdat console_putdat;
  OZ_IO_console_write console_write;
  uLong sts;

  /* Write without formatting */

  if (screenmode) {
    memset (&console_putdat, 0, sizeof console_putdat);
    console_putdat.size = size;
    console_putdat.buff = buff;
    sts = oz_sys_io (OZ_PROCMODE_KNL, h_tty, 0, OZ_IO_CONSOLE_PUTDAT, sizeof console_putdat, &console_putdat);
  }

  /* Write with formatting */

  else {
    memset (&console_write, 0, sizeof console_write);
    console_write.size = size;
    console_write.buff = buff;
    sts = oz_sys_io (OZ_PROCMODE_KNL, h_tty, 0, OZ_IO_CONSOLE_WRITE, sizeof console_write, &console_write);
  }

  return (sts == OZ_SUCCESS);

#else /* Generic CRTL version */

  int rc;
  uLong offs;

  for (offs = 0; offs < size; offs += rc) {
    rc = write (ttyfd, buff + offs, size - offs);
    if (rc <= 0) return (0);
  }

  return (1);

#endif
}

/************************************************************************/
/*									*/
/*  Get screen width and length						*/
/*									*/
/************************************************************************/

int os_getscreensize (int *width_r, int *length_r)

{
#if defined (VMS)

  *width_r  = originalmodes[2];
  *length_r = originalmodes[7];

  return (1);

#elif defined (_OZONE)

  *width_r  = modebuff.columns;
  *length_r = modebuff.rows;

  return (1);

#else

  char *p;

  *width_r  = tty_width;
  *length_r = tty_length;

  p = getenv ("EDT_SCREENWIDTH");
  if (p != NULL) *width_r = atoi (p);
  p = getenv ("EDT_SCREENLENGTH");
  if (p != NULL) *length_r = atoi (p);

  return (1);

#endif
}

/************************************************************************/
/*									*/
/*  Make a journal filename from a given filename			*/
/*									*/
/************************************************************************/

char *os_makejnlname (const char *filename)

{
  char *jnlfn;

#if defined (VMS)

  char *p;
  const char *q;

  q = strrchr (filename, ']');
  if (q == NULL) q = strrchr (filename, ':');
  if (q != NULL) q ++;
  else q = filename;
  jnlfn = malloc (strlen (q) + 6);
  strcpy (jnlfn, q);
  p = strchr (jnlfn, ';');
  if (p != NULL) *p = 0;
  strcat (jnlfn, "_edtj");

#elif defined (_OZONE)

  char *p;
  const char *q;

  q = strrchr (filename, '/');
  if (q == NULL) q = strrchr (filename, ':');
  if (q != NULL) q ++;
  else q = filename;
  jnlfn = malloc (strlen (q) + 6);
  strcpy (jnlfn, q);
  p = strchr (jnlfn, ';');
  if (p != NULL) *p = 0;
  strcat (jnlfn, "_edtj");

#else

  const char *q;

  q = strrchr (filename, '/');
  if (q != NULL) q ++;
  else q = filename;
  jnlfn = malloc (strlen (q) + 6);
  strcpy (jnlfn, q);
  strcat (jnlfn, ".edtj");

#endif
  return (jnlfn);
}

/************************************************************************/
/*									*/
/*  Create new file							*/
/*									*/
/*    Input:								*/
/*									*/
/*	name = name of file to create					*/
/*									*/
/*    Output:								*/
/*									*/
/*	os_crenewfile = NULL : error (message already output)		*/
/*	                else : file pointer				*/
/*									*/
/************************************************************************/

FILE *os_crenewfile (const char *name)

{
#if defined (VMS) || defined (_OZONE)

  FILE *newfile;

  newfile = fopen (name, "w");
  if (newfile == NULL) outerr (strlen (name) + strlen (strerror (errno)), "error creating file %s: %s\n", name, strerror (errno));
  return (newfile);

#else

  char *newname;
  FILE *newfile;
  struct stat statbuf;
  uLong ver;

  /* Get characteristics of old file - if it doesn't exist, just create new file */

  if (stat (name, &statbuf) < 0) {
    if (errno != ENOENT) {
      outerr (strlen (name) + strlen (strerror (errno)), "error statting file %s: %s\n", name, strerror (errno));
      return (NULL);
    }
    newfile = fopen (name, "w");
    if (newfile == NULL) outerr (strlen (name) + strlen (strerror (errno)), "error creating file %s: %s\n", name, strerror (errno));
    return (newfile);
  }

  /* Try to rename existing file to <name>.<n>~ */

  newname = malloc (strlen (name) + 12);
  for (ver = 0; ++ ver;) {
    sprintf (newname, "%s.%u~", name, ver);
    if (link (name, newname) >= 0) break;
    if (errno != EEXIST) {
      outerr (strlen (name) + strlen (newname) + strlen (strerror (errno)), "error renaming %s to %s: %s\n", name, newname, strerror (errno));
      free (newname);
      return (NULL);
    }
  }
  outerr (strlen (name) + strlen (newname), "renamed old file %s to %s\n", name, newname);
  free (newname);
  if (unlink (name) < 0) {
    outerr (strlen (name) + strlen (strerror (errno)), "error removing old name %s: %s\n", name, strerror (errno));
    return (NULL);
  }

  /* Create the new file */

  newfile = fopen (name, "w");
  if (newfile == NULL) {
    outerr (strlen (name) + strlen (strerror (errno)), "error creating file %s: %s\n", name, strerror (errno));
    return (NULL);
  }

  /* Change characteristics of new file to match old one */
  /* If error, just warn them, they can fix it manually  */

  if (fchown (fileno (newfile), statbuf.st_uid, statbuf.st_gid) < 0) {
    outerr (strlen (name) + strlen (strerror (errno)) + 12, "error setting %s owner %u.%u: %s\n", name, statbuf.st_uid, statbuf.st_gid, strerror (errno));
  }

  if (fchmod (fileno (newfile), statbuf.st_mode) < 0) {
    outerr (strlen (name) + strlen (strerror (errno)) + 6, "error setting %s mode %o: %s\n", name, statbuf.st_mode, statbuf.st_gid, strerror (errno));
  }

  return (newfile);

#endif
}

/************************************************************************/
/*									*/
/*  Get default initialization file name				*/
/*									*/
/************************************************************************/

char *os_defaultinitname (void)

{
#if defined (VMS)

  char *p;

  p = getenv ("EDT_INITFILE");
  if (p == NULL) p = "sys$login:edt_init.edt";
  return (p);

#elif defined (_OZONE)

  return ("EDT_INITFILE");

#else

  char *p, *q;

  p = getenv ("EDT_INITFILE");
  if (p != NULL) return (p);
  p = getenv ("HOME");
  if (p != NULL) {
    q = malloc (strlen (p) + 16);
    strcpy (q, p);
    strcat (q, "/.edt_init");
    return (q);
  }
  return (NULL);

#endif
}

#if defined (VMS)

/* Turn journal flushing ast on or off */
/* It is turned on during keyboard reading and turned off elsewhere, because 
/* we don't know how the ast would interfere with other crtl operations */

static void setjnlflush (int on)

{
  uLong sts;
  uQuad now;

  sys$setast (0);						/* inhibit ast delivery */
  jnlflushenable = on;						/* save the enable flag */
  sys$gettim (&now);						/* see what time it is now */

  /* If we are exiting read routine and we don't have a next flush time established, set one up */

  if (!on && (nextjournalflush == 0)) nextjournalflush = now + JOURNAL_FLUSH_INTERVAL * 10000000;

  /* If we are entering read routine and there is no timer in progress, start the timer going */

  if (on && (nextjournalflush != 0) && !jnlflushtimerip) {
    if (nextjournalflush <= now) {
      jnl_flush ();						/* it is already time, flush it */
      nextjournalflush = 0;					/* don't start timer until a read completes */
    } else {
      jnlflushtimerip = 1;					/* not time yet, start timer going */
      sts = sys$setimr (0, &nextjournalflush, jnlflushast, NULL, 0);
      if (!(sts & 1)) lib$stop (sts);
    }
  }
  sys$setast (1);						/* enable ast delivery */
}

/* This ast routine gets called when the journal flush timer expires. */
/* It flushes the journal iff we are in the read routine, so as to    */
/* not interfere with any other CRTL stuff that may be going on now.  */

static void jnlflushast (void)

{
  jnlflushtimerip = 0;		/* an timer ast is no longer pending */
  if (jnlflushenable) {		/* see if we are in the read routine */
    jnl_flush ();		/* if so, it is ok to flush the journal */
    nextjournalflush = 0;	/* ... and remember that is was just flushed */
  }
}

#elif defined (_OZONE)

/* Turn journal flushing ast on or off */
/* It is turned on during keyboard reading and turned off elsewhere, because 
/* we don't know how the ast would interfere with other crtl operations */

static void setjnlflush (int on)

{
  OZ_Datebin now;
  OZ_IO_timer_waituntil timer_waituntil;
  uLong sts;

  oz_sys_thread_setast (OZ_ASTMODE_INHIBIT);			/* inhibit ast delivery */
  jnlflushenable = on;						/* save the enable flag */
  now = oz_hw_tod_getnow ();					/* see what time it is now */

  /* If we are exiting read routine and we don't have a next flush time established, set one up */

  if (!on && (nextjournalflush == 0)) nextjournalflush = now + JOURNAL_FLUSH_INTERVAL * OZ_TIMER_RESOLUTION;

  /* If we are entering read routine and there is no timer in progress, start the timer going */

  if (on && (nextjournalflush != 0) && !jnlflushtimerip) {
    if (nextjournalflush <= now) {
      jnl_flush ();						/* it is already time, flush it */
      nextjournalflush = 0;					/* don't start timer until a read completes */
    } else {
      jnlflushtimerip = 1;					/* not time yet, start timer going */
      memset (&timer_waituntil, 0, sizeof timer_waituntil);
      timer_waituntil.datebin = nextjournalflush;
      sts = oz_sys_io_start (OZ_PROCMODE_KNL, h_timer, NULL, 0, jnlflushast, NULL, OZ_IO_TIMER_WAITUNTIL, sizeof timer_waituntil, &timer_waituntil);
      if (sts != OZ_STARTED) oz_sys_condhand_signal (2, sts, 0);
    }
  }
  oz_sys_thread_setast (OZ_ASTMODE_ENABLE);			/* enable ast delivery */
}

/* This ast routine gets called when the journal flush timer expires. */
/* It flushes the journal iff we are in the read routine, so as to    */
/* not interfere with any other CRTL stuff that may be going on now.  */

static void jnlflushast (void *dummy, uLong status, OZ_Mchargs *mchargs)

{
  jnlflushtimerip = 0;		/* an timer ast is no longer pending */
  if (jnlflushenable) {		/* see if we are in the read routine */
    jnl_flush ();		/* if so, it is ok to flush the journal */
    nextjournalflush = 0;	/* ... and remember that is was just flushed */
  }
}

#else

/* This one does a read but times out so the journal file can be flushed */

static int timedread (char *buf, int siz)

{
  fd_set readmask;
  int rc;
  struct timeval timeout;
  time_t now;

  while (nextjournalflush != 0) {		/* repeat while timeout is armed */
    now = time (NULL);				/* see if time has already elapsed */
    if (now >= nextjournalflush) {
      jnl_flush ();				/* if so, flush the journal */
      nextjournalflush = 0;			/* and disarm the timeout */
      break;
    }
    FD_ZERO (&readmask);			/* if not, set up the read mask for the terminal */
    FD_SET (ttyfd, &readmask);
    timeout.tv_usec = 0;			/* ... and set up how long until it times out */
    timeout.tv_sec  = nextjournalflush - now;
    rc = select (ttyfd + 1, &readmask, NULL, NULL, &timeout); /* wait for timer or for data to read */
    if (rc < 0) return (rc);
    if (rc > 0) break;				/* exit loop if there is something to read */
  }

  rc = read (ttyfd, buf, siz);			/* journal was just flushed, wait for read data */

  if (nextjournalflush == 0) {
    nextjournalflush  = time (NULL);		/* re-arm timeout */
    nextjournalflush += JOURNAL_FLUSH_INTERVAL;
  }

  return (rc);
}
#endif
