#include "linux/kernel.h"
#include "linux/stddef.h"
#include "linux/init.h"
#include "linux/netdevice.h"
#include "linux/if_arp.h"
#include "net_kern.h"
#include "net_user.h"
#include "kern.h"
#include "slip.h"
#include "slip_kern.h"

struct slip_data slip_priv[MAX_UML_NETDEV] = {
	[ 0 ... MAX_UML_NETDEV - 1 ] =
	{
		addr:		NULL,
		gate_addr:	NULL,
		slave:		-1,
		buf: 		{ 0 },
		pos:		0,
		esc:		0,
	}
};

struct net_device umn_dev;

struct net_device *slip_init(int private_size, int index)
{
	struct uml_net_private *private;
	struct slip_data *spri;

	private = kmalloc(private_size, GFP_KERNEL);
	if(private == NULL) return(NULL);
	umn_dev.priv = private;
	spri = (struct slip_data *) private->user;
	*spri = slip_priv[index];
	strncpy(umn_dev.name, "umn", IFNAMSIZ);
	umn_dev.init = NULL;
	umn_dev.hard_header_len = 0;
	umn_dev.addr_len = 4;
	umn_dev.type = ARPHRD_ETHER;
	umn_dev.tx_queue_len = 256;
	umn_dev.flags = IFF_NOARP;
#ifndef CONFIG_VMS
	if(register_netdev(&umn_dev))
		printk(KERN_ERR "Couldn't initialize umn\n");
#endif
	printk("SLIP backend - SLIP IP = %s\n", spri->gate_addr);
	
	return(&umn_dev);
}

static int slip_set_mac(struct sockaddr *hwaddr, void *data)
{
	return(0);
}

static unsigned short slip_protocol(u, buf)
{
	return(htons(ETH_P_IP));
}

static int slip_read(int fd, char * buf, int mtu,
		       struct uml_net_private *lp)
{
	return(slip_user_read(fd, buf, mtu,
			      (struct slip_data *) &lp->user));
}

static int slip_write(int fd, char * buf, int len,
		      struct uml_net_private *lp)
{
	return(slip_user_write(fd, buf, len,
			       (struct slip_data *) &lp->user));
}

struct net_kern_info slip_kern_info = {
	init:			slip_init,
	protocol:		slip_protocol,
	set_mac:		slip_set_mac,
	read:			slip_read,
	write:			slip_write,
};

static int slip_count = 0;

void slip_setup(char *str, struct uml_net *dev)
{
	int n = slip_count;

	dev->user = &slip_user_info;
	dev->kern = &slip_kern_info;
	dev->private_size = sizeof(struct slip_data);
	dev->transport_index = slip_count++;
	if(*str != ',') return;
	str++;
	if(str[0] != '\0') slip_priv[n].gate_addr = str;
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
