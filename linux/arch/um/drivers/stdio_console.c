/* 
 * Copyright (C) 2000, 2001 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include "linux/config.h"
#include "linux/posix_types.h"
#include "linux/tty.h"
#include "linux/tty_flip.h"
#include "linux/types.h"
#include "linux/major.h"
#include "linux/kdev_t.h"
#include "linux/console.h"
#include "linux/string.h"
#include "linux/sched.h"
#include "linux/list.h"
#include "linux/init.h"
#include "linux/interrupt.h"
#include "linux/devfs_fs_kernel.h"
#include "asm/current.h"
#include "asm/softirq.h"
#include "asm/hardirq.h"
#include "stdio_console.h"
#include "line.h"
#include "chan_kern.h"
#include "user_util.h"
#include "kern_util.h"
#include "init.h"
#include "2_5compat.h"

#define MAX_TTYS (8)

static struct tty_driver console_driver;
static struct tty_struct *console_table[MAX_TTYS];
static struct termios *console_termios[MAX_TTYS];
static struct termios *console_termios_locked[MAX_TTYS];
static int console_refcount;

static struct chan_ops init_console_ops = {
	init : 		NULL,
	open : 		NULL,
	close :		NULL,
	read : 		NULL,
	write :		NULL,
	console_write :	generic_write,
	window_size :	NULL,
	free : 		NULL,
	winch:		0,
};

static struct chan init_console_chan = {
	list : 		{ },
	primary :	1,
	input :		0,
	output :	1,
	opened :	1,
	fd :		1,
	pri :		INIT_STATIC,
	ops :		&init_console_ops,
	data :		NULL
};

void stdio_announce(char *dev_name, int dev)
{
	printk(KERN_INFO "Virtual console %d assigned device '%s'\n", dev,
	       dev_name);
}

static struct chan_opts opts = {
	announce: 	stdio_announce,
	xterm_title:	"Virtual Console #%d",
	raw:		1,
};

struct line vts[MAX_TTYS] = { LINE_INIT(CONFIG_CON_ZERO_CHAN),
			      [ 1 ... MAX_TTYS - 1 ] = 
			      LINE_INIT(CONFIG_CON_CHAN) };

static int setup_console_irq(int fd, int input, int output, void *data)
{
	return(um_request_irq(CONSOLE_IRQ, fd, line_interrupt, 
			      SA_INTERRUPT | SA_SHIRQ, "console", data));
}

static int open_console(int line, struct tty_struct *tty)
{
	return(line_open(vts, line, tty, setup_console_irq, &opts));
}

static int con_open(struct tty_struct * tty, struct file * filp)
{
	int line, ret;

	line = minor(tty->device) - tty->driver.minor_start;
	ret = open_console(line, tty);
	chan_window_size(&vts[line].chan_list, &tty->winsize.ws_row, 
			 &tty->winsize.ws_col);	
	return(ret);
}

static void con_close(struct tty_struct * tty, struct file * filp)
{
	line_close(vts, minor(tty->device) - tty->driver.minor_start);
}

static int con_write(struct tty_struct * tty, int from_user, 
		     const unsigned char *buf, int count)
{
	int line;

	if(in_interrupt() && tty->stopped) return 0;
	while(tty->stopped) schedule();

	line = minor(tty->device) - tty->driver.minor_start;
	return(write_chan(&vts[line].chan_list, buf, count));
}

static int write_room(struct tty_struct *tty)
{
	return(1024);
}

static void set_termios(struct tty_struct *tty, struct termios * old)
{
}

static int chars_in_buffer(struct tty_struct *tty)
{
	return(0);
}

int stdio_init(void)
{
	int i, err;

	printk(KERN_INFO "Initializing stdio console driver\n");
	memset(&console_driver, 0, sizeof(struct tty_driver));
	console_driver.magic = TTY_DRIVER_MAGIC;
	console_driver.driver_name = "stdio console";
	console_driver.name = "vc/%d";
	console_driver.major = TTY_MAJOR;
	console_driver.minor_start = 0;
	console_driver.num = MAX_TTYS;
	console_driver.type = TTY_DRIVER_TYPE_CONSOLE;
	console_driver.subtype = SYSTEM_TYPE_CONSOLE;
	console_driver.init_termios = tty_std_termios;
	console_driver.flags = TTY_DRIVER_REAL_RAW;
	console_driver.refcount = &console_refcount;
	console_driver.table = console_table;
	console_driver.termios = console_termios;
	console_driver.termios_locked = console_termios_locked;

	console_driver.open = con_open;
	console_driver.close = con_close;
	console_driver.write = con_write;
	console_driver.put_char = NULL;
	console_driver.flush_chars = NULL;
	console_driver.write_room = write_room;
	console_driver.chars_in_buffer = chars_in_buffer;
	console_driver.flush_buffer = NULL;
	console_driver.ioctl = NULL;
	console_driver.throttle = NULL;
	console_driver.unthrottle = NULL;
	console_driver.send_xchar = NULL;
	console_driver.set_termios = set_termios;
	console_driver.stop = NULL;
	console_driver.start = NULL;
	console_driver.hangup = NULL;
	console_driver.break_ctl = NULL;
	console_driver.wait_until_sent = NULL;
	console_driver.read_proc = NULL;
	if (tty_register_driver(&console_driver))
		panic("Couldn't register console driver\n");

	err = devfs_mk_symlink(NULL, "ttys", 0, "vc", NULL, NULL);
	if(err) printk("Symlink creation from /dev/ttys to /dev/vc "
		       "returned %d\n", err);

	for(i = 0; i < sizeof(vts)/sizeof(vts[0]); i++){
		if(!vts[i].valid) 
			tty_unregister_devfs(&console_driver, 
					     console_driver.minor_start + i);
	}

	for(i = 0; i < sizeof(vts)/sizeof(vts[0]); i++){
		INIT_LIST_HEAD(&vts[i].chan_list);
		sema_init(&vts[i].sem, 1);
	}
	
	open_console(0, NULL);
	return(0);
}

__initcall(stdio_init);

static void console_write(struct console *console, const char *string, 
			  unsigned len)
{
	console_write_chan(&vts[console->index].chan_list, string, len);
}

static kdev_t console_device(struct console *c)
{
	return mk_kdev(TTY_MAJOR, c->index);
}

static int console_setup(struct console *co, char *options)
{
	return(0);
}

static struct console stdiocons = INIT_CONSOLE("tty", console_write, 
					       console_device, console_setup,
					       CON_PRINTBUFFER);

void stdio_console_init(void)
{
	INIT_LIST_HEAD(&vts[0].chan_list);
	list_add(&init_console_chan.list, &vts[0].chan_list);
	register_console(&stdiocons);
}

static int console_chan_setup(char *str)
{
	line_setup(vts, sizeof(vts)/sizeof(vts[0]), str);
	return(1);
}

__setup("con", console_chan_setup);
__channel_help(console_chan_setup, "con");

static void console_exit(void)
{
	int i;

	line_close(vts, 0);
	for(i=0;i<sizeof(vts)/sizeof(vts[0]);i++){
		close_chan(&vts[i].chan_list);
	}
}

__uml_exitcall(console_exit);

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
