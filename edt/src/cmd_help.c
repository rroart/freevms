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
/*  Print out help							*/
/*									*/
/************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "edt.h"

#define MAXARGS 20

void cmd_help (char *cp)

{
  char *argv[MAXARGS], help_line[4096], *p, *q;
  FILE *help_file;
  int argc, argl[MAXARGS], argm, l, level, needmimsg, nomoreinfo;

  nomoreinfo = 0;
  if ((cp[11] <= ' ') && (strncasecmp (cp, "-nomoreinfo", 11) == 0)) {
    nomoreinfo = 1;
    cp = skipspaces (cp + 11);
  }

  /* Split arguments up into levels */

  for (argc = 0; argc < MAXARGS; cp = p) {
    cp = skipspaces (cp);
    p  = uptospace (cp);
    if (p == cp) break;
    argv[argc] = cp;
    if (*p != 0) *(p ++) = 0;
    argl[argc++] = strlen (cp);
  }

  /* Locate the help entry and print it */

  help_file = fopen (help_name, "r");
  if (help_file == NULL) {
    outerr (strlen (help_name) + strlen (strerror (errno)), "error opening %s: %s\n", help_name, strerror (errno));
    return;
  }

  argm  = 0;								/* none of argv has matched up */
  level = 0;								/* lines are at level 0 to start */
  needmimsg = 1;
  while (fgets (help_line, sizeof help_line, help_file) != NULL) {	/* read help record */
    if (help_line[0] == '#') continue;					/* skip over commented-out lines */
    if (help_line[0] > ' ') {						/* see if there is a number at the beginning */
      l = strtol (help_line, &p, 10);					/* see if there is a number there */
      if ((*p <= ' ') && (l > 0)) {
        level = l;							/* there is, go to the new level */
        if (level <= argm) {						/* reset level if we've matched it before */
          argm = level - 1;
          outchr ('\n');
        }
        p  = skipspaces (p);						/* point to the level's keyword */
        q  = uptospace  (p);
        *q = 0;								/* null terminate it */
        if (level == argm + 1) {					/* see if this is the next level to match */
          if (argm < argc) {
            if (strncasecmp (p, argv[argm], argl[argm]) != 0) continue;	/* skip if the name doesn't match the arg */
            if (!needmimsg) { outchr ('\n'); needmimsg = 1; }
            outfmt (strlen (p) + argm * 2, "%*.*s%s\n", argm * 2, argm * 2, "", p);
            argm ++;							/* it matches */
            continue;							/* don't need to print it again */
          }
          if (!nomoreinfo) {
            if (needmimsg) {
              outstr ("\nMore info available for:\n");
              needmimsg = 0;
            }
            outstr ("  ");
            outstr (p);
          }
        }
      }
    }
    if ((argm == argc) && (level <= argc)) {				/* ouput line if levels satisfied */
      if (!needmimsg) { outchr ('\n'); needmimsg = 1; }
      outstr (help_line);
    }
  }
  if (!needmimsg) { outchr ('\n'); needmimsg = 1; }			/* if were doing More info..., terminate the line */

  fclose (help_file);
}
