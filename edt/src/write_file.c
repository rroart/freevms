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
/*  Write range of lines to file					*/
/*									*/
/*    Input:								*/
/*									*/
/*	out_name = name of file to write the lines to			*/
/*	beg_line = starting line to write (inclusive)			*/
/*	end_line = ending line to write (inclusive)			*/
/*									*/
/*    Output:								*/
/*									*/
/*	write_file = 0 : write error					*/
/*	             1 : successful					*/
/*									*/
/************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "edt.h"

int write_file (const char *out_name, Line *beg_line, Line *end_line)

{
  FILE *out_file;
  Line *line;
  uLong count;

  out_file = os_crenewfile (out_name);
  if (out_file == NULL) return (0);

  count = 0;
  for (line = beg_line; line != NULL; line = line_next (line)) {
    if (fputs (string_getval (line_string (line)), out_file) < 0) {
      outerr (strlen (out_name) + strlen (strerror (errno)), "error writing file %s: %s\n", out_name, strerror (errno));
      fclose (out_file);
      return (0);
    }
    count ++;
    if (line == end_line) break;
  }

  if (fclose (out_file) < 0) {
    outerr (strlen (out_name) + strlen (strerror (errno)), "error closing file %s: %s\n", out_name, strerror (errno));
    return (0);
  }

  outerr (strlen (out_name) + 12, "%s %u line%s\n", out_name, count, (count != 1) ? "s" : "");
  return (1);
}
