// $Id$
// $Locker$

// Author. Roar Thronæs.
// Author. Linux people.

#if 1
#define ETH_P_MYSCS 0x6009

//#if 0
#include <linux/config.h>
#define CONFIG_DECNET
#include <linux/module.h>
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
#include <linux/netfilter.h>
#include <net/sock.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <asm/ioctls.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <net/neighbour.h>
#include <net/dst.h>
#if 0
#include <net/dn.h>
#include <net/dn_nsp.h>
#include <net/dn_dev.h>
#include <net/dn_route.h>
#include <net/dn_fib.h>
#include <net/dn_neigh.h>
#endif
#include<linux/vmalloc.h>
#include<sys/errno.h>

#include <cdldef.h>
#include <cdrpdef.h>
#include <cdtdef.h>
#include <chdef.h>
#include <cwpsdef.h>
#include <ddbdef.h>
#include <ddtdef.h>
#include <dptdef.h>
#include <dyndef.h>
#include <fdtdef.h>
#include <iodef.h>
#include <iosbdef.h>
#include <mscpdef.h> // does not belong here
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

struct _pb mypb;
struct _sb mysb;
struct _pb otherpb;
struct _sb othersb;

/*
 * decnet_address is kept in network order, decnet_ether_address is kept
 * as a string of bytes.
 */
dn_address decnet_address = 0;
unsigned char decnet_ether_address[ETH_ALEN] = { 0xAA, 0x00, 0x04, 0x00, 0x00, 0x00 };

#define DN_SK_HASH_SHIFT 8
#define DN_SK_HASH_SIZE (1 << DN_SK_HASH_SHIFT)
#define DN_SK_HASH_MASK (DN_SK_HASH_SIZE - 1)

static struct proto_ops dn_proto_ops;
rwlock_t dn_hash_lock = RW_LOCK_UNLOCKED;
static struct _cdt *dn_sk_hash[DN_SK_HASH_SIZE];

struct _cdt *dn_sklist_find_listener2(char *addr)
{
  struct _cdt **skp = (addr);
  struct _cdt *sk;
  struct _sbnb * sbnb = scs$gq_local_names;
  struct _sbnb * tmp = sbnb->sbnb$l_flink;

  while (tmp!=sbnb) {
    if (0==strncmp(&tmp->sbnb$b_procnam,addr,16)) goto out;
    tmp = tmp -> sbnb$l_flink;
  }
 out:
  if (tmp==sbnb)
    return 0;
  return tmp;
}

struct _cdt *dn_find_by_skb2(struct sk_buff *skb)
{
	struct _cdt *cb = (skb);
	struct _cdt *sk=0;
	struct _cdt *scp;

	read_lock(&dn_hash_lock);
	read_unlock(&dn_hash_lock);

	return sk;
}

int dn_nsp_backlog_rcv2(struct _cdt *sk, struct sk_buff *skb) { }

struct _cdt *dn_alloc_sock(struct socket *sock, int gfp)
{
	struct _cdt *sk;
	struct _cdt *scp;

	if  ((sk = sk_alloc(PF_DECnet, gfp, 1)) == NULL) 
		goto no_sock;

	if (sock) {
			sock->ops = &dn_proto_ops;
	}
	sock_init_data(sock,sk);
	scp = sk;

	sk->cdt$l_dginput = dn_nsp_backlog_rcv2;

	skb_queue_head_init(&scp->cdt$l_waitqfl);
	skb_queue_head_init(&scp->cdt$l_waitqfl);
	skb_queue_head_init(&scp->cdt$l_share_flink);

	init_timer(0);
	dn_start_slow_timer2(sk);

	MOD_INC_USE_COUNT;

	return sk;
no_sock:
	return NULL;
}

// whoa...
int scs_std$connect (void (*msgadr)(), void (*dgadr)(), void (*erradr)(), void *rsysid, void *rstadr, void *rprnam, void *lprnam, int initcr, int minscr, int initdg, int blkpri, void *condat, void *auxstr, void (*badrsp)(), void (*movadr)(), int load_rating,int (*req_fast_recvmsg)(), void (*fast_recvmsg_pm)(), void (*change_aff)(), void (*complete)(), int connect_parameter)
{
	struct _cdt *cdt = find_free_cdt();
	struct _cdt *sock = cdt;
	struct _cdt *sk = sock;
	struct _cdt *scp = sk;
	int err = -EISCONN;

	struct _sbnb *s = scs_register_name(lprnam,"");
	s->sbnb$w_local_index=cdt->cdt$l_lconid;
	cdt->cdt$w_state = CDT$C_CLOSED;

	sk->cdt$l_msginput=msgadr;
	sk->cdt$l_dginput=dgadr;

	sk->cdt$w_state   = CDT$C_CON_SENT;

	dn_nsp_send_conninit2(sk, SCS$C_CON_REQ,rprnam,lprnam,condat);

#if 0
	for( ; ; ) {
	  volatile state=sk->cdt$w_state;
	  if (state==CDT$C_OPEN) goto out2;
	}
#endif

 out2:

	if (auxstr) *((unsigned long **)auxstr)=cdt; // wrong, but temporary

	err = 0;
out:
        return err;
}

//static int dn_accept(struct _cdt *sock, struct _cdt *newsock, int flags)

int scs_std$accept (void (*msgadr)(), void (*dgadr)(), void (*erradr)(), void *rsysid, void *rstadr, void *rprnam, void *lprnam, int initcr, int minscr, int initdg, int blkpri, void *condat, void *auxstr, void (*badrsp)(), void (*movadr)(), int load_rating,int (*req_fast_recvmsg)(), void (*fast_recvmsg_pm)(), void (*change_aff)(), void (*complete)(), struct _cdt * cdt, int connect_parameter)
{
	struct _cdt *sk = cdt, *newsk, *newsock;
	struct sk_buff *skb = NULL;
	struct _cdt *cb;
	unsigned char menuver;
	int err = 0;
	unsigned char type;
	int flags;
	
        return err;
}

//static struct _cdt * cdl$l_freecdt;
extern struct _cdt cdtl[1024];
struct _rdt rdt;
extern struct _scs_rd rdtl[128];
struct _cdl cdl;

extern int du_msg();

void * find_mscp_cdt(void) {
  /* remember to fix cdldef */
  int i;
  //  return &cdtl[5]; // gross gross hack
  for (i=0; i<100; i++) {
    if (cdtl[i].cdt$l_rconid && cdtl[i].cdt$l_msginput==du_msg) return &cdtl[i];
    //if (cdtl[i].cdt$l_rconid) return &cdtl[i];
  }
  return 0;
}

void * find_a_cdt(int a) {
  /* remember to fix cdldef */
  int i;
  for (i=0; i<100; i++) {
    if (cdtl[i].cdt$l_rconid && cdtl[i].cdt$l_msginput==a) return &cdtl[i];
    //if (cdtl[i].cdt$l_rconid) return &cdtl[i];
  }
  return 0;
}

void * find_free_cdt(void) {
  /* remember to fix cdldef */
  struct _cdt * c=cdl.cdl$l_freecdt;
  cdl.cdl$l_freecdt=((struct _cdt *)cdl.cdl$l_freecdt)->cdt$l_link;
  c->cdt$l_rconid=0;
  qhead_init(&c->cdt$l_waitqfl);
  qhead_init(&c->cdt$l_crwaitqfl);
  return c;
}

int scs_std$alloc_rspid(struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp * c,  void (*ravail_p)()) {
  /* remember to fix rdtdef */
  struct _scs_rd * r=rdt.rdt$l_freerd;
  rdt.rdt$l_freerd=r->rd$l_link;
  r->rd$w_state=1;
  r->rd$l_cdrp=c;
  return r->rd$w_seqnum;
}

int   scs_std$find_rdte ( int rspid, struct _scs_rd **rdte_p ) {
  int i;
  struct _scs_rd * rd=&rdtl[rspid];
  *rdte_p=0;
  if (rspid>100) return 0;
  if (rd->rd$w_seqnum != rspid) return 0;
  //if (rd->rd$v_busy) return 0;
  *rdte_p=rd;
  return 1;
}

int   scs_std$deall_rspid (struct _cdrp *cdrp_p) {
  int seq=cdrp_p->cdrp$l_rspid;
  struct _scs_rd * rd;
  if (scs_std$find_rdte(seq,&rd)) {
    //rd->rd$w_seqnum++;
    rd->rd$w_state=0;
    rd->rd$l_link=rdt.rdt$l_freerd;
    rdt.rdt$l_freerd=rd;
    return 1;
  } else {
    return 0;
  }
}

void * scs_register_name(char * c1, char * c2) {
  struct _sbnb * s=vmalloc(sizeof(struct _sbnb));
  bzero(s,sizeof(struct _sbnb));

  bcopy(c1,&s->sbnb$b_procnam,min(16,strlen(c1)));

  insque(s,scs$gq_local_names);

  return s;
}

void * scs_find_name(char * c1) {
  struct _sbnb * head = &scs$gq_local_names;
  struct _sbnb * tmp = head->sbnb$l_flink;
  while(tmp!=head) {
    if (strcmp(c1,&tmp->sbnb$b_procnam)==0) return tmp;
    tmp=tmp->sbnb$l_flink;
  }
  return 0;
}

//static int scs$listen(struct _cdt *sock, int backlog)
//sock is about-ish lprnam;

int scs_std$listen (void (*msgadr)(void *msg_buf, struct _cdt **cdt, struct _pdt *pdt ),
		    void (*erradr)( unsigned int err_status, unsigned int reason, struct _cdt *cdt, struct _pdt *pdt), 
		    void *lprnam, 
		    void *prinfo,
		struct _cdt **cdt)
{
  int err = -EINVAL;

  struct _cdt *c = find_free_cdt();
  struct _sbnb *s;
  //

  c->cdt$l_msginput=msgadr;
  c->cdt$l_erraddr=erradr;

  s=scs_register_name(lprnam,prinfo);
  s->sbnb$w_local_index=c->cdt$l_lconid;
  c->cdt$w_state = CDT$C_LISTEN;

  if (cdt && *cdt) *cdt=c;
  return SS$_NORMAL;
}

void mydirerr() {
}

int dir_listen(void * packet, struct _cdt * c, struct _pdt * p) {

}

extern void cf_listen(void * packet, struct _cdt * c, struct _pdt * p);

void cf_myerr() {}

int /*__init*/ scs_init(void) {
  int i;
  char myname[]="scs$directory";
  char myinfo[]="directory srv";

  struct file * file;
  unsigned long long pos=0;

  bzero(cdtl,sizeof(cdtl));
  bzero(rdtl,sizeof(rdtl));

  scs$gl_cdl=&cdl;
  //cdl$l_freecdt=&cdtl;

  scs$gl_rdt=&rdt;

  cdl.cdl$w_maxconidx=scs$gw_cdtcnt-1;
  cdl.cdl$l_freecdt=&cdtl[0];
  cdl.cdl$b_type=DYN$C_SCS;
  cdl.cdl$b_subtyp=DYN$C_SCS_CDL;
  cdl.cdl$l_nocdt_cnt=0;

  for(i=0;i<scs$gw_cdtcnt-1;i++) {
    cdtl[i].cdt$b_type=DYN$C_SCS;
    cdtl[i].cdt$b_subtyp=DYN$C_SCS_CDT;
    cdtl[i].cdt$l_link=&cdtl[i+1];
    cdtl[i].cdt$l_lconid=i;
    cdtl[i].cdt$l_rconid=i;
    cdtl[i].cdt$w_state=0;
    qhead_init(&cdtl[i].cdt$l_waitqfl);
    qhead_init(&cdtl[i].cdt$l_crwaitqfl);
  }

  qhead_init(&rdt.rdt$l_waitfl);
  rdt.rdt$b_type=DYN$C_SCS;
  rdt.rdt$b_subtyp=DYN$C_SCS_RDT;
  rdt.rdt$l_freerd=&rdtl[0];
  rdt.rdt$l_qrdt_cnt=0;
  rdt.rdt$l_maxrdidx=scs$gw_rdtcnt;
  for(i=0;i<scs$gw_rdtcnt-1;i++) {
    struct _scs_rd * r=&rdtl[i];
    r->rd$l_link=&rdtl[i+1];
    r->rd$w_state=0;
    r->rd$w_seqnum=i; // ?
  }

  scs_std$listen(dir_listen,mydirerr,myname,myinfo,0);

  scs_std$listen(cf_listen,cf_myerr,"configure","hw conf",0); 

  file = filp_open("/vms$common/sysexe/params.dat",O_RDONLY,0);
  if (!IS_ERR(file)) {
    char * c, *b, *n;
    char buf[1024];
    int size=generic_file_read(file,buf,1024,&pos);
    int i=0;
    b=buf;
    while (b<(buf+size)) {
      c=strchr(b,'=');
      n=strchr(b,'\n');
      if (0==strncmp(b, "SCSNODE", c-b)) {
	memcpy(&mysb.sb$t_nodename, c+1, n-c-1);
	goto end;
      }
      b=n+1;
    }
    memcpy(&mysb.sb$t_nodename, "NONAME", 6);
  end:
    filp_close(file,0);
  }

}

//static int dn_sendmsg(struct _cdt *sock, struct msghdr *msg, int size,struct scm_cookie *scm)

int scs_std$senddg(int disposition_flag, int dg_msg_length, struct _cdrp *cdrp ) {
  struct _scs * scs = vmalloc(sizeof(struct _scs));
  struct _scs1 * scs1 = scs;
  struct _ppd * ppd = scs;
  struct _cdt * cdt = cdrp->cdrp$l_cdt;
  ppd->ppd$b_opc=PPD$C_SNDDG;
  scs->scs$w_mtype=SCS$C_APPL_DG;
  scs1->scs$l_lconid=cdt->cdt$l_lconid;
  scs->scs$l_src_conid=cdt->cdt$l_lconid;
  scs->scs$l_dst_conid=cdt->cdt$l_rconid;
  scs1->scs$l_rspid=cdrp->cdrp$l_rspid;
  scs1->scs$l_xct_len=cdrp->cdrp$l_xct_len;
  scs_lower_level_send(cdrp,scs);
}

void scs_lower_level_send(struct _cdrp * cdrp, struct _scs * scs) {
  struct sk_buff *skb = NULL;
  struct _cdt * sk=cdrp->cdrp$l_cdt;

  if ((skb = dn_alloc_skb2(sk, 1000, GFP_ATOMIC)) == NULL)
    return;

  scs_msg_fill(skb,sk,0,scs);

  scs_msg_fill_more(skb,sk,cdrp,600);

  printscs(skb->data);

  dn_nsp_send2(skb);	
}

static int scs_std$sendmsg(int msg_buf_len, struct _pdt *pdt_p, struct _cdrp *cdrp_p, void (* complete)(void))
{
	struct _cdt *sk = cdrp_p->cdrp$l_cdt;
	struct _cdt *scp = sk;
	struct msghdr *msg;
	struct _cdt *sock;
	int size;
	int mss;
	struct sk_buff_head *queue = &scp->cdt$l_waitqfl;
	int flags = msg->msg_flags;
	int err = 0;
	int sent = 0;
	int addr_len = msg->msg_namelen;
	struct sockaddr_dn *addr = (struct sockaddr_dn *)msg->msg_name;
	struct sk_buff *skb = NULL;
	struct _cdt *cb;
	unsigned char msgflg;
	unsigned char *ptr;
	unsigned short ack;
	int len;
	unsigned char fctype;

	return sent ? sent : err;
}

static int dn_device_event(struct notifier_block *this, unsigned long event,
			void *ptr)
{
	struct net_device *dev = (struct net_device *)ptr;

	switch(event) {
		case NETDEV_UP:
			dn_dev_up(dev);
			break;
		case NETDEV_DOWN:
			dn_dev_down(dev);
			break;
		default:
			break;
	}

	return NOTIFY_DONE;
}

static struct notifier_block dn_dev_notifier = {
	notifier_call:	dn_device_event,
};

extern int dn_route_rcv(struct sk_buff *, struct net_device *, struct packet_type *);

static struct packet_type dn_dix_packet_type = {
	type:		__constant_htons(ETH_P_MYSCS),
	dev:		NULL,		/* All devices */
	func:		dn_route_rcv,
	data:		(void*)1,
};

#define IS_NOT_PRINTABLE(x) ((x) < 32 || (x) > 126)

static void dn_printable_object(struct sockaddr_dn *dn, unsigned char *buf)
{
	int i;
    
	switch (dn_ntohs(dn->sdn_objnamel)) {
		case 0:
			sprintf(buf, "%d", dn->sdn_objnum);
			break;
		default:
			for (i = 0; i < dn_ntohs(dn->sdn_objnamel); i++) {
				buf[i] = dn->sdn_objname[i];
				if (IS_NOT_PRINTABLE(buf[i]))
					buf[i] = '.';
			}
			buf[i] = 0;
    	}
}

char *dn_addr2asc(dn_address addr, char *buf)
{
  unsigned short node, area;

  node = addr & 0x03ff;
  area = addr >> 10;
  sprintf(buf, "%hd.%hd", area, node);

  return buf;
}

static char *dn_state2asc(unsigned char state)
{
  switch(state) {
  case CDT$C_OPEN:
    return "OPEN";
  case CDT$C_CON_REC:
    return "CORE";
  case CDT$C_REJ_SENT:
    return "RESE";
  case CDT$C_CON_ACK:
    return "COAC";
  case CDT$C_VC_FAIL:
    return "VCFA";
  case CDT$C_CON_SENT:
    return "COSE";
  case CDT$C_DISC_SENT:
    return "DISE";
  case CDT$C_DISC_ACK:
    return "DIAC";
  case CDT$C_CLOSED:
    return "CLOS";
  case CDT$C_DISC_REC:
    return "DIRE";
  }

  return "????";
}


static int dn_get_info(char *buffer, char **start, off_t offset, int length)
{
	struct _cdt *sk;
	struct _cdt *scp;
	int len = 0;
	off_t pos = 0;
	off_t begin = 0;
	char buf1[DN_ASCBUF_LEN];
	char buf2[DN_ASCBUF_LEN];
	char local_object[DN_MAXOBJL+3];
	char remote_object[DN_MAXOBJL+3];
	int i;

	len += sprintf(buffer + len, "Local                                              Remote\n");

	read_lock(&dn_hash_lock);
	for(i = 0; i < DN_SK_HASH_SIZE; i++) {
		for(sk = dn_sk_hash[i]; sk != NULL; sk = sk->cdt$l_reserved3) {
			scp = sk;

			dn_printable_object(&scp->cdt$l_lconid, local_object);
			dn_printable_object(&scp->cdt$l_rconid, remote_object);

			len += sprintf(buffer + len,
					"%6s/%04X %04d:%04d %04d:%04d %01d %-16s %6s/%04X %04d:%04d %04d:%04d %01d %-16s %4s %s\n",
					dn_addr2asc(dn_ntohs(dn_saddr2dn(&scp->cdt$l_lconid)), buf1),
					scp->cdt$l_lconid,
					scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr,
					scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr,
					scp->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_rmt_chan_seq_num,
					scp->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_rmt_chan_seq_num,
				       0,
					local_object,
					dn_addr2asc(dn_ntohs(dn_saddr2dn(&scp->cdt$l_rconid)), buf2),
					scp->cdt$l_rconid,
					scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr,
					scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr,
					scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr,
					scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr,
				       0,
					remote_object,
					dn_state2asc(scp->cdt$w_state),
					"");

			pos = begin + len;
			if (pos < offset) {
				len = 0;
				begin = pos;
			}
			if (pos > (offset + length))
				break;
		}
	}
	read_unlock(&dn_hash_lock);

	*start = buffer + (offset - begin);
	len -= (offset - begin);

	if (len > length)
		len = length;

	return len;
}


static struct net_proto_family	dn_family_ops = {
	family:		AF_DECnet,
	create:		0,
};

static void none(void) { }

static struct proto_ops dn_proto_ops = {
	family:		AF_DECnet,

	release:	none,
	bind:		none,
	connect:	none,
	socketpair:	sock_no_socketpair,
	accept:		scs_std$accept,
	getname:	none,
	poll:		none,
	ioctl:		none,
	listen:		scs_std$listen,
	shutdown:	none,
	setsockopt:	none,
	getsockopt:	none,
	sendmsg:	scs_std$sendmsg,
	recvmsg:	none,
	mmap:		sock_no_mmap,
	sendpage:	sock_no_sendpage,
};

#ifdef CONFIG_SYSCTL
void dn_register_sysctl(void);
void dn_unregister_sysctl(void);
#endif


#ifdef MODULE
EXPORT_NO_SYMBOLS;
MODULE_DESCRIPTION("The Linux DECnet Network Protocol");
MODULE_AUTHOR("Linux DECnet Project Team");
MODULE_LICENSE("GPL");

static int addr[2] = {0, 0};

MODULE_PARM(addr, "2i");
MODULE_PARM_DESC(addr, "The DECnet address of this machine: area,node");
#endif

static char banner[] __initdata = KERN_INFO "%%KERNEL-I-STARTUP, NET5: MYSCS, based on DECnet for Linux: V.2.4.15-pre5s (C) 1995-2001 Linux DECnet Project Team\n";

static int __init decnet_init(void)
{
#ifdef MODULE
	if (addr[0] > 63 || addr[0] < 0) {
		printk(KERN_ERR "DECnet: Area must be between 0 and 63");
		return 1;
	}

	if (addr[1] > 1023 || addr[1] < 0) {
		printk(KERN_ERR "DECnet: Node must be between 0 and 1023");
		return 1;
	}

	decnet_address = dn_htons((addr[0] << 10) | addr[1]);
	dn_dn2eth(decnet_ether_address, dn_ntohs(decnet_address));
#endif

        printk(banner);

	sock_register(&dn_family_ops);
	dev_add_pack(&dn_dix_packet_type);
	register_netdevice_notifier(&dn_dev_notifier);

	proc_net_create("myscs", 0, dn_get_info);

	dn_neigh_init();
	dn_dev_init();
	dn_route_init();

#ifdef CONFIG_SYSCTL
	dn_register_sysctl();
#endif /* CONFIG_SYSCTL */

	/*
	 * Prevent DECnet module unloading until its fixed properly.
	 * Requires an audit of the code to check for memory leaks and
	 * initialisation problems etc.
	 */
	MOD_INC_USE_COUNT;

	return 0;

}

#ifndef MODULE
static int __init decnet_setup(char *str)
{
	unsigned short area = simple_strtoul(str, &str, 0);
	unsigned short node = simple_strtoul(*str > 0 ? ++str : str, &str, 0);

	decnet_address = dn_htons(area << 10 | node);
	dn_dn2eth(decnet_ether_address, dn_ntohs(decnet_address));

	return 1;
}

__setup("myscs=", decnet_setup);
#endif

static void __exit decnet_exit(void)
{
	sock_unregister(AF_DECnet);
	dev_remove_pack(&dn_dix_packet_type);

#ifdef CONFIG_SYSCTL
	dn_unregister_sysctl();
#endif /* CONFIG_SYSCTL */

	unregister_netdevice_notifier(&dn_dev_notifier);

	dn_route_cleanup();
	dn_dev_cleanup();
	dn_neigh_cleanup();

	proc_net_remove("myscs");
}

module_init(decnet_init);
module_exit(decnet_exit);
//#endif /* #if 0 second one */

#endif

int   scs_std$reqdata( struct _pdt *pdt_p, struct _cdrp *cdrp_p, void (*complete)() ) {
  struct _scs * scs = vmalloc(sizeof(struct _scs));
  struct _scs1 * scs1 = scs;
  struct _ppd * ppd = scs;
  struct _cdt * cdt = cdrp_p->cdrp$l_cdt;
  ppd->ppd$b_opc=PPD$C_REQDAT;
  scs->scs$w_mtype=SCS$C_APPL_DG;
  scs1->scs$l_lconid=cdt->cdt$l_lconid;
  scs->scs$l_src_conid=cdt->cdt$l_lconid;
  scs->scs$l_dst_conid=cdt->cdt$l_rconid;
  scs1->scs$l_rspid=cdrp_p->cdrp$l_rspid;
  scs1->scs$l_xct_len=cdrp_p->cdrp$l_xct_len;
  //insque(cdrp_p,&pdt_p->pdt$q_comql);
  scs_lower_level_send(cdrp_p,scs);
}


int   scs_std$request_data ( struct _pdt *pdt_p, struct _cdrp *cdrp_p, void (*complete)() ) {
  return scs_std$reqdata(pdt_p,cdrp_p,complete);
}

int   scs_std$senddata ( struct _pdt *pdt_p, struct _cdrp *cdrp_p, void (*complete)() ) {
  struct _scs * scs = vmalloc(sizeof(struct _scs));
  struct _scs1 * scs1 = scs;
  struct _ppd * ppd = scs;
  struct _cdt * cdt = cdrp_p->cdrp$l_cdt;
  ppd->ppd$b_opc=PPD$C_SNDDAT;
  scs->scs$w_mtype=SCS$C_APPL_DG;
  scs1->scs$l_lconid=cdt->cdt$l_lconid;
  scs->scs$l_src_conid=cdt->cdt$l_lconid;
  scs->scs$l_dst_conid=cdt->cdt$l_rconid;
  scs1->scs$l_rspid=cdrp_p->cdrp$l_rspid;
  scs1->scs$l_xct_len=cdrp_p->cdrp$l_xct_len;
  //insque(cdrp_p,&pdt_p->pdt$q_comql);
  scs_lower_level_send(cdrp_p,scs);
}

int   scs_std$send_data ( struct _pdt *pdt_p, struct _cdrp *cdrp_p, void (*complete)() ) { 
  return scs_std$senddata(pdt_p,cdrp_p,complete);
}

cwpsmyerr(){}

cwpslisten(void * packet, struct _cdt * c, struct _pdt * p) {
  int sts;
  struct _iosb * iosb=vmalloc(sizeof(struct _iosb));
  struct _cdrp * cdrp;
  struct _scs * scs = packet;
  struct _cdt * cdt = &cdtl[scs->scs$l_dst_conid];
  struct _cwpssrv * cp = ((unsigned long)packet) + sizeof(*scs);
  void * next = (unsigned long) cp + sizeof(struct _cwpssrv);
  switch (cp->cwpssrv$b_subtype) {
  case CWPSSRV$K_FORCEX:
    {
      struct _cwpsfex * fex = next;
      exe$forcex(&cp->cwpssrv$l_sought_epid,0,fex->cwpsfex$l_code);
    }
    break;
  default:
  }
}


init_cwps() {
  char myname[]="cwps";
  char myinfo[]="cwps service";

  //  listen(msgbuf,err,cdt,pdt,cdt);
  scs_std$listen(cwpslisten,cwpsmyerr,myname,myinfo,0);

}

cwps$getjpi(){}

cwps$getjpi_pscan(){}

cwps$srcv_getjpi_ast(){}

// module cwps_service_recv

int ddb_transfer(struct _cdt * conf_cdt) {
  struct _ddb * ddb;
  struct _ucb * ucb;
  char buf[512]; 
  char *b = buf;
  int i;
  struct _cdrp * cdrp=kmalloc(sizeof(struct _cdrp),GFP_KERNEL);
  struct _scs_rd *r;

  __du_init(); //temp placement?

  // now to transfer ddb

  printk("initiating ddb transfer\n");

  ddb = ioc$gl_devlist;

  while (ddb) {
    if (ddb->ddb$ps_sb==0 || ddb->ddb$ps_sb==&mysb) {
      *b++=DYN$C_DDB;
      memcpy(b,&ddb->ddb$t_name,16);
      b+=16;
      *b++=DYN$C_UCB;
      ucb=ddb->ddb$l_ucb;
      *b++=ucb->ucb$w_unit_seed;
    }
    ddb=ddb->ddb$ps_link;
  }
  *b++==0;
  *b++==0;
  *b++==0;
  *b++==0;

  cdrp->cdrp$l_rwcptr=0;
  cdrp->cdrp$l_rspid=scs_std$alloc_rspid(0,0,cdrp,0);
  scs_std$find_rdte( cdrp->cdrp$l_rspid, &r);
  r->rd$l_cdrp=cdrp;

  cdrp->cdrp$l_cdt=conf_cdt; //&cdtl[6]; // temp dummy ?
  cdrp->cdrp$w_cdrpsize=512;
  cdrp->cdrp$l_msg_buf=buf;
  cdrp->cdrp$l_xct_len=100+512;
  scs_std$senddg(0,100+512,cdrp);
  kfree(cdrp);
  printk("ending ddb transfer\n");

  // also more to transfer

}

extern int cf_msg();
extern int cf_dg();
extern int cf_err();
extern int dlm_msg();
extern int dlm_dg();
extern int dlm_err();
extern int du_msg();
extern int du_dg();
extern int du_err();
extern int forcex_msg();
extern int forcex_dg();
extern int forcex_err();

int startconnect(int none) {
  struct _cdt * cdt;
  struct _cdt * conf_cdt;
  char * s;

  printk("starting cluster connects\n");
  scs_std$connect(du_msg,du_dg,du_err,0,0,"mscp$disk","vms$disk_cl_drv",0,0,0,0,0/*s*/,0,0,0,0,0,0,0,0,0);
  // should be vms$disk_cl_drvr but use null-term for now

  scs_std$connect(cf_msg,cf_dg,cf_err,0,0,"configure","configclient",0,0,0,0,0/*s*/,&conf_cdt,0,0,0,0,0,0,0,0);

  scs_std$connect(forcex_msg,forcex_dg,forcex_err,0,0,"cwps","cwps$forcex",0,0,0,0,0,&cdt,0,0,0,0,0,0,0,0); 

  scs_std$connect(dlm_msg,dlm_dg,dlm_err,0,0,"dlm$dlm",&current->pcb$t_lname,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

	  {
	    signed long long time=-100000000;
	    //	    $DESCRIPTOR(tensec,"0 00:00:10.00");
	    //exe$bintim(&tensec,&time);
	    //time=-time;
	    exe$setimr(0,&time,ddb_transfer,conf_cdt,0);
	  }
}

