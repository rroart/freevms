// $Id$
// $Locker$

// Author. Linux people.
// Author. Roar Thronæs.

#if 1
#define ETH_P_MYSCS 0x6009

// These two here and others have done parts that are still here:

/*
 * DECnet       An implementation of the DECnet protocol suite for the LINUX
 *              operating system.  DECnet is implemented using the  BSD Socket
 *              interface as the means of communication with the user level.
 *
 * Authors:     Steve Whitehouse <SteveW@ACM.org>
 *              Eduardo Marcelo Serrat <emserrat@geocities.com>
 *
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

#include <linux/utsname.h>
#include <linux/vmalloc.h>
#include <linux/netfilter.h>

#include <ccdef.h>
#include <cdldef.h>
#include <cdtdef.h>
#include <cdrpdef.h>
#include <cdtdef.h>
#include <chdef.h>
#include <ddbdef.h>
#include <descrip.h>
#include <dyndef.h>
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
#include <ucbdef.h>
#include <vcdef.h>

extern struct _pb mypb;
extern struct _sb othersb;

extern struct _cdt cdtl[1024];
extern struct _rdt rdt;
extern struct _scs_rd rdtl[128];
extern struct _cdl cdl;

#define NF_DN_HELLO              5

#define DN_IFREQ_SIZE (sizeof(struct ifreq) - sizeof(struct sockaddr) + sizeof(struct sockaddr_dn))

static char dn_rt_all_end_mcast[ETH_ALEN] = {0xAB,0x00,0x04,0x01,0x00,0x00};
static char dn_rt_all_rt_mcast[ETH_ALEN]  = {0xAB,0x00,0x04,0x01,0x00,0x00};
static char dn_hiord[ETH_ALEN]            = {0xAB,0x00,0x04,0x01,0x00,0x00}; // remember to add clusterid + 1
static unsigned char dn_eco_version[3]    = {0x02,0x00,0x00};

struct net_device *decnet_default_device = 0;

int is_cluster_on() {
  return mypb.pb$w_state==PB$C_OPEN;
}

inline void dn_nsp_send2(struct sk_buff *skb)
{
  unsigned short int *pktlen;
#ifndef CONFIG_VMS
  pktlen = (unsigned short *)skb_push(skb,2);
#endif
  *pktlen = htons(skb->len - 2);
  skb->nh.raw = skb->data;
  skb->dev = decnet_default_device;
  //skb->dst = dst_clone(dst);
  dn_rt_finish_output2(skb,&mypb.pb$b_rstation);
  //  dev_queue_xmit(skb);
}

extern struct neigh_table dn_neigh_table;

static struct dn_dev *dn_dev_create(struct net_device *dev, int *err);
static void dn_dev_delete(struct net_device *dev);
static void rtmsg_ifa(int event, struct dn_ifaddr *ifa);

static void dn_send_brd_hello(struct net_device *dev);

#define DN_DEV_LIST_SIZE (sizeof(dn_dev_list)/sizeof(struct dn_dev_parms))

#define DN_DEV_PARMS_OFFSET(x) ((int) ((char *) &((struct dn_dev_parms *)0)->x))

static inline void dn_rt_finish_output2(struct sk_buff *skb, char *dst)
{
  struct net_device *dev = skb->dev;

  if ((dev->type != ARPHRD_ETHER) && (dev->type != ARPHRD_LOOPBACK))
    dst = NULL;

#ifndef CONFIG_VMS
  if (!dev->hard_header || (dev->hard_header(skb, dev, ETH_P_MYSCS,
					     dst, NULL, skb->len) >= 0))
    dev_queue_xmit(skb);
  else
    kfree_skb(skb);
#endif
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

#ifndef CONFIG_VMS
	msg = skb_put(skb,sizeof(*nisca));
	skb_put(skb,16);
#endif

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

        *pktlen = htons(skb->len - 2);

	skb->nh.raw = skb->data;

	dn_rt_finish_output2(skb, dn_rt_all_rt_mcast);
}


static void dn_send_brd_hello(struct net_device *dev)
{
	dn_send_endnode_hello(dev);
}

static void dn_dev_set_timer(struct net_device *dev);

static void dn_dev_timer_func(unsigned long arg)
{
	struct net_device *dev = (struct net_device *)arg;

	dn_send_brd_hello(dev);

	dn_dev_set_timer(dev);
}

static void dn_dev_set_timer(struct net_device *dev)
{
  struct timer_list timer;

  timer.data = (unsigned long)dev;
  timer.function = dn_dev_timer_func;
  timer.expires = jiffies + (10 * HZ);

  add_timer(&timer);
}

#ifdef CONFIG_PROC_FS

static int decnet_dev_get_info(char *buffer, char **start, off_t offset, int length)
{
        return 0;
}

#endif /* CONFIG_PROC_FS */

void __init dn_dev_init(void)
{

#ifdef CONFIG_PROC_FS
	proc_net_create("myscs_dev", 0, decnet_dev_get_info);
#endif /* CONFIG_PROC_FS */
}

void __exit dn_dev_cleanup(void)
{
#ifndef CONFIG_VMS
	rtnetlink_links[PF_DECnet] = NULL;
#endif

	proc_net_remove("myscs_dev");
}

static u32 dn_neigh_hash(const void *pkey, const struct net_device *dev);
static int dn_neigh_construct(struct neighbour *);
static void dn_short_error_report(struct neighbour *, struct sk_buff *);
static int dn_long_output(struct sk_buff *);

static int first_hello=0;

/*
 * Endnode hello message received
 */
extern int startconnect(int);

int dn_neigh_endnode_hello(struct sk_buff *skb)
{
	struct _nisca *msg = skb->data;
	struct neighbour *neigh;
	struct dn_neigh *dn;

	msg=getcc(msg);

 	if (0==strncmp("NODNAM",&msg->nisca$t_nodename[1],6))
	  return 1;

 	if (0==strncmp(&system_utsname.nodename,&msg->nisca$t_nodename[1],4))
	  return 1;

	if (!first_hello) {
	  first_hello++;
	  printk("scs received hello from node %s\n",&msg->nisca$t_nodename[1]);
	  {
	    signed long long time=-100000000;
	    //	    $DESCRIPTOR(tensec,"0 00:00:10.00");
	    //exe$bintim(&tensec,&time);
	    //time=-time;
	    exe$setimr(0,&time,startconnect,0,0);
	  }
	}

	// next should be more protocol-stuff or first a plain init

	mypb.pb$w_state=PB$C_OPEN;

	memcpy(&mypb.pb$b_rstation,&msg->nisca$ab_lan_hw_addr,6);
	memcpy(&othersb.sb$t_nodename,&msg->nisca$t_nodename,8);

#ifndef CONFIG_VMS
	kfree_skb(skb);
#endif
	return 0;
}

extern int decnet_log_martians;

/*
 * This function uses a slightly different lookup method
 * to find its sockets, since it searches on object name/number
 * rather than port numbers. Various tests are done to ensure that
 * the incoming data is in the correct format before it is queued to
 * a socket.
 */

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

#ifndef CONFIG_VMS
	if ((skb = alloc_skb(size + hdr, pri)) == NULL)
		return NULL;
#endif

	skb->protocol = __constant_htons(ETH_P_MYSCS);
	skb->pkt_type = PACKET_OUTGOING;

	//	if (sk)
	//	skb_set_owner_w(skb, sk);

#ifndef CONFIG_VMS
	skb_reserve(skb, hdr);
#endif

	return skb;
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
	if (condat) bcopy(condat,&scs->scs$b_con_dat,strlen(condat));

	dn_nsp_send2(skb);	
}

void scs_msg_ctl_fill(struct sk_buff *skb, struct _cdt * cdt, unsigned char msgflg)
{
	struct _nisca *nisca;
	struct _ppd * ppd;
	struct _scs * scs;
	struct _nisca *dx;
	void * data;
#ifndef CONFIG_VMS
	data = skb_put(skb,sizeof(*nisca));
#endif
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

	memcpy(&nisca->nisca$t_nodename,&othersb.sb$t_nodename,8);
}

void printscs(void * skbdata) {
	struct _nisca *nisca;
	struct _ppd * ppd;
	struct _scs * scs;
	struct _nisca *dx;
	void * data;
	data = (int)skbdata/*+14*/;
	nisca=gettr(data);
	ppd=getppdscs(data);
	scs=getppdscs(data);

	return;

	printk("\nSCS\n");
	printk("dx %x %x %x %x %x\n",nisca->nisca$w_dx_group,nisca->nisca$w_dx_dst_hi, nisca->nisca$l_dx_dst_lo,nisca->nisca$w_dx_src_hi,nisca->nisca$l_dx_src_lo);
	printk("ni %x %x %x %x %x\n",nisca->nisca$b_msg,nisca->nisca$l_maint,nisca->nisca$b_tr_flag,nisca->nisca$b_tr_pad,nisca->nisca$b_tr_pad_data_len);
	printk("na %s\n",&nisca->nisca$t_nodename[1]);
	printk("pp %x\n",ppd->ppd$b_opc); 
	printk("na %s %s %s\n",&scs->scs$t_dst_proc,&scs->scs$t_src_proc,&scs->scs$b_con_dat);
	printk("sc %x %x %x\n",scs->scs$w_mtype,scs->scs$l_dst_conid,scs->scs$l_src_conid);

}


void scs_msg_fill(struct sk_buff *skb, struct _cdt * cdt, unsigned char msgflg, struct _scs * newscs)
{
	struct _nisca *nisca;
	struct _ppd * ppd;
	struct _scs * scs;
	struct _nisca *dx;
	void * data;
#ifndef CONFIG_VMS
	data = skb_put(skb,sizeof(*nisca));
#endif
	scs_fill_dx(data,0xab00,0x04010000,0xaa00,(system_utsname.nodename[0]<<16)+system_utsname.nodename[1]);
	nisca=gettr(data);
	nisca->nisca$b_tr_flag=0;
	nisca->nisca$b_tr_pad=0x13;
	nisca->nisca$b_tr_pad_data_len=0x12;
	
	ppd=getppdscs(data);
	scs=getppdscs(data);

	bcopy(newscs,scs,sizeof(struct _scs));

	//ppd->ppb$b_opc=NISCA$C_MSGREC;
	
	//memcpy(&nisca->nisca$t_nodename,&othersb.sb$t_nodename,8);
}

void scs_msg_fill_more(struct sk_buff *skb,struct _cdt * cdt, struct _cdrp * cdrp, int bufsiz)
{
	struct _nisca *nisca;
	struct _ppd * ppd;
	struct _scs * scs;
	struct _nisca *dx;
	void * data;
#ifndef CONFIG_VMS
	data = skb_put(skb,sizeof(*nisca));
#endif
	data = skb->data;
	ppd=getppdscs(data);
	scs=getppdscs(data);

#ifndef CONFIG_VMS
	data = skb_put(skb,sizeof(*scs));
#endif
	data = (unsigned long)scs + sizeof(*scs);

	bcopy(cdrp->cdrp$l_msg_buf,data,bufsiz);

	//cdt->cdt$l_fp_scs_norecv=cdrp;
	//cdt->cdt$l_reserved3=current->pcb$l_pid;
	//cdt->cdt$l_reserved4=cdrp->cdrp$l_msg_buf;

#ifndef CONFIG_VMS
	data=skb_put(skb,bufsiz);
#endif
}

struct dn_rt_hash_bucket
{
	struct dn_route *chain;
	rwlock_t lock;
} __attribute__((__aligned__(8)));

static unsigned char dn_hiord_addr[6] = {0xAB,0x00,0x04,0x01,0x00,0x00}; // remember to add clusterid + 1

int dn_rt_min_delay = 2*HZ;
int dn_rt_max_delay = 10*HZ;

int decnet_dst_gc_interval = 2;

int opc_msgrec(struct sk_buff *skb) {
  struct _cdt *cb;
  struct _cdt *cdt;
  unsigned char flags = 0;
  __u16 len = ntohs(*(__u16 *)skb->data);
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
    //printk("CON_REQ src dst %x %x\n",scs->scs$l_src_conid,scs->scs$l_dst_conid);
    //printk("CON_REQ l r %x %x\n",cdt->cdt$l_lconid,cdt->cdt$l_rconid);
    cdt->cdt$l_rconid=scs->scs$l_src_conid;
    scs_msg_ctl_comm(cdt,SCS$C_CON_RSP);
    cdt->cdt$w_state=CDT$C_CON_REC;
    // do an accept or reject
    //cdt->cdt$w_state=CDT$C_REJ_SENT
    //scs_msg_ctl_comm(cdt,SCS$C_REJ_REQ);
    scs_std$accept(0,0,0,0,0,0,0,0,0,0,0,0,cdt,0);
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
    printk("CON_RSP src dst %x %x\n",scs->scs$l_src_conid,scs->scs$l_dst_conid);
    printk("CON_RSP l r %x %x\n",cdt->cdt$l_lconid,cdt->cdt$l_rconid);
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

#if 0
  // why did I add these?
  //struct _sbnb * sbnb = scs_find_name(&scs->scs$t_dst_proc);

  //struct _cdt * acdt = &cdtl[sbnb->sbnb$w_local_index];
#endif

  //printk("in nisca_snt_dg %s %x\n",&scs->scs$t_dst_proc,cdt);

  {
    void (*fn)(void *,void *,void *);
#if 0
    int savipl=setipl(0); // still something funny someplace
    int savis=current->psl_is;
    current->psl_is=0;
#endif
    fn=cdt->cdt$l_msginput;
    //printk("fn %x\n",fn);
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
#ifndef CONFIG_VMS
  kfree_skb(skb);
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

extern struct _ddt du$ddt;

void cf_err() { }
void cf_msg() { }
void cf_dg() { } 
void cf_listen (void * packet, struct _cdt * c, struct _pdt * p) {
  // just using something at some value to config ddb etc
  struct _scs * scs = packet;
  char * b = ((unsigned long)packet) + sizeof(*scs);
  char devnam[16];
  int num, i;
  char type;
  struct _ddb * ddb;
  struct _ucb * ucb;
  struct dsc$descriptor d;
  d.dsc$w_length=4;
  d.dsc$a_pointer=devnam;

 again:
  type=*b++;

  switch (type) {
  case 0:
    goto end;

  case DYN$C_DDB:
    memcpy(devnam,b,16);
    b+=16;
    printk("maybe creating remote ddb %s on other node\n",devnam);
#ifdef CONFIG_VMS
#ifdef __arch_um__
    if (0==strncmp(devnam,"daa",3)) 
      ddb=ubd_iodb_vmsinit();
#endif
#ifndef __arch_um__
    if (0==strncmp(devnam,"dqa",3)) 
      ddb=ide_iodb_vmsinit();
#endif
#endif
    if (0==strncmp(devnam,"dua",3)) 
      ddb=du_iodb_vmsinit();
    if (0==strncmp(devnam,"dfa",3)) {
      ddb=du_iodb_vmsinit(); // was file_
      du_iodb_clu_vmsinit(ddb->ddb$l_ucb);
      ddb->ddb$t_name[1]='f';
    }
    break;

  case DYN$C_UCB:
    num=*b++;
    printk("maybe creating remote ddb %s on other node with %x units\n",devnam,num);
    for (i=0;i<num;i++) {
      devnam[3]=48+i;
      ucb=0;
#ifdef CONFIG_VMS
#ifdef __arch_um__
      if (0==strncmp(devnam,"daa",3)) 
	ucb = ubd_iodbunit_vmsinit(ddb,i,&d);
#endif
#ifndef __arch_um__
      if (0==strncmp(devnam,"dqa",3)) 
	ucb = ide_iodbunit_vmsinit(ddb,i,&d);
#endif
#endif
      if (0==strncmp(devnam,"dua",3)) 
	ucb = du_iodbunit_vmsinit(ddb,i,&d);
      if (0==strncmp(devnam,"dfa",3)) 
	ucb = du_iodbunit_vmsinit(ddb,i,&d); //was file_...
      if (ucb)
	ucb->ucb$l_ddt=&du$ddt; // maybe bad thing to do?
    }
    break;

  default:
    printk("wrong scs dyn type %x\n",type);
    break;
  }

  if (ddb)
    ddb->ddb$ps_sb=&othersb;

  goto again;

 end:
  //kfree_skb(skb);
}

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
      __u16 len = ntohs(*(__u16 *)skb->data);
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

#ifndef CONFIG_VMS
      if ((skb = skb_share_check(skb, GFP_ATOMIC)) == NULL)
              goto out;

      skb_pull(skb, 2);

      skb_trim(skb, len);
#endif

      msg=skb->data;
      nisca=gettr(msg);
      scs=getppdscs(msg);
      ppd=getppdscs(msg);
      dx=getdx(msg);

      if (scs_from_myself(dx,0xaa00,(system_utsname.nodename[0]<<16)+system_utsname.nodename[1])) {
	//printk("discarding packet from myself (mcast...?)\n");
	goto dump_it;
      }

  printscs(skb->data);

      tr_flag=nisca->nisca$b_tr_flag;
      tr_pad=nisca->nisca$b_tr_pad;

	skb->nh.raw = skb->data;

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
#ifndef CONFIG_VMS
kfree_skb(skb);
#endif
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

/*
 * Fast timer is for delayed acks (200mS max)
 * Slow timer is for everything else (n * 500mS)
 */

#define FAST_INTERVAL (HZ/5)
#define SLOW_INTERVAL (HZ/2)

#ifdef CONFIG_SYSCTL
extern int decnet_dst_gc_interval;

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

#ifndef CONFIG_VMS
		if ((dev = __dev_get_by_name(devname)) == NULL)
			return -ENODEV;
#endif

		if (dev->dn_ptr == NULL)
			return -ENODEV;

		decnet_default_device = dev;
		dn_dev_set_timer(dev);
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

#ifndef CONFIG_VMS
		if ((dev = __dev_get_by_name(devname)) == NULL)
			return -ENODEV;
#endif

		decnet_default_device = dev;
		filp->f_pos += *lenp;

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
	{NET_DECNET_DEFAULT_DEVICE, "default_device", NULL, 16, 0644, NULL,
	dn_def_dev_handler, dn_def_dev_strategy, NULL, NULL, NULL},
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
