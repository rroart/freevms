//+++2004-05-02
//    Copyright (C) 2001,2004 Mike Rieker, Beverly, MA USA
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
//---2004-05-02

/************************************************************************/
/*									*/
/*  Convert character to its representation on the screen		*/
/*									*/
/*    Input:								*/
/*									*/
/*	c    = character to convert					*/
/*	temp = 16-byte temp buffer					*/
/*	col  = column number char will be displayed in (zero based)	*/
/*									*/
/*    Output:								*/
/*									*/
/*	representation = pointer to null-terminated string		*/
/*									*/
/************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "edt.h"

static const char *const chartable[32] = { "<NUL>", "^A",   "^B",   "^C",    "^D",   "^E",   "^F", "^G",
                                            "<BS>", "<HT>", "<LF>", "<VT>",  "<FF>", "<CR>", "^N", "^O",
                                              "^P", "^Q",   "^R",   "^S",    "^T",   "^U",   "^V", "^W",
                                              "^X", "^Y",   "^Z",   "<ESC>", "^\\",  "^]",   "^^", "^_" };

const char *representation (char c, char temp[16], int col)

{
  const char *strp;

  strp = temp;

  if (c & 0x80) sprintf (temp, "<x%2.2x>", c & 0xff);	/* chars with <7> set get displayed in hexadecimal */
  else if (c == 127) strp = "<DEL>";			/* rubouts get displayed as <DEL> */
  else if (c == 9) {					/* tabs ... */
    memset (temp, ' ', 8);				/* ... get displayed as a number of spaces */
    temp[8-(col&7)] = 0;				/* ... depending on what column we're in */
  }
  else if (c < 32) strp = chartable[c];			/* control chars are a string from the table */
  else {
    temp[0] = c;					/* everything else gets displayed as is */
    temp[1] = 0;
  }

  return (strp);
}
