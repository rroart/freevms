//+++2004-05-02
//    Copyright (C) 2001,2002,2004  Mike Rieker, Beverly, MA USA
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
//---2004-05-02

/************************************************************************/
/*									*/
/*  Maintain lines in a buffer						*/
/*									*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "edt.h"

#define LOG10FRAC 6			/* number of digits allowed after '.' in a line number */
#define FRAC 1000000

struct Buffer { Buffer *next;		/* next in 'buffers' list */
                Line *first_line;	/* first line in the buffer (NULL if empty) */
                Line *last_line;	/* last line in the buffer (NULL if empty) */
                uLong linecount;	/* number of lines in the buffer (0 if empty) */
                Position savpos;	/* 'saved' position */
                char *filename;		/* associated filename - gets written on exit */
                int dirty;		/* 0: buffer is clean */
					/* 1: buffer is dirty */
                FILE *readfile;		/* read more lines from this file */
                char name[1];		/* buffer's name */
              };

struct Line { Line *next;		/* next line in the buffer */
              Line *prev;		/* previous line in the buffer */
              Buffer *buffer;		/* buffer this line belongs to */
              String *string;		/* string associated with the line */
              uQuad number;		/* the line's number (binary), 0 if not yet assigned */
              char print[16];		/* printable line number (null if not yet assigned) */
            };

static Buffer *buffers = NULL;

static Line *line_alloc (Buffer *buffer, String *string);
static void getprint (char *print, uQuad number);

/************************************************************************/
/*									*/
/*  Create a new buffer							*/
/*									*/
/************************************************************************/

Buffer *buffer_create (int namel, const char *name)

{
  Buffer *buffer, **lbuffer, *nbuffer;

  if ((main_buffer != NULL) && (namel == 4) && (strncasecmp (name, "MAIN", 4) == 0)) return (main_buffer);

  for (lbuffer = &buffers; (nbuffer = *lbuffer) != NULL; lbuffer = &(nbuffer -> next)) {
    if (nbuffer -> name[namel] != 0) continue;			/* see if this name matches ... */
    if (strncasecmp (name, nbuffer -> name, namel) == 0) return (nbuffer); /* if so, return pointer */
  }

  buffer = malloc (sizeof *buffer + namel);			/* not already there, create new struct */
  memset (buffer, 0, sizeof *buffer);				/* clear out fixed portion */
  buffer -> next = nbuffer;					/* set up link to next in buffers list */
  buffer -> savpos.buffer = buffer;				/* set up buffer pointer in saved position */
  memcpy (buffer -> name, name, namel);				/* copy in the name string */
  buffer -> name[namel] = 0;					/* null terminate it */
  *lbuffer = buffer;						/* link previous one to me */
  return (buffer);						/* return pointer to new buffer */
}

/************************************************************************/
/*									*/
/*  Delete buffer							*/
/*									*/
/************************************************************************/

void buffer_delete (Buffer *buffer)

{
  Buffer **lbuffer, *nbuffer;

  for (lbuffer = &buffers; (nbuffer = *lbuffer) != buffer; lbuffer = &(nbuffer -> next)) {}
  *lbuffer = nbuffer -> next;
  if (buffer -> filename != NULL) free (buffer -> filename);
  if (buffer -> readfile != NULL) fclose (buffer -> readfile);
  free (buffer);
}

/************************************************************************/
/*									*/
/*  Get various things about a buffer					*/
/*									*/
/************************************************************************/

Buffer *buffer_next (Buffer *buffer)

{
  if (buffer == NULL) return (buffers);
  return (buffer -> next);
}

Line *buffer_first_line (Buffer *buffer)

{
  Line *line;
  String *string;

  if ((buffer -> first_line == NULL) && (buffer -> readfile != NULL)) {
    string = readfileline (buffer -> readfile, NULL);
    if (string != NULL) {
      line = line_alloc (buffer, string);
      line -> next = NULL;
      line -> prev = NULL;
      buffer -> first_line = buffer -> last_line = line;
    } else {
      fclose (buffer -> readfile);
      buffer -> readfile = NULL;
    }
  }
  return (buffer -> first_line);
}

Line *buffer_last_line (Buffer *buffer)

{
  Line *line;
  String *string;

  while (buffer -> readfile != NULL) {
    if (buffer -> last_line == NULL) buffer_first_line (buffer);
    else line_next (buffer -> last_line);
  }
  return (buffer -> last_line);
}

uLong buffer_linecount (Buffer *buffer)

{
  return (buffer -> linecount);
}

const char *buffer_name (Buffer *buffer)

{
  return (buffer -> name);
}

Position *buffer_savpos (Buffer *buffer)

{
  return (&(buffer -> savpos));
}

const char *buffer_filename (Buffer *buffer)

{
  return (buffer -> filename);
}

void buffer_setreadfile (Buffer *buffer, FILE *readfile)

{
  if (buffer -> readfile != NULL) fclose (buffer -> readfile);
  buffer -> readfile = readfile;
}

FILE *buffer_getreadfile (Buffer *buffer)

{
  return (buffer -> readfile);
}

void buffer_setfile (Buffer *buffer, const char *filename)

{
  if (buffer -> filename != NULL) free (buffer -> filename);
  buffer -> filename = NULL;
  if (filename != NULL) buffer -> filename = strdup (filename);
}

int buffer_dirty (Buffer *buffer, int newdirty)

{
  int olddirty;

  olddirty = buffer -> dirty;
  if (newdirty >= 0) buffer -> dirty = newdirty;
  return (olddirty);
}

/************************************************************************/
/*									*/
/*  Insert new line into buffer						*/
/*									*/
/*    Input:								*/
/*									*/
/*	buffer = buffer the line goes into				*/
/*	next   = next line in list (NULL to insert at the end)		*/
/*	string = string to be inserted					*/
/*									*/
/*    Output:								*/
/*									*/
/*	line_insert = new line that was inserted			*/
/*									*/
/************************************************************************/

Line *line_insert (Buffer *buffer, Line *next, String *string)

{
  Line *line, *prev;

  line = line_alloc (buffer, string);

  /* We are given who follows this new one in the list */
  /* Determine who comes before the new one            */

  if (next != NULL) prev = next -> prev;	/* find out who is before me */
  else prev = buffer_last_line (buffer);

  /* Link the new one to point to predecessor and successor */

  line -> next = next;				/* link me to the next in the list */
  line -> prev = prev;				/* link me to the previous in the list */

  /* Link the predecessor and successor to point to the new one */

  if (next != NULL) next -> prev = line;	/* if there is a next, link it to me */
  else buffer -> last_line  = line;		/* else, i'm the last in the list */
  if (prev != NULL) prev -> next = line;	/* if there is a prev, link it to me */
  else buffer -> first_line = line;		/* else, i'm the first in the list */

  /* Note that a line number doesn't exist until someone wants to see it.  Woo-Woo! */

  return (line);
}

static Line *line_alloc (Buffer *buffer, String *string)

{
  Line *line;

  line = malloc (sizeof *line);			/* allocate struct */
  line -> buffer = buffer;			/* save the buffer it belongs to */
  line -> string = string;			/* save string pointer */
  line -> number = 0;				/* it hasn't been assigned a number yet */
  line -> print[0] = 0;
  buffer -> linecount ++;

  return (line);
}

/************************************************************************/
/*									*/
/*  Remove line from list						*/
/*									*/
/*    Input:								*/
/*									*/
/*	line = line to be removed					*/
/*									*/
/*    Output:								*/
/*									*/
/*	line_remove = string associated with line that was removed	*/
/*									*/
/************************************************************************/

String *line_remove (Line *line)

{
  Line *next, *prev;
  String *string;

  string = line -> string;

  /* Get who my predecessor and successor are */

  next = line -> next;
  prev = line -> prev;

  /* Fix my successor's pointer back to my predecessor             */
  /* If I don't have a successor, my predecessor is last in buffer */

  if (next != NULL) next -> prev = prev;
  else line -> buffer -> last_line = prev;

  /* Fix my predecessor's pointer forward to my successor           */
  /* If I don't have a predecessor, my successor is first in buffer */

  if (prev != NULL) prev -> next = next;
  else line -> buffer -> first_line = next;

  /* One less line in the buffer */

  line -> buffer -> linecount --;

  /* Free off my memory */

  free (line);

  /* Return string pointer */

  return (string);
}

/************************************************************************/
/*									*/
/*  Get printable line number						*/
/*									*/
/*    Input:								*/
/*									*/
/*	line = line to get the printable line of			*/
/*									*/
/*    Output:								*/
/*									*/
/*	line_number = points to null-terminated printable line number	*/
/*									*/
/************************************************************************/

const char *line_number (Line *line)

{
  int l;
  Line *first, *next, *prev;
  uLong count, inc_number;
  uQuad beg_number, end_number, next_number, prev_number;

  if (line == NULL) return ("");

  /* If it hasn't been given a line yet, assign one */

  if (line -> print[0] == 0) {

    /* Count line of lines in a row that don't have lines     */
    /* Save pointer to first one that doesn't have a line     */
    /* Save pointer to previous and next ones that have lines */

    count = -1;
    for (prev = line; prev != NULL; prev = prev -> prev) { if (prev -> print[0] != 0) break; count ++; first = prev; }
    for (next = line; next != NULL; next = next -> next) { if (next -> print[0] != 0) break; count ++; }

    /* Get what number is before unlineed lines and what line is after */

    prev_number =  0;
    next_number = -1;
    if (prev != NULL) prev_number = prev -> number;
    if (next != NULL) next_number = next -> number;

    /* Now determine what increment must be used to make them come out even                                */
    /* Basically, use largest multiple of 10 that fits (fraction-wise), but never more than 1 (whole-wise) */

    for (inc_number = FRAC; inc_number != 0; inc_number /= 10) {
      beg_number = (prev_number / inc_number + 1) * inc_number;
      end_number = count * inc_number + beg_number;
      if (end_number <= next_number) break;
    }

    /* If it doesn't fit (there aren't enough numbers), then increment by 1 */
    /* We will continue on sequencing until the next > the previous         */

    if (inc_number == 0) inc_number = 1;

    /* Assign new line numbers and generate printable string -                           */
    /* Note that we keep going forward through the list until all numbers are increasing */

    do {
      first -> number = beg_number;
      getprint (first -> print, beg_number);
      beg_number += inc_number;
      first = first -> next;
    } while ((first != NULL) && (first -> number < beg_number));
  }

  /* Now that it has a number, return pointer to the string */

  return (line -> print);
}

/************************************************************************/
/*									*/
/*  Compare the given line's number to the given arbitrary number	*/
/*									*/
/************************************************************************/

int line_numcmp (Line *line, const char *number)

{
  char c;
  const char *cp;
  int dp;
  uQuad accum;

  line_number (line);				/* make sure line has a number assigned */
  cp = number;					/* point to given number */
  while ((*cp != 0) && (*cp <= ' ')) cp ++;	/* skip leading spaces */
  accum = 0;					/* clear accumulator */
  dp = -1;					/* no decimal point seen yet */
  while ((c = *(cp ++)) != 0) {			/* get a character */
    if (c == '.') dp = 0;			/* if decimal point, initialise counter */
    else {
      accum = accum * 10 + c - '0';		/* digit, stuff in accumulator */
      if (dp >= 0) dp ++;			/* maybe increment decimal digit counter */
      if (dp >= LOG10FRAC) break;		/* stop if reached max decimal digits */
    }
  }
  if (dp <= 0) accum *= FRAC;			/* if no decimal digits, just multiply by 1000000 */
  else while (dp < LOG10FRAC) {			/* else multiply by 10**(6-dp) */
    accum *= 10;
    dp ++;
  }
  if (line -> number > accum) return (1);	/* finally compare numbers */
  if (line -> number < accum) return (-1);
  return (0);
}

/************************************************************************/
/*									*/
/*  Get other various things about the line				*/
/*									*/
/************************************************************************/

Line *line_next (Line *line)

{
  Buffer *buffer;
  Line *newline;
  String *string;

  if ((line -> next == NULL) && (line -> buffer -> readfile != NULL)) {
    buffer = line -> buffer;
    string = readfileline (buffer -> readfile, NULL);
    if (string != NULL) {
      newline = line_alloc (buffer, string);
      buffer -> last_line -> next = newline;
      newline -> next = NULL;
      newline -> prev = buffer -> last_line;
      buffer -> last_line = newline;
    } else {
      fclose (buffer -> readfile);
      buffer -> readfile = NULL;
    }
  }
  return (line -> next);
}

Line *line_prev (Line *line)

{
  return (line -> prev);
}

String *line_string (Line *line)

{
  return (line -> string);
}

/************************************************************************/
/*									*/
/*  Flag a line for resequencing					*/
/*									*/
/*    Input:								*/
/*									*/
/*	line = line to be resequenced					*/
/*									*/
/*    Output:								*/
/*									*/
/*	line tagged for renumbering					*/
/*									*/
/************************************************************************/

void line_reseq (Line *line)

{
  line -> number   = 0;
  line -> print[0] = 0;
}

/************************************************************************/
/*									*/
/*  Print out line on terminal						*/
/*									*/
/************************************************************************/

void line_print (Line *line)

{
  char chr, temp[16];
  const char *linestr, *rep;
  int column;
  uLong linelen;

  if (line == NULL) outstr ("[EOB]\n");
  else {
    linelen = string_getlen (line -> string);
    linestr = string_getval (line -> string);
    column = 0;
    if (shownums != 0) {
      outfmt (16, "%-15s:", line_number (line));
      column = 16;
    }
    while (linelen > 0) {
      chr = *(linestr ++);
      if ((chr != '\n') || showlfs) {
        rep = representation (chr, temp, column);
        outstr (rep);
        column += strlen (rep);
      }
      if (chr == '\n') {
        outchr ('\n');
        column = 0;
      }
      linelen --;
    }
  }
}

/************************************************************************/
/*									*/
/*  Generates printable number string from internal binary		*/
/*  The string generated must be comparable with strcmp			*/
/*  The string generated must be at least 8 chars long and less than 	*/
/*  16 chars long							*/
/*									*/
/************************************************************************/

static void getprint (char *print, uQuad number)

{
  int l;

  sprintf (print, "%8u.%*.*u", (uLong)(number / FRAC), LOG10FRAC, LOG10FRAC, (uLong)(number % FRAC));
  for (l = strlen (print); print[l-1] == '0'; -- l) {}
  if (print[l-1] == '.') -- l;
  print[l] = 0;
}
