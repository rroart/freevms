//+++2004-06-10
//    Copyright (C) 2001,2002,2003  Mike Rieker, Beverly, MA USA
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
/*	change <range>							*/
/*									*/
/*  Enters change mode							*/
/*									*/
/************************************************************************/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "edt.h"

typedef struct Delbuff Delbuff;
struct Delbuff { Delbuff *next;
                 int direction;
                 char name[1];
               };

				/* set by these routines and is input to ch_screen_read: */
Line *ch_screen_top_line;	/* - pointer to what we want for top line on screen */
int ch_screen_shiftleft = 0;	/* - how many chars to leave off of beg of line */

				/* these are set by ch_screen_init: */
int ch_screen_num_lines;	/* - total number of lines on the screen */
int ch_screen_tmar_lines;	/* - number of lines to reserve for top margin */
int ch_screen_bmar_lines;	/* - number of lines to reserve for bottom margin */
int ch_screen_width;		/* - number of columns on screen, incl any used by 'set number show' */
int ch_screen_numofs;		/* - number of columns being used by 'set number show' (varies command to command) */

int autoshift;			/* zero : no autoshifting */
				/* else : number of 8's to shift by */

static const char worddelims[] = { 9, 10, 11, 12, 13, ' ' };

char *searchstr = "";		/* search string (set by " or ') */
uLong searchlen;

static int ch_exited;		/* set by 'EX' command to indicate exiting */
static int ch_defdir = 1;	/* 1: ADV mode; -1: BACK mode */

Position sel_position;		/* sel_position.buffer == NULL : no select range active */
				/* else, screen routines use this to highlight select range */

static int scroll_hint;		/* net scroll backward (<0) or forward (>0) done by command execution */
static Line *page_skip_line;	/* NULL: no page skip done, else: the line that had the <FF> */
static int vert_skip_coln = 0;	/* zero: not skipping vertically, up/down arrows will pick up column from where cursor is */
				/* else: screen column number (1..n) we started skipping vertically from, cursor will stay in this column */

static Delbuff *delbuffs = NULL; /* list of "DEL_<entity>" buffers and what direction they were deleted from */

static void check_top_line (void);
static void calc_top_line (void);
static int ch_process (const char *s);
static int getcount (const char **s_r);
static int decodekw (const char **s_r);
static int ch_cmd_adv (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_append (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_asc (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_back (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_chgcsr (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_chgc (int crpt, int cidx, int eidx, const char **s_r);
static int changecase (Position *bpos, Position *epos);
static int ch_cmd_clss (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_cut (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_d (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_defk (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_desel (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_ext (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_ex (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_help (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_i (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_nop (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_paste (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_ref (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_sel (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_shl (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_shr (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_und (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_repeat (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_buffer (int crpt, int cidx, int eidx, const char **s_r);
static int ch_cmd_move (int erpt, int eidx, const char **s_r);
static int skip_char (int erpt, Position *pos, const char **s_r);
static int skip_word (int erpt, Position *pos, const char **s_r);
static int skip_begl (int erpt, Position *pos, const char **s_r);
static int skip_endl (int erpt, Position *pos, const char **s_r);
static int skip_vert (int erpt, Position *pos, const char **s_r);
static int skip_page (int erpt, Position *pos, const char **s_r);
static int skip_find_prompt (int erpt, Position *pos, const char **s_r);
static int search_prompt (String *pmtstring, uLong pmtsize, const char *linebf, int *erpt_r);
static int skip_find (int erpt, Position *pos, const char **s_r);
static int skip_found (int erpt, Position *pos, const char **s_r);
static int skip_sel  (int erpt, Position *pos, const char **s_r);
static int skip_buff (int erpt, Position *pos, const char **s_r);
static void insertchar (char c);
static char *getpastebufname (const char **s_r);
static int cutitout (Position *bpos, Position *epos, const char *bufname);
static int pasteitin (int crpt, const char *bufname);
static void normalize (Line **line_r, uLong *offset_r);
static void normalise (Buffer *buffer, Line **line_r, uLong *offset_r);
static int matchstr (Line *line, uLong linesz, const char *linebf);
static void emptybuffer (const char *name);
static String *removeline (Line *line);
static void message (int extra, const char *format, ...);

void cmd_change (char *cp)

{
  Line *line, *xline;
  String *cmdstring;
  uLong columnumber;

  /* Get initial position */

  if (*cp != 0) {
    if ((range_single (cp, &cp, &cur_position) < 0) || !eoltest (cp)) return;
  }

  /* Change to screen mode so ch_screen_* can output escape sequences */

  output ();
  os_screenmode (1);

  /* Perform screen initialization */

  ch_screen_init ();				/* it fills in ch_screen_num_lines, _tmar_lines, _bmar_lines */

  /* Determine which line goes on the very top of screen */

  calc_top_line ();				/* this fills in ch_screen_top_line */

  /* Loop until we exit change mode */

  ch_exited = 0;				/* we haven't exited change mode yet */
  while (!ch_exited) {				/* repeat until we exit change mode */
    check_top_line ();				/* make sure ch_screen_top_line is ok */
    if ((autoshift != 0) && (cur_position.line != NULL)) {							/* see if auto-shift enabled */
      columnumber = ch_screen_chr2col (cur_position.offset, string_getval (line_string (cur_position.line)));	/* if so, see what column cursor is in */
      while (ch_screen_shiftleft > columnumber) {								/* see if it's shifted left off screen */
        ch_screen_shiftleft -= autoshift * 8;									/* if so, shift screen right */
        if (ch_screen_shiftleft < 0) ch_screen_shiftleft = 0;							/*        but not too far right */
      }
      while (ch_screen_shiftleft + ch_screen_width - ch_screen_numofs <= columnumber) ch_screen_shiftleft += autoshift * 8; /* shift left if off right end */
    }
    cmdstring = ch_screen_read ();		/* update screen if needed and read in a command or commands */
    if (cmdstring == NULL) break;		/* stop if hit eof on input */
    scroll_hint = 0;				/* reset the scrolling hint */
    ch_process (string_getval (cmdstring));	/* process command(s) */
    string_delete (cmdstring);			/* free off command string */
  }

  /* Terminate screen routines */

  ch_screen_term ();
  ch_screen_top_line = NULL;

  /* Return to non-screen mode */

  output ();
  os_screenmode (0);
}

/************************************************************************/
/*									*/
/*  This routine checks to make sure the current line is visible given 	*/
/*  the top line on the screen, and if not, it adjusts the top line.  	*/
/*  This is basically how we communicate to the terminal driver 	*/
/*  routines to scroll.							*/
/*									*/
/*    Input:								*/
/*									*/
/*	ch_screen_top_line = currently at top of screen			*/
/*	cur_position.line  = what line we have to make sure is visible	*/
/*	page_skip_line = line skipped to by page entity (or NULL)	*/
/*	scroll_hint > 0 : cur_position was moved forward		*/
/*	            < 0 : cur_position was moved backward		*/
/*									*/
/*    Output:								*/
/*									*/
/*	ch_screen_top_line = possibly modified				*/
/*									*/
/************************************************************************/

static void check_top_line (void)

{
  Line *line;
  uLong lineno;

  normalize (&cur_position.line, &cur_position.offset);

  /* If within top or mid-section of page line, use the page (if any) as the top */
  /* If this test fails, forget about the page skip line                         */

  lineno = ch_screen_num_lines - ch_screen_bmar_lines;
  for (line = page_skip_line; line != NULL; line = line_next (line)) {
    if (line == cur_position.line) {
      ch_screen_top_line = page_skip_line;
      return;
    }
    if (-- lineno == 0) break;
  }
  page_skip_line = NULL;

  /* If within existing top margin, scroll screen down so it is out of top margin */
  /* If within the existing middle section, just leave screen alone               */
  /* If within existing bottom margin, scroll screen up                           */

  line = ch_screen_top_line;						/* get line we had on top last time through */
  if (line != NULL) {
    for (lineno = 0; lineno < ch_screen_num_lines; lineno ++) {		/* count lines on the screen */
      if (line == cur_position.line) {					/* see if we found current line */

        /* If current line is in bottom margin, scroll screen up to get it out */
        /* But do not scroll up more than putting [EOB] at bottom of screen    */

        if (lineno >= ch_screen_num_lines - ch_screen_bmar_lines) {
          scroll_hint =  1;
          break;
        }

        /* If current line is in top margin, scroll screen down to get it out   */
        /* But do not scroll down more than putting first line at top of screen */

        if (lineno < ch_screen_tmar_lines) {
          scroll_hint = -1;
          break;
        }
        return;
      }
      if (line == NULL) break;						/* not current line, stop if hit [EOB] */
      line = line_next (line);						/* not [EOB], on to next line in buffer */
    }
  }

  /* Current line not on screen, use scrolling hint to find it.  If it says net backward, we scroll down so as */
  /* to put the current line on line just below top margin.  If it says net forward, we scroll up so as to put */
  /* the current line on line just above bottom margin, but we do it funny so if the [EOB] would show, it will */
  /* be on the very last line (and the cursor will be in the margin).                                          */

  ch_screen_top_line = cur_position.line;				/* start with current line on top of screen */
  lineno = ch_screen_tmar_lines;					/* assume it says net scroll backward */
  if (scroll_hint >= 0) {
    for (lineno = ch_screen_bmar_lines; lineno != 0; -- lineno) {	/* forward, scroll up bmar lines */
      if (ch_screen_top_line == NULL) break;				/* (but not past [EOB]) */
      ch_screen_top_line = line_next (ch_screen_top_line);
    }
    lineno = ch_screen_num_lines - 1;					/* ... then down num -1 lines */
  }
  for (; lineno != 0; -- lineno) {					/* scroll down lineno lines */
    if (ch_screen_top_line != NULL) line = line_prev (ch_screen_top_line);
    else line = buffer_last_line (cur_position.buffer);
    if (line == NULL) break;
    ch_screen_top_line = line;
  }
}

/************************************************************************/
/*									*/
/*  Calculate what line should be on the top of the screen when we 	*/
/*  start								*/
/*									*/
/*    Input:								*/
/*									*/
/*	cur_position = current position					*/
/*	ch_screen_num_lines = number of lines on screen			*/
/*									*/
/*    Output:								*/
/*									*/
/*	ch_screen_top_line = line that should be on top of screen	*/
/*									*/
/************************************************************************/

static void calc_top_line (void)

{
  Line *line;
  uLong lineno;

  page_skip_line = NULL;

  /* If we put the top line of the file on the top line of the screen, would the current line be in the 1st half of the screen? */

  lineno = 0;
  for (ch_screen_top_line = line = buffer_first_line (cur_position.buffer); line != NULL; line = line_next (line)) {
    if (line == cur_position.line) break;
    lineno ++;						/* count lines in file before current line */
    if (lineno > ch_screen_num_lines / 2) break;	/* but don't bother counting too far */
  }
  if (lineno <= ch_screen_num_lines / 2) return;

  /* If we put the [EOB] on the last line of the screen, would the current line be in the last half of the screen? */

  lineno = 1;
  for (line = buffer_last_line (cur_position.buffer); line != NULL; line = line_prev (line)) {
    if (line == cur_position.line) break;
    lineno ++;						/* count lines in file after current line */
    if (lineno > ch_screen_num_lines / 2) break;	/* but don't bother counting too far */
    ch_screen_top_line = line;				/* (assume this line will end up on top of screen though probably not) */
  }
  if (lineno <= ch_screen_num_lines / 2) {
    while (lineno < ch_screen_num_lines) {		/* it fits that way, see what line would be on top of screen */
      line = line_prev (ch_screen_top_line);
      if (line == NULL) break;
      lineno ++;
      ch_screen_top_line = line;
    }
    return;
  }

  /* Ok, just put current line in middle of screen */

  lineno = 0;
  for (line = cur_position.line; (line != NULL) && (line != buffer_first_line (cur_position.buffer)); line = line_prev (line)) {
    if (++ lineno >= ch_screen_num_lines / 2) break;
  }
  ch_screen_top_line = line;
}

/* Keyword table */

#define KW_FLAG_NEEDSENT 0x80000000
#define KW_FLAG_ISENTITY 0x40000000

static const struct { int nlen;
                      const char *name;
                      unsigned int flags;
                      int (*entry) ();
                    } kwtabl[] = {
                      1, "w",     KW_FLAG_ISENTITY, skip_word, 
                      1, "v",     KW_FLAG_ISENTITY, skip_vert, 
                      3, "und",   KW_FLAG_NEEDSENT, ch_cmd_und, 
                      3, "ref",                  0, ch_cmd_ref, 
                      1, "r",     KW_FLAG_ISENTITY, skip_buff, 
                      2, "sr",    KW_FLAG_ISENTITY, skip_sel, 
                      3, "shl",                  0, ch_cmd_shl, 
                      3, "shr",                  0, ch_cmd_shr, 
                      3, "sel",                  0, ch_cmd_sel, 
                      5, "paste",                0, ch_cmd_paste, 
                      4, "page",  KW_FLAG_ISENTITY, skip_page, 
                      1, "l",     KW_FLAG_ISENTITY, skip_begl, 
                      1, "i",                    0, ch_cmd_i, 
                      4, "help",                 0, ch_cmd_help, 
                      2, "fs",    KW_FLAG_ISENTITY, skip_found, 
                      3, "ext",                  0, ch_cmd_ext, 
                      2, "ex",                   0, ch_cmd_ex, 
                      2, "el",    KW_FLAG_ISENTITY, skip_endl, 
                      5, "desel",                0, ch_cmd_desel, 
                      4, "defk",                 0, ch_cmd_defk, 
                      1, "d",     KW_FLAG_NEEDSENT, ch_cmd_d, 
                      3, "cut",   KW_FLAG_NEEDSENT, ch_cmd_cut, 
                      4, "clss",                 0, ch_cmd_clss, 
                      6, "chgcsr",               0, ch_cmd_chgcsr, 
                      4, "chgc",  KW_FLAG_NEEDSENT, ch_cmd_chgc, 
                      1, "c",     KW_FLAG_ISENTITY, skip_char, 
                      4, "back",                 0, ch_cmd_back, 
                      6, "append", KW_FLAG_NEEDSENT, ch_cmd_append, 
                      3, "asc",                  0, ch_cmd_asc, 
                      3, "adv",                  0, ch_cmd_adv, 
                      1, "?",     KW_FLAG_ISENTITY, skip_find_prompt, 
                      1, "\"",    KW_FLAG_ISENTITY, skip_find, 
                      1, "'",     KW_FLAG_ISENTITY, skip_find, 
                      1, "(",                    0, ch_cmd_repeat, 
                      1, "=",                    0, ch_cmd_buffer, 
                      1, ".",                    0, ch_cmd_nop, 
                      0, NULL, 0, NULL };

/* Process command string */

static int ch_process (const char *s)

{
  char c;
  int i, j, n;

  while ((c = *s) != 0) {				/* repeat while there's stuff to do */
    if (c <= ' ') {					/* skip any leading spaces */
      s ++;
      continue;
    }
    n = getcount (&s);					/* process any leading +/-count */
    i = decodekw (&s);					/* decode keyword, it may be a command or an entity */
    if (i < 0) return (0);				/* stop if unknown */
    if (kwtabl[i].flags & KW_FLAG_ISENTITY) {		/* see if it is an entity */
      if (!ch_cmd_move (n, i, &s)) return (0);		/* perform move */
      continue;
    }
    j = -1;						/* command, assume no entity index */
    if (kwtabl[i].flags & KW_FLAG_NEEDSENT) {		/* see if the command requires an entity */
      j = decodekw (&s);				/* decode entity keyword */
      if (j < 0) return (0);				/* stop if unknown */
      if (!(kwtabl[j].flags & KW_FLAG_ISENTITY)) {	/* error if entity missing */
        message (strlen (kwtabl[i].name), "subcommand %s requires entity", kwtabl[i].name);
        return (0);
      }
    }
    if (!(*(kwtabl[i].entry)) (n, i, j, &s)) return (0); /* execute command, stop if error */
  }

  return (1);
}

/* Get optional repeat count from string - if none present, return 1 */

static int getcount (const char **s_r)

{
  char c;
  const char *s;
  int accum, digits, hadsign;

  s = *s_r;

  accum   = 0;
  digits  = 0;
  hadsign = 0;

  while (((c = *s) != 0) && (c <= ' ')) s ++;
  if (c == '+') hadsign =  1;
  if (c == '-') hadsign = -1;
  if (hadsign != 0) s ++;
  else hadsign = ch_defdir;

  while (((c = *s) >= '0') && (c <= '9')) {
    accum  = accum * 10 + c - '0';
    digits = 1;
    s ++;
  }

  if (!digits) accum = 1;
  if (hadsign < 0) accum = -accum;

  *s_r = s;
  return (accum);
}

/* Decode keyword from table - return -1 if not found, else index in table */

static int decodekw (const char **s_r)

{
  char c, d;
  const char *s, *t;
  int i;

  s = *s_r;

  while (((c = *s) != 0) && (c <= ' ')) s ++;

  for (i = 0; kwtabl[i].name != NULL; i ++) {			/* scan through the table */
    if (strncasecmp (s, kwtabl[i].name, kwtabl[i].nlen) == 0) {	/* see if we found a match */
      s += kwtabl[i].nlen;					/* ok, skip over the keyword */
      *s_r = s;							/* update pointer to next item in line */
      return (i);						/* return index of table entry that was found */
    }
  }
  message (strlen (s), "unknown subcommand or entity %s", s);
  return (-1);							/* couldn't find it */
}

/************************************************************************/
/*									*/
/*  Command processing routines						*/
/*									*/
/*    Input:								*/
/*									*/
/*	crpt = command repeat count					*/
/*	cidx = command kwtabl index					*/
/*	erpt = entity repeat count					*/
/*	eidx = entity kwtabl index					*/
/*	*s_r = command line pointer					*/
/*									*/
/*    Output:								*/
/*									*/
/*	ch_cmd_* = 0 : command failed					*/
/*	           1 : success						*/
/*	*s_r = possibly updated						*/
/*	buffer updated							*/
/*									*/
/************************************************************************/

/************************************/
/* Set default direction to forward */
/************************************/

static int ch_cmd_adv (int crpt, int cidx, int eidx, const char **s_r)

{
  ch_defdir = 1;
  return (1);
}

/*************************************************************************************/
/* Append the entity - just like CUT except removed data is appended to PASTE buffer */
/*************************************************************************************/

static int ch_cmd_append (int crpt, int cidx, int eidx, const char **s_r)

{
  char *bufname;
  int rc;
  Position bpos, epos;

  bufname = getpastebufname (s_r);				/* get PASTE or buffer name from command string */
  bpos = epos = cur_position;					/* start at current position */
  rc = (*(kwtabl[eidx].entry)) (crpt, &epos, s_r);		/* move end position to entity */
  if (rc && ((bpos.buffer == NULL) || (epos.buffer == NULL))) rc = 0;
  if (rc) rc = cutitout (&bpos, &epos, bufname);		/* cut out the range of stuff */
  if (rc) cur_position = bpos;					/* position to the cut */
  free (bufname);
  return (rc);
}

/*************************************************/
/* Insert ascii char represented by repeat count */
/*************************************************/

static int ch_cmd_asc (int crpt, int cidx, int eidx, const char **s_r)

{
  if (crpt < 0) crpt = - crpt;	/* in case we're set to BACK mode */
  insertchar (crpt);		/* insert the character */
  return (1);			/* always successful */
}

/*************************************/
/* Set default direction to backward */
/*************************************/

static int ch_cmd_back (int crpt, int cidx, int eidx, const char **s_r)

{
  ch_defdir = -1;
  return (1);
}

/**********************/
/* Change entity case */
/**********************/

/* Special processing for select range */

static int ch_cmd_chgcsr (int crpt, int cidx, int eidx, const char **s_r)

{
  char c, *linebf;
  Position bpos, epos, *xpos;
  uLong linesz;

  bpos = epos = cur_position;							/* start at current position */

  /* If select position defined in current buffer, use that */

  if (sel_position.buffer == cur_position.buffer) {
    epos = sel_position;
    sel_position.buffer = NULL;
  }

  /* Otherwise, if we're on the search string, use that */

  else if (epos.line != NULL) {
    if ((searchlen != 0) 
        && matchstr (epos.line, 
                     string_getlen (line_string (epos.line)) - epos.offset, 
                     string_getval (line_string (epos.line)) + epos.offset)) {
      epos.offset += searchlen;
      normalize (&epos.line, &epos.offset);
    }

  /* Otherwise, just use the number of characters given in the repeat count */

    else {
      xpos = &epos;					/* if skip forward, modify end position */
      if (crpt < 0) xpos = &bpos;			/* otherwise, mod begin position */
      if (!skip_char (crpt, xpos, s_r)) return (0);
      cur_position = *xpos;				/* put cursor at skip point */
    }
  }

  /* Change the character's case */

  return (changecase (&bpos, &epos));
}

/* All other entities are normal */

static int ch_cmd_chgc (int crpt, int cidx, int eidx, const char **s_r)

{
  char c, *linebf;
  Position bpos, epos;
  uLong linesz;

  bpos = epos = cur_position;							/* start at current position */

  if (!(*(kwtabl[eidx].entry)) (crpt, &epos, s_r)) return (0);			/* move end position to entity */
  return (changecase (&bpos, &epos));
}

static int changecase (Position *bpos, Position *epos)

{
  char c, *linebf;
  int rp;
  Position xpos;
  uLong linesz;

  rp = relposition (bpos, epos);						/* make sure bpos is before epos */
  if (rp == 0) return (1);
  if (rp < 0) {			
     xpos = *bpos;
    *bpos = *epos;
    *epos =  xpos;
  }

  linesz = string_getlen (line_string (bpos -> line)) - bpos -> offset;		/* get size of beginning line */
  linebf = (char *)string_getval (line_string (bpos -> line)) + bpos -> offset;	/* get address of beginning line */
  buffer_dirty (epos -> buffer, 1);						/* assume we will dirty it */
  while ((bpos -> line != epos -> line) || (bpos -> offset < epos -> offset)) { /* repeat until we are all done */
    if (linesz == 0) {								/* maybe we hit end of this line */
      bpos -> line   = line_next (bpos -> line);				/* if so, go on to next line */
      bpos -> offset = 0;
      if (bpos -> line != NULL) {
        linesz = string_getlen (line_string (bpos -> line));			/* get its size */
        linebf = (char *)string_getval (line_string (bpos -> line));		/* and its address */
      }
    } else {
      c = *linebf;								/* get a character */
      if ((c >= 'A') && (c <= 'Z')) *linebf = c - 'A' + 'a';			/* change upper to lower case */
      if ((c >= 'a') && (c <= 'z')) *linebf = c - 'a' + 'A';			/* change lower to upper case */
      -- linesz;								/* one less character on line to process */
      linebf ++;								/* point to next character on line */
      bpos -> offset ++;							/* increment offset in line */
    }
  }

  return (1);
}

/******************************/
/* Clear search string buffer */
/******************************/

static int ch_cmd_clss (int crpt, int cidx, int eidx, const char **s_r)

{
  if (searchstr[0] != 0) {	/* see if anything in it now */
    free (searchstr);		/* ok, free it off */
    searchlen = 0;		/* it now has the null string */
    searchstr = "";
  }
  return (1);			/* successful */
}

/****************************************************************************/
/* Cut the entity - just like Delete except the PASTE buffer is always used */
/****************************************************************************/

static int ch_cmd_cut (int crpt, int cidx, int eidx, const char **s_r)

{
  char *bufname;
  int rc;
  Position bpos, epos;

  bufname = getpastebufname (s_r);				/* get PASTE or buffer name from command string */
  bpos = epos = cur_position;					/* start at current position */
  rc = (*(kwtabl[eidx].entry)) (crpt, &epos, s_r);		/* move end position to entity */
  if (rc && ((bpos.buffer == NULL) || (epos.buffer == NULL))) rc = 0;
  if (rc) emptybuffer (bufname);				/* clear out any old paste buffer contents */
  if (rc) rc = cutitout (&bpos, &epos, bufname);		/* cut out the range of stuff */
  if (rc) cur_position = bpos;					/* position to the cut */
  free (bufname);
  return (rc);
}

/*****************************************************************************/
/* Delete the entity - just like CUT except the buffer depends on the entity */
/*****************************************************************************/

static int ch_cmd_d (int crpt, int cidx, int eidx, const char **s_r)

{
  char bufname[16];
  const char *p;
  Delbuff *delbuff;
  Position bpos, epos;

  strcpy (bufname, "DEL_");					/* this is where deleted data will go */
  p = kwtabl[eidx].name;
  if (strcmp (p, "el") == 0) p ++;
  strcat (bufname, p);

  bpos = epos = cur_position;					/* start at current position */
  if (!(*(kwtabl[eidx].entry)) (crpt, &epos, s_r)) return (0);	/* move end position to entity */
  emptybuffer (bufname);					/* empty out what's in there now */

  for (delbuff = delbuffs; delbuff != NULL; delbuff = delbuff -> next) {
    if (strcmp (delbuff -> name, bufname) == 0) break;
  }
  if (delbuff == NULL) {
    delbuff = malloc (strlen (bufname) + sizeof *delbuff);
    delbuff -> next = delbuffs;
    strcpy (delbuff -> name, bufname);
    delbuffs = delbuff;
  }
  delbuff -> direction = crpt;					/* save direction stuff is deleted from */

  if (!cutitout (&bpos, &epos, bufname)) return (0);		/* cut out the range of stuff */
  cur_position = epos;						/* position to the cut */
  return (1);
}

/**************/
/* Define key */
/**************/

static int ch_cmd_defk (int crpt, int cidx, int eidx, const char **s_r)

{
  char c, keyname[16], *p;
  const char *cmdval;
  int rc;
  String *cmdstring, *keystring, *pmtstring;
  uLong cmdlen, defofs, l;

  /* Get key to be defined */

  cmdstring = string_create (0, NULL);
  keystring = string_create (0, NULL);
  pmtstring = string_create (37, "Key to define (GOLD-DEL to cancel): ");
  ch_screen_prompt (pmtstring);						/* display prompt */
  do if (!jnl_readkeyseq (keystring)) goto abandon;			/* read a keystroke */
  while ((rc = keypad_getname (string_getval (keystring), keyname)) == 0); /* convert to key name */
  if (rc < 0) goto abandon;						/* abandon if unknown key */

  /* Now read the key's definition */

  string_setval (pmtstring,  8, "Define '");
  string_concat (pmtstring, strlen (keyname), keyname);			/* prompt has the keyname at the beginning */
  string_concat (pmtstring, 25, "' (GOLD-DEL to cancel): ");		/* followed by a friendly reminder */
  defofs = string_getlen (pmtstring);					/* save offset where definition starts */
  cmdval = keypad_getdef (keyname);					/* get current definition */
  if (cmdval == NULL) goto abandon;					/* they are trying to define GOLD-DEL */
  string_concat (pmtstring, strlen (cmdval), cmdval);			/* followed by current definition */
  while (1) {

    /* Display prompt = 'keyname: definition so far' */

    ch_screen_prompt (pmtstring);

    /* Read keypad key */

    string_setval (keystring, 0, NULL);
    string_setval (cmdstring, 0, NULL);
    do if (!jnl_readkeyseq (keystring)) goto abandon;
    while (!keypad_decode (keystring, cmdstring));

    /* If resulting command length is zero, they pressed GOLD-DEL to abandon command */

    cmdlen = string_getlen (cmdstring);
    cmdval = string_getval (cmdstring);
    if (cmdlen == 0) goto abandon;

    /* Process the code(s) they entered */

    while (cmdlen != 0) {
      if (cmdval[0] == '.') goto defineit;				/* dot (ie, KP-Enter), done */
      if ((cmdval[0] == 'i') || (cmdval[0] == 'I')) {			/* I<chars>^Z, add chars to definition */
        cmdlen --;
        cmdval ++;
        while ((cmdlen != 0) && (*cmdval != 26) && (*cmdval != 4)) {
          cmdlen --;
          string_concat (pmtstring, 1, cmdval ++);
        }
        if (cmdlen != 0) cmdlen --;
        continue;
      }
      if (strncasecmp (cmdval, "EX", 2) == 0) {				/* EX, means they pressed ^Z, so enter it */
        string_concat (pmtstring, 1, "");
        cmdlen -= 2;
        cmdval += 2;
        continue;
      }
      if (strncasecmp (cmdval, "(-DC)", 5) == 0) {			/* (-DC), means they pressed DEL key, */
        l = string_getlen (pmtstring);					/* so wipe a character from definitio */
        if (l > defofs) string_remove (pmtstring, 1, l - 1);
        cmdlen -= 5;
        cmdval += 5;
        continue;
      }
      if (cmdval[0] == '(') {						/* check for (nnnASC) */
        c = strtol (cmdval + 1, &p, 10);
        if (strncasecmp (p, "ASC)", 4) == 0) {
          string_concat (pmtstring, 1, &c);				/* if so, insert corresponding character */
          p += 4;
          cmdlen -= (const char *)p - cmdval;
          cmdval  = (const char *)p;
          continue;
        }
      }
      string_concat (pmtstring, cmdlen, cmdval);			/* all others get copied as is */
      break;
    }
  }

  /* Store key's new definition */

defineit:
  cmdval = string_getval (pmtstring) + defofs;				/* point to new command string */
  keypad_setdef (keyname, cmdval);					/* store the definition */
  rc = 1;
  goto cleanup;

abandon:
  rc = 0;

cleanup:
  string_setval (pmtstring, 1, "");					/* get prompt off screen */
  ch_screen_prompt (pmtstring);
  string_delete (cmdstring);						/* delete temp strings */
  string_delete (keystring);
  string_delete (pmtstring);
  return (rc);
}

/***************************/
/* Deactivate select range */
/***************************/

static int ch_cmd_desel (int crpt, int cidx, int eidx, const char **s_r)

{
  sel_position.buffer = NULL;
  return (1);
}

/**************************/
/* Exit back to line mode */
/**************************/

static int ch_cmd_ex (int crpt, int cidx, int eidx, const char **s_r)

{
  ch_exited = 1;
  return (1);
}

/****************************************/
/* Perform external (line mode) command */
/****************************************/

static int ch_cmd_ext (int crpt, int cidx, int eidx, const char **s_r)

{
  char c, d, *p;
  const char *linebf, *s;
  int done;
  Line *line;
  String *cmdstring, *keystring, *pmtstring;
  uLong i, linesz, pmtsize;

  pmtstring = NULL;

  s = *s_r;
  c = *(s ++);
  if (c != '?') goto noprompt;

  /* Prompt for the command to be executed */

  c = *(s ++);						/* get the " or ' that starts the prompt */
  if ((c != '\'') && (c != '"')) {
    message (0, "external ? must be followed by '<prompt>' or \"<prompt>\"");
    return (0);
  }
  while (((d = *s) != 0) && (d != c)) s ++;		/* scan to end of prompt string */
  pmtstring = string_create (s - *s_r - 2, *s_r + 2);	/* create prompt string for screen */
  string_concat (pmtstring, 1, "");			/* append a null to end prompt portion */
  pmtsize   = string_getlen (pmtstring);		/* get prompt size including the null */
  keystring = string_create (0, NULL);
  cmdstring = string_create (0, NULL);
  do {
    ch_screen_prompt (pmtstring);			/* display it on screen */
    if (!jnl_readkeyseq (keystring)) goto abandon;	/* read key sequence from keyboard */
    done = keypad_decode (keystring, cmdstring);	/* convert key sequence to changemode commands */
    if (!done) continue;				/* repeat if partial escape sequence received */
    string_remove (pmtstring, string_getlen (pmtstring) - pmtsize, pmtsize); /* reset the prompt string to original */
    linebf = string_getval (cmdstring);			/* point to changemode commands */
    while (*linebf != 0) {				/* process them */
      if (*linebf <= ' ') {
        linebf ++;
        continue;
      }
      if (linebf[0] == '.') {				/* . - terminates string, uses current direction */
        done = 1;
        break;
      }
      if (strncasecmp (linebf, "(-DC)", 5) == 0) {	/* (-DC) - delete last search character */
        i = string_getlen (pmtstring);
        if (i > pmtsize) string_remove (pmtstring, 1, -- i);
        linebf += 5;
        done = 0;
        continue;
      }
      if (strncasecmp (linebf, "(-DL)", 5) == 0) {	/* (-DL) - abandon operation */
        string_setval (pmtstring, 2, "\000");
        ch_screen_prompt (pmtstring);
        goto abandon;
      }
      if ((linebf[0] == 'I') || (linebf[0] == 'i')) {	/* I - supplies characters for the command */
        while (((c = *(++ linebf)) != 0) && (c != 26)) {
          string_concat (pmtstring, 1, linebf);
        }
        if (c != 0) linebf ++;
        done = 0;
        continue;
      }
      if (linebf[0] == '(') {				/* (nnnASC) - supplies a control char for the command */
        c = strtol (linebf + 1, &p, 10);
        while ((*p != 0) && (*p <= ' ')) p ++;
        if (strncasecmp (p, "ASC)", 4) == 0) {
          string_concat (pmtstring, 1, &c);
          linebf = p + 4;
          done = 0;
          continue;
        }
      }
      message (strlen (linebf), "invalid command terminator %s, press Enter to terminate", linebf);
      goto abandon;
    }
  } while (!done);

  linebf = string_getval (pmtstring) + pmtsize;
  goto executeit;

  /* Not prompt mode, string given as 'command' or "command" */

noprompt:
  if ((c != '\'') && (c != '"')) {
    message (0, "external must be followed by '<command>' or \"<command>\" or ?<prompt>");
    return (0);
  }
  while (((d = *s) != 0) && (d != c)) s ++;		/* scan to end of command string */
  pmtstring = string_create (s - *s_r - 1, *s_r + 1);	/* store in a string so we get null terminator */
  linebf = string_getval (pmtstring);

  /* Execute command pointed to by linebf */

executeit:
  message (0, "");					/* newline after the prompt string */
  message (0, "");					/* force a "Press RETURN to continue" prompt */
  output ();						/* turn off screen mode */
  os_screenmode (0);
  done = ln_command (linebf);				/* execute command */
  output ();						/* turn screen mode back on */
  os_screenmode (1);
  string_delete (pmtstring);				/* free off prompt/command string */
  if (d != 0) s ++;					/* skip over terminating " or ' */
  *s_r = s;
  return (done);

  /* Some error - abandon command */

abandon:
  string_delete (pmtstring);
  string_delete (keystring);
  string_delete (cmdstring);
  return (0);
}

/*********************/
/* Display help info */
/*********************/

static int ch_cmd_help (int crpt, int cidx, int eidx, const char **s_r)

{
  char command[64], keyname[16];
  int rc;
  String *keystring;

  strcpy (command, "-nomoreinfo KEYPAD VT100");
  keystring = string_create (0, NULL);

  while (1) {
    output ();						/* turn off screen mode */
    os_screenmode (0);
    cmd_help (command);					/* display help message */
    output ();						/* turn screen mode back on */
    os_screenmode (1);

    string_setval (keystring, 0, NULL);
    do if (!jnl_readkeyseq (keystring)) goto abandon;	/* read a keystroke */
    while ((rc = keypad_getname (string_getval (keystring), keyname)) == 0); /* convert to key name */
    if (rc < 0) goto abandon;				/* abandon if unknown key */

    strcpy (command, "-nomoreinfo KEYPAD VT100 ");
    strcat (command, keyname);
  }

abandon:
  string_delete (keystring);
  ch_screen_refresh ();
  return (1);
}

/***************/
/* Insert text */
/***************/

static int ch_cmd_i (int crpt, int cidx, int eidx, const char **s_r)

{
  char c;
  const char *s;

  if (crpt < 0) crpt = - crpt;

  do {
    s = *s_r;
    while ((c = *s) != 0) {
      s ++;
      if ((c == 4) || (c == 26)) break;
      insertchar (c);
    }
  } while (-- crpt > 0);

  *s_r = s;

  return (1);
}

/*********/
/* No-op */
/*********/

static int ch_cmd_nop (int crpt, int cidx, int eidx, const char **s_r)

{
  return (1);
}

/**********************************/
/* Paste the paste buffer back in */
/**********************************/

static int ch_cmd_paste (int crpt, int cidx, int eidx, const char **s_r)

{
  return (pasteitin (crpt, "PASTE"));
}

/******************/
/* Refresh screen */
/******************/

static int ch_cmd_ref (int crpt, int cidx, int eidx, const char **s_r)

{
  ch_screen_refresh ();
  return (1);
}

/************************************/
/* Start select at current position */
/************************************/

static int ch_cmd_sel (int crpt, int cidx, int eidx, const char **s_r)

{
  if (sel_position.buffer != NULL) {
    message (0, "select range already active");
    return (0);
  }
  sel_position = cur_position;
  return (1);
}

/****************************/
/* Shift screen to the left */
/****************************/

static int ch_cmd_shl (int crpt, int cidx, int eidx, const char **s_r)

{
  ch_screen_shiftleft += crpt * 8;
  if (ch_screen_shiftleft < 0) ch_screen_shiftleft = 0;
  return (1);
}

/*****************************/
/* Shift screen to the right */
/*****************************/

static int ch_cmd_shr (int crpt, int cidx, int eidx, const char **s_r)

{
  ch_screen_shiftleft -= crpt * 8;
  if (ch_screen_shiftleft < 0) ch_screen_shiftleft = 0;
  return (1);
}

/***********************/
/* Undelete the entity */
/***********************/

static int ch_cmd_und (int crpt, int cidx, int eidx, const char **s_r)

{
  char bufname[16];
  const char *p;
  Delbuff *delbuff;
  Position original;

  strcpy (bufname, "DEL_");					/* make DEL_<entity> buffer name */
  p = kwtabl[eidx].name;
  if (strcmp (p, "el") == 0) p ++;
  strcat (bufname, p);

  original = cur_position;					/* save current position at beg of insertion */
  if (!pasteitin (crpt, bufname)) return (0);			/* copy in the DEL_<entity> buffer */

  for (delbuff = delbuffs; delbuff != NULL; delbuff = delbuff -> next) {
    if (strcmp (delbuff -> name, bufname) == 0) {
      if (delbuff -> direction > 0) cur_position = original;	/* if delete was in forward direction, position to beg of insert */
      break;
    }
  }

  return (1);
}

/********************************/
/* Repeat commands between ()'s */
/********************************/

static int ch_cmd_repeat (int crpt, int cidx, int eidx, const char **s_r)

{
  char *p;
  int rc;

  if (crpt < 0) crpt = - crpt;				/* (in case we're set to BACK mode) */
  p = strchr (*s_r, ')');				/* find terminating ) */
  if (p != NULL) *p = 0;				/* if one present, temporarily replace with a null */
  rc = 1;
  while (rc && (-- crpt >= 0)) rc = ch_process (*s_r);	/* repeat command until error or repeat count runs out */
  if (p != NULL) *(p ++) = ')';				/* restore terminating ) */
  if (rc) *s_r = (const char *)p;			/* update pointer */
  return (rc);						/* return status */
}

/******************************/
/* Change to buffer =<buffer> */
/******************************/

static int ch_cmd_buffer (int crpt, int cidx, int eidx, const char **s_r)

{
  char c;
  const char *p, *s;

  p = s = *s_r;							/* point just past the = in command string */
  while (((c = *p) != 0) && (strchr (bufnamechars, c) != NULL)) p ++; /* find the end of valid buffer name chars */
  *buffer_savpos (cur_position.buffer) = cur_position;		/* save old current position */
  cur_position = *buffer_savpos (buffer_create (p - s, s));	/* set up new current position */
  ch_screen_top_line = NULL;					/* forget all about what's on screen now */
  *s_r = p;							/* return pointer to terminating character */
  return (1);  							/* all done */
}

/************************************************/
/* Move current position to beginning of entity */
/************************************************/

static int ch_cmd_move (int erpt, int eidx, const char **s_r)

{
  Position pos;

  pos = cur_position;						/* start at current position */
  if (!(*(kwtabl[eidx].entry)) (erpt, &pos, s_r)) return (0);	/* get position of specified entity */
  cur_position = pos;						/* set current position to specified entity */
  if (kwtabl[eidx].entry == skip_page) {			/* see if just skipped to page marker */
    page_skip_line = cur_position.line;				/* ok, put the <FF> at the top of screen */
  }
  return (1);
}

/************************************************************************/
/*									*/
/*  Skip position forward or backward the number of entities		*/
/*									*/
/*    Input:								*/
/*									*/
/*	erpt = number of entities to skip				*/
/*	*pos = starting position					*/
/*									*/
/*    Output:								*/
/*									*/
/*	skip_* = 0 : failed (beg/end of buf)				*/
/*	         1 : success						*/
/*	*pos = new position (normalised/normalized)			*/
/*									*/
/************************************************************************/

/* Skip forward/backward the number of characters */

static int skip_char (int erpt, Position *pos, const char **s_r)

{
  Line *line;
  uLong linesz, offset;

  vert_skip_coln = 0;

  line   = pos -> line;
  offset = pos -> offset;

  /* Move backwards */

  while (erpt < 0) {
    erpt ++;
    if (offset == 0) {
      normalise (pos -> buffer, &line, &offset);
      if (offset == 0) {
        message (0, "backup before beginning of buffer");
        return (0);
      }
      scroll_hint --;
    }
    offset --;
  }

  /* Move forwards */

  linesz = 0;
  if (line != NULL) linesz = string_getlen (line_string (line));
  while (erpt > 0) {
    erpt --;
    if (line == NULL) {
      message (0, "advance beyond end of buffer");
      return (0);
    }
    offset ++;
    if (offset >= linesz) {
      normalize (&line, &offset);
      linesz = 0;
      if (line != NULL) linesz = string_getlen (line_string (line));
      scroll_hint ++;
    }
  }

  /* All done moving */

  pos -> line   = line;
  pos -> offset = offset;

  return (1);
}

/* Skip forward/backward the number of words */
/* The position is always left at the beginning of a word */

static int skip_word (int erpt, Position *pos, const char **s_r)

{
  const char *linebf;
  int skipit;
  Line *line;
  uLong linesz, offset;

  vert_skip_coln = 0;

  line   = pos -> line;
  offset = pos -> offset;
  linesz = 0;
  linebf = NULL;
  if (line != NULL) {
    linesz = string_getlen (line_string (line));
    linebf = string_getval (line_string (line));
  }

  /* Move backward */

  while (erpt < 0) {
    erpt ++;
    do {
      if (offset == 0) {
        normalise (pos -> buffer, &line, &offset);
        if (offset == 0) {
          message (0, "backup before beginning of buffer");
          return (0);
        }
        offset = string_getlen (line_string (line));
        linebf = string_getval (line_string (line));
        scroll_hint --;
      }
      offset --;
    } while ((offset > 0) && ((linebf[offset] == ' ') || (memchr (worddelims, linebf[offset-1], sizeof worddelims) == NULL)));
  }

  /* Move forward */

  while (erpt > 0) {
    erpt --;
    while (1) {
      offset ++;
      if (offset >= linesz) {
        if (line == NULL) {
          message (0, "advance beyond end of buffer");
          return (0);
        }
        normalize (&line, &offset);
        linesz = 0;
        linebf = NULL;
        if (line != NULL) {
          linesz = string_getlen (line_string (line));
          linebf = string_getval (line_string (line));
        }
        scroll_hint ++;
      }
      if (offset == 0) break;								/* stop at beginning of line */
      if (linebf[offset] == ' ') continue;						/* skip over spaces */
      if (memchr (worddelims, linebf[offset], sizeof worddelims) != NULL) break;	/* stop if other delimeter */
      if (memchr (worddelims, linebf[offset-1], sizeof worddelims) != NULL) break;	/* stop on printable if preceded by delim */
    }
  }

  /* All done moving */

  pos -> line   = line;
  pos -> offset = offset;

  return (1);
}

/* Skip forward/backward the number of lines */
/* The position is always left at the beginning of a line */

static int skip_begl (int erpt, Position *pos, const char **s_r)

{
  const char *linebf;
  Line *line;
  uLong offset;

  vert_skip_coln = 0;

  line   = pos -> line;
  offset = pos -> offset;
  linebf = NULL;
  if (line != NULL) linebf = string_getval (line_string (line));

  /* Move backward */

  while (erpt < 0) {
    erpt ++;
    if (offset == 0) {
      normalise (pos -> buffer, &line, &offset);
      if (offset == 0) {
        message (0, "backup before beginning of buffer");
        return (0);
      }
      scroll_hint --;
    }
    offset = 0;
  }

  /* Move forward */

  while (erpt > 0) {
    erpt --;
    if (line == NULL) {
      message (0, "advance beyond end of buffer");
      return (0);
    }
    offset = 0;
    line   = line_next (line);
    normalize (&line, &offset);
    scroll_hint ++;
  }

  /* All done moving */

  pos -> line   = line;
  pos -> offset = offset;

  return (1);
}

/* Skip forward/backward the number of lines */
/* The position is always left at the end of a line (pointing at the \n) */

static int skip_endl (int erpt, Position *pos, const char **s_r)

{
  const char *linebf;
  Line *line;
  uLong offset;

  vert_skip_coln = 0;

  line   = pos -> line;
  offset = pos -> offset;
  linebf = NULL;
  if (line != NULL) linebf = string_getval (line_string (line));

  /* Move backward */

  while (erpt < 0) {
    erpt ++;
    offset = 0;
    normalise (pos -> buffer, &line, &offset);
    if (offset == 0) {
      message (0, "backup before beginning of buffer");
      return (0);
    }
    scroll_hint --;
    offset --;
  }

  /* Move forward */

  while (erpt > 0) {
    erpt --;
    if (line == NULL) {
      message (0, "advance beyond end of buffer");
      return (0);
    }
    offset ++;
    normalize (&line, &offset);
    scroll_hint ++;
    if (line != NULL) offset = string_getlen (line_string (line)) - 1;
  }

  /* All done moving */

  pos -> line   = line;
  pos -> offset = offset;

  return (1);
}

/* Skip forward/backward the number of lines vertically */
/* The position is always left at the same place on the new line */

static int skip_vert (int erpt, Position *pos, const char **s_r)

{
  Line *line;
  uLong linesz;

  line = pos -> line;

  /* Try to keep in same spot visually on line as when they first started with the vertical skips     */
  /* So here we save the screen column number they are at (if this is the first of a series of skips) */

  if ((vert_skip_coln == 0) && (pos -> line != NULL)) {
    vert_skip_coln = ch_screen_chr2col (pos -> offset, string_getval (line_string (pos -> line))) + 1;
  }

  /* Move backward */

  while (erpt < 0) {
    erpt ++;
    if (line != NULL) line = line_prev (line);
    else line = buffer_last_line (pos -> buffer);
    if (line == NULL) {
      message (0, "backup before beginning of buffer");
      return (0);
    }
    scroll_hint --;
  }

  /* Move forward */

  while (erpt > 0) {
    erpt --;
    if (line == NULL) {
      message (0, "advance beyond end of buffer");
      return (0);
    }
    line = line_next (line);
    scroll_hint ++;
  }

  /* All done moving up or down - set offset to be as near to starting physical screen column as possible */

  pos -> line = line;

  if (line == NULL) pos -> offset = 0;				/* if [EOB], position to beginning of the [EOB] */
  else {
    linesz = string_getlen (line_string (line));		/* otherwise, get the length of the line */
    if (linesz > 0) linesz --;					/* (don't include the last character) */
    pos -> offset = ch_screen_col2chr (linesz, string_getval (line_string (line)), vert_skip_coln - 1);
  }
  normalize (&(pos -> line), &(pos -> offset));

  return (1);
}

/* Skip forward/backward the number of pages */
/* The position is always left at the beginning of a page (just after the <FF>), or at beginning of file or end of file */

static int skip_page (int erpt, Position *pos, const char **s_r)

{
  const char *linebf;
  Line *line;
  uLong firstimethru, linesz, offset;

  vert_skip_coln = 1;		/* reset up/down arrows to first column */

  line   = pos -> line;
  offset = pos -> offset;

  /* Move backward */

  while (erpt < 0) {
    erpt ++;
    firstimethru = 1;					/* we must decrement pointer once for each erpt */
    do {
      if (offset == 0) {				/* see if at beginning of line */
        normalise (pos -> buffer, &line, &offset);	/* if so, move to end of previous line */
        if ((offset == 0) && firstimethru) {
          message (0, "backup before beginning of buffer");
          return (0);
        }
        scroll_hint --;
      }
      if (offset == 0) break;				/* stop if at beginning of buffer */
      offset -= firstimethru;				/* always back up one character first time through */
      firstimethru = 0;					/* ... to skip over the <FF> we started out just after */
      linebf = string_getval (line_string (line));
      while ((offset > 0) && (linebf[offset-1] != 12)) offset --;
    } while (offset == 0);
  }

  /* Move forward */

  linesz = 0;
  if (line != NULL) linesz = string_getlen (line_string (line));
  while (erpt > 0) {
    erpt --;
    firstimethru = 1;
    do {
      if (offset >= linesz) {
        normalize (&line, &offset);			/* make sure we have a char to compare */
        if (line == NULL) {
          linesz = 0;					/* stop if end-of-buffer */
          if (!firstimethru) break;			/* (but we have to have moved at least one char) */
          message (0, "advance beyond end of buffer");
          return (0);
        }
        linesz = string_getlen (line_string (line));
        scroll_hint ++;
      }
      firstimethru = 0;
      linebf = string_getval (line_string (line));
    } while (linebf[offset++] != 12);			/* repeat until we find a <FF> */
  }

  /* All done moving */

  pos -> line   = line;
  pos -> offset = offset;

  return (1);
}

/* Skip to find the search string, prompt for search string */

static int skip_find_prompt (int erpt, Position *pos, const char **s_r)

{
  char c, d;
  const char *linebf, *s;
  int done;
  Line *line;
  String *cmdstring, *keystring, *pmtstring;
  uLong i, pmtsize;

  s = *s_r;						/* get beginning of prompt string */
  c = *(s ++);						/* get the " or ' that starts the prompt */
  if ((c != '\'') && (c != '"')) {
    message (0, "search ? must be followed by '<prompt>' or \"<prompt>\"");
    return (0);
  }
  while (((d = *s) != 0) && (d != c)) s ++;		/* scan to end of prompt string */
  pmtstring = string_create (s - *s_r - 1, *s_r + 1);	/* create prompt string for screen */
  string_concat (pmtstring, 1, "");			/* append a null to end prompt portion */
  pmtsize   = string_getlen (pmtstring);		/* get prompt size including the null */
  keystring = string_create (0, NULL);
  cmdstring = string_create (0, NULL);
  if (d != 0) s ++;					/* point to anything there might be in typeahead */
  do {
    ch_screen_prompt (pmtstring);			/* display prompt on screen */
    string_remove (pmtstring, string_getlen (pmtstring) - pmtsize, pmtsize); /* reset the prompt string to original */
    done = search_prompt (pmtstring, pmtsize, s, &erpt); /* process any readahead that came in command string */
    if (done) break;					/* if done, don't bother reading keyboard */
    if (!jnl_readkeyseq (keystring)) goto abandon;	/* read key sequence from keyboard */
    done = keypad_decode (keystring, cmdstring);	/* convert key sequence to changemode commands */
    if (!done) continue;				/* repeat if partial escape sequence received */
    linebf = string_getval (cmdstring);			/* point to changemode commands */
    done = search_prompt (pmtstring, pmtsize, linebf, &erpt); /* append to prompt */
  } while (!done);
  if (done < 0) goto abandon;
  ch_screen_prompt (pmtstring);				/* display final search string on screen */
  if (searchlen != 0) free (searchstr);			/* free off old search string */
  searchlen = string_getlen (pmtstring) - pmtsize;	/* copy it to current search string buffer */
  searchstr = malloc (searchlen + 1);
  memcpy (searchstr, string_getval (pmtstring) + pmtsize, searchlen);
  searchstr[searchlen] = 0;
  string_delete (pmtstring);				/* free off temp strings */
  string_delete (keystring);
  string_delete (cmdstring);
  *s_r = strlen (s) + s;				/* wipe out anything that is left in typeahead string */
  s = "''" + 1;						/* now that searchstr is filled in, call normal search routine */
  return (skip_find (erpt, pos, &s));

  /* Some error - abandon search */

abandon:
  string_delete (pmtstring);
  string_delete (keystring);
  string_delete (cmdstring);
  return (0);
}

static int search_prompt (String *pmtstring, uLong pmtsize, const char *linebf, int *erpt_r)

{
  char c, *p;
  int i;

  while (*linebf != 0) {				/* process command string characters */
    if (*linebf <= ' ') {				/* skip leading spaces */
      linebf ++;
      continue;
    }
    if (strncasecmp (linebf, "ADV", 3) == 0) {		/* ADV - terminates string, searches forward */
      *erpt_r = ch_defdir = 1;
      return (1);
    }
    if (strncasecmp (linebf, "BACK", 4) == 0) {		/* BACK - terminates string, searches backward */
      *erpt_r = ch_defdir = -1;
      return (1);
    }
    if (linebf[0] == '.') {				/* . - terminates string, uses current direction */
      return (1);
    }
    if (strncasecmp (linebf, "(-DL)", 5) == 0) {	/* (-DL) - abandon operation */
      string_setval (pmtstring, 2, "\000");
      ch_screen_prompt (pmtstring);
      return (-1);
    }
    if (strncasecmp (linebf, "(-DC)", 5) == 0) {	/* (-DC) - delete last search character */
      i = string_getlen (pmtstring);
      if (i > pmtsize) string_remove (pmtstring, 1, -- i);
      linebf += 5;
      continue;
    }
    if ((linebf[0] == 'I') || (linebf[i] == 'i')) {	/* I - supplies characters for the search */
      while (((c = *(++ linebf)) != 0) && (c != 26) && (c != 4)) {
        string_concat (pmtstring, 1, linebf);
      }
      if (c != 0) linebf ++;
      continue;
    }
    if (linebf[0] == '(') {				/* (nnnASC) - supplies a control char for the search */
      c = strtol (linebf + 1, &p, 10);
      while ((*p != 0) && (*p <= ' ')) p ++;
      if (strncasecmp (p, "ASC)", 4) == 0) {
        string_concat (pmtstring, 1, &c);
        linebf = p + 4;
        continue;
      }
    }
    message (strlen (linebf), "invalid search terminator %s, valid are ADV, BACK, .", linebf);
    return (-1);
  }
  return (0);
}

/* Skip to find the search string */
/* Position is left pointing to beginning of string */

static int skip_find (int erpt, Position *pos, const char **s_r)

{
  char c, d;
  const char *linebf, *s;
  Line *line;
  uLong i, j, linesz, offset;

  /* Get search string - use current searchstr if null */

  s = *s_r;						/* get beginning of search string */
  c = s[-1];						/* get character that started it (" or ') */

  while (((d = *s) != 0) && (d != c)) s ++;		/* scan to end of search string */
  if (s > *s_r) {					/* see if non-null string */
    if (searchlen != 0) free (searchstr);		/* if so, free off old one */
    searchlen = s - *s_r;
    searchstr = malloc (searchlen + 1);			/* allocate a new one */
    memcpy (searchstr, *s_r, searchlen);		/* fill it in */
    searchstr[searchlen] = 0;				/* null terminate it */
  }

  if (d != 0) s ++;					/* any way, skip over terminating " or ' */
  *s_r = s;

  /* Point to buffer to start searching at */

  vert_skip_coln = 0;

  line   = pos -> line;
  offset = pos -> offset;
  linesz = 0;
  if (line != NULL) {
    linesz = string_getlen (line_string (line));
    linebf = string_getval (line_string (line));
  }

  /* Search backward */

  while (erpt < 0) {
    erpt ++;
    do {
      if (offset == 0) {				/* see if we're at beginning of a line */
        normalise (pos -> buffer, &line, &offset);	/* ok, get previous line */
        if (offset == 0) {
          message (0, "string not found in reverse direction");
          return (0);
        }
        linesz = offset;				/* point to end of previous line */
        linebf = string_getval (line_string (line));
        scroll_hint --;
      }
      offset --;					/* back up a character */
    } while (!matchstr (line, linesz - offset, linebf + offset)); /* repeat if no match yet */
  }

  /* Search forward */

  while (erpt > 0) {
    erpt --;
    do {
      if (offset == linesz) {				/* see if we are at the very end of a line */
        normalize (&line, &offset);			/* ok, point to very beginning of next line */
        if (line == NULL) {				/* stop if we are at the end-of-buffer now */
          message (0, "string not found in forward direction");
          return (0);
        }
        linesz = string_getlen (line_string (line));	/* ok, get next line */
        linebf = string_getval (line_string (line));
        scroll_hint ++;
      }
      else offset ++;					/* skip over a character */
    } while ((offset == linesz) || !matchstr (line, linesz - offset, linebf + offset)); /* repeat if no match yet */
  }

  /* All done searching */

  pos -> line   = line;
  pos -> offset = offset;

  return (1);
}

/* Assuming pos is at the beginning of a string that matches the searchstr, */
/* point pos at the char following the end of the searchstr                 */

static int skip_found (int erpt, Position *pos, const char **s_r)

{
  const char *linebf;
  int rc;
  uLong linesz;

  rc = 0;
  if (pos -> line != NULL) {
    linesz = string_getlen (line_string (pos -> line));
    linebf = string_getval (line_string (pos -> line));
    rc = matchstr (pos -> line, linesz - pos -> offset, linebf + pos -> offset);
  }
  if (!rc) message (0, "not positioned to search string");
  else {
    pos -> offset += searchlen;
    normalize (&(pos -> line), &(pos -> offset));
  }
  return (rc);
}

/* Set position to select position */

static int skip_sel (int erpt, Position *pos, const char **s_r)

{
  int n;

  vert_skip_coln = 0;

  if (sel_position.buffer == NULL) {
    message (0, "no select range active");
    return (0);
  }

  *pos = sel_position;			/* return select position */
  sel_position.buffer = NULL;		/* deactivate select range */
  return (1);
}

/* Set position to beginning (erpt < 0) or end (erpt > 0) of buffer */

static int skip_buff (int erpt, Position *pos, const char **s_r)

{
  vert_skip_coln = 1;		/* reset up/down arrows to first column */

  page_skip_line = NULL;
  if (erpt < 0) {
    pos -> line   = buffer_first_line (pos -> buffer);
    pos -> offset = 0;
  }
  if (erpt > 0) {
    pos -> line   = NULL;
    pos -> offset = 0;
  }
  ch_screen_top_line = pos -> line;
  return (1);
}

/************************************************************************/
/*									*/
/*  Insert char at current position and increment			*/
/*									*/
/*    Input:								*/
/*									*/
/*	c = character to be inserted					*/
/*	cur_position = current position					*/
/*									*/
/*    Output:								*/
/*									*/
/*	cur_position = incremented to point past char just inserted	*/
/*	               (it points at same char as it did on input)	*/
/*									*/
/************************************************************************/

static void insertchar (char c)

{
  const char *linebf;
  Line *newline;
  String *cur_string;
  uLong linesz;

  buffer_dirty (cur_position.buffer, 1);

  /* Normalize so we are sure not to insert a character after an <LF> in the same line */

  normalize (&cur_position.line, &cur_position.offset);

  /* If at end-of-buffer, start a new line unless last line does not have an \n on it */

  if (cur_position.line == NULL) {
    newline = buffer_last_line (cur_position.buffer);					/* point to last line in buffer */
    if (newline != NULL) {
      linesz = string_getlen (line_string (newline));					/* get last line's length */
      linebf = string_getval (line_string (newline));					/* get last line's address */
      if ((linesz != 0) && (linebf[linesz-1] == '\n')) newline = NULL;			/* if it has \n, pretend it's not there */
    }
    if (newline == NULL) {								/* see if we can append to last line */
      newline = line_insert (cur_position.buffer, NULL, string_create (0, NULL));	/* if not, create a new one */
      linesz  = 0;
    }
    cur_position.line   = newline;
    cur_position.offset = linesz;
  }

  /* Insert character at current position and increment current position */

  cur_string = line_string (cur_position.line);
  string_insert (cur_string, cur_position.offset, 1, &c);
  cur_position.offset ++;

  /* If we just inserted an newline char, split the line into two and set position to beginning of new line */

  linesz = string_getlen (cur_string);
  if ((c == '\n') && (cur_position.offset < linesz)) {
    linebf = string_getval (cur_string);
    newline = line_insert (cur_position.buffer, line_next (cur_position.line), 
                           string_create (linesz - cur_position.offset, linebf + cur_position.offset));
    string_remove (cur_string, linesz - cur_position.offset, cur_position.offset);
    cur_position.line   = newline;
    cur_position.offset = 0;
  }

  /* Normalize position in case we just inserted an <LF> at the end of */
  /* a line, we want to insert the next at beginning of its own line   */

  normalize (&cur_position.line, &cur_position.offset);
}

/************************************************************************/
/*									*/
/*  Get paste buffer name from command string				*/
/*									*/
/*    Input:								*/
/*									*/
/*	*s_r = points past entity name in APPEND or CUT command		*/
/*	       ie, to optional =BUFFER name				*/
/*									*/
/*    Output:								*/
/*									*/
/*	getpastebufname = buffer name (defaults to "PASTE")		*/
/*	*s_r = updated past the =BUFFER name string			*/
/*									*/
/************************************************************************/

static char *getpastebufname (const char **s_r)

{
  char *bufname;
  const char *s, *t;

  s = *s_r;								/* point to command string */
  if (*s != '=') bufname = strdup ("PASTE");				/* if no =, use default PASTE */
  else {
    t = ++ s;								/* save pointer to first char of name */
    while ((*s != 0) && (strchr (bufnamechars, *s) != NULL)) s ++;	/* skip past last char of name */
    bufname = malloc (s - t + 1);					/* malloc a buffer to copy name */
    memcpy (bufname, t, s - t);						/* copy name */
    bufname[s-t] = 0;							/* null terminate it */
    *s_r = s;								/* update command string pointer */
  }
  return (bufname);
}

/************************************************************************/
/*									*/
/*  Cut out a block of text and save in a buffer			*/
/*									*/
/*    Input:								*/
/*									*/
/*	bpos = position to start cutting at (inclusive)			*/
/*	epos = position to stop cutting at (exclusive)			*/
/*	bufname = name of buffer to append cut contents to		*/
/*									*/
/*    Output:								*/
/*									*/
/*	*bpos = *epos = position of cut					*/
/*	cut text appended to buffer					*/
/*									*/
/*    Note:								*/
/*									*/
/*	This routine will swap bpos<->epos if necessary to make epos 	*/
/*	come after bpos.						*/
/*									*/
/************************************************************************/

static int cutitout (Position *bpos, Position *epos, const char *bufname)

{
  const char *delstr;
  Buffer *delbuff;
  Line *delline, *nline, *t2;
  uLong deloffs, t1;

  delbuff = buffer_create (strlen (bufname), bufname);
  if (delbuff == bpos -> buffer) {
    message (strlen (bufname), "can't cut from buffer %s into same buffer", bufname);
    return (0);
  }

#if 00
  outerr (32 + strlen (bufname), "edt*: cutitout (%s[%u] .. %s[%u] -> =%s)\n", 
          (bpos -> line == NULL) ? "EOB" : line_number (bpos -> line), bpos -> offset, 
          (epos -> line == NULL) ? "EOB" : line_number (epos -> line), epos -> offset, 
          bufname);
#endif

  /* Assume we shall dirty both buffers */

  buffer_dirty (delbuff, 1);
  buffer_dirty (epos -> buffer, 1);

  /* Point to end of delete buffer (where stuff goes) */

repeat:
  delline = buffer_last_line (delbuff);
  if (delline == NULL) delline = line_insert (delbuff, NULL, string_create (0, NULL));
  deloffs = string_getlen (line_string (delline));
  if (deloffs > 0) {
    delstr = string_getval (line_string (delline));
    if (delstr[deloffs-1] == '\n') {
      delline = line_insert (delbuff, NULL, string_create (0, NULL));
      deloffs = 0;
    }
  }

  /* See if stuff being deleted is all in the same line */

  if (bpos -> line == epos -> line) {
    if (epos -> offset != bpos -> offset) {
      if (epos -> offset < bpos -> offset) {
        t1 = epos -> offset;
        epos -> offset = bpos -> offset;
        bpos -> offset = t1;
      }
      string_insert (line_string (delline), deloffs, epos -> offset - bpos -> offset, string_getval (line_string (epos -> line)) + bpos -> offset);
      string_remove (line_string (epos -> line), epos -> offset - bpos -> offset, bpos -> offset);
      if (string_getlen (line_string (delline)) == 0) string_delete (removeline (delline));
      epos -> offset = bpos -> offset;
    }
    return (1);
  }

  /* Make sure end is after beginning, switch around if not */

  for (t2 = bpos -> line; t2 != NULL; t2 = line_next (t2)) if (t2 == epos -> line) break;
  if (t2 != epos -> line) {
    t1 = epos -> offset;
    t2 = epos -> line;
    epos -> offset = bpos -> offset;
    epos -> line   = bpos -> line;
    bpos -> offset = t1;
    bpos -> line   = t2;
  }

  /* See if we're deleting the whole beginning line         */
  /* If so, just delete the whole thing and advance to next */

  if (bpos -> offset == 0) {
    nline = line_next (bpos -> line);
    if (deloffs == 0) {
      line_insert (delbuff, delline, line_string (bpos -> line));
      removeline (bpos -> line);
    } else {
      string_insert (line_string (delline), deloffs, string_getlen (line_string (bpos -> line)), string_getval (line_string (bpos -> line)));
      string_delete (removeline (bpos -> line));
    }
    bpos -> line = nline;
    goto repeat;
  }

  /* Keeping first part of the first line, delete the rest of the first line */

  string_insert (line_string (delline), deloffs, 
                 string_getlen (line_string (bpos -> line)) - bpos -> offset, 
                 string_getval (line_string (bpos -> line)) + bpos -> offset);

  string_remove (line_string (bpos -> line), string_getlen (line_string (bpos -> line)) - bpos -> offset, bpos -> offset);

  /* Delete all whole lines inbetween first and last */

  while ((nline = line_next (bpos -> line)) != epos -> line) {
    line_insert (delbuff, NULL, line_string (nline));
    removeline (nline);
  }

  /* Delete first part of last line (if any) */

  if (epos -> offset != 0) {
    delline = line_insert (delbuff, NULL, string_create (epos -> offset, string_getval (line_string (nline))));
  }

  /* Move last part of last line onto end of first line */

  if (epos -> line != NULL) {
    string_insert (line_string (bpos -> line), bpos -> offset, 
                   string_getlen (line_string (nline)) - epos -> offset, 
                   string_getval (line_string (nline)) + epos -> offset);
  }

  /* Delete the last line now that we copied what we want to save to the first line */

  if (epos -> line != NULL) string_delete (removeline (epos -> line));

  /* The two points are now zipped together */

  epos -> line   = bpos -> line;
  epos -> offset = bpos -> offset;

  return (1);
}

/************************************************************************/
/*									*/
/*  Paste a buffer back in						*/
/*									*/
/************************************************************************/

static int pasteitin (int crpt, const char *bufname)

{
  const char *s;
  Buffer *delbuff;
  Line *line;
  Position startpos;
  uLong n;

  /* Get buffer we are getting the data from */

  delbuff = buffer_create (strlen (bufname), bufname);
  if (delbuff == cur_position.buffer) {
    message (strlen (bufname), "can't paste from buffer %s into same buffer", bufname);
    return (0);
  }

  /* If inserting in a backwards direction, save starting position  */

  startpos.buffer = NULL;						/* assume we're going forward */
  if (crpt < 0) {							/* see if we're going backward */
    startpos = cur_position;						/* save current position */
    normalise (startpos.buffer, &startpos.line, &startpos.offset);	/* normalise back in case it is [EOB] */
    crpt = - crpt;							/* fix repeat count */
  }

  /* Perform insertions, a character at a time */

  while (-- crpt >= 0) {
    for (line = buffer_first_line (delbuff); line != NULL; line = line_next (line)) {
      n = string_getlen (line_string (line));
      s = string_getval (line_string (line));
      while (n > 0) { insertchar (*(s ++)); -- n; }
    }
  }

  /* If inserting in a backward direction, restore position to beginning of string */

  if (startpos.buffer != NULL) {					/* see if we are inserting backwards */
    if (startpos.line == NULL) {					/* ok, see if target buffer was completely empty */
      startpos.line   = buffer_first_line (startpos.buffer);		/* it was, so point to beginning of first (new) line */
      startpos.offset = 0;
    }
    cur_position = startpos;						/* ... then put current position there */
    normalize (&cur_position.line, &cur_position.offset);
  }

  return (1);
}

/************************************************************************/
/*									*/
/*  Normalize position (ie, if at very end of line, point to very 	*/
/*  beginning of next line)						*/
/*									*/
/************************************************************************/

static void normalize (Line **line_r, uLong *offset_r)

{
  Line *line;
  uLong length, offset;

  line   = *line_r;
  offset = *offset_r;

  while (line != NULL) {
    length = string_getlen (line_string (line));
    if (offset < length) break;
    line    = line_next (line);
    offset -= length;
  }

  if (line == NULL) offset = 0;
  *line_r   = line;
  *offset_r = offset;
}

/************************************************************************/
/*									*/
/*  The British version normalises backward				*/
/*									*/
/************************************************************************/

static void normalise (Buffer *buffer, Line **line_r, uLong *offset_r)

{
  Line *line;
  uLong offset;

  line   = *line_r;
  offset = *offset_r;

  while (offset == 0) {
    if (line == buffer_first_line (buffer)) break;
    if (line == NULL) line = buffer_last_line (buffer);
    else line = line_prev (line);
    offset = string_getlen (line_string (line));
  }

  *line_r   = line;
  *offset_r = offset;
}

/************************************************************************/
/*									*/
/*  See if the 'searchstr' matches at 'line[offset]'			*/
/*									*/
/*    Input:								*/
/*									*/
/*	line = line being searched					*/
/*	linesz = length remaining at 'offset' in line			*/
/*	linebf = first character at 'offset' in line			*/
/*									*/
/*    Output:								*/
/*									*/
/*	matchstr = 0 : doesn't match					*/
/*	           1 : matches						*/
/*									*/
/************************************************************************/

static int matchstr (Line *line, uLong linesz, const char *linebf)

{
  uLong i, j;

  for (i = 0;;) {
    j = searchlen - i;				/* see how much of searchstr is left to do */
    if (j > linesz) j = linesz;			/* but only do as much as is left in line buffer */
    if ((*xstrncmp) (linebf, searchstr + i, j) != 0) return (0); /* stop if doesn't match */
    i += j;					/* ok, offset past the matched part in searchstr */
    if (i == searchlen) return (1);		/* we're successful if the whole thing matched */
    line = line_next (line);			/* try to match the rest with the next line */
    if (line == NULL) return (0);
    linesz = string_getlen (line_string (line));
    linebf = string_getval (line_string (line));
  }
}

/************************************************************************/
/*									*/
/*  Free all lines of a buffer						*/
/*									*/
/************************************************************************/

static void emptybuffer (const char *name)

{
  Buffer *buffer;
  Line *line;

  buffer = buffer_create (strlen (name), name);
  buffer_dirty (buffer, 1);
  while ((line = buffer_first_line (buffer)) != NULL) {
    string_delete (removeline (line));
  }
}

/************************************************************************/
/*									*/
/*  Remove line from buffer (just line line_remove), but if it is the 	*/
/*  top or select line, advance to next in buffer			*/
/*									*/
/************************************************************************/

static String *removeline (Line *line)

{
  if (line == ch_screen_top_line) ch_screen_top_line = line_next (line);
  if (line == page_skip_line) page_skip_line = NULL;
  if (line == sel_position.line) { sel_position.line = line_next (line); sel_position.offset = 0; }
  return (line_remove (line));
}

/************************************************************************/
/*									*/
/*  Output (error) message to screen					*/
/*									*/
/*    Input:								*/
/*									*/
/*	extra = number of extra chars required for formatting		*/
/*	format = format string						*/
/*	... = parameters						*/
/*									*/
/************************************************************************/

static void message (int extra, const char *format, ...)

{
  char *buf;
  va_list ap;

  buf = malloc (extra + strlen (format) + 1);	/* allocate room for format and all parameter strings */
  va_start (ap, format);			/* point to parameters */
  vsprintf (buf, format, ap);			/* format the string */
  va_end (ap);
  ch_screen_message (buf);			/* output message to screen */
  free (buf);					/* free off buffer */
}
