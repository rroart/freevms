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
#include<system_data_cells.h>
#include<ipl.h>
#include<linux/vmalloc.h>
#include<linux/pci.h>
#include<system_service_setup.h>
#include<descrip.h>

#include<linux/blkdev.h>

struct _ucbnidef * euu;

static unsigned long startio (struct _irp * i, struct _ucb * u)
{
  unsigned long sts=SS$_NORMAL;
  struct request * rq = kmalloc (sizeof(struct request), GFP_KERNEL);

  //printk("eu startio %x %x %x\n",i->irp$l_qio_p1,i->irp$l_qio_p2,i->irp$l_qio_p3);

  //  eu_drive_t *drive=u->ucb$l_orb;

  //i->irp$l_qio_p3 <<= 1;
#if 0
  i->irp$l_qio_p3 += partadd(u->ucb$l_orb,i->irp$l_qio_p4);
#endif
    //drive->part[i->irp$l_qio_p4].start_sect + drive->sect0;

  euu=u;

  switch (i->irp$v_fcode) {

    case IO$_WRITELBLK:
    case IO$_WRITEPBLK: {
      rq->cmd=WRITE;
      rq->buffer=i->irp$l_qio_p1;
      rq->nr_sectors=(i->irp$l_qio_p2+511)>>9;
      i->irp$l_qio_p5=rq;
#if 0
      do_rw_disk(u->ucb$l_orb,rq,i->irp$l_qio_p3);
#endif
      return (sts);
    }

    case IO$_READLBLK:
    case IO$_READPBLK: {
      rq->cmd=READ;
      rq->buffer=i->irp$l_qio_p1;
      rq->nr_sectors=(i->irp$l_qio_p2+511)>>9;
      i->irp$l_qio_p5=rq;
#if 0
      do_rw_disk(u->ucb$l_orb,rq,i->irp$l_qio_p3);
#endif
      return (sts);
    }

    /* Who knows what */

    default: {
      return (SS$_BADPARAM);
    }
  }
}

static struct _irp * globali;
static struct _ucb * globalu;

static void  startio3 (struct _irp * i, struct _ucb * u) { 
  ioc$reqcom(SS$_NORMAL,0,u);
  return;
};

static void  startio2 (struct _irp * i, struct _ucb * u) { 
  u->ucb$l_fpc=startio3;
  exe$iofork(i,u);
  return;
}

static void ubd_intr2(int irq, void *dev, struct pt_regs *unused)
{
  struct _irp * i;
  struct _ucb * u;
  void (*func)();

  if (intr_blocked(20))
    return;
  regtrap(REG_INTR,20);
  setipl(20);
  /* have to do this until we get things more in order */
  i=globali;
  u=globalu;

  func=u->ucb$l_fpc;
  func(i,u);
  myrei();
}

static struct _fdt eu$fdt = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

/* more yet undefined dummies */
//static void  startio ();
static void  unsolint (void) { };
static void  cancel (void) { };
static void  ioc_std$cancelio (void) { };
static void  regdump (void) { };
static void  diagbuf (void) { };
static void  errorbuf (void) { };
static void  unitinit (void) { };
static void  altstart (void) { };
static void  mntver (void) { };
static void  cloneducb (void) { };
static void  mntv_sssc (void) { };
static void  mntv_for (void) { };
static void  mntv_sqd (void) { };
static void  aux_storage (void) { };
static void  aux_routine (void) { };

static struct _ddt eu$ddt = {
  ddt$l_start: startio,
  ddt$l_unsolint: unsolint,
  ddt$l_fdt: &eu$fdt,
  ddt$l_cancel: cancel,
  ddt$l_regdump: regdump,
  ddt$l_diagbuf: diagbuf,
  ddt$l_errorbuf: errorbuf,
  ddt$l_unitinit: unitinit,
  ddt$l_altstart: altstart,
  ddt$l_mntver: mntver,
  ddt$l_cloneducb: cloneducb,
  ddt$w_fdtsize: 0,
  ddt$l_mntv_sssc: mntv_sssc,
  ddt$l_mntv_for: mntv_for,
  ddt$l_mntv_sqd: mntv_sqd,
  ddt$l_aux_storage: aux_storage,
  ddt$l_aux_routine: aux_routine
};

int lan$setmode(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int lan$sensemode(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int lan$setchar(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int lan$sensechar(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int eu$readblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int eu$writeblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

extern void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type);

void eu$struc_init (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ucb->ucb$b_flck=IPL$_IOLOCK8;
  ucb->ucb$b_dipl=IPL$_IOLOCK8;

  ucb->ucb$l_devchar = DEV$M_REC | DEV$M_AVL | DEV$M_CCL | DEV$M_FOD/*| DEV$M_OOV*/;

  ucb->ucb$l_devchar2 = DEV$M_NNM;
  ucb->ucb$b_devclass = DC$_MISC;
  ucb->ucb$b_devtype = DT$_TTYUNKN;
  ucb->ucb$w_devbufsiz = 132;

  ucb->ucb$l_devdepend = 99; // just something to fill

  // dropped the mutex stuff

  return;
}

void eu$struc_reinit (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ddb->ddb$ps_ddt=&eu$ddt;
  //dpt_store_isr(crb,nl_isr);
  return;
}

int eu$unit_init (struct _idb * idb, struct _ucb * ucb) {
  ucb->ucb$v_online = 0;
  //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

  // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
  // no adp or cram stuff

  // or ints etc
  
  ucb->ucb$v_online = 1;

  return SS$_NORMAL;
}

struct _dpt eu$dpt;
struct _ddb eu$ddb;
struct _ucbnidef eu$ucb ;
struct _crb eu$crb;

int eu$init_tables() {
  ini_dpt_name(&eu$dpt, "EUDRIVER");
  ini_dpt_adapt(&eu$dpt, 0);
  ini_dpt_defunits(&eu$dpt, 1);
  ini_dpt_ucbsize(&eu$dpt,sizeof(struct _ucbnidef));
  ini_dpt_struc_init(&eu$dpt, eu$struc_init);
  ini_dpt_struc_reinit(&eu$dpt, eu$struc_reinit);
  ini_dpt_ucb_crams(&eu$dpt, 1/*NUMBER_CRAMS*/);
  ini_dpt_end(&eu$dpt);

  ini_ddt_unitinit(&eu$ddt, eu$unit_init);
  ini_ddt_start(&eu$ddt, startio);
  ini_ddt_cancel(&eu$ddt, ioc_std$cancelio);
  ini_ddt_end(&eu$ddt);

  /* for the fdt init part */
  /* a lot of these? */

  ini_fdt_act(&eu$fdt,IO$_READLBLK,eu$readblk,1);
  ini_fdt_act(&eu$fdt,IO$_READPBLK,eu$readblk,1);
  ini_fdt_act(&eu$fdt,IO$_READVBLK,eu$readblk,1);
  ini_fdt_act(&eu$fdt,IO$_WRITELBLK,eu$writeblk,1);
  ini_fdt_act(&eu$fdt,IO$_WRITEPBLK,eu$writeblk,1);
  ini_fdt_act(&eu$fdt,IO$_WRITEVBLK,eu$writeblk,1);
  ini_fdt_act(&eu$fdt,IO$_SETMODE,lan$setmode,1);
  ini_fdt_act(&eu$fdt,IO$_SETCHAR,lan$setchar,1);
  ini_fdt_act(&eu$fdt,IO$_SENSEMODE,lan$sensemode,1);
  ini_fdt_act(&eu$fdt,IO$_SENSECHAR,lan$sensechar,1);
  ini_fdt_end(&eu$fdt);

  return SS$_NORMAL;
}

int eu_iodb_vmsinit(int dev) {
#if 0
  struct _ucb * ucb=&eu$ucb;
  struct _ddb * ddb=&eu$ddb;
  struct _crb * crb=&eu$crb;
#endif 
  struct _ucb * ucb=kmalloc(sizeof(struct _ucbnidef),GFP_KERNEL);
  struct _ddb * ddb=kmalloc(sizeof(struct _ddb),GFP_KERNEL);
  struct _crb * crb=kmalloc(sizeof(struct _crb),GFP_KERNEL);
  unsigned long idb=0,orb=0;

  bzero(ucb,sizeof(struct _ucbnidef));
  bzero(ddb,sizeof(struct _ddb));
  bzero(crb,sizeof(struct _crb));

#if 0
  init_ddb(&eu$ddb,&eu$ddt,&eu$ucb,"dqa");
  init_ucb(&eu$ucb, &eu$ddb, &eu$ddt, &eu$crb);
  init_crb(&eu$crb);
#endif

  ucb -> ucb$w_size = sizeof(struct _ucbnidef); // temp placed
  ((struct _ucbnidef *)ucb)->ucb$l_extra_l_1=dev;

  init_ddb(ddb,&eu$ddt,ucb,"eua");
  init_ucb(ucb, ddb, &eu$ddt, crb);
  init_crb(crb);

//  ioc_std$clone_ucb(&eu$ucb,&ucb);
  eu$init_tables();
  eu$struc_init (crb, ddb, idb, orb, ucb);
  eu$struc_reinit (crb, ddb, idb, orb, ucb);
  eu$unit_init (idb, ucb);

  insertdevlist(ddb);

  return ddb;

}

int eu_iodbunit_vmsinit(struct _ddb * ddb,int unitno,void * dsc) {
  unsigned short int chan;
  struct _ucbnidef * newucb;
  ioc_std$clone_ucb(ddb->ddb$ps_ucb/*&er$ucb*/,&newucb);
  exe$assign(dsc,&chan,0,0,0);
  //  registerdevchan(MKDEV(IDE0_MAJOR,unitno),chan);

  euu = newucb;

  return newucb;
}

extern char * mydevice;

int eu_vmsinit(int dev) {
  //struct _ucb * u=makeucbetc(&ddb,&ddt,&dpt,&fdt,"hda","hddriver");

  unsigned short chan0, chan1, chan2;
  $DESCRIPTOR(u0,"eua0");
  mydevice="eua0";
  unsigned long idb=0,orb=0;
  struct _ccb * ccb;
  struct _ucb * newucb0,*newucb1,*newucb2;
  struct _ddb * ddb;

  printk(KERN_INFO "dev here pre\n");

  ddb=eu_iodb_vmsinit(dev);

  /* for the fdt init part */
  /* a lot of these? */

  eu_iodbunit_vmsinit(ddb,0,&u0);

  printk(KERN_INFO "dev here\n");

  // return chan0;

  return ddb->ddb$ps_ucb->ucb$l_link;
}

int eu$readblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  return lan$readblk(i,p,u,c);
}

int eu$writeblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  if (i->irp$l_iosb) *(long long *)i->irp$l_iosb=SS$_NORMAL|0x080000000000;
  int sts = uml_net_start_xmit  (i, p, u, c); 
  return sts = SS$_NORMAL;
}

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

//	extern struct _ucb * euu;
	struct _ucb * u = euu;

#if 0
	/* If we can't allocate memory, try again next round. */
	if ((skb = dev_alloc_skb(dev->mtu)) == NULL) {
		lp->stats.rx_dropped++;
		reactivate_fd(lp->fd);
		return 0;
	}

	skb->dev = dev;
	skb_put(skb, dev->mtu);
	skb->mac.raw = skb->data;
#endif
	struct _cxb * cb1 = lan$alloc_cxb(4096);
	struct _cxb * cb2 = cb1->cxb$l_link;
	char * buf=kmalloc(4096,GFP_KERNEL);
	pkt_len = (*lp->read)(lp->fd, buf, dev->mtu, lp);
	memcpy(cb1->cxb$ps_pktdata, buf, 14);
	if (pkt_len>=14)
		memcpy(cb2->cxb$ps_pktdata, &buf[14], pkt_len - 14);
	kfree(buf);
	cb2->cxb$w_length=pkt_len - 14;

	reactivate_fd(lp->fd);
	if (pkt_len > 0) {
		lan$netif_rx(u, cb1);

		lp->stats.rx_bytes += pkt_len;
		lp->stats.rx_packets++;
		return pkt_len;
	}

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

static int uml_net_start_xmit(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
	struct _ucbnidef * ni = u; 
	struct net_device *dev=ni->ucb$l_extra_l_1;

	struct uml_net_private *lp = dev->priv;
	unsigned long flags;
	int len;

	netif_stop_queue(dev);

	spin_lock_irqsave(&lp->lock, flags);

	char * buf = lan$alloc_xmit_buf(i,u,dev->dev_addr);

	len = (*lp->write)(lp->fd, buf, i->irp$l_qio_p2 + 14, lp);

	if(len == (i->irp$l_qio_p2 + 14)) {
		lp->stats.tx_packets++;
		lp->stats.tx_bytes += len;
		dev->trans_start = jiffies;
		netif_start_queue(dev);

		/* this is normally done in the interrupt when tx finishes */
		// not yet? netif_wake_queue(dev);
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

	kfree(buf);
	// check dev_kfree_skb(skb);

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
	// not yet? netif_wake_queue(dev);
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
		if (i==0) {
			eu_vmsinit(devices[0].dev);
		}
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

struct sk_buff *ether_adjust_skb_not(struct sk_buff *skb, int extra)
{
#if 0
	if((skb != NULL) && (skb_tailroom(skb) < extra)){
	  	struct sk_buff *skb2;

		skb2 = skb_copy_expand(skb, 0, extra, GFP_ATOMIC);
		dev_kfree_skb(skb);
		skb = skb2;
	}
	if(skb != NULL) skb_put(skb, extra);
	return(skb);
#endif
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
