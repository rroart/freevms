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
/*	type [<range>]							*/
/*									*/
/*  Types out the range of lines					*/
/*									*/
/************************************************************************/

#include <errno.h>
#include <stdio.h>

#include "edt.h"

static int type_range (void *firstv, Buffer *buffer, Line *line);

void cmd_type (char *cp)

{
  int first;

  first = 1;
  if (range_multiple (cp, &cp, type_range, &first) < 0) return;
  eoltest (cp);
}

static int type_range (void *firstv, Buffer *buffer, Line *line)

{
  /* Set current position to beginning of first line typed out */

  if (*((int *)firstv)) {
    *(int *)firstv = 0;
    if (buffer != cur_position.buffer) {
      *buffer_savpos (cur_position.buffer) = cur_position;
      cur_position.buffer = buffer;
    }
    cur_position.line   = line;
    cur_position.offset = 0;
  }

  /* Type out the line */

  if (line != NULL) line_print (line);
  else outfmt (strlen (buffer_name (buffer)), "[EOB=%s]\n", buffer_name (buffer));

  /* Keep going */

  return (0);
}
