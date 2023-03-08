// $Id$
// $Locker$

// Author. Roar Thron�s.
// Modified Linux source file, 2001-2006

/*
 * linux/arch/x86_64/kernel/sys_x86_64.c
 */

#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/smp_lock.h>
#include <linux/sem.h>
#include <linux/msg.h>
#include <linux/stat.h>
#include <linux/mman.h>
#include <linux/file.h>
#include <linux/utsname.h>
#include <linux/personality.h>

#include <asm/uaccess.h>
#include <asm/ipc.h>

#include <vfddef.h>

/*
 * sys_pipe() is the normal C calling standard for creating
 * a pipe. It's not the way Unix traditionally does this, though.
 */
asmlinkage long sys_pipe(unsigned long * fildes)
{
    return -EPERM;
}

long sys_mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags,
              unsigned long fd, unsigned long off)
{
    long error;
    struct file * file;
    struct vms_fd * vms_fd;

    error = -EINVAL;
    if (off & ~PAGE_MASK)
        goto out;

    error = -EBADF;
    file = NULL;
    flags &= ~(MAP_EXECUTABLE | MAP_DENYWRITE);
    if (!(flags & MAP_ANONYMOUS))
    {
        vms_fd = fget(fd);
        file = vms_fd->vfd$l_fd_p;
        if (!file)
            goto out;
    }

    down_write(&current->mm->mmap_sem);
    error = do_mmap_pgoff(file, addr, len, prot, flags, off >> PAGE_SHIFT);
    up_write(&current->mm->mmap_sem);

out:
    return error;
}


unsigned long arch_get_unmapped_area(struct file *filp, unsigned long addr, unsigned long len, unsigned long pgoff, unsigned long flags)
{
    struct _rde *vma;
    unsigned long end = TASK_SIZE;

    if (current->thread.flags & THREAD_IA32)
    {
        if (!addr)
            addr = TASK_UNMAPPED_32;
        end = 0xffff0000;
    }
    else if (flags & MAP_32BIT)
    {
        /* This is usually used needed to map code in small
           model: it needs to be in the first 31bit. Limit it
           to that.  This means we need to move the unmapped
           base down for this case.  This may give conflicts
           with the heap, but we assume that malloc falls back
           to mmap. Give it 1GB of playground for now. -AK */
        if (!addr)
            addr = 0x40000000;
        end = 0x80000000;
    }
    else
    {
        if (!addr)
            addr = TASK_UNMAPPED_64;
        end = TASK_SIZE;
    }

    if (len > end)
        return -ENOMEM;
    addr = PAGE_ALIGN(addr);

    for (vma = find_vma(current->pcb$l_phd, addr); ; vma = vma->rde$ps_va_list_flink)
    {
        /* At this point:  (!vma || addr < vma->vm_end). */
        if (end - len < addr)
            return -ENOMEM;
        if (!vma || addr + len <= vma->rde$pq_start_va)
            return addr;
        addr = vma->rde$pq_start_va + vma->rde$q_region_size;
    }
}

asmlinkage long sys_uname(struct new_utsname * name)
{
    int err;
    down_read(&uts_sem);
    err=copy_to_user(name, &system_utsname, sizeof (*name));
    up_read(&uts_sem);
    if (personality(current->personality) == PER_LINUX32)
        err = copy_to_user(name->machine, "i686", 5);
    return err?-EFAULT:0;
}

asmlinkage long sys_pause(void)
{
    current->state = TASK_INTERRUPTIBLE;
    schedule();
    return -ERESTARTNOHAND;
}

extern asmlinkage long sys_shmat (int shmid, char *shmaddr, int shmflg, ulong *raddr);
asmlinkage long wrap_sys_shmat(int shmid, char *shmaddr, int shmflg)
{
    unsigned long raddr;
    return sys_shmat(shmid,shmaddr,shmflg,&raddr) ?: raddr;
}

asmlinkage long sys_time64(long * tloc)
{
    struct timeval now;
    int i;

    do_gettimeofday(&now);
    i = now.tv_sec;
    if (tloc)
    {
        if (put_user(i,tloc))
            i = -EFAULT;
    }
    return i;
}
