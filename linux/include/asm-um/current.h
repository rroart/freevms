/* 
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef __UM_CURRENT_H
#define __UM_CURRENT_H

#ifndef __ASSEMBLY__

#include "linux/config.h"
#include "asm/page.h"

#include <system_data_cells.h>

struct task_struct;

#define OLD_CURRENT_TASK(dummy) (((unsigned long) dummy) & (PAGE_MASK << 2))

#if 0
#define CURRENT_TASK(dummy) (((unsigned long) &dummy) & (PAGE_MASK << 2))
#endif

#define CURRENT_TASK() (ctl$gl_pcb)

#define current CURRENT_TASK()

#endif /* __ASSEMBLY__ */

#endif

/*
 * Overrides for Emacs so that we follow Linus's tabbing style.
 * Emacs will notice this stuff at the end of the file and automatically
 * adjust the settings for this buffer only.  This must remain at the end
 * of the file.
 * ---------------------------------------------------------------------------
 * Local variables:
 * c-file-style: "linux"
 * End:
 */
