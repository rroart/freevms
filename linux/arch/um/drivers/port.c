/* 
 * Copyright (C) 2001 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include "user_util.h"
#include "kern_util.h"
#include "user.h"
#include "chan_user.h"
#include "port.h"

struct port_chan {
	int fd;
	int raw;
	int socket[2];
	struct termios tt;
	void *kernel_data;
};

void *port_init(char *str, int device, struct chan_opts *opts)
{
	struct port_chan *data;
	void *kern_data;
	char *end;
	int port;

	if(*str != ':'){
		printk("port_init : channel type 'port' must specify a "
		       "port number\n");
		return(NULL);
	}
	str++;
	port = strtoul(str, &end, 0);
	if(*end != '\0'){
		printk("port_init : couldn't parse port '%s'\n", str);
		return(NULL);
	}

	if((kern_data = port_data(port)) == NULL) return(NULL);

	if((data = um_kmalloc(sizeof(*data))) == NULL) return(NULL);
	*data = ((struct port_chan) { fd : 		-1,
				      raw : 		opts->raw,
				      socket : 		{ -1, -1 },
				      kernel_data :	kern_data });
	
	return(data);
}

int port_open(int input, int output, int primary, void *d)
{
	struct port_chan *data = d;
	int fd;

	fd = port_wait(data->kernel_data, data->socket);
	if((fd >= 0) && data->raw){
		tcgetattr(fd, &data->tt);
		raw(fd, 0);
	}
	data->fd = fd;
	return(fd);
}

void port_close(int fd, void *d)
{
	struct port_chan *data = d;

	close(data->fd);
	shutdown(data->socket[0], SHUT_RDWR);
	shutdown(data->socket[1], SHUT_RDWR);
}

int port_console_write(int fd, const char *buf, int n, void *d)
{
	struct port_chan *data = d;

	return(generic_console_write(fd, buf, n, &data->tt));
}

void port_free(void *d)
{
	struct port_chan *data = d;

	port_kern_free(data->kernel_data);
	kfree(data);
}

struct chan_ops port_ops = {
	init:		port_init,
	open:		port_open,
	close:		port_close,
	read:	        generic_read,
	write:		generic_write,
	console_write:	port_console_write,
	window_size:	generic_window_size,
	free:		port_free,
	winch:		1,
};

int port_listen_fd(int port)
{
	struct sockaddr_in addr;
	int fd;

	fd = socket(PF_INET, SOCK_STREAM, 0);
	if(fd == -1) return(-errno);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) 
		return(-errno);
  
	if(listen(fd, 1) < 0) return(-errno);

	return(fd);
}

struct port_connect_data {
	int sock_fd;
	int pipe_fds[2];
	int err;
};

void port_connect_tramp(void *d)
{
	struct port_connect_data *data = d;

	data->err = 0;
	if(fork() == 0){
		dup2(data->sock_fd, 0);
		dup2(data->sock_fd, 1);
		dup2(data->sock_fd, 2);
		close(data->sock_fd);
		dup2(data->pipe_fds[1], 3);
		close(data->pipe_fds[1]);
		execlp("/usr/sbin/in.telnetd", "in.telnetd", "-L", 
		       "/usr/lib/uml/port-helper", NULL);
		shutdown(3, SHUT_RDWR);
		shutdown(data->pipe_fds[0], SHUT_RDWR);
		data->err = errno;
		exit(1);
	}
}

static int rcv_fd(int fd, struct port_connect_data *data)
{
	int new, n;
	char buf[CMSG_SPACE(sizeof(new))];
	struct msghdr msg;
	struct cmsghdr *cmsg;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = NULL;
	msg.msg_iovlen = 0;
	msg.msg_control = buf;
	msg.msg_controllen = sizeof(buf);
	msg.msg_flags = 0;

	n = recvmsg(fd, &msg, 0);
	if(n < 0){
		printk("rcv_fd : recvmsg failed - errno = %d\n", errno);
		return(-1);
	}
  
	cmsg = CMSG_FIRSTHDR(&msg);
	if(cmsg == NULL){
		printk("rcv_fd didn't receive anything, error = %d\n", 
		       data->err);
		return(-1);
	}
	if((cmsg->cmsg_level != SOL_SOCKET) || 
	   (cmsg->cmsg_type != SCM_RIGHTS)){
		printk("rcv_fd didn't receive a descriptor\n");
		return(-1);
	}

	new = ((int *) CMSG_DATA(cmsg))[0];
	return(new);
}

int port_connection(int fd, int *socket)
{
	int new;
	struct port_connect_data data;

	if((new = accept(fd, NULL, 0)) < 0) return(-errno);

	if(socketpair(PF_UNIX, SOCK_DGRAM, 0, socket) < 0) return(-errno);

	data = ((struct port_connect_data)
		{ sock_fd : 		new,
		  pipe_fds :		{ socket[0], socket[1] } });

	tracing_cb(port_connect_tramp, &data);
	return(rcv_fd(socket[0], &data));
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
