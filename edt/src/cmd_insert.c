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
/*	insert [<range>] [;<text>]					*/
/*									*/
/*  Reads the input and inserts it just before the <range>		*/
/*									*/
/************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "edt.h"

void cmd_insert (char *cp)

{
  Line *line;
  String *string;

  if (range_single (cp, &cp, &cur_position) < 0) return;		/* see where to insert */
  if (*cp == ';') {							/* if ;, insert the remaining string before current line */
    cur_position.offset = 0;
    string = string_create (strlen (cp + 1), cp + 1);
    string_concat (string, 1, "\n");
    buffer_dirty (cur_position.buffer, 1);
    line_insert (cur_position.buffer, cur_position.line, string);
    return;
  }
  if (!eoltest (cp)) return;						/* otherwise, that's all there should be */

  /* Read tty input until eof into the current buffer just before the current line */

  cur_position.offset = 0;
  while ((string = jnl_readprompt ("\r\n               >")) != NULL) {
    string_concat (string, 1, "\n");					/* put line terminator on string */
    buffer_dirty (cur_position.buffer, 1);
    line_insert (cur_position.buffer, cur_position.line, string);	/* insert line just before current line */
  }
}
