/* 
 * Copyright (C) 2001 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef __IRQ_USER_H__
#define __IRQ_USER_H__

extern void sigio_handler(int sig, struct uml_pt_regs *regs);
extern int activate_fd(int irq, int fd, void *dev_id);
extern void free_irq_by_dev(void *dev_id);
extern void free_irq_by_fd(int fd);
extern void reactivate_fd(int fd);
extern void forward_interrupts(int pid);
extern void init_irq_signals(int on_sigstack);
extern void forward_ipi(int fd, int pid);

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
