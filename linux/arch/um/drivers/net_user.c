/* 
 * Copyright (C) 2001, 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "user.h"
#include "user_util.h"
#include "kern_util.h"
#include "net_user.h"

int tap_open_common(void *dev, int hw_setup, char *gate_addr)
{
	int tap_addr[4];

	if(gate_addr == NULL) return(0);
	if(sscanf(gate_addr, "%d.%d.%d.%d", &tap_addr[0], 
		  &tap_addr[1], &tap_addr[2], &tap_addr[3]) != 4){
		printk("Invalid tap IP address - '%s'\n", 
		       gate_addr);
		return(-EINVAL);
	}
	return(0);
}

void tap_check_mac(int *setup, char *mac, char *gate_addr, char *eth_addr,
		   void *dev)
{
	int tap_addr[4];

	if(*setup) return;
	if((gate_addr != NULL) && 
	   (sscanf(gate_addr, "%d.%d.%d.%d", &tap_addr[0], 
		   &tap_addr[1], &tap_addr[2], &tap_addr[3]) == 4) &&
	   (eth_addr[0] == tap_addr[0]) && 
	   (eth_addr[1] == tap_addr[1]) && 
	   (eth_addr[2] == tap_addr[2]) && 
	   (eth_addr[3] == tap_addr[3])){
		printk("The tap IP address and the UML eth IP address"
		       " must be different\n");
	}
	memcpy(&mac[2], eth_addr, 4);
	set_ether_mac(dev, mac);
	*setup = 1;
}

void read_output(int fd, char *output, int len)
{
	int remain, n, actual;
	char c;

	if(output == NULL){
		output = &c;
		len = sizeof(c);
	}
		
	*output = '\0';
	if(read(fd, &remain, sizeof(remain)) != sizeof(remain)){
		printk("read_output - read of length failed, errno = %d\n",
		       errno);
		return;
	}

	while(remain != 0){
		n = (remain < len) ? remain : len;
		actual = read(fd, output, n);
		if(actual != n){
			printk("read_output - read of data failed, "
			       "errno = %d\n", errno);
			return;
		}
		remain -= actual;
	}
	return;
}

int net_read(int fd, void *buf, int len)
{
	int n;

	while(((n = read(fd,  buf,  len)) < 0) && (errno == EINTR)) ;

	if(n < 0){
		if(errno == EAGAIN) return(0);
		return(-errno);
	}
	else if(n == 0) return(-ENOTCONN);
	return(n);
}

int net_recvfrom(int fd, void *buf, int len)
{
	int n;

	while(((n = recvfrom(fd,  buf,  len, 0, NULL, NULL)) < 0) && 
	      (errno == EINTR)) ;

	if(n < 0){
		if(errno == EAGAIN) return(0);
		return(-errno);
	}
	else if(n == 0) return(-ENOTCONN);
	return(n);
}

int net_write(int fd, void *buf, int len)
{
	int n;

	while(((n = write(fd, buf, len)) < 0) && (errno == EINTR)) ;
	if(n < 0){
		if(errno == EAGAIN) return(0);
		return(-errno);
	}
	else if(n == 0) return(-ENOTCONN);
	return(n);	
}

int net_send(int fd, void *buf, int len)
{
	int n;

	while(((n = send(fd, buf, len, 0)) < 0) && (errno == EINTR)) ;
	if(n < 0){
		if(errno == EAGAIN) return(0);
		return(-errno);
	}
	else if(n == 0) return(-ENOTCONN);
	return(n);	
}

int net_sendto(int fd, void *buf, int len, void *to, int sock_len)
{
	int n;

	while(((n = sendto(fd, buf, len, 0, (struct sockaddr *) to,
			   sock_len)) < 0) && (errno == EINTR)) ;
	if(n < 0){
		if(errno == EAGAIN) return(0);
		return(-errno);
	}
	else if(n == 0) return(-ENOTCONN);
	return(n);	
}

struct change_data {
	char *dev;
	char *what;
	char *address;
	char *netmask;
	int output_len;
	char *output;
};

static void change_tramp(void *arg)
{
	int pid, fds[2];
	struct change_data *data = arg;
	char version[sizeof("nnnnn\0")];
	char *argv[] = { "uml_net", version, data->what, data->dev, 
			 data->address, data->netmask, NULL };

	sprintf(version, "%d", UML_NET_VERSION);
	if(pipe(fds) < 0){
		printk("change_tramp - pipe failed, errno = %d\n",
		       errno);
		return;
	}
	if((pid = fork()) == 0){
		dup2(fds[1], 1);
		close(fds[0]);
		execvp(argv[0], argv);
		printk("Exec of '%s' failed - errno = %d\n", argv[0], errno);
		exit(1);		
	}
	close(fds[1]);
	read_output(fds[0], data->output, data->output_len);
	waitpid(pid, NULL, 0);
}

static void change(char *dev, char *what, unsigned char *addr,
		   unsigned char *netmask)
{
	char addr_buf[sizeof("255.255.255.255\0")];
	char netmask_buf[sizeof("255.255.255.255\0")];
	struct change_data data;

	data.dev = dev;
	data.what = what;
	sprintf(addr_buf, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
	sprintf(netmask_buf, "%d.%d.%d.%d", netmask[0], netmask[1], 
		netmask[2], netmask[3]);
	data.address = addr_buf;
	data.netmask = netmask_buf;
	data.output_len = page_size();
	data.output = um_kmalloc(data.output_len);
	if(data.output == NULL)
		printk("change : failed to allocate output buffer\n");
	tracing_cb(change_tramp, &data);
	if(data.output != NULL){
		printk("%s", data.output);
		kfree(data.output);
	}
}

void open_addr(unsigned char *addr, unsigned char *netmask, void *arg)
{
	change(arg, "add", addr, netmask);
}

void close_addr(unsigned char *addr, unsigned char *netmask, void *arg)
{
	change(arg, "del", addr, netmask);
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
