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

#include <stdio.h>
#include "edt.h"

char *skipspaces (char *s)

{
  while (*s != 0 && *s <= ' ') s ++;
  return (s);
}

char *uptospace (char *s)

{
  while (*s != 0 && *s > ' ') s ++;
  return (s);
}

int eoltest (char *s)

{
  s = skipspaces (s);
  if (*s == 0) return (1);
  outerr (strlen (s), "extra data at end of command: %s\n", s);
  return (0);
}
