/*
 * Copyright (C) 2001 Lennert Buytenhek (buytenh@gnu.org) and 
 * James Leu (jleu@mindspring.net).
 * Copyright (C) 2001 by various other people who didn't put their name here.
 * Licensed under the GPL.
 */

#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <net/if.h>
#include "user.h"
#include "kern_util.h"
#include "net_user.h"
#include "etap.h"

#define MAX_PACKET ETH_MAX_PACKET

void etap_user_init(void *data, void *dev)
{
	struct ethertap_data *pri = data;

	pri->dev = dev;
}

struct etap_open_data {
	char *name;
	char *gate;
	int data_remote;
	int data_me;
	int control_remote;
	int control_me;
	int err;
	int output_len;
	char *output;
};

struct addr_change {
	enum { ADD_ADDR, DEL_ADDR } what;
	unsigned char addr[4];
	unsigned char netmask[4];
};

static void etap_change(int op, unsigned char *addr, unsigned char *netmask,
			int fd)
{
	struct addr_change change;
	void *output;

	change.what = op;
	memcpy(change.addr, addr, sizeof(change.addr));
	memcpy(change.netmask, netmask, sizeof(change.netmask));
	if(write(fd, &change, sizeof(change)) != sizeof(change))
		printk("etap_change - request failed, errno = %d\n",
		       errno);
	output = um_kmalloc(page_size());
	if(output == NULL)
		printk("etap_change : Failed to allocate output buffer\n");
	read_output(fd, output, page_size());
	if(output != NULL){
		printk("%s", output);
		kfree(output);
	}
}

static void etap_open_addr(unsigned char *addr, unsigned char *netmask,
			   void *arg)
{
	etap_change(ADD_ADDR, addr, netmask, *((int *) arg));
}

static void etap_close_addr(unsigned char *addr, unsigned char *netmask,
			    void *arg)
{
	etap_change(DEL_ADDR, addr, netmask, *((int *) arg));
}

static void etap_tramp(void *arg)
{
	struct etap_open_data *data = arg;
	int pid, status;
	char version_buf[sizeof("nnnnn\0")];
	char data_fd_buf[sizeof("nnnnnn\0")];
	char gate_buf[sizeof("nnn.nnn.nnn.nnn\0")];
	char *setup_args[] = { "uml_net", version_buf, "ethertap", data->name,
			       data_fd_buf, gate_buf, NULL };
	char *nosetup_args[] = { "uml_net", version_buf, "ethertap", 
				 data->name, data_fd_buf, NULL };
	char **args, c;

	sprintf(data_fd_buf, "%d", data->data_remote);
	sprintf(version_buf, "%d", UML_NET_VERSION);
	if(data->gate != NULL){
		strcpy(gate_buf, data->gate);
		args = setup_args;
	}
	else args = nosetup_args;
	data->err = 0;
	if((pid = fork()) == 0){
		dup2(data->control_remote, 1);
		close(data->data_me);
		close(data->control_me);
		execvp(args[0], args);
		printk("Exec of '%s' failed - errno = %d\n", args[0], errno);
		exit(1);
	}
	else if(pid < 0) data->err = errno;
	close(data->data_remote);
	close(data->control_remote);
	data->output = NULL;
	if(read(data->control_me, &c, sizeof(c)) != sizeof(c)){
		printk("etap_tramp : read of status failed, errno = %d\n",
		       errno);
		data->err = EINVAL;
		return;
	}
	if(c != 1){
		printk("etap_tramp : uml_net failed\n");
		data->err = EINVAL;
		if(waitpid(pid, &status, 0) < 0) data->err = errno;
		else if(!WIFEXITED(status) || (WEXITSTATUS(status) != 1)){
			printk("uml_net didn't exit with status 1\n");
		}
		return;
	}
	read_output(data->control_me, data->output, data->output_len);
}

static int etap_open(void *data)
{
	struct ethertap_data *pri = data;
	struct etap_open_data tap_data;
	int data_fds[2], control_fds[2], err;

	err = tap_open_common(pri->dev, pri->hw_setup, pri->gate_addr);
	if(err) return(err);

	tap_data.name = pri->dev_name;

	if(socketpair(PF_UNIX, SOCK_DGRAM, 0, data_fds) < 0){
		printk("data socketpair failed - errno = %d\n", errno);
		return(-errno);
	}
	tap_data.data_remote = data_fds[1];
	tap_data.data_me = data_fds[0];

	if(socketpair(PF_UNIX, SOCK_STREAM, 0, control_fds) < 0){
		printk("data socketpair failed - errno = %d\n", errno);
		return(-errno);
	}
	tap_data.control_remote = control_fds[1];
	tap_data.control_me = control_fds[0];
	
	tap_data.gate = pri->gate_addr;
	tap_data.output_len = page_size();
	tap_data.output = um_kmalloc(tap_data.output_len);
	if(tap_data.output == NULL)
		printk("etap_open : failed to allocate output buffer\n");
	tracing_cb(etap_tramp, &tap_data);

	if(tap_data.output){
		printk("%s", tap_data.output);
		kfree(tap_data.output);
	}
	if(tap_data.err != 0){
		printk("etap_tramp failed - errno = %d\n", tap_data.err);
		return(-tap_data.err);
	}
	pri->data_fd = data_fds[0];
	pri->control_fd = control_fds[0];
	iter_addresses(pri->dev, etap_open_addr, &pri->control_fd);
	return(data_fds[0]);
}

static void etap_close(int fd, void *data)
{
	struct ethertap_data *pri = data;

	iter_addresses(pri->dev, etap_close_addr, &pri->control_fd);
	close(fd);
	shutdown(pri->data_fd, SHUT_RDWR);
	close(pri->data_fd);
	pri->data_fd = -1;
	close(pri->control_fd);
	pri->control_fd = -1;
}

static int etap_set_mtu(int mtu, void *data)
{
	return(mtu);
}

static void etap_add_addr(unsigned char *addr, unsigned char *netmask,
			  void *data)
{
	struct ethertap_data *pri = data;

	tap_check_mac(&pri->hw_setup, pri->hw_addr, pri->gate_addr, addr, 
		      pri->dev);
	if(pri->control_fd == -1) return;
	etap_open_addr(addr, netmask, &pri->control_fd);
}

static void etap_del_addr(unsigned char *addr, unsigned char *netmask, 
			  void *data)
{
	struct ethertap_data *pri = data;

	if(pri->control_fd == -1) return;
	etap_close_addr(addr, netmask, &pri->control_fd);
}

struct net_user_info ethertap_user_info = {
	init:		etap_user_init,
	open:		etap_open,
	close:	 	etap_close,
	set_mtu:	etap_set_mtu,
	add_address:	etap_add_addr,
	delete_address: etap_del_addr,
	max_packet:	MAX_PACKET - ETH_HEADER_ETHERTAP
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
