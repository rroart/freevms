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
/*  Read file into buffer						*/
/*									*/
/************************************************************************/

#include <stdio.h>
#include <string.h>

#include "edt.h"

void read_file (FILE *input_file, Buffer *buffer, Line *next_line)

{
  char input_buff[4096], *p, *q;
  int rc;
  String *string;

  while ((string = readfileline (input_file, NULL)) != NULL) {
    line_insert (buffer, next_line, string);
  }
  fclose (input_file);
}

/************************************************************************/
/*									*/
/*  Read line from a file						*/
/*									*/
/*    Input:								*/
/*									*/
/*	input_file = file to read from					*/
/*	string     = NULL : start a new string				*/
/*	             else : concat onto this string			*/
/*									*/
/*    Output:								*/
/*									*/
/*	readfileline = NULL : end-of-file				*/
/*	               else : pointer to string				*/
/*									*/
/************************************************************************/

String *readfileline (FILE *input_file, String *string)

{
  char input_buff[4096];
  int i, rc;

  i = 0;
  while ((rc = fgetc (input_file)) >= 0) {
    input_buff[i++] = rc;
    if (rc == '\n') break;
    if (i == sizeof input_buff) {
      if (string == NULL) string = string_create (i, input_buff);
      else string_concat (string, i, input_buff);
      i = 0;
    }
  }
  if (i != 0) {
    if (string == NULL) string = string_create (i, input_buff);
    else string_concat (string, i, input_buff);
  }
  return (string);
}
