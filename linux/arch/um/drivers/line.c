/* 
 * Copyright (C) 2001, 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include "linux/sched.h"
#include "linux/slab.h"
#include "asm/irq.h"
#include "chan_kern.h"
#include "irq_user.h"
#include "line.h"
#include "kern.h"
#include "user_util.h"

void line_interrupt(int irq, void *data, struct pt_regs *unused)
{
	struct line *dev = data;

	if(dev->count > 0) chan_interrupt(&dev->chan_list, dev->tty);
}

int line_open(struct line *lines, int n, struct tty_struct *tty, 
	      int (*setup_irq)(int fd, int input, int output, void *data),
	      struct chan_opts *opts)
{
	struct line *line = &lines[n];
	int err = 0;

	down(&line->sem);
	if(line->count == 0){
		if(!line->valid){
			err = -ENODEV;
			goto out;
		}
		if(list_empty(&line->chan_list)){
			err = parse_chan_pair(line->init_str, &line->chan_list,
					      line->init_pri, n, opts);
			if(err) goto out;
			err = open_chan(&line->chan_list);
			if(err) goto out;
		}
		enable_chan(&line->chan_list, setup_irq, line);
	}

	if((tty != NULL) && (line->tty == NULL))
		chan_enable_winch(&line->chan_list, line);

	/* This is outside the if because the initial console is opened
	 * with tty == NULL
	 */
	line->tty = tty;

	if(tty != NULL)	tty->driver_data = line;

	line->count++;
 out:
	up(&line->sem);
	return(err);
}

void line_close(struct line *lines, int n)
{
	struct line *line = &lines[n];

	line->count--;
	if(line->count == 0){
		disable_chan(&line->chan_list);
		line->tty = NULL;
	}
}

void line_setup(struct line *lines, int num, char *init)
{
	int i, n;
	char *end;

	if(*init == '=') n = -1;
	else {
		n = simple_strtoul(init, &end, 0);
		if(*end != '='){
			printk(KERN_ERR "line_setup failed to parse \"%s\"\n", 
			       init);
			return;
		}
		init = end;
	}
	init++;
	if(n == -1){
		for(i = 0; i < num; i++){
			if(lines[i].init_pri <= INIT_ALL){
				lines[i].init_pri = INIT_ALL;
				if(!strcmp(init, "none")) lines[i].valid = 0;
				else lines[i].init_str = init;
			}
		}
	}
	else if(lines[n].init_pri <= INIT_ONE){
		lines[n].init_pri = INIT_ONE;
		if(!strcmp(init, "none")) lines[n].valid = 0;
		else lines[n].init_str = init;
	}
}

struct winch {
	struct list_head list;
	int fd;
	int pid;
	struct line *line;
};

void winch_interrupt(int irq, void *data, struct pt_regs *unused)
{
	struct winch *winch = data;
	struct tty_struct *tty;
	char c;

	c = generic_read(winch->fd, NULL);
	if(c < 0){
		if(c != -EAGAIN){
			printk("winch_interrupt : read failed, errno = %d\n", 
			       -c);
			printk("fd %d is losing SIGWINCH support\n", 
			       winch->fd);
			free_irq_by_fd(winch->fd);
			return;
		}
		goto out;
	}
	tty = winch->line->tty;
	chan_window_size(&winch->line->chan_list, &tty->winsize.ws_row, 
			 &tty->winsize.ws_col);
	kill_pg(tty->pgrp, SIGWINCH, 1);
 out:
	reactivate_fd(winch->fd);
}

struct list_head winch_handlers = LIST_HEAD_INIT(winch_handlers);

void register_winch_irq(int fd, int pid, void *line)
{
	struct winch *winch;

	winch = kmalloc(GFP_KERNEL, sizeof(*winch));
	if(winch == NULL){
		printk("register_winch_irq - kmalloc failed\n");
		return;
	}
	*winch = ((struct winch) { list : 	LIST_HEAD_INIT(winch->list),
				   fd : 	fd,
				   pid : 	pid,
				   line :	line });
	list_add(&winch->list, &winch_handlers);
	if(um_request_irq(WINCH_IRQ, fd, winch_interrupt, 
			  SA_INTERRUPT | SA_SHIRQ, "winch", winch) < 0)
		printk("register_winch_irq - failed to register IRQ\n");
}

static void winch_cleanup(void)
{
	struct list_head *ele;
	struct winch *winch;

	list_for_each(ele, &winch_handlers){
		winch = list_entry(ele, struct winch, list);
		close(winch->fd);
		free_irq_by_fd(winch->fd);
		if(winch->pid != -1) kill_pid(winch->pid);
	}
}

__uml_exitcall(winch_cleanup);

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
