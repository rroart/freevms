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
/*	substitute /<old>/<new>/ [<range>]				*/
/*									*/
/*  Substitutes <new> for <old> in the range				*/
/*									*/
/************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "edt.h"

static char delim, *newstr, *oldstr;
static uLong count, newstr_l, oldstr_l;

static int subs_range (void *dummy, Buffer *buffer, Line *line);

void cmd_substitute (char *cp)

{
  char *p;
  const char *s, *t;
  Line *line;
  String *string;
  uLong offset;

  count  = 0;
  newstr = NULL;
  oldstr = NULL;

  delim  = *cp;					/* the delimeter is first non-blank character */
  if (delim == 0) goto badstr;			/* there has to be something there! */
  p = strchr (++ cp, delim);			/* old string starts immediately after the first delimeter */
  if (p == NULL) goto badstr;			/* and must be terminated by second delimeter */
  oldstr = cp;					/* ok, point to start of old string */
  oldstr_l = p - cp;				/* save length of old string */
  if (oldstr_l == 0) goto badstr;		/* it can't be null (can't search for a null string) */
  *(p ++) = 0;					/* null terminate old string, point to new string */
  newstr = p;					/* save pointer to new string */
  cp = strchr (p, delim);			/* look for third delimeter */
  newstr_l = strlen (p);			/* get new string length (in case no 3rd delim) */
  if (cp == NULL) goto justonce;		/* no 3rd delim, just take the whole thing as new string */
  newstr_l = cp - p;				/* 3rd delim, get proper new string length */
  *(cp ++) = 0;					/* replace 3rd delimeter with a null to terminate new string */
  cp = skipspaces (cp);				/* see if there is a range specified */
  if (*cp == 0) goto justonce;			/* if not, just do one substitution on the current line */

  if (range_multiple (cp, &cp, subs_range, NULL) >= 0) eoltest (cp);
  goto printsummary;

badstr:
  outerr (0, "bad strings\n");
  return;

justonce:
  line = cur_position.line;			/* point to current line */
  if (line == NULL) goto printsummary;		/* can't do anything with [EOB] line */
  string = line_string (line);			/* get the string pointer */
  s = string_getval (string);
  t = (*xstrstr) (s + cur_position.offset, oldstr); /* try to find old string there */
  if (t == NULL) goto printsummary;		/* too bad if not there */
  offset = t - s;				/* get the offset from beginning of line */
  string_remove (string, oldstr_l, offset);	/* remove old string from line */
  string_insert (string, offset, newstr_l, newstr); /* insert new string in line */
  cur_position.offset = offset + newstr_l;	/* set up new offset just past new string */
  count ++;					/* count the substitution */
  buffer_dirty (cur_position.buffer, 1);	/* buffer is dirty */
  line_print (line);				/* display the new line */

printsummary:
  if (count == 0) outerr (0, "no substitutions made\n");
  else outerr (12, "%u substitution%s made\n", count, (count == 1) ? "" : "s");
}

static int subs_range (void *dummy, Buffer *buffer, Line *line)

{
  const char *s, *t;
  String *string;
  uLong offset;

  /* We can't do anything with the [EOB] line */

  if (line == NULL) return (0);

  /* See if line contains our old string at all */

  string = line_string (line);			/* get line's string */
  s = string_getval (string);
  t = (*xstrstr) (s, oldstr);			/* see if it contains the old string */
  if (t == NULL) return (0);			/* if not, we're done with it */
  offset = t - s;				/* get the offset within the line of the old string */

  /* If this is the very first find, set current position there */

  if (count == 0) {
    if (buffer != cur_position.buffer) {
      *buffer_savpos (cur_position.buffer) = cur_position;
      cur_position.buffer = buffer;		/* save the buffer */
    }
    cur_position.line   = line;			/* save the line */
    cur_position.offset = offset + newstr_l;	/* save what offset will be at end of new string */
    buffer_dirty (buffer, 1);			/* buffer will be dirty */
  }

  /* Perform substitutions on the whole line */

  while (1) {
    string_remove (string, oldstr_l, offset);	/* remove the old string from the line */
    string_insert (string, offset, newstr_l, newstr); /* insert the new string into the line */
    count ++;					/* count the substitution */
    offset += newstr_l;				/* get offset past the new string */
    s = string_getval (string);			/* string may have moved in memory from _remove and _insert */
    t = (*xstrstr) (s + offset, oldstr);	/* see if there is another occurrence of oldstr in the line */
    if (t == NULL) break;			/* if not, we're done scanning it */
    offset = t - s;				/* if so, get the offset of the oldstr */
  }

  /* Type out the new line */

  line_print (line);

  /* Keep going */

  return (0);
}
