//+++2004-05-02
//    Copyright (C) 2001,2002,2003,2004  Mike Rieker, Beverly, MA USA
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
/*  Screen routines for an VT100 terminal				*/
/*									*/
/************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "edt.h"

/* Escape codes that we use */

#define ESC_BLINK "[5m"				/* turn blink on */
#define ESC_BOLDV "[1m"				/* turn bold on */
#define ESC_EREOL "[K"				/* erase to end of line */
#define ESC_HOMCL "[H[J"			/* home cursor and clear screen */
#define ESC_INIT  ")0=[?1l[?6l[4l"
/* initialization string */
/* <ESC>)0   : select special characters as G1 */
/* <ESC>=    : select keypad escape code mode */
/* <ESC>[?1l : select <ESC>[A,B,C,D for arrow keys */
/* <ESC>[?6l : absolute cursor position (not relative to scroll region) */
/* <ESC>[4l  : set to replacement mode (not insert mode) */
#define ESC_INSRT "[4k"				/* go into insert mode */
#define ESC_NORMV "[m"				/* go back to normal video */
#define ESC_OVSTK "[4l"				/* go into overstrike mode */
#define ESC_ROW   "[%d;1f"				/* go to row '%d', column 1 */
#define ESC_RC    "[%d;%df"			/* go to row '%d', column '%d' */
#define ESC_REVER "[7m"				/* turn reverse video on */
#define ESC_SCRDN "M"				/* scroll down one line */
#define ESC_SCROL "[%d;%dr"			/* set scroll region to line '%d' thru '%d' */
#define ESC_SCRUP "D"				/* scroll up one line */
#define ESC_TERM  ">"				/* termination string */
/* <ESC>> : select keypad numeric code mode */
#define ESC_UNDER "[4m"				/* turn underlining on */

#define CTL_G1ON   ''
#define CTL_G1OFF  ''
#define G1_DIAMOND '`'

/* Screen content struct definition */

#define MAXLENGTH 128
#define MAXWIDTH  256

#define AI(__l,__c) (__l-1)*MAXWIDTH+__c

#define CONTENT_CFLAG_G1SET 1
#define CONTENT_CFLAG_UNDER 2
#define CONTENT_CFLAG_BLINK 4
#define CONTENT_CFLAG_REVER 8
#define CONTENT_CFLAG_BOLD 16

typedef struct
{
    Line *lines[MAXLENGTH];
    char ccodes[MAXLENGTH*MAXWIDTH];
    char cflags[MAXLENGTH*MAXWIDTH];
} Content;

/* Static data */

static const char *const chartable[32] = { "<NUL>", "^A",  "^B",   "^C",    "^D",   "^E",   "^F", "^G",
        "^H", "   ", "<LF>", "<VT>",  "<FF>", "<CR>", "^N", "^O",
        "^P", "^Q",  "^R",   "^S",    "^T",   "^U",   "^V", "^W",
        "^X", "^Y",  "^Z",   "<ESC>", "^\\",  "^]",   "^^", "^_"
                                         };

static int screenlength;	/* number of lines on a page */
static int columnumber;		/* in range 0..ch_screen_width-1 */
static int linenumber;		/* in range 1..screenlength */
static int messagelines;

static Content curcontent;	/* current screen contents */
static Content newcontent;	/* new screen contents */

static void outputnewcontent (int begofs, int endofs, char *cflags_r);
static void printstring (String *string);
static void printchar (char c);
static void storechar (char c, char f);

/* Initialization */

int ch_screen_init (void)

{
    int i;

    if (!os_getscreensize (&ch_screen_width, &screenlength))   /* get screen size */
    {
        ch_screen_width = 0;
        screenlength = 0;
    }
    if (ch_screen_width == 0) ch_screen_width = 80;	/* - default to 80 chars */
    if (screenlength    == 0) screenlength    = 24;	/* - by 24 lines */
    if (ch_screen_width > MAXWIDTH)  ch_screen_width = MAXWIDTH;
    if (screenlength    > MAXLENGTH) screenlength    = MAXLENGTH;
    ch_screen_num_lines  = screenlength - 2;		/* 22 lines for display, 23rd for reading commands, 24th for error messages */
    ch_screen_tmar_lines = 7;				/* keep cursor away from top 7 lines */
    ch_screen_bmar_lines = 7;				/* keep cursor away from bottom 7 lines */
    if (ch_screen_num_lines < 21)
    {
        ch_screen_tmar_lines = ch_screen_num_lines / 3;
        ch_screen_bmar_lines = ch_screen_num_lines / 3;
    }

    for (i = 0; i < screenlength; i ++) outchr ('\n');	/* erase screen by scrolling stuff off the top */
    ch_screen_refresh ();					/* set up terminal modes and clear screen */
    return (1);						/* always successful */
}

/* Termination of screen mode */

void ch_screen_term (void)

{
    outfmt (6, ESC_ROW ESC_TERM "\n", screenlength);	/* go to last line on screen, select keypad numeric code mode */
    output ();						/* flush output buffer to screen */
}

/* Refresh screen */

void ch_screen_refresh (void)

{
    outstr (ESC_INIT ESC_HOMCL);				/* initialize and clear the screen */
    memset (&curcontent, 0, sizeof curcontent);		/* forget about all we have on screen */
    messagelines = 0;					/* no message lines on bottom of screen now */
}

/* Display prompt string (on next to last line of display) */
/* String consists of two null-terminated segments         */
/* The first is displayed reversed, the second is not      */

void ch_screen_prompt (String *prompt)

{
    char c;
    const char *s;

    s = string_getval (prompt);			/* get prompt string pointer */
    columnumber = strlen (s);			/* get what column we will be in after 1st half displayed */
    outfmt (columnumber + 6, ESC_ROW ESC_REVER "%s" ESC_NORMV, screenlength - 1, s); /* display 1st half */
    s += columnumber;				/* point to null before second half */
    while ((c = *(++ s)) != 0) printchar (c);	/* display second half but format any control chars */
    outstr (ESC_EREOL);				/* erase the rest on the line */
    output ();					/* flushit */
}

/* Display (error) message */

void ch_screen_message (const char *message)

{
    if (messagelines != 0) outfmt (strlen(message), "\r\n" ESC_BOLDV "%s" ESC_NORMV, message);
    else outfmt (strlen (message) + 6, ESC_ROW ESC_EREOL "" ESC_BOLDV "%s" ESC_NORMV, screenlength, message);
    messagelines ++;
    output ();					/* flush in case they are going to linemode temporarily */
}

/* Return the number of columns a given string would take up on the screen */

uLong ch_screen_chr2col (uLong linesz, const char *linebf)

{
    char temp[16];
    uLong i, ncols;

    ncols = 0;
    for (i = 0; i < linesz; i ++)
    {
        ncols += strlen (representation (linebf[i], temp, ncols));
    }

    return (ncols);
}

/* Return the number of characters that could wholly fit in the given number of columns */

uLong ch_screen_col2chr (uLong linesz, const char *linebf, uLong ncols)

{
    char temp[16];
    uLong mcols, nchrs;

    mcols = 0;
    for (nchrs = 0; nchrs < linesz; nchrs ++)
    {
        mcols += strlen (representation (linebf[nchrs], temp, mcols));
        if (mcols > ncols) break;
    }

    return (nchrs);
}

/* Update the screen and read command */

String *ch_screen_read (void)

{
    char c, cflags, *eobstr;
    const char *keystr, *s;
    int cpo, curscol, cursrow, i, insert, j, k, l, scrolling[MAXLENGTH], selrelcur, selreltop, spo;
    Line *line, *line2;
    Position tempos;
    uLong n, o;
    String *cmdstring, *keystring;

    /* If there are lots of messages on bottom of screen, make them press <return> to continue then erase the screen */

    if (messagelines > 1)
    {
        outstr ("\r\nPress any key to continue ");
        keystring = string_create (0, NULL);
        jnl_readkeyseq (keystring);
        string_delete (keystring);
        outstr (ESC_HOMCL);
        memset (&curcontent, 0, sizeof curcontent);
    }

    /* Reset message count for the next command cycle */

    messagelines = 0;

    /* If 'show numbers on', set ch_screen_numofs to 12 (the number of columns we use for numbers) */

    ch_screen_numofs = 0;
    if (shownums > 0) ch_screen_numofs = 12;

    /* See if the select range starts before or after the current position                 */
    /* selrelcur = 0: identical spot (or not active); 1: sel after cur; -1: sel before cur */
    /* Same with select range relative to top of screen                                    */

    selrelcur = relposition (&sel_position, &cur_position);
    tempos.buffer = cur_position.buffer;
    tempos.line   = ch_screen_top_line;
    tempos.offset = 0;
    selreltop = relposition (&sel_position, &tempos);

    /* Generate new screen contents, putting result in 'newcontent' struct */

    memset (&newcontent, 0, sizeof newcontent);				/* wipe new screen buffer clean */
    line2  = ch_screen_top_line;						/* point to next line to display */
    cflags = 0;								/* assume no special flags for the character */
    if (selreltop < 0) cflags = CONTENT_CFLAG_REVER;			/* if select starts before top, start in reverse video */
    j = k = n = 0;							/* current position hasn't taken up anything yet */
    linenumber  = 1;							/* set up starting position on screen */
    columnumber = ch_screen_numofs - ch_screen_shiftleft;
genloop:
    if (line2 == NULL)  							/* check for [EOB] */
    {
        cflags &= ~CONTENT_CFLAG_REVER;					/* it is never part of select range */
        if (columnumber < ch_screen_numofs) columnumber = ch_screen_numofs;	/* never shifted left off screen */
        if (columnumber == ch_screen_numofs) memset (newcontent.ccodes + AI (linenumber, 0), ' ', ch_screen_numofs);
        if (cur_position.line == NULL) cflags |= CONTENT_CFLAG_REVER | CONTENT_CFLAG_BOLD; /* reverse video if current position */
        eobstr = malloc (strlen (buffer_name (cur_position.buffer)) + 8);	/* make up the [EOB] string */
        sprintf (eobstr, "[EOB=%s]", buffer_name (cur_position.buffer));
        for (i = 0; (c = eobstr[i]) != 0; i ++) storechar (c, cflags);	/* store in newcontent buffer */
        free (eobstr);
        goto gendone;							/* all done */
    }
    line = line2;								/* point to new line */
    if (ch_screen_numofs > 0)  						/* see if 'show numbers' in effect */
    {
        s = line_number (line);						/* if so, point to line number string */
        l = strlen (s);							/*        get line number string length */
        if (l >= ch_screen_numofs - 1)  					/*        see if too big to fit */
        {
            memcpy (newcontent.ccodes + AI (linenumber, 0), s, ch_screen_numofs - 1); /*    if so, put what we can */
        }
        else
        {
            memset (newcontent.ccodes + AI (linenumber, 0), ' ', ch_screen_numofs - 1 - l); /* if not, put leading spaces */
            memcpy (newcontent.ccodes + AI (linenumber, ch_screen_numofs - 1 - l), s, l);   /*         followed by the number */
        }
        newcontent.ccodes[AI(linenumber,ch_screen_numofs-1)] = ':';		/*        put in the colon */
    }
    l = string_getlen (line_string (line));				/* get data length */
    s = string_getval (line_string (line));				/* get data address */
    line2 = line_next (line);						/* point to line following new line */
    spo = l + 1;								/* assume select position not on this line */
    cpo = l + 1;								/* assume current position not on this line */
    if (line == sel_position.line) spo = sel_position.offset;		/* get select position offset if in this line */
    if (line == cur_position.line) cpo = cur_position.offset;		/* get current position offset if in this line */
    newcontent.lines[linenumber-1] = line;				/* save pointer to the line (for scrolling) */

    for (i = 0; i < l; i ++)  						/* loop until whole line has been output */
    {
        if (i == spo)  							/* see if we are at the select position ... */
        {
            if (selrelcur < 0) cflags |=  CONTENT_CFLAG_REVER;		/* ... if select range starts here, set flag */
            if (selrelcur > 0) cflags &= ~CONTENT_CFLAG_REVER;		/* ... if select range ends here, clear flag */
        }
        if (i == cpo)  							/* see if we are at the current position ... */
        {
            if (selrelcur < 0) cflags &= ~CONTENT_CFLAG_REVER;		/* ... if select range ends here, clear flag */
            if (selrelcur > 0) cflags |=  CONTENT_CFLAG_REVER;		/* ... if select range starts here, set flag */
            j = columnumber;							/* ... save line and column number where char starts */
            n = linenumber;
        }
        c = s[i];								/* get the char to be displayed */
        if ((i == cpo) || (c != '\n') || showlfs)  				/* see if we want to display it */
        {
            if ((i == cpo) && (c == '\n') && !showlfs) c = ' ';		/* maybe use a space for <LF> at current position */
            storechar (c, cflags);						/* output to newcontent buffer */
        }
        if (i == cpo) k = columnumber;					/* ... save column number where char ends */
        if (s[i] == '\n')  							/* see if we just ended a screen line */
        {
            if (linenumber == screenlength) goto gendone;			/* stop if we're at bottom of screen */
            linenumber ++;							/* not at bottom, increment to next line */
            columnumber = ch_screen_numofs - ch_screen_shiftleft;		/* start in left column */
        }
    }
    goto genloop;
gendone:

    /* Modify the video attributes of the current position character to reverse and bold                                        */
    /* But if the current position character is off to the left or right, set the margin's character to reverse, bold and blink */

    /* make it blink in last column if off the end of the line */
    if (j >= ch_screen_width) newcontent.cflags[AI(n,ch_screen_width-1)] |= CONTENT_CFLAG_REVER | CONTENT_CFLAG_BOLD | CONTENT_CFLAG_BLINK;
    else if (j < k)
    {
        /* make it blink in first column if off the end of the line */
        if (k <= ch_screen_numofs)
        {
            k = AI (n, ch_screen_numofs);
            newcontent.cflags[k] |= CONTENT_CFLAG_REVER | CONTENT_CFLAG_BOLD | CONTENT_CFLAG_BLINK;
            if (newcontent.ccodes[k] == 0) newcontent.ccodes[k] = ' ';
        }
        /* otherwise, it is normal (reverse and bold) */
        else while (j < k) newcontent.cflags[AI(n,j++)] |= CONTENT_CFLAG_REVER | CONTENT_CFLAG_BOLD;
    }

    /* Determine where each of the old lines scrolled to */

    memset (scrolling, 0, sizeof scrolling);		/* reset all entries to neutral scrolling */
    for (i = 0; i < screenlength - 2; i ++)  		/* step through each of the old lines */
    {
        if ((i > 0) && (curcontent.lines[i] == NULL) && (curcontent.lines[i-1] == NULL)) break; /* stop on 2nd [EOB] line */
        scrolling[i] = MAXLENGTH;				/* assume it got scrolled off the screen */
        for (l = 0; l < screenlength - 2; l ++)
        {
            if (curcontent.lines[i] == newcontent.lines[l])
            {
                scrolling[i] = l - i;				/* neg: got scrolled up; zero: didn't scroll; pos: got scrolled down */
                break;
            }
        }
    }

    /* Scroll existing lines to where the new ones are */

    n = 0;						/* screen margins not changed */
    for (i = 0; i < screenlength - 2; i ++)
    {
        if (scrolling[i] == 0) continue;			/* just skip lines that are ok as is */
        if (scrolling[i] == MAXLENGTH) continue;		/* just skip over lines that get scrolled off */
        for (j = i; ++ j < screenlength - 2;)  		/* see how many lines to scroll in a like manner */
        {
            if (scrolling[j] != scrolling[i]) break;
            scrolling[j] = 0;					/* it scrolls in a like manner, so don't scroll it again */
        }

        /* i+1 = linenumber of top line to be moved                          */
        /* j   = linenumber of bottom line to move                           */
        /* scrolling[i] = number of lines to move them; up (<0) or down (>0) */

        memmove (curcontent.ccodes + AI (i + scrolling[i] + 1, 0), curcontent.ccodes + AI (i + 1, 0), (j - i) * MAXWIDTH);
        memmove (curcontent.cflags + AI (i + scrolling[i] + 1, 0), curcontent.cflags + AI (i + 1, 0), (j - i) * MAXWIDTH);
        if (scrolling[i] < 0)  				/* see if some lines to be scrolled up */
        {
            memset (curcontent.ccodes + AI (j + scrolling[i] + 1, 0), 0, scrolling[i] * -MAXWIDTH);
            memset (curcontent.cflags + AI (j + scrolling[i] + 1, 0), 0, scrolling[i] * -MAXWIDTH);
            outfmt (12, ESC_SCROL, i + scrolling[i] + 1, j);	/* set scrolling region [where first goes; where last comes from] */
            outfmt (6, ESC_ROW, j);				/* position curson in last line of scrolling region */
            for (j = scrolling[i]; ++ j <= 0;) outstr (ESC_SCRUP); /* scroll up */
        }
        else
        {
            memset (curcontent.ccodes + AI (i + 1, 0), 0, scrolling[i] * MAXWIDTH);
            memset (curcontent.cflags + AI (i + 1, 0), 0, scrolling[i] * MAXWIDTH);
            outfmt (12, ESC_SCROL, i + 1, j + scrolling[i]);	/* set scrolling region [where first comes from; where last goes] */
            outfmt (6, ESC_ROW, i + 1);			/* position curson in first line of scrolling region */
            for (j = scrolling[i]; -- j >= 0;) outstr (ESC_SCRDN); /* scroll down */
        }
        n = 1;						/* margins were changed */
    }
    if (n) outfmt (6, ESC_SCROL, 1, screenlength);	/* restore screen margins if they were changed */

    memcpy (curcontent.lines, newcontent.lines, screenlength * sizeof curcontent.lines[0]); /* the lines supposedly match now */

    /* Modify the line contents as necessary */

    cflags = 0;
    insert = 0;
    for (linenumber = 1; linenumber <= screenlength - 2; linenumber ++)
    {
        n = AI (linenumber, 0);

        /* Skip lines that match both codes and flags */

        if ((memcmp (curcontent.ccodes + n, newcontent.ccodes + n, ch_screen_width) == 0)
                && (memcmp (curcontent.cflags + n, newcontent.cflags + n, ch_screen_width) == 0)) continue;

        /* Not identical, set 'i' to number of chars at beginning of line that match */

        for (i = 0; (curcontent.ccodes[i+n] == newcontent.ccodes[i+n]) && (curcontent.cflags[i+n] == newcontent.cflags[i+n]); i ++) {}

        outfmt (12, ESC_RC, linenumber, i + 1);			/* position cursor at 'i' */
        for (k = ch_screen_width; k > i; -- k) if (newcontent.ccodes[k+n-1] != 0) break;
        outputnewcontent (i + n, k + n, &cflags);			/* overlay old contents with new contents */
        if (k < ch_screen_width)  					/* erase to end of line */
        {
            if (cflags & (CONTENT_CFLAG_UNDER | CONTENT_CFLAG_BLINK | CONTENT_CFLAG_REVER | CONTENT_CFLAG_BOLD))
            {
                cflags &= ~(CONTENT_CFLAG_UNDER | CONTENT_CFLAG_BLINK | CONTENT_CFLAG_REVER | CONTENT_CFLAG_BOLD);
                outstr (ESC_NORMV);					/* (make sure we erase with 'normal' video) */
            }
            outstr (ESC_EREOL);
        }

        /* Well, now supposedly the two lines match ... */

        memcpy (curcontent.ccodes + n + i, newcontent.ccodes + n + i, ch_screen_width - i);
        memcpy (curcontent.cflags + n + i, newcontent.cflags + n + i, ch_screen_width - i);
    }
    if (cflags & CONTENT_CFLAG_G1SET) outchr (CTL_G1OFF);
    if (cflags & (CONTENT_CFLAG_UNDER | CONTENT_CFLAG_BLINK | CONTENT_CFLAG_REVER | CONTENT_CFLAG_BOLD)) outstr (ESC_NORMV);
    if (insert) outstr (ESC_OVSTK);

    /* Read in key sequence, convert to change mode command */

    keystring = string_create (0, NULL);
    cmdstring = string_create (0, NULL);
    do
    {
        outfmt (12, ESC_RC, screenlength, ch_screen_width);		/* put real cursor in lower right corner */
        output ();							/* flush any pending output */
        if (!jnl_readkeyseq (keystring)) break;			/* read key sequence(s) from keyboard */
        i = keypad_decode (keystring, cmdstring);			/* convert them to change mode commands */
        outfmt (6, ESC_ROW ESC_EREOL ESC_BOLDV, screenlength - 1);	/* display on next to last line */
        columnumber = 0;
        printstring (keystring);
        outstr (ESC_NORMV "=>" ESC_BOLDV);
        printstring (cmdstring);
        outstr (ESC_NORMV);
    }
    while ((string_getlen (cmdstring) == 0) || !i);
    string_delete (keystring);
    outfmt (6, ESC_ROW ESC_EREOL, screenlength);			/* go to last line and erase it for messages */
    output ();							/* flush to position cursor while processing */
    return (cmdstring);
}

/************************************************************************/
/*									*/
/*  Output newcontent string to the screen				*/
/*									*/
/*    Input:								*/
/*									*/
/*	begofs = starting index in newcontet.ccodes/cflags arrays	*/
/*	endofs = ending index (exclusive)				*/
/*	*cflags_r = current video attributes				*/
/*	cursor already in position					*/
/*									*/
/*    Output:								*/
/*									*/
/*	characters output to screen					*/
/*	*cflags_r = possibly modified					*/
/*									*/
/************************************************************************/

static void outputnewcontent (int begofs, int endofs, char *cflags_r)

{
    char curcflags, newcflags;
    int i;

    curcflags = *cflags_r;
    for (i = begofs; i < endofs; i ++)
    {

        /* Get the new character's flags */

        newcflags = newcontent.cflags[i];

        /* Change terminal's video attributes iff chars attrs are different than current */

        if (newcflags != curcflags)
        {

            /* Flip the G1SET flag */

            if ((newcflags ^ curcflags) & CONTENT_CFLAG_G1SET)
            {
                if (newcflags & CONTENT_CFLAG_G1SET) outchr (CTL_G1ON);
                else outchr (CTL_G1OFF);
                curcflags ^= CONTENT_CFLAG_G1SET;
            }

            /* If any current video attribute that was set and is now to be cleared, clear them all */

            if (~newcflags & curcflags & (CONTENT_CFLAG_UNDER | CONTENT_CFLAG_BLINK | CONTENT_CFLAG_REVER | CONTENT_CFLAG_BOLD))
            {
                outstr (ESC_NORMV);
                curcflags &= ~(CONTENT_CFLAG_UNDER | CONTENT_CFLAG_BLINK | CONTENT_CFLAG_REVER | CONTENT_CFLAG_BOLD);
            }

            /* Now set any that need to be set */

            if ((newcflags ^ curcflags) & CONTENT_CFLAG_BOLD)
            {
                outstr (ESC_BOLDV);
                curcflags |= CONTENT_CFLAG_BOLD;
            }
            if ((newcflags ^ curcflags) & CONTENT_CFLAG_UNDER)
            {
                outstr (ESC_UNDER);
                curcflags |= CONTENT_CFLAG_UNDER;
            }
            if ((newcflags ^ curcflags) & CONTENT_CFLAG_BLINK)
            {
                outstr (ESC_BLINK);
                curcflags |= CONTENT_CFLAG_BLINK;
            }
            if ((newcflags ^ curcflags) & CONTENT_CFLAG_REVER)
            {
                outstr (ESC_REVER);
                curcflags |= CONTENT_CFLAG_REVER;
            }
        }

        /* Finally, output the character */

        outchr (newcontent.ccodes[i]);
    }

    *cflags_r = curcflags;
}

/************************************************************************/
/*									*/
/*  Display a character on the screen					*/
/*									*/
/*    Input:								*/
/*									*/
/*	c = character to be displayed					*/
/*	columnumber     = column to display it in (zero based)		*/
/*	                  (negative if still in 'shiftleft' area)	*/
/*	linenumber      = line number to display it on (one based)	*/
/*	ch_screen_width = max chars to output on a single line		*/
/*	screen positioned to (columnumber,linenumber)			*/
/*									*/
/*    Output:								*/
/*									*/
/*	columnumber = incremented by number of chars written to screen	*/
/*									*/
/************************************************************************/

static void printstring (String *string)

{
    char c;
    const char *s;

    s = string_getval (string);
    while ((c = *(s ++)) != 0) printchar (c);
}

static void printchar (char c)

{
    char temp[16];
    const char *strp;
    int newcol;

    /* Don't bother if we've already output the diamond for this line */

    if (columnumber > ch_screen_width) return;

    /* Get the representation for the character */

    strp = representation (c, temp, columnumber);

    /* Determine what the new column would be and if it fits, output it */

    newcol = columnumber + strlen (strp);					/* get what column would be after outputting */
    if (newcol <= ch_screen_width)  					/* see if it would be within screen boundaries */
    {
        if (columnumber >= 0) outstr (strp);				/* do the whole thing if not shifted left */
        else if (newcol > 0) outstr (strp + strlen (strp) - newcol);	/* do what we can if shifted left */
        columnumber = newcol;						/* anyway, remember where cursor is now */
    }

    /* It won't fit, draw a diamond in last column */

    else if (columnumber < ch_screen_width)
    {
        outbuf (ch_screen_width - columnumber - 1, strp);			/* draw what will fit with room for diamond */
        outchr (CTL_G1ON);							/* draw the diamond */
        outchr (G1_DIAMOND);
        outchr (CTL_G1OFF);
        columnumber = ch_screen_width + 1;					/* remember we overflowed line */
    }
}

/************************************************************************/
/*									*/
/*  Store character in newcontent buffer				*/
/*									*/
/*    Input:								*/
/*									*/
/*	c = character to be displayed					*/
/*	f = flags to store with it					*/
/*	columnumber     = column to display it in (zero based)		*/
/*	                  zero means left column			*/
/*	                  (lt ch_screen_numofs if in 'shiftleft' area)	*/
/*	linenumber      = line number to display it on (one based)	*/
/*	ch_screen_width = max chars to output on a single line		*/
/*									*/
/*    Output:								*/
/*									*/
/*	newcontent  = updated with character code(s) and flag(s)	*/
/*	columnumber = incremented by number of chars written to screen	*/
/*									*/
/************************************************************************/

static void storechar (char c, char f)

{
    char temp[16];
    const char *strp;
    int newcol, strl;

    /* Don't bother if we've already output the diamond for this line */

    if (columnumber > ch_screen_width) return;

    /* Get the representation for the character */

    strp   = representation (c, temp, columnumber - ch_screen_numofs);	/* get pointer to the rendition */
    strl   = strlen (strp);						/* get length of the rendition */
    newcol = columnumber + strl;						/* get what column would be after outputting */
    if (columnumber < ch_screen_numofs)  					/* see if we're 'shifted left' */
    {
        strp -= columnumber - ch_screen_numofs;				/* if so, lop off that much from start of string */
        strl += columnumber - ch_screen_numofs;
        columnumber = ch_screen_numofs;					/* if anything's left, it goes in first column (after colon) */
    }

    /* If it won't fit, chop at right margin then put a diamond in last column */

    if (newcol > ch_screen_width)
    {
        strl = ch_screen_width - columnumber - 1;				/* chop it off with room for diamond in last column */
        newcontent.ccodes[AI(linenumber,ch_screen_width-1)] = G1_DIAMOND;	/* code for a diamond */
        newcontent.cflags[AI(linenumber,ch_screen_width-1)] = f | CONTENT_CFLAG_G1SET; /* ... in G1 char set */
    }

    /* Output what's left (considering what was chopped off by 'shiftleft' and screen width) */

    if (strl > 0)
    {
        memcpy (newcontent.ccodes + AI (linenumber, columnumber), strp, strl);
        memset (newcontent.cflags + AI (linenumber, columnumber), f,    strl);
    }

    /* Save the new zero-based 'virtual' cursor position.  Lt ch_screen_numofs   */
    /* if still in 'shiftleft' area.  Gt ch_screen_width if we output a diamond. */

    columnumber = newcol;
}
