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
/*	show BUFFERS | FILES | KEYPAD					*/
/*									*/
/************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "edt.h"

void cmd_show (char *cp)

{
  char c, curbf, dirty, *p;
  const char *fname, *name;
  int l, rf;
  Buffer *buffer;
  uLong lines;

  if (*cp == 0) goto usage;

  do {
    p = uptospace (cp);

    /* Show buffer info */

    if (strncasecmp ("buffers", cp, p - cp) == 0) {
      outfmt (0, "\nBuffers:\n");
      l = 0;
      for (buffer = NULL; (buffer = buffer_next (buffer)) != NULL;) {
        name  = buffer_name (buffer);
        if (strlen (name) > l) l = strlen (name);
      }
      for (buffer = NULL; (buffer = buffer_next (buffer)) != NULL;) {
        name  = buffer_name (buffer);
        fname = buffer_filename (buffer);
        lines = buffer_linecount (buffer);
        rf    = (buffer_getreadfile (buffer) != NULL);
        curbf = (buffer == cur_position.buffer) ? '>' : ' ';
        dirty = buffer_dirty (buffer, -1) ? '*' : ' ';
        outfmt (strlen (name) + 16, " %c %c %*.*s: %5u%c line%c", curbf, dirty, l, l, name, lines, rf ? '+' : ' ', (lines == 1) ? ' ' : 's');
        if (fname != NULL) outfmt (strlen (fname), " => %s", fname);
        if (buffer == main_buffer) outstr ("  (main buffer)");
        outchr ('\n');
      }
      continue;
    }

    /* Show info about files */

    if (strncasecmp ("files", cp, p - cp) == 0) {
      outfmt (0, "\nFiles:\n");
      outfmt (strlen (help_name),    "     Help: %s\n", help_name);
      outfmt (strlen (journal_name), "  Journal: %s\n", journal_name[0] == 0 ? "<none>" : journal_name);
      continue;
    }

    /* Show keypad definitions */

    if (strncasecmp ("keypad", cp, p - cp) == 0) {
      show_keypad ();
      continue;
    }
    goto usage;

  } while (*(cp = skipspaces (p)) != 0);
  return;

usage:
  outerr (0, "specify BUFFERS, FILES, KEYPAD\n");
}
