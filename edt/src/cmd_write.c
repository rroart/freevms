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
/*	write filespec [range]						*/
/*									*/
/************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "edt.h"

static char *out_name;
static FILE *out_file;
static uLong linecount;

static int write_range (void *dummy, Buffer *buffer, Line *line);

void cmd_write (char *cp)

{
  int rc;
  Line *line;
  Position beg_write, end_write;

  out_file  = NULL;
  linecount = 0;

  /* Get filename */

  out_name = cp;
  cp = uptospace (cp);
  if (*cp != 0) {
    *(cp ++) = 0;
    cp = skipspaces (cp);
  }

  /* Get optional range.  If not specified, use the whole current buffer. */

  if (*cp != 0) {
    rc = range_multiple (cp, &cp, write_range, out_name);
    if (rc == 0) eoltest (cp);
  } else {
    for (line = buffer_first_line (cur_position.buffer); line != NULL; line = line_next (line)) {
      rc = write_range (out_name, cur_position.buffer, line);
      if (rc != 0) break;
    }
  }

  /* Close output file */

  if (out_file == NULL) outerr (0, "no file created\n");
  else {
    if (linecount == 0) outerr (strlen (out_name), "no lines written to %s\n", out_name);
    else outerr (12 + strlen (out_name), "%u line%s written to %s\n", linecount, (linecount == 1) ? "" : "s", out_name);
    if (fclose (out_file) < 0) outerr (strlen (out_name) + strlen (strerror (errno)), "error closing file %s: %s\n", out_name, strerror (errno));
    out_file = NULL;
  }
}

/* This routine is called back for each line to be written */

static int write_range (void *dummy, Buffer *buffer, Line *line)

{
  if (line != NULL) {

    /* Make sure the file is open */

    if (out_file == NULL) {
      out_file = os_crenewfile (out_name);
      if (out_file == NULL) return (1);
    }

    /* Write line to the file */

    if (fputs (string_getval (line_string (line)), out_file) < 0) {
      outerr (strlen (out_name) + strlen (strerror (errno)), "error writing file %s: %s\n", out_name, strerror (errno));
      return (1);
    }

    /* Increment line counter */

    linecount ++;
  }
  return (0);
}
