// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004.

/*
 * NET      An implementation of the SOCKET network access protocol.
 *
 * Version: @(#)socket.c    1.1.93  18/02/95
 *
 * Authors: Orest Zborowski, <obz@Kodak.COM>
 *      Ross Biro, <bir7@leland.Stanford.Edu>
 *      Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *
 * Fixes:
 *      Anonymous   :   NOTSOCK/BADF cleanup. Error fix in
 *                  shutdown()
 *      Alan Cox    :   verify_area() fixes
 *      Alan Cox    :   Removed DDI
 *      Jonathan Kamens :   SOCK_DGRAM reconnect bug
 *      Alan Cox    :   Moved a load of checks to the very
 *                  top level.
 *      Alan Cox    :   Move address structures to/from user
 *                  mode above the protocol layers.
 *      Rob Janssen :   Allow 0 length sends.
 *      Alan Cox    :   Asynchronous I/O support (cribbed from the
 *                  tty drivers).
 *      Niibe Yutaka    :   Asynchronous I/O for writes (4.4BSD style)
 *      Jeff Uphoff :   Made max number of sockets command-line
 *                  configurable.
 *      Matti Aarnio    :   Made the number of sockets dynamic,
 *                  to be allocated when needed, and mr.
 *                  Uphoff's max is used as max to be
 *                  allowed to allocate.
 *      Linus       :   Argh. removed all the socket allocation
 *                  altogether: it's in the inode now.
 *      Alan Cox    :   Made sock_alloc()/sock_release() public
 *                  for NetROM and future kernel nfsd type
 *                  stuff.
 *      Alan Cox    :   sendmsg/recvmsg basics.
 *      Tom Dyas    :   Export net symbols.
 *      Marcin Dalecki  :   Fixed problems with CONFIG_NET="n".
 *      Alan Cox    :   Added thread locking to sys_* calls
 *                  for sockets. May have errors at the
 *                  moment.
 *      Kevin Buhr  :   Fixed the dumb errors in the above.
 *      Andi Kleen  :   Some small cleanups, optimizations,
 *                  and fixed a copy_from_user() bug.
 *      Tigran Aivazian :   sys_send(args) calls sys_sendto(args, NULL, 0)
 *      Tigran Aivazian :   Made listen(2) backlog sanity checks
 *                  protocol-independent
 *
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 *
 *
 *  This module is effectively the top level interface to the BSD socket
 *  paradigm.
 *
 */

#include <linux/config.h>
#include <linux/mm.h>
#include <linux/smp_lock.h>
#include <linux/socket.h>
#include <linux/file.h>
#include <linux/net.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/cache.h>
#include <linux/module.h>
#include <linux/highmem.h>

#if defined(CONFIG_KMOD) && defined(CONFIG_NET)
#include <linux/kmod.h>
#endif

#include <asm/uaccess.h>

#include <net/sock.h>
#include <net/scm.h>
#include <linux/netfilter.h>

static int sock_no_open(struct inode *irrelevant, struct file *dontcare);
static loff_t sock_lseek(struct file *file, loff_t offset, int whence);
static ssize_t sock_read(struct file *file, char *buf,
                         size_t size, loff_t *ppos);
static ssize_t sock_write(struct file *file, const char *buf,
                          size_t size, loff_t *ppos);
static int sock_mmap(struct file *file, struct vm_area_struct * vma);

static int sock_close(struct inode *inode, struct file *file);
static unsigned int sock_poll(struct file *file,
                              struct poll_table_struct *wait);
static int sock_ioctl(struct inode *inode, struct file *file,
                      unsigned int cmd, unsigned long arg);
static int sock_fasync(int fd, struct file *filp, int on);
static ssize_t sock_readv(struct file *file, const struct iovec *vector,
                          unsigned long count, loff_t *ppos);
static ssize_t sock_writev(struct file *file, const struct iovec *vector,
                           unsigned long count, loff_t *ppos);
static ssize_t sock_sendpage(struct file *file, struct page *page,
                             int offset, size_t size, loff_t *ppos, int more);


/*
 *  The protocol list. Each protocol is registered in here.
 */

static struct net_proto_family *net_families[NPROTO];

#ifdef CONFIG_SMP
static atomic_t net_family_lockct = ATOMIC_INIT(0);
static spinlock_t net_family_lock = SPIN_LOCK_UNLOCKED;

/* The strategy is: modifications net_family vector are short, do not
   sleep and veeery rare, but read access should be free of any exclusive
   locks.
 */

static void net_family_write_lock(void)
{
    spin_lock(&net_family_lock);
    while (atomic_read(&net_family_lockct) != 0)
    {
        spin_unlock(&net_family_lock);

        //      current->policy |= SCHED_YIELD;
        //      current->need_resched=1;
        //      schedule();
        SOFTINT_RESCHED_VECTOR;

        spin_lock(&net_family_lock);
    }
}

static __inline__ void net_family_write_unlock(void)
{
    spin_unlock(&net_family_lock);
}

static __inline__ void net_family_read_lock(void)
{
    atomic_inc(&net_family_lockct);
    spin_unlock_wait(&net_family_lock);
}

static __inline__ void net_family_read_unlock(void)
{
    atomic_dec(&net_family_lockct);
}

#else
#define net_family_write_lock() do { } while(0)
#define net_family_write_unlock() do { } while(0)
#define net_family_read_lock() do { } while(0)
#define net_family_read_unlock() do { } while(0)
#endif


/*
 *  Statistics counters of the socket lists
 */

static union
{
    int counter;
    char    __pad[SMP_CACHE_BYTES];
} sockets_in_use[NR_CPUS] __cacheline_aligned = {{0}};

/*
 *  Support routines. Move socket addresses back and forth across the kernel/user
 *  divide and look after the messy bits.
 */

#define MAX_SOCK_ADDR   128     /* 108 for Unix domain - 
16 for IP, 16 for IPX,
       24 for IPv6,
              about 80 for AX.25
                  must be at least one bigger than
                  the AF_UNIX size (see net/unix/af_unix.c
                        :unix_mkname()).
                      */

                      /**
                       *    move_addr_to_kernel -   copy a socket address into kernel space
                       *    @uaddr: Address in user space
                       *    @kaddr: Address in kernel space
                       *    @ulen: Length in user space
                       *
                       *    The address is copied into kernel space. If the provided address is
                       *    too long an error code of -EINVAL is returned. If the copy gives
                       *    invalid addresses -EFAULT is returned. On a success 0 is returned.
                       */

                      int move_addr_to_kernel(void *uaddr, int ulen, void *kaddr)
            {
                if(ulen<0||ulen>MAX_SOCK_ADDR)
                    return -EINVAL;
                if(ulen==0)
                    return 0;
                if(copy_from_user(kaddr,uaddr,ulen))
                    return -EFAULT;
                return 0;
            }

/**
 *  move_addr_to_user   -   copy an address to user space
 *  @kaddr: kernel space address
 *  @klen: length of address in kernel
 *  @uaddr: user space address
 *  @ulen: pointer to user length field
 *
 *  The value pointed to by ulen on entry is the buffer length available.
 *  This is overwritten with the buffer space used. -EINVAL is returned
 *  if an overlong buffer is specified or a negative buffer size. -EFAULT
 *  is returned if either the buffer or the length field are not
 *  accessible.
 *  After copying the data up to the limit the user specifies, the true
 *  length of the data is written over the length limit the user
 *  specified. Zero is returned for a success.
 */

int move_addr_to_user(void *kaddr, int klen, void *uaddr, int *ulen)
{
    int err;
    int len;

    if((err=get_user(len, ulen)))
        return err;
    if(len>klen)
        len=klen;
    if(len<0 || len> MAX_SOCK_ADDR)
        return -EINVAL;
    if(len)
    {
        if(copy_to_user(uaddr,kaddr,len))
            return -EFAULT;
    }
    /*
     *  "fromlen shall refer to the value before truncation.."
     *          1003.1g
     */
    return __put_user(klen, ulen);
}

#define SOCKFS_MAGIC 0x534F434B
int sock_sendmsg(struct socket *sock, struct msghdr *msg, int size)
{
}

int sock_recvmsg(struct socket *sock, struct msghdr *msg, int size, int flags)
{
}


/*
 *  Create a pair of connected sockets.
 */

asmlinkage long sys_socketpair(int family, int type, int protocol, int usockvec[2])
{
    return -EAFNOSUPPORT;
}


/* Argument list sizes for sys_socketcall */
#define AL(x) ((x) * sizeof(unsigned long))
static unsigned char nargs[18]= {AL(0),AL(3),AL(3),AL(3),AL(2),AL(3),
                                 AL(3),AL(3),AL(4),AL(4),AL(4),AL(6),
                                 AL(6),AL(2),AL(5),AL(5),AL(3),AL(3)
                                };
#undef AL

/*
 *  System call vectors.
 *
 *  Argument checking cleaned up. Saved 20% in size.
 *  This function doesn't need to set the kernel lock because
 *  it is set by the callees.
 */

asmlinkage int sys_socket();
asmlinkage int sys_bind();
asmlinkage int sys_connect();
asmlinkage int sys_listen();
asmlinkage int sys_accept();
asmlinkage int sys_getsockname();
asmlinkage int sys_getpeername();
asmlinkage int sys_send();
asmlinkage int sys_sendto();
asmlinkage int sys_recv();
asmlinkage int sys_recvfrom();
asmlinkage int sys_shutdown();
asmlinkage int sys_setsockopt();
asmlinkage int sys_getsockopt();
asmlinkage int sys_sendmsg();
asmlinkage int sys_recvmsg();

asmlinkage long sys_socketcall(int call, unsigned long *args)
{
    unsigned long a[6];
    unsigned long a0,a1;
    int err;

    if(call<1||call>SYS_RECVMSG)
        return -EINVAL;

    /* copy_from_user should be SMP safe. */
    if (copy_from_user(a, args, nargs[call]))
        return -EFAULT;

    a0=a[0];
    a1=a[1];

    switch(call)
    {
    case SYS_SOCKET:
        err = sys_socket(a0,a1,a[2]);
        break;
    case SYS_BIND:
        err = sys_bind(a0,(struct sockaddr *)a1, a[2]);
        break;
    case SYS_CONNECT:
        err = sys_connect(a0, (struct sockaddr *)a1, a[2]);
        break;
    case SYS_LISTEN:
        err = sys_listen(a0,a1);
        break;
    case SYS_ACCEPT:
        err = sys_accept(a0,(struct sockaddr *)a1, (int *)a[2]);
        break;
    case SYS_GETSOCKNAME:
        err = sys_getsockname(a0,(struct sockaddr *)a1, (int *)a[2]);
        break;
    case SYS_GETPEERNAME:
        err = sys_getpeername(a0, (struct sockaddr *)a1, (int *)a[2]);
        break;
    case SYS_SOCKETPAIR:
        err = sys_socketpair(a0,a1, a[2], (int *)a[3]);
        break;
    case SYS_SEND:
        err = sys_send(a0, (void *)a1, a[2], a[3]);
        break;
    case SYS_SENDTO:
        err = sys_sendto(a0,(void *)a1, a[2], a[3],
                         (struct sockaddr *)a[4], a[5]);
        break;
    case SYS_RECV:
        err = sys_recv(a0, (void *)a1, a[2], a[3]);
        break;
    case SYS_RECVFROM:
        err = sys_recvfrom(a0, (void *)a1, a[2], a[3],
                           (struct sockaddr *)a[4], (int *)a[5]);
        break;
    case SYS_SHUTDOWN:
        err = sys_shutdown(a0,a1);
        break;
    case SYS_SETSOCKOPT:
        err = sys_setsockopt(a0, a1, a[2], (char *)a[3], a[4]);
        break;
    case SYS_GETSOCKOPT:
        err = sys_getsockopt(a0, a1, a[2], (char *)a[3], (int *)a[4]);
        break;
#ifndef __KERNEL__
    case SYS_SENDMSG:
        err = sys_sendmsg(a0, (struct msghdr *) a1, a[2]);
        break;
    case SYS_RECVMSG:
        err = sys_recvmsg(a0, (struct msghdr *) a1, a[2]);
        break;
#else
    case SYS_SENDMSG:
    case SYS_RECVMSG:
        err = -EAFNOSUPPORT;
        break;
#endif
    default:
        err = -EINVAL;
        break;
    }
    return err;
}

#ifdef __x86_64__
asmlinkage int sys_sendmsg(int fd, struct msghdr *msg, unsigned flags)
{
    return -EAFNOSUPPORT;
}

asmlinkage int sys_recvmsg(int fd, struct msghdr *msg, unsigned int flags)
{
    return -EAFNOSUPPORT;
}
#endif
