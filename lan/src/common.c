#include<cxbdef.h>
#include<crbdef.h>
#include<cdtdef.h>
#include<dcdef.h>
#include<ddtdef.h>
#include<dptdef.h>
#include<fdtdef.h>
#include<pdtdef.h>
#include<idbdef.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ucbnidef.h>
#include<ddbdef.h>
#include<ipldef.h>
#include<dyndef.h>
#include<ssdef.h>
#include<iodef.h>
#include<devdef.h>
#include<nmadef.h>
#include<system_data_cells.h>
#include<ipl.h>
#include<linux/vmalloc.h>
#include<linux/pci.h>
#include<system_service_setup.h>
#include<descrip.h>

#include <iosbdef.h>

#include<linux/mm.h>
#include<linux/if_ether.h>

#include <linux/netdevice.h>
#include <linux/inetdevice.h>

#include "../../cmuip/ipacp/src/xedrv.h"
#include "../../cmuip/central/include/netconfig.h"

static check_dup(struct _ucb * u, long * l) {
  long len=l[0]/(2*sizeof(long));
  long *addr = l[1];
  long proto=0;
  for(;len;len--) {
    switch (*addr++) {
    case NMA$C_PCLI_PTY:
      //lsb->lsb$l_valid_pty=*addr++;
      proto=htons(*addr++);
      break;
    default:
    }
  }
  struct _ucb * head=u ;
  struct _ucb * tmp=head->ucb$l_link;
  struct _ucbnidef * ni;
  while (tmp) {
    ni = tmp;
    if (proto == ni->ucb$l_ni_pty) break;
    tmp=tmp->ucb$l_link;
  }
  return (proto == ni->ucb$l_ni_pty);
}

extern struct net_device * scs_default_device;

int lan$setmode(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c){ 
  switch (i->irp$l_func&IO$M_FMODIFIERS) {
  case IO$M_CTRL|IO$M_STARTUP:
    {
    struct dsc$descriptor * d=i->irp$l_qio_p2;
    long * l=d;
    long len=l[0]/(2*sizeof(long));
    long *addr = d->dsc$a_pointer;

    unsigned short int chan;
    struct _ucbnidef * newucb;

    if (check_dup(u->ucb$l_ddb->ddb$ps_ucb,d)) goto dup;

    ioc_std$clone_ucb(u->ucb$l_ddb->ddb$ps_ucb /*&er$ucb*/,&newucb);
    //    exe$assign(dsc,&chan,0,0,0);

    c->ccb$l_ucb=newucb;

    struct _ucbnidef * ni=newucb;

    struct net_device * dev = ni -> ucb$l_extra_l_1;

    scs_default_device = dev;

    //struct _lsbdef * lsb=kmalloc(sizeof(struct _lsbdef),GFP_KERNEL);
    //bzero(lsb,sizeof(struct _lsbdef));
    for(;len;len--) {
      switch (*addr++) {
      case NMA$C_PCLI_PTY:
	//lsb->lsb$l_valid_pty=*addr++;
	ni->ucb$l_ni_pty=htons(*addr++);
	break;
      default:
      }
    }
    //lsb->lsb$l_next_lsb=ni->ucb$l_ni_lsb;
    //ni->ucb$l_ni_lsb=lsb;

    config_in_dev(&dev->ip_ptr);

    if (dev->open) {
      dev->open(dev);
    }
    set_bit(__LINK_STATE_START, &dev->state);
    }
  dup:
    break;

  case IO$M_CTRL|IO$M_SHUTDOWN:
    // not yet; doing a dup check above first
    break;

  default:
    printk("unknown lan$setmode\n");
  }

  if (i->irp$l_iosb) *(long long *)i->irp$l_iosb=SS$_NORMAL;
  return SS$_NORMAL;
}

int lan$sensemode(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  if ((i->irp$l_func&(IO$M_CTRL))==(IO$M_CTRL)) {
    struct dsc$descriptor * d=i->irp$l_qio_p2;
    long * l=d;
    long len=l[0]/(2*sizeof(long));

    char *addr = d->dsc$a_pointer;
    struct XE_Sense * sense=addr;

    struct _ucbnidef * ni=u;

    struct net_device * dev = ni -> ucb$l_extra_l_1;

    sense->XE_Sense_Param = NMA$C_PCLI_HWA;
    sense->XE_Sense_Type = 1;
    sense->XE_Sense_Length=6;
    
    memcpy(sense->XE_Sense_String,dev->dev_addr,6);

    sense=(long)sense+10+2; // 2 because of gcc feature/bug

    sense->XE_Sense_Param = NMA$C_PCLI_PHA;
    sense->XE_Sense_Type = 1;
    sense->XE_Sense_Length=6;
    
    signed long bc[3] = { -1, -1 , -1};

    memcpy(sense->XE_Sense_String,bc,6);

  }
  if (i->irp$l_iosb) {
    struct XE_iosb_structure * iosb = i->irp$l_iosb;
    iosb->xe$tran_size=2*(4+6+2); // 2 from gcc bug / feature
    iosb->xe$vms_code=SS$_NORMAL;
  }
  return SS$_NORMAL;
}

int lan$setchar(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) { }

int lan$sensechar(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) { }

int lan$eth_type_trans(struct _ucb * u, void * data ) {
	struct _ucbnidef * ni = u;
 	struct net_device * dev = ni -> ucb$l_extra_l_1;
	
	struct ethhdr *eth;
	unsigned char *rawp;
	
	eth= data;
	
	if(*eth->h_dest&1)
	{
#if 0
		if(memcmp(eth->h_dest,dev->broadcast, ETH_ALEN)==0)
			skb->pkt_type=PACKET_BROADCAST;
		else
			skb->pkt_type=PACKET_MULTICAST;
#endif
	}
	
	/*
	 *	This ALLMULTI check should be redundant by 1.4
	 *	so don't forget to remove it.
	 *
	 *	Seems, you forgot to remove it. All silly devices
	 *	seems to set IFF_PROMISC.
	 */
	 
	else if(1 /*dev->flags&IFF_PROMISC*/)
	{
#if 0
		if(memcmp(eth->h_dest,dev->dev_addr, ETH_ALEN))
			skb->pkt_type=PACKET_OTHERHOST;
#endif
	}
	
	if (ntohs(eth->h_proto) >= 1536)
		return eth->h_proto;
		
#if 0
	rawp = skb->data;
#else
	rawp = eth; // wrong
#endif

	/*
	 *	This is a magic hack to spot IPX packets. Older Novell breaks
	 *	the protocol design and runs IPX over 802.3 without an 802.2 LLC
	 *	layer. We look for FFFF which isn't a used 802.2 SSAP/DSAP. This
	 *	won't work for fault tolerant netware but does for the rest.
	 */
	if (*(unsigned short *)rawp == 0xFFFF)
		return htons(ETH_P_802_3);
		
	/*
	 *	Real 802.2 LLC
	 */
	return htons(ETH_P_802_2);
}

int lan$netif_rx(struct _ucb * u, void * bdsc) {
  struct _cxb * cb1 = bdsc;
  struct _cxb * cb2 = cb1->cxb$l_link; 
  int proto=ntohs(lan$eth_type_trans(u, cb1->cxb$ps_pktdata));
  struct _ucbnidef * ni;
  struct _ucb * head=u;
  struct _ucb * tmp=u->ucb$l_link;
  if (proto==ETH_P_IPV6) {
    kfreebuf(cb1);
    return;
  }
  while (tmp) {
    ni = tmp;
    if (proto == ni->ucb$l_ni_pty) break;
    tmp=tmp->ucb$l_link;
  }
  if (proto != ni->ucb$l_ni_pty) {
    kfreebuf(cb1);
    return;
  }

  if (aqempty(&tmp->ucb$l_ioqfl)) {
    kfreebuf(cb1);
    return;
  }

  struct _irp * i = remque(tmp->ucb$l_ioqfl, 0);
  i->irp$l_bcnt=i->irp$l_qio_p2;
  i->irp$l_svapte = cb1;
  cb1->cxb$ps_uva32=i->irp$l_qio_p5;
  cb2->cxb$ps_uva32=i->irp$l_qio_p1;
  i->irp$l_iost1=SS$_NORMAL|(cb2->cxb$w_length<<16);
  i->irp$l_iost2=0x0800;
  return com$post(i,tmp);

}

int lan$readblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  exe$insertirp(u,i);
  return SS$_NORMAL;
}

void * lan$alloc_cxb(int len) {
  struct _cxb * cb1 = kmalloc(sizeof(struct _cxb),GFP_KERNEL);
  struct _cxb * cb2 = kmalloc(sizeof(struct _cxb),GFP_KERNEL);
  char * buf1 = kmalloc(14,GFP_KERNEL);
  char * buf2 = kmalloc(len-14,GFP_KERNEL);
  cb1->cxb$b_type=DYN$C_CXB;
  cb2->cxb$b_type=DYN$C_CXB;
  cb1->cxb$w_length=14;
  cb2->cxb$w_length=len-14;
  cb1->cxb$ps_pktdata=buf1;
  cb2->cxb$ps_pktdata=buf2;
  cb1->cxb$l_link=cb2;
  cb2->cxb$l_link=0;
  return cb1;
}

void * lan$alloc_xmit_buf(struct _irp * i, struct _ucbnidef * u, char *dest) {
  char * buf=kmalloc(4096, GFP_KERNEL);
  unsigned short pty = htons(u->ucb$l_ni_pty);
  unsigned short * pty_p = &buf[12];
  memcpy(buf,i->irp$l_qio_p5,6);
  memcpy(&buf[6],dest,6);
  *pty_p=pty; // maybe it should be done before qio instead? -> above 6 -> 8
  memcpy(&buf[14],i->irp$l_qio_p1,i->irp$l_qio_p2);
  return buf;
}

#if 0
init_etherdev() {
  printk("init etherdev not imopl\n");
}
#endif

int              register_netdevice(struct net_device *dev) {
  printk("reg net not impl\n");
	return 0;
}

int              unregister_netdevice(struct net_device *dev) {
  printk ("unreg netdev not impl\n");
	return 0;
} 

int              register_netdev(struct net_device *dev) {
  printk("reg net not impl\n");
	return 0;
}

void              unregister_netdev(struct net_device *dev) {
  printk ("unreg netdev not impl\n");
} 

register_inetaddr_notifier(struct notifier_block *nb) {
  printk(" register_inetaddr_notifie not impl\n");
}

int              dev_close(struct net_device *dev) {
  printk("dev close not impol\n");
}

__u32 in_aton(const char *str)
{
	unsigned long l;
	unsigned int val;
	int i;

	l = 0;
	for (i = 0; i < 4; i++) 
	{
		l <<= 8;
		if (*str != '\0') 
		{
			val = 0;
			while (*str != '\0' && *str != '.') 
			{
				val *= 10;
				val += *str - '0';
				str++;
			}
			l |= val;
			if (*str != '\0') 
				str++;
		}
	}
	return(htonl(l));
}


/*
 *       Callers must hold the rtnl semaphore.  See the comment at the
 *       end of Space.c for details about the locking.
 */
int __init net_dev_init(void)
{
#if 0
	struct net_device *dev, **dp;
	int i;

	if (!dev_boot_phase)
		return 0;

#ifdef CONFIG_NET_DIVERT
	dv_init();
#endif /* CONFIG_NET_DIVERT */
	
	/*
	 *	Initialise the packet receive queues.
	 */

	for (i = 0; i < NR_CPUS; i++) {
		struct softnet_data *queue;

		queue = &softnet_data[i];
		skb_queue_head_init(&queue->input_pkt_queue);
		queue->throttle = 0;
		queue->cng_level = 0;
		queue->avg_blog = 10; /* arbitrary non-zero */
		queue->completion_queue = NULL;
	}
	
#ifdef CONFIG_NET_PROFILE
	net_profile_init();
	NET_PROFILE_REGISTER(dev_queue_xmit);
	NET_PROFILE_REGISTER(softnet_process);
#endif

#ifdef OFFLINE_SAMPLE
	samp_timer.expires = jiffies + (10 * HZ);
	add_timer(&samp_timer);
#endif

	/*
	 *	Add the devices.
	 *	If the call to dev->init fails, the dev is removed
	 *	from the chain disconnecting the device until the
	 *	next reboot.
	 *
	 *	NB At boot phase networking is dead. No locking is required.
	 *	But we still preserve dev_base_lock for sanity.
	 */

	dp = &dev_base;
	while ((dev = *dp) != NULL) {
		spin_lock_init(&dev->queue_lock);
		spin_lock_init(&dev->xmit_lock);
#ifdef CONFIG_NET_FASTROUTE
		dev->fastpath_lock = RW_LOCK_UNLOCKED;
#endif
		dev->xmit_lock_owner = -1;
		dev->iflink = -1;
		dev_hold(dev);

		/*
		 * Allocate name. If the init() fails
		 * the name will be reissued correctly.
		 */
#if 0
		if (strchr(dev->name, '%'))
			dev_alloc_name(dev, dev->name);
#endif

		/* 
		 * Check boot time settings for the device.
		 */
		netdev_boot_setup_check(dev);

		if (dev->init && dev->init(dev)) {
			/*
			 * It failed to come up. It will be unhooked later.
			 * dev_alloc_name can now advance to next suitable
			 * name that is checked next.
			 */
			dev->deadbeaf = 1;
			dp = &dev->next;
		} else {
			dp = &dev->next;
			dev->ifindex = dev_new_index();
			if (dev->iflink == -1)
				dev->iflink = dev->ifindex;
			if (dev->rebuild_header == NULL)
				dev->rebuild_header = default_rebuild_header;
			dev_init_scheduler(dev);
			set_bit(__LINK_STATE_PRESENT, &dev->state);
		}
	}
#endif

#if 0
	/*
	 * Unhook devices that failed to come up
	 */
	dp = &dev_base;
	while ((dev = *dp) != NULL) {
		if (dev->deadbeaf) {
			write_lock_bh(&dev_base_lock);
			*dp = dev->next;
			write_unlock_bh(&dev_base_lock);
			dev_put(dev);
		} else {
			dp = &dev->next;
		}
	}
#endif

#if 0
#ifdef CONFIG_PROC_FS
	proc_net_create("dev", 0, dev_get_info);
	create_proc_read_entry("net/softnet_stat", 0, 0, dev_proc_stats, NULL);
#ifdef WIRELESS_EXT
	proc_net_create("wireless", 0, dev_get_wireless_info);
#endif	/* WIRELESS_EXT */
#endif	/* CONFIG_PROC_FS */

	dev_boot_phase = 0;
#endif

#if 0
	open_softirq(NET_TX_SOFTIRQ, net_tx_action, NULL);
	open_softirq(NET_RX_SOFTIRQ, net_rx_action, NULL);
#endif

#if 0
	dst_init();
	dev_mcast_init();
#endif

#ifdef CONFIG_NET_SCHED
	pktsched_init();
#endif

	/*
	 *	Initialise network devices
	 */
	 
	net_device_init();

	return 0;
}

/**
 * init_etherdev - Register ethernet device
 * @dev: An ethernet device structure to be filled in, or %NULL if a new
 *	struct should be allocated.
 * @sizeof_priv: Size of additional driver-private structure to be allocated
 *	for this ethernet device
 *
 * Fill in the fields of the device structure with ethernet-generic values.
 *
 * If no device structure is passed, a new one is constructed, complete with
 * a private data area of size @sizeof_priv.  A 32-byte (not bit)
 * alignment is enforced for this private data area.
 *
 * If an empty string area is passed as dev->name, or a new structure is made,
 * a new name string is constructed.
 */

struct net_device *init_etherdev(struct net_device *dev, int sizeof_priv)
{
	return init_netdev(dev, sizeof_priv, "eth%d", ether_setup);
}

/* 
 *	Create and name a device from a prototype, then perform any needed
 *	setup.
 */

static struct net_device *init_netdev(struct net_device *dev, int sizeof_priv,
				      char *mask, void (*setup)(struct net_device *))
{
	int new_device = 0;

	/*
	 *	Allocate a device if one is not provided.
	 */
	 
	if (dev == NULL) {
		dev=init_alloc_dev(sizeof_priv);
		if(dev==NULL)
			return NULL;
		new_device = 1;
	}

	/*
	 *	Allocate a name
	 */
	 
#if 0
	if (dev->name[0] == '\0' || dev->name[0] == ' ') {
		strcpy(dev->name, mask);
		if (dev_alloc_name(dev, mask)<0) {
			if (new_device)
				kfree(dev);
			return NULL;
		}
	}
#endif

	netdev_boot_setup_check(dev);
	
	/*
	 *	Configure via the caller provided setup function then
	 *	register if needed.
	 */
	
	setup(dev);
	
	if (new_device) {
		int err;

#if 0
		rtnl_lock();
#endif
		err = register_netdevice(dev);
#if 0
		rtnl_unlock();
#endif

		if (err < 0) {
			kfree(dev);
			dev = NULL;
		}
	}
	return dev;
}

static struct net_device *init_alloc_dev(int sizeof_priv)
{
	struct net_device *dev;
	int alloc_size;

	/* ensure 32-byte alignment of the private area */
	alloc_size = sizeof (*dev) + sizeof_priv + 31;

	dev = (struct net_device *) kmalloc (alloc_size, GFP_KERNEL);
	if (dev == NULL)
	{
		printk(KERN_ERR "alloc_dev: Unable to allocate device memory.\n");
		return NULL;
	}

	memset(dev, 0, alloc_size);

	if (sizeof_priv)
		dev->priv = (void *) (((long)(dev + 1) + 31) & ~31);

	return dev;
}

void ether_setup(struct net_device *dev)
{
	/* Fill in the fields of the device structure with ethernet-generic values.
	   This should be in a common file instead of per-driver.  */
	
#if 0
	dev->change_mtu		= eth_change_mtu;
	dev->hard_header	= eth_header;
	dev->rebuild_header 	= eth_rebuild_header;
	dev->set_mac_address 	= eth_mac_addr;
	dev->hard_header_cache	= eth_header_cache;
	dev->header_cache_update= eth_header_cache_update;
	dev->hard_header_parse	= eth_header_parse;
#endif

#define ARPHRD_ETHER       1

	dev->type		= ARPHRD_ETHER;
	dev->hard_header_len 	= ETH_HLEN;
	dev->mtu		= 1500; /* eth_mtu */
	dev->addr_len		= ETH_ALEN;
	dev->tx_queue_len	= 100;	/* Ethernet wants good queues */	
	
	memset(dev->broadcast,0xFF, ETH_ALEN);

	/* New-style flags. */
	dev->flags		= IFF_BROADCAST|IFF_MULTICAST;
}
EXPORT_SYMBOL(ether_setup);

/* Boot time configuration table */
static struct netdev_boot_setup dev_boot_setup[NETDEV_BOOT_SETUP_MAX];

/**
 *	netdev_boot_setup_check	- check boot time settings
 *	@dev: the netdevice
 *
 * 	Check boot time settings for the device.
 *	The found settings are set for the device to be used
 *	later in the device probing.
 *	Returns 0 if no settings found, 1 if they are.
 */
int netdev_boot_setup_check(struct net_device *dev)
{
	struct netdev_boot_setup *s;
	int i;

	s = dev_boot_setup;
	for (i = 0; i < NETDEV_BOOT_SETUP_MAX; i++) {
		if (s[i].name[0] != '\0' && s[i].name[0] != ' ' &&
		    !strncmp(dev->name, s[i].name, strlen(s[i].name))) {
			dev->irq 	= s[i].map.irq;
			dev->base_addr 	= s[i].map.base_addr;
			dev->mem_start 	= s[i].map.mem_start;
			dev->mem_end 	= s[i].map.mem_end;
			return 1;
		}
	}
	return 0;
}

config_in_dev(struct in_device ** in) {
  extern Device_Configuration_Entry dev_config_tab[];
  struct in_ifaddr *in_ifa = kmalloc(sizeof(*in_ifa), GFP_KERNEL);
  struct in_device *in_dev = kmalloc(sizeof(*in_dev), GFP_KERNEL);
  memset(in_dev, 0, sizeof(*in_dev));
  memset(in_ifa, 0, sizeof(*in_ifa));
  *in=in_dev;
  in_dev->ifa_list=in_ifa;
  if (!in_dev)
    return NULL;
  in_ifa-> ifa_address = dev_config_tab[0].dc_ip_address;
  in_ifa-> ifa_mask = dev_config_tab[0].dc_ip_netmask;
}

struct net_device mynetdevice;
struct net_device mynetdevice2;

probe_units() {
#ifndef __arch_um__
  bzero(&mynetdevice,sizeof(mynetdevice));
  ne_probe(&mynetdevice);
#if 0
  bzero(&mynetdevice2,sizeof(mynetdevice2));
  ne_probe(&mynetdevice2);
#endif
#endif
}
