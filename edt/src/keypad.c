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
/*  These routines handle the keypad codes for change mode		*/
/*									*/
/************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "edt.h"

static struct {
	int clen;		/* strlen (code) */
	const char *code; 	/* code sequence from keyboard routine */
	const char *defk; 	/* 'define key' command's name for the key */
	char *chgm[2];		/* [0]: normal change mode command; [1]: gold change mode command */

	     } keycodes[] = {

	/* Delete key */

	1, "",	"DEL",	" (-DC)",	NULL, 

	/* Keypad keys */

	3, "Op",	"KP0",	" L",		" (10ASC-C)", 
	3, "Oq",	"KP1",	" W",		" CHGCSR", 
	3, "Or",	"KP2",	" EL",		" (+DEL)", 
	3, "Os",	"KP3",	" C",		" ASC", 
	3, "Ot",	"KP4",	" ADV",		" (+R)", 
	3, "Ou",	"KP5",	" BACK",	" (-R)", 
	3, "Ov",	"KP6",	" CUTSR",	" PASTE",
	3, "Ow",	"KP7",	" PAGE",	" EXT?'Command: '",
	3, "Ox",	"KP8",	" (LLLLLLLLLLLLLLLL)", " ",
	3, "Oy",	"KP9",	" APPENDSR",	" (DFSPASTE)",
	3, "Ol",	"KPC",	" (+DC)",	" UNDC", 		/* keypad comma */
	3, "OM",	"KPE",	" .",		" (DFSPASTE'')", 	/* keypad enter */
	3, "Om",	"KPH",	" (+DW)",	" UNDW", 		/* keypad hyphen */
	3, "On",	"KPD",	" SEL",		" DESEL", 		/* keypad period */
	3, "OP",	"PF1",	" GOLD",	" GOLD", 
	3, "OQ",	"PF2",	" HELP",	" HELP", 
	3, "OR",	"PF3",	" ''",		" ?'Search for: '", 
	3, "OS",	"PF4",	" (+DL)",	" UNDL", 

	/* PC Supplemental keypad */

	4, "[1~",	"KINS",	" ",		" ", 
	4, "[2~",	"KHOM",	" (-R)", 	" ", 
	4, "[3~",	"KPGU",	" (-PAGE)",	" ", 
	4, "[4~",	"KDEL",	" (+DW)", 	" UNDW", 
	4, "[5~", 	"KEND",	" (+R)", 	" ", 
	4, "[6~",	"KPGD",	" (+PAGE)",	" ", 

	/* Arrows: DOWN, LEFT, RIGHT, UP */

	3, "[B",	"DN",	" (+V)",	" ", 
	3, "[D",	"LF",	" (-C)",	" SHR", 
	3, "[C",	"RT",	" (+C)",	" SHL", 
	3, "[A",	"UP",	" (-V)",	" ", 

	/* Control keys: @, A-Z, [, \, ], ^, _ */

	1, "\000",	"NUL",  " (0ASC)",	" (0ASC)", 
	1, "",	"CTLA",	" (1ASC)",	" (1ASC)", 
	1, "",	"CTLB",	" (2ASC)",	" (2ASC)", 
	1, "",	"CTLC",	" (3ASC)",	" (3ASC)", 
	1, "",	"CTLD",	" EX",		" (4ASC)", 
	1, "",	"CTLE",	" (5ASC)",	" (5ASC)", 
	1, "",	"CTLF",	" (6ASC)",	" (6ASC)", 
	1, "",	"CTLG",	" (7ASC)",	" (7ASC)", 
	1, "",	"CTLH",	" (-L)",	" (8ASC)", 
	1, "	",	"CTLI",	" (9ASC)",	" (9ASC)", 
	1, "\n",	"CTLJ",	" (-DW)",	" (10ASC)", 
	1, "",	"CTLK",	" DEFK",	" (11ASC)", 
	1, "",	"CTLL",	" (12ASC)",	" (12ASC)", 
	1, "\r",	"CTLM",	" (10ASC)",	" (13ASC)", 
	1, "",	"CTLN",	" (14ASC)",	" (14ASC)", 
	1, "",	"CTLO",	" (15ASC)",	" (15ASC)", 
	1, "",	"CTLP",	" (16ASC)",	" (16ASC)", 
	1, "",	"CTLQ",	" (17ASC)",	" (17ASC)", 
	1, "",	"CTLR",	" REF",		" (18ASC)", 
	1, "",	"CTLS",	" (19ASC)",	" (19ASC)", 
	1, "",	"CTLT",	" (20ASC)",	" (20ASC)", 
	1, "",	"CTLU",	" (-DL)",	" (21ASC)", 
	1, "",	"CTLV",	" (22ASC)",	" (22ASC)", 
	1, "",	"CTLW",	" REF",		" (23ASC)", 
	1, "",	"CTLX",	" (24ASC)",	" (24ASC)", 
	1, "",	"CTLY",	" (25ASC)",	" (25ASC)", 
	1, "",	"CTLZ",	" EX",		" (26ASC)", 
	2, "", "ESC",	" (27ASC)",	" (27ASC)", 
	1, "",	"CTBS", " (28ASC)",	" (28ASC)", 
	1, "",	"CTCB", " (29ASC)",	" (29ASC)", 
	1, "",	"CTCT", " (30ASC)",	" (30ASC)", 
	1, "",	"CTUN", " (31ASC)",	" (31ASC)", 

	/* End of table */

	0, NULL, 	NULL,	NULL,		NULL };

/************************************************************************/
/*									*/
/*  Define keypad key							*/
/*									*/
/************************************************************************/

void cmd_defk (char *cmdpnt)

{
  char *p;
  int g, i;

  /* Set 'g' to 1 if there is a g prefix, indicating gold */

  g = 0;
  if ((*cmdpnt == 'g') || (*cmdpnt == 'G')) {
    g = 1;
    cmdpnt ++;
  }

  /* Point to end of key name */

  p = uptospace (cmdpnt);

  /* Search key table for key name */

  for (i = 0; keycodes[i].defk != NULL; i ++) {
    if (keycodes[i].defk[p-cmdpnt] != 0) continue;
    if (strncasecmp (cmdpnt, keycodes[i].defk, p - cmdpnt) == 0) goto foundit;
  }
  *p = 0;
  outerr (strlen (cmdpnt), "unknown keypad code %s\n", cmdpnt);
  return;
foundit:

  /* If command string pointer is NULL, the key is not definable */

  if (keycodes[i].chgm[g] == NULL) {
    *p = 0;
    outerr (strlen (cmdpnt - g), "key %s is not definable\n", cmdpnt - g);
    return;
  }

  /* If current command begins with a space, it is the original string and should not */
  /* be freed off.  Otherwise, it is a previous redefinition and should be freed.     */

  if (*(keycodes[i].chgm[g]) != ' ') free (keycodes[i].chgm[g]);

  /* Ok, skip to the command string and save pointer to the copy.  We   */
  /* are guaranteed the new command string does not begin with a space. */

  p = skipspaces (p);
  keycodes[i].chgm[g] = strdup (p);
}

/************************************************************************/
/*									*/
/*  Show keypad definitions						*/
/*									*/
/************************************************************************/

void show_keypad (void)

{
  char *gold, *stan;
  int i;

  outstr ("\nKeypad definitions:\n");
  outstr ("   Key  Standard                            with Gold key\n");
  outstr ("------  ----------------------------------- -----------------------------------\n");
  for (i = 0; keycodes[i].defk != NULL; i ++) {
    stan = keycodes[i].chgm[0];
    gold = keycodes[i].chgm[1];
    if (stan == NULL) stan = "<abort command>";
    if (gold == NULL) gold = "<abort command>";
    if (*stan == ' ') stan ++;
    if (*gold == ' ') gold ++;
    if (*stan == 0) stan = "<undefined>";
    if (*gold == 0) gold = "<undefined>";
    outfmt (41 + strlen (stan) + strlen (gold), "%6s  %-35s %s\n", keycodes[i].defk, stan, gold);
  }
}

/************************************************************************/
/*									*/
/*  Get definition for a keypad key					*/
/*									*/
/*    Input:								*/
/*									*/
/*	keyname = name of key to get definition for			*/
/*									*/
/*    Output:								*/
/*									*/
/*	keypad_getdef = NULL : unknown key name				*/
/*	                else : pointer to command string		*/
/*									*/
/************************************************************************/

const char *keypad_getdef (const char *keyname)

{
  const char *p;
  int g, i;

  g = 0;
  if ((keyname[0] == 'g') || (keyname[0] == 'G')) {
    g = 1;
    keyname ++;
  }
  for (i = 0; keycodes[i].defk != NULL; i ++) {
    if (strcasecmp (keycodes[i].defk, keyname) == 0) {
      p = keycodes[i].chgm[g];
      if ((p != NULL) && (*p == ' ')) p ++;
      return (p);
    }
  }
  return (NULL);
}

/************************************************************************/
/*									*/
/*  Set the definition for a keypad key					*/
/*									*/
/*    Input:								*/
/*									*/
/*	keyname = name of key to define					*/
/*	command = definition command string				*/
/*									*/
/************************************************************************/

void keypad_setdef (const char *keyname, const char *command)

{
  int g, i;

  g = 0;
  if ((keyname[0] == 'g') || (keyname[0] == 'G')) {
    g = 1;
    keyname ++;
  }
  for (i = 0; keycodes[i].defk != NULL; i ++) {
    if ((strcasecmp (keycodes[i].defk, keyname) == 0) && (keycodes[i].chgm[g] != NULL)) {
      if (*(keycodes[i].chgm[g]) != ' ') free (keycodes[i].chgm[g]);
      while ((*command != 0) && (*command <= ' ')) command ++;
      keycodes[i].chgm[g] = strdup (command);
      break;
    }
  }
}

/************************************************************************/
/*									*/
/*  Convert a keypad code string to a keypad key name			*/
/*									*/
/*    Input:								*/
/*									*/
/*	keystring = keypad code string					*/
/*									*/
/*    Output:								*/
/*									*/
/*	keypad_getname < 0 : invalid/unknown keystring			*/
/*	               = 0 : partial keystring, come back when complete	*/
/*	               > 0 : valid, number of keystring bytes used	*/
/*	                     keyname filled in with key's name		*/
/*									*/
/************************************************************************/

int keypad_getname (const char *keystring, char keyname[16])

{
  const char *s;
  int g, i;
  uLong l;

  g = 0;
  s = keystring;
  l = strlen (s);
scantable:
  for (i = 0; keycodes[i].clen != 0; i ++) {
    if ((l >= keycodes[i].clen) && (memcmp (s, keycodes[i].code, keycodes[i].clen) == 0)) {
      l -= keycodes[i].clen;
      s += keycodes[i].clen;
      if ((keycodes[i].chgm[g] != NULL) && (strcasecmp (keycodes[i].chgm[g], " GOLD") == 0)) {
        g = 1;
        goto scantable;
      }
      if (g) *(keyname ++) = 'G';
      strcpy (keyname, keycodes[i].defk);
      return (s - keystring);
    }
    if ((l < keycodes[i].clen) && (memcmp (s, keycodes[i].code, l) == 0)) return (0);
  }
  return (-1);
}

/************************************************************************/
/*									*/
/*  Convert a keypad code string to a changemode command string		*/
/*									*/
/*    Input:								*/
/*									*/
/*	keystring = key code string to process				*/
/*									*/
/*    Output:								*/
/*									*/
/*	keypad_decode = 0 : don't attempt to execute cmdstring, it is 	*/
/*	                    only a partial command sequence		*/
/*	                1 : it is ok to execute cmdstring, it ends on 	*/
/*	                    a completed command sequence		*/
/*	                    if string is null, means user has entered 	*/
/*	                    GOLD-DEL and wants to cancel command seq	*/
/*	cmdstring = processed codes					*/
/*									*/
/************************************************************************/

int keypad_decode (String *keystring, String *cmdstring)

{
  char c, *p;
  const char *s;
  int done, goldesc, goldmode, i, insertmode;
  uLong n;

  string_setval (cmdstring, 0, NULL);

  done       = 0;	/* we have not ended on a changemode command */
  goldesc    = 0;	/* not in gold escape mode - set when GOLD was followed by something alphanumeric */
			/* the alphanumerics get put in the cmdstring as direct changemode commands */
			/* or it's good for just entering repeat counts, too */
  goldmode   = 0;	/* the GOLD key is not in effect to start */
  insertmode = 0;	/* we do not have an open 'I' changemode command going */
  n = string_getlen (keystring);
  s = string_getval (keystring);

  while (n > 0) {
    c = *s;

    /* Look for keycode in table - if we get a partial one, we stop processing leaving the partial sequence */
    /* in the keycode buffer.  Hopefully caller will call us back very soon with the rest of the sequence.  */

    for (i = 0; keycodes[i].clen != 0; i ++) {
      if ((n >= keycodes[i].clen) && (memcmp (s, keycodes[i].code, keycodes[i].clen) == 0)) goto foundone;
      if ((n <  keycodes[i].clen) && (memcmp (s, keycodes[i].code, n) == 0)) {
        done = 0;					/* partial sequence, tell it to come back when it has more */
        goto stopnow;					/* we're all done for now till we get the rest */
      }
    }

    /* Not in table and we're gold, append directly to command string (user can type in a repeat count)            */
    /* Not in table and we're not gold, append as part of an insert string (user is typing in text to be inserted) */

    if (goldmode && (c >= '0' && c <= '9')) {		/* see if we're in gold mode and it is a number (for repeat count) */
      if (insertmode) {
        string_concat (cmdstring, 1, "");		/* if so, make sure we're not in insert mode */
        insertmode = 0;					/* now we're not in insert mode */
      }
      done = 0;						/* we can't finish on character about to be processed */
							/* because it would not be a complete changemode command */
      goldesc = 1;					/* we have output some 'gold escaped' characters */
    } else {
      if (!insertmode) {				/* nromal, make sure we're in insert mode */
        string_concat (cmdstring, 1, "I");		/* if not, put an insert command in command string */
        insertmode = 1;					/* now we're in insert mode */
      }
      goldmode = 0;					/* terminate any repeat count stuff */
      goldesc  = 0;
      done = 1;						/* we can finish on character about to be processed */
							/* (but we have to close insert mode first) */
    }
    string_concat (cmdstring, 1, s);			/* put on the character to be inserted */
    n --;						/* remove it from keycode buffer */
    s ++;
    continue;

    /* Found keycode, append equivalent change mode command to command string */

foundone:
    if (goldesc) {					/* if the reason we were in GOLD mode was just to enter escaped chars ... */
      goldesc  = 0;					/* ... make that no longer the case */
      goldmode = 0;					/* ... and exit gold mode, they have to press GOLD key again */
    }
    p = keycodes[i].chgm[goldmode];			/* point to key's corresponding change mode command string */
    if (p != NULL) {
      if (*p == ' ') p ++;				/* skip over leading space, if any */
      if (insertmode) {					/* get out of insert mode */
        string_concat (cmdstring, 1, "");
        insertmode = 0;
      }
      if (strcasecmp ("GOLD", p) == 0) goldmode = 1;	/* if the 'GOLD' key, enter gold mode */
      else {
        string_concat (cmdstring, strlen (p), p);	/* otherwise, append change mode command string */
        goldmode = 0;					/* not GOLD anymore */
        done = 1;					/* we can finish on sequence just processed */
      }
    } else {
      done       = 1;					/* GOLD-DEL, reset */
      goldesc    = 0;
      goldmode   = 0;
      insertmode = 0;
      string_setval (cmdstring, 0, NULL);
    }
    n -= keycodes[i].clen;				/* remove key sequence from input string */
    s += keycodes[i].clen;
  }

  /* If doing insert string, terminate it */

stopnow:
  if (insertmode) string_concat (cmdstring, 1, "");

  /* Return whether or not we need more stuff */

  return (done);
}
