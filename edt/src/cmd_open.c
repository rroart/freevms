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
/*	open <file> [=<buffer>] [-readonly] [-output <file>]		*/
/*									*/
/*  Reads the <file> and puts it in the empty <buffer>			*/
/*  It also causes it to be written on exit				*/
/*									*/
/************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "edt.h"

void cmd_open (char *cp)

{
  Buffer *buffer;
  char *bufname, c, *input_name, *output_name, *p;
  FILE *input_file;
  int endline, l, rdonly;
  Line *line;
  String *string;

  /* Decode command line options */

  bufname     = NULL;
  input_name  = NULL;
  output_name = NULL;
  rdonly      = 0;

  while ((c = *cp) != 0) {
    if (c == '-') {
      if ((cp[7] <= ' ') && (strncasecmp (cp, "-output", 7) == 0)) {
        cp = skipspaces (cp + 7);
        if ((*cp == 0) || (*cp == '-')) goto usage;
        if (output_name != NULL) free (output_name);
        output_name = strdup (cp);
        *(uptospace (output_name)) = 0;
        cp = uptospace (cp);
        cp = skipspaces (cp);
        continue;
      }
      if ((cp[9] <= ' ') && (strncasecmp (cp, "-readonly", 9) == 0)) {
        rdonly = 1;
        if (output_name != NULL) {
          free (output_name);
          output_name = NULL;
        }
        cp = skipspaces (cp + 9);
        continue;
      }
      goto usage;
    }
    if (c == '=') {
      if (bufname != NULL) goto usage;
      bufname = strdup (++ cp);
      for (p = bufname; (c = *p) > ' '; p ++) {
        if (strchr (bufnamechars, c) == NULL) {
          outerr (strlen (bufname), "bad buffer name character %c in %s\n", c, bufname);
          goto usage;
        }
      }
      *p = 0;
      cp = skipspaces (cp + (p - bufname));
      continue;
    }
    if (input_name != NULL) goto usage;
    input_name = strdup (cp);
    p  = uptospace (input_name);
    *p = 0;
    if (!rdonly) output_name = strdup (input_name);
    cp = uptospace  (cp);
    cp = skipspaces (cp);
  }
  if (input_name == NULL) {
    outerr (0, "no filename specified\n");
    goto usage;
  }

  /* If no buffer name given, create it from filename, replacing all garbage chars with underscores */

  if (bufname == NULL) {
    bufname = strdup (input_name);
    for (p = bufname; (c = *p) != 0; p ++) {
      if (strchr (bufnamechars, c) == NULL) *p = '_';
    }
  }

  /* Create the buffer, or if it already exists, it must be empty */

  buffer = buffer_create (strlen (bufname), bufname);
  if (buffer_first_line (buffer) != NULL) {
    outerr (strlen (bufname), "buffer %s is not empty\n", bufname);
    goto cleanup;
  }

  /* If not -readonly, set buffer filename attribute - this will cause it to be written automatically on exit  */
  /* If -readonly, use a blank filename - which will cause a prompt if they try to exit after modifying buffer */

  buffer_setfile (buffer, (output_name == NULL) ? "" : output_name);

  /* Make it the current buffer */

  cur_position.buffer = buffer;
  cur_position.line   = NULL;
  cur_position.offset = 0;

  /* Open the file */

  input_file = fopen (input_name, "r");
  if (input_file == NULL) {
    outerr (strlen (input_name) + strlen (strerror (errno)), "error opening %s: %s\n", input_name, strerror (errno));
    goto cleanup;
  }

  /* Set buffer up to read lines from the file as needed */

  buffer_setreadfile (buffer, input_file);
  cur_position.line = buffer_first_line (buffer);
  goto cleanup;

  /* Some command line error, output message */

usage:
  outerr (0, "usage: open <filename> [=<buffer>] [-output <filename>] [-readonly]\n");

  /* Done, free temp buffers */

cleanup:
  if (bufname != NULL) free (bufname);
  if (input_name != NULL) free (input_name);
  if (output_name != NULL) free (output_name);
}
