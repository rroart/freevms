// $Id$
// $Locker$

// Author. Roar Thronæs.
// Author. Linux people.

#if 0
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

static struct _cdt **dn_find_list(struct _cdt *sk)
{
	struct _cdt *scp = sk;

	if (scp->addr.sdn_flags & SDF_WILD)
		return dn_wild_sk ? NULL : &dn_wild_sk;

	return &dn_sk_hash[scp->addrloc & DN_SK_HASH_MASK];
}

/* 
 * Valid ports are those greater than zero and not already in use.
 */
static int check_port(unsigned short port)
{
	struct _cdt *sk = dn_sk_hash[port & DN_SK_HASH_MASK];
	if (port == 0)
		return -1;
	while(sk) {
		struct _cdt *scp = sk;
		if (scp->addrloc == port)
			return -1;
		sk = sk->next;
	}
	return 0;
}

static unsigned short port_alloc(struct _cdt *sk)
{
	struct _cdt *scp = sk;
static unsigned short port = 0x2000;
	unsigned short i_port = port;

	while(check_port(++port) != 0) {
		if (port == i_port)
			return 0;
	}

	scp->addrloc = port;

	return 1;
}

/*
 * Since this is only ever called from user
 * level, we don't need a write_lock() version
 * of this.
 */
static int dn_hash_sock(struct _cdt *sk)
{
	struct _cdt *scp = sk;
	struct _cdt **skp;
	int rv = -EUSERS;

	if (sk->next)
		BUG();
	if (sk->pprev)
		BUG();

	write_lock_bh(&dn_hash_lock);
	
	if (!scp->addrloc && !port_alloc(sk))
		goto out;

	rv = -EADDRINUSE;
	if ((skp = dn_find_list(sk)) == NULL)
		goto out;

	sk->next = *skp;
	sk->pprev = skp;
	*skp = sk;
	rv = 0;
out:
	write_unlock_bh(&dn_hash_lock);
	return rv;
}

static void dn_unhash_sock(struct _cdt *sk)
{
	struct _cdt **skp = sk->pprev;

	if (skp == NULL)
		return;

	write_lock(&dn_hash_lock);
	while(*skp != sk)
		skp = &((*skp)->next);
	*skp = sk->next;
	write_unlock(&dn_hash_lock);

	sk->next = NULL;
	sk->pprev = NULL;
}

static void dn_unhash_sock_bh(struct _cdt *sk)
{
	struct _cdt **skp = sk->pprev;

	if (skp == NULL)
		return;

	write_lock_bh(&dn_hash_lock);
	while(*skp != sk)
		skp = &((*skp)->next);
	*skp = sk->next;
	write_unlock_bh(&dn_hash_lock);

	sk->next = NULL;
	sk->pprev = NULL;
}

struct _cdt **listen_hash(struct _cdtaddr_dn *addr)
{
	int i;
	unsigned hash = addr->sdn_objnum;

	if (hash == 0) {
		hash = addr->sdn_objnamel;
		for(i = 0; i < addr->sdn_objnamel; i++) {
			hash ^= addr->sdn_objname[i];
			hash ^= (hash << 3);
		}
	}

	return &dn_sk_hash[hash & DN_SK_HASH_MASK];
}

/*
 * Called to transform a socket from bound (i.e. with a local address)
 * into a listening socket (doesn't need a local port number) and rehashes
 * based upon the object name/number.
 */
static void dn_rehash_sock(struct _cdt *sk)
{
	struct _cdt **skp = sk->pprev;
	struct _cdt *scp = sk;

	if (scp->addr.sdn_flags & SDF_WILD)
		return;

	write_lock_bh(&dn_hash_lock);
	while(*skp != sk)
		skp = &((*skp)->next);
	*skp = sk->next;

	sk->addrloc = 0;
	skp = listen_hash(&sk->addr);

	sk->next = *skp;
	sk->pprev = skp;
	*skp = sk;
	write_unlock_bh(&dn_hash_lock);
}

int dn_sockaddr2username(struct _cdtaddr_dn *sdn, unsigned char *buf, unsigned char type)
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
int dn_username2sockaddr(unsigned char *data, int len, struct _cdtaddr_dn *sdn, unsigned char *fmt)
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

struct _cdt *dn_sklist_find_listener(struct _cdtaddr_dn *addr)
{
	struct _cdt **skp = listen_hash(addr);
	struct _cdt *sk;

	read_lock(&dn_hash_lock);
	for(sk = *skp; sk != NULL; sk = sk->next) {
		struct _cdt *scp = sk;
		if (sk->cdt$w_state != CDT$C_LISTEN)
			continue;
		if (scp->addr.sdn_objnum) {
			if (scp->addr.sdn_objnum != addr->sdn_objnum)
				continue;
		} else {
			if (addr->sdn_objnum)
				continue;
			if (scp->addr.sdn_objnamel != addr->sdn_objnamel)
				continue;
			if (memcmp(scp->addr.sdn_objname, addr->sdn_objname, dn_ntohs(addr->sdn_objnamel)) != 0)
				continue;
		}
		sock_hold(sk);
		read_unlock(&dn_hash_lock);
		return sk;
	}

	if (dn_wild_sk && (dn_wild_sk->cdt$w_state == CDT$C_LISTEN))
		sock_hold((sk = dn_wild_sk));

	read_unlock(&dn_hash_lock);
	return sk;
}

struct _cdt *dn_find_by_skb(struct sk_buff *skb)
{
	struct _cdt *cb = (skb);
	struct _cdt *sk;
	struct _cdt *scp;

	read_lock(&dn_hash_lock);
	sk = dn_sk_hash[cb->dst_port & DN_SK_HASH_MASK];
	for (; sk != NULL; sk = sk->next) {
		scp = sk;
		if (cb->src != dn_saddr2dn(&scp->peer))
			continue;
		if (cb->dst_port != scp->addrloc)
			continue;
		if (scp->addrrem && (cb->src_port != scp->addrrem))
			continue;
		break;
	}

	if (sk)
		sock_hold(sk);

	read_unlock(&dn_hash_lock);

	return sk;
}



static void dn_destruct(struct _cdt *sk)
{
	struct _cdt *scp = sk;

	skb_queue_purge(&scp->cdt$l_waitqfl);
	skb_queue_purge(&scp->cdt$l_waitqfl);
	skb_queue_purge(&scp->cdt$l_share_flink);

	dst_release(xchg(&sk->dst_cache, NULL));

	MOD_DEC_USE_COUNT;
}

struct _cdt *dn_alloc_sock(struct _cdt *sock, int gfp)
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

	sk->backlog_rcv = dn_nsp_backlog_rcv;
	sk->destruct    = dn_destruct;
	sk->no_check    = 1;
	sk->family      = PF_DECnet;
	sk->protocol    = 0;
	sk->allocation  = gfp;

	/* Initialization of DECnet Session Control Port		*/
	scp->cdt$w_state	= CDT$C_OPEN;		/* Open			*/
	scp->numdat	= 1;		/* Next data seg to tx	*/
	scp->numoth	= 1;		/* Next oth data to tx  */
	scp->ackxmt_dat = 0;		/* Last data seg ack'ed */
	scp->ackxmt_oth = 0;		/* Last oth data ack'ed */
	scp->ackrcv_dat = 0;		/* Highest data ack recv*/
	scp->ackrcv_oth = 0;		/* Last oth data ack rec*/
        scp->flowrem_sw = DN_SEND;
	scp->flowloc_sw = DN_SEND;
	scp->flowrem_dat = 0;
	scp->flowrem_oth = 1;
	scp->flowloc_dat = 0;
	scp->flowloc_oth = 1;
	scp->services_rem = 0;
	scp->services_loc = 1 | NSP_FC_NONE;
	scp->info_rem = 0;
	scp->info_loc = 0x03; /* NSP version 4.1 */
	scp->segsize_rem = 230; /* Default: Updated by remote segsize */
	scp->segsize_loc = 1450; /* Best guess for ethernet */
	scp->nonagle = 0;
	scp->multi_ireq = 1;
	scp->accept_mode = ACC_IMMED;
	scp->addr.sdn_family    = AF_DECnet;
	scp->peer.sdn_family    = AF_DECnet;
	scp->accessdata.acc_accl = 5;
	memcpy(scp->accessdata.acc_acc, "LINUX", 5);

	scp->max_window   = NSP_MAX_WINDOW;
	scp->snd_window   = NSP_MIN_WINDOW;
	scp->nsp_srtt     = NSP_INITIAL_SRTT;
	scp->nsp_rttvar   = NSP_INITIAL_RTTVAR;
	scp->nsp_rxtshift = 0;

	skb_queue_head_init(&scp->cdt$l_waitqfl);
	skb_queue_head_init(&scp->cdt$l_waitqfl);
	skb_queue_head_init(&scp->cdt$l_share_flink);

	scp->persist = 0;
	scp->persist_fxn = NULL;
	scp->keepalive = 10 * HZ;
	scp->keepalive_fxn = dn_keepalive;

	init_timer(&scp->delack_timer);
	scp->delack_pending = 0;
	scp->delack_fxn = dn_nsp_delayed_ack;

	dn_start_slow_timer(sk);

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
		dn_nsp_send_link(sk, DN_NOCHANGE, 0);
}


/*
 * Timer for shutdown/destroyed sockets.
 * When socket is dead & no packets have been sent for a
 * certain amount of time, they are removed by this
 * routine. Also takes care of sending out DI & DC
 * frames at correct times.
 */
int dn_destroy_timer(struct _cdt *sk)
{
	struct _cdt *scp = sk;

	scp->persist = dn_nsp_persist(sk);

	switch(scp->cdt$w_state) {
		case DN_DI:
			dn_nsp_send_disc(sk, NSP_DISCINIT, 0, GFP_ATOMIC);
			if (scp->nsp_rxtshift >= decnet_di_count)
				scp->cdt$w_state = DN_CN;
			return 0;

		case DN_DR:
			dn_nsp_send_disc(sk, NSP_DISCINIT, 0, GFP_ATOMIC);
			if (scp->nsp_rxtshift >= decnet_dr_count)
				scp->cdt$w_state = DN_DRC;
			return 0;

		case DN_DN:
			if (scp->nsp_rxtshift < decnet_dn_count) {
				/* printk(KERN_DEBUG "dn_destroy_timer: DN\n"); */
				dn_nsp_send_disc(sk, NSP_DISCCONF, NSP_REASON_DC, GFP_ATOMIC);
				return 0;
			}
	}

	scp->persist = (HZ * decnet_time_wait);

	if (sk->socket)
		return 0;

	dn_stop_fast_timer(sk); /* unlikely, but possible that this is runninng */
	if ((jiffies - scp->stamp) >= (HZ * decnet_time_wait)) {
		dn_unhash_sock(sk);
		sock_put(sk);
		return 1;
	}

	return 0;
}

static void dn_destroy_sock(struct _cdt *sk)
{
	struct _cdt *scp = sk;

	scp->nsp_rxtshift = 0; /* reset back off */

	if (sk->socket) {
		if (sk->socket->cdt$w_state != SS_UNCONNECTED)
			sk->socket->cdt$w_state = SS_DISCONNECTING;
	}

	sk->cdt$w_state = CDT$C_CLOSED;

	switch(scp->cdt$w_state) {
		case DN_DN:
			dn_nsp_send_disc(sk, NSP_DISCCONF, NSP_REASON_DC, sk->allocation);
			scp->persist_fxn = dn_destroy_timer;
			scp->persist = dn_nsp_persist(sk);
			break;
		case DN_CR:
			scp->cdt$w_state = DN_DR;
			goto disc_reject;
		case DN_RUN:
			scp->cdt$w_state = DN_DI;
		case DN_DI:
		case DN_DR:
disc_reject:
			dn_nsp_send_disc(sk, NSP_DISCINIT, 0, sk->allocation);
		case DN_NC:
		case DN_NR:
		case DN_RJ:
		case DN_DIC:
		case DN_CN:
		case DN_DRC:
		case DN_CI:
		case DN_CD:
			scp->persist_fxn = dn_destroy_timer;
			scp->persist = dn_nsp_persist(sk);
			break;
		default:
			printk(KERN_DEBUG "DECnet: dn_destroy_sock passed socket in invalid state\n");
		case CDT$C_OPEN:
			dn_stop_fast_timer(sk);
			dn_stop_slow_timer(sk);

			dn_unhash_sock_bh(sk);
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
		case DN_CR:
			return "  CR";
		case DN_DR:
			return "  DR";
		case DN_DRC:
			return " DRC";
		case DN_CC:
			return "  CC";
		case DN_CI:
			return "  CI";
		case DN_NR:
			return "  NR";
		case DN_NC:
			return "  NC";
		case DN_CD:
			return "  CD";
		case DN_RJ:
			return "  RJ";
		case DN_RUN:
			return " RUN";
		case DN_DI:
			return "  DI";
		case DN_DIC:
			return " DIC";
		case DN_DN:
			return "  DN";
		case CDT$C_CLOSED:
			return "  CL";
		case DN_CN:
			return "  CN";
	}

	return "????";
}

static int dn_create(struct _cdt *sock, int protocol)
{
	struct _cdt *sk;

	switch(sock->type) {
		case SOCK_SEQPACKET:
			if (protocol != DNPROTO_NSP)
				return -EPROTONOSUPPORT;
			break;
		case SOCK_STREAM:
			break;
		default:
			return -ESOCKTNOSUPPORT;
	}


	if ((sk = dn_alloc_sock(sock, GFP_KERNEL)) == NULL) 
		return -ENOBUFS;

	sk->protocol = protocol;

	return 0;
}


static int
dn_release(struct _cdt *sock)
{
	struct _cdt *sk = sock->sk;

	if (sk) {
		sock_orphan(sk);
		sock_hold(sk);
		lock_sock(sk);
		dn_destroy_sock(sk);
		release_sock(sk);
		sock_put(sk);
	}

        return 0;
}

static int dn_bind(struct _cdt *sock, struct _cdtaddr *uaddr, int addr_len)
{
	struct _cdt *sk = sock->sk;
	struct _cdt *scp = sk;
	struct _cdtaddr_dn *saddr = (struct _cdtaddr_dn *)uaddr;
	struct net_device *dev;
	int rv;

	if (sk->zapped == 0)
		return -EINVAL;

	if (addr_len != sizeof(struct _cdtaddr_dn))
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


	memcpy(&scp->addr, saddr, addr_len);
	sk->zapped = 0;

	if ((rv = dn_hash_sock(sk)) != 0)
		sk->zapped = 1;

        return rv;
}


static int dn_auto_bind(struct _cdt *sock)
{
	struct _cdt *sk = sock->sk;
	struct _cdt *scp = sk;

	sk->zapped = 0;

	scp->addr.sdn_flags  = 0;
	scp->addr.sdn_objnum = 0;

	/*
	 * This stuff is to keep compatibility with Eduardo's
	 * patch. I hope I can dispense with it shortly...
	 */
	if ((scp->accessdata.acc_accl != 0) &&
		(scp->accessdata.acc_accl <= 12)) {
	
		scp->addr.sdn_objnamel = dn_htons(scp->accessdata.acc_accl);
		memcpy(scp->addr.sdn_objname, scp->accessdata.acc_acc, dn_ntohs(scp->addr.sdn_objnamel));

		scp->accessdata.acc_accl = 0;
		memset(scp->accessdata.acc_acc, 0, 40);
	}

	scp->addr.sdn_add.a_len = dn_htons(2);
	*(dn_address *)scp->addr.sdn_add.a_addr = decnet_address;

	dn_hash_sock(sk);

	return 0;
}


static int dn_connect(struct _cdt *sock, struct _cdtaddr *uaddr, int addr_len, int flags)
{
	struct _cdtaddr_dn *addr = (struct _cdtaddr_dn *)uaddr;
	struct _cdt *sk = sock->sk;
	struct _cdt *scp = sk;
	int err = -EISCONN;

	lock_sock(sk);

	if (sock->cdt$w_state == SS_CONNECTED) 
		goto out;

	if (sock->cdt$w_state == SS_CONNECTING) {
		err = 0;
		if (sk->cdt$w_state == TCP_ESTABLISHED)
			goto out;

		err = -ECONNREFUSED;
		if (sk->cdt$w_state == CDT$C_CLOSED)
			goto out;
	}

	err = -EINVAL;
	if (sk->cdt$w_state != CDT$C_OPEN)
		goto out;

	if (addr_len != sizeof(struct _cdtaddr_dn))
		goto out;

	if (addr->sdn_family != AF_DECnet)
		goto out;

	if (addr->sdn_flags & SDF_WILD)
		goto out;

	err = -EADDRNOTAVAIL;
	if (sk->zapped && (err = dn_auto_bind(sock)))
		goto out;

	memcpy(&scp->peer, addr, addr_len);

	err = -EHOSTUNREACH;
	if (dn_route_output(&sk->dst_cache, dn_saddr2dn(&scp->peer), dn_saddr2dn(&scp->addr), 0) < 0)
		goto out;

	sk->cdt$w_state   = TCP_SYN_SENT;
	sock->cdt$w_state = SS_CONNECTING;
	sk->cdt$w_state = DN_CI;

	dn_nsp_send_conninit(sk, NSP_CI);

	err = -EINPROGRESS;
	if ((sk->cdt$w_state == TCP_SYN_SENT) && (flags & O_NONBLOCK))
		goto out;

	while(sk->cdt$w_state == TCP_SYN_SENT) {

		err = -ERESTARTSYS;
		if (signal_pending(current))
			goto out;

		if ((err = sock_error(sk)) != 0) {
			sock->cdt$w_state = SS_UNCONNECTED;
			goto out;
		}

		SOCK_SLEEP_PRE(sk);

		if (sk->cdt$w_state == TCP_SYN_SENT)
			schedule();

		SOCK_SLEEP_POST(sk);
	}

	if (sk->cdt$w_state != TCP_ESTABLISHED) {
		sock->cdt$w_state = SS_UNCONNECTED;
		err = sock_error(sk);
		goto out;
	}

	err = 0;
	sock->cdt$w_state = SS_CONNECTED;
out:
	release_sock(sk);

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
        struct _cdt *sk = sock->sk;

        while(sk->cdt$w_state == CDT$C_LISTEN) {
                if (flags & O_NONBLOCK) {
                        return -EAGAIN;
                }

		SOCK_SLEEP_PRE(sk)

		if (sk->cdt$w_state == CDT$C_LISTEN)
			schedule();

		SOCK_SLEEP_POST(sk)

                if (signal_pending(current))
                        return -ERESTARTSYS; /* But of course you don't! */
        }

        if ((sk->cdt$w_state != DN_RUN) && (sk->cdt$w_state != DN_DRC)) {
                sock->cdt$w_state = SS_UNCONNECTED;
                return sock_error(sk);
        }

	sock->cdt$w_state = SS_CONNECTED;

        return 0;
}


static int dn_accept(struct _cdt *sock, struct _cdt *newsock, int flags)
{
	struct _cdt *sk = sock->sk, *newsk;
	struct sk_buff *skb = NULL;
	struct _cdt *cb;
	unsigned char menuver;
	int err = 0;
	unsigned char type;

	lock_sock(sk);

        if (sk->cdt$w_state != CDT$C_LISTEN) {
		release_sock(sk);
		return -EINVAL;
	}

	if (sk->cdt$w_state != CDT$C_OPEN) {
		release_sock(sk);
		return -EINVAL;
	}

        do
        {
                if ((skb = skb_dequeue(&sk->receive_queue)) == NULL)
                {
                        if (flags & O_NONBLOCK)
                        {
                                release_sock(sk);
                                return -EAGAIN;
                        }

			SOCK_SLEEP_PRE(sk);

			if (!skb_peek(&sk->receive_queue))
				schedule();

			SOCK_SLEEP_POST(sk);

                        if (signal_pending(current))
                        {
				release_sock(sk);
                                return -ERESTARTSYS;
                        }
                }
        } while (skb == NULL);

	cb = (skb);

	if ((newsk = dn_alloc_sock(newsock, sk->allocation)) == NULL) {
		release_sock(sk);
		kfree_skb(skb);
		return -ENOBUFS;
	}
	sk->ack_backlog--;
	release_sock(sk);

	dst_release(xchg(&newsk->dst_cache, skb->dst));
	skb->dst = NULL;

        DN_SK(newsk)->cdt$w_state        = DN_CR;
	DN_SK(newsk)->addrrem      = cb->src_port;
	DN_SK(newsk)->services_rem = cb->services;
	DN_SK(newsk)->info_rem     = cb->info;
	DN_SK(newsk)->segsize_rem  = cb->segsize;
	DN_SK(newsk)->accept_mode  = sk->accept_mode;
	
	if (DN_SK(newsk)->segsize_rem < 230)
		DN_SK(newsk)->segsize_rem = 230;

	if ((DN_SK(newsk)->services_rem & NSP_FC_MASK) == NSP_FC_NONE)
		DN_SK(newsk)->max_window = decnet_no_fc_max_cwnd;

	newsk->cdt$w_state  = CDT$C_LISTEN;
	newsk->zapped = 0;

	memcpy(&(DN_SK(newsk)->addr), &(sk->addr), sizeof(struct _cdtaddr_dn));

	/*
	 * If we are listening on a wild socket, we don't want
	 * the newly created socket on the wrong hash queue.
	 */
	DN_SK(newsk)->addr.sdn_flags &= ~SDF_WILD;

	skb_pull(skb, dn_username2sockaddr(skb->data, skb->len, &(DN_SK(newsk)->addr), &type));
	skb_pull(skb, dn_username2sockaddr(skb->data, skb->len, &(DN_SK(newsk)->peer), &type));
	*(dn_address *)(DN_SK(newsk)->peer.sdn_add.a_addr) = cb->src;
	*(dn_address *)(DN_SK(newsk)->addr.sdn_add.a_addr) = cb->dst;

	menuver = *skb->data;
	skb_pull(skb, 1);

	if (menuver & DN_MENUVER_ACC)
		dn_access_copy(skb, &(DN_SK(newsk)->accessdata));

	if (menuver & DN_MENUVER_USR)
		dn_user_copy(skb, &(DN_SK(newsk)->conndata_in));

	if (menuver & DN_MENUVER_PRX)
		DN_SK(newsk)->peer.sdn_flags |= SDF_PROXY;

	if (menuver & DN_MENUVER_UIC)
		DN_SK(newsk)->peer.sdn_flags |= SDF_UICPROXY;

	kfree_skb(skb);

	memcpy(&(DN_SK(newsk)->conndata_out), &(sk->conndata_out),
		sizeof(struct optdata_dn));
	memcpy(&(DN_SK(newsk)->discdata_out), &(sk->discdata_out),
		sizeof(struct optdata_dn));

	lock_sock(newsk);
	/*
	 * FIXME: This can fail if we've run out of local ports....
	 */
	dn_hash_sock(newsk);

	dn_send_conn_ack(newsk);

	/*
	 * Here we use sk->allocation since although the conn conf is
	 * for the newsk, the context is the old socket.
	 */
	if (DN_SK(newsk)->accept_mode == ACC_IMMED) {
		DN_SK(newsk)->cdt$w_state = DN_CC;
        	dn_send_conn_conf(newsk, sk->allocation);
		err = dn_wait_accept(newsock, flags);
	}

	release_sock(newsk);
        return err;
}


static int dn_getname(struct _cdt *sock, struct _cdtaddr *uaddr,int *uaddr_len,int peer)
{
	struct _cdtaddr_dn *sa = (struct _cdtaddr_dn *)uaddr;
	struct _cdt *sk = sock->sk;
	struct _cdt *scp = sk;

	*uaddr_len = sizeof(struct _cdtaddr_dn);

	lock_sock(sk);

	if (peer) {
		if ((sock->cdt$w_state != SS_CONNECTED && 
		     sock->cdt$w_state != SS_CONNECTING) && 
		    scp->accept_mode == ACC_IMMED)
			return -ENOTCONN;

		memcpy(sa, &scp->peer, sizeof(struct _cdtaddr_dn));
	} else {
		memcpy(sa, &scp->addr, sizeof(struct _cdtaddr_dn));
	}

	release_sock(sk);

        return 0;
}


static unsigned int dn_poll(struct file *file, struct _cdt *sock, poll_table  *wait)
{
	struct _cdt *sk = sock->sk;
	struct _cdt *scp = sk;
	int mask = datagram_poll(file, sock, wait);

	if (skb_queue_len(&scp->cdt$l_share_flink))
		mask |= POLLRDBAND;

	return mask;
}

static int dn_ioctl(struct _cdt *sock, unsigned int cmd, unsigned long arg)
{
	struct _cdt *sk = sock->sk;
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
		lock_sock(sk);
		val = (skb_queue_len(&scp->cdt$l_share_flink) != 0);
		if (scp->cdt$w_state != DN_RUN)
			val = -ENOTCONN;
		release_sock(sk);
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
		amount = sk->sndbuf - atomic_read(&sk->wmem_alloc);
		if (amount < 0)
			amount = 0;
		err = put_user(amount, (int *)arg);
		break;

	case TIOCINQ:
		lock_sock(sk);
		if ((skb = skb_peek(&scp->cdt$l_share_flink)) != NULL) {
			amount = skb->len;
		} else {
			struct sk_buff *skb = sk->receive_queue.next;
			for(;;) {
				if (skb == (struct sk_buff *)&sk->receive_queue)
					break;
				amount += skb->len;
				skb = skb->next;
			}
		}
		release_sock(sk);
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

void * register_name(char * c1, char * c2) {
  struct _sbnb * s=vmalloc(sizeof(struct _sbnb));
  bzero(s,sizeof(struct _sbnb));

  bcopy(c1,&s->sbnb$b_procnam,max(16,strlen(c1)));

  insque(s,scs$gq_local_names);

  return s;
}

//static int scs$listen(struct _cdt *sock, int backlog)
//sock is about-ish lprnam;

int scs$listen (void (*msgadr)(void *msg_buf, struct _cdt *cdt, struct _pdt *pdt ),
		    void (*erradr)( unsigned int err_status, unsigned int reason, struct _cdt *cdt, struct _pdt *pdt), 
		    void *lprnam, 
		    void *prinfo)
{
  int err = -EINVAL;

  struct _cdt *sk = find_free_cdt();
  //lock_sock(sk);

  //sk->cdt$l_msginput=msgadr;
  //sk->cdt$l_erraddr=erradr;

  register_name(lprnam,prinfo);

  //	lock_sock(sk);

  //#if 0
  //if (sk->zapped)
  //	goto out;

  //if ((sk->cdt$w_state != CDT$C_OPEN) || (sk->cdt$w_state == CDT$C_LISTEN))
  //	goto out;
	//#endif

	//sk->max_ack_backlog = backlog;
	//sk->ack_backlog     = 0;
	//	sk->cdt$w_state           = CDT$C_LISTEN;
	err                 = 0;
	dn_rehash_sock(sk);

out:
	//release_sock(sk);

        return err;
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
    cdtl[i].cdt$l_link=&cdtl[i+i];
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

  scs$listen(dir_listen,mydirerr,myname,myinfo);

}

//#if 0
static int dn_shutdown(struct _cdt *sock, int how)
{
	struct _cdt *sk = sock->sk;
	struct _cdt *scp = sk;
	int err = -ENOTCONN;

	lock_sock(sk);

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


	sk->shutdown = how;
	dn_destroy_sock(sk);
	err = 0;

out:
	release_sock(sk);

	return err;
}

static int dn_setsockopt(struct _cdt *sock, int level, int optname, char *optval, int optlen)
{
	struct _cdt *sk = sock->sk;
	int err;

	lock_sock(sk);
	err = __dn_setsockopt(sock, level, optname, optval, optlen, 0);
	release_sock(sk);

	return err;
}

static int __dn_setsockopt(struct _cdt *sock, int level,int optname, char *optval, int optlen, int flags) 
{
	struct	sock *sk = sock->sk;
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
			if ((scp->cdt$w_state != CDT$C_OPEN) && (scp->cdt$w_state != DN_CR))
				return -EINVAL;

			if (optlen != sizeof(struct optdata_dn))
				return -EINVAL;

			if (u.opt.opt_optl > 16)
				return -EINVAL;

			memcpy(&scp->conndata_out, &u.opt, optlen);
			break;

		case DSO_DISDATA:
	   	        if (sock->cdt$w_state != SS_CONNECTED && scp->accept_mode == ACC_IMMED)
				return -ENOTCONN;

			if (optlen != sizeof(struct optdata_dn))
				return -EINVAL;

			if (u.opt.opt_optl > 16)
				return -EINVAL;

			memcpy(&scp->discdata_out, &u.opt, optlen);
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

			memcpy(&scp->accessdata, &u.acc, optlen);
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

			scp->accept_mode = (unsigned char)u.mode;
			break;

		case DSO_CONACCEPT:

			if (scp->cdt$w_state != DN_CR)
				return -EINVAL;

			scp->cdt$w_state = DN_CC;
			dn_send_conn_conf(sk, sk->allocation);
			err = dn_wait_accept(sock, sock->file->f_flags);
			return err;

		case DSO_CONREJECT:

			if (scp->cdt$w_state != DN_CR)
				return -EINVAL;

			scp->cdt$w_state = DN_DR;
			sk->shutdown = SHUTDOWN_MASK;
			dn_nsp_send_disc(sk, 0x38, 0, sk->allocation);
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
			scp->max_window = u.win;
			if (scp->snd_window > u.win)
				scp->snd_window = u.win;
			break;

		case DSO_NODELAY:
			if (optlen != sizeof(int))
				return -EINVAL;
			if (scp->nonagle == 2)
				return -EINVAL;
			scp->nonagle = (u.val == 0) ? 0 : 1;
			/* if (scp->nonagle == 1) { Push pending frames } */
			break;

		case DSO_CORK:
			if (optlen != sizeof(int))
				return -EINVAL;
			if (scp->nonagle == 1)
				return -EINVAL;
			scp->nonagle = (u.val == 0) ? 0 : 2;
			/* if (scp->nonagle == 0) { Push pending frames } */
			break;

		case DSO_SERVICES:
			if (optlen != sizeof(unsigned char))
				return -EINVAL;
			if ((u.services & ~NSP_FC_MASK) != 0x01)
				return -EINVAL;
			if ((u.services & NSP_FC_MASK) == NSP_FC_MASK)
				return -EINVAL;
			scp->services_loc = u.services;
			break;

		case DSO_INFO:
			if (optlen != sizeof(unsigned char))
				return -EINVAL;
			if (u.info & 0xfc)
				return -EINVAL;
			scp->info_loc = u.info;
			break;
	}

	return 0;
}

static int dn_getsockopt(struct _cdt *sock, int level, int optname, char *optval, int *optlen)
{
	struct _cdt *sk = sock->sk;
	int err;

	lock_sock(sk);
	err = __dn_getsockopt(sock, level, optname, optval, optlen, 0);
	release_sock(sk);

	return err;
}

static int __dn_getsockopt(struct _cdt *sock, int level,int optname, char *optval,int *optlen, int flags)
{
	struct	sock *sk = sock->sk;
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
			r_data = &scp->conndata_in;
			break;

		case DSO_DISDATA:
			if (r_len > sizeof(struct optdata_dn))
				r_len = sizeof(struct optdata_dn);
			r_data = &scp->discdata_in;
			break;

		case DSO_CONACCESS:
			if (r_len > sizeof(struct accessdata_dn))
				r_len = sizeof(struct accessdata_dn);
			r_data = &scp->accessdata;
			break;

		case DSO_ACCEPTMODE:
			if (r_len > sizeof(unsigned char))
				r_len = sizeof(unsigned char);
			r_data = &scp->accept_mode;
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

			link.idn_segsize = scp->segsize_rem;
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
			r_data = &scp->max_window;
			break;

		case DSO_NODELAY:
			if (r_len > sizeof(int))
				r_len = sizeof(int);
			val = (scp->nonagle == 1);
			r_data = &val;
			break;

		case DSO_CORK:
			if (r_len > sizeof(int))
				r_len = sizeof(int);
			val = (scp->nonagle == 2);
			r_data = &val;
			break;

		case DSO_SERVICES:
			if (r_len > sizeof(unsigned char))
				r_len = sizeof(unsigned char);
			r_data = &scp->services_rem;
			break;

		case DSO_INFO:
			if (r_len > sizeof(unsigned char))
				r_len = sizeof(unsigned char);
			r_data = &scp->info_rem;
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
		case DN_RUN:
			return 0;

		case DN_CR:
			scp->cdt$w_state = DN_CC;
			dn_send_conn_conf(sk, sk->allocation);
			return dn_wait_accept(sk->socket, (flags & MSG_DONTWAIT) ? O_NONBLOCK : 0);
		case DN_CI:
		case DN_CC:
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

		SOCK_SLEEP_PRE(sk)

		if (scp->cdt$w_state != DN_RUN)
			schedule();

		SOCK_SLEEP_POST(sk)

	} while(scp->cdt$w_state != DN_RUN);

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

		if (cb->nsp_flags & 0x40) {
			/* SOCK_SEQPACKET reads to EOM */
			if (sk->type == SOCK_SEQPACKET)
				return 1;
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
	struct _cdt *sk = sock->sk;
	struct _cdt *scp = sk;
	struct sk_buff_head *queue = &sk->receive_queue;
	int target = size > 1 ? 1 : 0;
	int copied = 0;
	int rv = 0;
	struct sk_buff *skb, *nskb;
	struct _cdt *cb = NULL;
	unsigned char eor = 0;

	lock_sock(sk);

	if (sk->zapped) {
		rv = -EADDRNOTAVAIL;
		goto out;
	}

	if ((rv = dn_wait_run(sk, flags)) != 0)
		goto out;

	if (sk->shutdown & RCV_SHUTDOWN) {
		send_sig(SIGPIPE, current, 0);
		rv = -EPIPE;
		goto out;
	}

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
		if (sk->err)
			goto out;

		if (skb_queue_len(&scp->cdt$l_share_flink)) {
			if (!(flags & MSG_OOB)) {
				msg->msg_flags |= MSG_OOB;
				if (!scp->other_report) {
					scp->other_report = 1;
					goto out;
				}
			}
		}
		
		if (scp->cdt$w_state != DN_RUN)
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

		set_bit(SOCK_ASYNC_WAITDATA, &sock->flags);
		SOCK_SLEEP_PRE(sk)

		if (!dn_data_ready(sk, queue, flags, target))
			schedule();

		SOCK_SLEEP_POST(sk)
		clear_bit(SOCK_ASYNC_WAITDATA, &sock->flags);
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

		eor = cb->nsp_flags & 0x40;
		nskb = skb->next;

		if (skb->len == 0) {
			skb_unlink(skb);
			kfree_skb(skb);
			/* 
			 * N.B. Don't refer to skb or cb after this point
			 * in loop.
			 */
			if ((scp->flowloc_sw == DN_DONTSEND) && !dn_congested(sk)) {
				scp->flowloc_sw = DN_SEND;
				dn_nsp_send_link(sk, DN_SEND, 0);
			}
		}

		if (eor) { 
			if (sk->type == SOCK_SEQPACKET)
				break;
			if (!(flags & MSG_WAITALL))
				break;
		}

		if (flags & MSG_OOB)
			break;

		if (copied >= target)
			break;
	}

	rv = copied;


	if (eor && (sk->type == SOCK_SEQPACKET))
		msg->msg_flags |= MSG_EOR;

out:
	if (rv == 0)
		rv = (flags & MSG_PEEK) ? -sk->err : sock_error(sk);

	if ((rv >= 0) && msg->msg_name) {
		memcpy(msg->msg_name, &scp->peer, sizeof(struct _cdtaddr_dn));
		msg->msg_namelen = sizeof(struct _cdtaddr_dn);
	}

	release_sock(sk);

	return rv;
}


static inline int dn_queue_too_long(struct _cdt *scp, struct sk_buff_head *queue, int flags)
{
	unsigned char fctype = scp->services_rem & NSP_FC_MASK;
	if (skb_queue_len(queue) >= scp->snd_window)
		return 1;
	if (fctype != NSP_FC_NONE) {
		if (flags & MSG_OOB) {
			if (scp->flowrem_oth == 0)
				return 1;
		} else {
			if (scp->flowrem_dat == 0)
				return 1;
		}
	}
	return 0;
}

#if 0
static int dn_sendmsg(struct _cdt *sock, struct msghdr *msg, int size, 
	   struct scm_cookie *scm)
#endif
     static int dn_sendmsg(int msg_buf_len, struct _pdt *pdt_p, struct _cdrp *cdrp_p, void (*complete)())
{
	struct _cdt *sk cdrp_p->cdrp$l_cdt;
	struct *scp = sk;
	
	int mss;
	struct sk_buff_head *queue = &scp->cdt$l_waitqfl;
	int flags = msg->msg_flags;
	int err = 0;
	int sent = 0;
	int addr_len = msg->msg_namelen;
	struct _cdtaddr_dn *addr = (struct _cdtaddr_dn *)msg->msg_name;
	struct sk_buff *skb = NULL;
	struct _cdt *cb;
	unsigned char msgflg;
	unsigned char *ptr;
	unsigned short ack;
	int len;
	unsigned char fctype;

	if (flags & ~(MSG_TRYHARD|MSG_OOB|MSG_DONTWAIT|MSG_EOR))
		return -EOPNOTSUPP;

	if (addr_len && (addr_len != sizeof(struct _cdtaddr_dn)))
		return -EINVAL;

	if (scp->cdt$w_state == CDT$C_OPEN) {
		if (!addr_len || !addr) {
			err = -ENOTCONN;
			goto out;
		}

		if ((err = dn_connect(sock, (struct _cdtaddr *)addr, addr_len, (flags & MSG_DONTWAIT) ? O_NONBLOCK : 0)) < 0)
			goto out;
	}

	lock_sock(sk);

	if ((err = dn_wait_run(sk, flags)) < 0)
		goto out;

	mss = scp->segsize_rem;
	fctype = scp->services_rem & NSP_FC_MASK;

	if (sk->dst_cache && sk->dst_cache->neighbour) {
		struct dn_neigh *dn = (struct dn_neigh *)sk->dst_cache->neighbour;
		if (dn->blksize < (mss + 11))
			mss = dn->blksize - 11;
	}

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

			SOCK_SLEEP_PRE(sk)

			if (dn_queue_too_long(scp, queue, flags))
				schedule();

			SOCK_SLEEP_POST(sk)

			continue;
		}

		/*
		 * Get a suitably sized skb.
		 */
		skb = dn_alloc_send_skb(sk, &len, flags & MSG_DONTWAIT, &err);

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
			cb->segnum = scp->numoth;
			seq_add(&scp->numoth, 1);
			msgflg = 0x30;
			ack = (scp->numoth_rcv & 0x0FFF) | 0x8000;
			scp->ackxmt_oth = scp->numoth_rcv;
			if (fctype != NSP_FC_NONE)
				scp->flowrem_oth--;
		} else {
			cb->segnum = scp->numdat;
			seq_add(&scp->numdat, 1);
			msgflg = 0x00;
			if (sock->type == SOCK_STREAM)
				msgflg = 0x60;
			if (scp->seg_total == 0)
				msgflg |= 0x20;

			scp->seg_total += len;
		
			if (((sent + len) == size) && (flags & MSG_EOR)) {
				msgflg |= 0x40;
				scp->seg_total = 0;
				if (fctype == NSP_FC_SCMC)
					scp->flowrem_dat--;
			}
			ack = (scp->numdat_rcv & 0x0FFF) | 0x8000;
			scp->ackxmt_dat = scp->numdat_rcv;
			if (fctype == NSP_FC_SRC)
				scp->flowrem_dat--;
		}

		*ptr++ = msgflg;
		*(__u16 *)ptr = scp->addrrem;
		ptr += 2;
		*(__u16 *)ptr = scp->addrloc;
		ptr += 2;
		*(__u16 *)ptr = dn_htons(ack);
		ptr += 2;
		*(__u16 *)ptr = dn_htons(cb->segnum);

		sent += len;
		dn_nsp_queue_xmit(sk, skb, sk->allocation, flags & MSG_OOB);
		skb = NULL;

		scp->persist = dn_nsp_persist(sk);

	}
out:

	if (skb)
		kfree_skb(skb);

	release_sock(sk);

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

static void dn_printable_object(struct _cdtaddr_dn *dn, unsigned char *buf)
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
		for(sk = dn_sk_hash[i]; sk != NULL; sk = sk->next) {
			scp = sk;

			dn_printable_object(&scp->addr, local_object);
			dn_printable_object(&scp->peer, remote_object);

			len += sprintf(buffer + len,
					"%6s/%04X %04d:%04d %04d:%04d %01d %-16s %6s/%04X %04d:%04d %04d:%04d %01d %-16s %4s %s\n",
					dn_addr2asc(dn_ntohs(dn_saddr2dn(&scp->addr)), buf1),
					scp->addrloc,
					scp->numdat,
					scp->numoth,
					scp->ackxmt_dat,
					scp->ackxmt_oth,
					scp->flowloc_sw,
					local_object,
					dn_addr2asc(dn_ntohs(dn_saddr2dn(&scp->peer)), buf2),
					scp->addrrem,
					scp->numdat_rcv,
					scp->numoth_rcv,
					scp->ackrcv_dat,
					scp->ackrcv_oth,
					scp->flowrem_sw,
					remote_object,
					dn_state2asc(scp->cdt$w_state),
					((scp->accept_mode == ACC_IMMED) ? "IMMED" : "DEFER"));

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
	accept:		dn_accept,
	getname:	dn_getname,
	poll:		dn_poll,
	ioctl:		dn_ioctl,
	listen:		scs$listen,
	shutdown:	dn_shutdown,
	setsockopt:	dn_setsockopt,
	getsockopt:	dn_getsockopt,
	sendmsg:	dn_sendmsg,
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
