/*
 * INET     An implementation of the TCP/IP protocol suite for the LINUX
 *      operating system.  INET is implemented using the  BSD Socket
 *      interface as the means of communication with the user level.
 *
 *      Definitions for the AF_INET socket handler.
 *
 * Version: @(#)sock.h  1.0.4   05/13/93
 *
 * Authors: Ross Biro, <bir7@leland.Stanford.Edu>
 *      Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *      Corey Minyard <wf-rch!minyard@relay.EU.net>
 *      Florian La Roche <flla@stud.uni-sb.de>
 *
 * Fixes:
 *      Alan Cox    :   Volatiles in skbuff pointers. See
 *                  skbuff comments. May be overdone,
 *                  better to prove they can be removed
 *                  than the reverse.
 *      Alan Cox    :   Added a zapped field for tcp to note
 *                  a socket is reset and must stay shut up
 *      Alan Cox    :   New fields for options
 *  Pauline Middelink   :   identd support
 *      Alan Cox    :   Eliminate low level recv/recvfrom
 *      David S. Miller :   New socket lookup architecture.
 *              Steve Whitehouse:       Default routines for sock_ops
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */
#ifndef _SOCK_H
#define _SOCK_H

#include <linux/cache.h>
#include <linux/in.h>       /* struct sockaddr_in */

#include <linux/icmp.h>
#include <linux/tcp.h>      /* struct tcphdr */

#include <linux/netdevice.h>
#include <linux/skbuff.h>   /* struct sk_buff */

#if defined(CONFIG_DECNET) || defined(CONFIG_DECNET_MODULE)
#include <net/dn.h>
#endif

#include <net/dst.h>


/* The AF_UNIX specific socket options */
struct unix_opt
{
    struct unix_address *addr;
    struct dentry *     dentry;
    struct vfsmount *   mnt;
    struct semaphore    readsem;
    struct sock *       other;
    struct sock **      list;
    struct sock *       gc_tree;
    atomic_t        inflight;
    rwlock_t        lock;
    wait_queue_head_t   peer_wait;
};

#if defined(CONFIG_INET) || defined(CONFIG_INET_MODULE)
struct raw_opt
{
    struct icmp_filter  filter;
};
#endif

#if defined(CONFIG_INET) || defined (CONFIG_INET_MODULE)
struct inet_opt
{
    int         ttl;            /* TTL setting */
    int         tos;            /* TOS */
    unsigned        cmsg_flags;
    struct ip_options   *opt;
    unsigned char       hdrincl;        /* Include headers ? */
    __u8            mc_ttl;         /* Multicasting TTL */
    __u8            mc_loop;        /* Loopback */
    unsigned        recverr : 1,
                    freebind : 1;
    __u16           id;         /* ID counter for DF pkts */
    __u8            pmtudisc;
    int         mc_index;       /* Multicast device index */
    __u32           mc_addr;
    struct ip_mc_socklist   *mc_list;       /* Group array */
};
#endif

/* This defines a selective acknowledgement block. */
struct tcp_sack_block
{
    __u32   start_seq;
    __u32   end_seq;
};

struct tcp_opt
{
    int tcp_header_len; /* Bytes of tcp header to send      */

    /*
     *  Header prediction flags
     *  0x5?10 << 16 + snd_wnd in net byte order
     */
    __u32   pred_flags;

    /*
     *  RFC793 variables by their proper names. This means you can
     *  read the code and the spec side by side (and laugh ...)
     *  See RFC793 and RFC1122. The RFC writes these in capitals.
     */
    __u32   rcv_nxt;    /* What we want to receive next     */
    __u32   snd_nxt;    /* Next sequence we send        */

    __u32   snd_una;    /* First byte we want an ack for    */
    __u32   snd_sml;    /* Last byte of the most recently transmitted small packet */
    __u32   rcv_tstamp; /* timestamp of last received ACK (for keepalives) */
    __u32   lsndtime;   /* timestamp of last sent data packet (for restart window) */

    /* Delayed ACK control data */
    struct
    {
        __u8    pending;    /* ACK is pending */
        __u8    quick;      /* Scheduled number of quick acks   */
        __u8    pingpong;   /* The session is interactive       */
        __u8    blocked;    /* Delayed ACK was blocked by socket lock*/
        __u32   ato;        /* Predicted tick of soft clock     */
        unsigned long timeout;  /* Currently scheduled timeout      */
        __u32   lrcvtime;   /* timestamp of last received data packet*/
        __u16   last_seg_size;  /* Size of last incoming segment    */
        __u16   rcv_mss;    /* MSS used for delayed ACK decisions   */
    } ack;

    /* Data for direct copy to user */
    struct
    {
        struct sk_buff_head prequeue;
        int         memory;
        struct task_struct  *task;
        struct iovec        *iov;
        int         len;
    } ucopy;

    __u32   snd_wl1;    /* Sequence for window update       */
    __u32   snd_wnd;    /* The window we expect to receive  */
    __u32   max_window; /* Maximal window ever seen from peer   */
    __u32   pmtu_cookie;    /* Last pmtu seen by socket     */
    __u16   mss_cache;  /* Cached effective mss, not including SACKS */
    __u16   mss_clamp;  /* Maximal mss, negotiated at connection setup */
    __u16   ext_header_len; /* Network protocol overhead (IP/IPv6 options) */
    __u8    ca_state;   /* State of fast-retransmit machine     */
    __u8    retransmits;    /* Number of unrecovered RTO timeouts.  */

    __u8    reordering; /* Packet reordering metric.        */
    __u8    queue_shrunk;   /* Write queue has been shrunk recently.*/
    __u8    defer_accept;   /* User waits for some data after accept() */

    /* RTT measurement */
    __u8    backoff;    /* backoff              */
    __u32   srtt;       /* smothed round trip time << 3     */
    __u32   mdev;       /* medium deviation         */
    __u32   mdev_max;   /* maximal mdev for the last rtt period */
    __u32   rttvar;     /* smoothed mdev_max            */
    __u32   rtt_seq;    /* sequence number to update rttvar */
    __u32   rto;        /* retransmit timeout           */

    __u32   packets_out;    /* Packets which are "in flight"    */
    __u32   left_out;   /* Packets which leaved network     */
    __u32   retrans_out;    /* Retransmitted packets out        */


    /*
     *  Slow start and congestion control (see also Nagle, and Karn & Partridge)
     */
    __u32   snd_ssthresh;   /* Slow start size threshold        */
    __u32   snd_cwnd;   /* Sending congestion window        */
    __u16   snd_cwnd_cnt;   /* Linear increase counter      */
    __u16   snd_cwnd_clamp; /* Do not allow snd_cwnd to grow above this */
    __u32   snd_cwnd_used;
    __u32   snd_cwnd_stamp;

    /* Two commonly used timers in both sender and receiver paths. */
    unsigned long       timeout;
    struct timer_list   retransmit_timer;   /* Resend (no ack)  */
    struct timer_list   delack_timer;       /* Ack delay        */

    struct sk_buff_head out_of_order_queue; /* Out of order segments go here */

    struct tcp_func     *af_specific;   /* Operations which are AF_INET{4,6} specific   */
    struct sk_buff      *send_head; /* Front of stuff to transmit           */
    struct page     *sndmsg_page;   /* Cached page for sendmsg          */
    u32         sndmsg_off; /* Cached offset for sendmsg            */

    __u32   rcv_wnd;    /* Current receiver window      */
    __u32   rcv_wup;    /* rcv_nxt on last window update sent   */
    __u32   write_seq;  /* Tail(+1) of data held in tcp send buffer */
    __u32   pushed_seq; /* Last pushed seq, required to talk to windows */
    __u32   copied_seq; /* Head of yet unread data      */
    /*
     *      Options received (usually on last packet, some only on SYN packets).
     */
    char    tstamp_ok,  /* TIMESTAMP seen on SYN packet     */
            wscale_ok,  /* Wscale seen on SYN packet        */
            sack_ok;    /* SACK seen on SYN packet      */
    char    saw_tstamp; /* Saw TIMESTAMP on last packet     */
    __u8    snd_wscale; /* Window scaling received from sender  */
    __u8    rcv_wscale; /* Window scaling to send to receiver   */
    __u8    nonagle;    /* Disable Nagle algorithm?             */
    __u8    keepalive_probes; /* num of allowed keep alive probes   */

    /*  PAWS/RTTM data  */
    __u32   rcv_tsval;  /* Time stamp value                 */
    __u32   rcv_tsecr;  /* Time stamp echo reply            */
    __u32   ts_recent;  /* Time stamp to echo next      */
    long    ts_recent_stamp;/* Time we stored ts_recent (for aging) */

    /*  SACKs data  */
    __u16   user_mss;   /* mss requested by user in ioctl */
    __u8    dsack;      /* D-SACK is scheduled          */
    __u8    eff_sacks;  /* Size of SACK array to send with next packet */
    struct tcp_sack_block duplicate_sack[1]; /* D-SACK block */
    struct tcp_sack_block selective_acks[4]; /* The SACKS themselves*/

    __u32   window_clamp;   /* Maximal window to advertise      */
    __u32   rcv_ssthresh;   /* Current window clamp         */
    __u8    probes_out; /* unanswered 0 window probes       */
    __u8    num_sacks;  /* Number of SACK blocks        */
    __u16   advmss;     /* Advertised MSS           */

    __u8    syn_retries;    /* num of allowed syn retries */
    __u8    ecn_flags;  /* ECN status bits.         */
    __u16   prior_ssthresh; /* ssthresh saved at recovery start */
    __u32   lost_out;   /* Lost packets             */
    __u32   sacked_out; /* SACK'd packets           */
    __u32   fackets_out;    /* FACK'd packets           */
    __u32   high_seq;   /* snd_nxt at onset of congestion   */

    __u32   retrans_stamp;  /* Timestamp of the last retransmit,
                 * also used in SYN-SENT to remember stamp of
                 * the first SYN. */
    __u32   undo_marker;    /* tracking retrans started here. */
    int undo_retrans;   /* number of undoable retransmissions. */
    __u32   urg_seq;    /* Seq of received urgent pointer */
    __u16   urg_data;   /* Saved octet of OOB data and control flags */
    __u8    pending;    /* Scheduled timer event    */
    __u8    urg_mode;   /* In urgent mode       */
    __u32   snd_up;     /* Urgent pointer       */

    /* The syn_wait_lock is necessary only to avoid tcp_get_info having
     * to grab the main lock sock while browsing the listening hash
     * (otherwise it's deadlock prone).
     * This lock is acquired in read mode only from tcp_get_info() and
     * it's acquired in write mode _only_ from code that is actively
     * changing the syn_wait_queue. All readers that are holding
     * the master sock lock don't need to grab this lock in read mode
     * too as the syn_wait_queue writes are always protected from
     * the main sock lock.
     */
    rwlock_t        syn_wait_lock;
    struct tcp_listen_opt   *listen_opt;

    /* FIFO of established children */
    struct open_request *accept_queue;
    struct open_request *accept_queue_tail;

    int         write_pending;  /* A write to socket waits to start. */

    unsigned int        keepalive_time;   /* time before keep alive takes place */
    unsigned int        keepalive_intvl;  /* time interval between keep alive probes */
    int         linger2;

    unsigned long last_synq_overflow;
};


/*
 * This structure really needs to be cleaned up.
 * Most of it is for TCP, and not used by any of
 * the other protocols.
 */

/*
 * The idea is to start moving to a newer struct gradualy
 *
 * IMHO the newer struct should have the following format:
 *
 *  struct sock {
 *      sockmem [mem, proto, callbacks]
 *
 *      union or struct {
 *          ax25;
 *      } ll_pinfo;
 *
 *      union {
 *          ipv4;
 *          ipv6;
 *          ipx;
 *          netrom;
 *          rose;
 *          x25;
 *      } net_pinfo;
 *
 *      union {
 *          tcp;
 *          udp;
 *          spx;
 *          netrom;
 *      } tp_pinfo;
 *
 *  }
 *
 * The idea failed because IPv6 transition asssumes dual IP/IPv6 sockets.
 * So, net_pinfo is IPv6 are really, and protinfo unifies all another
 * private areas.
 */

/* Define this to get the sk->debug debugging facility. */
#define SOCK_DEBUGGING
#ifdef SOCK_DEBUGGING
#define SOCK_DEBUG(sk, msg...) do { if((sk) && ((sk)->debug)) printk(KERN_DEBUG msg); } while (0)
#else
#define SOCK_DEBUG(sk, msg...) do { } while (0)
#endif

/* This is the per-socket lock.  The spinlock provides a synchronization
 * between user contexts and software interrupt processing, whereas the
 * mini-semaphore synchronizes multiple users amongst themselves.
 */
typedef struct
{
    spinlock_t      slock;
    unsigned int        users;
    wait_queue_head_t   wq;
} socket_lock_t;

#define sock_lock_init(__sk) \
do {    spin_lock_init(&((__sk)->lock.slock)); \
    (__sk)->lock.users = 0; \
    init_waitqueue_head(&((__sk)->lock.wq)); \
} while(0);

struct sock
{
    /* Socket demultiplex comparisons on incoming packets. */
    __u32           daddr;      /* Foreign IPv4 addr            */
    __u32           rcv_saddr;  /* Bound local IPv4 addr        */
    __u16           dport;      /* Destination port         */
    unsigned short      num;        /* Local port               */
    int         bound_dev_if;   /* Bound device index if != 0       */

    /* Main hash linkage for various protocol lookup tables. */
    struct sock     *next;
    struct sock     **pprev;
    struct sock     *bind_next;
    struct sock     **bind_pprev;

    volatile unsigned char  state,      /* Connection state         */
             zapped;       /* In ax25 & ipx means not linked   */
    __u16           sport;      /* Source port              */

    unsigned short      family;     /* Address family           */
    unsigned char       reuse;      /* SO_REUSEADDR setting         */
    unsigned char       shutdown;
    atomic_t        refcnt;     /* Reference count          */

    socket_lock_t       lock;       /* Synchronizer...          */
    int         rcvbuf;     /* Size of receive buffer in bytes  */

    wait_queue_head_t   *sleep;     /* Sock wait queue          */
    struct dst_entry    *dst_cache; /* Destination cache            */
    rwlock_t        dst_lock;
    atomic_t        rmem_alloc; /* Receive queue bytes committed    */
    struct sk_buff_head receive_queue;  /* Incoming packets         */
    atomic_t        wmem_alloc; /* Transmit queue bytes committed   */
    struct sk_buff_head write_queue;    /* Packet sending queue         */
    atomic_t        omem_alloc; /* "o" is "option" or "other" */
    int         wmem_queued;    /* Persistent queue size */
    int         forward_alloc;  /* Space allocated forward. */
    __u32           saddr;      /* Sending source           */
    unsigned int        allocation; /* Allocation mode          */
    int         sndbuf;     /* Size of send buffer in bytes     */
    struct sock     *prev;

    /* Not all are volatile, but some are, so we might as well say they all are.
     * XXX Make this a flag word -DaveM
     */
    volatile char       dead,
             done,
             urginline,
             keepopen,
             linger,
             destroy,
             no_check,
             broadcast,
             bsdism;
    unsigned char       debug;
    unsigned char       rcvtstamp;
    unsigned char       use_write_queue;
    unsigned char       userlocks;
    /* Hole of 3 bytes. Try to pack. */
    int         route_caps;
    int         proc;
    unsigned long           lingertime;

    int         hashent;
    struct sock     *pair;

    /* The backlog queue is special, it is always used with
     * the per-socket spinlock held and requires low latency
     * access.  Therefore we special case it's implementation.
     */
    struct
    {
        struct sk_buff *head;
        struct sk_buff *tail;
    } backlog;

    rwlock_t        callback_lock;

    /* Error queue, rarely used. */
    struct sk_buff_head error_queue;

    struct proto        *prot;

    union
    {
        struct tcp_opt      af_tcp;
#if defined(CONFIG_INET) || defined (CONFIG_INET_MODULE)
        struct raw_opt      tp_raw4;
#endif
    } tp_pinfo;

    int         err, err_soft;  /* Soft holds errors that don't
                           cause failure but are the cause
                           of a persistent failure not just
                           'timed out' */
    unsigned short      ack_backlog;
    unsigned short      max_ack_backlog;
    __u32           priority;
    unsigned short      type;
    unsigned char       localroute; /* Route locally only */
    unsigned char       protocol;
    struct ucred        peercred;
    int         rcvlowat;
    long            rcvtimeo;
    long            sndtimeo;

    /* This is where all the private (optional) areas that don't
     * overlap will eventually live.
     */
    union
    {
        void *destruct_hook;
        struct unix_opt af_unix;
#if defined(CONFIG_INET) || defined (CONFIG_INET_MODULE)
        struct inet_opt af_inet;
#endif
#if defined (CONFIG_DECNET) || defined(CONFIG_DECNET_MODULE)
        struct dn_scp           dn;
#endif
#if defined (CONFIG_PACKET) || defined(CONFIG_PACKET_MODULE)
        struct packet_opt   *af_packet;
#endif
        struct netlink_opt  *af_netlink;
    } protinfo;


    /* This part is used for the timeout functions. */
    struct timer_list   timer;      /* This is the sock cleanup timer. */
    struct timeval      stamp;

    /* Identd and reporting IO signals */
    struct socket       *socket;

    /* RPC layer private data */
    void            *user_data;

    /* Callbacks */
    void            (*state_change)(struct sock *sk);
    void            (*data_ready)(struct sock *sk,int bytes);
    void            (*write_space)(struct sock *sk);
    void            (*error_report)(struct sock *sk);

    int         (*backlog_rcv) (struct sock *sk,
                                struct sk_buff *skb);
    void                    (*destruct)(struct sock *sk);
};

/* The per-socket spinlock must be held here. */
#define sk_add_backlog(__sk, __skb)         \
do {    if((__sk)->backlog.tail == NULL) {      \
        (__sk)->backlog.head =          \
             (__sk)->backlog.tail = (__skb);    \
    } else {                    \
        ((__sk)->backlog.tail)->next = (__skb); \
        (__sk)->backlog.tail = (__skb);     \
    }                       \
    (__skb)->next = NULL;               \
} while(0)

/* IP protocol blocks we attach to sockets.
 * socket layer -> transport layer interface
 * transport -> network interface is defined by struct inet_proto
 */
struct proto
{
    void            (*close)(struct sock *sk,
                             long timeout);
    int         (*connect)(struct sock *sk,
                           struct sockaddr *uaddr,
                           int addr_len);
    int         (*disconnect)(struct sock *sk, int flags);

    struct sock *       (*accept) (struct sock *sk, int flags, int *err);

    int         (*ioctl)(struct sock *sk, int cmd,
                         unsigned long arg);
    int         (*init)(struct sock *sk);
    int         (*destroy)(struct sock *sk);
    void            (*shutdown)(struct sock *sk, int how);
    int         (*setsockopt)(struct sock *sk, int level,
                              int optname, char *optval, int optlen);
    int         (*getsockopt)(struct sock *sk, int level,
                              int optname, char *optval,
                              int *option);
    int         (*sendmsg)(struct sock *sk, struct msghdr *msg,
                           int len);
    int         (*recvmsg)(struct sock *sk, struct msghdr *msg,
                           int len, int noblock, int flags,
                           int *addr_len);
    int         (*bind)(struct sock *sk,
                        struct sockaddr *uaddr, int addr_len);

    int         (*backlog_rcv) (struct sock *sk,
                                struct sk_buff *skb);

    /* Keeping track of sk's, looking them up, and port selection methods. */
    void            (*hash)(struct sock *sk);
    void            (*unhash)(struct sock *sk);
    int         (*get_port)(struct sock *sk, unsigned short snum);

    char            name[32];

    struct
    {
        int inuse;
        u8  __pad[SMP_CACHE_BYTES - sizeof(int)];
    } stats[NR_CPUS];
};

/* Called with local bh disabled */
static __inline__ void sock_prot_inc_use(struct proto *prot)
{
    prot->stats[smp_processor_id()].inuse++;
}

static __inline__ void sock_prot_dec_use(struct proto *prot)
{
    prot->stats[smp_processor_id()].inuse--;
}

/* About 10 seconds */
#define SOCK_DESTROY_TIME (10*HZ)

/* Sockets 0-1023 can't be bound to unless you are superuser */
#define PROT_SOCK   1024

#define SHUTDOWN_MASK   3
#define RCV_SHUTDOWN    1
#define SEND_SHUTDOWN   2

#define SOCK_SNDBUF_LOCK    1
#define SOCK_RCVBUF_LOCK    2
#define SOCK_BINDADDR_LOCK  4
#define SOCK_BINDPORT_LOCK  8


/* Used by processes to "lock" a socket state, so that
 * interrupts and bottom half handlers won't change it
 * from under us. It essentially blocks any incoming
 * packets, so that we won't get any new data or any
 * packets that change the state of the socket.
 *
 * While locked, BH processing will add new packets to
 * the backlog queue.  This queue is processed by the
 * owner of the socket lock right before it is released.
 *
 * Since ~2.3.5 it is also exclusive sleep lock serializing
 * accesses from user process context.
 */
extern void __lock_sock(struct sock *sk);
extern void __release_sock(struct sock *sk);
#define lock_sock(__sk) \
do {    spin_lock_bh(&((__sk)->lock.slock)); \
    if ((__sk)->lock.users != 0) \
        __lock_sock(__sk); \
    (__sk)->lock.users = 1; \
    spin_unlock_bh(&((__sk)->lock.slock)); \
} while(0)

#define release_sock(__sk) \
do {    spin_lock_bh(&((__sk)->lock.slock)); \
    if ((__sk)->backlog.tail != NULL) \
        __release_sock(__sk); \
    (__sk)->lock.users = 0; \
        if (waitqueue_active(&((__sk)->lock.wq))) wake_up(&((__sk)->lock.wq)); \
    spin_unlock_bh(&((__sk)->lock.slock)); \
} while(0)

/* BH context may only use the following locking interface. */
#define bh_lock_sock(__sk)  spin_lock(&((__sk)->lock.slock))
#define bh_unlock_sock(__sk)    spin_unlock(&((__sk)->lock.slock))

extern struct sock *        sk_alloc(int family, int priority, int zero_it);
extern void         sk_free(struct sock *sk);

/*
 * Socket reference counting postulates.
 *
 * * Each user of socket SHOULD hold a reference count.
 * * Each access point to socket (an hash table bucket, reference from a list,
 *   running timer, skb in flight MUST hold a reference count.
 * * When reference count hits 0, it means it will never increase back.
 * * When reference count hits 0, it means that no references from
 *   outside exist to this socket and current process on current CPU
 *   is last user and may/should destroy this socket.
 * * sk_free is called from any context: process, BH, IRQ. When
 *   it is called, socket has no references from outside -> sk_free
 *   may release descendant resources allocated by the socket, but
 *   to the time when it is called, socket is NOT referenced by any
 *   hash tables, lists etc.
 * * Packets, delivered from outside (from network or from another process)
 *   and enqueued on receive/error queues SHOULD NOT grab reference count,
 *   when they sit in queue. Otherwise, packets will leak to hole, when
 *   socket is looked up by one cpu and unhasing is made by another CPU.
 *   It is true for udp/raw, netlink (leak to receive and error queues), tcp
 *   (leak to backlog). Packet socket does all the processing inside
 *   BR_NETPROTO_LOCK, so that it has not this race condition. UNIX sockets
 *   use separate SMP lock, so that they are prone too.
 */

/* Grab socket reference count. This operation is valid only
   when sk is ALREADY grabbed f.e. it is found in hash table
   or a list and the lookup is made under lock preventing hash table
   modifications.
 */

static inline void sock_hold(struct sock *sk)
{
    atomic_inc(&sk->refcnt);
}

/* Ungrab socket in the context, which assumes that socket refcnt
   cannot hit zero, f.e. it is true in context of any socketcall.
 */
static inline void __sock_put(struct sock *sk)
{
    atomic_dec(&sk->refcnt);
}

/* Ungrab socket and destroy it, if it was the last reference. */
static inline void sock_put(struct sock *sk)
{
    if (atomic_dec_and_test(&sk->refcnt))
        sk_free(sk);
}

#define SOCK_MIN_SNDBUF 2048
#define SOCK_MIN_RCVBUF 256

/*
 *  Enable debug/info messages
 */

#if 0
#define NETDEBUG(x) do { } while (0)
#else
#define NETDEBUG(x) do { x; } while (0)
#endif

#endif  /* _SOCK_H */
