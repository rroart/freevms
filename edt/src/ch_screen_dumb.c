//+++2001-10-06
//    Copyright (C) 2001, Mike Rieker, Beverly, MA USA
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
//---2001-10-06

/************************************************************************/
/*									*/
/*  Dumb terminal screen routines					*/
/*									*/
/************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "edt.h"

#define MAXLENGTH  5
#define MAXWIDTH  80

#define AI(__l,__c) (__l-1)*MAXWIDTH+__c

#define CONTENT_CFLAG_G1SET 1
#define CONTENT_CFLAG_UNDER 2
#define CONTENT_CFLAG_BLINK 4
#define CONTENT_CFLAG_REVER 8
#define CONTENT_CFLAG_BOLD 16

typedef struct { Line *lines[MAXLENGTH];
                 char ccodes[MAXLENGTH*MAXWIDTH];
                 char cflags[MAXLENGTH*MAXWIDTH];
               } Content;

static const char *const chartable[32] = { "<NUL>", "^A",  "^B",   "^C",    "^D",   "^E",   "^F", "^G",
                                              "^H", "   ", "<LF>", "<VT>",  "<FF>", "<CR>", "^N", "^O",
                                              "^P", "^Q",  "^R",   "^S",    "^T",   "^U",   "^V", "^W",
                                              "^X", "^Y",  "^Z",   "<ESC>", "^\\",  "^]",   "^^", "^_" };

static int screenlength;	/* number of lines on a page */
static int screenwidth;		/* number of columns in a line */
static int columnumber;		/* in range 0..screenwidth-1 */
static int linenumber;		/* in range 1..screenlength */

static void printchar (char c);

/* Initialization */

int ch_screen_init (void)

{
  int i;

  screenwidth  = MAXWIDTH;
  screenlength = MAXLENGTH;
  ch_screen_num_lines  = screenlength;
  ch_screen_tmar_lines = 1;			/* keep cursor away from top 7 lines */
  ch_screen_bmar_lines = 1;			/* keep cursor away from bottom 7 lines */
  if (ch_screen_num_lines < 4) {
    ch_screen_tmar_lines = ch_screen_num_lines / 3;
    ch_screen_bmar_lines = ch_screen_num_lines / 3;
  }

  return (1);						/* always successful */
}

/* Termination of screen mode */

void ch_screen_term (void)

{
  output ();						/* flush output buffer to screen */
}

/* Refresh screen */

void ch_screen_refresh (void)

{}

/* Display prompt string (on last line of display) */
/* String consists of two null-terminated segments */

void ch_screen_prompt (String *prompt)

{
  char c;
  const char *s;

  s = string_getval (prompt);			/* get prompt string pointer */
  columnumber = strlen (s);			/* get what column we will be in after 1st half displayed */
  outfmt ("\r\n%s", screenlength, s);		/* display 1st half */
  s += columnumber;				/* point to null before second half */
  while ((c = *(++ s)) != 0) printchar (c);	/* display second half but format any control chars */
}

/* Display (error) message */

void ch_screen_message (const char *message)

{
  outfmt (strlen (message), "%s\n", message);
}

/* Return the number of columns a given string would take up on the screen */

uLong ch_screen_chr2col (uLong linesz, const char *linebf)

{
  char temp[16];
  uLong i, ncols;

  ncols = 0;
  for (i = 0; i < linesz; i ++) {
    ncols += strlen (representation (linebf[i], temp, ncols));
  }

  return (ncols);
}

/* Return the number of characters that could wholly fit in the given number of columns */

uLong ch_screen_col2chr (uLong linesz, const char *linebf, uLong ncols)

{
  char temp[16];
  uLong mcols, nchrs;

  mcols = 0;
  for (nchrs = 0; nchrs < linesz; nchrs ++) {
    mcols += strlen (representation (linebf[nchrs], temp, mcols));
    if (mcols > ncols) break;
  }

  return (nchrs);
}

/* Update the screen and read command */

String *ch_screen_read (void)

{
  char *bufname, c;
  const char *s;
  int cpo, i, l, spo;
  Line *line, *line2;
  String *cmdstring;

  /* Generate new screen contents */

  line2 = ch_screen_top_line;						/* point to next line to display */
  linenumber  = 1;							/* set up starting position on screen */
  columnumber = - ch_screen_shiftleft;
genloop:
  if (line2 == NULL) {							/* check for [EOB] */
    bufname = buffer_name (cur_position.buffer);			/* output it */
    outfmt (strlen (bufname), "[EOB=%s]\r\n", bufname);
    goto gendone;							/* all done */
  }
  line  = line2;							/* point to new line */
  l = string_getlen (line_string (line));				/* get data length */
  s = string_getval (line_string (line));				/* get data address */
  line2 = line_next (line);						/* point to line following new line */
  spo = l + 1;								/* assume select position not on this line */
  cpo = l + 1;								/* assume current position not on this line */
  if (line == sel_position.line) spo = sel_position.offset;		/* get select position offset if in this line */
  if (line == cur_position.line) cpo = cur_position.offset;		/* get current position offset if in this line */

  for (i = 0; i < l; i ++) {						/* loop until whole line has been output */
    if (i == spo) { outchr ('{'); columnumber ++; }			/* if we are at the select position output a { */
    if (i == cpo) { outchr ('['); columnumber ++; }			/* if we are at the current position output a [ */
    c = s[i];								/* get the char to be displayed */
    if ((c != '\n') || showlfs) printchar (c);				/* see if we want to display it & output it */
    if (i == cpo) { outchr (']'); columnumber ++; }			/* if we are at the current position output a ] */
    if (i == spo) { outchr ('}'); columnumber ++; }			/* if we are at the select position output a } */
    if (s[i] == '\n') {							/* see if we just ended a screen line */
      outstr ('\r\n');							/* if so, end the line on the terminal */
      if (linenumber == screenlength) goto gendone;			/* stop if we're at bottom of screen */
      linenumber ++;							/* not at bottom, increment to next line */
      columnumber = - ch_screen_shiftleft;				/* start in left column */
    }
  }
  goto genloop;
gendone:

  /* Read in change mode command from the keyboard */

  return (jnl_readprompt ("\r\nC*"));
}

/************************************************************************/
/*									*/
/*  Display a character on the screen					*/
/*									*/
/*    Input:								*/
/*									*/
/*	c = character to be displayed					*/
/*	columnumber = column to display it in (zero based)		*/
/*	              (negative if still in 'shiftleft' area)		*/
/*	linenumber  = line number to display it on (one based)		*/
/*	screenwidth = max chars to output on a single line		*/
/*	screen positioned to (columnumber,linenumber)			*/
/*									*/
/*    Output:								*/
/*									*/
/*	columnumber = incremented by number of chars written to screen	*/
/*									*/
/************************************************************************/

static void printchar (char c)

{
  char temp[16];
  const char *strp;
  int newcol;

  /* Get the representation for the character */

  strp = representation (c, temp, columnumber);

  /* Determine what the new column will be and output it */

  newcol = columnumber + strlen (strp);					/* get what column would be after outputting */
  if (columnumber >= 0) outstr (strp);					/* do the whole thing if not shifted left */
  else if (newcol > 0) outstr (strp + strlen (strp) - newcol);		/* do what we can if shifted left */
  columnumber = newcol;							/* anyway, remember where cursor is now */
}
