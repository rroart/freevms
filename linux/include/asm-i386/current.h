// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004.

#ifndef _I386_CURRENT_H
#define _I386_CURRENT_H

#include <system_data_cells.h>

struct task_struct;
//struct _cpu;
#ifdef __arch_um__
#ifndef smp_processor_id
#define smp_processor_id() 0
#endif
#endif

static inline struct task_struct * old_get_current(void)
{
  struct task_struct *current;
  __asm__("andl %%esp,%0; ":"=r" (current) : "0" (~8191UL));
  return current;
}

static inline struct task_struct * get_current(void)
{
  return ctl$gl_pcb;
}
 
#define current get_current()

// in case it is done very early

static inline struct task_struct * get_cur_task(void)
{
  return ctl$gl_pcb;
}
 
#define cur_task get_cur_task()

#endif /* !(_I386_CURRENT_H) */
