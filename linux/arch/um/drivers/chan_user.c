/* 
 * Copyright (C) 2000, 2001 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include "kern_util.h"
#include "user_util.h"
#include "chan_user.h"
#include "user.h"

void generic_close(int fd, void *unused)
{
	close(fd);
}

int generic_read(int fd, void *unused)
{
	int n;
	char c;

	n = read(fd, &c, sizeof(c));
	if(n < 0){
		if(errno == EAGAIN) return(0);
		return(-errno);
	}
	else if(n == 0) return(-EIO);
	return(c);
}

int generic_write(int fd, const char *buf, int n, void *unused)
{
	return(write(fd, buf, n));
}

int generic_console_write(int fd, const char *buf, int n, void *state)
{
	struct termios save, *orig = state;
	int err;

	if(isatty(fd)){
		tcgetattr(fd, &save);
		tcsetattr(fd, TCSADRAIN, orig);
	}
	err = generic_write(fd, buf, n, NULL);
	if(isatty(fd)) tcsetattr(fd, TCSADRAIN, &save);
	return(err);
}

int generic_window_size(int fd, void *unused, unsigned short *rows_out,
			unsigned short *cols_out)
{
	struct winsize size;
	int ret = 0;

	if(ioctl(fd, TIOCGWINSZ, &size) == 0){
		ret = ((*rows_out != size.ws_row) || 
		       (*cols_out != size.ws_col));
		*rows_out = size.ws_row;
		*cols_out = size.ws_col;
	}
	return(ret);
}

void generic_free(void *data)
{
	kfree(data);
}

int getmaster(char *line)
{
	struct stat stb;
	char *pty, *bank, *cp;
	int master;

	pty = &line[strlen("/dev/ptyp")];
	for (bank = "pqrs"; *bank; bank++) {
		line[strlen("/dev/pty")] = *bank;
		*pty = '0';
		if (stat(line, &stb) < 0)
			break;
		for (cp = "0123456789abcdef"; *cp; cp++) {
			*pty = *cp;
			master = open(line, O_RDWR);
			if (master >= 0) {
				char *tp = &line[strlen("/dev/")];
				int ok;

				/* verify slave side is usable */
				*tp = 't';
				ok = access(line, R_OK|W_OK) == 0;
				*tp = 'p';
				if (ok) return(master);
				(void) close(master);
			}
		}
	}
	return(-1);
}

static void winch_handler(int sig)
{
}

static int winch_thread(int pty_fd, int pipe_fd)
{
	sigset_t sigs;
	char c = 1;

	if(write(pipe_fd, &c, sizeof(c)) != sizeof(c))
		printk("winch_thread : failed to write synchronization "
		       "byte\n");

	signal(SIGWINCH, winch_handler);
	sigfillset(&sigs);
	sigdelset(&sigs, SIGWINCH);
	if(sigprocmask(SIG_SETMASK, &sigs, NULL) < 0){
		printk("winch_thread : sigprocmask failed, errno = %d\n", 
		       errno);
		exit(1);
	}

	if(setsid() < 0){
		printk("winch_thread : setsid failed, errno = %d\n", errno);
		exit(1);
	}

	if(ioctl(pty_fd, TIOCSCTTY, 0) < 0){
		printk("winch_thread : TIOCSCTTY failed, errno = %d\n", errno);
		exit(1);
	}
	if(tcsetpgrp(pty_fd, getpid()) < 0){
		printk("winch_thread : tcsetpgrp failed, errno = %d\n", errno);
		exit(1);
	}

	if(read(pipe_fd, &c, sizeof(c)) != sizeof(c))
		printk("winch_thread : failed to read synchronization byte\n");

	while(1){
		pause();

		if(write(pipe_fd, &c, sizeof(c)) != sizeof(c)){
			printk("winch_thread : write failed, errno = %d\n",
			       errno);
		}
	}
}

static int tracer_winch[2];

static void tracer_winch_handler(int sig)
{
	char c = 1;

	if(write(tracer_winch[1], &c, sizeof(c)) != sizeof(c))
		printk("tracer_winch_handler - write failed, errno = %d\n",
		       errno);
}

void setup_tracer_winch(void)
{
	if(socketpair(AF_UNIX, SOCK_STREAM, 0, tracer_winch) < 0){
		printk("winch_tramp : socketpair failed, errno = %d\n", errno);
		return;
	}
	signal(SIGWINCH, tracer_winch_handler);
}

struct winch_data {
	int fd;
	void *device_data;
	int fd_out;
	int pid_out;
};

static void winch_tramp(void *arg)
{
	struct winch_data *data = arg;
	int fds[2], pid, n;
	char c;

	if(socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0){
		printk("winch_tramp : socketpair failed, errno = %d\n", errno);
		return;
	}
	pid = fork();
	if(pid == 0){
		close(fds[0]);
		winch_thread(data->fd, fds[1]);
	}
	else if(pid < 0){
		printk("fork of winch_thread failed - errno = %d\n", errno);
		return;
	}
	close(fds[1]);
	data->fd_out = fds[0];
	data->pid_out = pid;
	n = read(fds[0], &c, sizeof(c));
	if(n != sizeof(c)){
		printk("winch_tramp : failed to read synchronization byte\n");
		printk("read returned %d, errno = %d\n", n, errno);
		printk("fd %d will not support SIGWINCH\n", data->fd);
		data->fd_out = -1;
	}
}

void register_winch(int fd, void *device_data)
{
	struct winch_data data;
	int pid;
	char c = 1;

	data = ((struct winch_data) { fd :		fd,
				      device_data : 	device_data,
				      fd_out :		-1,
				      pid_out :		-1 });

	pid = tcgetpgrp(fd);
	if(pid == tracing_pid)
		register_winch_irq(tracer_winch[0], -1, device_data);
	else if(pid == -1){
		tracing_cb(winch_tramp, &data);
		if(data.fd_out != -1){
			register_winch_irq(data.fd_out, data.pid_out, 
					   data.device_data);

			if(write(data.fd_out, &c, sizeof(c)) != sizeof(c))
				printk("register_winch : failed to write "
				       "synchronization byte\n");
		}
	}
}

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
