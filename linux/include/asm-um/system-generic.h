#ifndef __UM_SYSTEM_GENERIC_H
#define __UM_SYSTEM_GENERIC_H

#include "asm/arch/system.h"

#undef prepare_to_switch
#undef switch_to
#undef __save_flags
#undef save_flags
#undef __restore_flags
#undef restore_flags
#undef __cli
#undef __sti
#undef cli
#undef sti
#undef local_irq_save
#undef local_irq_restore
#undef local_irq_disable
#undef local_irq_enable

#define prepare_to_switch() do ; while(0)

void *_switch_to(void *prev, void *next);

#define switch_to(prev, next, last) prev = _switch_to(prev, next)

extern int set_signals(int enable);
extern void block_signals(void);
extern void unblock_signals(void);

#define local_irq_save(flags) do { (flags) = set_signals(0); } while(0)

#define local_irq_restore(flags) do { set_signals(flags); } while(0)

#define local_irq_enable() unblock_signals()
#define local_irq_disable() block_signals()

#define __sti() unblock_signals()
#define sti() unblock_signals()
#define __cli() block_signals()
#define cli() block_signals()

#define __save_flags(x) local_irq_save(x)
#define save_flags(x) __save_flags(x)

#define __restore_flags(x) local_irq_restore(x)
#define restore_flags(x) __restore_flags(x)

#endif
