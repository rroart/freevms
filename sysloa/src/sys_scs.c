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
#include <net/dn.h>
#include <net/dn_nsp.h>
#include <net/dn_dev.h>
#include <net/dn_route.h>
#include <net/dn_fib.h>
#include <net/dn_neigh.h>

#include <cdrpdef.h>
#include <cdtdef.h>
#include <chdef.h>
#include <nisca.h>
#include <pbdef.h>
#include <pdtdef.h>
#include <sbdef.h>
#include <sbnbdef.h>
#include <scsdef.h>
#include <ssdef.h>
#include <system_data_cells.h>
#include <vcdef.h>

struct _pb mypb;
struct _sb mysb;

static void dn_keepalive(struct _cdt *sk);

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
static struct _cdt *dn_wild_sk;

static int __dn_setsockopt(struct _cdt *sock, int level, int optname, char *optval, int optlen, int flags);
static int __dn_getsockopt(struct _cdt *sock, int level, int optname, char *optval, int *optlen, int flags);

int dn_sockaddr2username(struct sockaddr_dn *sdn, unsigned char *buf, unsigned char type)
{
	int len = 2;

	*buf++ = type;

	switch(type) {
		case 0:
			*buf++ = sdn->sdn_objnum;
			break;
		case 1:
			*buf++ = 0;
			*buf++ = dn_ntohs(sdn->sdn_objnamel);
			memcpy(buf, sdn->sdn_objname, dn_ntohs(sdn->sdn_objnamel));
			len = 3 + dn_ntohs(sdn->sdn_objnamel);
			break;
		case 2:
			memset(buf, 0, 5);
			buf += 5;
			*buf++ = dn_ntohs(sdn->sdn_objnamel);
			memcpy(buf, sdn->sdn_objname, dn_ntohs(sdn->sdn_objnamel));
			len = 7 + dn_ntohs(sdn->sdn_objnamel);
			break;
	}

	return len;
}

/*
 * On reception of usernames, we handle types 1 and 0 for destination
 * addresses only. Types 2 and 4 are used for source addresses, but the
 * UIC, GIC are ignored and they are both treated the same way. Type 3
 * is never used as I've no idea what its purpose might be or what its
 * format is.
 */
int dn_username2sockaddr(unsigned char *data, int len, struct sockaddr_dn *sdn, unsigned char *fmt)
{
	unsigned char type;
	int size = len;
	int namel = 12;

	sdn->sdn_objnum = 0;
	sdn->sdn_objnamel = dn_htons(0);
	memset(sdn->sdn_objname, 0, DN_MAXOBJL);

	if (len < 2)
		return -1;

	len -= 2;
	*fmt = *data++;
	type = *data++;

	switch(*fmt) {
		case 0:
			sdn->sdn_objnum = type;
			return 2;
		case 1:
			namel = 16;
			break;
		case 2:
			len  -= 4;
			data += 4;
			break;
		case 4:
			len  -= 8;
			data += 8;
			break;
		default:
			return -1;
	}

	len -= 1;

	if (len < 0)
		return -1;

	sdn->sdn_objnamel = dn_htons(*data++);
	len -= dn_ntohs(sdn->sdn_objnamel);

	if ((len < 0) || (dn_ntohs(sdn->sdn_objnamel) > namel))
		return -1;

	memcpy(sdn->sdn_objname, data, dn_ntohs(sdn->sdn_objnamel));

	return size - len;
}

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

extern int dn_nsp_backlog_rcv2(struct _cdt *sk, struct sk_buff *skb);

static void dn_destruct(struct _cdt *sk)
{
	struct _cdt *scp = sk;

	skb_queue_purge(&scp->cdt$l_waitqfl);
	skb_queue_purge(&scp->cdt$l_waitqfl);
	skb_queue_purge(&scp->cdt$l_share_flink);

	MOD_DEC_USE_COUNT;
}

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

	/* Initialization of DECnet Session Control Port		*/
	scp->cdt$w_state	= CDT$C_OPEN;		/* Open			*/
	scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr	= 1;		/* Next data seg to tx	*/
	scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr	= 1;		/* Next oth data to tx  */
	scp->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_rmt_chan_seq_num = 0;		/* Last data seg ack'ed */
	scp->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_rmt_chan_seq_num = 0;		/* Last oth data ack'ed */
	scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr = 0;		/* Highest data ack recv*/
	scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr = 0;		/* Last oth data ack rec*/

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

/*
 * Keepalive timer.
 * FIXME: Should respond to SO_KEEPALIVE etc.
 */
static void dn_keepalive(struct _cdt *sk)
{
	struct _cdt *scp = sk;

	/*
	 * By checking the other_data transmit queue is empty
	 * we are double checking that we are not sending too
	 * many of these keepalive frames.
	 */
	if (skb_queue_len(&scp->cdt$l_waitqfl) == 0)
		dn_nsp_send_link2(sk, DN_NOCHANGE, 0);
}


/*
 * Timer for shutdown/destroyed sockets.
 * When socket is dead & no packets have been sent for a
 * certain amount of time, they are removed by this
 * routine. Also takes care of sending out DI & DC
 * frames at correct times.
 */
int dn_destroy_timer2(struct _cdt *sk)
{
	struct _cdt *scp = sk;

	switch(scp->cdt$w_state) {
		case CDT$C_DISC_SENT:
			dn_nsp_send_disc2(sk, NSP_DISCINIT, 0, GFP_ATOMIC);
			scp->cdt$w_state = CDT$C_DISC_REC;
			return 0;

		case CDT$C_REJ_SENT:
			dn_nsp_send_disc2(sk, NSP_DISCINIT, 0, GFP_ATOMIC);
			scp->cdt$w_state = CDT$C_REJ_SENT;
			return 0;

			}


                if (sk)
		return 0;

	dn_stop_fast_timer2(sk); /* unlikely, but possible that this is runninng */
	if ((jiffies) >= (HZ * decnet_time_wait)) {
		sock_put(sk);
		return 1;
	}

	return 0;
}

static void dn_destroy_sock(struct _cdt *sk)
{
	struct _cdt *scp = sk;

	if (sk) {
		if (sk->cdt$w_state != SS_UNCONNECTED)
			sk->cdt$w_state = SS_DISCONNECTING;
	}

	sk->cdt$w_state = CDT$C_CLOSED;

	switch(scp->cdt$w_state) {
		case CDT$C_DISC_SENT:
			dn_nsp_send_disc2(sk, NSP_DISCCONF, NSP_REASON_DC, 0);
			break;
		case CDT$C_CON_REC:
			scp->cdt$w_state = CDT$C_REJ_SENT;
			goto disc_reject;
			scp->cdt$w_state = CDT$C_DISC_SENT;
		case CDT$C_REJ_SENT:
disc_reject:
			dn_nsp_send_disc2(sk, NSP_DISCINIT, 0, 0);
		case CDT$C_VC_FAIL:
		case CDT$C_DISC_ACK:
		case CDT$C_DISC_REC:
		case CDT$C_CON_SENT:
			break;
		default:
			printk(KERN_DEBUG "DECnet: dn_destroy_sock passed socket in invalid state\n");
		case CDT$C_OPEN:
			dn_stop_fast_timer2(sk);
			dn_stop_slow_timer2(sk);

			sock_put(sk);

			break;
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
			return "  CR";
		case CDT$C_REJ_SENT:
			return "  DR";
		case CDT$C_CON_ACK:
			return "  CC";
		case CDT$C_VC_FAIL:
			return "  NC";
		case CDT$C_CON_SENT:
			return "  CD";
		case CDT$C_DISC_SENT:
			return "  DI";
		case CDT$C_DISC_ACK:
			return " DIC";
		case CDT$C_CLOSED:
			return "  CL";
		case CDT$C_DISC_REC:
			return "  CN";
	}

	return "????";
}

static int dn_create(struct _cdt *sock, int protocol)
{
	struct _cdt *sk;

	if ((sk = dn_alloc_sock(sock, GFP_KERNEL)) == NULL) 
		return -ENOBUFS;

	return 0;
}


static int
dn_release(struct _cdt *sock)
{
	struct _cdt *sk = sock;

	if (sk) {
		sock_orphan(sk);
		sock_hold(sk);
		
		dn_destroy_sock(sk);
		
		sock_put(sk);
	}

        return 0;
}

static int dn_bind(struct _cdt *sock, struct sockaddr *uaddr, int addr_len)
{
	struct _cdt *sk = sock;
	struct _cdt *scp = sk;
	struct sockaddr_dn *saddr = (struct sockaddr_dn *)uaddr;
	struct net_device *dev;
	int rv;

	if (addr_len != sizeof(struct sockaddr_dn))
		return -EINVAL;

	if (saddr->sdn_family != AF_DECnet)
		return -EINVAL;

	if (dn_ntohs(saddr->sdn_nodeaddrl) && (dn_ntohs(saddr->sdn_nodeaddrl) != 2))
		return -EINVAL;

	if (saddr->sdn_objnum && !capable(CAP_NET_BIND_SERVICE))
		return -EPERM;

	if (dn_ntohs(saddr->sdn_objnamel) > DN_MAXOBJL)
		return -EINVAL;

	if (saddr->sdn_flags & ~SDF_WILD)
		return -EINVAL;

	if (saddr->sdn_flags & SDF_WILD) {
		if (!capable(CAP_NET_BIND_SERVICE))
			return -EPERM;
	} else {
		if (dn_ntohs(saddr->sdn_nodeaddrl)) {
			read_lock(&dev_base_lock);
			for(dev = dev_base; dev; dev = dev->next) {
				if (!dev->dn_ptr)
					continue;
				if (dn_dev_islocal(dev, dn_saddr2dn(saddr)))
					break;
			}
			read_unlock(&dev_base_lock);
			if (dev == NULL)
				return -EADDRNOTAVAIL;
		}
	}


	memcpy(&scp->cdt$l_lconid, saddr, addr_len);

        return rv;
}


static int dn_auto_bind(struct _cdt *sock)
{
	struct _cdt *sk = sock;
	struct _cdt *scp = sk;




	/*
	 * This stuff is to keep compatibility with Eduardo's
	 * patch. I hope I can dispense with it shortly...
	 */



	return 0;
}


static int dn_connect(struct _cdt *sock, struct sockaddr *uaddr, int addr_len, int flags) {

}

// whoa...
int scs$connect (void (*msgadr)(), void (*dgadr)(), void (*erradr)(), void *rsysid, void *rstadr, void *rprnam, void *lprnam, int initcr, int minscr, int initdg, int blkpri, void *condat, void *auxstr, void (*badrsp)(), void (*movadr)(), int load_rating,int (*req_fast_recvmsg)(), void (*fast_recvmsg_pm)(), void (*change_aff)(), void (*complete)(), int connect_parameter)
{
	struct _cdt *cdt = find_free_cdt();
	struct _cdt *sock = cdt;
	struct _cdt *sk = sock;
	struct _cdt *scp = sk;
	int err = -EISCONN;

	struct _sbnb *s = scs_register_name(lprnam,"");
	s->sbnb$w_local_index=cdt->cdt$l_lconid;
	cdt->cdt$w_state = CDT$C_CLOSED;

	sk->cdt$w_state   = CDT$C_CON_SENT;

	dn_nsp_send_conninit2(sk, SCS$C_CON_REQ,rprnam,lprnam,condat);

	for( ; ; ) {
	  volatile state=sk->cdt$w_state;
	  if (state==CDT$C_OPEN) goto out2;
	}
 out2:

	err = 0;
out:
        return err;
}

static void dn_access_copy(struct sk_buff *skb, struct accessdata_dn *acc)
{
        unsigned char *ptr = skb->data;

        acc->acc_userl = *ptr++;
        memcpy(&acc->acc_user, ptr, acc->acc_userl);
        ptr += acc->acc_userl;

        acc->acc_passl = *ptr++;
        memcpy(&acc->acc_pass, ptr, acc->acc_passl);
        ptr += acc->acc_passl;

        acc->acc_accl = *ptr++;
        memcpy(&acc->acc_acc, ptr, acc->acc_accl);

        skb_pull(skb, acc->acc_accl + acc->acc_passl + acc->acc_userl + 3);

}

static void dn_user_copy(struct sk_buff *skb, struct optdata_dn *opt)
{
        unsigned char *ptr = skb->data;
        
        opt->opt_optl   = *ptr++;
        opt->opt_status = 0;
        memcpy(opt->opt_data, ptr, opt->opt_optl);
        skb_pull(skb, opt->opt_optl + 1);

}


/*
 * This is here for use in the sockopt() call as well as
 * in accept(). Must be called with a locked socket.
 */
static int dn_wait_accept(struct _cdt *sock, int flags)
{
        struct _cdt *sk = sock;

        while(sk->cdt$w_state == CDT$C_LISTEN) {
                if (flags & O_NONBLOCK) {
                        return -EAGAIN;
                }

		

		if (sk->cdt$w_state == CDT$C_LISTEN)
			schedule();

		

                if (signal_pending(current))
                        return -ERESTARTSYS; /* But of course you don't! */
        }

        if ((sk->cdt$w_state != CDT$C_OPEN) && (sk->cdt$w_state != CDT$C_REJ_SENT)) {
                sock->cdt$w_state = SS_UNCONNECTED;
                return sock_error(sk);
        }

	sock->cdt$w_state = SS_CONNECTED;

        return 0;
}

//static int dn_accept(struct _cdt *sock, struct _cdt *newsock, int flags)

int scs$accept (void (*msgadr)(), void (*dgadr)(), void (*erradr)(), void *rsysid, void *rstadr, void *rprnam, void *lprnam, int initcr, int minscr, int initdg, int blkpri, void *condat, void *auxstr, void (*badrsp)(), void (*movadr)(), int load_rating,int (*req_fast_recvmsg)(), void (*fast_recvmsg_pm)(), void (*change_aff)(), void (*complete)(), struct _cdt * cdt, int connect_parameter)
{
	struct _cdt *sk = cdt, *newsk, *newsock;
	struct sk_buff *skb = NULL;
	struct _cdt *cb;
	unsigned char menuver;
	int err = 0;
	unsigned char type;
	int flags;
	

        if (sk->cdt$w_state != CDT$C_LISTEN) {
		
		return -EINVAL;
	}

	if (sk->cdt$w_state != CDT$C_OPEN) {
		
		return -EINVAL;
	}

        do
        {
                if ((skb = skb_dequeue(&sk->cdt$l_share_flink)) == NULL)
                {
                        if (flags & O_NONBLOCK)
                        {
                                
                                return -EAGAIN;
                        }

			

			if (!skb_peek(&sk->cdt$l_share_flink))
				schedule();

			

                        if (signal_pending(current))
                        {
				
                                return -ERESTARTSYS;
                        }
                }
        } while (skb == NULL);

	cb = (skb);

	if ((newsk = dn_alloc_sock(newsock, 0)) == NULL) {
		
		kfree_skb(skb);
		return -ENOBUFS;
	}

        (newsk)->cdt$w_state        = CDT$C_CON_REC;
	(newsk)->cdt$l_rconid      = 0;
	
	newsk->cdt$w_state  = CDT$C_LISTEN;

	memcpy(&((newsk)->cdt$l_lconid), &(sk->cdt$l_lconid), sizeof(struct sockaddr_dn));

	/*
	 * If we are listening on a wild socket, we don't want
	 * the newly created socket on the wrong hash queue.
	 */


	skb_pull(skb, dn_username2sockaddr(skb->data, skb->len, &((newsk)->cdt$l_lconid), &type));
	skb_pull(skb, dn_username2sockaddr(skb->data, skb->len, &((newsk)->cdt$l_rconid), &type));



	menuver = *skb->data;
	skb_pull(skb, 1);

	kfree_skb(skb);

	
	/*
	 * FIXME: This can fail if we've run out of local ports....
	 */

	dn_send_conn_ack2(newsk);

	/*
	 * Here we use sk->allocation since although the conn conf is
	 * for the newsk, the context is the old socket.
	 */
        return err;
}


static int dn_getname(struct _cdt *sock, struct sockaddr *uaddr,int *uaddr_len,int peer)
{
	struct sockaddr_dn *sa = (struct sockaddr_dn *)uaddr;
	struct _cdt *sk = sock;
	struct _cdt *scp = sk;

	*uaddr_len = sizeof(struct sockaddr_dn);

	

	if (peer) {
		if ((sock->cdt$w_state != SS_CONNECTED && 
		     sock->cdt$w_state != SS_CONNECTING))
			return -ENOTCONN;

		memcpy(sa, &scp->cdt$l_rconid, sizeof(struct sockaddr_dn));
	} else {
		memcpy(sa, &scp->cdt$l_lconid, sizeof(struct sockaddr_dn));
	}

	

        return 0;
}


static unsigned int dn_poll(struct file *file, struct _cdt *sock, poll_table  *wait)
{
	struct _cdt *sk = sock;
	struct _cdt *scp = sk;
	int mask = datagram_poll(file, sock, wait);

	if (skb_queue_len(&scp->cdt$l_share_flink))
		mask |= POLLRDBAND;

	return mask;
}

static int dn_ioctl(struct _cdt *sock, unsigned int cmd, unsigned long arg)
{
	struct _cdt *sk = sock;
	struct _cdt *scp = sk;
	int err = -EOPNOTSUPP;
	unsigned long amount = 0;
	struct sk_buff *skb;
	int val;

	switch(cmd)
	{
	case SIOCGIFADDR:
	case SIOCSIFADDR:
		return dn_dev_ioctl(cmd, (void *)arg);

	case SIOCATMARK:
		
		val = (skb_queue_len(&scp->cdt$l_share_flink) != 0);
		if (scp->cdt$w_state != CDT$C_OPEN)
			val = -ENOTCONN;
		
		return val;

	case OSIOCSNETADDR:
		if (!capable(CAP_NET_ADMIN)) {
			err = -EPERM;
			break;
		}

		dn_dev_devices_off();

		decnet_address = (unsigned short)arg;
		dn_dn2eth(decnet_ether_address, dn_ntohs(decnet_address));

		dn_dev_devices_on();
		err = 0;
		break;

	case OSIOCGNETADDR:
		err = put_user(decnet_address, (unsigned short *)arg);
		break;
        case SIOCGIFCONF:
        case SIOCGIFFLAGS:
        case SIOCGIFBRDADDR:
                return dev_ioctl(cmd,(void *)arg);

	case TIOCOUTQ:
		if (amount < 0)
			amount = 0;
		err = put_user(amount, (int *)arg);
		break;

	case TIOCINQ:
		
		if ((skb = skb_peek(&scp->cdt$l_share_flink)) != NULL) {
			amount = skb->len;
		} else {
			struct sk_buff *skb = sk->cdt$l_share_flink;
			for(;;) {
				if (skb == (struct sk_buff *)&sk->cdt$l_share_flink)
					break;
				amount += skb->len;
				skb = skb->next;
			}
		}
		
		err = put_user(amount, (int *)arg);
		break;
	}

	return err;
}

//#endif /* #if 0 */

#include<cdldef.h>
#include<cdtdef.h>
#include<ddtdef.h>
#include<dptdef.h>
#include<dyndef.h>
#include<fdtdef.h>
#include<mscpdef.h> // does not belong here
#include<pdtdef.h>
#include<rddef.h>
#include<rdtdef.h>
#include<sbnbdef.h>
#include<iodef.h>
#include<system_data_cells.h>
#include<linux/vmalloc.h>
#include<sys/errno.h>

//static struct _cdt * cdl$l_freecdt;
extern struct _cdt cdtl[1024];
struct _rdt rdt;
extern struct _scs_rd rdtl[128];
struct _cdl cdl;

void * find_mscp_cdt(void) {
  /* remember to fix cdldef */
  int i;
  for (i=0; i<100; i++) {
    if (cdtl[i].cdt$l_rconid) return &cdtl[i];
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

int rspid_alloc(struct _cdrp * c) {
  /* remember to fix rdtdef */
  struct _scs_rd * r=rdt.rdt$l_freerd;
  rdt.rdt$l_freerd=r->rd$l_link;
  r->rd$w_state=1;
  r->rd$l_cdrp=c;
  return r->rd$w_seqnum;
}

void * scs_register_name(char * c1, char * c2) {
  struct _sbnb * s=vmalloc(sizeof(struct _sbnb));
  bzero(s,sizeof(struct _sbnb));

  bcopy(c1,&s->sbnb$b_procnam,max(16,strlen(c1)));

  insque(s,scs$gq_local_names);

  return s;
}

void * scs_find_name(char * c1) {
  struct _sbnb * head = &scs$gq_local_names;
  struct _sbnb * tmp = head->sbnb$l_flink;
  while(tmp!=head) {
    if (strcmp(c1,&tmp->sbnb$b_procnam)) return tmp;
    tmp=tmp->sbnb$l_flink;
  }
  return 0;
}

//static int scs$listen(struct _cdt *sock, int backlog)
//sock is about-ish lprnam;

int scs$listen (void (*msgadr)(void *msg_buf, struct _cdt **cdt, struct _pdt *pdt ),
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

int /*__init*/ scs_init(void) {
  int i;
  char myname[]="scs$directory";
  char myinfo[]="directory srv";

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

  scs$listen(dir_listen,mydirerr,myname,myinfo,0);

}

//#if 0
static int dn_shutdown(struct _cdt *sock, int how)
{
	struct _cdt *sk = sock;
	struct _cdt *scp = sk;
	int err = -ENOTCONN;

	

	if (sock->cdt$w_state == SS_UNCONNECTED)
		goto out;

	err = 0;
	if (sock->cdt$w_state == SS_DISCONNECTING)
		goto out;

	err = -EINVAL;
	if (scp->cdt$w_state == CDT$C_OPEN)
		goto out;

	if (how != SHUTDOWN_MASK)
		goto out;


	dn_destroy_sock(sk);
	err = 0;

out:
	

	return err;
}

static int dn_setsockopt(struct _cdt *sock, int level, int optname, char *optval, int optlen)
{
	struct _cdt *sk = sock;
	int err;

	
	err = __dn_setsockopt(sock, level, optname, optval, optlen, 0);
	

	return err;
}

static int __dn_setsockopt(struct _cdt *sock, int level,int optname, char *optval, int optlen, int flags) 
{
	struct	sock *sk = sock;
	struct _cdt *scp = sk;
	union {
		struct optdata_dn opt;
		struct accessdata_dn acc;
		int mode;
		unsigned long win;
		int val;
		unsigned char services;
		unsigned char info;
	} u;
	int err;

	if (optlen && !optval)
		return -EINVAL;

	if (optlen > sizeof(u))
		return -EINVAL;

	if (copy_from_user(&u, optval, optlen))
		return -EFAULT;

	switch(optname) {
		case DSO_CONDATA:
			if (sock->cdt$w_state == SS_CONNECTED) 
				return -EISCONN;
			if ((scp->cdt$w_state != CDT$C_OPEN) && (scp->cdt$w_state != CDT$C_CON_REC))
				return -EINVAL;

			if (optlen != sizeof(struct optdata_dn))
				return -EINVAL;

			if (u.opt.opt_optl > 16)
				return -EINVAL;

			break;

		case DSO_DISDATA:
			if (optlen != sizeof(struct optdata_dn))
				return -EINVAL;

			if (u.opt.opt_optl > 16)
				return -EINVAL;

			break;

		case DSO_CONACCESS:
			if (sock->cdt$w_state == SS_CONNECTED) 
				return -EISCONN;
			if (scp->cdt$w_state != CDT$C_OPEN)
				return -EINVAL;

			if (optlen != sizeof(struct accessdata_dn))
				return -EINVAL;

			if ((u.acc.acc_accl > DN_MAXACCL) ||
					(u.acc.acc_passl > DN_MAXACCL) ||
					(u.acc.acc_userl > DN_MAXACCL))
				return -EINVAL;

			break;

		case DSO_ACCEPTMODE:
			if (sock->cdt$w_state == SS_CONNECTED)
				return -EISCONN;
			if (scp->cdt$w_state != CDT$C_OPEN)
				return -EINVAL;

			if (optlen != sizeof(int))
				return -EINVAL;

			if ((u.mode != ACC_IMMED) && (u.mode != ACC_DEFER))
				return -EINVAL;

			break;

		case DSO_CONACCEPT:

			if (scp->cdt$w_state != CDT$C_CON_REC)
				return -EINVAL;

			scp->cdt$w_state = CDT$C_CON_ACK;
			dn_send_conn_conf2(sk, 0);
			err = dn_wait_accept(sock, sock);
			return err;

		case DSO_CONREJECT:

			if (scp->cdt$w_state != CDT$C_CON_REC)
				return -EINVAL;

			scp->cdt$w_state = CDT$C_REJ_SENT;
			dn_nsp_send_disc2(sk, 0x38, 0, 0);
			break;

		default:
#ifdef CONFIG_NETFILTER
		return nf_setsockopt(sk, PF_DECnet, optname, optval, optlen);
#endif
		case DSO_LINKINFO:
		case DSO_STREAM:
		case DSO_SEQPACKET:
			return -ENOPROTOOPT;

		case DSO_MAXWINDOW:
			if (optlen != sizeof(unsigned long))
				return -EINVAL;
			if (u.win > NSP_MAX_WINDOW)
				u.win = NSP_MAX_WINDOW;
			if (u.win == 0)
				return -EINVAL;
			break;

		case DSO_NODELAY:
			if (optlen != sizeof(int))
				return -EINVAL;
			break;

		case DSO_CORK:
			if (optlen != sizeof(int))
				return -EINVAL;
			break;

		case DSO_SERVICES:
			if (optlen != sizeof(unsigned char))
				return -EINVAL;
			if ((u.services & ~NSP_FC_MASK) != 0x01)
				return -EINVAL;
			if ((u.services & NSP_FC_MASK) == NSP_FC_MASK)
				return -EINVAL;
			break;

		case DSO_INFO:
			if (optlen != sizeof(unsigned char))
				return -EINVAL;
			if (u.info & 0xfc)
				return -EINVAL;
			break;
	}

	return 0;
}

static int dn_getsockopt(struct _cdt *sock, int level, int optname, char *optval, int *optlen)
{
	struct _cdt *sk = sock;
	int err;

	
	err = __dn_getsockopt(sock, level, optname, optval, optlen, 0);
	

	return err;
}

static int __dn_getsockopt(struct _cdt *sock, int level,int optname, char *optval,int *optlen, int flags)
{
	struct	sock *sk = sock;
	struct _cdt *scp = sk;
	struct linkinfo_dn link;
	unsigned int r_len;
	void *r_data = NULL;
	unsigned int val;

	if(get_user(r_len , optlen))
		return -EFAULT;
		
	switch(optname) {
		case DSO_CONDATA:
			if (r_len > sizeof(struct optdata_dn))
				r_len = sizeof(struct optdata_dn);
			break;

		case DSO_DISDATA:
			if (r_len > sizeof(struct optdata_dn))
				r_len = sizeof(struct optdata_dn);
			break;

		case DSO_CONACCESS:
			break;

		case DSO_ACCEPTMODE:
			if (r_len > sizeof(unsigned char))
				r_len = sizeof(unsigned char);
			break;

		case DSO_LINKINFO:
			if (r_len > sizeof(struct linkinfo_dn))
				r_len = sizeof(struct linkinfo_dn);

			switch(sock->cdt$w_state) {
				case SS_CONNECTING:
					link.idn_linkstate = LL_CONNECTING;
					break;
				case SS_DISCONNECTING:
					link.idn_linkstate = LL_DISCONNECTING;
					break;
				case SS_CONNECTED:
					link.idn_linkstate = LL_RUNNING;
					break;
				default:
					link.idn_linkstate = LL_INACTIVE;
			}

			r_data = &link;
			break;

		default:
#ifdef CONFIG_NETFILTER
		{
			int val, len;
			
			if(get_user(len, optlen))
				return -EFAULT;
			
			val = nf_getsockopt(sk, PF_DECnet, optname, 
							optval, &len);
			if (val >= 0)
				val = put_user(len, optlen);
			return val;
		}
#endif
		case DSO_STREAM:
		case DSO_SEQPACKET:
		case DSO_CONACCEPT:
		case DSO_CONREJECT:
        		return -ENOPROTOOPT;

		case DSO_MAXWINDOW:
			if (r_len > sizeof(unsigned long))
				r_len = sizeof(unsigned long);
			break;

		case DSO_NODELAY:
			if (r_len > sizeof(int))
				r_len = sizeof(int);
			r_data = &val;
			break;

		case DSO_CORK:
			if (r_len > sizeof(int))
				r_len = sizeof(int);
			r_data = &val;
			break;

		case DSO_SERVICES:
			if (r_len > sizeof(unsigned char))
				r_len = sizeof(unsigned char);
			r_data = 0;
			break;

		case DSO_INFO:
			if (r_len > sizeof(unsigned char))
				r_len = sizeof(unsigned char);
			break;
	}

	if (r_data) {
		if (copy_to_user(optval, r_data, r_len))
			return -EFAULT;
		if (put_user(r_len, optlen))
			return -EFAULT;
	}

	return 0;
}


/*
 * Used by send/recvmsg to wait until the socket is connected
 * before passing data.
 */
static int dn_wait_run(struct _cdt *sk, int flags)
{
	struct _cdt *scp = sk;
	int err = 0;

	switch(scp->cdt$w_state) {
		case CDT$C_OPEN:
			return 0;

		case CDT$C_CON_REC:
			scp->cdt$w_state = CDT$C_CON_ACK;
			dn_send_conn_conf2(sk, 0);
			return dn_wait_accept(sk, (flags & MSG_DONTWAIT) ? O_NONBLOCK : 0);
		case CDT$C_CON_SENT:
		case CDT$C_CON_ACK:
			break;
		default:
			return -ENOTCONN;
	}

	if (flags & MSG_DONTWAIT)
		return -EWOULDBLOCK;

	do {
		if ((err = sock_error(sk)) != 0)
			break;

		if (signal_pending(current)) {
			err = -ERESTARTSYS;
			break;
		}

		

		if (scp->cdt$w_state != CDT$C_OPEN)
			schedule();

		

	} while(scp->cdt$w_state != CDT$C_OPEN);

	return 0;
}


static int dn_data_ready(struct _cdt *sk, struct sk_buff_head *q, int flags, int target)
{
	struct sk_buff *skb = q->next;
	int len = 0;

	if (flags & MSG_OOB)
		return skb_queue_len(q) ? 1 : 0;

	while(skb != (struct sk_buff *)q) {
		struct _cdt *cb = (skb);
		len += skb->len;

		if (cb->cdt$b_ls_flag & 0x40) {
			/* SOCK_SEQPACKET reads to EOM */
			/* so does SOCK_STREAM unless WAITALL is specified */
			if (!(flags & MSG_WAITALL))
				return 1;
		}

		/* minimum data length for read exceeded */
		if (len >= target)
			return 1;

		skb = skb->next;
	}

	return 0;
}


static int dn_recvmsg(struct _cdt *sock, struct msghdr *msg, int size,
	int flags, struct scm_cookie *scm)
{
	struct _cdt *sk = sock;
	struct _cdt *scp = sk;
	struct sk_buff_head *queue = &sk->cdt$l_share_flink;
	int target = size > 1 ? 1 : 0;
	int copied = 0;
	int rv = 0;
	struct sk_buff *skb, *nskb;
	struct _cdt *cb = NULL;
	unsigned char eor = 0;

	

	if ((rv = dn_wait_run(sk, flags)) != 0)
		goto out;

	if (flags & ~(MSG_PEEK|MSG_OOB|MSG_WAITALL|MSG_DONTWAIT)) {
		rv = -EOPNOTSUPP;
		goto out;
	}

	if (flags & MSG_OOB)
		queue = &scp->cdt$l_share_flink;

	if (flags & MSG_WAITALL)
		target = size;


	/*
	 * See if there is data ready to read, sleep if there isn't
	 */
	for(;;) {

		if (skb_queue_len(&scp->cdt$l_share_flink)) {
			if (!(flags & MSG_OOB)) {
				msg->msg_flags |= MSG_OOB;
			}
		}
		
		if (scp->cdt$w_state != CDT$C_OPEN)
			goto out;

		if (signal_pending(current)) {
			rv = -ERESTARTSYS;
			goto out;
		}

		if (dn_data_ready(sk, queue, flags, target))
			break;

		if (flags & MSG_DONTWAIT) {
			rv = -EWOULDBLOCK;
			goto out;
		}

		set_bit(SOCK_ASYNC_WAITDATA, &sock->cdt$b_ls_flag);
		

		if (!dn_data_ready(sk, queue, flags, target))
			schedule();

		
		clear_bit(SOCK_ASYNC_WAITDATA, &sock->cdt$b_ls_flag);
	}

	for(skb = queue->next; skb != (struct sk_buff *)queue; skb = nskb) {
		int chunk = skb->len;
		cb = (skb);

		if ((chunk + copied) > size)
			chunk = size - copied;

		if (memcpy_toiovec(msg->msg_iov, skb->data, chunk)) {
			rv = -EFAULT;
			break;
		}
		copied += chunk;

		if (!(flags & MSG_PEEK))
			skb_pull(skb, chunk);

		eor = cb->cdt$b_ls_flag & 0x40;
		nskb = skb->next;

		if (skb->len == 0) {
			skb_unlink(skb);
			kfree_skb(skb);
			/* 
			 * N.B. Don't refer to skb or cb after this point
			 * in loop.
			 */
				dn_nsp_send_link2(sk, DN_SEND, 0);
		}

		if (eor) { 
			if (!(flags & MSG_WAITALL))
				break;
		}

		if (flags & MSG_OOB)
			break;

		if (copied >= target)
			break;
	}

	rv = copied;


out:
	if (rv == 0)
		rv = (flags & MSG_PEEK) ? 0 : sock_error(sk);

	if ((rv >= 0) && msg->msg_name) {
		memcpy(msg->msg_name, &scp->cdt$l_rconid, sizeof(struct sockaddr_dn));
		msg->msg_namelen = sizeof(struct sockaddr_dn);
	}

	

	return rv;
}


static inline int dn_queue_too_long(struct _cdt *scp, struct sk_buff_head *queue, int flags)
{
	unsigned char fctype = 0;
	if (fctype != NSP_FC_NONE) {
		if (flags & MSG_OOB) {
		} else {
		}
	}
	return 0;
}

//static int dn_sendmsg(struct _cdt *sock, struct msghdr *msg, int size,struct scm_cookie *scm)

int scs_std$senddg(int disposition_flag, int dg_msg_length, struct _cdrp *cdrp ) {
  struct _cdt * sk=cdrp->cdrp$l_cdt;
  struct sk_buff *skb = NULL;
  if ((skb = dn_alloc_skb2(sk, 1000, GFP_ATOMIC)) == NULL)
    return;

  scs_msg_fill(skb,sk,0);

  scs_msg_fill_more(skb,sk,cdrp);

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

	if (flags & ~(MSG_TRYHARD|MSG_OOB|MSG_DONTWAIT|MSG_EOR))
		return -EOPNOTSUPP;

	if (addr_len && (addr_len != sizeof(struct sockaddr_dn)))
		return -EINVAL;

	if (scp->cdt$w_state == CDT$C_OPEN) {
		if (!addr_len || !addr) {
			err = -ENOTCONN;
			goto out;
		}

		if ((err = dn_connect(sock, (struct sockaddr *)addr, addr_len, (flags & MSG_DONTWAIT) ? O_NONBLOCK : 0)) < 0)
			goto out;
	}

	

	if ((err = dn_wait_run(sk, flags)) < 0)
		goto out;

	fctype = 0;

	/*
	 * The only difference between SEQPACKET & STREAM sockets under DECnet
	 * is that SEQPACKET sockets set the MSG_EOR flag for the last
	 * session control message segment. 
	 */

	if (flags & MSG_OOB) {
		mss = 16;
		queue = &scp->cdt$l_waitqfl;
		if (size > mss) {
			err = -EMSGSIZE;
			goto out;
		}
	}

	while(sent < size) {
		err = sock_error(sk);
		if (err)
			goto out;

		if (signal_pending(current)) {
			err = -ERESTARTSYS;
			goto out;
		}

		/*
		 * Calculate size that we wish to send.
		 */
		len = size - sent;

		if (len > mss)
			len = mss;

		/*
		 * Wait for queue size to go down below the window
		 * size.
		 */
		if (dn_queue_too_long(scp, queue, flags)) {
			if (flags & MSG_DONTWAIT) {
				err = -EWOULDBLOCK;
				goto out;
			}

			

			if (dn_queue_too_long(scp, queue, flags))
				schedule();

			

			continue;
		}

		/*
		 * Get a suitably sized skb.
		 */
		skb = dn_alloc_send_skb2(sk, &len, flags & MSG_DONTWAIT, &err);

		if (err)
			break;

		if (!skb)
			continue;

		cb = (skb);

		ptr = skb_put(skb, 9);

		if (memcpy_fromiovec(skb_put(skb, len), msg->msg_iov, len)) {
			err = -EFAULT;
			goto out;
		}

		if (flags & MSG_OOB) {
			cb->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_lcl_chan_seq_num = scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr;
			seq_add(&scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr, 1);
			msgflg = 0x30;
			ack = (scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr & 0x0FFF) | 0x8000;
			scp->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_rmt_chan_seq_num = scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr;
		} else {
			cb->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_lcl_chan_seq_num = scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr;
			seq_add(&scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr, 1);
			msgflg = 0x00;
			if (((sent + len) == size) && (flags & MSG_EOR)) {
				msgflg |= 0x40;
			}
			ack = (scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr & 0x0FFF) | 0x8000;
			scp->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_rmt_chan_seq_num = scp->cdt$l_pb->pb$l_vc_addr->vc$w_hsr;
		}

		*ptr++ = msgflg;
		*(__u16 *)ptr = scp->cdt$l_rconid;
		ptr += 2;
		*(__u16 *)ptr = scp->cdt$l_lconid;
		ptr += 2;
		*(__u16 *)ptr = dn_htons(ack);
		ptr += 2;
		*(__u16 *)ptr = dn_htons(cb->cdt$l_pb->pb$l_vc_addr->vc$l_preferred_channel->ch$w_lcl_chan_seq_num);

		sent += len;
		dn_nsp_queue_xmit2(sk, skb, 0, flags & MSG_OOB);
		skb = NULL;

	}
out:

	if (skb)
		kfree_skb(skb);

	

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
	create:		dn_create,
};

static struct proto_ops dn_proto_ops = {
	family:		AF_DECnet,

	release:	dn_release,
	bind:		dn_bind,
	connect:	dn_connect,
	socketpair:	sock_no_socketpair,
	accept:		scs$accept,
	getname:	dn_getname,
	poll:		dn_poll,
	ioctl:		dn_ioctl,
	listen:		scs$listen,
	shutdown:	dn_shutdown,
	setsockopt:	dn_setsockopt,
	getsockopt:	dn_getsockopt,
	sendmsg:	scs_std$sendmsg,
	recvmsg:	dn_recvmsg,
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

static char banner[] __initdata = KERN_INFO "NET5: MYSCS, based on DECnet for Linux: V.2.4.15-pre5s (C) 1995-2001 Linux DECnet Project Team\n";

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
