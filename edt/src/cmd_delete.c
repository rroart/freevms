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
/*	delete [<range>]						*/
/*									*/
/*  deletes the range of lines						*/
/*									*/
/************************************************************************/

#include <stdio.h>

#include "edt.h"

static int first;
static uLong count;

static int delete_range (void *dummy, Buffer *buffer, Line *line);

void cmd_delete (char *cp)

{

  first = 1;
  count = 0;
  if (range_multiple (cp, &cp, delete_range, &first) >= 0) eoltest (cp);
  if (count == 0) outerr (0, "no lines deleted\n");
  else outerr (12, "%u line%s deleted\n", count, (count == 1) ? "" : "s");
  if (cur_position.line != NULL) line_print (cur_position.line);
  else outfmt (strlen (buffer_name (cur_position.buffer)), "[EOB=%s]\n", buffer_name (cur_position.buffer));
}

static int delete_range (void *dummy, Buffer *buffer, Line *line)

{
  /* Don't even try to delete the [EOB] line */

  if (line == NULL) return (0);

  /* Set current position to beginning of next line after first line deleted */

  if (first) {
    first = 0;
    if (buffer != cur_position.buffer) {
      *buffer_savpos (cur_position.buffer) = cur_position;
      cur_position.buffer = buffer;
    }
    cur_position.line   = line;
    cur_position.offset = 0;
    buffer_dirty (buffer, 1);
  }

  if (cur_position.line == line) cur_position.line = line_next (line);

  /* Delete the line (and your little string, too!) */

  string_delete (line_remove (line));
  count ++;

  /* Keep going */

  return (0);
}
