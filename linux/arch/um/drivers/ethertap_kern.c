/*
 * Copyright (C) 2001 Lennert Buytenhek (buytenh@gnu.org) and 
 * James Leu (jleu@mindspring.net).
 * Copyright (C) 2001 by various other people who didn't put their name here.
 * Licensed under the GPL.
 */

#include "linux/init.h"
#include "linux/netdevice.h"
#include "linux/etherdevice.h"
#include "net_kern.h"
#include "net_user.h"
#include "etap.h"
#include "etap_kern.h"

struct ethertap_setup {
	char *dev_name;
	unsigned char hw_addr[ETH_ALEN];
	int hw_setup;
	char *gate_addr;
};

struct ethertap_setup ethertap_priv[MAX_UML_NETDEV] = { 
	[ 0 ... MAX_UML_NETDEV - 1 ] =
	{
		dev_name:	NULL,
		hw_addr:	{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
		hw_setup:	0,
		gate_addr:	NULL,
	}
};

struct net_device *etap_init(int private_size, int index)
{
	struct net_device *dev;
	struct uml_net_private *pri;
	struct ethertap_data *epri;

	dev = init_etherdev(NULL, private_size);
	if(dev == NULL) return(NULL);
	pri = dev->priv;
	epri = (struct ethertap_data *) pri->user;
	epri->dev_name = ethertap_priv[index].dev_name;
	epri->gate_addr = ethertap_priv[index].gate_addr;
	memcpy(dev->dev_addr, ethertap_priv[index].hw_addr, ETH_ALEN);
	memcpy(epri->hw_addr, ethertap_priv[index].hw_addr, 
	       sizeof(epri->hw_addr));
	printk("ethertap backend - %s", epri->dev_name);
	if(epri->gate_addr != NULL) 
		printk(", IP = %s", epri->gate_addr);
	epri->hw_setup = ethertap_priv[index].hw_setup;
	if(epri->hw_setup)
		printk(", ether = %x:%x:%x:%x:%x:%x",
		       epri->hw_addr[0], epri->hw_addr[1], epri->hw_addr[2],
		       epri->hw_addr[3], epri->hw_addr[4], epri->hw_addr[5]);
	printk("\n");
	epri->data_fd = -1;
	epri->control_fd = -1;
	return(dev);
}

static unsigned short etap_protocol(struct sk_buff *skb)
{
	return(eth_type_trans(skb, skb->dev));
}

static int etap_set_mac(struct sockaddr *addr, void *data)
{
	struct ethertap_data *pri = data;
	struct sockaddr *hwaddr = addr;

	memcpy(pri->hw_addr, hwaddr->sa_data, ETH_ALEN);

	return 0;
}

static int etap_read(int fd, struct sk_buff **skb, struct uml_net_private *lp)
{
	int len;

	*skb = ether_adjust_skb(*skb, ETH_HEADER_ETHERTAP);
	if(*skb == NULL) return(-ENOMEM);
	len = net_recvfrom(fd, (*skb)->mac.raw, 
			   (*skb)->dev->mtu + 2 * ETH_HEADER_ETHERTAP);
	if(len <= 0) return(len);
	skb_pull(*skb, 2);
	len -= 2;
	return(len);
}

static int etap_write(int fd, struct sk_buff **skb, struct uml_net_private *lp)
{
	if(skb_headroom(*skb) < 2){
	  	struct sk_buff *skb2;

		skb2 = skb_realloc_headroom(*skb, 2);
		dev_kfree_skb(*skb);
		if (skb2 == NULL) return(-ENOMEM);
		*skb = skb2;
	}
	skb_push(*skb, 2);
	return(net_send(fd, (*skb)->data, (*skb)->len));
}

struct net_kern_info ethertap_kern_info = {
	init:			etap_init,
	protocol:		etap_protocol,
	set_mac:		etap_set_mac,
	read:			etap_read,
	write: 			etap_write,
};

static int ethertap_count = 0;

void ethertap_setup(char *str, struct uml_net *dev)
{
	struct ethertap_setup *pri;

	dev->user = &ethertap_user_info;
	dev->kern = &ethertap_kern_info;
	dev->private_size = sizeof(struct ethertap_data);
	pri = &ethertap_priv[ethertap_count];
	dev->transport_index = ethertap_count++;
	tap_setup_common(str, "ethertap", &pri->dev_name, pri->hw_addr,  
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
