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
/*  Execute line-mode command						*/
/*									*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "edt.h"

static void cmd_ambiguous (char *cp);

#include "cmdtbl.h"

int ln_command (const char *cmdstr)

{
  char *cmdbuf, *cmdpnt;
  int i, l;

  cmdbuf = strdup (cmdstr);					/* copy const string to writable */
  cmdpnt = skipspaces (cmdbuf);					/* skip leading spaces */
  for (l = 0; cmdpnt[l] != 0; l ++) if ((cmdpnt[l] < 'A') || (cmdpnt[l] > 'z') || (cmdpnt[l] > 'Z' && cmdpnt[l] < 'a')) break;
  if (l == 0) cmd_type (cmdpnt);				/* no alphabetics means 'type <whatever>' */
  else {
    for (i = 0; cmdtbl[i].name != NULL; i ++) if (strncasecmp (cmdtbl[i].name, cmdpnt, l) == 0) break;
    if (cmdtbl[i].name == NULL) outerr (l, "unknown command: %*.*s\n", l, l, cmdpnt);
    else {
      cmdpnt = skipspaces (cmdpnt + l);				/* ok, skip spaces following command name */
      (*(cmdtbl[i].entry)) (cmdpnt);				/* execute the command */
    }
  }
  free (cmdbuf);						/* free off writable copy */
  return (1);
}

static void cmd_ambiguous (char *cmdpnt)

{
  outerr (0, "ambiguous command\n");
}
