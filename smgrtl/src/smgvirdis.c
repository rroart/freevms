// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ssdef.h>
#include <smg$routines.h>

#include "smg.h"

struct virtual_display
{
    long smg$l_viewport;
    int smg$l_number_of_rows;
    int smg$l_number_of_columns;
    int smg$l_display_attributes;
    int smg$l_video_attributes;
    int smg$l_character_set;
    int smg$l_x;
    int smg$l_y;
    char * smg$t_buffer;
    long * smg$l_pasteboard;
};

#undef smg$create_virtual_display
int smg$create_virtual_display (int * number_of_rows , int * number_of_columns , long * display_id , int * display_attributes , int * video_attributes , int * character_set)
{
    int status;
    $DESCRIPTOR(dsc, "sys$input");
    struct virtual_display * smg = malloc (sizeof (struct virtual_display));
    *display_id = smg;
    smg->smg$l_number_of_rows = *number_of_rows;
    smg->smg$l_number_of_columns = *number_of_columns;
    if (display_attributes)
        smg->smg$l_display_attributes = *display_attributes;
    if (video_attributes)
        smg->smg$l_video_attributes = *video_attributes;
    if (character_set)
        smg->smg$l_character_set = *character_set;
    smg->smg$t_buffer = malloc(smg->smg$l_number_of_rows * smg->smg$l_number_of_columns);
    smg->smg$l_viewport = 0;
    smg->smg$l_x = 1;
    smg->smg$l_y = 1;
    return SS$_NORMAL;
}

int smg$$insert_viewport(struct virtual_display * s, long ** v)
{
    *v = s;
    s->smg$l_viewport = v;
    return SS$_NORMAL;
}

int smg$$get_viewport(struct virtual_display * s, long ** v)
{
    *v = s->smg$l_viewport;
    return SS$_NORMAL;
}

#undef smg$set_cursor_abs
int smg$set_cursor_abs (long * display_id , int * start_row, int * start_column)
{
    struct virtual_display * smg = *display_id;
    smg->smg$l_x = *start_column;
    smg->smg$l_y = *start_row;
    char buf[16];
    sprintf(buf, "\33[%d;%dH", smg->smg$l_y, smg->smg$l_x);
    write (1, buf, strlen(buf));
    return SS$_NORMAL;
}

#undef smg$set_cursor_rel
int smg$set_cursor_rel (long * display_id , int * delta_row, int * delta_column)
{
    struct virtual_display * smg = *display_id;
    smg->smg$l_x += *delta_column;
    smg->smg$l_y += *delta_row;
    char buf[16];
    sprintf(buf, "\33[%d;%dH", smg->smg$l_y, smg->smg$l_x);
    write (1, buf, strlen(buf));
    return SS$_NORMAL;
}

int smg$$cursor (long * display_id)
{
    struct virtual_display * smg = display_id;
    char buf[16];
    sprintf(buf, "\33[%d;%dH", smg->smg$l_y, smg->smg$l_x);
    write (1, buf, strlen(buf));
    return SS$_NORMAL;
}

extern int smgunix;

// check connection to paste
#undef smg$put_chars
int smg$put_chars ( long * display_id, void * text, int * start_row, int * start_column, int * flags, int * rendition_set, int * rendition_complement, int * character_set)
{
    struct dsc$descriptor * dsc = text;
    struct virtual_display * smg = *display_id;
    // replace later
    int x = smg->smg$l_x;
    int y = smg->smg$l_y;
    if (start_column)
        x = *start_column;
    if (start_row)
        y= *start_row;
    x--;
    y--;
    char *buf = smg->smg$t_buffer + x + y * smg->smg$l_number_of_columns;
    memcpy (buf, dsc->dsc$a_pointer, dsc->dsc$w_length);
    smg->smg$l_x += dsc->dsc$w_length;
    smg$$pasteboard_update(smg->smg$l_pasteboard);
    return SS$_NORMAL;
}

// check connection to paste
#undef smg$put_line
int smg$put_line ( long * display_id, void * text, int * line_advance, int * rendition_set, int * rendition_complement, int * flags, int * direction)
{
    struct dsc$descriptor * dsc = text;
    struct virtual_display * smg = *display_id;
    // replace later
    int x = smg->smg$l_x;
    int y = smg->smg$l_y;
    x--;
    y--;
    char *buf = smg->smg$t_buffer + x + y * smg->smg$l_number_of_columns;
    memcpy (buf, dsc->dsc$a_pointer, dsc->dsc$w_length);
    if (line_advance)
        smg->smg$l_y += *line_advance;
    else
        smg->smg$l_y++;
    if (smg->smg$l_y > smg->smg$l_number_of_rows)
    {
        y = smg->smg$l_y = smg->smg$l_number_of_rows;
        x = 0;
        y--;
        int lines = 1;
        char *buf = smg->smg$t_buffer + x + lines * smg->smg$l_number_of_columns;
        char * c = smg->smg$t_buffer;
        memcpy (c, buf, (smg->smg$l_number_of_rows - lines) * smg->smg$l_number_of_columns);
        memset (c + (smg->smg$l_number_of_rows - lines) * smg->smg$l_number_of_columns, 0, smg->smg$l_number_of_columns);
    }
    smg$$pasteboard_update(smg->smg$l_pasteboard);
    return SS$_NORMAL;
}

static bufwrite(char * buf, char * src, int * index)
{
    int len = strlen(src);
    memcpy (buf + *index, src, len);
    *index += len;
}

static bufwrite2(char * buf, char * src, int * index, int len)
{
    memcpy (buf + *index, src, len);
    *index += len;
}

int smg$$display (long * display_id, int row, int column)
{
    struct virtual_display * smg = display_id;
    int x = 1;
    int y = 1;
    int xsize = smg->smg$l_number_of_columns;
    int ysize = smg->smg$l_number_of_rows;
    if (smg->smg$l_viewport)
        smg$get_viewport_char (&smg, &y, &x, &ysize, &xsize);
    x--;
    y--;
    char buf[2048];
    char buf2[256];
    int bufi = 0;
    int i;
    for ( i = 0; i < ysize; i++)
    {
        sprintf(buf2, "\33[%d;%dH", row + i, column);
        bufwrite (buf, buf2, &bufi);
        bufwrite2 (buf, smg->smg$t_buffer + x + (i + y) * smg->smg$l_number_of_columns, &bufi, xsize);
    }
    write (1, buf, bufi);
    return SS$_NORMAL;
}

// remap later
#undef smg$change_virtual_display
int smg$change_virtual_display (long * display_id, int * number_of_rows, int * number_of_columns, int * display_attributes, int * video_attributes, int * character_set)
{
    int status;
    struct virtual_display * smg = *display_id;
    *display_id = smg;
    if (number_of_rows)
        smg->smg$l_number_of_rows = *number_of_rows;
    if (number_of_columns)
        smg->smg$l_number_of_columns = *number_of_columns;
    if (display_attributes)
        smg->smg$l_display_attributes = *display_attributes;
    if (video_attributes)
        smg->smg$l_video_attributes = *video_attributes;
    if (character_set)
        smg->smg$l_character_set = *character_set;
    smg->smg$l_x = 1;
    smg->smg$l_y = 1;
    return SS$_NORMAL;
}

int smg$copy_virtual_display (long * current_display_id , long * new_display_id)
{
    struct virtual_display * smg = malloc (sizeof (struct virtual_display));
    *new_display_id = smg;
    memcpy (smg, *current_display_id, sizeof(struct virtual_display));
    return SS$_NORMAL;
}

int smg$cursor_column (long * display_id)
{
    struct virtual_display * smg = *display_id;
    return smg->smg$l_x;
}

int smg$cursor_row (long * display_id)
{
    struct virtual_display * smg = *display_id;
    return smg->smg$l_y;
}

// check connection to paste. and do not fill? this is more like erase?
int smg$delete_chars ( long * display_id, int * number_of_characters, int * start_row, int * start_column)
{
    struct virtual_display * smg = *display_id;
    // replace later
    int x = smg->smg$l_x;
    int y = smg->smg$l_y;
    if (start_column)
        x = *start_column;
    if (start_row)
        y= *start_row;
    x--;
    y--;
    char *buf = smg->smg$t_buffer + x + y * smg->smg$l_number_of_columns;
    memcpy (buf, buf + *number_of_characters, smg->smg$l_number_of_columns - x);
    memset (buf + smg->smg$l_number_of_columns - x, 0, *number_of_characters);
    return SS$_NORMAL;
}

// check connection to paste. do not fill? more like erase?
#undef smg$delete_line
int smg$delete_line ( long * display_id, int * start_row, int * number_of_rows)
{
    struct virtual_display * smg = *display_id;
    // replace later
    int x = 1;
    int y = *start_row;
    int lines = 1;
    if (number_of_rows)
        lines = *number_of_rows;
    x--;
    y--;
    char *buf = smg->smg$t_buffer + x + y * smg->smg$l_number_of_columns;
    memcpy (buf, buf + lines * smg->smg$l_number_of_columns, (smg->smg$l_number_of_rows - lines) * smg->smg$l_number_of_columns);
    memset (buf + (smg->smg$l_number_of_rows - lines) * smg->smg$l_number_of_columns, 0, lines * smg->smg$l_number_of_columns);
    return SS$_NORMAL;
}

// check connection to paste
#undef smg$insert_chars
int smg$insert_chars ( long * display_id, void * character_string, int * start_row, int * start_column, int * rendition_set, int * rendition_complement, int * character_set)
{
    struct dsc$descriptor * dsc = character_string;
    struct virtual_display * smg = *display_id;
    // replace later
    int x = smg->smg$l_x;
    int y = smg->smg$l_y;
    if (start_column)
        x = *start_column;
    if (start_row)
        y= *start_row;
    x--;
    y--;
    char *buf = smg->smg$t_buffer + x + y * smg->smg$l_number_of_columns;
    char c[160];
    memcpy (c, buf, smg->smg$l_number_of_columns - x - dsc->dsc$w_length);
    memcpy (buf, dsc->dsc$a_pointer, dsc->dsc$w_length);
    memcpy (buf + dsc->dsc$w_length, c, smg->smg$l_number_of_columns - x - dsc->dsc$w_length);
    // smg->smg$l_x += dsc->dsc$w_length;
    return SS$_NORMAL;
}

// check connection to paste
#undef smg$insert_line
int smg$insert_line ( long * display_id, int * start_row, void * character_string, int * direction, int * rendition_set, int * rendition_complement, int * flags, int * character_set)
{
    struct dsc$descriptor * dsc = character_string;
    struct virtual_display * smg = *display_id;
    // replace later
    int x = 1;
    int y = *start_row;
    x--;
    y--;
    char *buf = smg->smg$t_buffer + x + y * smg->smg$l_number_of_columns;
    char c[2048];
    int lines = 1;
    memcpy (c, buf, (smg->smg$l_number_of_rows - y - lines) * smg->smg$l_number_of_columns);
    memcpy (buf, dsc->dsc$a_pointer, dsc->dsc$w_length);
    memset (buf + dsc->dsc$w_length, 0, smg->smg$l_number_of_columns - dsc->dsc$w_length);
    memcpy (buf, buf + lines * smg->smg$l_number_of_columns, (smg->smg$l_number_of_rows - y - lines) * smg->smg$l_number_of_columns);
    return SS$_NORMAL;
}

int smg$delete_viewport (long * display_id)
{
    struct virtual_display * v = *display_id;
    v->smg$l_viewport = 0; // check
    return SS$_NORMAL;
}

int smg$begin_display_update (long * display_id)
{
    return SS$_NORMAL;
}

int smg$end_display_update (long * display_id)
{
    struct virtual_display * smg = *display_id;
    return SS$_NORMAL;
}

// check connection to paste.
int smg$erase_chars ( long * display_id, int * number_of_characters, int * start_row, int * start_column)
{
    struct virtual_display * smg = *display_id;
    // replace later
    int x = smg->smg$l_x;
    int y = smg->smg$l_y;
    if (start_column)
        x = *start_column;
    if (start_row)
        y= *start_row;
    x--;
    y--;
    char *buf = smg->smg$t_buffer + x + y * smg->smg$l_number_of_columns;
    memset (buf, 32, *number_of_characters);
    return SS$_NORMAL;
}

// check connection to paste.
#undef smg$erase_line
int smg$erase_line ( long * display_id, int * start_row, int * number_of_rows)
{
    struct virtual_display * smg = *display_id;
    // replace later
    int x = 1;
    int y = *start_row;
    int lines = 1;
    if (number_of_rows)
        lines = *number_of_rows;
    x--;
    y--;
    char *buf = smg->smg$t_buffer + x + y * smg->smg$l_number_of_columns;
    memset (buf, 32, lines * smg->smg$l_number_of_columns);
    return SS$_NORMAL;
}

// check connection to paste.
#undef smg$erase_display
int smg$erase_display ( long * display_id, int * start_row, int * start_column, int * end_row, int * end_column)
{
    struct virtual_display * smg = *display_id;
    // replace later
    int x = 1;
    int y = 1;
    if (start_column)
        x = *start_column;
    if (start_row)
        y= *start_row;
    int x2 = 1;
    int y2 = 1;
    if (end_column)
        x2 = *end_column;
    if (end_row)
        y2= *end_row;
    x--;
    y--;
    char *buf = smg->smg$t_buffer + x + y * smg->smg$l_number_of_columns;
    int size = (x2 - x) + (y2 - y) * smg->smg$l_number_of_columns;
    memset (buf, 32, size);
    return SS$_NORMAL;
}

#undef smg$label_border
int smg$label_border (long * display_id, void * text, int * position_code, int * units, int * rendition_set, int * rendition_complement, int * character_set)
{
    return SS$_NORMAL;
}

#undef smg$put_help_text
int smg$put_help_text (long * display_id , long * keyboard_id, void * help_topic, int * help_library, int * rendition_set, int * rendition_complement)
{
    return SS$_NORMAL;
}

int smg$unpaste_virtual_display (long * display_id, long * pasteboard_id)
{
    return SS$_NORMAL;
}

int smg$delete_virtual_display (long * display_id)
{
    free(*display_id);
    return SS$_NORMAL;
}

#undef smg$save_physical_screen
int smg$save_physical_screen (long * pasteboard_id, long * display_id, int * desired_start_row, int * desired_end_row)
{
    return SS$_NORMAL;
}

int smg$restore_physical_screen (long * pasteboard_id, long * display_id)
{
    return SS$_NORMAL;
}

int smg$ring_bell (long * display_id)
{
    char c = 8;
    write (1, &c, 1);
    return SS$_NORMAL;
}

int smg$repaint_screen (long * pasteboard_id)
{
    return SS$_NORMAL;
}

int smg$$set_display_pasteboard(long * display_id, long * pasteboard_id)
{
    struct virtual_display * smg = display_id;
    smg->smg$l_pasteboard = pasteboard_id;
}
