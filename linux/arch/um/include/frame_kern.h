/* 
 * Copyright (C) 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef __FRAME_KERN_H_
#define __FRAME_KERN_H_

extern int setup_signal_stack_sc(unsigned long stack_top, int sig, 
				 unsigned long handler,
				 void (*restorer)(void), 
				 struct pt_regs *regs, void *context_sc);
extern int setup_signal_stack_si(unsigned long stack_top, int sig, 
				 unsigned long handler,
				 void (*restorer)(void), 
				 struct pt_regs *regs, void *context_sc,
				 siginfo_t *info);

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
