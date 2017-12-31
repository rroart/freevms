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

#ifndef _EDT_H
#define _EDT_H

#ifdef _OZONE
#include "ozone.h"
#else
typedef unsigned long uLong;
typedef unsigned long long uQuad;
typedef unsigned short uWord;
typedef unsigned char uByte;
#endif

typedef struct Buffer Buffer;
typedef struct Line Line;
typedef struct Position Position;
typedef struct String String;

struct Position
{
    Buffer *buffer;
    Line *line;
    uLong offset;
};

#include <stdio.h>

/* chrutils.c */

char *skipspaces (char *s);
char *uptospace (char *s);
int eoltest (char *s);

/* ch_screen_*.c */

int ch_screen_init (void);		/* initiate screen mode */
String *ch_screen_read (void);		/* update screen / read command string */
void ch_screen_refresh (void);		/* refresh (redraw) screen */
void ch_screen_term (void);		/* terminate screen mode */
void ch_screen_prompt (String *prompt);	/* display prompt string */
void ch_screen_message (const char *message); /* display (error) message */
uLong ch_screen_chr2col (uLong linesz, const char *linebf); /* convert character count to column count */
uLong ch_screen_col2chr (uLong linesz, const char *linebf, uLong ncols); /* convert column count to character count */
String *ch_screen_rdkpad (void);	/* read keypad codes from keyboard */

/* cmd_change.c */

extern Line *ch_screen_top_line;	/* pointer to what we want for top line on screen */
extern int ch_screen_shiftleft;		/* number of characters to leave off beg of line */
extern int ch_screen_num_lines;		/* total number of lines on the screen */
extern int ch_screen_tmar_lines;	/* number of lines to reserve for top margin */
extern int ch_screen_bmar_lines;	/* number of lines to reserve for bottom margin */
extern int ch_screen_width;		/* number of columns on the screen, including any being used for 'set number show' */
extern int ch_screen_numofs;		/* number of columns on left currently being used for 'set number show' */
extern int autoshift;
extern char *searchstr;
extern uLong searchlen;
extern Position sel_position;

/* cmd_help.c */

void cmd_help (char *cp);

/* cmd_set.c */

extern char *(*xstrstr) ();
extern int  (*xstrncmp) ();

/* crash.c */

void crash (char *message);

/* edt.c */

extern Buffer *main_buffer;
extern char *journal_name;
extern const char *pn;
extern FILE *journal_file, *recover_file;
extern int showlfs;
extern int shownums;
extern Position cur_position;

/* journaling.c */

String *jnl_readprompt (const char *prompt);
int jnl_readkeyseq (String *keystring);
void jnl_flush (void);
void jnl_close (int del);

/* keypad.c */

void show_keypad (void);
const char *keypad_getdef (const char *keyname);
void keypad_setdef (const char *keyname, const char *command);
int keypad_getname (const char *keystring, char keyname[16]);
int keypad_decode (String *keystring, String *cmdstring);

/* line.c */

Buffer *buffer_create (int namel, const char *name);
void buffer_delete (Buffer *buffer);
Buffer *buffer_next (Buffer *buffer);
Line *buffer_first_line (Buffer *buffer);
Line *buffer_last_line (Buffer *buffer);
uLong buffer_linecount (Buffer *buffer);
Position *buffer_savpos (Buffer *buffer);
const char *buffer_name (Buffer *buffer);
const char *buffer_filename (Buffer *buffer);
void buffer_setreadfile (Buffer *buffer, FILE *readfile);
FILE *buffer_getreadfile (Buffer *buffer);
void buffer_setfile (Buffer *buffer, const char *filename);
int buffer_dirty (Buffer *buffer, int newdirty);
Line *line_insert (Buffer *buffer, Line *next, String *string);
String *line_remove (Line *line);
const char *line_number (Line *line);
int line_numcmp (Line *line, const char *number);
Line *line_next (Line *line);
Line *line_prev (Line *line);
String *line_string (Line *line);
void line_reseq (Line *line);
void line_print (Line *line);

/* ln_command.c */

int ln_command (const char *cmdstr);

/* os.c */

extern char *help_name;
void os_initialization (void);
void os_screenmode (int on);
String *os_readprompt (const char *prompt);
int os_readkeyseq (String *keystring);
int os_writebuffer (int size, const char *buff);
int os_getscreensize (int *width_r, int *length_r);
char *os_makejnlname (const char *filename);
FILE *os_crenewfile (const char *name);
char *os_defaultinitname (void);

/* output.c */

void outerr (int extra, const char *format, ...);
void outfmt (int extra, const char *format, ...);
void outstr (const char *string);
void outchr (char c);
void outbuf (int size, const char *buff);
void output (void);

/* range.c */

extern const char bufnamechars[];
int range_single (char *cp, char **cp_r, Position *pos_r);
int range_multiple (char *cp, char **cp_r, int (*entry) (void *param, Buffer *buffer, Line *line), void *param);
int matchkeyword (const char *cp, const char *kw, int min);

/* read_file.c */

void read_file (FILE *input_file, Buffer *buffer, Line *next_line);
String *readfileline (FILE *input_file, String *string);

/* relposition.c */

int relposition (Position *p1, Position *p2);

/* representation.c */

const char *representation (char c, char temp[16], int col);

/* string.c */

String *string_create (uLong len, const char *val);
void string_delete (String *string);
const char *string_getval (String *string);
uLong string_getlen (String *string);
void string_setval (String *string, uLong len, const char *value);
void string_concat (String *string, uLong len, const char *value);
void string_remove (String *string, uLong length, uLong offset);
void string_insert (String *string, uLong offset, uLong length, const char *insert);
int string_scanchr (String *string, char chr);
int string_scanstr (String *string, const char *str);

/* write_file.c */

int write_file (const char *out_name, Line *beg_line, Line *end_line);

#endif
