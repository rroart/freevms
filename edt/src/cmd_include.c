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
/*	include <file> [<range>]					*/
/*									*/
/*  Reads the <file> and inserts it just before the <range>		*/
/*									*/
/************************************************************************/

#include <errno.h>
#include <string.h>

#include "edt.h"

void cmd_include (char *cp)

{
  char *input_name;
  FILE *input_file;
  Line *line;
  String *string;

  if (*cp == 0) {						/* make sure there's a filename there */
    outerr (0, "no filename specified\n");
    return;
  }
  input_name = cp;						/* get input filename */
  cp = uptospace (cp);						/* it goes up to next space or eol */
  if (range_single (cp, &cp, &cur_position) < 0) return;	/* decode the range to get what line to insert before */
  if (!eoltest (cp)) return;

  /* Open the file */

  input_file = fopen (input_name, "r");
  if (input_file == NULL) {
    outerr (strlen (input_name) + strlen (strerror (errno)), "error opening %s: %s\n", input_name, strerror (errno));
    return;
  }

  /* Read it into the current buffer just before the current line */

  buffer_dirty (cur_position.buffer, 1);				/* it will soon be dirty */
  cur_position.offset = 0;						/* insert before beginning of current line */
  read_file (input_file, cur_position.buffer, cur_position.line);	/* read file in */
}
