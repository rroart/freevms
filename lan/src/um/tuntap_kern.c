// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified User Mode Linux source file, 2001-2004.

/* 
 * Copyright (C) 2001 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include "linux/stddef.h"
#include "linux/netdevice.h"
#include "linux/etherdevice.h"
#include "linux/skbuff.h"
#include "asm/errno.h"
#include "net_kern.h"
#include "net_user.h"
#include "tuntap.h"

struct tuntap_setup {
	char *dev_name;
	unsigned char hw_addr[ETH_ALEN];
	int hw_setup;
	char *gate_addr;
};

struct tuntap_setup tuntap_priv[MAX_UML_NETDEV] = { 
	[ 0 ... MAX_UML_NETDEV - 1 ] =
	{
		dev_name:	NULL,
		hw_addr:	{ 0xfe, 0xfd, 0x0, 0x0, 0x0, 0x0 },
		hw_setup:	0,
		gate_addr:	NULL,
	}
};

struct net_device *tuntap_init(int private_size, int index)
{
	struct net_device *dev;
	struct uml_net_private *pri;
	struct tuntap_data *tpri;

	dev = init_etherdev(NULL, private_size);
	if(dev == NULL) return(NULL);
	pri = dev->priv;
	tpri = (struct tuntap_data *) pri->user;
	tpri->dev_name = tuntap_priv[index].dev_name;
	tpri->fixed_config = (tpri->dev_name != NULL);
	tpri->gate_addr = tuntap_priv[index].gate_addr;
	memcpy(dev->dev_addr, tuntap_priv[index].hw_addr, ETH_ALEN);
	memcpy(tpri->hw_addr, tuntap_priv[index].hw_addr, 
	       sizeof(tpri->hw_addr));
	printk("TUN/TAP backend - ");
	if(tpri->gate_addr != NULL) 
		printk("IP = %s", tpri->gate_addr);
	tpri->hw_setup = tuntap_priv[index].hw_setup;
	if(tpri->hw_setup)
		printk(" ether = %x:%x:%x:%x:%x:%x",
		       tpri->hw_addr[0], tpri->hw_addr[1], tpri->hw_addr[2],
		       tpri->hw_addr[3], tpri->hw_addr[4], tpri->hw_addr[5]);
	printk("\n");
	tpri->fd = -1;
	return(dev);
}

static unsigned short tuntap_protocol(u, buf) 
{
	return(lan$eth_type_trans(u, buf));
}

static int tuntap_set_mac(struct sockaddr *addr, void *data)
{
	struct tuntap_data *pri = data;
	struct sockaddr *hwaddr = addr;

	memcpy(pri->hw_addr, hwaddr->sa_data, ETH_ALEN);

	return 0;
}

static int tuntap_read(int fd, char * buf, int mtu,
		       struct uml_net_private *lp)
{
	return(net_read(fd, buf,
			mtu + ETH_HEADER_OTHER));
}

static int tuntap_write(int fd, char * buf, int len,
			struct uml_net_private *lp)
{
	return(net_write(fd, buf, len));
}

struct net_kern_info tuntap_kern_info = {
	init:			tuntap_init,
	protocol:		tuntap_protocol,
	set_mac:		tuntap_set_mac,
	read:			tuntap_read,
	write: 			tuntap_write,
};

static int tuntap_count = 0;

void tuntap_setup(char *str, struct uml_net *dev)
{
	struct tuntap_setup *pri;

	dev->user = &tuntap_user_info;
	dev->kern = &tuntap_kern_info;
	dev->private_size = sizeof(struct tuntap_data);
	pri = &tuntap_priv[tuntap_count];
	dev->transport_index = tuntap_count++;
	tap_setup_common(str, "tuntap", &pri->dev_name, pri->hw_addr,  
			 &pri->hw_setup, &pri->gate_addr);
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
