/*
 * Copyright (C) 2001 Lennert Buytenhek (buytenh@gnu.org) and 
 * James Leu (jleu@mindspring.net).
 * Copyright (C) 2001 by various other people who didn't put their name here.
 * Licensed under the GPL.
 */

#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include "net_user.h"
#include "daemon.h"
#include "kern_util.h"
#include "user_util.h"
#include "user.h"

#define MAX_PACKET (ETH_MAX_PACKET + ETH_HEADER_OTHER)

enum request_type { REQ_NEW_CONTROL };

#define SWITCH_MAGIC 0xfeedface

struct request_v1 {
  unsigned long magic;
  enum request_type type;
  union {
    struct {
      unsigned char addr[ETH_ADDR_LEN];
      struct sockaddr_un name;
    } new_control;
  } u;
};

static struct sockaddr_un *new_addr(void *name, int len)
{
	struct sockaddr_un *sun;

	sun = um_kmalloc(sizeof(struct sockaddr_un));
	if(sun == NULL){
		printk("new_addr: allocation of sockaddr_un failed\n");
		return(NULL);
	}
	sun->sun_family = AF_UNIX;
	memcpy(sun->sun_path, name, len);
	return(sun);
}

static void daemon_user_init(void *data, void *dev)
{
	struct daemon_data *pri = data;
	struct timeval tv;
	struct {
		char zero;
		int pid;
		int usecs;
	} name;

	if(!strcmp(pri->sock_type, "unix"))
		pri->ctl_addr = new_addr(pri->ctl_sock, 
					 strlen(pri->ctl_sock) + 1);
	name.zero = 0;
	name.pid = getpid();
	gettimeofday(&tv, NULL);
	name.usecs = tv.tv_usec;
	pri->local_addr = new_addr(&name, sizeof(name));
	pri->dev = dev;
}

static int daemon_open(void *data)
{
	struct daemon_data *pri = data;
	struct sockaddr_un *ctl_addr = pri->ctl_addr;
	struct sockaddr_un *local_addr = pri->local_addr;
	struct sockaddr_un *sun;
	struct request_v1 req;
	char addr[sizeof("255.255.255.255\0")];
	int fd, n, err;

	if(!pri->hw_setup){
		pri->hwaddr[0] = 0xfe;
		pri->hwaddr[1] = 0xfd;
		pri->hwaddr[2] = 0x0;
		pri->hwaddr[3] = 0x0;
		pri->hwaddr[4] = 0x0;
		pri->hwaddr[5] = 0x0;
		dev_ip_addr(pri->dev, addr, &pri->hwaddr[2]);
		set_ether_mac(pri->dev, pri->hwaddr);
	}
	if((ctl_addr == NULL) || (pri->local_addr == NULL))
		return(-EINVAL);

	if((pri->control = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		printk("daemon_open : control socket failed, errno = %d\n", 
		       errno);		
		return(-errno);
	}

	if(connect(pri->control, (struct sockaddr *) ctl_addr, 
		   sizeof(*ctl_addr)) < 0){
		printk("daemon_open : control connect failed, errno = %d\n",
		       errno);
		err = -errno;
		goto out;
	}

	if((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0){
		printk("daemon_open : data socket failed, errno = %d\n", 
		       errno);
		err = -errno;
		goto out;
	}
	if(bind(fd, (struct sockaddr *) local_addr, sizeof(*local_addr)) < 0){
		printk("daemon_open : data bind failed, errno = %d\n", 
		       errno);
		err = -errno;
		goto out_close;
	}

	sun = um_kmalloc(sizeof(struct sockaddr_un));
	if(sun == NULL){
		printk("new_addr: allocation of sockaddr_un failed\n");
		err = -ENOMEM;
		goto out_close;
	}

	req.magic = SWITCH_MAGIC;
	req.type = REQ_NEW_CONTROL;
	memcpy(req.u.new_control.addr, pri->hwaddr, 
	       sizeof(req.u.new_control.addr));
	req.u.new_control.name = *local_addr;
	n = write(pri->control, &req, sizeof(req));
	if(n != sizeof(req)){
		printk("daemon_open : control setup request returned %d, "
		       "errno = %d\n", n, errno);
		err = -ENOTCONN;
		goto out;		
	}

	n = read(pri->control, sun, sizeof(*sun));
	if(n != sizeof(*sun)){
		printk("daemon_open : read of data socket returned %d, "
		       "errno = %d\n", n, errno);
		err = -ENOTCONN;
		goto out_close;		
	}
	pri->data_addr = sun;

	return(fd);

 out_close:
	close(fd);
 out:
	close(pri->control);
	return(err);
}

static void daemon_close(int fd, void *data)
{
	struct daemon_data *pri = data;

	close(fd);
	close(pri->control);
}

int daemon_user_write(int fd, void *buf, int len, struct daemon_data *pri)
{
	struct sockaddr_un *data_addr = pri->data_addr;

	return(net_sendto(fd, buf, len, data_addr, sizeof(*data_addr)));
}

static int daemon_set_mtu(int mtu, void *data)
{
	return(mtu);
}

int daemon_user_set_mac(struct daemon_data *pri, unsigned char *hwaddr,
			 int len)
{
	memcpy(pri->hwaddr, hwaddr, len);
	return(0);
}

struct net_user_info daemon_user_info = {
	init:		daemon_user_init,
	open:		daemon_open,
	close:	 	daemon_close,
	set_mtu:	daemon_set_mtu,
	add_address:	NULL,
	delete_address: NULL,
	max_packet:	MAX_PACKET - ETH_HEADER_OTHER
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
