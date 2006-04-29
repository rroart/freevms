// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004.

/*
 *  linux/fs/stat.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <linux/config.h>
#include <linux/mm.h>
#include <linux/errno.h>
#include <linux/file.h>
#include <linux/smp_lock.h>
#include <linux/highuid.h>

#include <asm/uaccess.h>

#include <linux/ext2_fs.h>
#include <fcbdef.h>

/*
 * Revalidate the inode. This is required for proper NFS attribute caching.
 */
static __inline__ int
do_revalidate(struct dentry *dentry)
{
#ifndef CONFIG_VMS 
	struct inode * inode = dentry->d_inode;
	if (inode->i_op && inode->i_op->revalidate)
		return inode->i_op->revalidate(dentry);
	return 0;
#else
	return 0;
#endif
}


#if !defined(__alpha__) && !defined(__sparc__) && !defined(__ia64__) && !defined(CONFIG_ARCH_S390) && !defined(__hppa__) && !defined(__x86_64__)

/*
 * For backward compatibility?  Maybe this should be moved
 * into arch/i386 instead?
 */
static int cp_old_stat(struct inode * inode, struct __old_kernel_stat * statbuf)
{
	static int warncount = 5;
	struct __old_kernel_stat tmp;

	if (warncount > 0) {
		warncount--;
		printk(KERN_WARNING "VFS: Warning: %s using old stat() call. Recompile your binary.\n",
			current->pcb$t_lname);
	} else if (warncount < 0) {
		/* it's laughable, but... */
		warncount = 0;
	}

#ifndef CONFIG_VMS
	tmp.st_dev = kdev_t_to_nr(inode->i_dev);
	tmp.st_ino = inode->i_ino;
	tmp.st_mode = inode->i_mode;
	tmp.st_nlink = inode->i_nlink;
	SET_OLDSTAT_UID(tmp, inode->i_uid);
	SET_OLDSTAT_GID(tmp, inode->i_gid);
	tmp.st_rdev = kdev_t_to_nr(inode->i_rdev);
#if BITS_PER_LONG == 32
	if (inode->i_size > MAX_NON_LFS)
		return -EOVERFLOW;
#endif	
	tmp.st_size = inode->i_size;
	tmp.st_atime = inode->i_atime;
	tmp.st_mtime = inode->i_mtime;
	tmp.st_ctime = inode->i_ctime;
#else
	struct _fcb * fcb = inode;
	tmp.st_dev = 0 ; // fcb->fcb$w_fid_dirnum;
	tmp.st_ino = FCB_FID_TO_INO(fcb);
	tmp.st_mode = 0755;
	if (fcb->fcb$v_dir) 
	  tmp.st_mode |= S_IFDIR;
	tmp.st_nlink = 1;
	SET_OLDSTAT_UID(tmp, fcb->fcb$w_uicmember);
	SET_OLDSTAT_GID(tmp, fcb->fcb$w_uicgroup);
	tmp.st_rdev = 0; // fcb->fcb$w_fid_dirnum;
	tmp.st_size = fcb->fcb$l_filesize;
	tmp.st_atime = 0;
	tmp.st_mtime = 0;
	tmp.st_ctime = 0;
#endif
	return copy_to_user(statbuf,&tmp,sizeof(tmp)) ? -EFAULT : 0;
}

#endif

static int cp_new_stat(struct inode * inode, struct stat * statbuf)
{
	struct stat tmp;
	unsigned int blocks, indirect;

	memset(&tmp, 0, sizeof(tmp));
#ifndef CONFIG_VMS
	tmp.st_dev = kdev_t_to_nr(inode->i_dev);
	tmp.st_ino = inode->i_ino;
	tmp.st_mode = inode->i_mode;
	tmp.st_nlink = inode->i_nlink;
	SET_STAT_UID(tmp, inode->i_uid);
	SET_STAT_GID(tmp, inode->i_gid);
	tmp.st_rdev = kdev_t_to_nr(inode->i_rdev);
#if BITS_PER_LONG == 32
	if (inode->i_size > MAX_NON_LFS)
		return -EOVERFLOW;
#endif	
	tmp.st_size = inode->i_size;
	tmp.st_atime = inode->i_atime;
	tmp.st_mtime = inode->i_mtime;
	tmp.st_ctime = inode->i_ctime;
/*
 * st_blocks and st_blksize are approximated with a simple algorithm if
 * they aren't supported directly by the filesystem. The minix and msdos
 * filesystems don't keep track of blocks, so they would either have to
 * be counted explicitly (by delving into the file itself), or by using
 * this simple algorithm to get a reasonable (although not 100% accurate)
 * value.
 */

/*
 * Use minix fs values for the number of direct and indirect blocks.  The
 * count is now exact for the minix fs except that it counts zero blocks.
 * Everything is in units of BLOCK_SIZE until the assignment to
 * tmp.st_blksize.
 */
#define D_B   7
#define I_B   (BLOCK_SIZE / sizeof(unsigned short))

	if (!inode->i_blksize) {
		blocks = (tmp.st_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
		if (blocks > D_B) {
			indirect = (blocks - D_B + I_B - 1) / I_B;
			blocks += indirect;
			if (indirect > 1) {
				indirect = (indirect - 1 + I_B - 1) / I_B;
				blocks += indirect;
				if (indirect > 1)
					blocks++;
			}
		}
		tmp.st_blocks = (BLOCK_SIZE / 512) * blocks;
		tmp.st_blksize = BLOCK_SIZE;
	} else {
		tmp.st_blocks = inode->i_blocks;
		tmp.st_blksize = inode->i_blksize;
	}
#else
	struct _fcb * fcb = inode;
	tmp.st_dev = 0; // fcb->fcb$w_fid_dirnum;
	tmp.st_ino = FCB_FID_TO_INO(fcb);
	tmp.st_mode = 0755;
	if (fcb->fcb$v_dir) 
	  tmp.st_mode |= S_IFDIR;
	tmp.st_nlink = 1;
	SET_STAT_UID(tmp, fcb->fcb$w_uicmember);
	SET_STAT_GID(tmp, fcb->fcb$w_uicgroup);
	tmp.st_rdev = 0; // fcb->fcb$w_fid_dirnum;
	tmp.st_size = fcb->fcb$l_filesize;
	tmp.st_atime = 0;
	tmp.st_mtime = 0;
	tmp.st_ctime = 0;
#endif
	return copy_to_user(statbuf,&tmp,sizeof(tmp)) ? -EFAULT : 0;
}


#if !defined(__alpha__) && !defined(__sparc__) && !defined(__ia64__) && !defined(CONFIG_ARCH_S390) && !defined(__hppa__) && !defined(__x86_64__)
/*
 * For backward compatibility?  Maybe this should be moved
 * into arch/i386 instead?
 */
#ifndef CONFIG_VMS
asmlinkage long sys_stat(char * filename, struct __old_kernel_stat * statbuf)
{
	struct nameidata nd;
	int error;

	error = user_path_walk(filename, &nd);
	if (!error) {
		error = do_revalidate(nd.dentry);
		if (!error)
			error = cp_old_stat(nd.dentry->d_inode, statbuf);
		path_release(&nd);
	}
	return error;
}
#else
asmlinkage long sys_stat(char * filename, struct __old_kernel_stat * statbuf)
{
	struct nameidata nd;
	int error = 0;

	int fd = sys_open(filename, 0, 0);
	void * f = fget(fd);
	if (f==0)
	  error = -ENOENT;
	if (!error)
	  error = cp_old_stat(f, statbuf);
	return error;
}
#endif
#endif

#ifndef CONFIG_VMS
asmlinkage long sys_newstat(char * filename, struct stat * statbuf)
{
	struct nameidata nd;
	int error;

	error = user_path_walk(filename, &nd);
	if (!error) {
		error = do_revalidate(nd.dentry);
		if (!error)
			error = cp_new_stat(nd.dentry->d_inode, statbuf);
		path_release(&nd);
	}
	return error;
}
#else
asmlinkage long sys_newstat(char * filename, struct stat * statbuf)
{
	struct nameidata nd;
	int error = 0;

	int fd = sys_open(filename, 0, 0);
	void * f = fget(fd);
	if (f==0)
	  error = -ENOENT;
	if (!error)
	  error = cp_new_stat(f, statbuf);
	return error;
}
#endif

#if !defined(__alpha__) && !defined(__sparc__) && !defined(__ia64__) && !defined(CONFIG_ARCH_S390) && !defined(__hppa__) && !defined(__x86_64__)

/*
 * For backward compatibility?  Maybe this should be moved
 * into arch/i386 instead?
 */
#ifndef CONFIG_VMS
asmlinkage long sys_lstat(char * filename, struct __old_kernel_stat * statbuf)
{
	struct nameidata nd;
	int error;

	error = user_path_walk_link(filename, &nd);
	if (!error) {
		error = do_revalidate(nd.dentry);
		if (!error)
			error = cp_old_stat(nd.dentry->d_inode, statbuf);
		path_release(&nd);
	}
	return error;
}
#else
asmlinkage long sys_lstat(char * filename, struct __old_kernel_stat * statbuf)
{
	struct nameidata nd;
	int error = 0;

	int fd = sys_open(filename, 0, 0);
	void * f = fget(fd);
	if (f==0)
	  error = -ENOENT;
	if (!error)
	  error = cp_old_stat(f, statbuf);
	return error;
}
#endif
#endif

asmlinkage long sys_newlstat(char * filename, struct stat * statbuf)
{
#ifndef CONFIG_VMS
	struct nameidata nd;
	int error;

	error = user_path_walk_link(filename, &nd);
	if (!error) {
		error = do_revalidate(nd.dentry);
		if (!error)
			error = cp_new_stat(nd.dentry->d_inode, statbuf);
		path_release(&nd);
	}
	return error;
#else
	struct nameidata nd;
	int error = 0;

	int fd = sys_open(filename, 0, 0);
	void * f = fget(fd);
	if (f==0)
	  error = -ENOENT;
	if (!error)
	  error = cp_new_stat(f, statbuf);
	return error;
#endif
}

#if !defined(__alpha__) && !defined(__sparc__) && !defined(__ia64__) && !defined(CONFIG_ARCH_S390) && !defined(__hppa__) && !defined(__x86_64__)

/*
 * For backward compatibility?  Maybe this should be moved
 * into arch/i386 instead?
 */
asmlinkage long sys_fstat(unsigned int fd, struct __old_kernel_stat * statbuf)
{
#ifndef CONFIG_VMS
	struct file * f;
	int err = -EBADF;

	f = fget(fd);
	if (f) {
		struct dentry * dentry = f->f_dentry;

		err = do_revalidate(dentry);
		if (!err)
			err = cp_old_stat(dentry->d_inode, statbuf);
		fput(f);
	}
	return err;
#else
	struct nameidata nd;
	int error = 0;

	void * f = fget(fd);
	if (f==0)
	  error = -ENOENT;
	if (!error)
	  error = cp_old_stat(f, statbuf);
	return error;
#endif
}

#endif

asmlinkage long sys_newfstat(unsigned int fd, struct stat * statbuf)
{
#ifndef CONFIG_VMS
	struct file * f;
	int err = -EBADF;

	f = fget(fd);
	if (f) {
		struct dentry * dentry = f->f_dentry;

		err = do_revalidate(dentry);
		if (!err)
			err = cp_new_stat(dentry->d_inode, statbuf);
		fput(f);
	}
	return err;
#else
	struct nameidata nd;
	int error = 0;

	void * f = fget(fd);
	if (f==0)
	  error = -ENOENT;
	if (!error)
	  error = cp_new_stat(f, statbuf);
	return error;
#endif
}

asmlinkage long sys_readlink(const char * path, char * buf, int bufsiz)
{
#ifndef CONFIG_VMS
	struct nameidata nd;
	int error;

	if (bufsiz <= 0)
		return -EINVAL;

	error = user_path_walk_link(path, &nd);
	if (!error) {
		struct inode * inode = nd.dentry->d_inode;

		error = -EINVAL;
		if (inode->i_op && inode->i_op->readlink &&
		    !(error = do_revalidate(nd.dentry))) {
			UPDATE_ATIME(inode);
			error = inode->i_op->readlink(nd.dentry, buf, bufsiz);
		}
		path_release(&nd);
	}
	return error;
#else
	return -EPERM;
#endif
}


/* ---------- LFS-64 ----------- */
#if !defined(__alpha__) && !defined(__ia64__) && !defined(__mips64) && !defined(__x86_64__) && !defined(CONFIG_ARCH_S390X)

static long cp_new_stat64(struct inode * inode, struct stat64 * statbuf)
{
	struct stat64 tmp;
	unsigned int blocks, indirect;

	memset(&tmp, 0, sizeof(tmp));
#ifndef CONFIG_VMS 
	tmp.st_dev = kdev_t_to_nr(inode->i_dev);
	tmp.st_ino = inode->i_ino;
#ifdef STAT64_HAS_BROKEN_ST_INO
	tmp.__st_ino = inode->i_ino;
#endif
	tmp.st_mode = inode->i_mode;
	tmp.st_nlink = inode->i_nlink;
	tmp.st_uid = inode->i_uid;
	tmp.st_gid = inode->i_gid;
	tmp.st_rdev = kdev_t_to_nr(inode->i_rdev);
	tmp.st_atime = inode->i_atime;
	tmp.st_mtime = inode->i_mtime;
	tmp.st_ctime = inode->i_ctime;
	tmp.st_size = inode->i_size;
/*
 * st_blocks and st_blksize are approximated with a simple algorithm if
 * they aren't supported directly by the filesystem. The minix and msdos
 * filesystems don't keep track of blocks, so they would either have to
 * be counted explicitly (by delving into the file itself), or by using
 * this simple algorithm to get a reasonable (although not 100% accurate)
 * value.
 */

/*
 * Use minix fs values for the number of direct and indirect blocks.  The
 * count is now exact for the minix fs except that it counts zero blocks.
 * Everything is in units of BLOCK_SIZE until the assignment to
 * tmp.st_blksize.
 */
#define D_B   7
#define I_B   (BLOCK_SIZE / sizeof(unsigned short))

	if (!inode->i_blksize) {
		blocks = (tmp.st_size + BLOCK_SIZE - 1) >> BLOCK_SIZE_BITS;
		if (blocks > D_B) {
			indirect = (blocks - D_B + I_B - 1) / I_B;
			blocks += indirect;
			if (indirect > 1) {
				indirect = (indirect - 1 + I_B - 1) / I_B;
				blocks += indirect;
				if (indirect > 1)
					blocks++;
			}
		}
		tmp.st_blocks = (BLOCK_SIZE / 512) * blocks;
		tmp.st_blksize = BLOCK_SIZE;
	} else {
		tmp.st_blocks = inode->i_blocks;
		tmp.st_blksize = inode->i_blksize;
	}
#else
	struct _fcb * fcb = inode;
	tmp.st_dev = 0;
	tmp.st_ino = FCB_FID_TO_INO(fcb);
	tmp.st_mode = 0755;
	if (fcb->fcb$v_dir) 
	  tmp.st_mode |= S_IFDIR;
	tmp.st_nlink = 1;
	tmp.st_uid = fcb->fcb$w_uicmember;
	tmp.st_gid = fcb->fcb$w_uicgroup;
	tmp.st_rdev = 0; // fcb->fcb$w_fid_dirnum;
	tmp.st_size = fcb->fcb$l_filesize;
	tmp.st_atime = 0;
	tmp.st_mtime = 0;
	tmp.st_ctime = 0;
#endif
	return copy_to_user(statbuf,&tmp,sizeof(tmp)) ? -EFAULT : 0;
}

asmlinkage long sys_stat64(char * filename, struct stat64 * statbuf, long flags)
{
#ifndef CONFIG_VMS
	struct nameidata nd;
	int error;

	error = user_path_walk(filename, &nd);
	if (!error) {
		error = do_revalidate(nd.dentry);
		if (!error)
			error = cp_new_stat64(nd.dentry->d_inode, statbuf);
		path_release(&nd);
	}
	return error;
#else
	struct nameidata nd;
	int error = 0;

	int fd = sys_open(filename, 0, 0);
	void * f = fget(fd);
	extern int mount_root_vfs;
	if (mount_root_vfs==0 && f==0) {
	  char c[256];
	  int len = strlen(filename);
	  memcpy(c, filename, len);
	  memcpy(c+len, ".dir", 4);
	  c[len+4]=0;
	  fd = sys_open(c, 0, 0);
	  f = fget(fd);
	}
	if (f==0)
	  error = -ENOENT;
	if (!error)
	  error = cp_new_stat64(f, statbuf);
	return error;
#endif
}

asmlinkage long sys_lstat64(char * filename, struct stat64 * statbuf, long flags)
{
	struct nameidata nd;
	int error;

#ifndef CONFIG_VMS 
	error = user_path_walk_link(filename, &nd);
	if (!error) {
		error = do_revalidate(nd.dentry);
		if (!error)
			error = cp_new_stat64(nd.dentry->d_inode, statbuf);
		path_release(&nd);
	}
#else
	error = 0;
#endif
	return error;
}

asmlinkage long sys_fstat64(unsigned long fd, struct stat64 * statbuf, long flags)
{
#ifdef CONFIG_VMS
#if 0
  extern int mount_root_vfs;
  if (mount_root_vfs==0) {
    return 0;
  }
#endif
  if (fd<3)
    return 0;
#endif
	struct file * f;
	int err = -EBADF;

	f = fget(fd);
	if (f) {
#ifndef CONFIG_VMS 
		struct dentry * dentry = f->f_dentry;

		err = do_revalidate(dentry);
		if (!err)
			err = cp_new_stat64(dentry->d_inode, statbuf);
		fput(f);
#else
		err = cp_new_stat64(f, statbuf);
#endif
	}
	return err;
}

#endif /* LFS-64 */
