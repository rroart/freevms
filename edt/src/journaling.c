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
/*  Journaling routines							*/
/*									*/
/*  These routines are wrappers around the os_read* routines.  If the 	*/
/*  recovery file is open, they read from it, else they call the os 	*/
/*  read routine.  Either way, if the journal file is open, they write 	*/
/*  the results from the read to it.					*/
/*									*/
/************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "edt.h"

/************************************************************************/
/*									*/
/*  Read from journal or terminal (in line mode) with prompt		*/
/*									*/
/*    Input:								*/
/*									*/
/*	prompt = prompt string pointer					*/
/*									*/
/*    Output:								*/
/*									*/
/*	jnl_readprompt = NULL : eof					*/
/*	                 else : string pointer				*/
/*									*/
/************************************************************************/

String *jnl_readprompt (const char *prompt)

{
  char *buff, header[16], *p;
  String *string;
  uLong strln;

  /* If recovery file open, read from it, otherwise read from terminal */

  if (recover_file == NULL) goto readterminal;
  if (fgets (header, sizeof header, recover_file) == NULL) goto closerecovery;
  if (header[0] != '*') goto badrecovery;
  if (strcmp (header + 1, "EOF\n") == 0) {
    string = NULL;
    goto writejournal;
  }
  strln = strtoul (header + 1, &p, 10);
  if (*p != '\n') goto badrecovery;
  buff = malloc (strln + 1);
  if (fread (buff, strln, 1, recover_file) == 0) goto closerecovery;
  string = string_create (strln, buff);
  buff[strln] = 0;
  outfmt (strlen (prompt) + strlen (buff), "%s%s\n", prompt, buff);
  free (buff);
  goto writejournal;

badrecovery:
  outerr (strlen (header), "bad recovery file format at %s\n", header);
closerecovery:
  fclose (recover_file);
  recover_file = NULL;

  /* Read from terminal screen after flushing any output */

readterminal:
  output ();
  string = os_readprompt (prompt);

  /* Write journal record */

writejournal:
  if (journal_file != NULL) {
    if (string == NULL) fprintf (journal_file, "*EOF\n");
    else {
      strln = string_getlen (string);
      fprintf (journal_file, "*%u\n", strln);
      fwrite (string_getval (string), strln, 1, journal_file);
    }
  }

  /* Return pointer to string (NULL for eof) */

  return (string);
}

/************************************************************************/
/*									*/
/*  Read keypad sequence from journal or terminal without echoing	*/
/*									*/
/*    Output:								*/
/*									*/
/*	jnl_readkeyseq = 0 : eof					*/
/*	                 1 : keysequences appended to keystring		*/
/*									*/
/************************************************************************/

int jnl_readkeyseq (String *keystring)

{
  char *buff, header[16], *p;
  int ok;
  uLong origlen, strln;

  origlen = string_getlen (keystring);

  /* If recovery file open, read from it, otherwise read from terminal */

  if (recover_file == NULL) goto readterminal;
  ok = 1;
  if (fgets (header, sizeof header, recover_file) == NULL) goto closerecovery;
  if (header[0] != '*') goto badrecovery;
  if (strcmp (header + 1, "EOF\n") == 0) {
    ok = 0;
    goto writejournal;
  }
  strln = strtoul (header + 1, &p, 10);
  if (*p != '\n') goto badrecovery;
  buff = malloc (strln);
  if (fread (buff, strln, 1, recover_file) == 0) goto closerecovery;
  string_concat (keystring, strln, buff);
  free (buff);
  goto writejournal;

badrecovery:
  outerr (strlen (header), "bad recovery file format at %s\n", header);
closerecovery:
  fclose (recover_file);
  recover_file = NULL;

  /* Read from terminal screen after flushing any output */

readterminal:
  output ();
  ok = os_readkeyseq (keystring);

  /* Write journal record */

writejournal:
  if (journal_file != NULL) {
    if (!ok) fprintf (journal_file, "*EOF\n");
    else {
      strln = string_getlen (keystring) - origlen;
      fprintf (journal_file, "*%u\n", strln);
      fwrite (string_getval (keystring) + origlen, strln, 1, journal_file);
    }
  }

  return (ok);
}

/************************************************************************/
/*									*/
/*  Call this periodically to flush journal file			*/
/*									*/
/************************************************************************/

void jnl_flush (void)

{
  if (journal_file != NULL) {
    if (fflush (journal_file) < 0) {
      outerr (strlen (strerror (errno)), "error flushing journal file: %s\n", strerror (errno));
      fclose (journal_file);
      journal_file = NULL;
    }
  }
}

/************************************************************************/
/*									*/
/*  Close and maybe delete journal file					*/
/*									*/
/************************************************************************/

void jnl_close (int del)

{
  if (journal_file != NULL) {
    if (fclose (journal_file) < 0) outerr (strlen (strerror (errno)), "error closing journal file: %s\n", strerror (errno));
    journal_file = NULL;
  }
  if (del && (journal_name != NULL) && (journal_name[0] != 0) && (unlink (journal_name) < 0)) {
    outerr (strlen (journal_name) + strlen (strerror (errno)), "error deleting journal file %s: %s\n", journal_name, strerror (errno));
  }
}
