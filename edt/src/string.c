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
/*  String object routines						*/
/*									*/
/************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "edt.h"

#define EXTRA_ROOM 128

struct String { uLong len;
                uLong siz;
                char *val;
              };

/* Create a new string */

String *string_create (uLong len, const char *val)

{
  String *string;

  string = malloc (sizeof *string);
  string -> siz = EXTRA_ROOM + len;
  string -> len = len;
  string -> val = malloc (len + EXTRA_ROOM);
  memcpy (string -> val, val, len);

  return (string);
}

/* Delete string */

void string_delete (String *string)

{
  free (string -> val);
  free (string);
}

/* Get the value of a string - returns normal null-terminated array pointer */

const char *string_getval (String *string)

{
  string -> val[string->len] = 0;
  return (string -> val);
}

/* Get the length of a string */

uLong string_getlen (String *string)

{
  return (string -> len);
}

/* Concat a new string onto the end of an old one */

void string_concat (String *string, uLong len, const char *value)

{
  char *newval;

  if (len > 0) {
    if (string -> len + len >= string -> siz) {
      newval = malloc (string -> len + len + EXTRA_ROOM);
      memcpy (newval, string -> val, string -> len);
      free (string -> val);
      string -> val = newval;
    }
    memcpy (string -> val + string -> len, value, len);
    string -> len += len;
  }
}

/* Set an old string to a new value */

void string_setval (String *string, uLong len, const char *value)

{
  if (len >= string -> siz) {
    free (string -> val);
    string -> val = malloc (len + EXTRA_ROOM);
  }
  memcpy (string -> val, value, len);
  string -> len = len;
}

/* Remove a substring */

void string_remove (String *string, uLong length, uLong offset)

{
  if (length + offset > string -> len) crash ("string_remove: length+offset > string's length");
  if (length > 0) {
    memmove (string -> val + offset, string -> val + offset + length, string -> len - length - offset);
    string -> len -= length;
  }
}

/* Insert a new string into an old one */

void string_insert (String *string, uLong offset, uLong length, const char *insert)

{
  char *newval;

  if (offset > string -> len) crash ("string_insert: offset > string's len");

  if (length > 0) {
    if (string -> len + length >= string -> siz) {
      newval = malloc (string -> len + length + EXTRA_ROOM);
      memcpy (newval, string -> val, offset);
      memcpy (newval + offset, insert, length);
      memcpy (newval + offset + length, string -> val + offset, string -> len - offset);
      free (string -> val);
      string -> val = newval;
    } else {
      memmove (string -> val + length + offset, string -> val + offset, string -> len - offset);
      memcpy (string -> val + offset, insert, length);
    }
    string -> len += length;
  }
}

/* Scan a string for a given character */

int string_scanchr (String *string, char chr)

{
  char *p;

  p = memchr (string -> val, string -> len, chr);
  if (p == NULL) return (-1);
  return (p - string -> val);
}

/* Scan a string for a given string */

int string_scanstr (String *string, const char *str)

{
  char *p;

  string -> val[string->len] = 0;
  p = strstr (string -> val, str);
  if (p == NULL) return (-1);
  return (p - string -> val);
}
