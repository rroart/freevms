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
/*	quit [-save]							*/
/*									*/
/************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "edt.h"

void cmd_quit (char *cp)

{
  Buffer *buffer;
  const char *bn, *fn, *ynanswer;
  int modbuffs, saveflag, ynflag;
  Position beg_of_file, end_of_file;
  String *ynstring;

  /* Check for -save qualifier */

  saveflag = 0;
  if ((strncasecmp (cp, "-save", 5) == 0) && (cp[5] <= ' ')) {
    if ((journal_name == NULL) || (journal_name[0] == 0)) {
      outerr (0, "there is no journal file to save\n");
      return;
    }
    saveflag = 1;
    cp = skipspaces (cp + 5);
  }

  if (!eoltest (cp)) return;

  /* If not -save, scan for modified buffers */

  if (!saveflag) {
    modbuffs = 0;
    for (buffer = NULL; (buffer = buffer_next (buffer)) != NULL;) {
      fn = buffer_filename (buffer);							/* get output filename */
      if (fn == NULL) continue;								/* if none, don't try to write */
      bn = buffer_name (buffer);							/* ok, write it out */
      if (!buffer_dirty (buffer, -1)) continue;
      if (!modbuffs) outchr ('\n');
      outerr (strlen (fn) + strlen (bn), "file %s modified in buffer %s\n", fn, bn);
      modbuffs = 1;
    }
    if (modbuffs) do {
      ynstring = jnl_readprompt ("\r\n  do you still want to quit (yes or no)? ");
      if (ynstring == NULL) return;
      ynflag   = -1;
      ynanswer = string_getval (ynstring);
      if (strcasecmp (ynanswer, "no")  == 0) ynflag = 0;
      if (strcasecmp (ynanswer, "yes") == 0) ynflag = 1;
      string_delete (ynstring);
    } while (ynflag < 0);
    if (!ynflag) return;
  }

  /* Don't bother if recovery mode, we want them to be able to do more.  They can re-type quit command */

  if (recover_file != NULL) {
    outerr (0, "QUIT command ignored in recovery mode\n");
    return;
  }

  /* Maybe delete journal file and terminate process */

  jnl_close (!saveflag);
  output ();
  exit (0);
}
