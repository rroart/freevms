/* 
 * Copyright (C) 2001 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include "chan_user.h"
#include "user.h"
#include "user_util.h"

struct pty_chan {
	void (*announce)(char *dev_name, int dev);
	int dev;
	int raw;
	struct termios tt;
};

void *pty_chan_init(char *str, int device, struct chan_opts *opts)
{
	struct pty_chan *data;

	if((data = um_kmalloc(sizeof(*data))) == NULL) return(NULL);
	*data = ((struct pty_chan) { announce : 	opts->announce, 
				     dev : 		device,
				     raw : 		opts->raw });
	return(data);
}

int pts_open(int input, int output, int primary, void *d)
{
	struct pty_chan *data = d;
	int fd;

	if((fd = get_pty()) < 0){
		printk("open_pts : Failed to open pts\n");
		return(-errno);
	}
	if(data->raw){
		tcgetattr(fd, &data->tt);
		raw(fd, 0);
	}
	if(data->announce) (*data->announce)(ptsname(fd), data->dev);
	return(fd);
}

int pty_open(int input, int output, int primary, void *d)
{
	struct pty_chan *data = d;
	int fd;
	char dev[sizeof("/dev/ptyxx\0")] = "/dev/ptyxx";

	fd = getmaster(dev);
	if(fd < 0) return(-errno);
	if(data->raw) raw(fd, 0);
	if(data->announce) (*data->announce)(dev, data->dev);
	return(fd);
}

int pty_console_write(int fd, const char *buf, int n, void *d)
{
	struct pty_chan *data = d;

	return(generic_console_write(fd, buf, n, &data->tt));
}

struct chan_ops pty_ops = {
	init:		pty_chan_init,
	open:		pty_open,
	close:		generic_close,
	read:		generic_read,
	write:		generic_write,
	console_write:	pty_console_write,
	window_size:	generic_window_size,
	free:		generic_free,
	winch:		0,
};

struct chan_ops pts_ops = {
	init:		pty_chan_init,
	open:		pts_open,
	close:		generic_close,
	read:		generic_read,
	write:		generic_write,
	console_write:	pty_console_write,
	window_size:	generic_window_size,
	free:		generic_free,
	winch:		0,
};

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
