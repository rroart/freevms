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
/*	exit [-save] [<filename>]					*/
/*									*/
/************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "edt.h"

void cmd_exit (char *cp)

{
  const char *bn, *fn, *ynanswer;
  Buffer *buffer;
  int prompt, saveflag, ynflag;
  String *ynstring;

  /* Check for -save qualifier - it inhibits deleting the journal file */

  saveflag = 0;
  if ((strncasecmp (cp, "-save", 5) == 0) && (cp[5] <= ' ')) {
    if ((journal_name == NULL) || (journal_name[0] == 0)) {
      outerr (0, "there is no journal file to save\n");
      return;
    }
    saveflag = 1;
    cp = skipspaces (cp + 5);
  }

  /* Maybe use a different name for main buffer */

  if (*cp != 0) buffer_setfile (main_buffer, cp);

  /* Don't bother if recovery mode, we want them to be able to do more.  They can re-type exit command */

  if (recover_file != NULL) {
    outerr (0, "EXIT command ignored in recovery mode\n");
    return;
  }

  /* Check for buffers that have filename of "" (opened -readonly) but were modified */

  prompt = 0;
  for (buffer = NULL; (buffer = buffer_next (buffer)) != NULL;) {
    fn = buffer_filename (buffer);
    if (fn == NULL) continue;
    if (fn[0] != 0) continue;
    if (buffer_dirty (buffer, -1)) {
      bn = buffer_name (buffer);
      if (!prompt) outerr (0, "\n");
      outerr (strlen (bn), "buffer %s was modified but has no output file\n", bn);
      prompt = 1;
    }
  }
  if (prompt) {
    do {
      ynstring = jnl_readprompt ("\r\n  do you still want to exit (yes or no)? ");
      if (ynstring == NULL) return;
      ynflag   = -1;
      ynanswer = string_getval (ynstring);
      if (strcasecmp (ynanswer, "no")  == 0) ynflag = 0;
      if (strcasecmp (ynanswer, "yes") == 0) ynflag = 1;
      string_delete (ynstring);
    } while (ynflag < 0);
    if (!ynflag) return;
  }

  /* Write the entire contents of all the buffers that have files and that have been modified to their respective files */

  for (buffer = NULL; (buffer = buffer_next (buffer)) != NULL;) {
    fn = buffer_filename (buffer);							/* get output filename */
    if (fn == NULL) continue;								/* if none, don't try to write */
    bn = buffer_name (buffer);								/* ok, write it out */
    if (!buffer_dirty (buffer, -1)) {
      outerr (strlen (fn) + strlen (bn), "not writing %s from =%s because it is unmodified\n", fn, bn);
      continue;
    }
    outerr (strlen (fn) + strlen (bn), "writing %s from =%s: ", fn, bn);
    if (!write_file (fn, buffer_first_line (buffer), buffer_last_line (buffer))) {
      outerr (strlen (fn), "output file %s not written, not exiting\n", fn);		/* if error, don't exit */
      return;
    }
    buffer_dirty (buffer, 0);
  }

  /* Write successful, maybe delete journal file and terminate process */

  jnl_close (!saveflag);
  output ();
  exit (0);
}
