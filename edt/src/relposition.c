//+++2002-08-17
//    Copyright (C) 2001,2002  Mike Rieker, Beverly, MA USA
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
//---2002-08-17

/************************************************************************/
/*									*/
/*  See how p1 is positioned relative to p2				*/
/*									*/
/*    Output:								*/
/*									*/
/*	relposition = 0 : p1 is same as p2 or not related to p2		*/
/*	              1 : p1 comes after p2				*/
/*	             -1 : p1 comes before p2				*/
/*									*/
/************************************************************************/

#include "edt.h"

int relposition (Position *p1, Position *p2)

{
  int firstimethru, p1relp2;
  Line *linebwd, *linefwd;

  p1relp2 = 0;
  if ((p1 -> buffer == p2 -> buffer) && ((p1 -> line != p2 -> line) || (p1 -> offset != p2 -> offset))) {
    if (p1 -> line != p2 -> line) {
      firstimethru = 1;
      for (linebwd = linefwd = p2 -> line; (linebwd != p1 -> line) && (linefwd != p1 -> line);) {
        if (linebwd != NULL) linebwd = line_prev (linebwd);
        else if (firstimethru) linebwd = buffer_last_line (p2 -> buffer);
        if (linefwd != NULL) linefwd = line_next (linefwd);
        if ((linefwd == NULL) && (linebwd == NULL)) break;
        firstimethru = 0;
      }
      if (p1 -> line == linebwd) p1relp2 = -1;
      if (p1 -> line == linefwd) p1relp2 =  1;
    } else {
      p1relp2 = (p1 -> offset > p2 -> offset) * 2 - 1;
    }
  }

  return (p1relp2);
}
