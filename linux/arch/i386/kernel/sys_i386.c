/*
 * linux/arch/i386/kernel/sys_i386.c
 *
 * This file contains various random system calls that
 * have a non-standard calling sequence on the Linux/i386
 * platform.
 */

#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/smp_lock.h>
#include <linux/sem.h>
#include <linux/msg.h>
#include <linux/shm.h>
#include <linux/stat.h>
#include <linux/mman.h>
#include <linux/file.h>
#include <linux/utsname.h>

#include <asm/uaccess.h>
#include <asm/ipc.h>

#include <vfddef.h>

/*
 * sys_pipe() is the normal C calling standard for creating
 * a pipe. It's not the way Unix traditionally does this, though.
 */
asmlinkage int sys_pipe(unsigned long * fildes)
{
	return -EPERM;
}

/* common code for old and new mmaps */
static inline long do_mmap2(
	unsigned long addr, unsigned long len,
	unsigned long prot, unsigned long flags,
	unsigned long fd, unsigned long pgoff)
{
	int error = -EBADF;
	struct file * file = NULL;
	struct vms_fd * vms_fd;

	flags &= ~(MAP_EXECUTABLE | MAP_DENYWRITE);
	if (!(flags & MAP_ANONYMOUS)) {
		vms_fd = fget(fd);
		file = vms_fd->vfd$l_fd_p;
		if (!file)
			goto out;
	}

	down_write(&current->mm->mmap_sem);
	error = do_mmap_pgoff(file, addr, len, prot, flags, pgoff);
	up_write(&current->mm->mmap_sem);

out:
	return error;
}

asmlinkage long sys_mmap2(unsigned long addr, unsigned long len,
	unsigned long prot, unsigned long flags,
	unsigned long fd, unsigned long pgoff)
{
	return do_mmap2(addr, len, prot, flags, fd, pgoff);
}

/*
 * Perform the select(nd, in, out, ex, tv) and mmap() system
 * calls. Linux/i386 didn't use to be able to handle more than
 * 4 system call parameters, so these system calls used a memory
 * block for parameter passing..
 */

struct mmap_arg_struct {
	unsigned long addr;
	unsigned long len;
	unsigned long prot;
	unsigned long flags;
	unsigned long fd;
	unsigned long offset;
};

asmlinkage int old_mmap(struct mmap_arg_struct *arg)
{
	struct mmap_arg_struct a;
	int err = -EFAULT;

	if (copy_from_user(&a, arg, sizeof(a)))
		goto out;

	err = -EINVAL;
	if (a.offset & ~PAGE_MASK)
		goto out;

	err = do_mmap2(a.addr, a.len, a.prot, a.flags, a.fd, a.offset >> PAGE_SHIFT);
out:
	return err;
}


extern asmlinkage int sys_select(int, fd_set *, fd_set *, fd_set *, struct timeval *);

struct sel_arg_struct {
	unsigned long n;
	fd_set *inp, *outp, *exp;
	struct timeval *tvp;
};

asmlinkage int old_select(struct sel_arg_struct *arg)
{
	struct sel_arg_struct a;

	if (copy_from_user(&a, arg, sizeof(a)))
		return -EFAULT;
	/* sys_select() does the appropriate kernel locking */
	return sys_select(a.n, a.inp, a.outp, a.exp, a.tvp);
}

/*
 * sys_ipc() is the de-multiplexer for the SysV IPC calls..
 *
 * This is really horribly ugly.
 */
asmlinkage int sys_ipc (uint call, int first, int second,
			int third, void *ptr, long fifth)
{
	return -EPERM;
}

/*
 * Old cruft
 */
asmlinkage int sys_uname(struct old_utsname * name)
{
	int err;
	if (!name)
		return -EFAULT;
	down_read(&uts_sem);
	err=copy_to_user(name, &system_utsname, sizeof (*name));
	up_read(&uts_sem);
	return err?-EFAULT:0;
}

asmlinkage int sys_olduname(struct oldold_utsname * name)
{
	int error;

	if (!name)
		return -EFAULT;
	if (!access_ok(VERIFY_WRITE,name,sizeof(struct oldold_utsname)))
		return -EFAULT;
  
  	down_read(&uts_sem);
	
	error = __copy_to_user(&name->sysname,&system_utsname.sysname,__OLD_UTS_LEN);
	error |= __put_user(0,name->sysname+__OLD_UTS_LEN);
	error |= __copy_to_user(&name->nodename,&system_utsname.nodename,__OLD_UTS_LEN);
	error |= __put_user(0,name->nodename+__OLD_UTS_LEN);
	error |= __copy_to_user(&name->release,&system_utsname.release,__OLD_UTS_LEN);
	error |= __put_user(0,name->release+__OLD_UTS_LEN);
	error |= __copy_to_user(&name->version,&system_utsname.version,__OLD_UTS_LEN);
	error |= __put_user(0,name->version+__OLD_UTS_LEN);
	error |= __copy_to_user(&name->machine,&system_utsname.machine,__OLD_UTS_LEN);
	error |= __put_user(0,name->machine+__OLD_UTS_LEN);
	
	up_read(&uts_sem);
	
	error = error ? -EFAULT : 0;

	return error;
}

asmlinkage int sys_pause(void)
{
	current->state = TASK_INTERRUPTIBLE;
	schedule();
	return -ERESTARTNOHAND;
}

