// $Id$
// $Locker$

// Author. Roar Thron�s.
// Modified Linux source file, 2001-2006

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/sem.h>
#include <linux/msg.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/ipc.h>
#include <asm/mman.h>
#include <asm/types.h>
#include <asm/uaccess.h>
#include <asm/semaphore.h>
#include <asm/ipc.h>

#include <asm/ia32.h>

extern int sem_ctls[];

/*
 * sys32_ipc() is the de-multiplexer for the SysV IPC calls in 32bit emulation..
 *
 * This is really horribly ugly.
 */

struct msgbuf32
{
    s32 mtype;
    char mtext[1];
};

struct ipc_perm32
{
    int key;
    __kernel_uid_t32 uid;
    __kernel_gid_t32 gid;
    __kernel_uid_t32 cuid;
    __kernel_gid_t32 cgid;
    unsigned short mode;
    unsigned short seq;
};

struct ipc64_perm32
{
    unsigned key;
    __kernel_uid32_t32 uid;
    __kernel_gid32_t32 gid;
    __kernel_uid32_t32 cuid;
    __kernel_gid32_t32 cgid;
    unsigned short mode;
    unsigned short __pad1;
    unsigned short seq;
    unsigned short __pad2;
    unsigned int unused1;
    unsigned int unused2;
};

struct semid_ds32
{
    struct ipc_perm32 sem_perm;               /* permissions .. see ipc.h */
    __kernel_time_t32 sem_otime;              /* last semop time */
    __kernel_time_t32 sem_ctime;              /* last change time */
    u32 sem_base;              /* ptr to first semaphore in array */
    u32 sem_pending;          /* pending operations to be processed */
    u32 sem_pending_last;    /* last pending operation */
    u32 undo;                  /* undo requests on this array */
    unsigned short  sem_nsems;              /* no. of semaphores in array */
};

struct semid64_ds32
{
    struct ipc64_perm32 sem_perm;
    __kernel_time_t32 sem_otime;
    unsigned int __unused1;
    __kernel_time_t32 sem_ctime;
    unsigned int __unused2;
    unsigned int sem_nsems;
    unsigned int __unused3;
    unsigned int __unused4;
};

struct msqid_ds32
{
    struct ipc_perm32 msg_perm;
    u32 msg_first;
    u32 msg_last;
    __kernel_time_t32 msg_stime;
    __kernel_time_t32 msg_rtime;
    __kernel_time_t32 msg_ctime;
    u32 wwait;
    u32 rwait;
    unsigned short msg_cbytes;
    unsigned short msg_qnum;
    unsigned short msg_qbytes;
    __kernel_ipc_pid_t32 msg_lspid;
    __kernel_ipc_pid_t32 msg_lrpid;
};

struct msqid64_ds32
{
    struct ipc64_perm32 msg_perm;
    __kernel_time_t32 msg_stime;
    unsigned int __unused1;
    __kernel_time_t32 msg_rtime;
    unsigned int __unused2;
    __kernel_time_t32 msg_ctime;
    unsigned int __unused3;
    unsigned int msg_cbytes;
    unsigned int msg_qnum;
    unsigned int msg_qbytes;
    __kernel_pid_t32 msg_lspid;
    __kernel_pid_t32 msg_lrpid;
    unsigned int __unused4;
    unsigned int __unused5;
};

struct ipc_kludge
{
    u32 msgp;
    s32 msgtyp;
};

#define A(__x)      ((unsigned long)(__x))
#define AA(__x)     ((unsigned long)(__x))

#define IPCOP_MASK(__x) (1UL << (__x))

#if 1
// temp
int sys_semtimedop()
{
    printk("sys_semtimedop not impl\n");
}
#endif

static int
ipc_parse_version32 (int *cmd)
{
    if (*cmd & IPC_64)
    {
        *cmd ^= IPC_64;
        return IPC_64;
    }
    else
    {
        return IPC_OLD;
    }
}

static int put_semid(void *user_semid, struct semid64_ds *s, int version)
{
    int err2;
    switch (version)
    {
    case IPC_64:
    {
        struct semid64_ds32 *usp64 = (struct semid64_ds32 *) user_semid;

        if (!access_ok(VERIFY_WRITE, usp64, sizeof(*usp64)))
        {
            err2 = -EFAULT;
            break;
        }
        err2 = __put_user(s->sem_perm.key, &usp64->sem_perm.key);
        err2 |= __put_user(s->sem_perm.uid, &usp64->sem_perm.uid);
        err2 |= __put_user(s->sem_perm.gid, &usp64->sem_perm.gid);
        err2 |= __put_user(s->sem_perm.cuid, &usp64->sem_perm.cuid);
        err2 |= __put_user(s->sem_perm.cgid, &usp64->sem_perm.cgid);
        err2 |= __put_user(s->sem_perm.mode, &usp64->sem_perm.mode);
        err2 |= __put_user(s->sem_perm.seq, &usp64->sem_perm.seq);
        err2 |= __put_user(s->sem_otime, &usp64->sem_otime);
        err2 |= __put_user(s->sem_ctime, &usp64->sem_ctime);
        err2 |= __put_user(s->sem_nsems, &usp64->sem_nsems);
        break;
    }
    default:
    {
        struct semid_ds32 *usp32 = (struct semid_ds32 *) user_semid;

        if (!access_ok(VERIFY_WRITE, usp32, sizeof(*usp32)))
        {
            err2 = -EFAULT;
            break;
        }
        err2 = __put_user(s->sem_perm.key, &usp32->sem_perm.key);
        err2 |= __put_user(s->sem_perm.uid, &usp32->sem_perm.uid);
        err2 |= __put_user(s->sem_perm.gid, &usp32->sem_perm.gid);
        err2 |= __put_user(s->sem_perm.cuid, &usp32->sem_perm.cuid);
        err2 |= __put_user(s->sem_perm.cgid, &usp32->sem_perm.cgid);
        err2 |= __put_user(s->sem_perm.mode, &usp32->sem_perm.mode);
        err2 |= __put_user(s->sem_perm.seq, &usp32->sem_perm.seq);
        err2 |= __put_user(s->sem_otime, &usp32->sem_otime);
        err2 |= __put_user(s->sem_ctime, &usp32->sem_ctime);
        err2 |= __put_user(s->sem_nsems, &usp32->sem_nsems);
        break;
    }
    }
    return err2;
}

static int
semctl32 (int first, int second, int third, void *uptr)
{
    union semun fourth;
    u32 pad;
    int err = 0;
    struct semid64_ds s;
    mm_segment_t old_fs;
    int version = ipc_parse_version32(&third);

    if (!uptr)
        return -EINVAL;
    if (get_user(pad, (u32 *)uptr))
        return -EFAULT;
    if (third == SETVAL)
        fourth.val = (int)pad;
    else
        fourth.__pad = (void *)A(pad);
    switch (third)
    {
    case IPC_INFO:
    case IPC_RMID:
    case IPC_SET:
    case SEM_INFO:
    case GETVAL:
    case GETPID:
    case GETNCNT:
    case GETZCNT:
    case GETALL:
    case SETVAL:
    case SETALL:
        err = sys_semctl(first, second, third, fourth);
        break;

    case IPC_STAT:
    case SEM_STAT:
        fourth.__pad = &s;
        old_fs = get_fs();
        set_fs(KERNEL_DS);
        err = sys_semctl(first, second, third|IPC_64, fourth);
        set_fs(old_fs);

        if (!err)
            err = put_semid((void *)A(pad), &s, version);

        break;
    default:
        err = -EINVAL;
        break;
    }
    return err;
}

#define MAXBUF (64*1024)

static int
do_sys32_msgsnd (int first, int second, int third, void *uptr)
{
    struct msgbuf *p;
    struct msgbuf32 *up = (struct msgbuf32 *)uptr;
    mm_segment_t old_fs;
    int err;

    if (second >= MAXBUF-sizeof(struct msgbuf))
        return -EINVAL;
    p = kmalloc(second + sizeof(struct msgbuf), GFP_USER);
    if (!p)
        return -ENOMEM;
    err = get_user(p->mtype, &up->mtype);
    err |= (copy_from_user(p->mtext, &up->mtext, second) ? -EFAULT : 0);
    if (err)
        goto out;
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    err = sys_msgsnd(first, p, second, third);
    set_fs(old_fs);
out:
    kfree(p);
    return err;
}

static int
do_sys32_msgrcv (int first, int second, int msgtyp, int third, int version, void *uptr)
{
    struct msgbuf32 *up;
    struct msgbuf *p;
    mm_segment_t old_fs;
    int err;

    if (!version)
    {
        struct ipc_kludge *uipck = (struct ipc_kludge *)uptr;
        struct ipc_kludge ipck;

        err = -EINVAL;
        if (!uptr)
            goto out;
        err = -EFAULT;
        if (copy_from_user(&ipck, uipck, sizeof(struct ipc_kludge)))
            goto out;
        uptr = (void *)A(ipck.msgp);
        msgtyp = ipck.msgtyp;
    }
    if (second >= MAXBUF-sizeof(struct msgbuf))
        return -EINVAL;
    err = -ENOMEM;
    p = kmalloc(second + sizeof(struct msgbuf), GFP_USER);
    if (!p)
        goto out;
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    err = sys_msgrcv(first, p, second, msgtyp, third);
    set_fs(old_fs);
    if (err < 0)
        goto free_then_out;
    up = (struct msgbuf32 *)uptr;
    if (put_user(p->mtype, &up->mtype) || copy_to_user(&up->mtext, p->mtext, err))
        err = -EFAULT;
free_then_out:
    kfree(p);
out:
    return err;
}

static int
msgctl32 (int first, int second, void *uptr)
{
    int err = -EINVAL, err2;
    struct msqid_ds m;
    struct msqid64_ds m64;
    struct msqid_ds32 *up32 = (struct msqid_ds32 *)uptr;
    struct msqid64_ds32 *up64 = (struct msqid64_ds32 *)uptr;
    mm_segment_t old_fs;
    int version = ipc_parse_version32(&second);

    switch (second)
    {
    case IPC_INFO:
    case IPC_RMID:
    case MSG_INFO:
        err = sys_msgctl(first, second, (struct msqid_ds *)uptr);
        break;

    case IPC_SET:
        if (version == IPC_64)
        {
            err = get_user(m.msg_perm.uid, &up64->msg_perm.uid);
            err |= get_user(m.msg_perm.gid, &up64->msg_perm.gid);
            err |= get_user(m.msg_perm.mode, &up64->msg_perm.mode);
            err |= get_user(m.msg_qbytes, &up64->msg_qbytes);
        }
        else
        {
            err = get_user(m.msg_perm.uid, &up32->msg_perm.uid);
            err |= get_user(m.msg_perm.gid, &up32->msg_perm.gid);
            err |= get_user(m.msg_perm.mode, &up32->msg_perm.mode);
            err |= get_user(m.msg_qbytes, &up32->msg_qbytes);
        }
        if (err)
            break;
        old_fs = get_fs();
        set_fs(KERNEL_DS);
        err = sys_msgctl(first, second, &m);
        set_fs(old_fs);
        break;

    case IPC_STAT:
    case MSG_STAT:
        old_fs = get_fs();
        set_fs(KERNEL_DS);
        err = sys_msgctl(first, second|IPC_64, (void *) &m64);
        set_fs(old_fs);

        if (version == IPC_64)
        {
            if (!access_ok(VERIFY_WRITE, up64, sizeof(*up64)))
            {
                err = -EFAULT;
                break;
            }
            err2 = __put_user(m64.msg_perm.key, &up64->msg_perm.key);
            err2 |= __put_user(m64.msg_perm.uid, &up64->msg_perm.uid);
            err2 |= __put_user(m64.msg_perm.gid, &up64->msg_perm.gid);
            err2 |= __put_user(m64.msg_perm.cuid, &up64->msg_perm.cuid);
            err2 |= __put_user(m64.msg_perm.cgid, &up64->msg_perm.cgid);
            err2 |= __put_user(m64.msg_perm.mode, &up64->msg_perm.mode);
            err2 |= __put_user(m64.msg_perm.seq, &up64->msg_perm.seq);
            err2 |= __put_user(m64.msg_stime, &up64->msg_stime);
            err2 |= __put_user(m64.msg_rtime, &up64->msg_rtime);
            err2 |= __put_user(m64.msg_ctime, &up64->msg_ctime);
            err2 |= __put_user(m64.msg_cbytes, &up64->msg_cbytes);
            err2 |= __put_user(m64.msg_qnum, &up64->msg_qnum);
            err2 |= __put_user(m64.msg_qbytes, &up64->msg_qbytes);
            err2 |= __put_user(m64.msg_lspid, &up64->msg_lspid);
            err2 |= __put_user(m64.msg_lrpid, &up64->msg_lrpid);
            if (err2)
                err = -EFAULT;
        }
        else
        {
            if (!access_ok(VERIFY_WRITE, up32, sizeof(*up32)))
            {
                err = -EFAULT;
                break;
            }
            err2 = __put_user(m64.msg_perm.key, &up32->msg_perm.key);
            err2 |= __put_user(m64.msg_perm.uid, &up32->msg_perm.uid);
            err2 |= __put_user(m64.msg_perm.gid, &up32->msg_perm.gid);
            err2 |= __put_user(m64.msg_perm.cuid, &up32->msg_perm.cuid);
            err2 |= __put_user(m64.msg_perm.cgid, &up32->msg_perm.cgid);
            err2 |= __put_user(m64.msg_perm.mode, &up32->msg_perm.mode);
            err2 |= __put_user(m64.msg_perm.seq, &up32->msg_perm.seq);
            err2 |= __put_user(m64.msg_stime, &up32->msg_stime);
            err2 |= __put_user(m64.msg_rtime, &up32->msg_rtime);
            err2 |= __put_user(m64.msg_ctime, &up32->msg_ctime);
            err2 |= __put_user(m64.msg_cbytes, &up32->msg_cbytes);
            err2 |= __put_user(m64.msg_qnum, &up32->msg_qnum);
            err2 |= __put_user(m64.msg_qbytes, &up32->msg_qbytes);
            err2 |= __put_user(m64.msg_lspid, &up32->msg_lspid);
            err2 |= __put_user(m64.msg_lrpid, &up32->msg_lrpid);
            if (err2)
                err = -EFAULT;
        }
        break;
    }
    return err;
}

asmlinkage long
sys32_ipc (u32 call, int first, int second, int third, u32 ptr, u32 fifth)
{
#if 0
    // not yet
    int version;

    version = call >> 16; /* hack for backward compatibility */
    call &= 0xffff;

    switch (call)
    {
    case SEMOP:
        /* struct sembuf is the same on 32 and 64bit :)) */
        return sys_semtimedop(first, (struct sembuf *)AA(ptr), second, NULL);
    case SEMTIMEDOP:
    {
        int err;
        mm_segment_t oldfs = get_fs();
        struct timespec32 *ts32 = (struct timespec32 *)AA(fifth);
        struct timespec ts;
        if ((unsigned)second > sem_ctls[2])
            return -EINVAL;
        if (ts32)
        {
            if (get_user(ts.tv_sec, &ts32->tv_sec) ||
                    __get_user(ts.tv_nsec, &ts32->tv_nsec) ||
                    verify_area(VERIFY_READ, (void *)AA(ptr),
                                second*sizeof(struct sembuf)))
                return -EFAULT;
        }
        set_fs(KERNEL_DS);
        err = sys_semtimedop(first, (struct sembuf *)AA(ptr), second,
                             ts32 ? &ts : NULL);
        set_fs(oldfs);
        return err;
    }
    case SEMGET:
        return sys_semget(first, second, third);
    case SEMCTL:
        return semctl32(first, second, third, (void *)AA(ptr));

    case MSGSND:
        return do_sys32_msgsnd(first, second, third, (void *)AA(ptr));
    case MSGRCV:
        return do_sys32_msgrcv(first, second, fifth, third, version, (void *)AA(ptr));
    case MSGGET:
        return sys_msgget((key_t) first, second);
    case MSGCTL:
        return msgctl32(first, second, (void *)AA(ptr));

    case SHMAT:
    case SHMDT:
    case SHMGET:
    case SHMCTL:
        return -EINVAL;

    }
#endif
    return -ENOSYS;
}

