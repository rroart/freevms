// $Id$
// $Locker$

// Author. Linux people.
// Author. Roar Thronæs.

#if 1
#define ETH_P_MYSCS 0x6009

/*
 * DECnet       An implementation of the DECnet protocol suite for the LINUX
 *              operating system.  DECnet is implemented using the  BSD Socket
 *              interface as the means of communication with the user level.
 *
 *              DECnet Device Layer
 *
 * Authors:     Steve Whitehouse <SteveW@ACM.org>
 *              Eduardo Marcelo Serrat <emserrat@geocities.com>
 *
 * Changes:
 *          Steve Whitehouse : Devices now see incoming frames so they
 *                             can mark on who it came from.
 *          Steve Whitehouse : Fixed bug in creating neighbours. Each neighbour
 *                             can now have a device specific setup func.
 *          Steve Whitehouse : Added /proc/sys/net/decnet/conf/<dev>/
 *          Steve Whitehouse : Fixed bug which sometimes killed timer
 *          Steve Whitehouse : Multiple ifaddr support
 *          Steve Whitehouse : SIOCGIFCONF is now a compile time option
 *          Steve Whitehouse : /proc/sys/net/decnet/conf/<sys>/forwarding
 *          Steve Whitehouse : Removed timer1 - its a user space issue now
 *         Patrick Caulfield : Fixed router hello message format
 */

#include <linux/config.h>
#define CONFIG_DECNET
#include <linux/net.h>
#include <linux/netdevice.h>
#include <linux/proc_fs.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/rtnetlink.h>
#include <linux/sysctl.h>
#include <asm/uaccess.h>
#include <net/neighbour.h>
#include <net/dst.h>
#include <net/dn.h>
#include <net/dn_dev.h>
#include <net/dn_route.h>
#include <net/dn_neigh.h>
#include <net/dn_fib.h>

#include <linux/utsname.h>
#include <linux/vmalloc.h>

#include <ccdef.h>
#include <cdldef.h>
#include <cdtdef.h>
#include <cdrpdef.h>
#include <cdtdef.h>
#include <chdef.h>
#include <mscpdef.h> // does not belong?
#include <nisca.h>
#include <pbdef.h>
#include <pdtdef.h>
#include <rddef.h>
#include <rdtdef.h>
#include <sbdef.h>
#include <sbnbdef.h>
#include <scsdef.h>
#include <ssdef.h>
#include <system_data_cells.h>
#include <vcdef.h>

extern struct _pb mypb;
extern struct _sb mysb;

extern struct _cdt cdtl[1024];
extern struct _rdt rdt;
extern struct _scs_rd rdtl[128];
extern struct _cdl cdl;

#define DN_IFREQ_SIZE (sizeof(struct ifreq) - sizeof(struct sockaddr) + sizeof(struct sockaddr_dn))

static char dn_rt_all_end_mcast[ETH_ALEN] = {0xAB,0x00,0x04,0x01,0x00,0x00};
static char dn_rt_all_rt_mcast[ETH_ALEN]  = {0xAB,0x00,0x04,0x01,0x00,0x00};
static char dn_hiord[ETH_ALEN]            = {0xAB,0x00,0x04,0x01,0x00,0x00}; // remember to add clusterid + 1
static unsigned char dn_eco_version[3]    = {0x02,0x00,0x00};

inline void dn_nsp_send2(struct sk_buff *skb)
{
  unsigned short int *pktlen;
  pktlen = (unsigned short *)skb_push(skb,2);
  *pktlen = dn_htons(skb->len - 2);
  skb->nh.raw = skb->data;
  skb->dev = decnet_default_device;
  //skb->dst = dst_clone(dst);
  dn_rt_finish_output2(skb,&mypb.pb$b_rstation);
  //  dev_queue_xmit(skb);
}

extern struct neigh_table dn_neigh_table;

struct net_device *decnet_default_device;

static struct dn_dev *dn_dev_create(struct net_device *dev, int *err);
static void dn_dev_delete(struct net_device *dev);
static void rtmsg_ifa(int event, struct dn_ifaddr *ifa);

static int dn_eth_up(struct net_device *);
static void dn_send_brd_hello(struct net_device *dev);
#if 0
static void dn_send_ptp_hello(struct net_device *dev);
#endif

static struct dn_dev_parms dn_dev_list[] =  {
{
	type:		ARPHRD_ETHER, /* Ethernet */
	mode:		DN_DEV_BCAST,
	state:		DN_DEV_S_RU,
	blksize:	1498,
	t2:		1,
	t3:		10,
	name:		"ethernet",
	ctl_name:	NET_DECNET_CONF_ETHER,
	up:		dn_eth_up,
	timer3:		dn_send_brd_hello,
},
{
	type:		ARPHRD_IPGRE, /* DECnet tunneled over GRE in IP */
	mode:		DN_DEV_BCAST,
	state:		DN_DEV_S_RU,
	blksize:	1400,
	t2:		1,
	t3:		10,
	name:		"ipgre",
	ctl_name:	NET_DECNET_CONF_GRE,
	timer3:		dn_send_brd_hello,
},
{
	type:		ARPHRD_LOOPBACK, /* Loopback interface - always last */
	mode:		DN_DEV_BCAST,
	state:		DN_DEV_S_RU,
	blksize:	1498,
	t2:		1,
	t3:		10,
	name:		"loopback",
	ctl_name:	NET_DECNET_CONF_LOOPBACK,
	timer3:		dn_send_brd_hello,
}
};

#define DN_DEV_LIST_SIZE (sizeof(dn_dev_list)/sizeof(struct dn_dev_parms))

#define DN_DEV_PARMS_OFFSET(x) ((int) ((char *) &((struct dn_dev_parms *)0)->x))

#ifdef CONFIG_SYSCTL

static int min_t2[] = { 1 };
static int max_t2[] = { 60 }; /* No max specified, but this seems sensible */
static int min_t3[] = { 1 };
static int max_t3[] = { 8191 }; /* Must fit in 16 bits when multiplied by BCT3MULT or T3MULT */

static int min_priority[1];
static int max_priority[] = { 127 }; /* From DECnet spec */

static int dn_forwarding_proc(ctl_table *, int, struct file *,
			void *, size_t *);
static int dn_forwarding_sysctl(ctl_table *table, int *name, int nlen,
			void *oldval, size_t *oldlenp,
			void *newval, size_t newlen,
			void **context);

static struct dn_dev_sysctl_table {
	struct ctl_table_header *sysctl_header;
	ctl_table dn_dev_vars[5];
	ctl_table dn_dev_dev[2];
	ctl_table dn_dev_conf_dir[2];
	ctl_table dn_dev_proto_dir[2];
	ctl_table dn_dev_root_dir[2];
} dn_dev_sysctl = {
	NULL,
	{
	{NET_DECNET_CONF_DEV_FORWARDING, "forwarding",
	(void *)DN_DEV_PARMS_OFFSET(forwarding),
	sizeof(int), 0644, NULL,
	dn_forwarding_proc, dn_forwarding_sysctl,
	NULL, NULL, NULL},
	{NET_DECNET_CONF_DEV_PRIORITY, "priority",
	(void *)DN_DEV_PARMS_OFFSET(priority),
	sizeof(int), 0644, NULL,
	proc_dointvec_minmax, sysctl_intvec,
	NULL, &min_priority, &max_priority},
	{NET_DECNET_CONF_DEV_T2, "t2", (void *)DN_DEV_PARMS_OFFSET(t2),
	sizeof(int), 0644, NULL,
	proc_dointvec_minmax, sysctl_intvec,
	NULL, &min_t2, &max_t2},
	{NET_DECNET_CONF_DEV_T3, "t3", (void *)DN_DEV_PARMS_OFFSET(t3),
	sizeof(int), 0644, NULL,
	proc_dointvec_minmax, sysctl_intvec,
	NULL, &min_t3, &max_t3},
	{0}
	},
	{{0, "", NULL, 0, 0555, dn_dev_sysctl.dn_dev_vars}, {0}},
	{{NET_DECNET_CONF, "conf", NULL, 0, 0555, dn_dev_sysctl.dn_dev_dev}, {0}},
	{{NET_DECNET, "myscs", NULL, 0, 0555, dn_dev_sysctl.dn_dev_conf_dir}, {0}},
	{{CTL_NET, "net", NULL, 0, 0555, dn_dev_sysctl.dn_dev_proto_dir}, {0}}
};

static void dn_dev_sysctl_register(struct net_device *dev, struct dn_dev_parms *parms)
{
	struct dn_dev_sysctl_table *t;
	int i;

	t = kmalloc(sizeof(*t), GFP_KERNEL);
	if (t == NULL)
		return;

	memcpy(t, &dn_dev_sysctl, sizeof(*t));

	for(i = 0; i < (sizeof(t->dn_dev_vars)/sizeof(t->dn_dev_vars[0]) - 1); i++) {
		long offset = (long)t->dn_dev_vars[i].data;
		t->dn_dev_vars[i].data = ((char *)parms) + offset;
		t->dn_dev_vars[i].de = NULL;
	}

	if (dev) {
		t->dn_dev_dev[0].procname = dev->name;
		t->dn_dev_dev[0].ctl_name = dev->ifindex;
	} else {
		t->dn_dev_dev[0].procname = parms->name;
		t->dn_dev_dev[0].ctl_name = parms->ctl_name;
	}

	t->dn_dev_dev[0].child = t->dn_dev_vars;
	t->dn_dev_dev[0].de = NULL;
	t->dn_dev_conf_dir[0].child = t->dn_dev_dev;
	t->dn_dev_conf_dir[0].de = NULL;
	t->dn_dev_proto_dir[0].child = t->dn_dev_conf_dir;
	t->dn_dev_proto_dir[0].de = NULL;
	t->dn_dev_root_dir[0].child = t->dn_dev_proto_dir;
	t->dn_dev_root_dir[0].de = NULL;
	t->dn_dev_vars[0].extra1 = (void *)dev;

	t->sysctl_header = register_sysctl_table(t->dn_dev_root_dir, 0);
	if (t->sysctl_header == NULL)
		kfree(t);
	else
		parms->sysctl = t;
}

static void dn_dev_sysctl_unregister(struct dn_dev_parms *parms)
{
	if (parms->sysctl) {
		struct dn_dev_sysctl_table *t = parms->sysctl;
		parms->sysctl = NULL;
		unregister_sysctl_table(t->sysctl_header);
		kfree(t);
	}
}


static int dn_forwarding_proc(ctl_table *table, int write, 
				struct file *filep,
				void *buffer, size_t *lenp)
{
	return -EINVAL;
}

static int dn_forwarding_sysctl(ctl_table *table, int *name, int nlen,
			void *oldval, size_t *oldlenp,
			void *newval, size_t newlen,
			void **context)
{
	return -EINVAL;
}

#else /* CONFIG_SYSCTL */
static void dn_dev_sysctl_unregister(struct dn_dev_parms *parms)
{
}
static void dn_dev_sysctl_register(struct net_device *dev, struct dn_dev_parms *parms)
{
}

#endif /* CONFIG_SYSCTL */

static struct dn_ifaddr *dn_dev_alloc_ifa(void)
{
	struct dn_ifaddr *ifa;

	ifa = kmalloc(sizeof(*ifa), GFP_KERNEL);

	if (ifa) {
		memset(ifa, 0, sizeof(*ifa));
	}

	return ifa;
}

static __inline__ void dn_dev_free_ifa(struct dn_ifaddr *ifa)
{
	kfree(ifa);
}

static void dn_dev_del_ifa(struct dn_dev *dn_db, struct dn_ifaddr **ifap, int destroy)
{
	struct dn_ifaddr *ifa1 = *ifap;

	*ifap = ifa1->ifa_next;

	rtmsg_ifa(RTM_DELADDR, ifa1);

	if (destroy) {
		dn_dev_free_ifa(ifa1);

		if (dn_db->ifa_list == NULL)
			dn_dev_delete(dn_db->dev);
	}
}

static int dn_dev_insert_ifa(struct dn_dev *dn_db, struct dn_ifaddr *ifa)
{
	/*
	 * FIXME: Duplicate check here.
	 */

	ifa->ifa_next = dn_db->ifa_list;
	dn_db->ifa_list = ifa;

	rtmsg_ifa(RTM_NEWADDR, ifa);

	return 0;
}

static int dn_dev_set_ifa(struct net_device *dev, struct dn_ifaddr *ifa)
{
	struct dn_dev *dn_db = dev->dn_ptr;

	if (dn_db == NULL) {
		int err;
		dn_db = dn_dev_create(dev, &err);
		if (dn_db == NULL)
			return err;
	}

	ifa->ifa_dev = dn_db;

	if (dev->flags & IFF_LOOPBACK)
		ifa->ifa_scope = RT_SCOPE_HOST;

	return dn_dev_insert_ifa(dn_db, ifa);
}


int dn_dev_ioctl(unsigned int cmd, void *arg)
{
	char buffer[DN_IFREQ_SIZE];
	struct ifreq *ifr = (struct ifreq *)buffer;
	struct sockaddr_dn *sdn = (struct sockaddr_dn *)&ifr->ifr_addr;
	struct dn_dev *dn_db;
	struct net_device *dev;
	struct dn_ifaddr *ifa = NULL, **ifap = NULL;
	int exclusive = 0;
	int ret = 0;

	if (copy_from_user(ifr, arg, DN_IFREQ_SIZE))
		return -EFAULT;
	ifr->ifr_name[IFNAMSIZ-1] = 0;

#ifdef CONFIG_KMOD
	dev_load(ifr->ifr_name);
#endif

	switch(cmd) {
		case SIOCGIFADDR:
			break;
		case SIOCSIFADDR:
			if (!capable(CAP_NET_ADMIN))
				return -EACCES;
			if (sdn->sdn_family != AF_DECnet)
				return -EINVAL;
			rtnl_lock();
			exclusive = 1;
			break;
		default:
			return -EINVAL;
	}

	if ((dev = __dev_get_by_name(ifr->ifr_name)) == NULL) {
		ret = -ENODEV;
		goto done;
	}

	if ((dn_db = dev->dn_ptr) != NULL) {
		for (ifap = &dn_db->ifa_list; (ifa=*ifap) != NULL; ifap = &ifa->ifa_next)
			if (strcmp(ifr->ifr_name, ifa->ifa_label) == 0)
				break;
	}

	if (ifa == NULL && cmd != SIOCSIFADDR) {
		ret = -EADDRNOTAVAIL;
		goto done;
	}

	switch(cmd) {
		case SIOCGIFADDR:
			*((dn_address *)sdn->sdn_nodeaddr) = ifa->ifa_local;
			goto rarok;

		case SIOCSIFADDR:
			if (!ifa) {
				if ((ifa = dn_dev_alloc_ifa()) == NULL) {
					ret = -ENOBUFS;
					break;
				}
				memcpy(ifa->ifa_label, dev->name, IFNAMSIZ);
			} else {
				if (ifa->ifa_local == dn_saddr2dn(sdn))
					break;
				dn_dev_del_ifa(dn_db, ifap, 0);
			}

			ifa->ifa_local = dn_saddr2dn(sdn);

			ret = dn_dev_set_ifa(dev, ifa);
	}
done:
	if (exclusive)
		rtnl_unlock();

	return ret;
rarok:
	if (copy_to_user(arg, ifr, DN_IFREQ_SIZE))
		return -EFAULT;

	return 0;
}

static struct dn_dev *dn_dev_by_index(int ifindex)
{
	struct net_device *dev;
	struct dn_dev *dn_dev = NULL;
	dev = dev_get_by_index(ifindex);
	if (dev) {
		dn_dev = dev->dn_ptr;
		dev_put(dev);
	}

	return dn_dev;
}

static int dn_dev_rtm_deladdr(struct sk_buff *skb, struct nlmsghdr *nlh, void *arg)
{
	struct rtattr **rta = arg;
	struct dn_dev *dn_db;
	struct ifaddrmsg *ifm = NLMSG_DATA(nlh);
	struct dn_ifaddr *ifa, **ifap;

	if ((dn_db = dn_dev_by_index(ifm->ifa_index)) == NULL)
		return -EADDRNOTAVAIL;

	for(ifap = &dn_db->ifa_list; (ifa=*ifap) != NULL; ifap = &ifa->ifa_next) {
		void *tmp = rta[IFA_LOCAL-1];
		if ((tmp && memcmp(RTA_DATA(tmp), &ifa->ifa_local, 2)) ||
				(rta[IFA_LABEL-1] && strcmp(RTA_DATA(rta[IFA_LABEL-1]), ifa->ifa_label)))
			continue;

		dn_dev_del_ifa(dn_db, ifap, 1);
		return 0;
	}

	return -EADDRNOTAVAIL;
}

static int dn_dev_rtm_newaddr(struct sk_buff *skb, struct nlmsghdr *nlh, void *arg)
{
	struct rtattr **rta = arg;
	struct net_device *dev;
	struct dn_dev *dn_db;
	struct ifaddrmsg *ifm = NLMSG_DATA(nlh);
	struct dn_ifaddr *ifa;

	if (rta[IFA_LOCAL-1] == NULL)
		return -EINVAL;

	if ((dev = __dev_get_by_index(ifm->ifa_index)) == NULL)
		return -ENODEV;

	if ((dn_db = dev->dn_ptr) == NULL) {
		int err;
		dn_db = dn_dev_create(dev, &err);
		if (!dn_db)
			return err;
	}
	
	if ((ifa = dn_dev_alloc_ifa()) == NULL)
		return -ENOBUFS;

	memcpy(&ifa->ifa_local, RTA_DATA(rta[IFA_LOCAL-1]), 2);
	ifa->ifa_flags = ifm->ifa_flags;
	ifa->ifa_scope = ifm->ifa_scope;
	ifa->ifa_dev = dn_db;
	if (rta[IFA_LABEL-1])
		memcpy(ifa->ifa_label, RTA_DATA(rta[IFA_LABEL-1]), IFNAMSIZ);
	else
		memcpy(ifa->ifa_label, dev->name, IFNAMSIZ);

	return dn_dev_insert_ifa(dn_db, ifa);
}

static int dn_dev_fill_ifaddr(struct sk_buff *skb, struct dn_ifaddr *ifa,
				u32 pid, u32 seq, int event)
{
	struct ifaddrmsg *ifm;
	struct nlmsghdr *nlh;
	unsigned char *b = skb->tail;

	nlh = NLMSG_PUT(skb, pid, seq, event, sizeof(*ifm));
	ifm = NLMSG_DATA(nlh);

	ifm->ifa_family = AF_DECnet;
	ifm->ifa_prefixlen = 16;
	ifm->ifa_flags = ifa->ifa_flags | IFA_F_PERMANENT;
	ifm->ifa_scope = ifa->ifa_scope;
	ifm->ifa_index = ifa->ifa_dev->dev->ifindex;
	RTA_PUT(skb, IFA_LOCAL, 2, &ifa->ifa_local);
	if (ifa->ifa_label[0])
		RTA_PUT(skb, IFA_LABEL, IFNAMSIZ, &ifa->ifa_label);
	nlh->nlmsg_len = skb->tail - b;
	return skb->len;

nlmsg_failure:
rtattr_failure:
        skb_trim(skb, b - skb->data);
        return -1;
}

static void rtmsg_ifa(int event, struct dn_ifaddr *ifa)
{
	struct sk_buff *skb;
	int size = NLMSG_SPACE(sizeof(struct ifaddrmsg)+128);

	skb = alloc_skb(size, GFP_KERNEL);
	if (!skb) {
		netlink_set_err(rtnl, 0, RTMGRP_DECnet_IFADDR, ENOBUFS);
		return;
	}
	if (dn_dev_fill_ifaddr(skb, ifa, 0, 0, event) < 0) {
		kfree_skb(skb);
		netlink_set_err(rtnl, 0, RTMGRP_DECnet_IFADDR, EINVAL);
		return;
	}
	NETLINK_CB(skb).dst_groups = RTMGRP_DECnet_IFADDR;
	netlink_broadcast(rtnl, skb, 0, RTMGRP_DECnet_IFADDR, GFP_KERNEL);
}

static int dn_dev_dump_ifaddr(struct sk_buff *skb, struct netlink_callback *cb)
{
	int idx, dn_idx;
	int s_idx, s_dn_idx;
	struct net_device *dev;
	struct dn_dev *dn_db;
	struct dn_ifaddr *ifa;

	s_idx = cb->args[0];
	s_dn_idx = dn_idx = cb->args[1];
	read_lock(&dev_base_lock);
	for(dev = dev_base, idx = 0; dev; dev = dev->next) {
		if ((dn_db = dev->dn_ptr) == NULL)
			continue;
		idx++;
		if (idx < s_idx)
			continue;
		if (idx > s_idx)
			s_dn_idx = 0;
		if ((dn_db = dev->dn_ptr) == NULL)
			continue;

		for(ifa = dn_db->ifa_list, dn_idx = 0; ifa; ifa = ifa->ifa_next, dn_idx++) {
			if (dn_idx < s_dn_idx)
				continue;

			if (dn_dev_fill_ifaddr(skb, ifa, NETLINK_CB(cb->skb).pid, cb->nlh->nlmsg_seq, RTM_NEWADDR) <= 0)
				goto done;
		}
	}
done:
	read_unlock(&dev_base_lock);
	cb->args[0] = idx;
	cb->args[1] = dn_idx;

	return skb->len;
}

static inline void dn_rt_finish_output2(struct sk_buff *skb, char *dst)
{
  struct net_device *dev = skb->dev;

  if ((dev->type != ARPHRD_ETHER) && (dev->type != ARPHRD_LOOPBACK))
    dst = NULL;

  if (!dev->hard_header || (dev->hard_header(skb, dev, ETH_P_MYSCS,
					     dst, NULL, skb->len) >= 0))
    dn_rt_send(skb);
  else
    kfree_skb(skb);
}

// this is not right, but works
void scs_fill_dx(struct _nisca * msg,unsigned short dx_hi,unsigned long dx_lo,unsigned short sr_hi,unsigned long sr_lo) {
  msg->nisca$w_dx_group=0xfffe;
  msg->nisca$w_dx_dst_hi=dx_hi;
  msg->nisca$l_dx_dst_lo=dx_lo;
  msg->nisca$w_dx_src_hi=sr_hi;
  msg->nisca$l_dx_src_lo=sr_lo;
}

int scs_from_myself(struct _nisca * msg,unsigned short sr_hi,unsigned long sr_lo) {
  return 
  (msg->nisca$w_dx_src_hi==sr_hi) &&
  (msg->nisca$l_dx_src_lo==sr_lo);
}

static void dn_send_endnode_hello(struct net_device *dev)
{
        struct _nisca *intro;
        struct _nisca *nisca, *dx;
        struct sk_buff *skb = NULL;
        unsigned short int *pktlen;
	void * msg;
	struct dn_dev *dn_db = (struct dn_dev *)dev->dn_ptr;

        if ((skb = dn_alloc_skb2(NULL, sizeof(*nisca)+16, GFP_ATOMIC)) == NULL)
		return;

        skb->dev = dev;

	msg = skb_put(skb,sizeof(*nisca));
	skb_put(skb,16);

        intro = msg;

	dx=getdx(msg);

	scs_fill_dx(msg,0xab00,0x04010000,0xaa00,(system_utsname.nodename[0]<<16)+system_utsname.nodename[1]);

	nisca=getcc(msg);

        nisca->nisca$b_msg  = NISCA$C_HELLO;
        nisca->nisca$b_msg  |= NISCA$M_TR_CTL|NISCA$M_TR_CCFLG;
	nisca->nisca$l_maint  = NISCA$M_MAINT;
	if (strlen(system_utsname.nodename)) {
	  nisca->nisca$t_nodename[0]=4; //strlen(system_utsname.nodename);
	  memcpy(&nisca->nisca$t_nodename[1],system_utsname.nodename,4);
	  memcpy(&nisca->nisca$ab_lan_hw_addr,dev->dev_addr,6);
	  
	} else {
	  nisca->nisca$t_nodename[0]=6;
	  memcpy(&nisca->nisca$t_nodename[1],"NODNAM",6);
	}
#if 0
        memcpy(msg->tiver, dn_eco_version, 3);
        memcpy(msg->id, decnet_ether_address, 6);
        msg->iinfo   = DN_RT_INFO_ENDN;
        msg->blksize = dn_htons(dn_db->parms.blksize);
        msg->area    = 0x00;
        memset(msg->seed, 0, 8);
        memcpy(msg->neighbor, dn_hiord, ETH_ALEN);

	if (dn_db->router) {
		struct dn_neigh *dn = (struct dn_neigh *)dn_db->router;
		dn_dn2eth(msg->neighbor, dn->addr);
	}

        msg->timer   = dn_htons((unsigned short)dn_db->parms.t3);
        msg->mpd     = 0x00;
        msg->datalen = 0x02;
        memset(msg->data, 0xAA, 2);
#endif

        pktlen = (unsigned short *)skb_push(skb,2);
        *pktlen = dn_htons(skb->len - 2);

	skb->nh.raw = skb->data;

	dn_rt_finish_output2(skb, dn_rt_all_rt_mcast);
}


static void dn_send_brd_hello(struct net_device *dev)
{
	dn_send_endnode_hello(dev);
}

static int dn_eth_up(struct net_device *dev)
{
	struct dn_dev *dn_db = dev->dn_ptr;

	if (dn_db->parms.forwarding == 0)
		dev_mc_add(dev, dn_rt_all_end_mcast, ETH_ALEN, 0);
	else
		dev_mc_add(dev, dn_rt_all_rt_mcast, ETH_ALEN, 0);

	dev_mc_upload(dev);

	dn_db->use_long = 1;

	return 0;
}

static void dn_dev_set_timer(struct net_device *dev);

static void dn_dev_timer_func(unsigned long arg)
{
	struct net_device *dev = (struct net_device *)arg;
	struct dn_dev *dn_db = dev->dn_ptr;

	if (dn_db->t3 <= dn_db->parms.t2) {
		if (dn_db->parms.timer3)
			dn_db->parms.timer3(dev);
		dn_db->t3 = dn_db->parms.t3;
	} else {
		dn_db->t3 -= dn_db->parms.t2;
	}

	dn_dev_set_timer(dev);
}

static void dn_dev_set_timer(struct net_device *dev)
{
	struct dn_dev *dn_db = dev->dn_ptr;

	if (dn_db->parms.t2 > dn_db->parms.t3)
		dn_db->parms.t2 = dn_db->parms.t3;

	dn_db->timer.data = (unsigned long)dev;
	dn_db->timer.function = dn_dev_timer_func;
	dn_db->timer.expires = jiffies + (dn_db->parms.t2 * HZ);

	add_timer(&dn_db->timer);
}

struct dn_dev *dn_dev_create(struct net_device *dev, int *err)
{
	int i;
	struct dn_dev_parms *p = dn_dev_list;
	struct dn_dev *dn_db;

	for(i = 0; i < DN_DEV_LIST_SIZE; i++, p++) {
		if (p->type == dev->type)
			break;
	}

	*err = -ENODEV;
	if (i == DN_DEV_LIST_SIZE)
		return NULL;

	*err = -ENOBUFS;
	if ((dn_db = kmalloc(sizeof(struct dn_dev), GFP_ATOMIC)) == NULL)
		return NULL;

	memset(dn_db, 0, sizeof(struct dn_dev));
	memcpy(&dn_db->parms, p, sizeof(struct dn_dev_parms));
	dev->dn_ptr = dn_db;
	dn_db->dev = dev;
	init_timer(&dn_db->timer);

	memcpy(dn_db->addr, decnet_ether_address, ETH_ALEN); /* To go... */

	dn_db->uptime = jiffies;
	if (dn_db->parms.up) {
		if (dn_db->parms.up(dev) < 0) {
			dev->dn_ptr = NULL;
			kfree(dn_db);
			return NULL;
		}
	}

	dn_db->neigh_parms = neigh_parms_alloc(dev, &dn_neigh_table);
	/* dn_db->neigh_parms->neigh_setup = dn_db->parms.neigh_setup; */

	dn_dev_sysctl_register(dev, &dn_db->parms);

	dn_dev_set_timer(dev);

	*err = 0;
	return dn_db;
}


/*
 * This processes a device up event. We only start up
 * the loopback device & ethernet devices with correct
 * MAC addreses automatically. Others must be started
 * specifically.
 */
void dn_dev_up(struct net_device *dev)
{
	struct dn_ifaddr *ifa;

	if ((dev->type != ARPHRD_ETHER) && (dev->type != ARPHRD_LOOPBACK))
		return;

	if (dev->type == ARPHRD_ETHER)
		if (memcmp(dev->dev_addr, decnet_ether_address, ETH_ALEN) != 0)
			return;

	if ((ifa = dn_dev_alloc_ifa()) == NULL)
		return;

	ifa->ifa_local = decnet_address;
	ifa->ifa_flags = 0;
	ifa->ifa_scope = RT_SCOPE_UNIVERSE;
	strcpy(ifa->ifa_label, dev->name);

	dn_dev_set_ifa(dev, ifa);
}

static void dn_dev_delete(struct net_device *dev)
{
	struct dn_dev *dn_db = dev->dn_ptr;

	if (dn_db == NULL)
		return;

	del_timer_sync(&dn_db->timer);

	dn_dev_sysctl_unregister(&dn_db->parms);

	neigh_ifdown(&dn_neigh_table, dev);

	if (dev == decnet_default_device)
		decnet_default_device = NULL;

	if (dn_db->parms.down)
		dn_db->parms.down(dev);

	dev->dn_ptr = NULL;

	neigh_parms_release(&dn_neigh_table, dn_db->neigh_parms);

	if (dn_db->router)
		neigh_release(dn_db->router);
	if (dn_db->peer)
		neigh_release(dn_db->peer);

	kfree(dn_db);
}

void dn_dev_down(struct net_device *dev)
{
	struct dn_dev *dn_db = dev->dn_ptr;
	struct dn_ifaddr *ifa;

	if (dn_db == NULL)
		return;

	while((ifa = dn_db->ifa_list) != NULL) {
		dn_dev_del_ifa(dn_db, &dn_db->ifa_list, 0);
		dn_dev_free_ifa(ifa);
	}

	dn_dev_delete(dev);
}

void dn_dev_init_pkt(struct sk_buff *skb)
{
	return;
}

void dn_dev_veri_pkt(struct sk_buff *skb)
{
	return;
}

void dn_dev_hello(struct sk_buff *skb)
{
	return;
}

void dn_dev_devices_off(void)
{
	struct net_device *dev;

	for(dev = dev_base; dev; dev = dev->next)
		dn_dev_down(dev);

}

void dn_dev_devices_on(void)
{
	struct net_device *dev;

	for(dev = dev_base; dev; dev = dev->next) {
		if (dev->flags & IFF_UP)
			dn_dev_up(dev);
	}
}


#ifdef CONFIG_DECNET_SIOCGIFCONF
/*
 * Now we support multiple addresses per interface.
 * Since we don't want to break existing code, you have to enable
 * it as a compile time option. Probably you should use the
 * rtnetlink interface instead.
 */
int dnet_gifconf(struct net_device *dev, char *buf, int len)
{
	struct dn_dev *dn_db = (struct dn_dev *)dev->dn_ptr;
	struct dn_ifaddr *ifa;
	struct ifreq *ifr = (struct ifreq *)buf;
	int done = 0;

	if ((dn_db == NULL) || ((ifa = dn_db->ifa_list) == NULL))
		return 0;

	for(; ifa; ifa = ifa->ifa_next) {
		if (!ifr) {
			done += sizeof(DN_IFREQ_SIZE);
			continue;
		}
		if (len < DN_IFREQ_SIZE)
			return done;
		memset(ifr, 0, DN_IFREQ_SIZE);

		if (ifa->ifa_label)
			strcpy(ifr->ifr_name, ifa->ifa_label);
		else
			strcpy(ifr->ifr_name, dev->name);

		(*(struct sockaddr_dn *) &ifr->ifr_addr).sdn_family = AF_DECnet;
		(*(struct sockaddr_dn *) &ifr->ifr_addr).sdn_add.a_len = 2;
		(*(dn_address *)(*(struct sockaddr_dn *) &ifr->ifr_addr).sdn_add.a_addr) = ifa->ifa_local;

		ifr = (struct ifreq *)((char *)ifr + DN_IFREQ_SIZE);
		len  -= DN_IFREQ_SIZE;
		done += DN_IFREQ_SIZE;
	}

	return done;
}
#endif /* CONFIG_DECNET_SIOCGIFCONF */


#ifdef CONFIG_PROC_FS

static char *dn_type2asc(char type)
{
	switch(type) {
		case DN_DEV_BCAST:
			return "B";
		case DN_DEV_UCAST:
			return "U";
		case DN_DEV_MPOINT:
			return "M";
	}

	return "?";
}

static int decnet_dev_get_info(char *buffer, char **start, off_t offset, int length)
{
        struct dn_dev *dn_db;
	struct net_device *dev;
        int len = 0;
        off_t pos = 0;
        off_t begin = 0;
	char peer_buf[DN_ASCBUF_LEN];
	char router_buf[DN_ASCBUF_LEN];


        len += sprintf(buffer, "Name     Flags T1   Timer1 T3   Timer3 BlkSize Pri State DevType    Router Peer\n");

	read_lock(&dev_base_lock);
        for (dev = dev_base; dev; dev = dev->next) {
		if ((dn_db = (struct dn_dev *)dev->dn_ptr) == NULL)
			continue;

                len += sprintf(buffer + len, "%-8s %1s     %04u %04u   %04lu %04lu   %04hu    %03d %02x    %-10s %-7s %-7s\n",
                             	dev->name ? dev->name : "???",
                             	dn_type2asc(dn_db->parms.mode),
                             	0, 0,
				dn_db->t3, dn_db->parms.t3,
				dn_db->parms.blksize,
				dn_db->parms.priority,
				dn_db->parms.state, dn_db->parms.name,
				dn_db->router ? dn_addr2asc(dn_ntohs(*(dn_address *)dn_db->router->primary_key), router_buf) : "",
				dn_db->peer ? dn_addr2asc(dn_ntohs(*(dn_address *)dn_db->peer->primary_key), peer_buf) : "");


                pos = begin + len;

                if (pos < offset) {
                        len   = 0;
                        begin = pos;
                }
                if (pos > offset + length)
                        break;
        }

	read_unlock(&dev_base_lock);

        *start = buffer + (offset - begin);
        len   -= (offset - begin);

        if (len > length) len = length;

        return(len);
}

#endif /* CONFIG_PROC_FS */

static struct rtnetlink_link dnet_rtnetlink_table[RTM_MAX-RTM_BASE+1] = 
{
	{ NULL,			NULL,			},
	{ NULL,			NULL,			},
	{ NULL,			NULL,			},
	{ NULL,			NULL,			},
	{ dn_dev_rtm_newaddr,	NULL,			},
	{ dn_dev_rtm_deladdr,	NULL,			},
	{ NULL,			dn_dev_dump_ifaddr,	},
	{ NULL,			NULL,			},

	{ NULL,			NULL,			},
	{ NULL,			NULL,			},
	{ dn_cache_getroute,	dn_cache_dump,		},
	{ NULL,			NULL,			},
	{ NULL,			NULL,			},
	{ NULL,			NULL,			},
	{ NULL,			NULL,			},
	{ NULL,			NULL,			},
	{ NULL,			NULL,			},
	{ NULL,			NULL,			},
	{ NULL,			NULL,			},
	{ NULL,			NULL,			}
};

void __init dn_dev_init(void)
{

	dn_dev_devices_on();
#ifdef CONFIG_DECNET_SIOCGIFCONF
	register_gifconf(PF_DECnet, dnet_gifconf);
#endif /* CONFIG_DECNET_SIOCGIFCONF */

	rtnetlink_links[PF_DECnet] = dnet_rtnetlink_table;

#ifdef CONFIG_PROC_FS
	proc_net_create("myscs_dev", 0, decnet_dev_get_info);
#endif /* CONFIG_PROC_FS */

#ifdef CONFIG_SYSCTL
	{
		int i;
		for(i = 0; i < DN_DEV_LIST_SIZE; i++)
			dn_dev_sysctl_register(NULL, &dn_dev_list[i]);
	}
#endif /* CONFIG_SYSCTL */
}

void __exit dn_dev_cleanup(void)
{
	rtnetlink_links[PF_DECnet] = NULL;

#ifdef CONFIG_DECNET_SIOCGIFCONF
	unregister_gifconf(PF_DECnet);
#endif /* CONFIG_DECNET_SIOCGIFCONF */

#ifdef CONFIG_SYSCTL
	{
		int i;
		for(i = 0; i < DN_DEV_LIST_SIZE; i++)
			dn_dev_sysctl_unregister(&dn_dev_list[i]);
	}
#endif /* CONFIG_SYSCTL */

	proc_net_remove("myscs_dev");

	dn_dev_devices_off();
}
/*
 * DECnet       An implementation of the DECnet protocol suite for the LINUX
 *              operating system.  DECnet is implemented using the  BSD Socket
 *              interface as the means of communication with the user level.
 *
 *              DECnet Neighbour Functions (Adjacency Database and 
 *                                                        On-Ethernet Cache)
 *
 * Author:      Steve Whitehouse <SteveW@ACM.org>
 *
 *
 * Changes:
 *     Steve Whitehouse     : Fixed router listing routine
 *     Steve Whitehouse     : Added error_report functions
 *     Steve Whitehouse     : Added default router detection
 *     Steve Whitehouse     : Hop counts in outgoing messages
 *     Steve Whitehouse     : Fixed src/dst in outgoing messages so
 *                            forwarding now stands a good chance of
 *                            working.
 *     Steve Whitehouse     : Fixed neighbour states (for now anyway).
 *     Steve Whitehouse     : Made error_report functions dummies. This
 *                            is not the right place to return skbs.
 *
 */

#include <linux/net.h>
#include <linux/socket.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/netfilter_decnet.h>
#include <linux/spinlock.h>
#include <asm/atomic.h>
#include <net/neighbour.h>
#include <net/dst.h>
#include <net/dn.h>
#include <net/dn_dev.h>
#include <net/dn_neigh.h>
#include <net/dn_route.h>

static u32 dn_neigh_hash(const void *pkey, const struct net_device *dev);
static int dn_neigh_construct(struct neighbour *);
static void dn_long_error_report(struct neighbour *, struct sk_buff *);
static void dn_short_error_report(struct neighbour *, struct sk_buff *);
static int dn_long_output(struct sk_buff *);
static int dn_short_output(struct sk_buff *);
static int dn_phase3_output(struct sk_buff *);


/*
 * For talking to broadcast devices: Ethernet & PPP
 */
static struct neigh_ops dn_long_ops = {
	family:			AF_DECnet,
	error_report:		dn_long_error_report,
	output:			dn_long_output,
	connected_output:	dn_long_output,
	hh_output:		dev_queue_xmit,
	queue_xmit:		dev_queue_xmit,
};

/*
 * For talking to pointopoint and multidrop devices: DDCMP and X.25
 */
static struct neigh_ops dn_short_ops = {
	family:			AF_DECnet,
	error_report:		dn_short_error_report,
	output:			dn_short_output,
	connected_output:	dn_short_output,
	hh_output:		dev_queue_xmit,
	queue_xmit:		dev_queue_xmit,
};

/*
 * For talking to DECnet phase III nodes
 */
static struct neigh_ops dn_phase3_ops = {
	family:			AF_DECnet,
	error_report:		dn_short_error_report, /* Can use short version here */
	output:			dn_phase3_output,
	connected_output:	dn_phase3_output,
	hh_output:		dev_queue_xmit,
	queue_xmit:		dev_queue_xmit
};

struct neigh_table dn_neigh_table = {
	family:				PF_DECnet,
	entry_size:			sizeof(struct dn_neigh),
	key_len:			sizeof(dn_address),
	hash:				dn_neigh_hash,
	constructor:			dn_neigh_construct,
	id:				"dn_neigh_cache",
	parms:	{
		tbl:			&dn_neigh_table,
		entries:		0,
		base_reachable_time:	30 * HZ,
		retrans_time:		1 * HZ,
		gc_staletime:		60 * HZ,
		reachable_time:		30 * HZ,
		delay_probe_time:	5 * HZ,
		queue_len:		3,
		ucast_probes:		0,
		app_probes:		0,
		mcast_probes:		0,
		anycast_delay:		0,
		proxy_delay:		0,
		proxy_qlen:		0,
		locktime:		1 * HZ,
	},
	gc_interval:			30 * HZ,
	gc_thresh1:			128,
	gc_thresh2:			512,
	gc_thresh3:			1024,
};

static u32 dn_neigh_hash(const void *pkey, const struct net_device *dev)
{
	u32 hash_val;

	hash_val = *(dn_address *)pkey;
	hash_val ^= (hash_val >> 10);
	hash_val ^= (hash_val >> 3);

	return hash_val & NEIGH_HASHMASK;
}

static int dn_neigh_construct(struct neighbour *neigh)
{
	struct net_device *dev = neigh->dev;
	struct dn_neigh *dn = (struct dn_neigh *)neigh;
	struct dn_dev *dn_db = (struct dn_dev *)dev->dn_ptr;

	if (dn_db == NULL)
		return -EINVAL;

	if (dn_db->neigh_parms)
		neigh->parms = dn_db->neigh_parms;

	if (dn_db->use_long)
		neigh->ops = &dn_long_ops;
	else
		neigh->ops = &dn_short_ops;

	if (dn->flags & DN_NDFLAG_P3)
		neigh->ops = &dn_phase3_ops;

	neigh->nud_state = NUD_NOARP;
	neigh->output = neigh->ops->connected_output;

	if ((dev->type == ARPHRD_IPGRE) || (dev->flags & IFF_POINTOPOINT))
		memcpy(neigh->ha, dev->broadcast, dev->addr_len);
	else if ((dev->type == ARPHRD_ETHER) || (dev->type == ARPHRD_LOOPBACK))
		dn_dn2eth(neigh->ha, dn->addr);
	else {
		if (net_ratelimit())
			printk(KERN_DEBUG "Trying to create neigh for hw %d\n",  dev->type);
		return -EINVAL;
	}

	dn->blksize = 230;

	return 0;
}

static void dn_long_error_report(struct neighbour *neigh, struct sk_buff *skb)
{
	printk(KERN_DEBUG "dn_long_error_report: called\n");
	kfree_skb(skb);
}


static void dn_short_error_report(struct neighbour *neigh, struct sk_buff *skb)
{
	printk(KERN_DEBUG "dn_short_error_report: called\n");
	kfree_skb(skb);
}

static int dn_neigh_output_packet(struct sk_buff *skb)
{
	struct dst_entry *dst = skb->dst;
	struct neighbour *neigh = dst->neighbour;
	struct net_device *dev = neigh->dev;

	if (!dev->hard_header || dev->hard_header(skb, dev, ntohs(skb->protocol), neigh->ha, NULL, skb->len) >= 0)
		return neigh->ops->queue_xmit(skb);

	if (net_ratelimit())
		printk(KERN_DEBUG "dn_neigh_output_packet: oops, can't send packet\n");

	kfree_skb(skb);
	return -EINVAL;
}

static int dn_long_output(struct sk_buff *skb)
{
	struct dst_entry *dst = skb->dst;
	struct neighbour *neigh = dst->neighbour;
	struct net_device *dev = neigh->dev;
	int headroom = dev->hard_header_len + sizeof(struct dn_long_packet) + 3;
	unsigned char *data;
	struct dn_long_packet *lp;
	struct _cdt *cb = (skb);


	if (skb_headroom(skb) < headroom) {
		struct sk_buff *skb2 = skb_realloc_headroom(skb, headroom);
		if (skb2 == NULL) {
			if (net_ratelimit())
				printk(KERN_CRIT "dn_long_output: no memory\n");
			kfree_skb(skb);
			return -ENOBUFS;
		}
		kfree_skb(skb);
		skb = skb2;
		if (net_ratelimit())
			printk(KERN_INFO "dn_long_output: Increasing headroom\n");
	}

	data = skb_push(skb, sizeof(struct dn_long_packet) + 3);
	lp = (struct dn_long_packet *)(data+3);

	*((unsigned short *)data) = dn_htons(skb->len - 2);
	*(data + 2) = 1 | DN_RT_F_PF; /* Padding */

	lp->msgflg   = DN_RT_PKT_LONG|(cb->cdt$b_ls_flag&(DN_RT_F_IE|DN_RT_F_RQR|DN_RT_F_RTS));
	lp->d_area   = lp->d_subarea = 0;
	dn_dn2eth(lp->d_id, dn_ntohs(cb->cdt$l_rconid));
	lp->s_area   = lp->s_subarea = 0;
	dn_dn2eth(lp->s_id, dn_ntohs(cb->cdt$l_lconid));
	lp->nl2      = 0;

	lp->s_class  = 0;
	lp->pt       = 0;

	skb->nh.raw = skb->data;

	return NF_HOOK(PF_DECnet, NF_DN_POST_ROUTING, skb, NULL, neigh->dev, dn_neigh_output_packet);
}

static int dn_short_output(struct sk_buff *skb)
{
	struct dst_entry *dst = skb->dst;
	struct neighbour *neigh = dst->neighbour;
	struct net_device *dev = neigh->dev;
	int headroom = dev->hard_header_len + sizeof(struct dn_short_packet) + 2;
	struct dn_short_packet *sp;
	unsigned char *data;
	struct _cdt *cb = (skb);


        if (skb_headroom(skb) < headroom) {
                struct sk_buff *skb2 = skb_realloc_headroom(skb, headroom);
                if (skb2 == NULL) {
			if (net_ratelimit())
                        	printk(KERN_CRIT "dn_short_output: no memory\n");
                        kfree_skb(skb);
                        return -ENOBUFS;
                }
                kfree_skb(skb);
                skb = skb2;
		if (net_ratelimit())
                	printk(KERN_INFO "dn_short_output: Increasing headroom\n");
        }

	data = skb_push(skb, sizeof(struct dn_short_packet) + 2);
	*((unsigned short *)data) = dn_htons(skb->len - 2);
	sp = (struct dn_short_packet *)(data+2);

	sp->msgflg     = DN_RT_PKT_SHORT|(cb->cdt$b_ls_flag&(DN_RT_F_RQR|DN_RT_F_RTS));
	sp->dstnode    = cb->cdt$l_rconid;
	sp->srcnode    = cb->cdt$l_lconid;


	skb->nh.raw = skb->data;

	return NF_HOOK(PF_DECnet, NF_DN_POST_ROUTING, skb, NULL, neigh->dev, dn_neigh_output_packet);
}

/*
 * Phase 3 output is the same is short output, execpt that
 * it clears the area bits before transmission.
 */
static int dn_phase3_output(struct sk_buff *skb)
{
	struct dst_entry *dst = skb->dst;
	struct neighbour *neigh = dst->neighbour;
	struct net_device *dev = neigh->dev;
	int headroom = dev->hard_header_len + sizeof(struct dn_short_packet) + 2;
	struct dn_short_packet *sp;
	unsigned char *data;
	struct _cdt *cb = (skb);

	if (skb_headroom(skb) < headroom) {
		struct sk_buff *skb2 = skb_realloc_headroom(skb, headroom);
		if (skb2 == NULL) {
			if (net_ratelimit())
				printk(KERN_CRIT "dn_phase3_output: no memory\n");
			kfree_skb(skb);
			return -ENOBUFS;
		}
		kfree_skb(skb);
		skb = skb2;
		if (net_ratelimit())
			printk(KERN_INFO "dn_phase3_output: Increasing headroom\n");
	}

	data = skb_push(skb, sizeof(struct dn_short_packet) + 2);
	((unsigned short *)data) = dn_htons(skb->len - 2);
	sp = (struct dn_short_packet *)(data + 2);

	sp->msgflg   = DN_RT_PKT_SHORT|(cb->cdt$b_ls_flag&(DN_RT_F_RQR|DN_RT_F_RTS));
	sp->dstnode  = cb->cdt$l_rconid & dn_htons(0x03ff);
	sp->srcnode  = cb->cdt$l_lconid & dn_htons(0x03ff);


	skb->nh.raw = skb->data;

	return NF_HOOK(PF_DECnet, NF_DN_POST_ROUTING, skb, NULL, neigh->dev, dn_neigh_output_packet);
}

/*
 * Unfortunately, the neighbour code uses the device in its hash
 * function, so we don't get any advantage from it. This function
 * basically does a neigh_lookup(), but without comparing the device
 * field. This is required for the On-Ethernet cache
 */
struct neighbour *dn_neigh_lookup(struct neigh_table *tbl, void *ptr)
{
	struct neighbour *neigh;
	u32 hash_val;

	hash_val = tbl->hash(ptr, NULL);

	read_lock_bh(&tbl->lock);
	for(neigh = tbl->hash_buckets[hash_val]; neigh != NULL; neigh = neigh->next) {
		if (memcmp(neigh->primary_key, ptr, tbl->key_len) == 0) {
			atomic_inc(&neigh->refcnt);
			read_unlock_bh(&tbl->lock);
			return neigh;
		}
	}
	read_unlock_bh(&tbl->lock);

	return NULL;
}


/*
 * Any traffic on a pointopoint link causes the timer to be reset
 * for the entry in the neighbour table.
 */
void dn_neigh_pointopoint_notify(struct sk_buff *skb)
{
	return;
}

/*
 * Pointopoint link receives a hello message
 */
void dn_neigh_pointopoint_hello(struct sk_buff *skb)
{
	kfree_skb(skb);
}

static int first_hello=0;

/*
 * Endnode hello message received
 */
int dn_neigh_endnode_hello(struct sk_buff *skb)
{
	struct _nisca *msg = skb->data;
	struct neighbour *neigh;
	struct dn_neigh *dn;
	dn_address src;

	msg=getcc(msg);

 	if (0==strncmp("NODNAM",&msg->nisca$t_nodename[1],6))
	  return 1;

 	if (0==strncmp(&system_utsname.nodename,&msg->nisca$t_nodename[1],4))
	  return 1;

	if (!first_hello) {
	  first_hello++;
	  printk("scs received hello from node %s\n",&msg->nisca$t_nodename[1]);
	}

	// next should be more protocol-stuff or first a plain init

	mypb.pb$w_state=PB$C_OPEN;

	memcpy(&mypb.pb$b_rstation,&msg->nisca$ab_lan_hw_addr,6);
	memcpy(&mysb.sb$t_nodename,&msg->nisca$t_nodename,8);

#if 0
	src = dn_htons(dn_eth2dn(msg->id));

	neigh = __neigh_lookup(&dn_neigh_table, &src, skb->dev, 1);

	dn = (struct dn_neigh *)neigh;

	if (neigh) {
		write_lock(&neigh->lock);

		neigh->used = jiffies;

		if (!(neigh->nud_state & NUD_PERMANENT)) {
			neigh->updated = jiffies;

			if (neigh->dev->type == ARPHRD_ETHER)
				memcpy(neigh->ha, &skb->mac.ethernet->h_source, ETH_ALEN);
			dn->flags   &= ~(DN_NDFLAG_R1 | DN_NDFLAG_R2);
			dn->blksize  = dn_ntohs(msg->blksize);
			dn->priority = 0;
		}

		write_unlock(&neigh->lock);
		neigh_release(neigh);
	}
#endif

	kfree_skb(skb);
	return 0;
}

#ifdef CONFIG_PROC_FS
static int dn_neigh_get_info(char *buffer, char **start, off_t offset, int length)
{
        int len     = 0;
        off_t pos   = 0;
        off_t begin = 0;
	struct neighbour *n;
	int i;
	char buf[DN_ASCBUF_LEN];

	len += sprintf(buffer + len, "Addr    Flags State Use Blksize Dev\n");

	for(i=0;i <= NEIGH_HASHMASK; i++) {
		read_lock_bh(&dn_neigh_table.lock);
		n = dn_neigh_table.hash_buckets[i];
		for(; n != NULL; n = n->next) {
			struct dn_neigh *dn = (struct dn_neigh *)n;

			read_lock(&n->lock);
			len += sprintf(buffer+len, "%-7s %s%s%s   %02x    %02d  %07ld %-8s\n",
					dn_addr2asc(dn_ntohs(dn->addr), buf),
					(dn->flags&DN_NDFLAG_R1) ? "1" : "-",
					(dn->flags&DN_NDFLAG_R2) ? "2" : "-",
					(dn->flags&DN_NDFLAG_P3) ? "3" : "-",
					dn->n.nud_state,
					atomic_read(&dn->n.refcnt),
					dn->blksize,
					(dn->n.dev) ? dn->n.dev->name : "?");
			read_unlock(&n->lock);

			pos = begin + len;

                	if (pos < offset) {
                        	len = 0;
                        	begin = pos;
                	}

                	if (pos > offset + length) {
				read_unlock_bh(&dn_neigh_table.lock);
                       		goto done;
			}
		}
		read_unlock_bh(&dn_neigh_table.lock);
	}

done:

        *start = buffer + (offset - begin);
        len   -= offset - begin;

        if (len > length) len = length;

        return len;
}

#endif

void __init dn_neigh_init(void)
{
	neigh_table_init(&dn_neigh_table);

#ifdef CONFIG_PROC_FS
	proc_net_create("myscs_neigh",0,dn_neigh_get_info);
#endif /* CONFIG_PROC_FS */
}

void __exit dn_neigh_cleanup(void)
{
	proc_net_remove("myscs_neigh");
	neigh_table_clear(&dn_neigh_table);
}
/*
 * DECnet       An implementation of the DECnet protocol suite for the LINUX
 *              operating system.  DECnet is implemented using the  BSD Socket
 *              interface as the means of communication with the user level.
 *
 *              DECnet Network Services Protocol (Input)
 *
 * Author:      Eduardo Marcelo Serrat <emserrat@geocities.com>
 *
 * Changes:
 *
 *    Steve Whitehouse:  Split into dn_nsp_in.c and dn_nsp_out.c from
 *                       original dn_nsp.c.
 *    Steve Whitehouse:  Updated to work with my new routing architecture.
 *    Steve Whitehouse:  Add changes from Eduardo Serrat's patches.
 *    Steve Whitehouse:  Put all ack handling code in a common routine.
 *    Steve Whitehouse:  Put other common bits into dn_nsp_rx()
 *    Steve Whitehouse:  More checks on skb->len to catch bogus packets
 *                       Fixed various race conditions and possible nasties.
 *    Steve Whitehouse:  Now handles returned conninit frames.
 *     David S. Miller:  New socket locking
 *    Steve Whitehouse:  Fixed lockup when socket filtering was enabled.
 *         Paul Koning:  Fix to push CC sockets into RUN when acks are
 *                       received.
 *    Steve Whitehouse:
 *   Patrick Caulfield:  Checking conninits for correctness & sending of error
 *                       responses.
 *    Steve Whitehouse:  Added backlog congestion level return codes.
 *   Patrick Caulfield:
 *    Steve Whitehouse:  Added flow control support (outbound)
 */

/******************************************************************************
    (c) 1995-1998 E.M. Serrat		emserrat@geocities.com
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*******************************************************************************/

#include <linux/errno.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/sockios.h>
#include <linux/net.h>
#include <linux/netdevice.h>
#include <linux/inet.h>
#include <linux/route.h>
#include <net/sock.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/termios.h>      
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/netfilter_decnet.h>
#include <net/neighbour.h>
#include <net/dst.h>
#include <net/dn_nsp.h>
#include <net/dn_dev.h>
#include <net/dn_route.h>

extern int decnet_log_martians;

static void dn_log_martian(struct sk_buff *skb, const char *msg)
{
	if (decnet_log_martians && net_ratelimit()) {
		char *devname = skb->dev ? skb->dev->name : "???";
		struct _cdt *cb = (skb);
		printk(KERN_INFO "DECnet: Martian packet (%s) dev=%s src=0x%04hx dst=0x%04hx srcport=0x%04hx dstport=0x%04hx\n", msg, devname, cb->cdt$l_lconid, cb->cdt$l_rconid, cb->cdt$l_lconid, cb->cdt$l_rconid);
	}
}

/*
 * For this function we've flipped the cross-subchannel bit
 * if the message is an otherdata or linkservice message. Thus
 * we can use it to work out what to update.
 */
static void dn_ack(struct _cdt *sk, struct sk_buff *skb, unsigned short ack)
{
	struct _cdt *scp = sk;
	unsigned short type = ((ack >> 12) & 0x0003);
	int wakeup = 0;

	switch(type) {
		case 0: /* ACK - Data */
			if (after(ack, scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr)) {
				scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr = ack & 0x0fff;
				wakeup |= dn_nsp_check_xmit_queue2(sk, skb, &scp->cdt$l_waitqfl, ack);
			}
			break;
		case 1: /* NAK - Data */
			break;
		case 2: /* ACK - OtherData */
			if (after(ack, scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr)) {
				scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr = ack & 0x0fff;
				wakeup |= dn_nsp_check_xmit_queue2(sk, skb, &scp->cdt$l_waitqfl, ack);
			}
			break;
		case 3: /* NAK - OtherData */
			break;
	}

}

/*
 * This function is a universal ack processor.
 */
static int dn_process_ack(struct _cdt *sk, struct sk_buff *skb, int oth)
{
	unsigned short *ptr = (unsigned short *)skb->data;
	int len = 0;
	unsigned short ack;

	if (skb->len < 2)
		return len;

	if ((ack = dn_ntohs(*ptr)) & 0x8000) {
		skb_pull(skb, 2);
		ptr++;
		len += 2;
		if ((ack & 0x4000) == 0) {
			if (oth) 
				ack ^= 0x2000;
			dn_ack(sk, skb, ack);
		}
	}

	if (skb->len < 2)
		return len;

	if ((ack = dn_ntohs(*ptr)) & 0x8000) {
		skb_pull(skb, 2);
		len += 2;
		if ((ack & 0x4000) == 0) {
			if (oth) 
				ack ^= 0x2000;
			dn_ack(sk, skb, ack);
		}
	}

	return len;
}


/**
 * dn_check_idf - Check an image data field format is correct.
 * @pptr: Pointer to pointer to image data
 * @len: Pointer to length of image data
 * @max: The maximum allowed length of the data in the image data field
 * @follow_on: Check that this many bytes exist beyond the end of the image data
 *
 * Returns: 0 if ok, -1 on error
 */
static inline int dn_check_idf(unsigned char **pptr, int *len, unsigned char max, unsigned char follow_on)
{
	unsigned char *ptr = *pptr;
	unsigned char flen = *ptr++;

	(*len)--;
	if (flen > max)
		return -1;
	if ((flen + follow_on) > *len)
		return -1;

	*len -= flen;
	*pptr = ptr + flen;
	return 0;
}

/*
 * Table of reason codes to pass back to node which sent us a badly
 * formed message, plus text messages for the log. A zero entry in
 * the reason field means "don't reply" otherwise a disc init is sent with
 * the specified reason code.
 */
static struct {
	unsigned short reason;
	const char *text;
} ci_err_table[] = {
 { 0,             "CI: Truncated message" },
 { NSP_REASON_ID, "CI: Destination username error" },
 { NSP_REASON_ID, "CI: Destination username type" },
 { NSP_REASON_US, "CI: Source username error" },
 { 0,             "CI: Truncated at menuver" },
 { 0,             "CI: Truncated before access or user data" },
 { NSP_REASON_IO, "CI: Access data format error" },
 { NSP_REASON_IO, "CI: User data format error" }
};

/*
 * This function uses a slightly different lookup method
 * to find its sockets, since it searches on object name/number
 * rather than port numbers. Various tests are done to ensure that
 * the incoming data is in the correct format before it is queued to
 * a socket.
 */
static struct _cdt *dn_find_listener(struct sk_buff *skb, unsigned short *reason)
{
	struct _cdt *cb = (skb);
	struct nsp_conn_init_msg *msg = (struct nsp_conn_init_msg *)skb->data;
	struct sockaddr_dn dstaddr;
	struct sockaddr_dn srcaddr;
	unsigned char type = 0;
	int dstlen;
	int srclen;
	unsigned char *ptr;
	int len;
	int err = 0;
	unsigned char menuver;

	memset(&dstaddr, 0, sizeof(struct sockaddr_dn));
	memset(&srcaddr, 0, sizeof(struct sockaddr_dn));

	/*
	 * 1. Decode & remove message header
	 */
	cb->cdt$l_lconid = msg->srcaddr;
	cb->cdt$l_rconid = msg->dstaddr;
	if (skb->len < sizeof(*msg))
		goto err_out;

	skb_pull(skb, sizeof(*msg));

	len = skb->len;
	ptr = skb->data;

	/*
	 * 2. Check destination end username format
	 */
	dstlen = dn_username2sockaddr(ptr, len, &dstaddr, &type);
	err++;
	if (dstlen < 0)
		goto err_out;

	err++;
	if (type > 1)
		goto err_out;

	len -= dstlen;
	ptr += dstlen;

	/*
	 * 3. Check source end username format
	 */
	srclen = dn_username2sockaddr(ptr, len, &srcaddr, &type);
	err++;
	if (srclen < 0)
		goto err_out;

	len -= srclen;
	ptr += srclen;
	err++;
	if (len < 1)
		goto err_out;

	menuver = *ptr;
	ptr++;
	len--;

	/*
	 * 4. Check that optional data actually exists if menuver says it does
	 */
	err++;
	if ((menuver & (DN_MENUVER_ACC | DN_MENUVER_USR)) && (len < 1))
		goto err_out;

	/*
	 * 5. Check optional access data format
	 */
	err++;
	if (menuver & DN_MENUVER_ACC) {
		if (dn_check_idf(&ptr, &len, 39, 1))
			goto err_out;
		if (dn_check_idf(&ptr, &len, 39, 1))
			goto err_out;
		if (dn_check_idf(&ptr, &len, 39, (menuver & DN_MENUVER_USR) ? 1 : 0))
			goto err_out;
	}

	/*
	 * 6. Check optional user data format
	 */
	err++;
	if (menuver & DN_MENUVER_USR) {
		if (dn_check_idf(&ptr, &len, 16, 0))
			goto err_out;
	}

	/*
	 * 7. Look up socket based on destination end username
	 */
	return dn_sklist_find_listener2(&dstaddr);
err_out:
	dn_log_martian(skb, ci_err_table[err].text);
	*reason = ci_err_table[err].reason;
	return NULL;
}


static void dn_nsp_conn_init(struct _cdt *sk, struct sk_buff *skb)
{
		kfree_skb(skb);
		return;

	skb_queue_tail(&sk->cdt$l_share_flink, skb);

}

static void dn_nsp_conn_conf2(struct _cdt *sk, struct sk_buff *skb)
{
	struct _cdt *cb = (skb);
	struct _cdt *scp = sk;
	unsigned char *ptr;

	if (skb->len < 4)
		goto out;

	ptr = skb->data;
	if ((scp->cdt$w_state == CDT$C_CON_SENT) || (scp->cdt$w_state == CDT$C_CON_SENT)) {
                scp->cdt$l_rconid = cb->cdt$l_lconid;
                sk->cdt$w_state = CDT$C_OPEN;
                scp->cdt$w_state = CDT$C_OPEN;

		if (skb->len > 0) {
			unsigned char dlen = *skb->data;
		}
                dn_nsp_send_link2(sk, DN_NOCHANGE, 0);
        }

out:
        kfree_skb(skb);
}

static void dn_nsp_conn_ack(struct _cdt *sk, struct sk_buff *skb)
{
	struct _cdt *scp = sk;

	if (scp->cdt$w_state == CDT$C_CON_SENT) {
		scp->cdt$w_state = CDT$C_CON_SENT;

	}

	kfree_skb(skb);
}

static void dn_nsp_disc_init(struct _cdt *sk, struct sk_buff *skb)
{
	struct _cdt *scp = sk;
	struct _cdt *cb = (skb);
	unsigned short reason;

	if (skb->len < 2)
		goto out;

	reason = dn_ntohs(*(__u16 *)skb->data);
	skb_pull(skb, 2);

	if (skb->len > 0) {
		unsigned char dlen = *skb->data;
		if ((dlen <= 16) && (dlen <= skb->len)) {
		}
	}

	scp->cdt$l_rconid = cb->cdt$l_lconid;
	sk->cdt$w_state    = CDT$C_CLOSED;

	switch(scp->cdt$w_state) {
		case CDT$C_CON_SENT:
			scp->cdt$w_state = CDT$C_REJ_SENT;
			break;
		case CDT$C_OPEN:
			scp->cdt$w_state = CDT$C_DISC_SENT;
			break;
		case DN_DI:
			scp->cdt$w_state = CDT$C_DISC_ACK;
			break;
	}

	if (1) {
		if (sk->cdt$w_state != SS_UNCONNECTED)
			sk->cdt$w_state = SS_DISCONNECTING;

	}

	dn_nsp_send_disc2(sk, NSP_DISCCONF, NSP_REASON_DC, GFP_ATOMIC);



out:
	kfree_skb(skb);
}

/*
 * disc_conf messages are also called no_resources or no_link
 * messages depending upon the "reason" field.
 */
static void dn_nsp_disc_conf(struct _cdt *sk, struct sk_buff *skb)
{
	struct _cdt *scp = sk;
	unsigned short reason;

	if (skb->len != 2)
		goto out;

	reason = dn_ntohs(*(__u16 *)skb->data);

	sk->cdt$w_state = CDT$C_CLOSED;

	switch(scp->cdt$w_state) {
		case CDT$C_CON_SENT:
			scp->cdt$w_state = CDT$C_VC_FAIL;
			break;
		case CDT$C_REJ_SENT:
			if (reason == NSP_REASON_DC)
				scp->cdt$w_state = CDT$C_REJ_SENT;
			if (reason == NSP_REASON_NL)
				scp->cdt$w_state = CDT$C_DISC_REC;
			break;
		case DN_DI:
			scp->cdt$w_state = CDT$C_DISC_ACK;
			break;
		case CDT$C_OPEN:
		case CDT$C_CON_ACK:
			scp->cdt$w_state = CDT$C_DISC_REC;
	}

	if (1) {
		if (sk->cdt$w_state != SS_UNCONNECTED)
			sk->cdt$w_state = SS_DISCONNECTING;

	}




out:
	kfree_skb(skb);
}

static void dn_nsp_linkservice(struct _cdt *sk, struct sk_buff *skb)
{
	struct _cdt *scp = sk;
	unsigned short segnum;
	unsigned char lsflags;
	char fcval;
	int wake_up = 0;
	char *ptr = skb->data;
	unsigned char fctype = 0;

	if (skb->len != 4)
		goto out;

	segnum = dn_ntohs(*(__u16 *)ptr);
	ptr += 2;
	lsflags = *(unsigned char *)ptr++;
	fcval = *ptr;

	/*
	 * Here we ignore erronous packets which should really
	 * should cause a connection abort. It is not critical 
	 * for now though.
	 */
	if (lsflags & 0xf8)
		goto out;

	if (seq_next(scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr, segnum)) {
		seq_add(&scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr, 1);
		switch(lsflags & 0x04) { /* FCVAL INT */
		case 0x00: /* Normal Request */
			switch(lsflags & 0x03) { /* FCVAL MOD */
       	         	case 0x00: /* Request count */
				if (fcval < 0) {
					unsigned char p_fcval = -fcval;
				} else if (fcval > 0) {
					wake_up = 1;
				}
               	       	 	break;
			case 0x01: /* Stop outgoing data */
				break;
			case 0x02: /* Ok to start again */
				dn_nsp_output2(sk);
				wake_up = 1;
			}
			break;
		case 0x04: /* Interrupt Request */
			if (fcval > 0) {
				wake_up = 1;
			}
			break;
                }
        }

	dn_nsp_send_oth_ack2(sk);

out:
	kfree_skb(skb);
}

/*
 * Copy of sock_queue_rcv_skb (from sock.h) without
 * bh_lock_sock() (its already held when this is called) which
 * also allows data and other data to be queued to a socket.
 */
static __inline__ int dn_queue_skb(struct _cdt *sk, struct sk_buff *skb, int sig, struct sk_buff_head *queue)
{

        /* Cast skb->rcvbuf to unsigned... It's pointless, but reduces
           number of warnings when compiling with -W --ANK
         */

        skb_set_owner_r(skb, sk);
        skb_queue_tail(queue, skb);

	/* This code only runs from BH or BH protected context.
	 * Therefore the plain read_lock is ok here. -DaveM
	 */
        if (1) {
		struct _cdt *sock = sk;
		wake_up_interruptible(sk);
	}

        return 0;
}

static void dn_nsp_otherdata(struct _cdt *sk, struct sk_buff *skb)
{
	struct _cdt *scp = sk;
	unsigned short segnum;
	struct _cdt *cb = (skb);
	int queued = 0;

	if (skb->len < 2)
		goto out;

	cb->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_lcl_chan_seq_num = segnum = dn_ntohs(*(__u16 *)skb->data);
	skb_pull(skb, 2);

	if (seq_next(scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr, segnum)) {

		if (dn_queue_skb(sk, skb, SIGURG, &scp->cdt$l_share_flink) == 0) {
			seq_add(&scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr, 1);
			queued = 1;
		}
	}

	dn_nsp_send_oth_ack2(sk);
out:
	if (!queued)
		kfree_skb(skb);
}

static void dn_nsp_data(struct _cdt *sk, struct sk_buff *skb)
{
	int queued = 0;
	unsigned short segnum;
	struct _cdt *cb = (skb);
	struct _cdt *scp = sk;

	if (skb->len < 2)
		goto out;

	cb->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_lcl_chan_seq_num = segnum = dn_ntohs(*(__u16 *)skb->data);
	skb_pull(skb, 2);

	if (seq_next(scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr, segnum)) {
                if (dn_queue_skb(sk, skb, SIGIO, &sk->cdt$l_share_flink) == 0) {
			seq_add(&scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr, 1);
                	queued = 1;
                }

		dn_nsp_send_link2(sk, DN_DONTSEND, 0);
        }

	dn_nsp_send_data_ack2(sk);
out:
	if (!queued)
		kfree_skb(skb);
}

/*
 * If one of our conninit messages is returned, this function
 * deals with it. It puts the socket into the NO_COMMUNICATION
 * state.
 */
static void dn_returned_conn_init(struct _cdt *sk, struct sk_buff *skb)
{
	struct _cdt *scp = sk;

	if (scp->cdt$w_state == CDT$C_CON_SENT) {
		scp->cdt$w_state = CDT$C_VC_FAIL;
		sk->cdt$w_state = CDT$C_CLOSED;
	}

	kfree_skb(skb);
}

static int dn_nsp_no_socket(struct sk_buff *skb, unsigned short reason)
{
	struct _cdt *cb = (skb);
	int ret = NET_RX_DROP;

	/* Must not reply to returned packets */
	if (cb->cdt$b_ls_flag & DN_RT_F_RTS)
		goto out;

	if ((reason != NSP_REASON_OK) && ((cb->cdt$b_ls_flag & 0x0c) == 0x08)) {
		switch(cb->cdt$b_ls_flag & 0x70) {
			case 0x10:
			case 0x60: /* (Retransmitted) Connect Init */
				dn_nsp_return_disc(skb, NSP_DISCINIT, reason);
				ret = NET_RX_SUCCESS;
				break;
			case 0x20: /* Connect Confirm */
				dn_nsp_return_disc(skb, NSP_DISCCONF, reason);
				ret = NET_RX_SUCCESS;
				break;
		}
	}

out:
	kfree_skb(skb);
	return ret;
}

static int dn_nsp_rx_packet(struct sk_buff *skb)
{
	struct _cdt *cb = (skb);
	struct _cdt *sk = NULL;
	unsigned char *ptr = (unsigned char *)skb->data;
	unsigned short reason = NSP_REASON_NL;

	skb->h.raw    = skb->data;
	cb->cdt$b_ls_flag = *ptr++;

	if (decnet_debug_level & 2)
		printk(KERN_DEBUG "dn_nsp_rx: Message type 0x%02x\n", (int)cb->cdt$b_ls_flag);

	if (skb->len < 2) 
		goto free_out;

	if (cb->cdt$b_ls_flag & 0x83) 
		goto free_out;

	/*
	 * Returned packets...
	 * Swap src & dst and look up in the normal way.
	 */
	if (cb->cdt$b_ls_flag & DN_RT_F_RTS) {
		unsigned short tmp = cb->cdt$l_rconid;
		cb->cdt$l_rconid = cb->cdt$l_lconid;
		cb->cdt$l_lconid = tmp;
		tmp = cb->cdt$l_rconid;
		cb->cdt$l_rconid = cb->cdt$l_lconid;
		cb->cdt$l_lconid = tmp;
		sk = dn_find_by_skb2(skb);
		goto got_it;
	}

	/*
	 * Filter out conninits and useless packet types
	 */
	if ((cb->cdt$b_ls_flag & 0x0c) == 0x08) {
		switch(cb->cdt$b_ls_flag & 0x70) {
			case 0x00: /* NOP */
			case 0x70: /* Reserved */
			case 0x50: /* Reserved, Phase II node init */
				goto free_out;
			case 0x10:
			case 0x60:
				sk = dn_find_listener(skb, &reason);
				goto got_it;
		}
	}

	if (skb->len < 3)
		goto free_out;

	/*
	 * Grab the destination address.
	 */
	cb->cdt$l_rconid = *(unsigned short *)ptr;
	cb->cdt$l_lconid = 0;
	ptr += 2;

	/*
	 * If not a connack, grab the source address too.
	 */
	if (skb->len >= 5) {
		cb->cdt$l_lconid = *(unsigned short *)ptr;
		ptr += 2;
		skb_pull(skb, 5);
	}

	/*
	 * Find the socket to which this skb is destined.
	 */
	sk = dn_find_by_skb2(skb);
got_it:
	if (sk != NULL) {
		struct _cdt *scp = sk;
		int ret;

		/* Reset backoff */

		
		ret = NET_RX_SUCCESS;
		if (decnet_debug_level & 8)
			printk(KERN_DEBUG "NSP: 0x%02x 0x%02x 0x%04x 0x%04x %d\n",
				(int)cb->cdt$b_ls_flag, (int)cb->cdt$b_ls_flag, 
				(int)cb->cdt$l_lconid, (int)cb->cdt$l_rconid, 
				(int)0);

			ret = dn_nsp_backlog_rcv2(sk, skb);
		
		sock_put(sk);

		return ret;
	}

	return dn_nsp_no_socket(skb, reason);

free_out:
	kfree_skb(skb);
	return NET_RX_DROP;
}

int dn_nsp_rx(struct sk_buff *skb)
{
	return NF_HOOK(PF_DECnet, NF_DN_LOCAL_IN, skb, skb->dev, NULL, dn_nsp_rx_packet);
}

/*
 * This is the main receive routine for sockets. It is called
 * from the above when the socket is not busy, and also from
 * sock_release() when there is a backlog queued up.
 */
int dn_nsp_backlog_rcv2(struct _cdt *sk, struct sk_buff *skb)
{
	struct _cdt *scp = sk;
	struct _cdt *cb = (skb);

	if (cb->cdt$b_ls_flag & DN_RT_F_RTS) {
		dn_returned_conn_init(sk, skb);
		return NET_RX_SUCCESS;
	}

	/*
	 * Control packet.
	 */
	if ((cb->cdt$b_ls_flag & 0x0c) == 0x08) {
		switch(cb->cdt$b_ls_flag & 0x70) {
			case 0x10:
			case 0x60:
				dn_nsp_conn_init(sk, skb);
				break;
			case 0x20:
				dn_nsp_conn_conf2(sk, skb);
				break;
			case 0x30:
				dn_nsp_disc_init(sk, skb);
				break;
			case 0x40:      
				dn_nsp_disc_conf(sk, skb);
				break;
		}

	} else if (cb->cdt$b_ls_flag == 0x24) {
		/*
		 * Special for connacks, 'cos they don't have
		 * ack data or ack otherdata info.
		 */
		dn_nsp_conn_ack(sk, skb);
	} else {
		int other = 1;

		/* both data and ack frames can kick a CC socket into RUN */
		if ((scp->cdt$w_state == CDT$C_CON_ACK)) {
			scp->cdt$w_state = CDT$C_OPEN;
			sk->cdt$w_state = CDT$C_OPEN;

		}

		if ((cb->cdt$b_ls_flag & 0x1c) == 0)
			other = 0;
		if (cb->cdt$b_ls_flag == 0x04)
			other = 0;

		/*
		 * Read out ack data here, this applies equally
		 * to data, other data, link serivce and both
		 * ack data and ack otherdata.
		 */
		dn_process_ack(sk, skb, other);

		/*
		 * If we've some sort of data here then call a
		 * suitable routine for dealing with it, otherwise
		 * the packet is an ack and can be discarded.
		 */
		if ((cb->cdt$b_ls_flag & 0x0c) == 0) {

			if (scp->cdt$w_state != CDT$C_OPEN)
				goto free_out;

			switch(cb->cdt$b_ls_flag) {
				case 0x10: /* LS */
					dn_nsp_linkservice(sk, skb);
					break;
				case 0x30: /* OD */
					dn_nsp_otherdata(sk, skb);
					break;
				default:
					dn_nsp_data(sk, skb);
			}

		} else { /* Ack, chuck it out here */
free_out:
			kfree_skb(skb);
		}
	}

	return NET_RX_SUCCESS;
}


/*
 * DECnet       An implementation of the DECnet protocol suite for the LINUX
 *              operating system.  DECnet is implemented using the  BSD Socket
 *              interface as the means of communication with the user level.
 *
 *              DECnet Network Services Protocol (Output)
 *
 * Author:      Eduardo Marcelo Serrat <emserrat@geocities.com>
 *
 * Changes:
 *
 *    Steve Whitehouse:  Split into dn_nsp_in.c and dn_nsp_out.c from
 *                       original dn_nsp.c.
 *    Steve Whitehouse:  Updated to work with my new routing architecture.
 *    Steve Whitehouse:  Added changes from Eduardo Serrat's patches.
 *    Steve Whitehouse:  Now conninits have the "return" bit set.
 *    Steve Whitehouse:  Fixes to check alloc'd skbs are non NULL!
 *                       Moved output state machine into one function
 *    Steve Whitehouse:  New output state machine
 *         Paul Koning:  Connect Confirm message fix.
 *      Eduardo Serrat:  Fix to stop dn_nsp_do_disc() sending malformed packets.
 *    Steve Whitehouse:  dn_nsp_output() and friends needed a spring clean
 */

/******************************************************************************
    (c) 1995-1998 E.M. Serrat		emserrat@geocities.com
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*******************************************************************************/

#include <linux/errno.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/sockios.h>
#include <linux/net.h>
#include <linux/netdevice.h>
#include <linux/inet.h>
#include <linux/route.h>
#include <net/sock.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/termios.h>      
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/if_packet.h>
#include <net/neighbour.h>
#include <net/dst.h>
#include <net/dn_nsp.h>
#include <net/dn_dev.h>
#include <net/dn_route.h>


static int nsp_backoff[NSP_MAXRXTSHIFT + 1] = { 1, 2, 4, 8, 16, 32, 64, 64, 64, 64, 64, 64, 64 };

/*
 * If sk == NULL, then we assume that we are supposed to be making
 * a routing layer skb. If sk != NULL, then we are supposed to be
 * creating an skb for the NSP layer.
 *
 * The eventual aim is for each socket to have a cached header size
 * for its outgoing packets, and to set hdr from this when sk != NULL.
 */
struct sk_buff *dn_alloc_skb2(struct _cdt *sk, int size, int pri)
{
	struct sk_buff *skb;
	int hdr = 64;

	if ((skb = alloc_skb(size + hdr, pri)) == NULL)
		return NULL;

	skb->protocol = __constant_htons(ETH_P_MYSCS);
	skb->pkt_type = PACKET_OUTGOING;

	//	if (sk)
	//	skb_set_owner_w(skb, sk);

	skb_reserve(skb, hdr);

	return skb;
}

/*
 * Wrapper for the above, for allocs of data skbs. We try and get the
 * whole size thats been asked for (plus 11 bytes of header). If this
 * fails, then we try for any size over 16 bytes for SOCK_STREAMS.
 */
struct sk_buff *dn_alloc_send_skb2(struct _cdt *sk, int *size, int noblock, int *err)
{
	int space;
	int len;
	struct sk_buff *skb = NULL;

	*err = 0;

	while(skb == NULL) {
		if (signal_pending(current)) {
			*err = ERESTARTSYS;
			break;
		}

		len = *size + 11;

		if (space < len) {
		}

		if (space < len) {
			set_bit(SOCK_ASYNC_NOSPACE, &sk->cdt$b_ls_flag);
			if (noblock) {
				*err = EWOULDBLOCK;
				break;
			}

			clear_bit(SOCK_ASYNC_WAITDATA, &sk->cdt$b_ls_flag);
			
			continue;

		}

		if ((skb = dn_alloc_skb2(sk, len, 0)) == NULL)
			continue;

		*size = len - 11;
	}

	return skb;
}

/*
 * Calculate persist timer based upon the smoothed round
 * trip time and the variance. Backoff according to the
 * nsp_backoff[] array.
 */
unsigned long dn_nsp_persist2(struct _cdt *sk)
{
	struct _cdt *scp = sk;

	unsigned long t = 0;


	if (t < HZ) t = HZ;
	if (t > (600*HZ)) t = (600*HZ);

	/* printk(KERN_DEBUG "rxtshift %lu, t=%lu\n", scp->nsp_rxtshift, t); */

	return t;
}

/*
 * This is called each time we get an estimate for the rtt
 * on the link.
 */
static void dn_nsp_rtt(struct _cdt *sk, long rtt)
{
	struct _cdt *scp = sk;
	long srtt = 0;
	long rttvar = 0;
	long delta;

	/*
	 * If the jiffies clock flips over in the middle of timestamp
	 * gathering this value might turn out negative, so we make sure
	 * that is it always positive here.
	 */
	if (rtt < 0) 
		rtt = -rtt;
	/*
	 * Add new rtt to smoothed average
	 */
	delta = ((rtt << 3) - srtt);
	srtt += (delta >> 3);
	/*
	 * Add new rtt varience to smoothed varience
	 */
	delta >>= 1;
	rttvar += ((((delta>0)?(delta):(-delta)) - rttvar) >> 2);

	/* printk(KERN_DEBUG "srtt=%lu rttvar=%lu\n", scp->nsp_srtt, scp->nsp_rttvar); */
}

/**
 * dn_nsp_clone_and_send - Send a data packet by cloning it
 * @skb: The packet to clone and transmit
 * @gfp: memory allocation flag
 *
 * Clone a queued data or other data packet and transmit it.
 *
 * Returns: The number of times the packet has been sent previously
 */
static inline unsigned dn_nsp_clone_and_send(struct sk_buff *skb, int gfp)
{
	struct _cdt *cb = (skb);
	struct sk_buff *skb2;
	int ret = 0;

	if ((skb2 = skb_clone(skb, gfp)) != NULL) {
		ret = cb->cdt$l_pb->pb$l_vc_addr->vc$l_xmt_msg;
		cb->cdt$l_pb->pb$l_vc_addr->vc$l_xmt_msg++;
		skb2->sk = skb->sk;
		dn_nsp_send2(skb2);
	}

	return ret;
}

/**
 * dn_nsp_output - Try and send something from socket queues
 * @sk: The socket whose queues are to be investigated
 * @gfp: The memory allocation flags
 *
 * Try and send the packet on the end of the data and other data queues.
 * Other data gets priority over data, and if we retransmit a packet we
 * reduce the window by dividing it in two.
 *
 */
void dn_nsp_output2(struct _cdt *sk)
{
	struct _cdt *scp = sk;
	struct sk_buff *skb;
	unsigned reduce_win = 0;

	/*
	 * First we check for otherdata/linkservice messages
	 */
	if ((skb = skb_peek(&scp->cdt$l_waitqfl)) != NULL)
		reduce_win = dn_nsp_clone_and_send(skb, GFP_ATOMIC);

	/*
	 * If we may not send any data, we don't.
	 * If we are still trying to get some other data down the
	 * channel, we don't try and send any data.
	 */
	if ((skb = skb_peek(&scp->cdt$l_waitqfl)) != NULL)
		reduce_win = dn_nsp_clone_and_send(skb, GFP_ATOMIC);

	/*
	 * If we've sent any frame more than once, we cut the
	 * send window size in half. There is always a minimum
	 * window size of one available.
	 */
recalc_window:
	if (reduce_win) {
	}
}

int dn_nsp_xmit_timeout2(struct _cdt *sk)
{
	struct _cdt *scp = sk;

	dn_nsp_output2(sk);

	return 0;
}

static inline unsigned char *dn_mk_common_header(struct _cdt *scp, struct sk_buff *skb, unsigned char msgflag, int len)
{
	unsigned char *ptr = skb_push(skb, len);

	if (len < 5)
		BUG();

	*ptr++ = msgflag;
	*((unsigned short *)ptr) = scp->cdt$l_rconid;
	ptr += 2;
	*((unsigned short *)ptr) = scp->cdt$l_lconid;
	ptr += 2;
	return ptr;
}

static unsigned short *dn_mk_ack_header(struct _cdt *sk, struct sk_buff *skb, unsigned char msgflag, int hlen, int other)
{
	struct _cdt *scp = sk;
	unsigned short acknum = scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr & 0x0FFF;
	unsigned short ackcrs = scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr & 0x0FFF;
	unsigned short *ptr;

	if (hlen < 9)
		BUG();

	scp->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_rmt_chan_seq_num = acknum;
	scp->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_rmt_chan_seq_num = ackcrs;
	acknum |= 0x8000;
	ackcrs |= 0x8000;

	/* If this is an "other data/ack" message, swap acknum and ackcrs */
	if (other) {
		unsigned short tmp = acknum;
		acknum = ackcrs;
		ackcrs = tmp;
	}

	/* Set "cross subchannel" bit in ackcrs */
	ackcrs |= 0x2000;

	ptr = (unsigned short *)dn_mk_common_header(scp, skb, msgflag, hlen);

	*ptr++ = dn_htons(acknum);
	*ptr++ = dn_htons(ackcrs);

	return ptr;
}

void dn_nsp_queue_xmit2(struct _cdt *sk, struct sk_buff *skb, int gfp, int oth)
{
	struct _cdt *scp = sk;
	struct _cdt *cb = (skb);
	unsigned long t = 0;

	/*
	 * Slow start: If we have been idle for more than
	 * one RTT, then reset window to min size.
	 */

	cb->cdt$l_pb->pb$l_vc_addr->vc$l_xmt_msg = 0;

	if (oth)
		skb_queue_tail(&scp->cdt$l_waitqfl, skb);
	else
		skb_queue_tail(&scp->cdt$l_waitqfl, skb);

	dn_nsp_clone_and_send(skb, gfp);
}


int dn_nsp_check_xmit_queue2(struct _cdt *sk, struct sk_buff *skb, struct sk_buff_head *q, unsigned short acknum)
{
	struct _cdt *cb = (skb);
	struct _cdt *scp = sk;
	struct sk_buff *skb2, *list, *ack = NULL;
	int wakeup = 0;
	int try_retrans = 0;
	unsigned long reftime = -0;
	unsigned long pkttime;
	unsigned short xmit_count;
	unsigned short segnum;

	skb2 = q->next;
	list = (struct sk_buff *)q;
	while(list != skb2) {
		struct _cdt *cb2 = (skb2);

		if (before_or_equal(cb2->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_lcl_chan_seq_num, acknum))
			ack = skb2;

		/* printk(KERN_DEBUG "ack: %s %04x %04x\n", ack ? "ACK" : "SKIP", (int)cb2->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_lcl_chan_seq_num, (int)acknum); */

		skb2 = skb2->next;

		if (ack == NULL)
			continue;

		/* printk(KERN_DEBUG "check_xmit_queue: %04x, %d\n", acknum, cb2->cdt$l_pb->pb$l_vc_addr->vc$l_xmt_msg); */

		/* Does _last_ packet acked have xmit_count > 1 */
		try_retrans = 0;
		/* Remember to wake up the sending process */
		wakeup = 1;
		/* Keep various statistics */
		xmit_count = cb2->cdt$l_pb->pb$l_vc_addr->vc$l_xmt_msg;
		segnum = cb2->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_lcl_chan_seq_num;
		/* Remove and drop ack'ed packet */
		skb_unlink(ack);
		kfree_skb(ack);
		ack = NULL;

		/*
		 * We don't expect to see acknowledgements for packets we
		 * haven't sent yet.
		 */
		if (xmit_count == 0)
			BUG();
		/*
		 * If the packet has only been sent once, we can use it
		 * to calculate the RTT and also open the window a little
		 * further.
		 */
		if (xmit_count == 1) {
			if (equal(segnum, acknum)) 
				dn_nsp_rtt(sk, (long)(pkttime - reftime));

		}

		/*
		 * Packet has been sent more than once. If this is the last
		 * packet to be acknowledged then we want to send the next
		 * packet in the send queue again (assumes the remote host does
		 * go-back-N error control).
		 */
		if (xmit_count > 1)
			try_retrans = 1;
	}

	if (try_retrans)
		dn_nsp_output2(sk);

	return wakeup;
}

void dn_nsp_send_data_ack2(struct _cdt *sk)
{
	struct sk_buff *skb = NULL;

	if ((skb = dn_alloc_skb2(sk, 9, GFP_ATOMIC)) == NULL)
		return;

	skb_reserve(skb, 9);
	dn_mk_ack_header(sk, skb, 0x04, 9, 0);
	dn_nsp_send2(skb);
}

void dn_nsp_send_oth_ack2(struct _cdt *sk)
{
	struct sk_buff *skb = NULL;

	if ((skb = dn_alloc_skb2(sk, 9, GFP_ATOMIC)) == NULL)
		return;

	skb_reserve(skb, 9);
	dn_mk_ack_header(sk, skb, 0x14, 9, 1);
	dn_nsp_send2(skb);
}


void dn_send_conn_ack2 (struct _cdt *sk)
{
	struct _cdt *scp = sk;
	struct sk_buff *skb = NULL;
        struct nsp_conn_ack_msg *msg;

	if ((skb = dn_alloc_skb2(sk, 3, 0)) == NULL)
		return;

        msg = (struct nsp_conn_ack_msg *)skb_put(skb, 3);
        msg->msgflg = 0x24;                   
	msg->dstaddr = scp->cdt$l_rconid;

	dn_nsp_send2(skb);	
}

void dn_nsp_delayed_ack2(struct _cdt *sk)
{
	struct _cdt *scp = sk;

	if (scp->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_rmt_chan_seq_num != scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr)
		dn_nsp_send_oth_ack2(sk);

	if (scp->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_rmt_chan_seq_num != scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr)
		dn_nsp_send_data_ack2(sk);
}

static int dn_nsp_retrans_conn_conf2(struct _cdt *sk)
{
	struct _cdt *scp = sk;

	if (scp->cdt$w_state == CDT$C_CON_ACK)
		dn_send_conn_conf2(sk, GFP_ATOMIC);

	return 0;
}

void dn_send_conn_conf2(struct _cdt *sk, int gfp)
{
	struct _cdt *scp = sk;
	struct sk_buff *skb = NULL;
        struct nsp_conn_init_msg *msg;
	unsigned char len = 0;

        msg = (struct nsp_conn_init_msg *)skb_put(skb, sizeof(*msg));
        msg->msgflg = 0x28;                   
	msg->dstaddr = scp->cdt$l_rconid;
        msg->srcaddr = scp->cdt$l_lconid;

	*skb_put(skb,1) = len;

	dn_nsp_send2(skb);

}


static __inline__ void dn_nsp_do_disc(struct _cdt *sk, unsigned char msgflg, 
			unsigned short reason, int gfp, struct dst_entry *dst,
			int ddl, unsigned char *dd, __u16 rem, __u16 loc)
{
	struct sk_buff *skb = NULL;
	int size = 7 + ddl + ((msgflg == NSP_DISCINIT) ? 1 : 0);
	unsigned char *msg;

	if ((dst == NULL) || (rem == 0)) {
		if (net_ratelimit())
			printk(KERN_DEBUG "DECnet: dn_nsp_do_disc: BUG! Please report this to SteveW@ACM.org rem=%u dst=%p\n", (unsigned)rem, dst);
		return;
	}

	if ((skb = dn_alloc_skb2(sk, size, gfp)) == NULL)
		return;

	msg = skb_put(skb, size);
	*msg++ = msgflg;
	*(__u16 *)msg = rem;
	msg += 2;
	*(__u16 *)msg = loc;
	msg += 2;
	*(__u16 *)msg = dn_htons(reason);
	msg += 2;
	if (msgflg == NSP_DISCINIT)
		*msg++ = ddl;

	if (ddl) {
		memcpy(msg, dd, ddl);
	}

	/*
	 * This doesn't go via the dn_nsp_send() fucntion since we need
	 * to be able to send disc packets out which have no socket
	 * associations.
	 */
	skb->dst = dst_clone(dst);
	dev_queue_xmit(skb);
}


void dn_nsp_send_disc2(struct _cdt *sk, unsigned char msgflg, 
			unsigned short reason, int gfp)
{
	struct _cdt *scp = sk;
	int ddl = 0;

	dn_nsp_do_disc(sk, msgflg, reason, gfp, 0, ddl, 
		0, scp->cdt$l_rconid, scp->cdt$l_lconid);
}


void dn_nsp_return_disc(struct sk_buff *skb, unsigned char msgflg, 
			unsigned short reason)
{
	struct _cdt *cb = (skb);
	int ddl = 0;
	int gfp = GFP_ATOMIC;

	dn_nsp_do_disc(NULL, msgflg, reason, gfp, skb->dst, ddl, 
			NULL, cb->cdt$l_lconid, cb->cdt$l_rconid);
}


void dn_nsp_send_link2(struct _cdt *sk, unsigned char lsflags, char fcval)
{
	struct _cdt *scp = sk;
	struct sk_buff *skb;
	unsigned short *segnum;
	unsigned char *ptr;
	int gfp = GFP_ATOMIC;

	if ((skb = dn_alloc_skb2(sk, 13, gfp)) == NULL)
		return;

	skb_reserve(skb, 13);
	segnum = dn_mk_ack_header(sk, skb, 0x10, 13, 1);
	*segnum = dn_htons(scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr);
        DN_SKB_CB(skb)->segnum = scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr;
	seq_add(&scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr, 1);
	ptr = (unsigned char *)(segnum + 1);
	*ptr++ = lsflags;
	*ptr = fcval;

	dn_nsp_queue_xmit2(sk, skb, gfp, 1);

}

static int dn_nsp_retrans_conninit(struct _cdt *sk)
{
	dn_nsp_send_conninit2(sk, SCS$C_CON_REQ,"a","b","c");
	return 0;
}

void scs_msg_ctl_comm(struct _cdt *sk, unsigned char msgflg)
{
	struct sk_buff *skb = NULL;
	if ((skb = dn_alloc_skb2(sk, 200, (msgflg == SCS$C_CON_REQ) ? 0 : GFP_ATOMIC)) == NULL)
	  return;

	scs_msg_ctl_fill(skb,sk,msgflg);

	dn_nsp_send2(skb);	
}

void dn_nsp_send_conninit2(struct _cdt *sk, unsigned char msgflg, char * rprnam, char * lprnam, char * condat)
{
	struct sk_buff *skb = NULL;
	struct _nisca *nisca;
	struct _ppd * ppd;
	struct _scs * scs;
	if ((skb = dn_alloc_skb2(sk, 200, (msgflg == SCS$C_CON_REQ) ? 0 : GFP_ATOMIC)) == NULL)
	  return;

	scs_msg_ctl_fill(skb,sk,msgflg);

	scs=getppdscs(skb->data);

	bzero(&scs->scs$t_dst_proc,16);
	bzero(&scs->scs$t_src_proc,16);
	bzero(&scs->scs$b_con_dat,16);
	bcopy(rprnam,&scs->scs$t_dst_proc,strlen(rprnam));
	bcopy(lprnam,&scs->scs$t_src_proc,strlen(lprnam));
	bcopy(condat,&scs->scs$b_con_dat,strlen(condat));

	dn_nsp_send2(skb);	
}

void scs_msg_ctl_fill(struct sk_buff *skb, struct _cdt * cdt, unsigned char msgflg)
{
	struct _nisca *nisca;
	struct _ppd * ppd;
	struct _scs * scs;
	struct _nisca *dx;
	void * data;
	data = skb_put(skb,sizeof(*nisca));
	scs_fill_dx(data,0xab00,0x04010000,0xaa00,(system_utsname.nodename[0]<<16)+system_utsname.nodename[1]);
	nisca=gettr(data);
	nisca->nisca$b_tr_flag=NISCA$M_TR_CTL;
	nisca->nisca$b_tr_pad=0x13;
	nisca->nisca$b_tr_pad_data_len=0x12;
	
	ppd=getppdscs(data);
	scs=getppdscs(data);

	//ppd->ppb$b_opc=NISCA$C_MSGREC;
	
	scs->scs$w_mtype=msgflg;
	scs->scs$l_dst_conid=cdt->cdt$l_rconid;
	scs->scs$l_src_conid=cdt->cdt$l_lconid;

	memcpy(&nisca->nisca$t_nodename,&mysb.sb$t_nodename,8);
}


void scs_msg_fill(struct sk_buff *skb, struct _cdt * cdt, unsigned char msgflg)
{
	struct _nisca *nisca;
	struct _ppd * ppd;
	struct _scs * scs;
	struct _nisca *dx;
	void * data;
	data = skb_put(skb,sizeof(*nisca));
	scs_fill_dx(data,0xab00,0x04010000,0xaa00,(system_utsname.nodename[0]<<16)+system_utsname.nodename[1]);
	nisca=gettr(data);
	nisca->nisca$b_tr_flag=0;
	nisca->nisca$b_tr_pad=0x13;
	nisca->nisca$b_tr_pad_data_len=0x12;
	
	ppd=getppdscs(data);
	scs=getppdscs(data);

	//ppd->ppb$b_opc=NISCA$C_MSGREC;
	
	scs->scs$w_mtype=SCS$C_APPL_DG;
	scs->scs$l_dst_conid=cdt->cdt$l_rconid;
	scs->scs$l_src_conid=cdt->cdt$l_lconid;

	memcpy(&nisca->nisca$t_nodename,&mysb.sb$t_nodename,8);
}

void scs_msg_fill_more(struct sk_buff *skb,struct _cdt * cdt, struct _cdrp * cdrp)
{
	struct _nisca *nisca;
	struct _ppd * ppd;
	struct _scs * scs;
	struct _nisca *dx;
	void * data;
	data = skb_put(skb,sizeof(*nisca));
	data = skb->data;
	ppd=getppdscs(data);
	scs=getppdscs(data);

	data = skb_put(skb,sizeof(*scs));
	data = (unsigned long)scs + sizeof(*scs);

	bcopy(cdrp->cdrp$l_msg_buf,data,cdrp->cdrp$w_cdrpsize);

	cdt->cdt$l_reserved3=current->pid;
	cdt->cdt$l_reserved4=cdrp->cdrp$l_msg_buf;

	data=skb_put(skb,cdrp->cdrp$w_cdrpsize);
}


/*
 * DECnet       An implementation of the DECnet protocol suite for the LINUX
 *              operating system.  DECnet is implemented using the  BSD Socket
 *              interface as the means of communication with the user level.
 *
 *              DECnet Routing Functions (Endnode and Router)
 *
 * Authors:     Steve Whitehouse <SteveW@ACM.org>
 *              Eduardo Marcelo Serrat <emserrat@geocities.com>
 *
 * Changes:
 *              Steve Whitehouse : Fixes to allow "intra-ethernet" and
 *                                 "return-to-sender" bits on outgoing
 *                                 packets.
 *		Steve Whitehouse : Timeouts for cached routes.
 *              Steve Whitehouse : Use dst cache for input routes too.
 *              Steve Whitehouse : Fixed error values in dn_send_skb.
 *              Steve Whitehouse : Rework routing functions to better fit
 *                                 DECnet routing design
 *              Alexey Kuznetsov : New SMP locking
 *              Steve Whitehouse : More SMP locking changes & dn_cache_dump()
 *              Steve Whitehouse : Prerouting NF hook, now really is prerouting.
 *				   Fixed possible skb leak in rtnetlink funcs.
 *              Steve Whitehouse : Dave Miller's dynamic hash table sizing and
 *                                 Alexey Kuznetsov's finer grained locking
 *                                 from ipv4/route.c.
 *              Steve Whitehouse : Routing is now starting to look like a
 *                                 sensible set of code now, mainly due to
 *                                 my copying the IPv4 routing code. The
 *                                 hooks here are modified and will continue
 *                                 to evolve for a while.
 *              Steve Whitehouse : Real SMP at last :-) Also new netfilter
 *                                 stuff. Look out raw sockets your days
 *                                 are numbered!
 *              Steve Whitehouse : Added return-to-sender functions. Added
 *                                 backlog congestion level return codes.
 *                                 
 */

/******************************************************************************
    (c) 1995-1998 E.M. Serrat		emserrat@geocities.com
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*******************************************************************************/

#include <linux/errno.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/kernel.h>
#include <linux/sockios.h>
#include <linux/net.h>
#include <linux/netdevice.h>
#include <linux/inet.h>
#include <linux/route.h>
#include <net/sock.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/rtnetlink.h>
#include <linux/string.h>
#include <linux/netfilter_decnet.h>
#include <asm/errno.h>
#include <net/neighbour.h>
#include <net/dst.h>
#include <net/dn.h>
#include <net/dn_dev.h>
#include <net/dn_nsp.h>
#include <net/dn_route.h>
#include <net/dn_neigh.h>
#include <net/dn_fib.h>

struct dn_rt_hash_bucket
{
	struct dn_route *chain;
	rwlock_t lock;
} __attribute__((__aligned__(8)));

extern struct neigh_table dn_neigh_table;


static unsigned char dn_hiord_addr[6] = {0xAB,0x00,0x04,0x01,0x00,0x00}; // remember to add clusterid + 1

int dn_rt_min_delay = 2*HZ;
int dn_rt_max_delay = 10*HZ;
static unsigned long dn_rt_deadline = 0;

static int dn_dst_gc(void);
static struct dst_entry *dn_dst_check(struct dst_entry *, __u32);
static struct dst_entry *dn_dst_reroute(struct dst_entry *, struct sk_buff *skb);
static struct dst_entry *dn_dst_negative_advice(struct dst_entry *);
static void dn_dst_link_failure(struct sk_buff *);
static int dn_route_input(struct sk_buff *);
static void dn_run_flush(unsigned long dummy);

static struct dn_rt_hash_bucket *dn_rt_hash_table;
static unsigned dn_rt_hash_mask;

static struct timer_list dn_route_timer;
static struct timer_list dn_rt_flush_timer = { function: dn_run_flush };
int decnet_dst_gc_interval = 2;

static struct dst_ops dn_dst_ops = {
	family:			PF_DECnet,
	protocol:		__constant_htons(ETH_P_MYSCS),
	gc_thresh:		128,
	gc:			dn_dst_gc,
	check:			dn_dst_check,
	reroute:		dn_dst_reroute,
	negative_advice:	dn_dst_negative_advice,
	link_failure:		dn_dst_link_failure,
	entry_size:		sizeof(struct dn_route),
	entries:		ATOMIC_INIT(0),
};

static __inline__ unsigned dn_hash(unsigned short src, unsigned short dst)
{
	unsigned short tmp = src ^ dst;
	tmp ^= (tmp >> 3);
	tmp ^= (tmp >> 5);
	tmp ^= (tmp >> 10);
	return dn_rt_hash_mask & (unsigned)tmp;
}

static void SMP_TIMER_NAME(dn_dst_check_expire)(unsigned long dummy)
{
	int i;
	struct dn_route *rt, **rtp;
	unsigned long now = jiffies;
	unsigned long expire = 120 * HZ;

	for(i = 0; i <= dn_rt_hash_mask; i++) {
		rtp = &dn_rt_hash_table[i].chain;

		write_lock(&dn_rt_hash_table[i].lock);
		while((rt=*rtp) != NULL) {
			if (atomic_read(&rt->u.dst.__refcnt) ||
					(now - rt->u.dst.lastuse) < expire) {
				rtp = &rt->u.rt_next;
				continue;
			}
			*rtp = rt->u.rt_next;
			rt->u.rt_next = NULL;
			dst_free(&rt->u.dst);
		}
		write_unlock(&dn_rt_hash_table[i].lock);

		if ((jiffies - now) > 0)
			break;
	}

	mod_timer(&dn_route_timer, now + decnet_dst_gc_interval * HZ);
}

SMP_TIMER_DEFINE(dn_dst_check_expire, dn_dst_task);

static int dn_dst_gc(void)
{
	struct dn_route *rt, **rtp;
	int i;
	unsigned long now = jiffies;
	unsigned long expire = 10 * HZ;

	for(i = 0; i <= dn_rt_hash_mask; i++) {

		write_lock_bh(&dn_rt_hash_table[i].lock);
		rtp = &dn_rt_hash_table[i].chain;

		while((rt=*rtp) != NULL) {
			if (atomic_read(&rt->u.dst.__refcnt) ||
					(now - rt->u.dst.lastuse) < expire) {
				rtp = &rt->u.rt_next;
				continue;
			}
			*rtp = rt->u.rt_next;
			rt->u.rt_next = NULL;
			dst_free(&rt->u.dst);
			break;
		}
		write_unlock_bh(&dn_rt_hash_table[i].lock);
	}

	return 0;
}

static struct dst_entry *dn_dst_check(struct dst_entry *dst, __u32 cookie)
{
	dst_release(dst);
	return NULL;
}

static struct dst_entry *dn_dst_reroute(struct dst_entry *dst,
					struct sk_buff *skb)
{
	return NULL;
}

/*
 * This is called through sendmsg() when you specify MSG_TRYHARD
 * and there is already a route in cache.
 */
static struct dst_entry *dn_dst_negative_advice(struct dst_entry *dst)
{
	dst_release(dst);
	return NULL;
}

static void dn_dst_link_failure(struct sk_buff *skb)
{
	return;
}

static void dn_insert_route(struct dn_route *rt, unsigned hash)
{
	unsigned long now = jiffies;

	write_lock_bh(&dn_rt_hash_table[hash].lock);
	rt->u.rt_next = dn_rt_hash_table[hash].chain;
	dn_rt_hash_table[hash].chain = rt;
	
	dst_hold(&rt->u.dst);
	rt->u.dst.__use++;
	rt->u.dst.lastuse = now;

	write_unlock_bh(&dn_rt_hash_table[hash].lock);
}

void SMP_TIMER_NAME(dn_run_flush)(unsigned long dummy)
{
	int i;
	struct dn_route *rt, *next;

	for(i = 0; i < dn_rt_hash_mask; i++) {
		write_lock_bh(&dn_rt_hash_table[i].lock);

		if ((rt = xchg(&dn_rt_hash_table[i].chain, NULL)) == NULL)
			goto nothing_to_declare;

		for(; rt; rt=next) {
			next = rt->u.rt_next;
			rt->u.rt_next = NULL;
			dst_free((struct dst_entry *)rt);
		}

nothing_to_declare:
		write_unlock_bh(&dn_rt_hash_table[i].lock);
	}
}

SMP_TIMER_DEFINE(dn_run_flush, dn_flush_task);

static spinlock_t dn_rt_flush_lock = SPIN_LOCK_UNLOCKED;

void dn_rt_cache_flush(int delay)
{
	unsigned long now = jiffies;
	int user_mode = !in_interrupt();

	if (delay < 0)
		delay = dn_rt_min_delay;

	spin_lock_bh(&dn_rt_flush_lock);

	if (del_timer(&dn_rt_flush_timer) && delay > 0 && dn_rt_deadline) {
		long tmo = (long)(dn_rt_deadline - now);

		if (user_mode && tmo < dn_rt_max_delay - dn_rt_min_delay)
			tmo = 0;

		if (delay > tmo)
			delay = tmo;
	}

	if (delay <= 0) {
		spin_unlock_bh(&dn_rt_flush_lock);
		dn_run_flush(0);
		return;
	}

	if (dn_rt_deadline == 0)
		dn_rt_deadline = now + dn_rt_max_delay;

	dn_rt_flush_timer.expires = now + delay;
	add_timer(&dn_rt_flush_timer);
	spin_unlock_bh(&dn_rt_flush_lock);
}

/**
 * dn_return_short - Return a short packet to its sender
 * @skb: The packet to return
 *
 */
static int dn_return_short(struct sk_buff *skb)
{
	struct _cdt *cb;
	unsigned char *ptr;
	dn_address *src;
	dn_address *dst;
	dn_address tmp;

	/* Add back headers */
	skb_push(skb, skb->data - skb->nh.raw);

	if ((skb = skb_unshare(skb, GFP_ATOMIC)) == NULL)
		return NET_RX_DROP;

	cb = (skb);
	/* Skip packet length and point to flags */
	ptr = skb->data + 2;
	*ptr++ = (cb->cdt$b_ls_flag & ~DN_RT_F_RQR) | DN_RT_F_RTS;

	dst = (dn_address *)ptr;
	ptr += 2;
	src = (dn_address *)ptr;
	ptr += 2;
	*ptr = 0; /* Zero hop count */

	/* Swap source and destination */
	tmp  = *src;
	*src = *dst;
	*dst = tmp;

	skb->pkt_type = PACKET_OUTGOING;
	dn_rt_finish_output2(skb, NULL);
	return NET_RX_SUCCESS;
}

/**
 * dn_return_long - Return a long packet to its sender
 * @skb: The long format packet to return
 *
 */
static int dn_return_long(struct sk_buff *skb)
{
	struct _cdt *cb;
	unsigned char *ptr;
	unsigned char *src_addr, *dst_addr;
	unsigned char tmp[ETH_ALEN];

	/* Add back all headers */
	skb_push(skb, skb->data - skb->nh.raw);

	if ((skb = skb_unshare(skb, GFP_ATOMIC)) == NULL)
		return NET_RX_DROP;

	cb = (skb);
	/* Ignore packet length and point to flags */
	ptr = skb->data + 2;

	/* Skip padding */
	if (*ptr & DN_RT_F_PF) {
		char padlen = (*ptr & ~DN_RT_F_PF);
		ptr += padlen;
	}

	*ptr++ = (cb->cdt$b_ls_flag & ~DN_RT_F_RQR) | DN_RT_F_RTS;
	ptr += 2;
	dst_addr = ptr;
	ptr += 8;
	src_addr = ptr;
	ptr += 6;
	*ptr = 0; /* Zero hop count */

	/* Swap source and destination */
	memcpy(tmp, src_addr, ETH_ALEN);
	memcpy(src_addr, dst_addr, ETH_ALEN);
	memcpy(dst_addr, tmp, ETH_ALEN);

	skb->pkt_type = PACKET_OUTGOING;
	dn_rt_finish_output2(skb, tmp);
	return NET_RX_SUCCESS;
}

/**
 * dn_route_rx_packet - Try and find a route for an incoming packet
 * @skb: The packet to find a route for
 *
 * Returns: result of input function if route is found, error code otherwise
 */
static int dn_route_rx_packet(struct sk_buff *skb)
{
	struct _cdt *cb = (skb);
	int err;

	if ((err = dn_route_input(skb)) == 0)
		return skb->dst->input(skb);

	if (decnet_debug_level & 4) {
		char *devname = skb->dev ? skb->dev->name : "???";
		struct _cdt *cb = (skb);
		printk(KERN_DEBUG
			"DECnet: dn_route_rx_packet: rt_flags=0x%02x dev=%s len=%d src=0x%04hx dst=0x%04hx err=%d type=%d\n",
			(int)cb->cdt$b_ls_flag, devname, skb->len, cb->cdt$l_lconid, cb->cdt$l_rconid, 
			err, skb->pkt_type);
	}

	if ((skb->pkt_type == PACKET_HOST) && (cb->cdt$b_ls_flag & DN_RT_F_RQR)) {
		switch(cb->cdt$b_ls_flag & DN_RT_PKT_MSK) {
			case DN_RT_PKT_SHORT:
				return dn_return_short(skb);
			case DN_RT_PKT_LONG:
				return dn_return_long(skb);
		}
	}

	kfree_skb(skb);
	return NET_RX_DROP;
}

static int dn_route_rx_long(struct sk_buff *skb)
{
	struct _cdt *cb = (skb);
	unsigned char *ptr = skb->data;

	if (skb->len < 21) /* 20 for long header, 1 for shortest nsp */
		goto drop_it;

	skb_pull(skb, 20);
	skb->h.raw = skb->data;

        /* Destination info */
        ptr += 2;
	cb->cdt$l_rconid = dn_htons(dn_eth2dn(ptr));
        if (memcmp(ptr, dn_hiord_addr, 4) != 0)
                goto drop_it;
        ptr += 6;


        /* Source info */
        ptr += 2;
	cb->cdt$l_lconid = dn_htons(dn_eth2dn(ptr));
        if (memcmp(ptr, dn_hiord_addr, 4) != 0)
                goto drop_it;
        ptr += 6;
        /* Other junk */
        ptr++;


	return NF_HOOK(PF_DECnet, NF_DN_PRE_ROUTING, skb, skb->dev, NULL, dn_route_rx_packet);

drop_it:
	kfree_skb(skb);
	return NET_RX_DROP;
}



static int dn_route_rx_short(struct sk_buff *skb)
{
	struct _cdt *cb = (skb);
	unsigned char *ptr = skb->data;

	if (skb->len < 6) /* 5 for short header + 1 for shortest nsp */
		goto drop_it;

	skb_pull(skb, 5);
	skb->h.raw = skb->data;

	cb->cdt$l_rconid = *(dn_address *)ptr;
        ptr += 2;
        cb->cdt$l_lconid = *(dn_address *)ptr;
        ptr += 2;


	return NF_HOOK(PF_DECnet, NF_DN_PRE_ROUTING, skb, skb->dev, NULL, dn_route_rx_packet);

drop_it:
        kfree_skb(skb);
        return NET_RX_DROP;
}

static int dn_route_discard(struct sk_buff *skb)
{
	/*
	 * I know we drop the packet here, but thats considered success in
	 * this case
	 */
	kfree_skb(skb);
	return NET_RX_SUCCESS;
}

static int dn_route_ptp_hello(struct sk_buff *skb)
{
	dn_dev_hello(skb);
	dn_neigh_pointopoint_hello(skb);
	return NET_RX_SUCCESS;
}

int opc_msgrec(struct sk_buff *skb) {
  struct _cdt *cb;
  struct _cdt *cdt;
  unsigned char flags = 0;
  __u16 len = dn_ntohs(*(__u16 *)skb->data);
  unsigned char padlen = 0;
  struct _scs * msg=skb->data;
  struct _scs * scs;
  struct _sbnb * sbnb;
  scs=getppdscs(msg);
  
  if (scs->scs$w_mtype) { // if other than con_req
    cdt=&cdtl[scs->scs$l_dst_conid];
  } else {
    cdt=find_free_cdt();
  }

  switch (scs->scs$w_mtype) {
  case SCS$C_CON_REQ: 
    cdt->cdt$l_rconid=scs->scs$l_src_conid;
    scs_msg_ctl_comm(cdt,SCS$C_CON_RSP);
    cdt->cdt$w_state=CDT$C_CON_REC;
    // do an accept or reject
    //cdt->cdt$w_state=CDT$C_REJ_SENT
    //scs_msg_ctl_comm(cdt,SCS$C_REJ_REQ);
    scs$accept(0,0,0,0,0,0,0,0,0,0,0,0,cdt,0);
    cdt->cdt$l_condat=vmalloc(16);
    cdt->cdt$l_lprocnam=vmalloc(16);
    bcopy(&scs->scs$b_con_dat,cdt->cdt$l_condat,16);
    bcopy(&scs->scs$t_dst_proc,cdt->cdt$l_lprocnam,16);
    sbnb=scs_find_name(cdt->cdt$l_lprocnam);
    //    cdt->cdt$l_lconid=sbnb->sbnb$w_local_index;
    cdt->cdt$l_msginput=cdtl[sbnb->sbnb$w_local_index].cdt$l_msginput;
    cdt->cdt$l_dginput=cdtl[sbnb->sbnb$w_local_index].cdt$l_dginput;
    cdt->cdt$w_state=CDT$C_ACCP_SENT;
    scs_msg_ctl_comm(cdt,SCS$C_ACCP_REQ);
    break;
  case SCS$C_CON_RSP: 
    cdt->cdt$l_rconid=scs->scs$l_src_conid;
    cdt->cdt$w_state=CDT$C_CON_ACK;
    break;
  case SCS$C_ACCP_REQ: 
    scs_msg_ctl_comm(cdt,SCS$C_ACCP_RSP);
    cdt->cdt$w_state=CDT$C_OPEN;
    break;
  case SCS$C_ACCP_RSP: 
    cdt->cdt$w_state=CDT$C_OPEN;
    break;
  case SCS$C_REJ_REQ: 
    scs_msg_ctl_comm(cdt,SCS$C_REJ_RSP);
    cdt->cdt$w_state=CDT$C_CLOSED;
    // and then kill it;
    break;
  case SCS$C_REJ_RSP: 
    cdt->cdt$w_state=CDT$C_CLOSED;
    // and then kill it?
    break;
  case SCS$C_DISC_REQ: 
    scs_msg_ctl_comm(cdt,SCS$C_DISC_RSP);
    if (cdt->cdt$w_state == CDT$C_OPEN) {
      cdt->cdt$w_state=CDT$C_DISC_REC;
    } else {
      cdt->cdt$w_state=CDT$C_CLOSED;
      // and then kill it
    }
    break;
  case SCS$C_DISC_RSP: 
    if (cdt->cdt$w_state == CDT$C_DISC_MTCH) {
      cdt->cdt$w_state=CDT$C_CLOSED;
      // and then kill it;
    } else {
      cdt->cdt$w_state=CDT$C_DISC_ACK;
    }
    break;

  }
}

//int nisca_snt_dg (struct sk_buff * skb, void * addr) 
int nisca_snt_dg (struct sk_buff * skb) { 
  //  return do_opc_dispatch(skb);
  void * addr = getppdscs(skb->data);
  struct _scs * scs = addr;
  struct _cdt * cdt = &cdtl[scs->scs$l_dst_conid];
  // shortcut
  struct _mscp_basic_pkt * basic = ((unsigned long)addr) + sizeof(*scs);

  struct _sbnb * sbnb = scs_find_name(&scs->scs$t_dst_proc);

  struct _cdt * acdt = &cdtl[sbnb->sbnb$w_local_index];

  {
    void (*fn)(void *,void *,void *);
#if 0
    int savipl=setipl(0); // still something funny someplace
    int savis=current->psl_is;
    current->psl_is=0;
#endif
    fn=cdt->cdt$l_msginput;
    fn(addr,cdt,0);
#if 0
    if (savis) current->psl_is=1;
    setipl(savipl);
#endif
  }

#if 0  
  if (basic->mscp$b_opcode == MSCP$K_OP_END) {
    du_dg(addr,cdt,0);
  } else {
    int savipl=setipl(0); // still something funny someplace
    int savis=current->psl_is;
    current->psl_is=0;
    mscplisten(addr,cdt,0);
    if (savis) current->psl_is=1;
    setipl(savipl);
  }
#endif
}

int nisca_snt_lb (struct sk_buff * skb, void * addr) { }
int nisca_snt_seq (struct sk_buff * skb, void * addr) { }
int nisca_snt_datm (struct sk_buff * skb, void * addr) { }
int nisca_snt_dat (struct sk_buff * skb, void * addr) { }
int nisca_snt_dat_lp (struct sk_buff * skb, void * addr) { }
int nisca_req_id (struct sk_buff * skb, void * addr) { }
int nisca_req_datm (struct sk_buff * skb, void * addr) { }
int nisca_req_dat0 (struct sk_buff * skb, void * addr) { }
int nisca_req_dat1 (struct sk_buff * skb, void * addr) { }
int nisca_req_dat2 (struct sk_buff * skb, void * addr) { }
int nisca_ret_datm (struct sk_buff * skb, void * addr) { }
int nisca_ret_dat (struct sk_buff * skb, void * addr) { }
int nisca_ret_dat_lp (struct sk_buff * skb, void * addr) { }
int nisca_ret_cnfm (struct sk_buff * skb, void * addr) { }
int nisca_ret_cnf (struct sk_buff * skb, void * addr) { }
int nisca_ret_id (struct sk_buff * skb, void * addr) { }
int nisca_ret_lb (struct sk_buff * skb, void * addr) { }
int nisca_reset (struct sk_buff * skb, void * addr) { }
int nisca_start (struct sk_buff * skb, void * addr) { }
int nisca_snt_datwm (struct sk_buff * skb, void * addr) { }
int nisca_snt_datwm_lp (struct sk_buff * skb, void * addr) { }
int nisca_ret_cnfwm (struct sk_buff * skb, void * addr) { }

int opc_snddg (struct sk_buff * skb, void * addr) { }
int opc_sndmsg (struct sk_buff * skb, void * addr) { }
int opc_retcnf (struct sk_buff * skb, void * addr) { }
int opc_reqid (struct sk_buff * skb, void * addr) { }
int opc_sndrst (struct sk_buff * skb, void * addr) { }
int opc_sndstrt (struct sk_buff * skb, void * addr) { }
int opc_reqdat (struct sk_buff * skb, void * addr) { }
int opc_reqdat1 (struct sk_buff * skb, void * addr) { }
int opc_reqdat2 (struct sk_buff * skb, void * addr) { }
int opc_sndlb (struct sk_buff * skb, void * addr) { }
int opc_reqmdat (struct sk_buff * skb, void * addr) { }
int opc_snddat (struct sk_buff * skb, void * addr) { }
int opc_retdat (struct sk_buff * skb, void * addr) { }
int opc_sndmdat (struct sk_buff * skb, void * addr) { }
int opc_invtc (struct sk_buff * skb, void * addr) { }
int opc_setckt (struct sk_buff * skb, void * addr) { }
int opc_rdcnt (struct sk_buff * skb, void * addr) { }
int opc_setckt_len (struct sk_buff * skb, void * addr) { }
int opc_dgrec (struct sk_buff * skb, void * addr) { }
int opc_msgrec2 (struct sk_buff * skb, void * addr) { }
int opc_cnfrec (struct sk_buff * skb, void * addr) { }
int opc_mcnfrec (struct sk_buff * skb, void * addr) { }
int opc_idrec (struct sk_buff * skb, void * addr) { }
int opc_lbrec (struct sk_buff * skb, void * addr) { }
int opc_datrec (struct sk_buff * skb, void * addr) { }
int opc_mdatrec (struct sk_buff * skb, void * addr) { }
int opc_snddatwm (struct sk_buff * skb, void * addr) { }
int opc_cnfwmrec (struct sk_buff * skb, void * addr) { }
int opc_retcnfwm (struct sk_buff * skb, void * addr) { }

unsigned long nisca_dispatch[0x17]={
nisca_snt_dg,
nisca_snt_lb,
nisca_snt_seq,
nisca_snt_datm,
nisca_snt_dat,
nisca_snt_dat_lp,
nisca_req_id,
nisca_req_datm,
nisca_req_dat0,
nisca_req_dat1,
nisca_req_dat2,
nisca_ret_datm,
nisca_ret_dat,
nisca_ret_dat_lp,
nisca_ret_cnfm,
nisca_ret_cnf,
nisca_ret_id,
nisca_ret_lb,
nisca_reset,
nisca_start,
nisca_snt_datwm,
nisca_snt_datwm_lp,
nisca_ret_cnfwm
};

unsigned long opc_dispatch[0x40] =
{
0,
opc_snddg,
opc_sndmsg,
opc_retcnf,
0,
opc_reqid,
opc_sndrst,
opc_sndstrt,
opc_reqdat,
opc_reqdat1,
opc_reqdat2,
0,
0,
opc_sndlb,
opc_reqmdat,
0,
opc_snddat,
opc_retdat,
opc_sndmdat,
0,
0,
0,
0,
0,
opc_invtc,
opc_setckt,
opc_rdcnt,
0,
opc_setckt_len,
0,
0,
0,
0,
opc_dgrec,
opc_msgrec, // not used?
opc_cnfrec,
0,
0,
0,
0,
0,
opc_mcnfrec,
0,
opc_idrec,
0,
opc_lbrec,
0,
0,
0,
opc_datrec,
0,
opc_mdatrec,
0,
0,
0,
0,
0,
0,
0,
0,
opc_snddatwm,
opc_cnfwmrec,
opc_retcnfwm,
0
};

void * getdx(void * buf) {
  return buf;
}

void * getcc(void * buf) {
  unsigned long l=(unsigned long)buf;
  struct _nisca * nisca=(struct _nisca *)(l+14);
  return nisca;
}

void * gettr(void * buf) {
  unsigned long l=(unsigned long)buf;
  struct _nisca * nisca=(struct _nisca *)(l+14);
  return nisca;
}

void * getppdscs(void * buf) {
  unsigned long l=(unsigned long)buf;
  struct _nisca * nisca=(struct _nisca *)(l+14);
  unsigned long tr_flag=nisca->nisca$b_tr_flag;
  unsigned long tr_pad=nisca->nisca$b_tr_pad_data_len;
  unsigned long retadr=(unsigned long)(&nisca->nisca$b_tr_pad_data_len)+tr_pad;
  retadr-=16; // offset related
  return retadr;
}

int dn_route_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt)
{
      struct _cdt *cb;
      unsigned char flags = 0;
      __u16 len = dn_ntohs(*(__u16 *)skb->data);
      struct dn_dev *dn = (struct dn_dev *)dev->dn_ptr;
      unsigned char padlen = 0;
      struct _nisca * nisca;
      unsigned char tr_flag;
      unsigned char tr_pad;
      unsigned char * msg; 
      struct _ppd *ppd;
      struct _scs *scs;
      struct _nisca *dx;
      int (*func)();

      if ((skb = skb_share_check(skb, GFP_ATOMIC)) == NULL)
              goto out;

      skb_pull(skb, 2);

      skb_trim(skb, len);

      msg=skb->data;
      nisca=gettr(msg);
      scs=getppdscs(msg);
      ppd=getppdscs(msg);
      dx=getdx(msg);

      if (scs_from_myself(dx,0xaa00,(system_utsname.nodename[0]<<16)+system_utsname.nodename[1])) {
	//printk("discarding packet from myself (mcast...?)\n");
	goto dump_it;
      }

      tr_flag=nisca->nisca$b_tr_flag;
      tr_pad=nisca->nisca$b_tr_pad;

      /*
       * If we have padding, remove it.
       */
      if (flags & DN_RT_F_PF) {
              padlen = flags & ~DN_RT_F_PF;
              skb_pull(skb, padlen);
              flags = *skb->data;
      }

	skb->nh.raw = skb->data;

	if (decnet_debug_level & 1)
		printk(KERN_DEBUG 
			"dn_route_rcv: got 0x%02x from %s [%d %d %d]\n",
			(int)flags, (dev) ? dev->name : "???", len, skb->len, 
			padlen);

	if (nisca->nisca$b_msg & NISCA$M_TR_CCFLG) {
	  switch((nisca->nisca$b_msg)&0x1f) {
	  case NISCA$C_HELLO:
	    return NF_HOOK(PF_DECnet, NF_DN_HELLO, skb, skb->dev, NULL, dn_neigh_endnode_hello);
	  }
	}

	if (nisca->nisca$b_msg & NISCA$M_TR_CTL) {
	    return NF_HOOK(PF_DECnet, NF_DN_HELLO, skb, skb->dev, NULL, opc_msgrec);
	}

	if (scs->scs$w_mtype>0x17)
	  panic("scs$w_mtype too large\n");
#if 0
	func=nisca_dispatch[scs->scs$w_mtype];
	return func(skb,scs);
#endif
	//	nisca_snt_dg(skb,scs);
	return NF_HOOK(PF_DECnet, NF_DN_HELLO, skb, skb->dev, NULL, nisca_snt_dg);
dump_it:
kfree_skb(skb);
out:
return NET_RX_DROP;
}

int do_opc_dispatch(struct sk_buff *skb)
{
  int (*func)();
  struct _ppd * ppd=getppdscs(skb->data);
  if (ppd->ppd$b_opc>0x40)
    panic("ppd$b_opc too large\n");
  func=opc_dispatch[ppd->ppd$b_opc];
  return func(skb,ppd);
}

static int dn_output(struct sk_buff *skb)
{
	struct dst_entry *dst = skb->dst;
	struct dn_route *rt = (struct dn_route *)dst;
	struct net_device *dev = dst->dev;
	struct _cdt *cb = (skb);
	struct neighbour *neigh;

	int err = -EINVAL;

	if ((neigh = dst->neighbour) == NULL)
		goto error;

	skb->dev = dev;

	cb->cdt$l_lconid = rt->rt_saddr;
	cb->cdt$l_rconid = rt->rt_daddr;

	/*
	 * Always set the Intra-Ethernet bit on all outgoing packets
	 * originated on this node. Only valid flag from upper layers
	 * is return-to-sender-requested. Set hop count to 0 too.
	 */
	cb->cdt$b_ls_flag &= ~DN_RT_F_RQR;
	cb->cdt$b_ls_flag |= DN_RT_F_IE;


	return NF_HOOK(PF_DECnet, NF_DN_LOCAL_OUT, skb, NULL, dev, neigh->output);

error:
	if (net_ratelimit())
		printk(KERN_DEBUG "dn_output: This should not happen\n");

	kfree_skb(skb);

	return err;
}

/*
 * Drop packet. This is used for endnodes and for
 * when we should not be forwarding packets from
 * this dest.
 */
static int dn_blackhole(struct sk_buff *skb)
{
	kfree_skb(skb);
	return NET_RX_DROP;
}

/*
 * Used to catch bugs. This should never normally get
 * called.
 */
static int dn_rt_bug(struct sk_buff *skb)
{
	if (net_ratelimit()) {
		struct _cdt *cb = (skb);

		printk(KERN_DEBUG "dn_rt_bug: skb from:%04x to:%04x\n",
				cb->cdt$l_lconid, cb->cdt$l_rconid);
	}

	kfree_skb(skb);

	return NET_RX_BAD;
}

static int dn_route_output_slow(struct dst_entry **pprt, dn_address dst, dn_address src, int flags)
{
	struct dn_route *rt = NULL;
	struct net_device *dev = decnet_default_device;
	struct neighbour *neigh = NULL;
	struct dn_dev *dn_db;
	unsigned hash;

	/* Look in On-Ethernet cache first */
	if (!(flags & MSG_TRYHARD)) {
		if ((neigh = dn_neigh_lookup(&dn_neigh_table, &dst)) != NULL)
			goto got_route;
	}

	if (dev == NULL)
		return -EINVAL;

	dn_db = dev->dn_ptr;

	if (dn_db == NULL)
		return -EINVAL;

	/* Try default router */
	if ((neigh = neigh_clone(dn_db->router)) != NULL)
		goto got_route;

	/* Send to default device (and hope for the best) if above fail */
	if ((neigh = __neigh_lookup(&dn_neigh_table, &dst, dev, 1)) != NULL)
		goto got_route;


	return -EINVAL;

got_route:

	if ((rt = dst_alloc(&dn_dst_ops)) == NULL) {
		neigh_release(neigh);
		return -EINVAL;
	}

	dn_db = (struct dn_dev *)neigh->dev->dn_ptr;
	
	rt->key.saddr  = src;
	rt->rt_saddr   = src;
	rt->key.daddr  = dst;
	rt->rt_daddr   = dst;
	rt->key.oif    = neigh ? neigh->dev->ifindex : -1;
	rt->key.iif    = 0;
	rt->key.fwmark = 0;

	rt->u.dst.neighbour = neigh;
	rt->u.dst.dev = neigh ? neigh->dev : NULL;
	rt->u.dst.lastuse = jiffies;
	rt->u.dst.output = dn_output;
	rt->u.dst.input  = dn_rt_bug;

	if (dn_dev_islocal(neigh->dev, rt->rt_daddr))
		rt->u.dst.input = dn_nsp_rx;

	hash = dn_hash(rt->key.saddr, rt->key.daddr);
	dn_insert_route(rt, hash);
	*pprt = &rt->u.dst;

	return 0;
}

int dn_route_output(struct dst_entry **pprt, dn_address dst, dn_address src, int flags)
{
	unsigned hash = dn_hash(src, dst);
	struct dn_route *rt = NULL;

	if (!(flags & MSG_TRYHARD)) {
		read_lock_bh(&dn_rt_hash_table[hash].lock);
		for(rt = dn_rt_hash_table[hash].chain; rt; rt = rt->u.rt_next) {
			if ((dst == rt->key.daddr) &&
					(src == rt->key.saddr) &&
					(rt->key.iif == 0) &&
					(rt->key.oif != 0)) {
				rt->u.dst.lastuse = jiffies;
				dst_hold(&rt->u.dst);
				rt->u.dst.__use++;
				read_unlock_bh(&dn_rt_hash_table[hash].lock);
				*pprt = &rt->u.dst;
				return 0;
			}
		}
		read_unlock_bh(&dn_rt_hash_table[hash].lock);
	}

	return dn_route_output_slow(pprt, dst, src, flags);
}

static int dn_route_input_slow(struct sk_buff *skb)
{
	struct dn_route *rt = NULL;
	struct _cdt *cb = (skb);
	struct net_device *dev = skb->dev;
	struct dn_dev *dn_db;
	struct neighbour *neigh = NULL;
	int (*dnrt_input)(struct sk_buff *skb);
	int (*dnrt_output)(struct sk_buff *skb);
	u32 fwmark = 0;
	unsigned hash;
	dn_address saddr = cb->cdt$l_lconid;
	dn_address daddr = cb->cdt$l_rconid;
	if (dev == NULL)
		return -EINVAL;

	if ((dn_db = dev->dn_ptr) == NULL)
		return -EINVAL;

	/*
	 * In this case we've just received a packet from a source
	 * outside ourselves pretending to come from us. We don't
	 * allow it any further to prevent routing loops, spoofing and
	 * other nasties. Loopback packets already have the dst attached
	 * so this only affects packets which have originated elsewhere.
	 */
	if (dn_dev_islocal(dev, cb->cdt$l_lconid))
		return -ENOTUNIQ;

	/*
	 * Default is to create a drop everything entry
	 */
	dnrt_input  = dn_blackhole;
	dnrt_output = dn_rt_bug;

	/*
	 * Is the destination us ?
	 */
	if (!dn_dev_islocal(dev, cb->cdt$l_rconid))
		goto non_local_input;

	/*
	 * Local input... find source of skb
	 */
	dnrt_input  = dn_nsp_rx;
	dnrt_output = dn_output;
	saddr = cb->cdt$l_rconid;
	daddr = cb->cdt$l_lconid;

	if ((neigh = neigh_lookup(&dn_neigh_table, &cb->cdt$l_lconid, dev)) != NULL)
		goto add_entry;

	if (dn_db->router && ((neigh = neigh_clone(dn_db->router)) != NULL))
		goto add_entry;

	neigh = neigh_create(&dn_neigh_table, &cb->cdt$l_lconid, dev);
	if (!IS_ERR(neigh)) {
		if (dev->type == ARPHRD_ETHER)
			memcpy(neigh->ha, skb->mac.ethernet->h_source, ETH_ALEN);
		goto add_entry;
	}

	return PTR_ERR(neigh);

non_local_input:

add_entry:

	if ((rt = dst_alloc(&dn_dst_ops)) == NULL) {
                neigh_release(neigh);
                return -EINVAL;
        }

	rt->key.saddr  = cb->cdt$l_lconid;
	rt->rt_saddr   = saddr;
	rt->key.daddr  = cb->cdt$l_rconid;
	rt->rt_daddr   = daddr;
	rt->key.oif    = 0;
	rt->key.iif    = dev->ifindex;
	rt->key.fwmark = fwmark;

	rt->u.dst.neighbour = neigh;
	rt->u.dst.dev = neigh ? neigh->dev : NULL;
	rt->u.dst.lastuse = jiffies;
	rt->u.dst.output = dnrt_output;
	rt->u.dst.input = dnrt_input;

	hash = dn_hash(rt->key.saddr, rt->key.daddr);
	dn_insert_route(rt, hash);
	skb->dst = (struct dst_entry *)rt;

	return 0;
}

int dn_route_input(struct sk_buff *skb)
{
	struct dn_route *rt;
	struct _cdt *cb = (skb);
	unsigned hash = dn_hash(cb->cdt$l_lconid, cb->cdt$l_rconid);

	if (skb->dst)
		return 0;

	read_lock(&dn_rt_hash_table[hash].lock);
	for(rt = dn_rt_hash_table[hash].chain; rt != NULL; rt = rt->u.rt_next) {
		if ((rt->key.saddr == cb->cdt$l_lconid) &&
				(rt->key.daddr == cb->cdt$l_rconid) &&
				(rt->key.oif == 0) &&
#ifdef CONFIG_DECNET_ROUTE_FWMARK
				(rt->key.fwmark == skb->nfmark) &&
#endif
		    1) {
			rt->u.dst.lastuse = jiffies;
			dst_hold(&rt->u.dst);
			rt->u.dst.__use++;
			read_unlock(&dn_rt_hash_table[hash].lock);
			skb->dst = (struct dst_entry *)rt;
			return 0;
		}
	}
	read_unlock(&dn_rt_hash_table[hash].lock);

	return dn_route_input_slow(skb);
}

static int dn_rt_fill_info(struct sk_buff *skb, u32 pid, u32 seq, int event, int nowait)
{
	struct dn_route *rt = (struct dn_route *)skb->dst;
	struct rtmsg *r;
	struct nlmsghdr *nlh;
	unsigned char *b = skb->tail;

	nlh = NLMSG_PUT(skb, pid, seq, event, sizeof(*r));
	r = NLMSG_DATA(nlh);
	nlh->nlmsg_flags = nowait ? NLM_F_MULTI : 0;
	r->rtm_family = AF_DECnet;
	r->rtm_dst_len = 16;
	r->rtm_src_len = 16;
	r->rtm_tos = 0;
	r->rtm_table = 0;
	r->rtm_type = 0;
	r->rtm_flags = 0;
	r->rtm_scope = RT_SCOPE_UNIVERSE;
	r->rtm_protocol = RTPROT_UNSPEC;
	RTA_PUT(skb, RTA_DST, 2, &rt->rt_daddr);
	RTA_PUT(skb, RTA_SRC, 2, &rt->rt_saddr);
	if (rt->u.dst.dev)
		RTA_PUT(skb, RTA_OIF, sizeof(int), &rt->u.dst.dev->ifindex);
	if (rt->u.dst.window)
		RTA_PUT(skb, RTAX_WINDOW, sizeof(unsigned), &rt->u.dst.window);
	if (rt->u.dst.rtt)
		RTA_PUT(skb, RTAX_RTT, sizeof(unsigned), &rt->u.dst.rtt);

	nlh->nlmsg_len = skb->tail - b;
	return skb->len;

nlmsg_failure:
rtattr_failure:
        skb_trim(skb, b - skb->data);
        return -1;
}

/*
 * This is called by both endnodes and routers now.
 */
int dn_cache_getroute(struct sk_buff *in_skb, struct nlmsghdr *nlh, void *arg)
{
	struct rtattr **rta = arg;
	struct dn_route *rt = NULL;
	struct _cdt *cb;
	dn_address dst = 0;
	dn_address src = 0;
	int iif = 0;
	int err;
	struct sk_buff *skb;

	skb = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
	if (skb == NULL)
		return -ENOBUFS;
	skb->mac.raw = skb->data;
	cb = (skb);

	if (rta[RTA_SRC-1])
		memcpy(&src, RTA_DATA(rta[RTA_SRC-1]), 2);
	if (rta[RTA_DST-1])
		memcpy(&dst, RTA_DATA(rta[RTA_DST-1]), 2);
	if (rta[RTA_IIF-1])
		memcpy(&iif, RTA_DATA(rta[RTA_IIF-1]), sizeof(int));

	if (iif) {
		struct net_device *dev;
		if ((dev = dev_get_by_index(iif)) == NULL) {
			kfree_skb(skb);
			return -ENODEV;
		}
		if (!dev->dn_ptr) {
			dev_put(dev);
			kfree_skb(skb);
			return -ENODEV;
		}
		skb->protocol = __constant_htons(ETH_P_MYSCS);
		skb->dev = dev;
		cb->cdt$l_lconid = src;
		cb->cdt$l_rconid = dst;
		local_bh_disable();
		err = dn_route_input(skb);
		local_bh_enable();
		memset(cb, 0, sizeof(struct _cdt));
		rt = (struct dn_route *)skb->dst;
	} else {
		err = dn_route_output((struct dst_entry **)&rt, dst, src, 0);
	}

	if (!err && rt->u.dst.error)
		err = rt->u.dst.error;
	if (skb->dev)
		dev_put(skb->dev);
	skb->dev = NULL;
	if (err)
		goto out_free;
	skb->dst = &rt->u.dst;

	NETLINK_CB(skb).dst_pid = NETLINK_CB(in_skb).pid;

	err = dn_rt_fill_info(skb, NETLINK_CB(in_skb).pid, nlh->nlmsg_seq, RTM_NEWROUTE, 0);

	if (err == 0)
		goto out_free;
	if (err < 0) {
		err = -EMSGSIZE;
		goto out_free;
	}

	err = netlink_unicast(rtnl, skb, NETLINK_CB(in_skb).pid, MSG_DONTWAIT);

	return err;

out_free:
	kfree_skb(skb);
	return err;
}

/*
 * For routers, this is called from dn_fib_dump, but for endnodes its
 * called directly from the rtnetlink dispatch table.
 */
int dn_cache_dump(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct dn_route *rt;
	int h, s_h;
	int idx, s_idx;

	if (NLMSG_PAYLOAD(cb->nlh, 0) < sizeof(struct rtmsg))
		return -EINVAL;
	if (!(((struct rtmsg *)NLMSG_DATA(cb->nlh))->rtm_flags&RTM_F_CLONED))
		return 0;

	s_h = cb->args[0];
	s_idx = idx = cb->args[1];
	for(h = 0; h <= dn_rt_hash_mask; h++) {
		if (h < s_h)
			continue;
		if (h > s_h)
			s_idx = 0;
		read_lock_bh(&dn_rt_hash_table[h].lock);
		for(rt = dn_rt_hash_table[h].chain, idx = 0; rt; rt = rt->u.rt_next, idx++) {
			if (idx < s_idx)
				continue;
			skb->dst = dst_clone(&rt->u.dst);
			if (dn_rt_fill_info(skb, NETLINK_CB(cb->skb).pid,
					cb->nlh->nlmsg_seq, RTM_NEWROUTE, 1) <= 0) {
				dst_release(xchg(&skb->dst, NULL));
				read_unlock_bh(&dn_rt_hash_table[h].lock);
				goto done;
			}
			dst_release(xchg(&skb->dst, NULL));
		}
		read_unlock_bh(&dn_rt_hash_table[h].lock);
	}

done:
	cb->args[0] = h;
	cb->args[1] = idx;
	return skb->len;
}

#ifdef CONFIG_PROC_FS

static int decnet_cache_get_info(char *buffer, char **start, off_t offset, int length)
{
        int len = 0;
        off_t pos = 0;
        off_t begin = 0;
	struct dn_route *rt;
	int i;
	char buf1[DN_ASCBUF_LEN], buf2[DN_ASCBUF_LEN];

	for(i = 0; i <= dn_rt_hash_mask; i++) {
		read_lock_bh(&dn_rt_hash_table[i].lock);
		rt = dn_rt_hash_table[i].chain;
		for(; rt != NULL; rt = rt->u.rt_next) {
			len += sprintf(buffer + len, "%-8s %-7s %-7s %04d %04d %04d\n",
					rt->u.dst.dev ? rt->u.dst.dev->name : "*",
					dn_addr2asc(dn_ntohs(rt->rt_daddr), buf1),
					dn_addr2asc(dn_ntohs(rt->rt_saddr), buf2),
					atomic_read(&rt->u.dst.__refcnt),
					rt->u.dst.__use,
					(int)rt->u.dst.rtt
					);



	                pos = begin + len;
	
        	        if (pos < offset) {
                	        len   = 0;
                        	begin = pos;
                	}
              		if (pos > offset + length)
                	        break;
		}
		read_unlock_bh(&dn_rt_hash_table[i].lock);
		if (pos > offset + length)
			break;
	}

        *start = buffer + (offset - begin);
        len   -= (offset - begin);

        if (len > length) len = length;

        return(len);
} 

#endif /* CONFIG_PROC_FS */

void __init dn_route_init(void)
{
	int i, goal, order;

	dn_dst_ops.kmem_cachep = kmem_cache_create("dn_dst_cache",
						   sizeof(struct dn_route),
						   0, SLAB_HWCACHE_ALIGN,
						   NULL, NULL);

	if (!dn_dst_ops.kmem_cachep)
		panic("DECnet: Failed to allocate dn_dst_cache\n");

	dn_route_timer.function = dn_dst_check_expire;
	dn_route_timer.expires = jiffies + decnet_dst_gc_interval * HZ;
	add_timer(&dn_route_timer);

	goal = num_physpages >> (26 - PAGE_SHIFT);

	for(order = 0; (1UL << order) < goal; order++)
		/* NOTHING */;

        /*
         * Only want 1024 entries max, since the table is very, very unlikely
         * to be larger than that.
         */
        while(order && ((((1UL << order) * PAGE_SIZE) / 
                                sizeof(struct dn_rt_hash_bucket)) >= 2048))
                order--;

        do {
                dn_rt_hash_mask = (1UL << order) * PAGE_SIZE /
                        sizeof(struct dn_rt_hash_bucket);
                while(dn_rt_hash_mask & (dn_rt_hash_mask - 1))
                        dn_rt_hash_mask--;
                dn_rt_hash_table = (struct dn_rt_hash_bucket *)
                        __get_free_pages(GFP_ATOMIC, order);
        } while (dn_rt_hash_table == NULL && --order > 0);

	if (!dn_rt_hash_table)
                panic("Failed to allocate DECnet route cache hash table\n");

	printk(KERN_INFO 
		"DECnet: Routing cache hash table of %u buckets, %ldKbytes\n", 
		dn_rt_hash_mask, 
		(long)(dn_rt_hash_mask*sizeof(struct dn_rt_hash_bucket))/1024);

	dn_rt_hash_mask--;
        for(i = 0; i <= dn_rt_hash_mask; i++) {
                dn_rt_hash_table[i].lock = RW_LOCK_UNLOCKED;
                dn_rt_hash_table[i].chain = NULL;
        }

        dn_dst_ops.gc_thresh = (dn_rt_hash_mask + 1);

#ifdef CONFIG_PROC_FS
	proc_net_create("myscs_cache",0,decnet_cache_get_info);
#endif /* CONFIG_PROC_FS */
}

void __exit dn_route_cleanup(void)
{
	del_timer(&dn_route_timer);
	dn_run_flush(0);

	proc_net_remove("myscs_cache");
}

/*
 * DECnet       An implementation of the DECnet protocol suite for the LINUX
 *              operating system.  DECnet is implemented using the  BSD Socket
 *              interface as the means of communication with the user level.
 *
 *              DECnet Socket Timer Functions
 *
 * Author:      Steve Whitehouse <SteveW@ACM.org>
 *
 *
 * Changes:
 *       Steve Whitehouse      : Made keepalive timer part of the same
 *                               timer idea.
 *       Steve Whitehouse      : Added checks for sk->sock_readers
 *       David S. Miller       : New socket locking
 *       Steve Whitehouse      : Timer grabs socket ref.
 */
#include <linux/net.h>
#include <linux/socket.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/timer.h>
#include <linux/spinlock.h>
#include <net/sock.h>
#include <asm/atomic.h>
#include <net/dn.h>

/*
 * Fast timer is for delayed acks (200mS max)
 * Slow timer is for everything else (n * 500mS)
 */

#define FAST_INTERVAL (HZ/5)
#define SLOW_INTERVAL (HZ/2)

static void dn_slow_timer(unsigned long arg);

void dn_start_slow_timer2(struct _cdt *sk)
{
	add_timer(&sk);
}

void dn_stop_slow_timer2(struct _cdt *sk)
{
	del_timer(&sk);
}

static void dn_slow_timer(unsigned long arg)
{
	struct _cdt *sk = (struct _cdt *)arg;
	struct _cdt *scp = sk;

	sock_hold(sk);
	

	/*
	 * The persist timer is the standard slow timer used for retransmits
	 * in both connection establishment and disconnection as well as
	 * in the RUN state. The different states are catered for by changing
	 * the function pointer in the socket. Setting the timer to a value
	 * of zero turns it off. We allow the persist_fxn to turn the
	 * timer off in a permant way by returning non-zero, so that
	 * timer based routines may remove sockets. This is why we have a
	 * sock_hold()/sock_put() around the timer to prevent the socket
	 * going away in the middle.
	 */
	/*
	 * Check for keepalive timeout. After the other timer 'cos if
	 * the previous timer caused a retransmit, we don't need to
	 * do this. scp->stamp is the last time that we sent a packet.
	 * The keepalive function sends a link service packet to the
	 * other end. If it remains unacknowledged, the standard
	 * socket timers will eventually shut the socket down. Each
	 * time we do this, scp->stamp will be updated, thus
	 * we won't try and send another until scp->keepalive has passed
	 * since the last successful transmission.
	 */
out:
	
	sock_put(sk);
}

static void dn_fast_timer(unsigned long arg)
{
	struct _cdt *sk = (struct _cdt *)arg;
	struct _cdt *scp = sk;

	
out:
	
}

void dn_start_fast_timer2(struct _cdt *sk)
{
	struct _cdt *scp = sk;

}

void dn_stop_fast_timer2(struct _cdt *sk)
{
	struct _cdt *scp = sk;

}

/*
 * DECnet       An implementation of the DECnet protocol suite for the LINUX
 *              operating system.  DECnet is implemented using the  BSD Socket
 *              interface as the means of communication with the user level.
 *
 *              DECnet sysctl support functions
 *
 * Author:      Steve Whitehouse <SteveW@ACM.org>
 *
 *
 * Changes:
 *
 */

#include <linux/mm.h>
#include <linux/sysctl.h>
#include <linux/fs.h>
#include <linux/netdevice.h>
#include <linux/string.h>
#include <net/neighbour.h>
#include <net/dst.h>

#include <asm/uaccess.h>

#include <net/dn.h>
#include <net/dn_dev.h>
#include <net/dn_route.h>


int decnet_debug_level;
int decnet_time_wait = 30;
int decnet_dn_count = 1;
int decnet_di_count = 3;
int decnet_dr_count = 3;
int decnet_log_martians = 1;
int decnet_no_fc_max_cwnd = NSP_MIN_WINDOW;

#ifdef CONFIG_SYSCTL
extern int decnet_dst_gc_interval;
static int min_decnet_time_wait[] = { 5 };
static int max_decnet_time_wait[] = { 600 };
static int min_state_count[] = { 1 };
static int max_state_count[] = { NSP_MAXRXTSHIFT };
static int min_decnet_dst_gc_interval[] = { 1 };
static int max_decnet_dst_gc_interval[] = { 60 };
static int min_decnet_no_fc_max_cwnd[] = { NSP_MIN_WINDOW };
static int max_decnet_no_fc_max_cwnd[] = { NSP_MAX_WINDOW };
static char node_name[7] = "???";

static struct ctl_table_header *dn_table_header = NULL;

/*
 * ctype.h :-)
 */
#define ISNUM(x) (((x) >= '0') && ((x) <= '9'))
#define ISLOWER(x) (((x) >= 'a') && ((x) <= 'z'))
#define ISUPPER(x) (((x) >= 'A') && ((x) <= 'Z'))
#define ISALPHA(x) (ISLOWER(x) || ISUPPER(x))
#define INVALID_END_CHAR(x) (ISNUM(x) || ISALPHA(x))

static void strip_it(char *str)
{
	for(;;) {
		switch(*str) {
			case ' ':
			case '\n':
			case '\r':
			case ':':
				*str = 0;
			case 0:
				return;
		}
		str++;
	}
}

/*
 * Simple routine to parse an ascii DECnet address
 * into a network order address.
 */
static int parse_addr(dn_address *addr, char *str)
{
	dn_address area, node;

	while(*str && !ISNUM(*str)) str++;

	if (*str == 0)
		return -1;

	area = (*str++ - '0');
	if (ISNUM(*str)) {
		area *= 10;
		area += (*str++ - '0');
	}

	if (*str++ != '.')
		return -1;

	if (!ISNUM(*str))
		return -1;

	node = *str++ - '0';
	if (ISNUM(*str)) {
		node *= 10;
		node += (*str++ - '0');
	}
	if (ISNUM(*str)) {
		node *= 10;
		node += (*str++ - '0');
	}
	if (ISNUM(*str)) {
		node *= 10;
		node += (*str++ - '0');
	}

	if ((node > 1023) || (area > 63))
		return -1;

	if (INVALID_END_CHAR(*str))
		return -1;

	*addr = dn_htons((area << 10) | node);

	return 0;
}


static int dn_node_address_strategy(ctl_table *table, int *name, int nlen,
				void *oldval, size_t *oldlenp,
				void *newval, size_t newlen,
				void **context)
{
	size_t len;
	dn_address addr;

	if (oldval && oldlenp) {
		if (get_user(len, oldlenp))
			return -EFAULT;
		if (len) {
			if (len != sizeof(unsigned short))
				return -EINVAL;
			if (put_user(decnet_address, (unsigned short *)oldval))
				return -EFAULT;
		}
	}
	if (newval && newlen) {
		if (newlen != sizeof(unsigned short))
			return -EINVAL;
		if (get_user(addr, (unsigned short *)newval))
			return -EFAULT;

		dn_dev_devices_off();

		decnet_address = addr;
		dn_dn2eth(decnet_ether_address, dn_ntohs(decnet_address));

		dn_dev_devices_on();
	}
	return 0;
}

static int dn_node_address_handler(ctl_table *table, int write, 
				struct file *filp,
				void *buffer, size_t *lenp)
{
	char addr[DN_ASCBUF_LEN];
	size_t len;
	dn_address dnaddr;

	if (!*lenp || (filp->f_pos && !write)) {
		*lenp = 0;
		return 0;
	}

	if (write) {
		int len = (*lenp < DN_ASCBUF_LEN) ? *lenp : (DN_ASCBUF_LEN-1);

		if (copy_from_user(addr, buffer, len))
			return -EFAULT;

		addr[len] = 0;
		strip_it(addr);

		if (parse_addr(&dnaddr, addr))
			return -EINVAL;

		dn_dev_devices_off();

		decnet_address = dnaddr;
		dn_dn2eth(decnet_ether_address, dn_ntohs(decnet_address));

		dn_dev_devices_on();

		filp->f_pos += len;

		return 0;
	}

	dn_addr2asc(dn_ntohs(decnet_address), addr);
	len = strlen(addr);
	addr[len++] = '\n';

	if (len > *lenp) len = *lenp;

	if (copy_to_user(buffer, addr, len))
		return -EFAULT;

	*lenp = len;
	filp->f_pos += len;

	return 0;
}


static int dn_def_dev_strategy(ctl_table *table, int *name, int nlen,
				void *oldval, size_t *oldlenp,
				void *newval, size_t newlen,
				void **context)
{
	size_t len;
	struct net_device *dev = decnet_default_device;
	char devname[17];
	size_t namel;

	devname[0] = 0;

	if (oldval && oldlenp) {
		if (get_user(len, oldlenp))
			return -EFAULT;
		if (len) {
			if (dev)
				strcpy(devname, dev->name);

			namel = strlen(devname) + 1;
			if (len > namel) len = namel;	

			if (copy_to_user(oldval, devname, len))
				return -EFAULT;

			if (put_user(len, oldlenp))
				return -EFAULT;
		}
	}

	if (newval && newlen) {
		if (newlen > 16)
			return -E2BIG;

		if (copy_from_user(devname, newval, newlen))
			return -EFAULT;

		devname[newlen] = 0;

		if ((dev = __dev_get_by_name(devname)) == NULL)
			return -ENODEV;

		if (dev->dn_ptr == NULL)
			return -ENODEV;

		decnet_default_device = dev;
	}

	return 0;
}


static int dn_def_dev_handler(ctl_table *table, int write, 
				struct file * filp,
				void *buffer, size_t *lenp)
{
	size_t len;
	int err;
	struct net_device *dev = decnet_default_device;
	char devname[17];

	if (!*lenp || (filp->f_pos && !write)) {
		*lenp = 0;
		return 0;
	}

	if (write) {
		if (*lenp > 16)
			return -E2BIG;

		if (copy_from_user(devname, buffer, *lenp))
			return -EFAULT;

		devname[*lenp] = 0;
		strip_it(devname);

		if ((dev = __dev_get_by_name(devname)) == NULL)
			return -ENODEV;

#if 0
		// not now. this will be set in dn_dev_create
		if (dev->dn_ptr == NULL)
			return -ENODEV;
#endif

		decnet_default_device = dev;
		filp->f_pos += *lenp;
		dn_dev_create(dev, &err); // put it here to make it start

		return 0;
	}

	if (dev == NULL) {
		*lenp = 0;
		return 0;
	}

	strcpy(devname, dev->name);
	len = strlen(devname);
	devname[len++] = '\n';

	if (len > *lenp) len = *lenp;

	if (copy_to_user(buffer, devname, len))
		return -EFAULT;

	*lenp = len;
	filp->f_pos += len;

	return 0;
}

static ctl_table dn_table[] = {
	{NET_DECNET_NODE_ADDRESS, "node_address", NULL, 7, 0644, NULL,
	dn_node_address_handler, dn_node_address_strategy, NULL,
	NULL, NULL},
	{NET_DECNET_NODE_NAME, "node_name", node_name, 7, 0644, NULL,
	&proc_dostring, &sysctl_string, NULL, NULL, NULL},
	{NET_DECNET_DEFAULT_DEVICE, "default_device", NULL, 16, 0644, NULL,
	dn_def_dev_handler, dn_def_dev_strategy, NULL, NULL, NULL},
	{NET_DECNET_TIME_WAIT, "time_wait", &decnet_time_wait,
	sizeof(int), 0644,
	NULL, &proc_dointvec_minmax, &sysctl_intvec, NULL,
	&min_decnet_time_wait, &max_decnet_time_wait},
	{NET_DECNET_DN_COUNT, "dn_count", &decnet_dn_count,
	sizeof(int), 0644,
	NULL, &proc_dointvec_minmax, &sysctl_intvec, NULL,
	&min_state_count, &max_state_count},
	{NET_DECNET_DI_COUNT, "di_count", &decnet_di_count,
	sizeof(int), 0644,
	NULL, &proc_dointvec_minmax, &sysctl_intvec, NULL,
	&min_state_count, &max_state_count},
	{NET_DECNET_DR_COUNT, "dr_count", &decnet_dr_count,
	sizeof(int), 0644,
	NULL, &proc_dointvec_minmax, &sysctl_intvec, NULL,
	&min_state_count, &max_state_count},
	{NET_DECNET_DST_GC_INTERVAL, "dst_gc_interval", &decnet_dst_gc_interval,
	sizeof(int), 0644,
	NULL, &proc_dointvec_minmax, &sysctl_intvec, NULL,
	&min_decnet_dst_gc_interval, &max_decnet_dst_gc_interval},
	{NET_DECNET_NO_FC_MAX_CWND, "no_fc_max_cwnd", &decnet_no_fc_max_cwnd,
	sizeof(int), 0644,
	NULL, &proc_dointvec_minmax, &sysctl_intvec, NULL,
	&min_decnet_no_fc_max_cwnd, &max_decnet_no_fc_max_cwnd},
	{NET_DECNET_DEBUG_LEVEL, "debug", &decnet_debug_level, 
	sizeof(int), 0644, 
	NULL, &proc_dointvec, &sysctl_intvec, NULL,
	NULL, NULL},
	{0}
};

static ctl_table dn_dir_table[] = {
	{NET_DECNET, "myscs", NULL, 0, 0555, dn_table},
	{0}
};

static ctl_table dn_root_table[] = {
	{CTL_NET, "net", NULL, 0, 0555, dn_dir_table},
	{0}
};

void dn_register_sysctl(void)
{
	dn_table_header = register_sysctl_table(dn_root_table, 1);
}

void dn_unregister_sysctl(void)
{
	unregister_sysctl_table(dn_table_header);
}

#else  /* CONFIG_SYSCTL */
void dn_unregister_sysctl(void)
{
}
void dn_register_sysctl(void)
{
}

#endif

#endif
