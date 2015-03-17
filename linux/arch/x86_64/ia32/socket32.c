// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2006

/*
 * 32bit Socket syscall emulation. Based on arch/sparc64/kernel/sys_sparc32.c.
 *
 * Copyright (C) 2000		VA Linux Co
 * Copyright (C) 2000		Don Dugger <n0ano@valinux.com>
 * Copyright (C) 1999 		Arun Sharma <arun.sharma@intel.com>
 * Copyright (C) 1997,1998 	Jakub Jelinek (jj@sunsite.mff.cuni.cz)
 * Copyright (C) 1997 		David S. Miller (davem@caip.rutgers.edu)
 * Copyright (C) 2000		Hewlett-Packard Co.
 * Copyright (C) 2000		David Mosberger-Tang <davidm@hpl.hp.com>
 * Copyright (C) 2000,2001	Andi Kleen, SuSE Labs
 */

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/file.h>
#include <linux/icmpv6.h>
#include <linux/socket.h>
#include <linux/filter.h>

#include <net/scm.h>
#include <net/sock.h>
#include <asm/ia32.h>
#include <asm/uaccess.h>
#include <asm/socket32.h>

#define A(__x)		((unsigned long)(__x))
#define AA(__x)		((unsigned long)(__x))


static inline int iov_from_user32_to_kern(struct iovec *kiov,
        struct iovec32 *uiov32,
        int niov)
{
    int tot_len = 0;

    while(niov > 0)
    {
        u32 len, buf;

        if(get_user(len, &uiov32->iov_len) ||
                get_user(buf, &uiov32->iov_base))
        {
            tot_len = -EFAULT;
            break;
        }
        tot_len += len;
        kiov->iov_base = (void *)A(buf);
        kiov->iov_len = (__kernel_size_t) len;
        uiov32++;
        kiov++;
        niov--;
    }
    return tot_len;
}

static inline int msghdr_from_user32_to_kern(struct msghdr *kmsg,
        struct msghdr32 *umsg)
{
    u32 tmp1, tmp2, tmp3;
    int err;

    err = get_user(tmp1, &umsg->msg_name);
    err |= __get_user(tmp2, &umsg->msg_iov);
    err |= __get_user(tmp3, &umsg->msg_control);
    if (err)
        return -EFAULT;

    kmsg->msg_name = (void *)A(tmp1);
    kmsg->msg_iov = (struct iovec *)A(tmp2);
    kmsg->msg_control = (void *)A(tmp3);

    err = get_user(kmsg->msg_namelen, &umsg->msg_namelen);
    err |= get_user(kmsg->msg_iovlen, &umsg->msg_iovlen);
    err |= get_user(kmsg->msg_controllen, &umsg->msg_controllen);
    err |= get_user(kmsg->msg_flags, &umsg->msg_flags);

    return err;
}

/* I've named the args so it is easy to tell whose space the pointers are in. */
static int verify_iovec32(struct msghdr *kern_msg, struct iovec *kern_iov,
                          char *kern_address, int mode)
{
    int tot_len;

    if(kern_msg->msg_namelen)
    {
        if(mode==VERIFY_READ)
        {
            int err = move_addr_to_kernel(kern_msg->msg_name,
                                          kern_msg->msg_namelen,
                                          kern_address);
            if(err < 0)
                return err;
        }
        kern_msg->msg_name = kern_address;
    }
    else
        kern_msg->msg_name = NULL;

    if(kern_msg->msg_iovlen > UIO_FASTIOV)
    {
        if (kern_msg->msg_iovlen > (2*PAGE_SIZE)/ sizeof(struct iovec))
            return -EINVAL;
        kern_iov = kmalloc(kern_msg->msg_iovlen * sizeof(struct iovec),
                           GFP_KERNEL);
        if(!kern_iov)
            return -ENOMEM;
    }

    tot_len = iov_from_user32_to_kern(kern_iov,
                                      (struct iovec32 *)kern_msg->msg_iov,
                                      kern_msg->msg_iovlen);
    if(tot_len >= 0)
        kern_msg->msg_iov = kern_iov;
    else if(kern_msg->msg_iovlen > UIO_FASTIOV)
        kfree(kern_iov);

    return tot_len;
}

/* There is a lot of hair here because the alignment rules (and
 * thus placement) of cmsg headers and length are different for
 * 32-bit apps.  -DaveM
 */
static int cmsghdr_from_user32_to_kern(struct msghdr *kmsg,
                                       unsigned char *stackbuf, int stackbuf_size)
{
    struct cmsghdr32 *ucmsg;
    struct cmsghdr *kcmsg, *kcmsg_base;
    __kernel_size_t32 ucmlen;
    __kernel_size_t kcmlen, tmp;

    kcmlen = 0;
    kcmsg_base = kcmsg = (struct cmsghdr *)stackbuf;
    ucmsg = CMSG32_FIRSTHDR(kmsg);
    while(ucmsg != NULL)
    {
        if(get_user(ucmlen, &ucmsg->cmsg_len))
            return -EFAULT;

        /* Catch bogons. */
        if (!CMSG32_OK(ucmlen, ucmsg, kmsg))
            return -EINVAL;

        if (kmsg->msg_controllen > 65536)
            return -EINVAL;

        tmp = ((ucmlen - CMSG32_ALIGN(sizeof(*ucmsg))) +
               CMSG_ALIGN(sizeof(struct cmsghdr)));
        kcmlen += tmp;
        ucmsg = CMSG32_NXTHDR(kmsg, ucmsg, ucmlen);
    }
    if(kcmlen == 0)
        return -EINVAL;

    /* The kcmlen holds the 64-bit version of the control length.
     * It may not be modified as we do not stick it into the kmsg
     * until we have successfully copied over all of the data
     * from the user.
     */
    if(kcmlen > stackbuf_size)
        kcmsg_base = kcmsg = kmalloc(kcmlen, GFP_KERNEL);
    if(kcmsg == NULL)
        return -ENOBUFS;

    /* Now copy them over neatly. */
    memset(kcmsg, 0, kcmlen);
    ucmsg = CMSG32_FIRSTHDR(kmsg);
    while(ucmsg != NULL)
    {
        __get_user(ucmlen, &ucmsg->cmsg_len);
        tmp = ((ucmlen - CMSG32_ALIGN(sizeof(*ucmsg))) +
               CMSG_ALIGN(sizeof(struct cmsghdr)));
        kcmsg->cmsg_len = tmp;
        __get_user(kcmsg->cmsg_level, &ucmsg->cmsg_level);
        __get_user(kcmsg->cmsg_type, &ucmsg->cmsg_type);

        /* Copy over the data. */
        if(copy_from_user(CMSG_DATA(kcmsg),
                          CMSG32_DATA(ucmsg),
                          (ucmlen - CMSG32_ALIGN(sizeof(*ucmsg)))))
            goto out_free_efault;

        /* Advance. */
        kcmsg = (struct cmsghdr *)((char *)kcmsg + CMSG_ALIGN(tmp));
        ucmsg = CMSG32_NXTHDR(kmsg, ucmsg, ucmlen);
    }

    /* Ok, looks like we made it.  Hook it up and return success. */
    kmsg->msg_control = kcmsg_base;
    kmsg->msg_controllen = kcmlen;
    return 0;

out_free_efault:
    if(kcmsg_base != (struct cmsghdr *)stackbuf)
        kfree(kcmsg_base);
    return -EFAULT;
}

static void put_cmsg32(struct msghdr *kmsg, int level, int type,
                       int len, void *data)
{
    struct cmsghdr32 *cm = (struct cmsghdr32 *) kmsg->msg_control;
    struct cmsghdr32 cmhdr;
    int cmlen = CMSG32_LEN(len);

    if(cm == NULL || kmsg->msg_controllen < sizeof(*cm))
    {
        kmsg->msg_flags |= MSG_CTRUNC;
        return;
    }

    if(kmsg->msg_controllen < cmlen)
    {
        kmsg->msg_flags |= MSG_CTRUNC;
        cmlen = kmsg->msg_controllen;
    }
    cmhdr.cmsg_level = level;
    cmhdr.cmsg_type = type;
    cmhdr.cmsg_len = cmlen;

    if(copy_to_user(cm, &cmhdr, sizeof cmhdr))
        return;
    if(copy_to_user(CMSG32_DATA(cm), data, cmlen - sizeof(struct cmsghdr32)))
        return;
    cmlen = CMSG32_SPACE(len);
    kmsg->msg_control += cmlen;
    kmsg->msg_controllen -= cmlen;
}

static void scm_detach_fds32(struct msghdr *kmsg, struct scm_cookie *scm)
{
}

/* In these cases we (currently) can just copy to data over verbatim
 * because all CMSGs created by the kernel have well defined types which
 * have the same layout in both the 32-bit and 64-bit API.  One must add
 * some special cased conversions here if we start sending control messages
 * with incompatible types.
 *
 * SCM_RIGHTS and SCM_CREDENTIALS are done by hand in recvmsg32 right after
 * we do our work.  The remaining cases are:
 *
 * SOL_IP	IP_PKTINFO	struct in_pktinfo	32-bit clean
 *		IP_TTL		int			32-bit clean
 *		IP_TOS		__u8			32-bit clean
 *		IP_RECVOPTS	variable length		32-bit clean
 *		IP_RETOPTS	variable length		32-bit clean
 *		(these last two are clean because the types are defined
 *		 by the IPv4 protocol)
 *		IP_RECVERR	struct sock_extended_err +
 *				struct sockaddr_in	32-bit clean
 * SOL_IPV6	IPV6_RECVERR	struct sock_extended_err +
 *				struct sockaddr_in6	32-bit clean
 *		IPV6_PKTINFO	struct in6_pktinfo	32-bit clean
 *		IPV6_HOPLIMIT	int			32-bit clean
 *		IPV6_FLOWINFO	u32			32-bit clean
 *		IPV6_HOPOPTS	ipv6 hop exthdr		32-bit clean
 *		IPV6_DSTOPTS	ipv6 dst exthdr(s)	32-bit clean
 *		IPV6_RTHDR	ipv6 routing exthdr	32-bit clean
 *		IPV6_AUTHHDR	ipv6 auth exthdr	32-bit clean
 */
static void cmsg32_recvmsg_fixup(struct msghdr *kmsg,
                                 unsigned long orig_cmsg_uptr, __kernel_size_t orig_cmsg_len)
{
    unsigned char *workbuf, *wp;
    unsigned long bufsz, space_avail;
    struct cmsghdr *ucmsg;

    bufsz = ((unsigned long)kmsg->msg_control) - orig_cmsg_uptr;
    space_avail = kmsg->msg_controllen + bufsz;
    wp = workbuf = kmalloc(bufsz, GFP_KERNEL);
    if(workbuf == NULL)
        goto fail;

    /* To make this more sane we assume the kernel sends back properly
     * formatted control messages.  Because of how the kernel will truncate
     * the cmsg_len for MSG_TRUNC cases, we need not check that case either.
     */
    ucmsg = (struct cmsghdr *) orig_cmsg_uptr;
    while(((unsigned long)ucmsg) <=
            (((unsigned long)kmsg->msg_control) - sizeof(struct cmsghdr)))
    {
        struct cmsghdr32 *kcmsg32 = (struct cmsghdr32 *) wp;
        int clen64, clen32;

        /* UCMSG is the 64-bit format CMSG entry in user-space.
         * KCMSG32 is within the kernel space temporary buffer
         * we use to convert into a 32-bit style CMSG.
         */
        __get_user(kcmsg32->cmsg_len, &ucmsg->cmsg_len);
        __get_user(kcmsg32->cmsg_level, &ucmsg->cmsg_level);
        __get_user(kcmsg32->cmsg_type, &ucmsg->cmsg_type);

        clen64 = kcmsg32->cmsg_len;
        if ((clen64 < CMSG_ALIGN(sizeof(*ucmsg))) ||
                (clen64 > (orig_cmsg_len + wp - workbuf)))
            break;
        copy_from_user(CMSG32_DATA(kcmsg32), CMSG_DATA(ucmsg),
                       clen64 - CMSG_ALIGN(sizeof(*ucmsg)));
        clen32 = ((clen64 - CMSG_ALIGN(sizeof(*ucmsg))) +
                  CMSG32_ALIGN(sizeof(struct cmsghdr32)));
        kcmsg32->cmsg_len = clen32;

        ucmsg = (struct cmsghdr *) (((char *)ucmsg) + CMSG_ALIGN(clen64));
        wp = (((char *)kcmsg32) + CMSG32_ALIGN(clen32));
    }

    /* Copy back fixed up data, and adjust pointers. */
    bufsz = (wp - workbuf);
    copy_to_user((void *)orig_cmsg_uptr, workbuf, bufsz);

    kmsg->msg_control = (struct cmsghdr *)
                        (((char *)orig_cmsg_uptr) + bufsz);
    kmsg->msg_controllen = space_avail - bufsz;

    kfree(workbuf);
    return;

fail:
    /* If we leave the 64-bit format CMSG chunks in there,
     * the application could get confused and crash.  So to
     * ensure greater recovery, we report no CMSGs.
     */
    kmsg->msg_controllen += bufsz;
    kmsg->msg_control = (void *) orig_cmsg_uptr;
}

asmlinkage long sys32_sendmsg(int fd, struct msghdr32 *user_msg, unsigned user_flags)
{
    return -EPERM;
}

asmlinkage long sys32_recvmsg(int fd, struct msghdr32 *user_msg, unsigned int user_flags)
{
}

extern asmlinkage int sys_setsockopt(int fd, int level, int optname,
                                     char *optval, int optlen);

static int do_set_attach_filter(int fd, int level, int optname,
                                char *optval, int optlen)
{
    struct sock_fprog32
    {
        __u16 len;
        __u32 filter;
    } *fprog32 = (struct sock_fprog32 *)optval;
    struct sock_fprog kfprog;
    mm_segment_t old_fs;
    __u32 uptr;
    int ret;

    if (get_user(kfprog.len, &fprog32->len) ||
            __get_user(uptr, &fprog32->filter))
        return -EFAULT;

    kfprog.filter = (struct sock_filter *)A(uptr);

    if (verify_area(VERIFY_WRITE, kfprog.filter, kfprog.len*sizeof(struct sock_filter)))
        return -EFAULT;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    ret = sys_setsockopt(fd, level, optname,
                         (char *)&kfprog, sizeof(kfprog));
    set_fs(old_fs);

    return ret;
}

static int do_set_icmpv6_filter(int fd, int level, int optname,
                                char *optval, int optlen)
{
    struct icmp6_filter kfilter;
    mm_segment_t old_fs;
    int ret, i;

    if (copy_from_user(&kfilter, optval, sizeof(kfilter)))
        return -EFAULT;


    for (i = 0; i < 8; i += 2)
    {
        u32 tmp = kfilter.data[i];

        kfilter.data[i] = kfilter.data[i + 1];
        kfilter.data[i + 1] = tmp;
    }

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    ret = sys_setsockopt(fd, level, optname,
                         (char *) &kfilter, sizeof(kfilter));
    set_fs(old_fs);

    return ret;
}

asmlinkage long sys32_setsockopt(int fd, int level, int optname,
                                 char *optval, int optlen)
{
    if (level == SOL_SOCKET && optname == SO_ATTACH_FILTER)
        return do_set_attach_filter(fd, level, optname,
                                    optval, optlen);
    if (level == SOL_ICMPV6 && optname == ICMPV6_FILTER)
        return do_set_icmpv6_filter(fd, level, optname,
                                    optval, optlen);

    return sys_setsockopt(fd, level, optname, optval, optlen);
}


/* Argument list sizes for sys_socketcall */
#define AL(x) ((x) * sizeof(u32))
static unsigned char nas[18]= {AL(0),AL(3),AL(3),AL(3),AL(2),AL(3),
                               AL(3),AL(3),AL(4),AL(4),AL(4),AL(6),
                               AL(6),AL(2),AL(5),AL(5),AL(3),AL(3)
                              };
#undef AL

extern asmlinkage long sys_bind(int fd, struct sockaddr *umyaddr, int addrlen);
extern asmlinkage long sys_connect(int fd, struct sockaddr *uservaddr,
                                   int addrlen);
extern asmlinkage long sys_accept(int fd, struct sockaddr *upeer_sockaddr,
                                  int *upeer_addrlen);
extern asmlinkage long sys_getsockname(int fd, struct sockaddr *usockaddr,
                                       int *usockaddr_len);
extern asmlinkage long sys_getpeername(int fd, struct sockaddr *usockaddr,
                                       int *usockaddr_len);
extern asmlinkage long sys_send(int fd, void *buff, size_t len, unsigned flags);
extern asmlinkage long sys_sendto(int fd, u32 buff, __kernel_size_t32 len,
                                  unsigned flags, u32 addr, int addr_len);
extern asmlinkage long sys_recv(int fd, void *ubuf, size_t size, unsigned flags);
extern asmlinkage long sys_recvfrom(int fd, u32 ubuf, __kernel_size_t32 size,
                                    unsigned flags, u32 addr, u32 addr_len);
extern asmlinkage long sys_getsockopt(int fd, int level, int optname,
                                      u32 optval, u32 optlen);

extern asmlinkage long sys_socket(int family, int type, int protocol);
extern asmlinkage long sys_socketpair(int family, int type, int protocol,
                                      int usockvec[2]);
extern asmlinkage long sys_shutdown(int fd, int how);
extern asmlinkage long sys_listen(int fd, int backlog);

asmlinkage long sys32_socketcall(int call, u32 *args)
{
    int ret;
    u32 a[6];
    u32 a0,a1;

    if (call<SYS_SOCKET||call>SYS_RECVMSG)
        return -EINVAL;
    if (copy_from_user(a, args, nas[call]))
        return -EFAULT;
    a0=a[0];
    a1=a[1];

    switch(call)
    {
    case SYS_SOCKET:
        ret = sys_socket(a0, a1, a[2]);
        break;
    case SYS_BIND:
        ret = sys_bind(a0, (struct sockaddr *)A(a1), a[2]);
        break;
    case SYS_CONNECT:
        ret = sys_connect(a0, (struct sockaddr *)A(a1), a[2]);
        break;
    case SYS_LISTEN:
        ret = sys_listen(a0, a1);
        break;
    case SYS_ACCEPT:
        ret = sys_accept(a0, (struct sockaddr *)A(a1),
                         (int *)A(a[2]));
        break;
    case SYS_GETSOCKNAME:
        ret = sys_getsockname(a0, (struct sockaddr *)A(a1),
                              (int *)A(a[2]));
        break;
    case SYS_GETPEERNAME:
        ret = sys_getpeername(a0, (struct sockaddr *)A(a1),
                              (int *)A(a[2]));
        break;
    case SYS_SOCKETPAIR:
        ret = sys_socketpair(a0, a1, a[2], (int *)A(a[3]));
        break;
    case SYS_SEND:
        ret = sys_send(a0, (void *)A(a1), a[2], a[3]);
        break;
    case SYS_SENDTO:
        ret = sys_sendto(a0, a1, a[2], a[3], a[4], a[5]);
        break;
    case SYS_RECV:
        ret = sys_recv(a0, (void *)A(a1), a[2], a[3]);
        break;
    case SYS_RECVFROM:
        ret = sys_recvfrom(a0, a1, a[2], a[3], a[4], a[5]);
        break;
    case SYS_SHUTDOWN:
        ret = sys_shutdown(a0,a1);
        break;
    case SYS_SETSOCKOPT:
        ret = sys32_setsockopt(a0, a1, a[2], (char *)A(a[3]),
                               a[4]);
        break;
    case SYS_GETSOCKOPT:
        ret = sys_getsockopt(a0, a1, a[2], a[3], a[4]);
        break;
    case SYS_SENDMSG:
        ret = sys32_sendmsg(a0, (struct msghdr32 *)A(a1),
                            a[2]);
        break;
    case SYS_RECVMSG:
        ret = sys32_recvmsg(a0, (struct msghdr32 *)A(a1),
                            a[2]);
        break;
    default:
        ret = -EINVAL;
        break;
    }
    return ret;
}
