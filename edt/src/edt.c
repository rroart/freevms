//+++2004-06-10
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
//---2004-06-10

/************************************************************************/
/*									*/
/*  Editor main program							*/
/*									*/
/************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "edt.h"

void cmd_exit (char *cmdpnt);
void cmd_open (char *cmdpnt);
void cmd_type (char *cmdpnt);
void cmd_write (char *cmdpnt);

Buffer *main_buffer;
char *input_name, *recover_name;
char *journal_name;			/* journal filename */
const char *pn = "edt";			/* the program name (for error messages) */
FILE *journal_file, *recover_file;	/* journal file and recovery file */
Position cur_position;			/* current position */
int showlfs = 0;			/* 0: LF's are silent; 1: show LF's as <LF> */
int shownums = -1;			/* -1: on for linemode, off for changemode; 0: always off; 1: always on */

int main (int argc, char *argv[])

{
  setvbuf(stdout, NULL, _IONBF, 0);      // need this to see i/o at all
  char *buf, *init_name, *output_name, *p;
  const char *cmdpnt;
  FILE *init_file;
  int i, len, rdonly, recover, siz;
  String *cmdstr;
  uLong sts;

  if (argc > 0) pn = argv[0];

  fprintf (stderr, 
	"Copyright (C) 2001,2002,2003,2004 Mike Rieker, Beverly, MA USA\n"
	"Version 2004-06-10, EDT comes with ABSOLUTELY NO WARRANTY\n"
	"EXPECT it to FAIL when someone's HeALTh or PROpeRTy is at RISk\n\n");
  fflush (stderr);

  /* Parse command line */

  init_name    = NULL;
  input_name   = NULL;
  journal_file = NULL;
  journal_name = NULL;
  output_name  = NULL;
  rdonly       = 0;
  recover      = 0;
  recover_file = NULL;
  recover_name = NULL;

  for (i = 1; i < argc; i ++) {

    /* -init <file> : process the given initialization file */

    if (strcasecmp (argv[i], "-init") == 0) {
      if (++ i >= argc) goto usage;
      if (argv[i][0] == '-') goto usage;
      if (init_name != NULL) goto usage;
      init_name = argv[i];
      continue;
    }

    /* -journal <file> : write journal to specified file */
    /* by default, it gets written to <output_file>.edtj */

    if (strcasecmp (argv[i], "-journal") == 0) {
      if (++ i >= argc) goto usage;
      if (argv[i][0] == '-') goto usage;
      journal_name = argv[i];
      continue;
    }

    /* -noinit : don't process default init file */

    if (strcasecmp (argv[i], "-noinit") == 0) {
      if (init_name != NULL) goto usage;
      init_name = "";
      continue;
    }

    /* -output <file> : write output to specified file */
    /* by default, it gets written to <input_file>     */

    if (strcasecmp (argv[i], "-output") == 0) {
      if (++ i >= argc) goto usage;
      if (argv[i][0] == '-') goto usage;
      output_name = argv[i];
      continue;
    }

    /* -readonly : don't write an output file */

    if (strcasecmp (argv[i], "-readonly") == 0) {
      if (output_name == input_name) output_name = NULL;
      rdonly = 1;
      continue;
    }

    /* -recover [<file>] : process recovery from file           */
    /* by default, recovery is processed from <input_file>.edtj */

    if (strcasecmp (argv[i], "-recover") == 0) {
      recover = 1;
      if (i + 1 == argc) continue;
      if (argv[i+1][0] == '-') continue;
      if ((input_name != NULL) || (i + 2 <= argc)) recover_name = argv[++i];
      continue;
    }

    /* No more options */

    if (argv[i][0] == '-') goto usage;

    /* first and only parameter <file> : input filename */

    if (input_name == NULL) {
      input_name = argv[i];
      if (!rdonly) output_name = input_name;
      continue;
    }
    goto usage;
  }

  /* Open recovery file */

  if (recover && (input_name == NULL)) {
    fprintf (stderr, "no input file specified to recover\n");
    return (-1);
  }
  if (recover && (recover_name == NULL)) recover_name = os_makejnlname (input_name);
  if (recover_name != NULL) {
    recover_file = fopen (recover_name, "r");
    if (recover_file == NULL) {
      fprintf (stderr, "error opening recovery file %s: %s\n", recover_name, strerror (errno));
      return (-1);
    }
  }

  /* Create journal file */

  if ((output_name != NULL) && (journal_name == NULL)) journal_name = os_makejnlname (output_name);
  if (journal_name != NULL) {
    journal_file = os_crenewfile (journal_name);
    if (journal_file == NULL) {
      fprintf (stderr, "error creating journal file %s: %s\n", journal_name, strerror (errno));
      return (-1);
    }
  }

  /* Initialize os dependent routines.  No using stdin/stdout/stderr from now on. */

  os_initialization ();

  /* If input file was given, read it into a buffer and mark that buffer for writing on exit */

  if (input_name != NULL) {
    p = input_name;
    if (rdonly) {
      p = malloc (strlen (input_name) + 12);
      strcpy (p, "-readonly ");
      strcat (p, input_name);
    }
    cmd_open (p);
    if (p != input_name) free (p);
    cur_position.line   = buffer_first_line (cur_position.buffer);
    cur_position.offset = 0;
  }

  /* Otherwise, allocate initial 'MAIN' buffer with no lines in it */

  else {
    cur_position.buffer = buffer_create (4, "MAIN");
    cur_position.line   = NULL;
    cur_position.offset = 0;
  }

  /* Either way, that is the one used by the EXIT command */

  main_buffer = cur_position.buffer;
  if (output_name != NULL) buffer_setfile (main_buffer, output_name);

  /* Process initialization file */

  if (init_name == NULL) init_name = os_defaultinitname ();	/* if no -noinit or -init, get default name */
  if ((init_name != NULL) && (init_name[0] != 0)) {
    init_file = fopen (init_name, "r");				/* try to open init file */
    if (init_file != NULL) {
      siz = 256;						/* start with 256 byte buffer */
      buf = malloc (siz);
      len = 0;							/* don't have anything in it */
      while (fgets (buf + len, siz - len, init_file) != NULL) {	/* read onto end of what's there */
        len += strlen (buf + len);				/* get total length */
        if (len == 0) break;					/* stop if nothing there (eof?) */
        if (buf[len-1] != '\n') {				/* check for line terminator */
          siz += 256;						/* didn't get the whole thing, increase buffer */
          buf  = realloc (buf, siz);				/* ... then loop to read more of the line */
        } else {
          buf[--len] = 0;					/* got terminator, remove it from buffer */
          cmdpnt = skipspaces (buf);				/* skip leading spaces */
          if (*cmdpnt != 0) ln_command (cmdpnt);		/* process command line */
          len = 0;						/* empty line buffer for next read */
        }
      }
      fclose (init_file);
      free (buf);
    } else if (errno != ENOENT) {
      outerr (strlen (init_name) + strlen (strerror (errno)), "error opening init file %s: %s\n", init_name, strerror (errno));
      exit (-1);
    }
  }

  /* Type out line at current position to begin with */

  cmd_type (".");

  /* Read and process commands until eof */

  i = 0;
  while (1) {
    cmdstr = jnl_readprompt ("\r\n*");
    if (cmdstr == NULL) {						/* prompt then read command line */
      if (++ i < 3) outerr (0, "use either EXIT or QUIT to terminate\n");
      else {
        outerr (12, "%d EOF's in a row or fatal terminal error, exiting ...\n", i);
        cmd_exit ("");
        exit (-1);
      }
      continue;
    }
    i = 0;
    for (cmdpnt = string_getval (cmdstr); *cmdpnt != 0; cmdpnt ++) if (*cmdpnt > ' ') break;
    if (*cmdpnt == 0) cmd_type (".+1");					/* blank line means 'type .+1' */
    else ln_command (cmdpnt);						/* process command */
    string_delete (cmdstr);						/* free the command string off */
  }

  /* Bad command line parameter */

usage:
  fprintf (stderr, "usage: %s [-init <init_file>] [-journal <journal_output>] [-noinit] [-output <output_file>] [-readonly] [-recover [<journal_input>]] [<input_file>]\n", pn);
  return (-1);
}
