// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004.

/*
 * linux/ipc/shm.c
 * Copyright (C) 1992, 1993 Krishna Balasubramanian
 *   Many improvements/fixes by Bruno Haible.
 * Replaced `struct shm_desc' by `struct vm_area_struct', July 1994.
 * Fixed the shm swap deallocation (shm_unuse()), August 1998 Andrea Arcangeli.
 *
 * /proc/sysvipc/shm support (c) 1999 Dragos Acostachioaie <dragos@iname.com>
 * BIGMEM support, Andrea Arcangeli <andrea@suse.de>
 * SMP thread shm, Jean-Luc Boyard <jean-luc.boyard@siemens.fr>
 * HIGHMEM support, Ingo Molnar <mingo@redhat.com>
 * Make shmmax, shmall, shmmni sysctl'able, Christoph Rohland <cr@sap.com>
 * Shared /dev/zero support, Kanoj Sarcar <kanoj@sgi.com>
 * Move the mm functionality over to mm/shmem.c, Christoph Rohland <cr@sap.com>
 *
 */

#include <linux/config.h>
#include <linux/slab.h>
#include <linux/shm.h>
#include <linux/init.h>
#include <linux/file.h>
#include <linux/mman.h>
#include <asm/uaccess.h>
#include <misc_routines.h>

#include "util.h"

#undef CONFIG_PROC_FS

struct shmid_kernel /* private to the kernel */
{
    struct kern_ipc_perm    shm_perm;
    struct file *       shm_file;
    int         id;
    unsigned long       shm_nattch;
    unsigned long       shm_segsz;
    time_t          shm_atim;
    time_t          shm_dtim;
    time_t          shm_ctim;
    pid_t           shm_cprid;
    pid_t           shm_lprid;
};

#define shm_flags   shm_perm.mode

static struct file_operations shm_file_operations;
static struct vm_operations_struct shm_vm_ops;

static struct ipc_ids shm_ids;

#define shm_lock(id)    ((struct shmid_kernel*)ipc_lock(&shm_ids,id))
#define shm_unlock(id)  ipc_unlock(&shm_ids,id)
#define shm_lockall()   ipc_lockall(&shm_ids)
#define shm_unlockall() ipc_unlockall(&shm_ids)
#define shm_get(id) ((struct shmid_kernel*)ipc_get(&shm_ids,id))
#define shm_buildid(id, seq) \
    ipc_buildid(&shm_ids, id, seq)

static int newseg (key_t key, int shmflg, size_t size);
static void shm_open (struct vm_area_struct *shmd);
static void shm_close (struct vm_area_struct *shmd);
#ifdef CONFIG_PROC_FS
static int sysvipc_shm_read_proc(char *buffer, char **start, off_t offset, int length, int *eof, void *data);
#endif

static int shm_tot; /* total number of shared memory pages */

void __init shm_init (void)
{
    ipc_init_ids(&shm_ids, 1);
#ifdef CONFIG_PROC_FS
    create_proc_read_entry("sysvipc/shm", 0, 0, sysvipc_shm_read_proc, NULL);
#endif
}

static inline int shm_checkid(struct shmid_kernel *s, int id)
{
    if (ipc_checkid(&shm_ids,&s->shm_perm,id))
        return -EIDRM;
    return 0;
}

static inline struct shmid_kernel *shm_rmid(int id)
{
    return (struct shmid_kernel *)ipc_rmid(&shm_ids,id);
}

static inline void shm_inc (int id)
{
    struct shmid_kernel *shp;

    if(!(shp = shm_lock(id)))
        BUG();
    shp->shm_atim = CURRENT_TIME;
    shp->shm_lprid = current->pcb$l_pid;
    shp->shm_nattch++;
    shm_unlock(id);
}

/* This is called by fork, once for every shm attach. */
static void shm_open (struct vm_area_struct *shmd)
{
}

/*
 * shm_destroy - free the struct shmid_kernel
 *
 * @shp: struct to free
 *
 * It has to be called with shp and shm_ids.sem locked
 */
static void shm_destroy (struct shmid_kernel *shp)
{
}

/*
 * remove the attach descriptor shmd.
 * free memory for segment if it is marked destroyed.
 * The descriptor has already been removed from the current->mm->mmap list
 * and will later be kfree()d.
 */
static void shm_close (struct vm_area_struct *shmd)
{
}

static int shm_mmap(struct file * file, struct vm_area_struct * vma)
{
    return 0;
}

static struct file_operations shm_file_operations =
{
};

static struct vm_operations_struct shm_vm_ops =
{
};

static int newseg (key_t key, int shmflg, size_t size)
{
}

asmlinkage long sys_shmget (key_t key, size_t size, int shmflg)
{
    struct shmid_kernel *shp;
    int err, id = 0;

    down(&shm_ids.sem);
    if (key == IPC_PRIVATE)
    {
        err = newseg(key, shmflg, size);
    }
    else if ((id = ipc_findkey(&shm_ids, key)) == -1)
    {
        if (!(shmflg & IPC_CREAT))
            err = -ENOENT;
        else
            err = newseg(key, shmflg, size);
    }
    else if ((shmflg & IPC_CREAT) && (shmflg & IPC_EXCL))
    {
        err = -EEXIST;
    }
    else
    {
        shp = shm_lock(id);
        if(shp==NULL)
            BUG();
        if (shp->shm_segsz < size)
            err = -EINVAL;
        else if (ipcperms(&shp->shm_perm, shmflg))
            err = -EACCES;
        else
            err = shm_buildid(id, shp->shm_perm.seq);
        shm_unlock(id);
    }
    up(&shm_ids.sem);
    return err;
}

static inline unsigned long copy_shmid_to_user(void *buf, struct shmid64_ds *in, int version)
{
    switch(version)
    {
    case IPC_64:
        return copy_to_user(buf, in, sizeof(*in));
    case IPC_OLD:
    {
        struct shmid_ds out;

        ipc64_perm_to_ipc_perm(&in->shm_perm, &out.shm_perm);
        out.shm_segsz   = in->shm_segsz;
        out.shm_atime   = in->shm_atime;
        out.shm_dtime   = in->shm_dtime;
        out.shm_ctime   = in->shm_ctime;
        out.shm_cpid    = in->shm_cpid;
        out.shm_lpid    = in->shm_lpid;
        out.shm_nattch  = in->shm_nattch;

        return copy_to_user(buf, &out, sizeof(out));
    }
    default:
        return -EINVAL;
    }
}

struct shm_setbuf
{
    uid_t   uid;
    gid_t   gid;
    mode_t  mode;
};

static inline unsigned long copy_shmid_from_user(struct shm_setbuf *out, void *buf, int version)
{
    return -EINVAL;
}

static inline unsigned long copy_shminfo_to_user(void *buf, struct shminfo64 *in, int version)
{
}

static void shm_get_stat (unsigned long *rss, unsigned long *swp)
{
}

asmlinkage long sys_shmctl (int shmid, int cmd, struct shmid_ds *buf)
{
    return -EINVAL;
}

/*
 * Fix shmaddr, allocate descriptor, map shm, add attach descriptor to lists.
 */
asmlinkage long sys_shmat (int shmid, char *shmaddr, int shmflg, ulong *raddr)
{
    return -EINVAL;
}

/*
 * detach and kill segment if marked destroyed.
 * The work is done in shm_close.
 */
asmlinkage long sys_shmdt (char *shmaddr)
{
    struct mm_struct *mm = current->mm;
    struct vm_area_struct *shmd, *shmdnext;

    return 0;
}

#ifdef CONFIG_PROC_FS
static int sysvipc_shm_read_proc(char *buffer, char **start, off_t offset, int length, int *eof, void *data)
{
    off_t pos = 0;
    off_t begin = 0;
    int i, len = 0;

    down(&shm_ids.sem);
    len += sprintf(buffer, "       key      shmid perms       size  cpid  lpid nattch   uid   gid  cuid  cgid      atime      dtime      ctime\n");

    for(i = 0; i <= shm_ids.max_id; i++)
    {
        struct shmid_kernel* shp;

        shp = shm_lock(i);
        if(shp!=NULL)
        {
#define SMALL_STRING "%10d %10d  %4o %10u %5u %5u  %5d %5u %5u %5u %5u %10lu %10lu %10lu\n"
#define BIG_STRING   "%10d %10d  %4o %21u %5u %5u  %5d %5u %5u %5u %5u %10lu %10lu %10lu\n"
            char *format;

            if (sizeof(size_t) <= sizeof(int))
                format = SMALL_STRING;
            else
                format = BIG_STRING;
            len += sprintf(buffer + len, format,
                           shp->shm_perm.key,
                           shm_buildid(i, shp->shm_perm.seq),
                           shp->shm_flags,
                           shp->shm_segsz,
                           shp->shm_cprid,
                           shp->shm_lprid,
                           shp->shm_nattch,
                           shp->shm_perm.uid,
                           shp->shm_perm.gid,
                           shp->shm_perm.cuid,
                           shp->shm_perm.cgid,
                           shp->shm_atim,
                           shp->shm_dtim,
                           shp->shm_ctim);
            shm_unlock(i);

            pos += len;
            if(pos < offset)
            {
                len = 0;
                begin = pos;
            }
            if(pos > offset + length)
                goto done;
        }
    }
    *eof = 1;
done:
    up(&shm_ids.sem);
    *start = buffer + (offset - begin);
    len -= (offset - begin);
    if(len > length)
        len = length;
    if(len < 0)
        len = 0;
    return len;
}
#endif
