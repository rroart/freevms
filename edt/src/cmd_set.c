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
/*	set variable value						*/
/*									*/
/************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "edt.h"

static char *strcasestr (const char *s1, const char *s2);

char *(*xstrstr) () = strcasestr;
int  (*xstrncmp) () = strncasecmp;

void cmd_set (char *cp)

{
  Buffer *buffer;
  char c, *p;
  int i, v;

  /* Set autoshift count */

  if (i = matchkeyword (cp, "autoshift", 1)) {
    if (cp[i] > ' ') goto usage;
    cp = skipspaces (cp + i);
    v = strtol (cp, &p, 10);
    if ((p == cp) || !eoltest (p)) goto usage;
    autoshift = v;
    return;
  }

  /* Set lfs to hide or show */

  if (i = matchkeyword (cp, "lfs", 1)) {
    if (cp[i] > ' ') goto usage;
    cp = skipspaces (cp + i);
    if (i = matchkeyword (cp, "hide", 1)) showlfs = 0;
    else if (i = matchkeyword (cp, "show", 1)) showlfs = 1;
    else goto usage;
    if (!eoltest (cp + i)) goto usage;
    return;
  }

  /* Set numbers to hide or show */

  if (i = matchkeyword (cp, "numbers", 1)) {
    if (cp[i] > ' ') goto usage;
    cp = skipspaces (cp + i);
    if (i = matchkeyword (cp, "auto", 1)) shownums = -1;
    else if (i = matchkeyword (cp, "hide", 1)) shownums = 0;
    else if (i = matchkeyword (cp, "show", 1)) shownums = 1;
    else goto usage;
    if (!eoltest (cp + i)) goto usage;
    return;
  }

  /* Set search to exact or generic */

  if (i = matchkeyword (cp, "search", 1)) {
    if (cp[i] > ' ') goto usage;
    cp = skipspaces (cp + i);
    if (i = matchkeyword (cp, "exact", 1)) {
      xstrstr  = strstr;
      xstrncmp = strncmp;
    } else if (i = matchkeyword (cp, "generic", 1)) {
      xstrstr  = strcasestr;
      xstrncmp = strncasecmp;
    } else goto usage;
    if (!eoltest (cp + i)) goto usage;
    return;
  }

  /* Set buffer attributes (-output filename or -readonly) */

  if (*cp == '=') {
    for (p = ++ cp; (c = *cp) != 0; cp ++) if (strchr (bufnamechars, c) == NULL) break;
    if (cp == p) goto usage;
    if (*cp > ' ') goto usage;
    buffer = buffer_create (cp - p, p);
    cp = skipspaces (cp);
    if ((cp[7] <= ' ') && (strncasecmp (cp, "-output", 7) == 0)) {
      cp = skipspaces (cp + 7);
      p  = uptospace (cp);
      if (*p != 0) goto usage;
      buffer_setfile (buffer, cp);
      return;
    }
    if ((cp[9] <= ' ') && (strncasecmp (cp, "-readonly", 9) == 0)) {
      cp = skipspaces (cp + 9);
      if (*cp != 0) goto usage;
      buffer_setfile (buffer, NULL);
      return;
    }
    goto usage;
  }

usage:
  outerr (0, "set autoshift <count>\n\n");
  outerr (0, "set lfs {hide | show}\n\n");
  outerr (0, "set numbers {auto | hide | show}\n\n");
  outerr (0, "set search {exact | generic}\n\n");
  outerr (0, "set =<buffer> -output <filename>\n");
  outerr (0, "              -readonly\n\n");
}

/* Generic (case-insensitive) search routines */

static char *strcasestr (const char *s1, const char *s2)

{
  int l;

  l = strlen (s2);
  while (*s1 != 0) {
    if (strncasecmp (s1, s2, l) == 0) return ((char *)s1);
    s1 ++;
  }
  return (NULL);
}
