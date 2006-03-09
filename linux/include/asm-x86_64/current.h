#ifndef _X86_64_CURRENT_H
#define _X86_64_CURRENT_H

#if !defined(__ASSEMBLY__) 
struct task_struct;

#include <asm/pda.h>

#include <system_data_cells.h>

static inline struct task_struct *get_current(void) 
{ 
	return ctl$gl_pcb; // check
	struct task_struct *t = read_pda(pcurrent); 
	return t;
} 


#if 0
// not yet. smp
static inline struct task_struct *stack_current(void)
{
	// check
	struct task_struct *current;
	__asm__("andq %%rsp,%0; ":"=r" (current) 
		: "0" (~(unsigned long)(THREAD_SIZE-1)));
	return current;
}
#endif


#define current get_current()

#else

#ifndef ASM_OFFSET_H
#include <asm/offset.h> 
#endif

#if 0
#define GET_CURRENT(reg) movq %gs:(pda_pcurrent),reg
#else
#define GET_CURRENT(reg) movq 0x7ffff000,reg 
// check
#endif

#endif

#endif /* !(_X86_64_CURRENT_H) */
