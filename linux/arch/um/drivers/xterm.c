/* 
 * Copyright (C) 2001 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <sched.h>
#include "kern_util.h"
#include "user_util.h"
#include "chan_user.h"
#include "user.h"

struct xterm_chan {
	int pid;
	int fd;
	char *title;
	int device;
	int raw;
	struct termios tt;
};

void *xterm_init(char *str, int device, struct chan_opts *opts)
{
	struct xterm_chan *data;

	if((data = malloc(sizeof(*data))) == NULL) return(NULL);
	*data = ((struct xterm_chan) { pid :		-1, 
				       device :		device, 
				       title :		opts->xterm_title,
				       raw : 		opts->raw });
	return(data);
}

struct xterm_info {
	char tty[2];
	int fd;
	int slave;
	int console_num;
	int *pid_out;
	char *title;
	int winch;
};

static void xterm_tramp(void *arg)
{
	struct xterm_info *info;
	int pid;
	char title[256], flag[sizeof("Sxxnn\0")], c;

	info = arg;
	sprintf(flag, "-S%c%c%d", info->tty[0], info->tty[1], info->fd);
	sprintf(title, info->title, info->console_num);
	if((pid = fork()) != 0) *info->pid_out = pid;
	else {
		execlp("xterm", "xterm", flag, "-T", title, NULL);
		printk("execlp of xterm failed - errno = %d\n", errno);
		close(info->fd);
		exit(1);
	}
	close(info->fd);
	while((read(info->slave, &c, sizeof(c)) == sizeof(c)) && (c != '\n')) ;
}

int xterm_open(int input, int output, int primary, void *d)
{
	struct xterm_chan *data = d;
	struct xterm_info info;
	int master, slave;
	char dev[] = "/dev/ptyXX";

	master = getmaster(dev);
	if(master == -1){
		printk("No unused host ptys found\n");
		return(-ENODEV);
	}
	dev[strlen("/dev/")] = 't';
	slave = open(dev, O_RDWR);
	if(slave == -1) return(-errno);
	tcgetattr(slave, &data->tt);
	raw(slave, 0);
	info.tty[0] = dev[strlen("/dev/pty")];
	info.tty[1] = dev[strlen("/dev/ptyX")];
	info.fd = master;
	info.slave = slave;
	info.console_num = data->device;
	info.pid_out = &data->pid;
	info.title = data->title;
	tracing_cb(xterm_tramp, &info);
	tcsetattr(slave, TCSADRAIN, &data->tt);
	if(data->raw) raw(slave, 0);
	data->fd = slave;

	return(slave);
}

void xterm_close(int fd, void *d)
{
	struct xterm_chan *data = d;
	
	if(data->pid != -1) kill(data->pid, SIGKILL);
	close(fd);
}

void xterm_free(void *d)
{
	free(d);
}

int xterm_console_write(int fd, const char *buf, int n, void *d)
{
	struct xterm_chan *data = d;

	return(generic_console_write(fd, buf, n, &data->tt));
}

struct chan_ops xterm_ops = {
	init:		xterm_init,
	open:		xterm_open,
	close:		xterm_close,
	read:		generic_read,
	write:		generic_write,
	console_write:	xterm_console_write,
	window_size:	generic_window_size,
	free:		xterm_free,
	winch:		1,
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
