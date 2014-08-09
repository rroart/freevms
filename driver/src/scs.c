// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source, 2001-2004. Based on decnet files.

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
#include <linux/net.h>
#include <linux/netdevice.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/rtnetlink.h>
#include <asm/uaccess.h>

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
#include <iodef.h>
#include <mscpdef.h> // does not belong?
#include <nisca.h>
#include <nmadef.h>
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
#include <xmdef.h>
#include <prcpoldef.h>
#include <queue.h>
#include <exe_routines.h>
#include <scs_routines.h>
#include <misc_routines.h>

#include "../../cmuip/ipacp/src/xedrv.h"
#include <starlet.h>

extern struct _pb mypb;
extern struct _sb mysb;
extern struct _pdt mypdt;
extern struct _sb othersb;

extern struct _cdt cdtl[1024];
extern struct _rdt rdt;
extern struct _scs_rd rdtl[128];
extern struct _cdl cdl;

scs_startio ( int dummy );
int scs_rcv2(char * bufh, char * buf);

#define NF_SCS_HELLO              5

#define SCS_IFREQ_SIZE (sizeof(struct ifreq) - sizeof(struct sockaddr) + sizeof(struct sockaddr_dn))

static char scs_rt_all_end_mcast[ETH_ALEN] = {0xAB,0x00,0x04,0x01,0x00,0x00};
static char scs_rt_all_rt_mcast[ETH_ALEN]  = {0xAB,0x00,0x04,0x01,0x00,0x00};
static char scs_hiord[ETH_ALEN]            = {0xAB,0x00,0x04,0x01,0x00,0x00}; // remember to add clusterid + 1
static unsigned char scs_eco_version[3]    = {0x02,0x00,0x00};

struct net_device *scs_default_device = 0;
short scs_chan = 0;

void * getdx(void * buf);
void * getcc(void * buf);
void * gettr(void * buf);
void * getppdscs(void * buf);
void * skb_getdata (void * buf);

void scs_msg_ctl_fill(char * buf, struct _cdt * cdt, unsigned char msgflg);
struct sk_buff *scs_alloc_skb2(struct _cdt *sk, int size, int pri);
static inline void scs_rt_finish_output2(char *buf, int len, char *dst);

#define DRV$MAX_PHYSICAL_BUFSIZE 1600
#define     MAX_RCV_BUF   64

struct  scs_interface_structure
{
  void *     sei$dev_config		;
  unsigned long     sei$io_chan			;
  unsigned long     sei$arp_io_chan		;
  void *     sei$rcvhdrs			;
  void *     sei$recv_qhead		;
  void *     sei$recv_qtail		;
  void *     sei$arp_buffer		;
  void *     sei$arp_block		;
  unsigned short     sei$phy_size		;
  unsigned char     sei$phy_addr		[6];
  unsigned long     sei$restart_time		;
  unsigned short     sei$restart_count		;
  unsigned short     sei$retry_count		;
  unsigned short     sei$max_retry		;
  unsigned short     sei$mpbs			;
  unsigned short     sei$curhdr			;
  union {
    unsigned short     sei$flags			;
    struct {
      unsigned 	sei$need_2_free	 : 1;	// XEshutdown buf free pending
      unsigned 	sei$io_queued	 : 1;	// XE I/O has been started
      unsigned 	sei$xe_decnet	 : 1;	// XE DECNET address seen
      unsigned 	sei$xe_started	 : 1;// XE started at least once
    };
  };
} scs_int_, * scs_int = &scs_int_;

int is_cluster_on() {
  return mypb.pb$w_state==PB$C_OPEN;
}

inline void scs_nsp_send2(char * buf, int len)
{
  unsigned short int *pktlen;
  //skb->dst = dst_clone(dst);
  scs_rt_finish_output2(buf,len,&mypb.pb$b_rstation);
  //  dev_queue_xmit(skb);
}

static void scs_dev_delete(struct net_device *dev);

static void scs_send_brd_hello(struct net_device *dev);

static inline void scs_rt_finish_output2(char * buf , int len, char *dst)
{

  struct XE_iosb_structure IOS_, * IOS = &IOS_; 
  int sts = exe$qiow(4,	scs_int->sei$io_chan,
		     IO$_WRITEVBLK,
		     IOS, 0, 0,
		     buf,
		     len, 0, 0,
		     dst, 0);

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

static void scs_send_endnode_hello(struct net_device *dev)
{
  struct _nisca *intro;
  struct _nisca *nisca, *dx;
  unsigned short int *pktlen;
  void * msg;

  char * buf = kmalloc (256, GFP_KERNEL);
  memset(buf, 0, 256);
  int len = sizeof(*nisca)+16;

  msg = buf;
  intro = msg;

  dx=getdx(msg);

  scs_fill_dx(msg,0xab00,0x04010000,0xaa00,(mysb.sb$t_nodename[0]<<16)+mysb.sb$t_nodename[1]);

  nisca=getcc(msg);

  nisca->nisca$b_msg  = NISCA$C_HELLO;
  nisca->nisca$b_msg  |= NISCA$M_TR_CTL|NISCA$M_TR_CCFLG;
  nisca->nisca$l_maint  = NISCA$M_MAINT;
  if (strlen(&mysb.sb$t_nodename)) {
    nisca->nisca$t_nodename[0]=4; //strlen(system_utsname.nodename);
    memcpy(&nisca->nisca$t_nodename[1],&mysb.sb$t_nodename,4);
    memcpy(&nisca->nisca$ab_lan_hw_addr,dev->dev_addr,6);
	  
  } else {
    nisca->nisca$t_nodename[0]=6;
    memcpy(&nisca->nisca$t_nodename[1],"NODNAM",6);
  }

  scs_rt_finish_output2(buf, len, scs_rt_all_rt_mcast);
}


static void scs_send_brd_hello(struct net_device *dev)
{
	scs_send_endnode_hello(dev);
}

static do_once=0;

static void scs_dev_set_timer();

static void scs_dev_timer_func(unsigned long arg)
{
  struct net_device *dev = (struct net_device *)arg;

  if (dev) scs_send_brd_hello(dev);

  scs_dev_set_timer(dev);
}

static timers=5;

static void scs_dev_set_timer()
{
  struct timer_list * timer=kmalloc(sizeof(struct timer_list),GFP_KERNEL);
  memset(timer,0,sizeof(timer));

  timer->data = (unsigned long)scs_default_device;
  timer->function = scs_dev_timer_func;
  timer->expires = jiffies + (10 * HZ);

  //  add_timer(timer);
  // fix regular timing later
  signed long long time=-1000000000;
  if (timers) {
    exe$setimr (0, &time ,scs_dev_timer_func,scs_default_device,0);
    timers--;
  }
}

void __init scs_dev_init(void)
{
  scs_dev_set_timer();
}

void __exit scs_dev_cleanup(void)
{
}

static int first_hello=0;

/*
 * Endnode hello message received
 */
extern int startconnect(int);

int scs_neigh_endnode_hello(struct sk_buff *skb)
{
	struct _nisca *msg = (long)getcc(skb);

 	if (0==strncmp("NODNAM",&msg->nisca$t_nodename[1],6))
	  return 1;

 	if (0==strncmp(&mysb.sb$t_nodename,&msg->nisca$t_nodename[1],4))
	  return 1;

	if (!first_hello) {
	  first_hello++;
	  printk("scs received hello from node %s\n",&msg->nisca$t_nodename[1]);
	  {
	    signed long long time=-1000000000;
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

	return 0;
}

void scs_msg_ctl_comm(struct _cdt *sk, unsigned char msgflg)
{
	int len=200;
	char * buf = kmalloc (200, GFP_KERNEL);
	memset (buf, 0, 200);

	scs_msg_ctl_fill(buf,sk,msgflg);

	scs_nsp_send2(buf, len);	
}

void scs_nsp_send_conninit2(struct _cdt *sk, unsigned char msgflg, char * rprnam, char * lprnam, char * condat)
{
	struct _nisca *nisca;
	struct _ppd * ppd;
	struct _scs * scs;
	
	int len=200;
	char * buf=kmalloc(200, GFP_KERNEL);
	memset(buf, 0, 200);

	scs_msg_ctl_fill(buf,sk,msgflg);

	scs=getppdscs(buf);

	memset(&scs->scs$t_dst_proc,0,16);
	memset(&scs->scs$t_src_proc,0,16);
	memset(&scs->scs$b_con_dat,0,16);
	memcpy(&scs->scs$t_dst_proc,rprnam,strlen(rprnam));
	memcpy(&scs->scs$t_src_proc,lprnam,strlen(lprnam));
	if (condat) memcpy(&scs->scs$b_con_dat,condat,strlen(condat));

	scs_nsp_send2(buf,len);	
}

void scs_msg_ctl_fill(char * buf, struct _cdt * cdt, unsigned char msgflg)
{
	struct _nisca *nisca;
	struct _ppd * ppd;
	struct _scs * scs;
	struct _nisca *dx;
	void * data;
	data=buf;
	scs_fill_dx(data,0xab00,0x04010000,0xaa00,(mysb.sb$t_nodename[0]<<16)+mysb.sb$t_nodename[1]);
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
	data = (int)skb_getdata(skbdata)/*+14*/;
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


void scs_msg_fill(char * buf, struct _cdt * cdt, unsigned char msgflg, struct _scs * newscs)
{
	struct _nisca *nisca;
	struct _ppd * ppd;
	struct _scs * scs;
	struct _nisca *dx;
	void * data;
	data = buf;

	scs_fill_dx(data,0xab00,0x04010000,0xaa00,(mysb.sb$t_nodename[0]<<16)+mysb.sb$t_nodename[1]);
	nisca=gettr(data);
	nisca->nisca$b_tr_flag=0;
	nisca->nisca$b_tr_pad=0x13;
	nisca->nisca$b_tr_pad_data_len=0x12;

	ppd=getppdscs(data);
	scs=getppdscs(data);

	memcpy(scs,newscs,sizeof(struct _scs));

	//ppd->ppb$b_opc=NISCA$C_MSGREC;
	
	//memcpy(&nisca->nisca$t_nodename,&othersb.sb$t_nodename,8);
}

void scs_msg_fill_more(char * buf,struct _cdt * cdt, struct _cdrp * cdrp, int bufsiz)
{
	struct _nisca *nisca;
	struct _ppd * ppd;
	struct _scs * scs;
	struct _nisca *dx;
	void * data;
	data = buf;

	ppd=getppdscs(data);
	scs=getppdscs(data);

	data = (unsigned long)scs + sizeof(*scs);

	memcpy(data, cdrp->cdrp$l_msg_buf, bufsiz);

}

static unsigned char dn_hiord_addr[6] = {0xAB,0x00,0x04,0x01,0x00,0x00}; // remember to add clusterid + 1

int opc_msgrec(char * buf) {
  struct _cdt *cb;
  struct _cdt *cdt;
  unsigned char flags = 0;
  unsigned char padlen = 0;
  struct _scs * msg=buf;
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
    scs_std$accept(0,0,0,0,0,0,0,0,0,0,0,0,cdt,0,0);
    cdt->cdt$l_condat=kmalloc(16,GFP_KERNEL);
    cdt->cdt$l_lprocnam=kmalloc(16,GFP_KERNEL);
    memcpy(cdt->cdt$l_condat,&scs->scs$b_con_dat,16);
    memcpy(cdt->cdt$l_lprocnam,&scs->scs$t_dst_proc,16);
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
int nisca_snt_dg (char * buf) { 
  void * addr = getppdscs(buf);
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
    int savipl=getipl();
    setipl(0); // still something funny someplace
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
    int savipl=getipl();
    setipl(0); // still something funny someplace
    int savis=current->psl_is;
    current->psl_is=0;
    mscplisten(addr,cdt,0);
    if (savis) current->psl_is=1;
    setipl(savipl);
  }
#endif
  //kfree(buf);
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
  struct _ddb * ddb = 0;
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
    memcpy(devnam,b+1,16);
    b+=16;
    printk("maybe creating remote ddb %s on other node\n",devnam);
    if (0==strncmp(devnam,"dqa",3)) {
      ddb=ide_iodb_vmsinit(1);
      du_iodb_clu_vmsinit(ddb->ddb$ps_ucb);
    }
    if (0==strncmp(devnam,"dua",3)) 
      ddb=du_iodb_vmsinit();
    if (0==strncmp(devnam,"dfa",3)) {
      ddb=du_iodb_vmsinit(); // was file_
      du_iodb_clu_vmsinit(ddb->ddb$ps_ucb);
      ddb->ddb$t_name[2]='f';
    }
    break;

  case DYN$C_UCB:
    num=*b++;
    printk("maybe creating remote ddb %s on other node with %x units\n",devnam,num);
    for (i=0;i<num;i++) {
      devnam[3]=48+i;
      ucb=0;
      if (0==strncmp(devnam,"dqa",3)) 
	ucb = ide_iodbunit_vmsinit(ddb,i,&d);
      if (0==strncmp(devnam,"dqa",3)) 
	((struct _mscp_ucb *)ucb)->ucb$w_mscpunit=ucb->ucb$w_unit;
      if (0==strncmp(devnam,"dqa",3)) 
	printk("UCB MSCPUNIT %x\n",((struct _mscp_ucb *)ucb)->ucb$w_mscpunit);
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
  {
  }
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

void * skb_getdata (void * buf) {
  return ((long)buf)+16;
}

void * getcc(void * buf) {
  unsigned long l=(unsigned long)buf;
  struct _nisca * nisca=(struct _nisca *)(l+16);
  return nisca;
}

void * gettr(void * buf) {
  unsigned long l=(unsigned long)buf;
  struct _nisca * nisca=(struct _nisca *)(l+16);
  return nisca;
}

void * getppdscs(void * buf) {
  unsigned long l=(unsigned long)buf;
  struct _nisca * nisca=(struct _nisca *)(l+16);
  unsigned long tr_flag=nisca->nisca$b_tr_flag;
  unsigned long tr_pad=nisca->nisca$b_tr_pad_data_len;
  unsigned long retadr=(unsigned long)(&nisca->nisca$b_tr_pad_data_len)+tr_pad;
  retadr-=16; // offset related
  return retadr;
}

int scs_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt)
{
      struct _cdt *cb;
      unsigned char flags = 0;
      __u16 len = ntohs(*(__u16 *)skb->data);
      unsigned char padlen = 0;
      struct _nisca * nisca;
      unsigned char tr_flag;
      unsigned char tr_pad;
      unsigned char * msg; 
      struct _ppd *ppd;
      struct _scs *scs;
      struct _nisca *dx;
      int (*func)();

      msg=skb->data;
      nisca=gettr(msg);
      scs=getppdscs(msg);
      ppd=getppdscs(msg);
      dx=getdx(msg);

      if (scs_from_myself(dx,0xaa00,(mysb.sb$t_nodename[0]<<16)+mysb.sb$t_nodename[1])) {
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
	    return NF_HOOK(PF_DECnet, NF_SCS_HELLO, skb, skb->dev, NULL, scs_neigh_endnode_hello);
	  }
	}

	if (nisca->nisca$b_msg & NISCA$M_TR_CTL) {
	    return NF_HOOK(PF_DECnet, NF_SCS_HELLO, skb, skb->dev, NULL, opc_msgrec);
	}

	if (scs->scs$w_mtype>0x17)
	  panic("scs$w_mtype too large\n");
#if 0
	func=nisca_dispatch[scs->scs$w_mtype];
	return func(skb,scs);
#endif
	//	nisca_snt_dg(skb,scs);
	return NF_HOOK(PF_DECnet, NF_SCS_HELLO, skb, skb->dev, NULL, nisca_snt_dg);
dump_it:
out:
return NET_RX_DROP;
}

int do_opc_dispatch(struct sk_buff *skb)
{
  int (*func)();
  struct _ppd * ppd=getppdscs(skb);
  if (ppd->ppd$b_opc>0x40)
    panic("ppd$b_opc too large\n");
  func=opc_dispatch[ppd->ppd$b_opc];
  return func(skb,ppd);
}

//extern signed long XE_BROADCAST[];
signed long XE_BROADCAST[3]= { -1 , -1 , -1}; // check

scs_startdev ( scs_int2 , setflag , setaddr)

// This routine initializes the ethernet device to receive SCS packets.
// Issue start command to the controller.
// Accepts:
//   XE_INT	EtherNet interface info block
//   SETFLAG	TRUE if device physical address needs to be set
//   SETADDR	If SETFLAG, then physical address to set
// Returns:
//   0 (false) on failure, device not started
//  -1 (true) on success, device ready, reads queued

	struct scs_interface_structure * scs_int2;
    {
    signed long
	RC,
	plen;
	struct XE_iosb_structure IOS_ , * IOS = &IOS_;
	struct XE_setup_structure Setup_, * Setup= &Setup_;
	struct XE_sdesc_structure Paramdescr_, * Paramdescr= &Paramdescr_;

	memset(Setup,0,sizeof(struct XE_setup_structure));

	scs_int->sei$rcvhdrs = kmalloc(MAX_RCV_BUF*16, GFP_KERNEL);
	memset(scs_int->sei$rcvhdrs, 0, MAX_RCV_BUF*16);
	qhead_init(& scs_int->sei$recv_qhead );

// Build the nasty setup block required by the ethernet device

    Setup->XE$c_pcli_bus      = NMA$C_PCLI_BUS;
    Setup->XE$l_buffer_length = 1500; // was: DRV$MAX_PHYSICAL_BUFSIZE;
    Setup->XE$c_pcli_bfn      = NMA$C_PCLI_BFN;
    Setup->XE$l_number_buff   = 1500; // was: MAX_RCV_BUF;
    Setup->XE$c_pcli_pad      = NMA$C_PCLI_PAD;
    Setup->XE$l_padding       = NMA$C_STATE_OFF;
    Setup->XE$c_pcli_pty      = NMA$C_PCLI_PTY;
    Setup->XE$c_pcli_prm      = NMA$C_PCLI_PRM;
    Setup->XE$l_promiscuous   = NMA$C_STATE_OFF;
    Setup->XE$c_pcli_dch      = NMA$C_PCLI_DCH;
    Setup->XE$l_data_chain    = NMA$C_STATE_OFF;
    Setup->XE$c_pcli_con      = NMA$C_PCLI_CON;
    Setup->XE$l_control_mode  = NMA$C_LINCN_NOR;


// Change XE$W_mca_mode to NMA$C_LINMC_SET if you want IP broadcasts.

    Setup ->XE$c_pcli_mca     = NMA$C_PCLI_MCA;
    Setup ->XE$w_mca_length   = 2 + XE_ADR_SIZE;
    Setup ->xe$w_mca_mode     = NMA$C_LINMC_CLR;
    CH$MOVE (XE_ADR_SIZE, XE_BROADCAST, Setup ->XE$L_mca_address);

// If he wants us to set the physical address, then do so.

    if (setflag)
	{
	Setup->XE$c_pcli_pha = NMA$C_PCLI_PHA;
	Setup->XE$w_pcli_phlen = XE_ADR_SIZE+2;
	Setup->XE$w_pcli_phmode = NMA$C_LINMC_SET;
	CH$MOVE(XE_ADR_SIZE,CH$PTR(setaddr),CH$PTR(Setup->XE$l_pcli_phaddr));
	plen = (long)Setup->xe$setup_pha_end - (long)Setup;
	}
    else
	plen = (long)Setup->xe$setup_end - (long)Setup;

// Set up for SCS protocol on this channel

    Setup->XE$l_protocol      = ETH_P_MYSCS ;
    swapbytes(1,&Setup->XE$l_protocol);

    Paramdescr->xe$setup_length = plen;
    Paramdescr->xe$setup_address = Setup;

    printk("H2 %s %x\n",&mysb.sb$t_hwtype,&mysb.sb$t_hwtype);
    $DESCRIPTOR(dev_desc, mysb.sb$t_hwtype);

    // Assign Ethernet Controller
    if (! (RC=exe$assign (&dev_desc, &scs_int->sei$io_chan, 0, 0, 0)))
         // Ethernet controller assign failed
	{
	printk("SCS $ASSIGN failure (dev=%s), EC = %x\n",
		    dev_desc.dsc$a_pointer,RC);
	return;
	};

// Issue the startup command to controller

    RC = exe$qiow (1, scs_int->sei$io_chan,
		IO$_SETMODE+IO$M_CTRL+IO$M_STARTUP,IOS,0,0,0,Paramdescr,0,0,0,0);
    if (!( (RC == SS$_NORMAL) && (IOS->xe$vms_code == SS$_NORMAL) ))
	{
	if (IOS->xe$vms_code == SS$_BADPARAM)
	   printk("SCS startup failure, RC=%x,VMS_code=%x,Param=%x",
		  RC,IOS->xe$vms_code,((long *)IOS)[1]);
	else
	   printk("SCS startup failure, RC=%x,VMS_code=%x,Xfer size=%x",
	       RC,IOS->xe$vms_code,IOS->xe$tran_size);
	return 0;
	};

    scs_startio(0);

// Everything OK - return TRUE value

    return -1;
    }

struct scs_rcv_qb_structure
{
  void *     scs_rcv$next		;
  void *     scs_rcv$last		;
  unsigned short int     scs_rcv$vms_code	;
  unsigned short int     scs_rcv$tran_size	;
  unsigned short     scs_rcv$cmd_status	;
  unsigned char     scs_rcv$error_summary	;
  unsigned char     scs_rcv$iosb_unused2	;
  unsigned char     scs_rcv$data		;
};

void scs_receive ( int i);

scs_startio ( int dummy )
{
  signed long i, sts;
  struct scs_rcv_qb_structure * buff;
  struct xe_arp_structure * ARbuf;
  struct XERCV_structure * rcvhdrs = scs_int->sei$rcvhdrs ;
  short XE_chan = scs_int->sei$io_chan;
  short XAR_chan = scs_int->sei$arp_io_chan;

  scs_int->sei$curhdr = 0;
  for (i=0;i<=(MAX_RCV_BUF-1);i++)
    {	// Get buffer, put on Q and issue IO$_READVBLK function
      buff = kmalloc(DRV$MAX_PHYSICAL_BUFSIZE+(Qhead_len+IOS_len),GFP_KERNEL);
      memset(buff, 0, DRV$MAX_PHYSICAL_BUFSIZE+(Qhead_len+IOS_len));
      INSQUE ( buff , scs_int-> sei$recv_qtail  );
      buff = buff + XE_hdr_offset;
      sts = sys$qio(2,scs_int->sei$io_chan,IO$_READVBLK,&buff->scs_rcv$vms_code,
		   scs_receive,  scs_int,
		   &buff->scs_rcv$data,
		   DRV$MAX_PHYSICAL_BUFSIZE,
		   0, 0,
		   rcvhdrs[i].XERCV$buf,0);
    }
      return -1;
}

void scs_receive ( int i)
{
  struct scs_rcv_qb_structure * Rbuf;
  struct scs_rcv_qb_structure * NRbuf;
  //  Device_Configuration_Entry * dev_config;
  signed long
    rcvix,
    RC,
    IRC,
    Error_Flag  = 0;

  struct XERCV_structure * rcvhdrs = scs_int->sei$rcvhdrs ;

  // Get first input packet off of the queue
  //!!HACK!!// What if the first packet wasn't the one which $QIO returned?
  // not yet REMQUE(scs_int->sei$recv_qhead,&Rbuf);
  struct scs_rcv_qb_structure * head=&scs_int->sei$recv_qhead, *tmp=head->scs_rcv$next;
  while (tmp!=head) {
    if (tmp->scs_rcv$vms_code)
      break;
    tmp=tmp->scs_rcv$next;
  }
  REMQUE(tmp,&Rbuf);
  Rbuf = Rbuf + XE_hdr_offset;
  rcvix = scs_int->sei$curhdr;

  if ((RC = Rbuf->scs_rcv$vms_code) != SS$_NORMAL)
    {
      //	Error_Flag = 1;
      switch (Rbuf->scs_rcv$vms_code)
	{
	case SS$_ABORT:
#if 0
	  DRV$OPR_FAO("SCS abort, cmd_status=!XW, error_summary=!XB",
		      Rbuf->scs_rcv$cmd_status,Rbuf->scs_rcv$error_summary);
#endif
	  break;

	case SS$_DATAOVERUN: case SS$_TIMEOUT:
	  {
#if 0
	    DRV$OPR_FAO("SCS read error (timeout/overrun), RC=!XL",RC);
	    DRV$OPR_FAO("DEBUG - bytes received=!XL",Rbuf->scs_rcv$tran_size);
#endif
	  };
	  break;

	default :
	  {}
#if 0
	  DRV$OPR_FAO("SCS: VMS Error, cmd_status=!XW, error_summary=!XB",
		      Rbuf->scs_rcv$cmd_status,Rbuf->scs_rcv$error_summary);
#endif
	}
    };

  //Ignore the timeout bit for DEQNA's
  if ((Rbuf->scs_rcv$cmd_status  & 0x0FF00) != XM$M_STS_ACTIVE)
    {
      Error_Flag = 1;
      // Error from board
#if 0
      DRV$OPR_FAO("SCS status error.  Status = !XL",
		  Rbuf->scs_rcv$cmd_status & 0x0FF00);
#endif
    };

  if (Error_Flag)
    {
      if (scs_int->sei$retry_count >= scs_int->sei$max_retry)
#if 0
	XE$ERR(scs_int,"SCS retried !ZB times.",scs_int->sei$retry_count);
#else
      { } 
#endif
      else
	{
	  // Stuff this packet back on the queue for deallocation
	  Rbuf = Rbuf - XE_hdr_offset;
	  INSQUE(Rbuf, scs_int->sei$recv_qtail);
	  scs_int->sei$retry_count = scs_int->sei$retry_count+1;
	  return;
	}
    }
  else
    scs_int->sei$retry_count = 0;

  // Get another buffer and put it on the receive Q for this device

  scs_int->sei$curhdr = scs_int->sei$curhdr+1;
  if (scs_int->sei$curhdr > (MAX_RCV_BUF-1))
    scs_int->sei$curhdr = 0;

  NRbuf = kmalloc(DRV$MAX_PHYSICAL_BUFSIZE+(Qhead_len+IOS_len), GFP_KERNEL);
  memset(NRbuf, 0, DRV$MAX_PHYSICAL_BUFSIZE+(Qhead_len+IOS_len));
  INSQUE(NRbuf,scs_int->sei$recv_qtail);
  NRbuf = NRbuf + XE_hdr_offset;
  RC = sys$qio(2,scs_int->sei$io_chan,
	       IO$_READVBLK,
	       &NRbuf->scs_rcv$vms_code, scs_receive, scs_int,
	       &NRbuf->scs_rcv$data,
	       DRV$MAX_PHYSICAL_BUFSIZE,
	       0,0,
	       rcvhdrs[scs_int->sei$curhdr].XERCV$buf,0);

  // Check for recoverable error. This hack is necessary because the DEQNA
  // is known to get wedged, and the driver gives back SS$_DEVINACT when this
  // happens.

  if (BLISSIFNOT(RC))
    {
      if (RC == SS$_DEVINACT)
	{
	  // Stuff the packet back on the queue for deallocation
	  Rbuf = Rbuf - XE_hdr_offset;
	  INSQUE(Rbuf, scs_int->sei$recv_qtail);
#if 0
	  XE$ERR(scs_int,"SCS $QIO read error (dev_inact), RC=!XL",RC);
#endif
	}
#if 0
      else
	DRV$FATAL_FAO("Ethernet $QIO queue read error, EC = !XL",RC);
#endif
    }
  else
    {

      char * buf = &Rbuf->scs_rcv$data;
      char * bufh = rcvhdrs[rcvix].XERCV$buf;

      scs_rcv2(bufh, buf);
#if 0
      // extend scs_rcv2?
      drv$ip_receive(Rbuf-XE_hdr_offset,
		     DRV$MAX_PHYSICAL_BUFSIZE+(Qhead_len+IOS_len),
		     &Rbuf->scs_rcv$data,Rbuf->scs_rcv$tran_size,dev_config);
#endif
    };

}

int scs_rcv2(char * bufh, char * buf)
{
  struct _cdt *cb;
  unsigned char flags = 0;
#if 0
  __u16 len = ntohs(*(__u16 *)skb->data);
#endif
  unsigned char padlen = 0;
  struct _nisca * nisca;
  unsigned char tr_flag;
  unsigned char tr_pad;
  unsigned char * msg; 
  struct _ppd *ppd;
  struct _scs *scs;
  struct _nisca *dx;
  int (*func)();

  msg=buf;
  nisca=(long)gettr(msg);
  scs=(long)getppdscs(msg);
  ppd=(long)getppdscs(msg);
  dx=(long)getdx(msg);

  if (scs_from_myself(dx,0xaa00,(mysb.sb$t_nodename[0]<<16)+mysb.sb$t_nodename[1])) {
    //printk("discarding packet from myself (mcast...?)\n");
    goto dump_it;
  }

  printscs(buf);

  tr_flag=nisca->nisca$b_tr_flag;
  tr_pad=nisca->nisca$b_tr_pad;

  if (nisca->nisca$b_msg & NISCA$M_TR_CCFLG) {
    switch((nisca->nisca$b_msg)&0x1f) {
    case NISCA$C_HELLO:
      return NF_HOOK(PF_DECnet, NF_SCS_HELLO, buf, 0, NULL, scs_neigh_endnode_hello);
    }
  }

  if (nisca->nisca$b_msg & NISCA$M_TR_CTL) {
    return NF_HOOK(PF_DECnet, NF_SCS_HELLO, buf, 0, NULL, opc_msgrec);
  }

  if (scs->scs$w_mtype>0x17)
    panic("scs$w_mtype too large\n");
#if 0
  func=nisca_dispatch[scs->scs$w_mtype];
  return func(skb,scs);
#endif
  //	nisca_snt_dg(skb,scs);
  return NF_HOOK(PF_DECnet, NF_SCS_HELLO, buf, 0, NULL, nisca_snt_dg);
 dump_it:
 out:
  return NET_RX_DROP;
}
