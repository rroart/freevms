/*
 * Copyright (C) 2001 Lennert Buytenhek (buytenh@gnu.org) and 
 * James Leu (jleu@mindspring.net).
 * Copyright (C) 2001 by various other people who didn't put their name here.
 * Licensed under the GPL.
 */

#include "linux/config.h"
#include "linux/kernel.h"
#include "linux/netdevice.h"
#include "linux/skbuff.h"
#include "linux/socket.h"
#include "linux/spinlock.h"
#include "linux/module.h"
#include "linux/init.h"
#include "linux/etherdevice.h"
#include "linux/list.h"
#include "linux/inetdevice.h"
#include "linux/ctype.h"
#include "user_util.h"
#include "kern_util.h"
#include "net_kern.h"
#include "net_user.h"
#include "slip.h"
#include "slip_kern.h"
#include "etap.h"
#include "etap_kern.h"
#include "tuntap.h"
#include "tuntap_kern.h"
#include "daemon.h"
#include "daemon_kern.h"
#include "mcast.h"
#include "mcast_kern.h"
#include "mconsole_kern.h"
#include "init.h"
#include "irq_user.h"

LIST_HEAD(opened);

struct uml_net devices[MAX_UML_NETDEV] = {
	[ 0 ... MAX_UML_NETDEV - 1 ] = 
	{
		dev:		NULL,
		user:		NULL,
		kern:		NULL,
		private_size:	0,
	}
};

static int eth_setup_common(char *str, int *index_out)
{
	char *end;
	int n;

	n = simple_strtoul(str, &end, 0);
	if(end == str){
		printk(KERN_ERR "eth_setup: Failed to parse '%s'\n", str);
		return(1);
	}
	if((n < 0) || (n > sizeof(devices)/sizeof(devices[0]))){
		printk(KERN_ERR "eth_setup: device %d out of range\n", n);
		return(1);
	}
	str = end;
	if(*str != '='){
		printk(KERN_ERR 
		       "eth_setup: expected '=' after device number\n");
		return(1);
	}
	str++;
	if(devices[n].dev != NULL){
		printk(KERN_ERR "eth_setup: Device %d already configured\n",
		       n);
		return(1);
	}
	if(index_out) *index_out = n;
#ifdef CONFIG_UML_NET_ETHERTAP
	if(!strncmp(str, "ethertap", strlen("ethertap"))){
		ethertap_setup(&str[strlen("ethertap")], &devices[n]);
		return(0);
	}
#endif
#ifdef CONFIG_UML_NET_TUNTAP
	if(!strncmp(str, "tuntap", strlen("tuntap"))){
		tuntap_setup(&str[strlen("tuntap")], &devices[n]);
		return(0);
	}
#endif
#ifdef CONFIG_UML_NET_DAEMON
	if(!strncmp(str, "daemon", strlen("daemon"))){
		daemon_setup(&str[strlen("daemon")], &devices[n]);
		return(0);
	}
#endif
#ifdef CONFIG_UML_NET_SLIP
	if(!strncmp(str, "slip", strlen("slip"))){
		slip_setup(&str[strlen("slip")], &devices[n]);
		return(0);
	}
#endif
#ifdef CONFIG_UML_NET_MCAST
	if(!strncmp(str, "mcast", strlen("mcast"))){
		mcast_setup(&str[strlen("mcast")], &devices[n]);
		return(0);
	}
#endif
	printk(KERN_ERR "Unknown transport in eth_setup : %s\n", str);
	return(1);
}

static int eth_setup(char *str)
{
	eth_setup_common(str, NULL);
	return(1);
}

#ifdef CONFIG_UML_NET_ETHERTAP
#define UML_NET_ETHERTAP_HELP	\
"    eth[0-9]+=ethertap,<device>,<ethernet address>,<tap ip address>\n" \
"    eth0=ethertap,tap0,,192.168.0.1\n\n"
#else
#define UML_NET_ETHERTAP_HELP
#endif
#ifdef CONFIG_UML_NET_TUNTAP
#define UML_NET_TUNTAP_HELP \
"    eth[0-9]+=tuntap,,<ethernet address>,<ip address>\n" \
"    eth0=tuntap,,fe:fd:0:0:0:1,192.168.0.1\n\n"
#else
#define UML_NET_TUNTAP_HELP
#endif
#ifdef CONFIG_UML_NET_DAEMON
#define UML_NET_DAEMON_HELP \
"    eth[0-9]+=daemon,<ethernet address>,<type>,<control socket>,<data socket>\n" \
"    eth0=daemon,unix,/tmp/uml.ctl,/tmp/uml.data\n\n"
#else
#define UML_NET_DAEMON_HELP
#endif
#ifdef CONFIG_UML_NET_SLIP
#define UML_NET_SLIP_HELP \
"    eth[0-9]+=slip,<slip ip>\n" \
"    eth0=slip,192.168.0.1\n\n"
#else
#define UML_NET_SLIP_HELP
#endif
#ifdef CONFIG_UML_NET_MCAST
#define UML_NET_MCAST_HELP \
"    eth[0-9]+=mcast,<ethernet address>,<address>,<port>,<ttl>\n" \
"    eth0=mcast,,224.2.3.4:5555,3\n\n"
#else
#define UML_NET_MCAST_HELP
#endif

__setup("eth", eth_setup);
__uml_help(eth_setup,
"eth[0-9]+=<transport>,<options>\n"
"    Configure a network device.  Formats and examples follow (one \n"
"    for each configured transport).\n\n"
UML_NET_ETHERTAP_HELP
UML_NET_TUNTAP_HELP
UML_NET_DAEMON_HELP
UML_NET_SLIP_HELP
UML_NET_MCAST_HELP
);
int ndev = 0;

static int uml_net_rx(struct net_device *dev)
{
	struct uml_net_private *lp = dev->priv;
	int pkt_len;
	struct sk_buff *skb;

	/* If we can't allocate memory, try again next round. */
	if ((skb = dev_alloc_skb(dev->mtu)) == NULL) {
		lp->stats.rx_dropped++;
		reactivate_fd(lp->fd);
		return 0;
	}

	skb->dev = dev;
	skb_put(skb, dev->mtu);
	skb->mac.raw = skb->data;
	pkt_len = (*lp->read)(lp->fd, &skb, lp);

	reactivate_fd(lp->fd);
	if (pkt_len > 0) {
		skb_trim(skb, pkt_len);
		skb->protocol = (*lp->protocol)(skb);
		netif_rx(skb);

		lp->stats.rx_bytes += skb->len;
		lp->stats.rx_packets++;
		return pkt_len;
	}

	kfree_skb(skb);
	return pkt_len;
}

void uml_net_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct net_device *dev = dev_id;
	struct uml_net_private *lp = dev->priv;
	int err;

	if (netif_running(dev)) {
		spin_lock(&lp->lock);
		while((err = uml_net_rx(dev)) > 0) ;
		if(err < 0) {
			printk(KERN_ERR 
			       "Device '%s' read returned %d, shutting it "
			       "down\n", dev->name, err);
			dev->flags &= ~IFF_UP;
			dev_close(dev);
		}
		spin_unlock(&lp->lock);
	}
}

static int uml_net_open(struct net_device *dev)
{
	struct uml_net_private *lp = dev->priv;
	int err;

	spin_lock(&lp->lock);

	if(lp->fd >= 0){
		err = -ENXIO;
		goto out;
	}

	lp->fd = (*lp->open)(&lp->user);
	if(lp->fd < 0){
		err = lp->fd;
		goto out;
	}

	err = um_request_irq(dev->irq, lp->fd, uml_net_interrupt,
			     SA_INTERRUPT | SA_SHIRQ, dev->name, dev);
	if(err != 0){
		printk(KERN_ERR "uml_net_open: failed to get irq(%d)\n", err);
		(*lp->close)(lp->fd, &lp->user);
		lp->fd = -1;
		err = -ENETUNREACH;
	}

	lp->tl.data = (unsigned long) &lp->user;
	netif_start_queue(dev);

	list_add(&lp->list, &opened);
	MOD_INC_USE_COUNT;
 out:
	spin_unlock(&lp->lock);
	return(err);
}

static int uml_net_close(struct net_device *dev)
{
	struct uml_net_private *lp = dev->priv;
	
	netif_stop_queue(dev);
	spin_lock(&lp->lock);

	free_irq(dev->irq, dev);
	(*lp->close)(lp->fd, &lp->user);
	lp->fd = -1;
	list_del(&lp->list);
	
	MOD_DEC_USE_COUNT;
	spin_unlock(&lp->lock);
	return 0;
}

static int uml_net_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct uml_net_private *lp = dev->priv;
	unsigned long flags;
	int len;

	netif_stop_queue(dev);

	spin_lock_irqsave(&lp->lock, flags);

	len = (*lp->write)(lp->fd, &skb, lp);

	if(len == skb->len) {
		lp->stats.tx_packets++;
		lp->stats.tx_bytes += skb->len;
		dev->trans_start = jiffies;
		netif_start_queue(dev);

		/* this is normally done in the interrupt when tx finishes */
		netif_wake_queue(dev);
	} 
	else if(len == 0){
		netif_start_queue(dev);
		lp->stats.tx_dropped++;
	}
	else {
		netif_start_queue(dev);
		printk(KERN_ERR "uml_net_start_xmit: failed(%d)\n", len);
	}

	spin_unlock_irqrestore(&lp->lock, flags);

	dev_kfree_skb(skb);

	return 0;
}

static struct net_device_stats *uml_net_get_stats(struct net_device *dev)
{
	struct uml_net_private *lp = dev->priv;
	return &lp->stats;
}

static void uml_net_set_multicast_list(struct net_device *dev)
{
	if (dev->flags & IFF_PROMISC) return;
	else if (dev->mc_count)	dev->flags |= IFF_ALLMULTI;
	else dev->flags &= ~IFF_ALLMULTI;
}

static void uml_net_tx_timeout(struct net_device *dev)
{
	dev->trans_start = jiffies;
	netif_wake_queue(dev);
}

static int uml_net_set_mac(struct net_device *dev, void *addr)
{
	struct uml_net_private *lp = dev->priv;
	struct sockaddr *hwaddr = addr;
	int err;

	spin_lock(&lp->lock);

	err = (*lp->set_mac)(hwaddr, &lp->user);

	spin_unlock(&lp->lock);

	return err;
}

static int uml_net_change_mtu(struct net_device *dev, int new_mtu)
{
	struct uml_net_private *lp = dev->priv;
	int err = 0;

	spin_lock(&lp->lock);

	new_mtu = (*lp->set_mtu)(new_mtu, &lp->user);
	if(new_mtu < 0){
		err = new_mtu;
		goto out;
	}

	dev->mtu = new_mtu;

 out:
	spin_unlock(&lp->lock);
	return err;
}

static int uml_net_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	return(-EINVAL);
}

void uml_net_user_timer_expire(unsigned long _conn)
{
#ifdef undef
	struct connection *conn = (struct connection *)_conn;

	dprintk(KERN_INFO "uml_net_user_timer_expire [%p]\n", conn);
	do_connect(conn);
#endif
}

static int eth_configure(struct uml_net *device, int n)
{
	struct net_device *dev;
	struct uml_net_private *lp;

	device->private_size += sizeof(struct uml_net_private) + 
		sizeof(((struct uml_net_private *) 0)->user);
	printk(KERN_INFO "Netdevice %d : ", n);
	dev = (*device->kern->init)(device->private_size,
				    device->transport_index);
	device->dev = dev;

	if (dev == NULL){
		printk(KERN_ERR "eth_configure: Out of memory on device %d\n",
		       n);
		return(1);
	}

	dev->mtu = device->user->max_packet;
	dev->open = uml_net_open;
	dev->hard_start_xmit = uml_net_start_xmit;
	dev->stop = uml_net_close;
	dev->get_stats = uml_net_get_stats;
	dev->set_multicast_list = uml_net_set_multicast_list;
	dev->tx_timeout = uml_net_tx_timeout;
	dev->set_mac_address = uml_net_set_mac;
	dev->change_mtu = uml_net_change_mtu;
	dev->do_ioctl = uml_net_ioctl;
	dev->watchdog_timeo = (HZ >> 1);
	dev->irq = UM_ETH_IRQ;

	lp = dev->priv;
	spin_lock_init(&lp->lock);
	init_timer(&lp->tl);
	lp->tl.function = uml_net_user_timer_expire;
	lp->list = ((struct list_head) LIST_HEAD_INIT(lp->list));
	memset(&lp->stats, 0, sizeof(lp->stats));
	lp->fd = -1;
	lp->protocol = device->kern->protocol;
	lp->set_mac = device->kern->set_mac;
	lp->open = device->user->open;
	lp->close = device->user->close;
	lp->read = device->kern->read;
	lp->write = device->kern->write;
	lp->add_address = device->user->add_address;
	lp->delete_address = device->user->delete_address;
	lp->set_mtu = device->user->set_mtu;
		
	if(device->user->init) 
		(*device->user->init)(&lp->user, dev);
	return(0);
}

int __init uml_net_probe(void)
{
	int i;

	for(i = 0; i < sizeof(devices)/sizeof(devices[0]); i++){
		if(devices[i].user == NULL) continue;
		eth_configure(&devices[i], i);
	}
	return(0);
}

static int net_config(char *str)
{
	int err, n;

	str = uml_strdup(str);
	if(str == NULL){
		printk(KERN_ERR "net_config failed to strdup string\n");
		return(1);
	}
	err = eth_setup_common(str, &n);
	if(err){
		kfree(str);
		return(err);
	}
	err = eth_configure(&devices[n], n);
	return(err);
}

static int net_remove(char *str)
{
	struct net_device *dev;
	struct uml_net_private *lp;
	int n;

	if(!isdigit(*str)) return(-1);
	n = *str - '0';
	if(devices[n].dev == NULL) return(0);
	dev = devices[n].dev;
	lp = dev->priv;
	if(lp->fd > 0) return(-1);
	unregister_netdev(dev);
	devices[n].dev = NULL;
	return(0);
}

static struct mc_device net_mc = {
	name:		"eth",
	config:		net_config,
	remove:		net_remove,
};

static int uml_inetaddr_event(struct notifier_block *this, unsigned long event,
			      void *ptr)
{
	struct in_ifaddr *ifa = ptr;
	u32 addr = ifa->ifa_address;
	u32 netmask = ifa->ifa_mask;
	struct net_device *dev = ifa->ifa_dev->dev;
	struct uml_net_private *lp;
	void (*proc)(unsigned char *, unsigned char *, void *);
	unsigned char addr_buf[4], netmask_buf[4];

	if(dev->open != uml_net_open) return(NOTIFY_DONE);

	lp = dev->priv;

	proc = NULL;
	switch (event){
	case NETDEV_UP:
		proc = lp->add_address;
		break;
	case NETDEV_DOWN:
		proc = lp->delete_address;
		break;
	}
	if(proc != NULL){
		addr_buf[0] = addr & 0xff;
		addr_buf[1] = (addr >> 8) & 0xff;
		addr_buf[2] = (addr >> 16) & 0xff;
		addr_buf[3] = addr >> 24;
		netmask_buf[0] = netmask & 0xff;
		netmask_buf[1] = (netmask >> 8) & 0xff;
		netmask_buf[2] = (netmask >> 16) & 0xff;
		netmask_buf[3] = netmask >> 24;
		(*proc)(addr_buf, netmask_buf, &lp->user);
	}
	return(NOTIFY_DONE);
}

struct notifier_block uml_inetaddr_notifier = {
	notifier_call:		uml_inetaddr_event,
};

static int uml_net_init(void)
{
	mconsole_register_dev(&net_mc);
	register_inetaddr_notifier(&uml_inetaddr_notifier);
	return(0);
}

__initcall(uml_net_init);

static void close_devices(void)
{
	struct list_head *ele;
	struct uml_net_private *lp;	

	list_for_each(ele, &opened){
		lp = list_entry(ele, struct uml_net_private, list);
		(*lp->close)(lp->fd, &lp->user);
	}
}

__uml_exitcall(close_devices);

int setup_etheraddr(char *str, unsigned char *addr)
{
	char *end;
	int i;

	for(i=0;i<6;i++){
		addr[i] = simple_strtoul(str, &end, 16);
		if((end == str) ||
		   ((*end != ':') && (*end != ',') && (*end != '\0'))){
			printk(KERN_ERR 
			       "setup_etheraddr: failed to parse '%s' "
			       "as an ethernet address\n", str);
			return(-1);
		}
		str = end + 1;
	}
	if(addr[0] & 1){
		printk(KERN_ERR 
		       "Attempt to assign a broadcast ethernet address to a "
		       "device disallowed\n");
		return(-1);
	}
	return(0);
}

void dev_ip_addr(void *d, char *buf, char *bin_buf)
{
	struct net_device *dev = d;
	struct in_device *ip = dev->ip_ptr;
	struct in_ifaddr *in;
	u32 addr;

	if((ip == NULL) || ((in = ip->ifa_list) == NULL)){
		printk(KERN_WARNING "dev_ip_addr - device not assigned an "
		       "IP address\n");
		return;
	}
	addr = in->ifa_address;
	sprintf(buf, "%d.%d.%d.%d", addr & 0xff, (addr >> 8) & 0xff, 
		(addr >> 16) & 0xff, addr >> 24);
	if(bin_buf){
		bin_buf[0] = addr & 0xff;
		bin_buf[1] = (addr >> 8) & 0xff;
		bin_buf[2] = (addr >> 16) & 0xff;
		bin_buf[3] = addr >> 24;
	}
}

void set_ether_mac(void *d, unsigned char *addr)
{
	struct net_device *dev = d;

	memcpy(dev->dev_addr, addr, ETH_ALEN);	
}

struct sk_buff *ether_adjust_skb(struct sk_buff *skb, int extra)
{
	if((skb != NULL) && (skb_tailroom(skb) < extra)){
	  	struct sk_buff *skb2;

		skb2 = skb_copy_expand(skb, 0, extra, GFP_ATOMIC);
		dev_kfree_skb(skb);
		skb = skb2;
	}
	if(skb != NULL) skb_put(skb, extra);
	return(skb);
}

void iter_addresses(void *d, void (*cb)(unsigned char *, unsigned char *, 
					void *), 
		    void *arg)
{
	struct net_device *dev = d;
	struct in_device *ip = dev->ip_ptr;
	struct in_ifaddr *in;
	unsigned char address[4], netmask[4];

	if(ip == NULL) return;
	in = ip->ifa_list;
	while(in != NULL){
		address[0] = in->ifa_address & 0xff;
		address[1] = (in->ifa_address >> 8) & 0xff;
		address[2] = (in->ifa_address >> 16) & 0xff;
		address[3] = in->ifa_address >> 24;
		netmask[0] = in->ifa_mask & 0xff;
		netmask[1] = (in->ifa_mask >> 8) & 0xff;
		netmask[2] = (in->ifa_mask >> 16) & 0xff;
		netmask[3] = in->ifa_mask >> 24;
		(*cb)(address, netmask, arg);
		in = in->ifa_next;
	}
}

void *get_output_buffer(int *len_out)
{
	void *ret;

	ret = (void *) __get_free_pages(GFP_KERNEL, 0);
	if(ret) *len_out = PAGE_SIZE;
	else *len_out = 0;
	return(ret);
}

void free_output_buffer(void *buffer)
{
	free_pages((unsigned long) buffer, 0);
}

void tap_setup_common(char *str, char *type, char **dev_name, char *hw_addr, 
		      int *hw_setup, char **gate_addr)
{
	int err;

	if(*str != ','){
		printk(KERN_ERR 
		       "ethertap_setup: expected ',' after '%s'\n", type);
		return;
	}
	str++;
	if(*str != ',') *dev_name = str;
	str = strchr(str, ',');
	if(str == NULL) return;
	*str++ = '\0';
	if(*str != ','){
		err = setup_etheraddr(str, hw_addr);
		if(!err) *hw_setup = 1;
	}
	str = strchr(str, ',');
	if(str == NULL) return;
	*str++ = '\0';
	if(*str != '\0') *gate_addr = str;
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
