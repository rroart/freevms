/* 
 * Copyright (C) 2001 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef __LINE_H__
#define __LINE_H__

#include "linux/list.h"
#include "asm/semaphore.h"
#include "chan_user.h"

struct line {
	char *init_str;
	int init_pri;
	struct list_head chan_list;
	int valid;
	int count;
	struct tty_struct *tty;
	struct semaphore sem;
	int initialized;
};

#define LINE_INIT(str) \
	{ init_str :	str, \
	  init_pri :	INIT_STATIC, \
	  chan_list : 	{ }, \
	  valid :	1, \
	  count :	0, \
	  tty :	NULL, \
	  sem : { } }

extern void line_interrupt(int irq, void *data, struct pt_regs *unused);
extern void line_close(struct line *lines, int n);
extern int line_open(struct line *lines, int n, struct tty_struct *tty, 
		     int (*setup_irq)(int fd, int input, int output, 
				      void *data),
		     struct chan_opts *opts);
extern void line_setup(struct line *lines, int num, char *init);

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
