/* 
 * Copyright (C) 2001, 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include "net_user.h"
#include "tuntap.h"
#include "kern_util.h"
#include "user.h"

#define MAX_PACKET ETH_MAX_PACKET

void tuntap_user_init(void *data, void *dev)
{
	struct tuntap_data *pri = data;

	pri->dev = dev;
}

struct tuntap_open_data {
	char *name;
	char *gate;
	int data_fd;
	int remote;
	int me;
	int err;
	char *buffer;
	int len;
	int used;
};

static void tuntap_open_tramp(void *arg)
{
	struct tuntap_open_data *data = arg;
	char version_buf[sizeof("nnnnn\0")];
	char *args[] = { "uml_net", version_buf, "tuntap", "up", data->gate,
			 NULL };
	char buf[CMSG_SPACE(sizeof(data->data_fd))];
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct iovec iov;
	int pid, n;

	sprintf(version_buf, "%d", UML_NET_VERSION);
	data->err = 0;
	if((pid = fork()) == 0){
		dup2(data->remote, 1);
		close(data->me);
		execvp(args[0], args);
		printk("Exec of '%s' failed - errno = %d\n", args[0], errno);
		exit(1);		
	}	
	else if(pid < 0) data->err = errno;
	close(data->remote);

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	if(data->buffer != NULL){
		iov = ((struct iovec) { data->buffer, data->len });
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
	}
	else {
		msg.msg_iov = NULL;
		msg.msg_iovlen = 0;
	}
	msg.msg_control = buf;
	msg.msg_controllen = sizeof(buf);
	msg.msg_flags = 0;
	n = recvmsg(data->me, &msg, 0);
	data->used = n;
	if(n < 0){
		printk("tuntap_open_tramp : recvmsg failed - errno = %d\n", 
		       errno);
		data->err = errno;
		return;
	}
	waitpid(pid, NULL, 0);

	cmsg = CMSG_FIRSTHDR(&msg);
	if(cmsg == NULL){
		printk("tuntap_open_tramp : didn't receive a message\n");
		data->err = EINVAL;
		return;
	}
	if((cmsg->cmsg_level != SOL_SOCKET) || 
	   (cmsg->cmsg_type != SCM_RIGHTS)){
		printk("tuntap_open_tramp : didn't receive a descriptor\n");
		data->err = EINVAL;
		return;
	}
	data->data_fd = ((int *) CMSG_DATA(cmsg))[0];
}

static void tuntap_add_addr(unsigned char *addr, unsigned char *netmask,
			    void *data)
{
	struct tuntap_data *pri = data;

	tap_check_mac(&pri->hw_setup, pri->hw_addr, pri->gate_addr, addr, 
		      pri->dev);
	if((pri->fd == -1) || pri->fixed_config) return;
	open_addr(addr, netmask, pri->dev_name);
}

static void tuntap_del_addr(unsigned char *addr, unsigned char *netmask,
			    void *data)
{
	struct tuntap_data *pri = data;

	if((pri->fd == -1) || pri->fixed_config) return;
	close_addr(addr, netmask, pri->dev_name);
}

static int tuntap_open(void *data)
{
	struct ifreq ifr;
	struct tuntap_data *pri = data;
	struct tuntap_open_data tap_data;
	char *output;
	int err, fds[2];

	err = tap_open_common(pri->dev, pri->hw_setup, pri->gate_addr);
	if(err) return(err);

	if(pri->fixed_config){
		if((pri->fd = open("/dev/net/tun", O_RDWR)) < 0){
			printk("Failed to open /dev/net/tun, errno = %d\n",
			       errno);
			return(-errno);
		}
		memset(&ifr, 0, sizeof(ifr));
		ifr.ifr_flags = IFF_TAP;
		strncpy(ifr.ifr_name, pri->dev_name, sizeof(ifr.ifr_name) - 1);
		if(ioctl(pri->fd, TUNSETIFF, (void *) &ifr) < 0){
			printk("TUNSETIFF failed, errno = %d", errno);
			close(pri->fd);
			return(-errno);
		}
	}
	else {
		if(socketpair(PF_UNIX, SOCK_DGRAM, 0, fds) < 0){
			printk("data socketpair failed - errno = %d\n", errno);
			return(-errno);
		}

		tap_data.me = fds[0];
		tap_data.remote = fds[1];
		tap_data.data_fd = -1;
		tap_data.gate = pri->gate_addr;
		tap_data.buffer = get_output_buffer(&tap_data.len);
		if(tap_data.buffer != NULL) tap_data.len--;
		tap_data.used = 0;

		tracing_cb(tuntap_open_tramp, &tap_data);
		output = tap_data.buffer;
		if(tap_data.err == 0){
			pri->dev_name = uml_strdup(tap_data.buffer);
			output += IFNAMSIZ;
			printk(output);
			free_output_buffer(tap_data.buffer);
		}
		else {
			printk(output);
			free_output_buffer(tap_data.buffer);
			printk("tuntap_open_tramp failed - errno = %d\n", 
			       tap_data.err);
			return(-tap_data.err);
		}
		close(fds[0]);
		pri->fd = tap_data.data_fd;
		iter_addresses(pri->dev, open_addr, pri->dev_name);
	}

	return(pri->fd);
}

static void tuntap_close(int fd, void *data)
{
	struct tuntap_data *pri = data;

	if(!pri->fixed_config) 
		iter_addresses(pri->dev, close_addr, pri->dev_name);
	close(fd);
	pri->fd = -1;
}

static int tuntap_set_mtu(int mtu, void *data)
{
	return(mtu);
}

struct net_user_info tuntap_user_info = {
	init:		tuntap_user_init,
	open:		tuntap_open,
	close:	 	tuntap_close,
	set_mtu:	tuntap_set_mtu,
	add_address:	tuntap_add_addr,
	delete_address: tuntap_del_addr,
	max_packet:	MAX_PACKET
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
