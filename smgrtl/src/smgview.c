// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iodef.h>
#include <ssdef.h>
#include <smgdef.h>

#include "smg.h"

struct viewport
{
    long smg$l_link;
    int smg$l_viewport_row_start;
    int smg$l_viewport_column_start;
    int smg$l_viewport_number_rows;
    int smg$l_viewport_number_columns;
};

int smg$create_viewport (long * display_id, int * viewport_row_start, int * viewport_column_start, int * viewport_number_rows, int * viewport_number_columns)
{
    struct viewport * v = malloc(sizeof(struct viewport));
    v->smg$l_viewport_row_start = *viewport_row_start;
    v->smg$l_viewport_column_start = *viewport_column_start;
    v->smg$l_viewport_number_rows = *viewport_number_rows;
    v->smg$l_viewport_number_columns = *viewport_number_columns;
    smg$$insert_viewport(*display_id, v);
    return SS$_NORMAL;
}

#undef smg$change_viewport
int smg$change_viewport (long * display_id, int * viewport_row_start, int * viewport_column_start, int * viewport_number_rows, int * viewport_number_columns)
{
    struct viewport * v;
    smg$$get_viewport (*display_id, &v);
    if (viewport_row_start)
        v->smg$l_viewport_row_start = *viewport_row_start;
    if (viewport_column_start)
        v->smg$l_viewport_column_start = *viewport_column_start;
    if (viewport_number_rows)
        v->smg$l_viewport_number_rows = *viewport_number_rows;
    if (viewport_number_columns)
        v->smg$l_viewport_number_columns = *viewport_number_columns;
    return SS$_NORMAL;
}

#undef smg$get_viewport_char
int smg$get_viewport_char (long * display_id, int * viewport_row_start, int * viewport_column_start, int * viewport_number_rows, int * viewport_number_columns)
{
    struct viewport * smg;
    smg$$get_viewport (*display_id, &smg);
    if (viewport_row_start)
        *viewport_row_start = smg->smg$l_viewport_row_start;
    if (viewport_column_start)
        *viewport_column_start = smg->smg$l_viewport_column_start;
    if (viewport_number_rows)
        *viewport_number_rows = smg->smg$l_viewport_number_rows;
    if (viewport_number_columns)
        *viewport_number_columns = smg->smg$l_viewport_number_columns;
    return SS$_NORMAL;
}

#undef smg$scroll_viewport
int smg$scroll_viewport (long * display_id, int * direction, int * count)
{
    struct viewport * smg;
    smg$$get_viewport (*display_id, &smg);
    int dir = SMG$M_UP;
    if (direction)
        dir = *direction;
    int cnt = 1;
    if (count)
        cnt = *count;
    if (dir & SMG$M_UP)
        smg->smg$l_viewport_row_start-=cnt;
    if (dir & SMG$M_DOWN)
        smg->smg$l_viewport_row_start+=cnt;
    if (dir & SMG$M_LEFT)
        smg->smg$l_viewport_column_start-=cnt;
    if (dir & SMG$M_RIGHT)
        smg->smg$l_viewport_column_start+=cnt;
    return SS$_NORMAL;
}
