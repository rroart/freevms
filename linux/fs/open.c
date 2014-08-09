// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004.

/*
 *  linux/fs/open.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <linux/string.h>
#include <linux/mm.h>
#include <linux/utime.h>
#include <linux/file.h>
#include <linux/smp_lock.h>
#include <linux/quotaops.h>
#include <linux/dnotify.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/iobuf.h>

#include <asm/uaccess.h>

#include <dyndef.h>
#include <fabdef.h>
#include <rabdef.h>
#include <xabdatdef.h>
#include <xabfhcdef.h>
#include <exe_routines.h>
#include <misc_routines.h>

#include <vfddef.h>

#define special_file(m) (S_ISCHR(m)||S_ISBLK(m)||S_ISFIFO(m)||S_ISSOCK(m))

int vfs_statfs(struct super_block *sb, struct statfs *buf)
{
	int retval = -ENODEV;

	if (sb) {
		retval = -ENOSYS;
		if (sb->s_op && sb->s_op->statfs) {
			memset(buf, 0, sizeof(struct statfs));
			lock_kernel();
			retval = sb->s_op->statfs(sb, buf);
			unlock_kernel();
		}
	}
	return retval;
}


asmlinkage long sys_statfs(const char * path, struct statfs * buf)
{
	return -EPERM;
}

asmlinkage long sys_fstatfs(unsigned int fd, struct statfs * buf)
{
	return -EPERM;
}

int do_truncate(struct dentry *dentry, loff_t length)
{
	return -EINVAL;
}

static inline long do_sys_truncate(const char * path, loff_t length)
{
	struct nameidata nd;
	struct inode * inode;
	int error;

	error = -EINVAL;
	if (length < 0)	/* sorry, but loff_t says... */
		goto out;

	error = user_path_walk(path, &nd);
	if (error)
		goto out;
	inode = nd.dentry->d_inode;

	/* For directories it's -EISDIR, for other non-regulars - -EINVAL */
	error = -EISDIR;
	if (S_ISDIR(inode->i_mode))
		goto dput_and_out;

	error = -EINVAL;
	if (!S_ISREG(inode->i_mode))
		goto dput_and_out;

	error = permission(inode,MAY_WRITE);
	if (error)
		goto dput_and_out;

	error = -EROFS;
	if (IS_RDONLY(inode))
		goto dput_and_out;

	error = -EPERM;
	if (IS_IMMUTABLE(inode) || IS_APPEND(inode))
		goto dput_and_out;

	/*
	 * Make sure that there are no leases.
	 */
	error = get_lease(inode, FMODE_WRITE);
	if (error)
		goto dput_and_out;

	error = get_write_access(inode);
	if (error)
		goto dput_and_out;

	error = locks_verify_truncate(inode, NULL, length);
	if (!error) {
		DQUOT_INIT(inode);
		error = do_truncate(nd.dentry, length);
	}
	put_write_access(inode);

dput_and_out:
	path_release(&nd);
out:
	return error;
}

asmlinkage long sys_truncate(const char * path, unsigned long length)
{
	return -EPERM;
}

static inline long do_sys_ftruncate(unsigned int fd, loff_t length, int small)
{
	struct inode * inode;
	struct dentry *dentry;
	struct file * file;
	struct vms_fd * vms_fd;
	int error;

	error = -EINVAL;
	if (length < 0)
		goto out;
	error = -EBADF;
	vms_fd = fget(fd);
	file = vms_fd->vfd$l_fd_p;
	if (!file)
		goto out;

	/* explicitly opened as large or we are on 64-bit box */
	if (file->f_flags & O_LARGEFILE)
		small = 0;

	dentry = file->f_dentry;
	inode = dentry->d_inode;
	error = -EINVAL;
	if (!S_ISREG(inode->i_mode) || !(file->f_mode & FMODE_WRITE))
		goto out_putf;

	error = -EINVAL;
	/* Cannot ftruncate over 2^31 bytes without large file support */
	if (small && length > MAX_NON_LFS)
		goto out_putf;

	error = -EPERM;
	if (IS_APPEND(inode))
		goto out_putf;

	error = locks_verify_truncate(inode, file, length);
	if (!error)
		error = do_truncate(dentry, length);
out_putf:
	fput(file);
out:
	return error;
}

asmlinkage long sys_ftruncate(unsigned int fd, unsigned long length)
{
	return -EPERM;
}

/* LFS versions of truncate are only needed on 32 bit machines */
#if BITS_PER_LONG == 32
asmlinkage long sys_truncate64(const char * path, loff_t length)
{
	return -EPERM;
}

asmlinkage long sys_ftruncate64(unsigned int fd, loff_t length)
{
	return -EPERM;
}
#endif

#if !(defined(__alpha__) || defined(__ia64__))

/*
 * sys_utime() can be implemented in user-level using sys_utimes().
 * Is this for backwards compatibility?  If so, why not move it
 * into the appropriate arch directory (for those architectures that
 * need it).
 */

/* If times==NULL, set access and modification to current time,
 * must be owner or have write permission.
 * Else, update from *times, must be owner or super user.
 */
asmlinkage long sys_utime(char * filename, struct utimbuf * times)
{
	return -EPERM;
}

#endif

/* If times==NULL, set access and modification to current time,
 * must be owner or have write permission.
 * Else, update from *times, must be owner or super user.
 */
asmlinkage long sys_utimes(char * filename, struct timeval * utimes)
{
	return -EPERM;
}

/*
 * access() needs to use the real uid/gid, not the effective uid/gid.
 * We do this by temporarily clearing all FS-related capabilities and
 * switching the fsuid/fsgid around to the real ones.
 */
asmlinkage long sys_access(const char * filename, int mode)
{
	int fd, error;
	struct file *file=0;
	int err = 0;
	struct _fabdef fab = cc$rms_fab;
	int sts;

	if (mode & ~S_IRWXO)	/* where's F_OK, X_OK, W_OK, R_OK? */
		return -EINVAL;

	char vms_filename[256];
	path_unix_to_vms(vms_filename, filename);
	convert_soname(vms_filename);

	fab.fab$l_fna = vms_filename;
	fab.fab$b_fns = strlen(fab.fab$l_fna);
	if ((sts = exe$open(&fab)) & 1) {
	  exe$close(&fab);
	} else
	  return -1;
	return 0;

}

asmlinkage long sys_chdir(const char * filename)
{
	return 0;
}

asmlinkage long sys_fchdir(unsigned int fd)
{
	return 0;
}

asmlinkage long sys_chroot(const char * filename)
{
	return -EPERM;
}

asmlinkage long sys_fchmod(unsigned int fd, mode_t mode)
{
	return 0;
}

asmlinkage long sys_chmod(const char * filename, mode_t mode)
{
	return 0;
}

asmlinkage long sys_chown(const char * filename, uid_t user, gid_t group)
{
	return -EPERM;
}

asmlinkage long sys_lchown(const char * filename, uid_t user, gid_t group)
{
	return -EPERM;
}


asmlinkage long sys_fchown(unsigned int fd, uid_t user, gid_t group)
{
	return -EPERM;
}

/*
 * Find an empty file descriptor entry, and mark it busy.
 */
int get_unused_fd(void)
{
	struct files_struct * files = current->files;
	int fd, error;

  	error = -EMFILE;
	write_lock(&files->file_lock);

repeat:
 	fd = find_next_zero_bit(files->open_fds, 
				files->max_fdset, 
				files->next_fd);

	/*
	 * N.B. For clone tasks sharing a files structure, this test
	 * will limit the total number of files that can be opened.
	 */
	if (fd >= current->rlim[RLIMIT_NOFILE].rlim_cur)
		goto out;

	/* Do we need to expand the fdset array? */
	if (fd >= files->max_fdset) {
		error = expand_fdset(files, fd);
		if (!error) {
			error = -EMFILE;
			goto repeat;
		}
		goto out;
	}
	
	/* 
	 * Check whether we need to expand the fd array.
	 */
	if (fd >= files->max_fds) {
		error = expand_fd_array(files, fd);
		if (!error) {
			error = -EMFILE;
			goto repeat;
		}
		goto out;
	}

	FD_SET(fd, files->open_fds);
	FD_CLR(fd, files->close_on_exec);
	files->next_fd = fd + 1;
#if 1
	/* Sanity check */
	if (files->fd[fd] != NULL) {
		printk(KERN_WARNING "get_unused_fd: slot %d not NULL!\n", fd);
		files->fd[fd] = NULL;
	}
#endif
	error = fd;

out:
	write_unlock(&files->file_lock);
	return error;
}

asmlinkage long sys_open(const char * filename, int flags, int mode)
{
	int fd, error;
	struct file *file=0;
	int err = 0;
#if 0
	struct _fabdef fab = cc$rms_fab;
#else
	struct _xabfhcdef cc$rms_xabfhc = {XAB$C_FHC,0,0,0,0,0,0,0,0,0,0,0};
	struct _xabdatdef cc$rms_xabdat={XAB$C_DAT,XAB$C_DATLEN,0,0,0,0,0,0,0,0,0,0};
	struct _fabdef * fab = kmalloc(sizeof(struct _fabdef), GFP_KERNEL);
	struct _rabdef * rab = kmalloc(sizeof(struct _rabdef), GFP_KERNEL);
	// remember too free next two
	struct _xabdatdef * dat = kmalloc(sizeof(struct _xabdatdef), GFP_KERNEL);
	struct _xabfhcdef * fhc = kmalloc(sizeof(struct _xabfhcdef), GFP_KERNEL);
	*fab = cc$rms_fab;
	*rab = cc$rms_rab;
	*dat = cc$rms_xabdat;
	*fhc = cc$rms_xabfhc;
	fab->fab$l_xab = dat;
	dat->xab$l_nxt = fhc;
#endif
	int sts;

	char vms_filename[256];
	path_unix_to_vms(vms_filename, filename);
	convert_soname(vms_filename);

#if 0
	long prev_xqp_fcb = get_xqp_prim_fcb();
	long prev_x2p_fcb = get_x2p_prim_fcb();

	fab.fab$l_fna = vms_filename;
	fab.fab$b_fns = strlen(fab.fab$l_fna);
	if ((sts = exe$open(&fab)) & 1) {
	  long xqp_fcb = get_xqp_prim_fcb();
	  long x2p_fcb = get_x2p_prim_fcb();
	  extern int mount_root_ext2;
	  if (mount_root_ext2==0 || xqp_fcb!=prev_xqp_fcb)
	    file=xqp_fcb;
	  else
	    file=x2p_fcb;
	  exe$close(&fab);
	} else
	  return -1;
#else
	fab->fab$l_fna = vms_filename;
	fab->fab$b_fns = strlen(fab->fab$l_fna);
	if ((sts = exe$open(fab)) & 1) {
	  rab->rab$l_fab = fab;
	  if ((sts = exe$connect(rab)) & 1) {
	  }
	} else
	  return -1;
#endif
	fd = get_unused_fd();
#if 0
	fd_install(fd, file);
#else
	struct vms_fd * vms_fd = kmalloc(sizeof(struct vms_fd), GFP_KERNEL);
	vms_fd->vfd$l_is_cmu = 0;
	vms_fd->vfd$l_fd_p = rab;
	vms_fd->vfd$l_refcnt = 1;
	fd_install(fd, vms_fd);
#endif
	return fd;
}

#ifndef __alpha__

/*
 * For backward compatibility?  Maybe this should be moved
 * into arch/i386 instead?
 */
asmlinkage long sys_creat(const char * pathname, int mode)
{
	return sys_open(pathname, O_CREAT | O_WRONLY | O_TRUNC, mode);
}

#endif

/*
 * "id" is the POSIX thread ID. We use the
 * files pointer for this..
 */
int filp_close(struct file *filp, fl_owner_t id)
{
	int retval;

#if 0
	if (((struct _fcb *)filp)->fcb$b_type==DYN$C_FCB)
	    return 0;
#else
	return 0;
#endif

	if (!file_count(filp)) {
		printk(KERN_ERR "VFS: Close: file count is 0\n");
		return 0;
	}
	retval = 0;
	if (filp->f_op && filp->f_op->flush) {
		lock_kernel();
		retval = filp->f_op->flush(filp);
		unlock_kernel();
	}
	extern is_tty_fops(struct file * f);
	if (!is_tty_fops(filp)) {
	  locks_remove_posix(filp, id);
	  fput(filp);
	}
	return retval;
}

/*
 * Careful here! We test whether the file pointer is NULL before
 * releasing the fd. This ensures that one clone task can't release
 * an fd while another clone is opening it.
 */
asmlinkage long sys_close(unsigned int fd)
{
	struct file * filp;
	struct vms_fd * vms_fd;
	struct files_struct *files = current->files;

	write_lock(&files->file_lock);
	if (fd >= files->max_fds)
		goto out_unlock;
	vms_fd = files->fd[fd];
	int cmu_close();
	if (vms_fd->vfd$l_is_cmu)
	  return cmu_close;
	filp = vms_fd->vfd$l_fd_p;
	if (!filp)
		goto out_unlock;
	files->fd[fd] = NULL;
	FD_CLR(fd, files->close_on_exec);
	__put_unused_fd(files, fd);
	write_unlock(&files->file_lock);
#if 0
	if (fd<3) return 0; // temp workaround
	if (filp == files->fd[0]) return 0; // another temp workaround
#else
	vms_fd->vfd$l_refcnt--;
	if (vms_fd->vfd$l_refcnt)
	  return 0;
#endif
	struct _rabdef * rab = filp;
	struct _fabdef * fab = rab->rab$l_fab;
	struct _xabdatdef * dat = fab->fab$l_xab;
	struct _xabfhcdef * fhc = 0;
	if (dat)
	  fhc = dat->xab$l_nxt;
	exe$disconnect(rab);
	exe$close(fab);
	kfree(vms_fd);
	kfree(rab);
	kfree(fab);
	if (dat)
	  kfree(dat);
	if (fhc)
	  kfree(fhc);
	return 0;

out_unlock:
	write_unlock(&files->file_lock);
	return -EBADF;
}

/*
 * This routine simulates a hangup on the tty, to arrange that users
 * are given clean terminals at login time.
 */
asmlinkage long sys_vhangup(void)
{
	if (capable(CAP_SYS_TTY_CONFIG)) {
		tty_vhangup(current->tty);
		return 0;
	}
	return -EPERM;
}

/*
 * Called when an inode is about to be open.
 * We use this to disallow opening RW large files on 32bit systems if
 * the caller didn't specify O_LARGEFILE.  On 64bit systems we force
 * on this flag in sys_open.
 */
int generic_file_open(struct _fcb * inode, struct file * filp)
{
#if 0
	if (!(filp->f_flags & O_LARGEFILE) && inode->i_size > MAX_NON_LFS)
		return -EFBIG;
#endif
	return 0;
}

void sys_open_term(char * name)
{
	  struct _fabdef * fab = kmalloc(sizeof(struct _fabdef), GFP_KERNEL);
	  struct _rabdef * rab = kmalloc(sizeof(struct _rabdef), GFP_KERNEL);
	  *fab = cc$rms_fab;
	  *rab = cc$rms_rab;
	  fab->fab$l_fna = name;
	  fab->fab$b_fns = strlen(fab->fab$l_fna);
	  exe$open(fab);
	  rab->rab$l_fab = fab;
	  exe$connect(rab);
	  int fd = get_unused_fd();
	  struct vms_fd * vms_fd = kmalloc(sizeof(struct vms_fd), GFP_KERNEL);
	  vms_fd->vfd$l_is_cmu = 0;
	  vms_fd->vfd$l_fd_p = rab;
	  vms_fd->vfd$l_refcnt = 1;
	  fd_install(fd, vms_fd);
}

EXPORT_SYMBOL(generic_file_open);
