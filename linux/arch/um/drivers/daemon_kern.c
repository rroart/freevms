/*
 * Copyright (C) 2001 Lennert Buytenhek (buytenh@gnu.org) and 
 * James Leu (jleu@mindspring.net).
 * Copyright (C) 2001 by various other people who didn't put their name here.
 * Licensed under the GPL.
 */

#include "linux/kernel.h"
#include "linux/init.h"
#include "linux/netdevice.h"
#include "linux/etherdevice.h"
#include "net_kern.h"
#include "net_user.h"
#include "daemon.h"
#include "daemon_kern.h"

struct daemon_data daemon_priv[MAX_UML_NETDEV] = {
	[ 0 ... MAX_UML_NETDEV - 1 ] =
	{
		sock_type:	"unix",
		ctl_sock:	"/tmp/uml.ctl",
		hwaddr:		{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
		hw_setup:	0,
		control:	-1,
	}
};

struct net_device *daemon_init(int private_size, int index)
{
	struct net_device *dev;
	struct uml_net_private *pri;
	struct daemon_data *dpri;

	dev = init_etherdev(NULL, private_size);
	if(dev == NULL) return(NULL);
	pri = dev->priv;
	dpri = (struct daemon_data *) pri->user;
	*dpri = daemon_priv[index];
	memcpy(dev->dev_addr, dpri->hwaddr, ETH_ALEN);
	printk("daemon backend");
	if(dpri->hw_setup)
		printk("- ethernet address = %x:%x:%x:%x:%x:%x\n",
		       dpri->hwaddr[0], dpri->hwaddr[1], dpri->hwaddr[2], 
		       dpri->hwaddr[3], dpri->hwaddr[4], dpri->hwaddr[5]);
	printk("\n");
	return(dev);
}

static unsigned short daemon_protocol(struct sk_buff *skb)
{
	return(eth_type_trans(skb, skb->dev));
}

static int daemon_set_mac(struct sockaddr *addr, void *data)
{
	struct daemon_data *pri = data;
	struct net_device *dev = pri->dev;
	struct sockaddr *hwaddr = addr;

	memcpy(dev->dev_addr, hwaddr->sa_data, ETH_ALEN);
	return(daemon_user_set_mac(pri, hwaddr->sa_data, ETH_ALEN));
}

static int daemon_read(int fd, struct sk_buff **skb, 
		       struct uml_net_private *lp)
{
	*skb = ether_adjust_skb(*skb, ETH_HEADER_OTHER);
	if(*skb == NULL) return(-ENOMEM);
	return(net_recvfrom(fd, (*skb)->mac.raw, 
			    (*skb)->dev->mtu + ETH_HEADER_OTHER));
}

static int daemon_write(int fd, struct sk_buff **skb,
			struct uml_net_private *lp)
{
	return(daemon_user_write(fd, (*skb)->data, (*skb)->len, 
				 (struct daemon_data *) &lp->user));
}

static struct net_kern_info daemon_kern_info = {
	init:			daemon_init,
	protocol:		daemon_protocol,
	set_mac:		daemon_set_mac,
	read:			daemon_read,
	write:			daemon_write,
};

static int daemon_count = 0;

void daemon_setup(char *str, struct uml_net *dev)
{
	int err, n = daemon_count;

	dev->user = &daemon_user_info;
	dev->kern = &daemon_kern_info;
	dev->private_size = sizeof(struct daemon_data);
	dev->transport_index = daemon_count++;
	if(*str != ',') return;
	str++;
	if(*str != ','){
		err = setup_etheraddr(str, daemon_priv[n].hwaddr);
		if(!err) daemon_priv[n].hw_setup = 1;
	}
	str = strchr(str, ',');
	if(str == NULL) return;
	*str++ = '\0';
	if(*str != ',') daemon_priv[n].sock_type = str;
	str = strchr(str, ',');
	if(str == NULL) return;
	*str++ = '\0';
	if(*str != ',') daemon_priv[n].ctl_sock = str;
	str = strchr(str, ',');
	if(str == NULL) return;
	*str = '\0';
	printk(KERN_WARNING "daemon_setup : Ignoring data socket "
	       "specification\n");
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
