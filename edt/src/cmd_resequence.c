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
/*	resequence [<range>]						*/
/*									*/
/*  resequences the range of lines					*/
/*  does not move current position					*/
/*									*/
/************************************************************************/

#include <stdio.h>

#include "edt.h"

static uLong count;

static int resequence_range (void *dummy, Buffer *buffer, Line *line);

void cmd_resequence (char *cp)

{
  Line *line;

  count = 0;

  /* If no range given, resequence the whole current buffer */

  if (*cp == 0) {
    for (line = buffer_first_line (cur_position.buffer); line != NULL; line = line_next (line)) {
      line_reseq (line);
      count ++;
    }
  }

  /* Otherwise, process the requested lines */

  else if (range_multiple (cp, &cp, resequence_range, NULL) >= 0) eoltest (cp);

  /* Either way, print out summary */

  if (count == 0) outerr (0, "no lines resequenced\n");
  else outerr (12, "%u line%s resequenced\n", count, (count == 1) ? "" : "s");
}

static int resequence_range (void *dummy, Buffer *buffer, Line *line)

{
  /* Resequence the line */

  if (line != NULL) {
    line_reseq (line);
    count ++;
  }

  /* Keep going */

  return (0);
}
