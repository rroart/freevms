// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified User Mode Linux source file, 2001-2004.

/*
 * user-mode-linux networking multicast transport
 * Copyright (C) 2001 by Harald Welte <laforge@gnumonks.org>
 *
 * based on the existing uml-networking code, which is
 * Copyright (C) 2001 Lennert Buytenhek (buytenh@gnu.org) and 
 * James Leu (jleu@mindspring.net).
 * Copyright (C) 2001 by various other people who didn't put their name here.
 *
 * Licensed under the GPL.
 */

#include "linux/kernel.h"
#include "linux/init.h"
#include "linux/netdevice.h"
#include "linux/etherdevice.h"
#include "linux/in.h"
#include "linux/inet.h"
#include "net_kern.h"
#include "net_user.h"
#include "mcast.h"
#include "mcast_kern.h"

struct mcast_data mcast_priv[MAX_UML_NETDEV] = {
	[ 0 ... MAX_UML_NETDEV - 1 ] =
	{
		addr:		"239.192.168.1",
		port:		1102,
		ttl:		1,
		hwaddr:		{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
		hw_setup:	0,
	}
};

struct net_device *mcast_init(int private_size, int index)
{
	struct net_device *dev;
	struct uml_net_private *pri;
	struct mcast_data *dpri;

	dev = init_etherdev(NULL, private_size);
	if (!dev) 
		return NULL;

	pri = dev->priv;
	dpri = (struct mcast_data *) pri->user;
	*dpri = mcast_priv[index];
	memcpy(dev->dev_addr, dpri->hwaddr, ETH_ALEN);
	printk("mcast backend ");
	if(dpri->hw_setup)
		printk("ethernet address=%x:%x:%x:%x:%x:%x ",
		       dpri->hwaddr[0], dpri->hwaddr[1], dpri->hwaddr[2], 
		       dpri->hwaddr[3], dpri->hwaddr[4], dpri->hwaddr[5]);
		
	printk("multicast adddress: %s:%u, TTL:%u ",
	       dpri->addr, dpri->port, dpri->ttl);

	printk("\n");
	return(dev);
}

static unsigned short mcast_protocol(u, buf)
{
	return lan$eth_type_trans(u, buf);
}

static int mcast_set_mac(struct sockaddr *addr, void *data)
{
	struct mcast_data *pri = data;
	struct net_device *dev = pri->dev;
	struct sockaddr *hwaddr = addr;

	memcpy(dev->dev_addr, hwaddr->sa_data, ETH_ALEN);
	return mcast_user_set_mac(pri, hwaddr->sa_data, ETH_ALEN);
}

static int mcast_read(int fd, char * buf, int mtu, struct uml_net_private *lp)
{
	return(net_recvfrom(fd, buf, 
			    mtu + ETH_HEADER_OTHER));
}

static int mcast_write(int fd, char * buf, int len,
			struct uml_net_private *lp)
{
	return mcast_user_write(fd, buf, len,
				 (struct mcast_data *) &lp->user);
}

static struct net_kern_info mcast_kern_info = {
	init:			mcast_init,
	protocol:		mcast_protocol,
	set_mac:		mcast_set_mac,
	read:			mcast_read,
	write:			mcast_write,
};

static int mcast_count = 0;

void mcast_setup(char *str, struct uml_net *dev)
{
	int err, n = mcast_count;
	int num = 0;
	char *p1, *p2;

	dev->user = &mcast_user_info;
	dev->kern = &mcast_kern_info;
	dev->private_size = sizeof(struct mcast_data);
	dev->transport_index = mcast_count++;


	/* somewhat more sophisticated parser, needed for in_aton */

	p1 = str;
	if (*str == ',')
		p1++;
	while (p1 && *p1) {
		if ((p2 = strchr(p1, ',')))
			*p2++ = '\0';
		if (strlen(p1) > 0) {
			switch (num) {
			case 0:
				/* First argument: Ethernet address */
				err = setup_etheraddr(p1, 
						      mcast_priv[n].hwaddr);
				if (!err) 
					mcast_priv[n].hw_setup = 1;
				break;
			case 1:
				/* Second argument: Multicast group */
				mcast_priv[n].addr = p1;
				break;
			case 2:
				/* Third argument: Port number */
				mcast_priv[n].port = 
					htons(simple_strtoul(p1, NULL, 10));
				break;
			case 3:
				/* Fourth argument: TTL */
				mcast_priv[n].ttl = 
						simple_strtoul(p1, NULL, 10);
				break;
			}
		}
		p1 = p2;
		num++;
	}

	printk(KERN_INFO "Configured mcast device: %s:%u-%u\n",
		mcast_priv[n].addr, mcast_priv[n].port,
		mcast_priv[n].ttl);

	return;
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
