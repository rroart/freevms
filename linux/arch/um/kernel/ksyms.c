#include "linux/module.h"
#include "linux/string.h"
#include "asm/current.h"
#include "asm/delay.h"
#include "asm/processor.h"
#include "asm/unistd.h"
#include "asm/pgalloc.h"
#include "asm/page.h"
#include "kern_util.h"
#include "user_util.h"

EXPORT_SYMBOL(stop);
EXPORT_SYMBOL(strtok);
EXPORT_SYMBOL(uml_physmem);
EXPORT_SYMBOL(set_signals);
EXPORT_SYMBOL(kernel_thread);
EXPORT_SYMBOL(__const_udelay);
EXPORT_SYMBOL(sys_waitpid);
EXPORT_SYMBOL(task_size);
EXPORT_SYMBOL(__do_copy_from_user);
EXPORT_SYMBOL(__do_strncpy_from_user);
EXPORT_SYMBOL(flush_tlb_range);
EXPORT_SYMBOL(__do_clear_user);
EXPORT_SYMBOL(honeypot);
EXPORT_SYMBOL(host_task_size);
EXPORT_SYMBOL(arch_validate);

/* This is here because UML expands open to sys_open, not to a system
 * call instruction.
 */
EXPORT_SYMBOL(sys_open);
