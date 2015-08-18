// $Id$
// $Locker$

// Author. Roar Thron�s.
// Modified Linux source file, 2001-2004.

#ifndef _I386_CURRENT_H
#define _I386_CURRENT_H

#include <system_data_cells.h>

struct task_struct;
//struct _cpu;

static struct task_struct * get_current(void)
{
    return ctl$gl_pcb;
}

#define current get_current()

#endif /* !(_I386_CURRENT_H) */
