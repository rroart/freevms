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
/*  Parse range spec from command line					*/
/*									*/
/*    Input:								*/
/*									*/
/*	cp = command line pointer					*/
/*	entry = called back for each matching line			*/
/*	param = parameter to pass to entry routine			*/
/*									*/
/*    Output:								*/
/*									*/
/*	range = -1 : syntax error, error message already output		*/
/*	         0 : successful						*/
/*	      else : non-zero status returned by entry routine		*/
/*									*/
/************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "edt.h"

const char bufnamechars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789";

static int range_mult_run (char *cp, char **cp_r, int (*entry) (void *param, Buffer *buffer, Line *line), void *param);
static int getline (char *cp, char **cp_r, Position *position);

/************************************************************************/
/*									*/
/*  Parse range spec from command line for a single line		*/
/*									*/
/*    Input:								*/
/*									*/
/*	cp = command line pointer					*/
/*									*/
/*    Output:								*/
/*									*/
/*	range_single = -1 : syntax error, error message already output	*/
/*	                0 : successful					*/
/*	*cp_r = points to terminating character				*/
/*	*position_r = points to located line				*/
/*									*/
/************************************************************************/

int range_single (char *cp, char **cp_r, Position *position_r)

{
  char c, *p;
  int i, n, rc, valid;
  Position position;

  /* If nothing specified, just use current line */

  cp = skipspaces (cp);
  if (*cp == 0) {
    *cp_r       = cp;
    *position_r = cur_position;
    return (0);
  }

  /* Default is the current position */

  position = cur_position;

  /* See if buffer specified */

  cp = skipspaces (cp);
  i  = 0;
  if ((*cp == '=') || (i = matchkeyword (cp, "buffer", 2))) {
    if (i == 0) p = skipspaces (cp + 1);
    else if (cp[i] > ' ') goto badrange;
    else p = skipspaces (cp + i);
    for (cp = p; (c = *cp) != 0; cp ++) if (strchr (bufnamechars, c) == NULL) break;
    if (cp == p) goto badrange;
    position.buffer = buffer_create (cp - p, p);	/* point to the buffer (creating it if necessary) */
    position.line   = buffer_first_line (position.buffer); /* point to beginning of first line in buffer */
    position.offset = 0;
    valid = 1;						/* we found something legitimate */
  }

  /* Get line */

  rc = getline (cp, &cp, &position);			/* process line spec */
  if (rc < 0) return (-1);				/* fatal syntax error */

  /* Nothing more to do, return pointer to terminator */

  *cp_r = cp;
  *position_r = position;
  return (0);

  /* Can't decode it */

badrange:
  outerr (strlen (cp), "bad range specification %s\n", cp);
  return (-1);
}

/************************************************************************/
/*									*/
/*  Parse range spec from command line for multiple lines		*/
/*									*/
/*    Input:								*/
/*									*/
/*	cp = command line pointer					*/
/*	entry = called back for each matching line			*/
/*	param = parameter to pass to entry routine			*/
/*									*/
/*    Output:								*/
/*									*/
/*	range = -1 : syntax error, error message already output		*/
/*	         0 : successful						*/
/*	      else : non-zero status returned by entry routine		*/
/*									*/
/************************************************************************/

int range_multiple (char *cp, char **cp_r, int (*entry) (void *param, Buffer *buffer, Line *line), void *param)

{
  int rc;

  rc = range_mult_run (cp, cp_r, NULL, NULL);			/* do syntax check first */
  if (rc == 0) rc = range_mult_run (cp, cp_r, entry, param);	/* if ok, do it for real */
  return (rc);							/* return final status */
}

static int range_mult_run (char *cp, char **cp_r, int (*entry) (void *param, Buffer *buffer, Line *line), void *param)

{
  Buffer *buffer;
  char c, *p;
  int i, n, rc, valid;
  Line *firstline, *lastline, *line, *nline;
  Position pos;

  /* If nothing specified, just use current line */

  cp = skipspaces (cp);
  if (*cp == 0) {
    if (entry == NULL) return (0);
    return ((*entry) (param, cur_position.buffer, cur_position.line));
  }

  /* Default is just the current line */

  buffer    = cur_position.buffer;
  firstline = cur_position.line;
  lastline  = cur_position.line;

  /* Process items from string */

loop:
  valid = 0;

  /* See if buffer specified */

  cp = skipspaces (cp);
  i  = 0;
  if ((*cp == '=') || (i = matchkeyword (cp, "buffer", 2))) {
    if (i == 0) p = skipspaces (cp + 1);
    else if (cp[i] > ' ') goto badrange;
    else p = skipspaces (cp + i);
    for (cp = p; (c = *cp) != 0; cp ++) if (strchr (bufnamechars, c) == NULL) break;
    if (cp == p) goto badrange;
    buffer    = buffer_create (cp - p, p);		/* point to the buffer (creating it if necessary) */
    firstline = buffer_first_line (buffer);		/* default range is the whole thing */
    lastline  = NULL;
    valid = 1;						/* we found something legitimate */
  }

  /* If WHOLE, use the whole buffer */

  if (i = matchkeyword (cp, "whole", 1)) {
    if (cp[i] > ' ') goto badrange;
    valid = 1;
    firstline = buffer_first_line (buffer);
    lastline  = NULL;
    cp = skipspaces (cp + i);
  }

  /* If BEFORE, use all lines before the current line */

  else if (i = matchkeyword (cp, "before", 3)) {
    if (cp[i] > ' ') goto badrange;
    if (buffer != cur_position.buffer) {
      outerr (0, "BEFORE only valid for current buffer\n");
      return (-1);
    }
    valid = 1;
    firstline = buffer_first_line (buffer);
    lastline  = cur_position.line;
    if ((cur_position.offset == 0) && (firstline != lastline)) { /* if it ends on first char of a line ... */
      if (lastline != NULL) lastline = line_prev (lastline);	/* ... back up to previous line */
      else lastline = buffer_last_line (cur_position.buffer);
    }
    cp = skipspaces (cp + i);
  }

  /* If REST, use all lines at and after current line */

  else if (i = matchkeyword (cp, "rest", 1)) {
    if (cp[i] > ' ') goto badrange;
    if (buffer != cur_position.buffer) {
      outerr (0, "REST only valid for current buffer\n");
      return (-1);
    }
    valid = 1;
    firstline = cur_position.line;
    lastline  = NULL;
    cp = skipspaces (cp + i);
  }

  /* If SELECT, use from the select position line to the current position line */

  else if (i = matchkeyword (cp, "select", 3)) {
    uLong firstoffset, lastoffset;

    if (cp[i] > ' ') goto badrange;
    if (buffer != cur_position.buffer) {
      outerr (0, "SELECT only valid for current buffer\n");
      return (-1);
    }
    valid = 1;							/* we found something legit to process */
    firstline   = sel_position.line;				/* assume select line is before current line */
    firstoffset = sel_position.offset;
    lastline    = cur_position.line;
    lastoffset  = cur_position.offset;
    if (relposition (&cur_position, &sel_position) < 0) {	/* see if that is the case */
      firstline   = cur_position.line;				/* if not, put the current line first */
      firstoffset = cur_position.offset;
      lastline    = sel_position.line;
      lastoffset  = sel_position.offset;
    }
    if ((lastoffset == 0) && (firstline != lastline)) {		/* if it ends on first char of a line ... */
      if (lastline != NULL) lastline = line_prev (lastline);	/* ... back up to previous line */
      else lastline = buffer_last_line (cur_position.buffer);
    }
    if (entry != NULL) sel_position.buffer = NULL;		/* if this is for real, deactivate select range */
    cp = skipspaces (cp + i);					/* point to next item in command string */
  }

  /* Get basic first:last, first#count lines of the range - */
  /* Sets up firstline to the first line to scan,           */
  /* lastline to next after last line to scan               */

  else {
    pos.buffer = buffer;
    pos.line   = firstline;
    pos.offset = 0;
    rc = getline (cp, &cp, &pos);						/* process first line number */
    if (rc < 0) return (-1);							/* fatal syntax error */
    if (rc > 0) {
      valid = 1;								/* we found something legitimate */
      lastline = firstline = pos.line;
    }
    cp = skipspaces (cp);							/* see if followed by : */
    if (*cp == ':') {
      valid = 1;								/* if so, we found something legitimate */
      pos.line = lastline;
      pos.offset = 0;
      rc = getline (cp + 1, &cp, &pos);						/* get last line number */
      if (rc < 0) return (-1);							/* fatal syntax error */
      if (rc == 0) lastline = NULL;						/* if missing, use end of buffer */
      else lastline = pos.line;
      if (rc == 2) {
        if (lastline != NULL) lastline = line_prev (lastline);			/* next after last found, back up one line */
        else lastline = buffer_last_line (buffer);
      }
    } else if (*cp == '#') {							/* check for # */
      n = strtol (cp + 1, &p, 0);						/* ok, decode the number */
      if (n <= 0) goto badrange;						/* must have something! */
      valid = 1;								/* ok, its valid */
      lastline = firstline;
      while ((-- n > 0) && (lastline != NULL)) {
        lastline = line_next (lastline);
      }
      cp = p;
    }
  }

  /* If there is an ALL 'string', just those lines that contain the string */

  if (i = matchkeyword (cp, "all", 1)) {
    cp = skipspaces (cp + i);
    if ((*cp != '\'') && (*cp != '\"')) {
      outerr (strlen (cp), "ALL must be followed by ' or \", not %s\n", cp);
      return (-1);
    }
    for (p = cp; (c = *(++ cp)) != 0;) if (c == *p) break;
    if (entry != NULL) {
      *cp = 0;
      for (line = firstline; line != NULL; line = nline) {		/* start with the first line */
        if (line != NULL) nline = line_next (line);			/* get what next line will be */
        if ((*xstrstr) (string_getval (line_string (line)), p + 1) != NULL) {
          rc = (*entry) (param, buffer, line);				/* ... in case this routine deletes line */
          if (rc != 0) {						/* abort if routine says to */
            *cp = c;
            return (rc);
          }
        }
        if (line == lastline) break;					/* stop if just did the last line */
      }
      *cp = c;
    }
    if (c != 0) cp ++;
    cp = skipspaces (cp);
  }

  /* Otherwise, error if nothing valid */

  else if (!valid) goto badrange;

  /* Otherwise, do all lines in the range firstline:lastline */

  else if (entry != NULL) {
    for (line = firstline;; line = nline) {		/* start with the first line */
      if (line != NULL) nline = line_next (line);	/* get what next line will be */
      rc = (*entry) (param, buffer, line);		/* ... in case this routine deletes line */
      if (rc != 0) return (rc);				/* abort if routine says to */
      if ((line == NULL) || (line == lastline)) break;	/* stop if just did the last line */
    }
  }

  /* Maybe there are more lines to process */

  cp = skipspaces (cp);
  if (*cp == ',') {
    cp ++;
    goto loop;
  }
  if (strncasecmp (cp, "and", 3) == 0) {
    cp += 3;
    if (*cp <= ' ') goto loop;
  }

  /* Nothing more to do, return pointer to terminator */

  *cp_r = cp;
  return (0);

  /* Can't decode it */

badrange:
  outerr (strlen (cp), "bad range specification %s\n", cp);
  return (-1);
}

/************************************************************************/
/*									*/
/*  Try to get line number						*/
/*									*/
/*    Input:								*/
/*									*/
/*	cp = points to line number string				*/
/*	buffer = points to buffer the line is in			*/
/*	*line_r = line in buffer to start at for relative searches	*/
/*									*/
/*    Output:								*/
/*									*/
/*	getline = -1 : syntax error (error message already output)	*/
/*	           0 : no line number found in command line		*/
/*	           1 : exact match found				*/
/*	           2 : next line > requested found			*/
/*	**cp_r  = terminating character					*/
/*	*line_r = requested line (NULL for end of buffer)		*/
/*									*/
/************************************************************************/

static int search_setup (char *cp, char **cp_r, const char **p_r, char *c_r);

static int getline (char *cp, char **cp_r, Position *position)

{
  char c, *p, *q, *s;
  const char *pp, *qq, *ss;
  int n, rc;
  Line *line, *nline;
  uLong offset;

  cp = skipspaces (cp);
  c  = *cp;

  /* Check for BEGIN and END keywords */

  if (n = matchkeyword (cp, "begin", 1)) {
    line   = buffer_first_line (position -> buffer);
    offset = 0;
    cp    += n;
    rc     = 1;
    goto checkpm;
  }

  if (n = matchkeyword (cp, "end", 1)) {
    line   = NULL;
    offset = 0;
    cp    += n;
    rc     = 1;
    goto checkpm;
  }

  /* Process 'dot' reference */

  if (c == '.') {
    if (position -> buffer != cur_position.buffer) {
      outerr (0, "dot reference only allowed in current buffer\n");
      return (-1);
    }
    line   = cur_position.line;				/* point to beginning of current line */
    offset = 0;
    cp ++;
    rc     = 1;
    goto checkpm;
  }

  /* Process absolute line numbers */

  if ((c >= '0') && (c <= '9')) {			/* starts with a string of digits */
    for (p = cp; (c = *p) != 0; p ++) if ((c < '0')  || (c > '9')) break;
    if (c == '.') {					/* ... then optional . followed by digits */
      while ((c = *(++ p)) != 0) if ((c < '0')  || (c > '9')) break;
    }
    *p = 0;						/* ok, put a temp null terminator at end */
    line = position -> line;				/* start with default line */
    if (line == NULL) line = buffer_last_line (position -> buffer);
    rc = 1;						/* assume we will find exact match */
    while (line != NULL) {				/* repeat until we hit end of buffer */
      n = line_numcmp (line, cp);			/* compare line's number with desired number */
      if (n == 0) break;				/* stop if exact match */
      if (n < 0) line = line_next (line);		/* if line is too low, check next in buffer */
      if (n > 0) {
        nline = line_prev (line);			/* if line is too high, see if there is a previous in buffer */
        if ((nline == NULL) || (line_numcmp (nline, cp) < 0)) {
          rc = 2;					/* this one too high, one before it is too low, stop now */
          break;
        }
        line = nline;
      }
    }
    if (line == NULL) rc = 2;				/* if we ran off end, say approximate match */
    offset = 0;						/* always point to beginning of matched line */
    *p = c;						/* restore terminating character */
    cp = p;						/* return pointer to terminating character */
    goto checkpm;
  }

  /* That's all we can do */

  rc     = 0;
  line   = position -> line;
  offset = position -> offset;
  if ((c == '\'') || (c == '"')) goto search_forward;

  /* Check for [+/-[n]] */

checkpm:
  cp = skipspaces (cp);					/* see what follows what we found */
  n  = 0;						/* assume there isn't */
  if (*cp == '+') {					/* check for '+ n' */
    cp = skipspaces (cp + 1);				/* ok, skip over the '+' */
    if ((*cp == '\'') || (*cp == '"')) goto search_forward;
    n  = strtol (cp, &p, 0);				/* decode the 'n', leave it positive */
    if ((n == 0) && (cp == *cp_r)) n = 1;
  } else if (*cp == '-') {				/* check for '- n' */
    cp = skipspaces (cp + 1);				/* ok, skip over the '-' */
    if ((*cp == '\'') || (*cp == '"')) goto search_backward;
    n  = - strtol (cp, &p, 0);				/* decode the 'n', make it negative */
    if ((n == 0) && (cp == *cp_r)) n = -1;
  } else {
    *cp_r = cp;						/* no + or -, string ends here */
    position -> line = line;				/* return pointer to line found */
    position -> offset = 0;
    return (rc);					/* all done */
  }
  cp = p;
  *cp_r = p;
  position -> offset = 0;				/* these searches all end at beginning of line */
  if (rc == 0) rc = 1;					/* we did find something after all */
  while (n < 0) {					/* if negative, skip backwards that number */
    position -> line = line;				/* just in case we run off top, return pointer to current line */
    if (line != NULL) line = line_prev (line);		/* link to previous line in buffer */
    else line = buffer_last_line (cur_position.buffer); /* (first time through only, point to last line in buffer if started at end)  */
    if (line == NULL) return (2);			/* if ran off top, return 'line found > requested' status */
    n ++;						/* one less line to skip over */
  }
  while (n > 0) {					/* if positive, skip forwards that number */
    if (line == NULL) break;				/* if ran off end, we're done */
    line = line_next (line);				/* point to next line in buffer */
    n --;						/* one less line to skip over */
  }
  goto checkpm;						/* check for another +/- garbo (like a search) */

  /* +'string' or +"string" - search forward */

search_forward:
  if (!search_setup (cp, &cp, &pp, &c)) return (-1);
  if ((line == position -> line) && (offset == position -> offset)) offset ++;
  while (line != NULL) {
    ss = string_getval (line_string (line));
    if (offset < string_getlen (line_string (line))) {
      qq = (*xstrstr) (ss + offset, pp);
      offset = qq - ss;
      if (qq != NULL) goto search_found;
    }
    line = line_next (line);
    offset = 0;
  }
  goto search_notfound;

  /* -'string' or -"string" - search backward */

search_backward:
  if (!search_setup (cp, &cp, &pp, &c)) return (-1);
  if (line == NULL) {
    line = buffer_last_line (position -> buffer);
    if (line == NULL) goto search_notfound;
    offset = string_getlen (line_string (line));
  }
  n = strlen (pp);
  while (1) {
    ss = string_getval (line_string (line));
    while (1) {
      if ((*xstrncmp) (ss + offset, pp, n) == 0) goto search_found;
      if (offset == 0) break;
      -- offset;
    }
    line = line_prev (line);
    if (line == NULL) goto search_notfound;
    offset = string_getlen (line_string (line));
  }

search_found:
  position -> line   = line;
  position -> offset = offset;
  *cp = c;
  if (c != 0) cp ++;
  *cp_r = cp;
  return (1);

search_notfound:
  outerr (strlen (pp), "can't find line containing %s\n", pp);
  *cp = c;
  return (-1);
}

static int search_setup (char *cp, char **cp_r, const char **p_r, char *c_r)

{
  char c;
  const char *p;

  c = *(cp ++);						/* get the ' or " */
  p = cp;						/* point to beginning of search string */
  while (*cp != 0) {					/* find end of search string */
    if (*cp == c) break;
    cp ++;
  }
  c   = *cp;						/* get the ', " or null */
  *cp = 0;						/* make sure there's a null terminator */
  if (*p == 0) {					/* check for null search string */
    p = searchstr;					/* if so, use same as last time */
  } else {
    if (searchlen != 0) free (searchstr);		/* else, save the string for next time */
    searchlen = strlen (p);
    searchstr = strdup (p);
  }

  *cp_r = cp;						/* return pointer to search string terminator */
  *p_r  = p;						/* return pointer to search string */
  *c_r  = c;						/* return search string terminator */
  return (1);						/* success */
}

/************************************************************************/
/*									*/
/*  Match keyword in string						*/
/*									*/
/*    Input:								*/
/*									*/
/*	cp = pointer to command string					*/
/*	kw = pointer to keyword string					*/
/*	min = minimum number of chars that must match			*/
/*									*/
/*    Output:								*/
/*									*/
/*	matchkeyword = 0 : no match					*/
/*	            else : number of chars that matched			*/
/*									*/
/************************************************************************/

int matchkeyword (const char *cp, const char *kw, int min)

{
  int i;

  for (i = strlen (kw); i >= min; i --) if (strncasecmp (cp, kw, i) == 0) return (i);
  return (0);
}
