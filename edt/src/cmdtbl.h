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

void cmd_change (char *cmdpnt);
void cmd_copy (char *cmdpnt);
void cmd_defk (char *cmdpnt);
void cmd_delete (char *cmdpnt);
void cmd_exit (char *cmdpnt);
void cmd_find (char *cmdpnt);
void cmd_help (char *cmdpnt);
void cmd_insert (char *cmdpnt);
void cmd_include (char *cmdpnt);
void cmd_move (char *cmdpnt);
void cmd_open (char *cmdpnt);
void cmd_quit (char *cmdpnt);
void cmd_resequence (char *cmdpnt);
void cmd_set (char *cmdpnt);
void cmd_show (char *cmdpmt);
void cmd_substitute (char *cmdpnt);
void cmd_type (char *cmdpnt);
void cmd_write (char *cmdpnt);

static struct { const char *name;
                void (*entry) (char *cmdpnt);
              } cmdtbl[] = {
                 "change", cmd_change, 		/* c... */
                   "copy", cmd_copy, 		/* co... */
                 "delete", cmd_delete, 		/* d... */
                 "defkey", cmd_defk, 		/* def... */
                      "e", cmd_ambiguous, 
                   "exit", cmd_exit, 		/* ex... */
                   "find", cmd_find, 		/* f... */
                   "help", cmd_help, 		/* h... */
                 "insert", cmd_insert, 		/* i... */
                "include", cmd_include, 	/* inc... */
                   "move", cmd_move, 		/* m... */
                   "open", cmd_open,            /* o... */
                    "qui", cmd_ambiguous, 
                   "quit", cmd_quit, 		/* quit */
                    "qvi", cmd_ambiguous, 
                   "qvit", cmd_quit, 		/* qvit */
                   "rese", cmd_ambiguous, 
             "resequence", cmd_resequence, 	/* reseq... */
             "substitute", cmd_substitute, 	/* s... */
                    "set", cmd_set, 		/* se... */
                   "show", cmd_show, 		/* sh... */
                   "type", cmd_type, 		/* t... */
                     "wr", cmd_ambiguous, 
                  "write", cmd_write, 		/* wri */
                      "?", cmd_help, 		/* ? */
                     NULL, NULL };

