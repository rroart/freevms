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
/*	copy <range_1> TO <range_2>					*/
/*									*/
/*  copies the range_1 lines just before range_2			*/
/*									*/
/************************************************************************/

#include <stdio.h>
#include <string.h>

#include "edt.h"

static Position to_position;
static uLong count;

static int copy_range (void *dummy, Buffer *buffer, Line *line);

void cmd_copy (char *cp)

{
  char *p;
  int i;

  count = 0;

  for (i = 0; cp[i] != 0; i ++) {					/* find the ' TO ' in the command string */
    if ((cp[i] <= ' ') && (cp[i+1] == 't' || cp[i+1] == 'T') && (cp[i+2] == 'o' || cp[i+2] == 'O') && (cp[i+3] <= ' ')) break;
  }
  if (cp[i] == 0) {
    outerr (strlen (cp), "can't find ' TO ' in command string %s\n", cp);
    return;
  }

  cp[i] = 0;								/* the range_1 terminates before the 'TO' */
  i += 4;								/* the range_2 starts after the 'TO' */
  if (range_single (cp + i, &p, &to_position) < 0) return;		/* decode range_2 as a single line */
  if (!eoltest (p)) return;
  to_position.offset = 0;

  if (range_multiple (cp, &cp, copy_range, NULL) >= 0) eoltest (cp);	/* decode and process range_1 */

  if (count == 0) outerr (0, "no lines copied\n");			/* print results */
  else outerr (12, "%u line%s copied\n", count, (count == 1) ? "" : "s");

  cur_position = to_position;						/* set current position to range_2 position */
}

static int copy_range (void *dummy, Buffer *buffer, Line *line)

{
  String *string;

  /* Copy the string (don't bother copying the [EOB] line) */

  if (line != NULL) {
    buffer_dirty (to_position.buffer, 1);
    string = line_string (line);
    line_insert (to_position.buffer, to_position.line, string_create (string_getlen (string), string_getval (string)));
    count ++;
  }

  /* Keep going */

  return (0);
}
