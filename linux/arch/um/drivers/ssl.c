/* 
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include "linux/config.h"
#include "linux/fs.h"
#include "linux/tty.h"
#include "linux/tty_driver.h"
#include "linux/major.h"
#include "linux/mm.h"
#include "linux/init.h"
#include "linux/devfs_fs_kernel.h"
#include "asm/termbits.h"
#include "asm/irq.h"
#include "line.h"
#include "ssl.h"
#include "chan_kern.h"
#include "user_util.h"
#include "kern_util.h"
#include "kern.h"
#include "init.h"
#include "2_5compat.h"

static int ssl_version = 1;

static struct tty_driver ssl_driver;

static int ssl_refcount = 0;

#define NR_PORTS 64

static struct tty_struct *ssl_table[NR_PORTS];
static struct termios *ssl_termios[NR_PORTS];
static struct termios *ssl_termios_locked[NR_PORTS];

void ssl_announce(char *dev_name, int dev)
{
	printk(KERN_INFO "Serial line %d assigned device '%s'\n", dev,
	       dev_name);
}

static struct chan_opts opts = {
	announce: 	ssl_announce,
	xterm_title:	"Serial Line #%d",
	raw:		1,
};

static struct line serial_lines[NR_PORTS] = 
	{ [0 ... NR_PORTS - 1] = LINE_INIT(CONFIG_SSL_CHAN) };

static int setup_ssl_irq(int fd, int input, int output, void *data)
{
	return(um_request_irq(SSL_IRQ, fd, line_interrupt, 
			      SA_INTERRUPT | SA_SHIRQ, "ssl", data));
}

int ssl_open(struct tty_struct *tty, struct file *filp)
{
	int line;

	line = minor(tty->device) - tty->driver.minor_start;
	if ((line < 0) || (line >= NR_PORTS))
		return -ENODEV;
	return(line_open(serial_lines, line, tty, setup_ssl_irq, &opts));
}

static void ssl_close(struct tty_struct *tty, struct file * filp)
{
	line_close(serial_lines, minor(tty->device) - tty->driver.minor_start);
}

static int ssl_write(struct tty_struct * tty, int from_user,
		     const unsigned char *buf, int count)
{
	int line;

	line = minor(tty->device) - tty->driver.minor_start;
	if ((line < 0) || (line >= NR_PORTS))
		panic("Bad tty in ssl_put_char");
	return(write_chan(&serial_lines[line].chan_list, buf, count));
}

static void ssl_put_char(struct tty_struct *tty, unsigned char ch)
{
	int line;

	line = minor(tty->device) - tty->driver.minor_start;
	if ((line < 0) || (line >= NR_PORTS))
		panic("Bad tty in ssl_put_char");
	write_chan(&serial_lines[line].chan_list, &ch, sizeof(ch));
}

static void ssl_flush_chars(struct tty_struct *tty)
{
	return;
}

static int ssl_write_room(struct tty_struct *tty)
{
	return(16384);
}

static int ssl_chars_in_buffer(struct tty_struct *tty)
{
	return(0);
}

static void ssl_flush_buffer(struct tty_struct *tty)
{
	return;
}

static int ssl_ioctl(struct tty_struct *tty, struct file * file,
		     unsigned int cmd, unsigned long arg)
{
	int ret;

	ret = 0;
	switch(cmd){
	case TCGETS:
	case TCSETS:
	case TCFLSH:
	case TCSETSF:
	case TCSETSW:
	case TCGETA:
	case TIOCMGET:
		ret = -ENOIOCTLCMD;
		break;
	default:
		printk(KERN_ERR 
		       "Unimplemented ioctl in ssl_ioctl : 0x%x\n", cmd);
		ret = -ENOIOCTLCMD;
		break;
	}
	return(ret);
}

static void ssl_throttle(struct tty_struct * tty)
{
	printk(KERN_ERR "Someone should implement ssl_throttle\n");
}

static void ssl_unthrottle(struct tty_struct * tty)
{
	printk(KERN_ERR "Someone should implement ssl_unthrottle\n");
}

static void ssl_set_termios(struct tty_struct *tty, 
			    struct termios *old_termios)
{
}

static void ssl_stop(struct tty_struct *tty)
{
	printk(KERN_ERR "Someone should implement ssl_stop\n");
}

static void ssl_start(struct tty_struct *tty)
{
	printk(KERN_ERR "Someone should implement ssl_start\n");
}

void ssl_hangup(struct tty_struct *tty)
{
}

int ssl_init(void)
{
	int i, err;

	printk(KERN_INFO "Initializing software serial port version %d\n", 
	       ssl_version);
  
	/* Initialize the tty_driver structure */
	
	memset(&ssl_driver, 0, sizeof(struct tty_driver));
	ssl_driver.magic = TTY_DRIVER_MAGIC;
	ssl_driver.name = "tts/%d";
	ssl_driver.major = TTYAUX_MAJOR;
	ssl_driver.minor_start = 64;
	ssl_driver.num = NR_PORTS;
	ssl_driver.type = TTY_DRIVER_TYPE_SERIAL;
	ssl_driver.subtype = 0;
	ssl_driver.init_termios = tty_std_termios;
	ssl_driver.init_termios.c_cflag =
		B9600 | CS8 | CREAD | HUPCL | CLOCAL;
	ssl_driver.flags = TTY_DRIVER_REAL_RAW;
	ssl_driver.refcount = &ssl_refcount;
	ssl_driver.table = ssl_table;
	ssl_driver.termios = ssl_termios;
	ssl_driver.termios_locked = ssl_termios_locked;

	ssl_driver.open = ssl_open;
	ssl_driver.close = ssl_close;
	ssl_driver.write = ssl_write;
	ssl_driver.put_char = ssl_put_char;
	ssl_driver.flush_chars = ssl_flush_chars;
	ssl_driver.write_room = ssl_write_room;
	ssl_driver.chars_in_buffer = ssl_chars_in_buffer;
	ssl_driver.flush_buffer = ssl_flush_buffer;
	ssl_driver.ioctl = ssl_ioctl;
	ssl_driver.throttle = ssl_throttle;
	ssl_driver.unthrottle = ssl_unthrottle;
	ssl_driver.set_termios = ssl_set_termios;
	ssl_driver.stop = ssl_stop;
	ssl_driver.start = ssl_start;
	ssl_driver.hangup = ssl_hangup;
	if (tty_register_driver(&ssl_driver))
		panic("Couldn't register ssl driver\n");

	err = devfs_mk_symlink(NULL, "serial", 0, "tts", NULL, NULL);
	if(err) printk("Symlink creation from /dev/serial to /dev/tts "
		       "returned %d\n", err);

	for(i = 0; i < sizeof(serial_lines)/sizeof(serial_lines[0]); i++){
		if(!serial_lines[i].valid) 
			tty_unregister_devfs(&ssl_driver, 
					     ssl_driver.minor_start + i);
	}

	for(i = 0; i < sizeof(serial_lines)/sizeof(serial_lines[0]); i++){
		INIT_LIST_HEAD(&serial_lines[i].chan_list);
		sema_init(&serial_lines[i].sem, 1);
	}

	return(0);
}

__initcall(ssl_init);

static int ssl_chan_setup(char *str)
{
	line_setup(serial_lines, sizeof(serial_lines)/sizeof(serial_lines[0]),
		   str);
	return(1);
}

__setup("ssl", ssl_chan_setup);
__channel_help(ssl_chan_setup, "ssl");

static void ssl_exit(void)
{
	int i;

	for(i=0;i<sizeof(serial_lines)/sizeof(serial_lines[0]);i++){
		close_chan(&serial_lines[i].chan_list);
	}
}

__uml_exitcall(ssl_exit);

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
