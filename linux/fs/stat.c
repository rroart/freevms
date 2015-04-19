// $Id$
// $Locker$

// Author. Roar Thron�s.
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
    return 0;
}


#if !defined(__alpha__) && !defined(__ia64__) && !defined(__hppa__) && !defined(__x86_64__)

/*
 * For backward compatibility?  Maybe this should be moved
 * into arch/i386 instead?
 */
static int cp_old_stat(struct inode * inode, struct __old_kernel_stat * statbuf)
{
    static int warncount = 5;
    struct __old_kernel_stat tmp;

    if (warncount > 0)
    {
        warncount--;
        printk(KERN_WARNING "VFS: Warning: %s using old stat() call. Recompile your binary.\n",
               current->pcb$t_lname);
    }
    else if (warncount < 0)
    {
        /* it's laughable, but... */
        warncount = 0;
    }

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
    return copy_to_user(statbuf,&tmp,sizeof(tmp)) ? -EFAULT : 0;
}

#endif

static int cp_new_stat(struct inode * inode, struct stat * statbuf)
{
    struct stat tmp;
    unsigned int blocks, indirect;

    memset(&tmp, 0, sizeof(tmp));
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
    return copy_to_user(statbuf,&tmp,sizeof(tmp)) ? -EFAULT : 0;
}


#if !defined(__alpha__) && !defined(__ia64__) && !defined(__hppa__) && !defined(__x86_64__)
/*
 * For backward compatibility?  Maybe this should be moved
 * into arch/i386 instead?
 */
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

#if !defined(__alpha__) && !defined(__ia64__) && !defined(__hppa__) && !defined(__x86_64__)

/*
 * For backward compatibility?  Maybe this should be moved
 * into arch/i386 instead?
 */
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

asmlinkage long sys_newlstat(char * filename, struct stat * statbuf)
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

#if !defined(__alpha__) && !defined(__ia64__) && !defined(__hppa__) && !defined(__x86_64__)

/*
 * For backward compatibility?  Maybe this should be moved
 * into arch/i386 instead?
 */
asmlinkage long sys_fstat(unsigned int fd, struct __old_kernel_stat * statbuf)
{
    struct nameidata nd;
    int error = 0;

    void * f = fget(fd);
    if (f==0)
        error = -ENOENT;
    if (!error)
        error = cp_old_stat(f, statbuf);
    return error;
}

#endif

asmlinkage long sys_newfstat(unsigned int fd, struct stat * statbuf)
{
    struct nameidata nd;
    int error = 0;

    void * f = fget(fd);
    if (f==0)
        error = -ENOENT;
    if (!error)
        error = cp_new_stat(f, statbuf);
    return error;
}

asmlinkage long sys_readlink(const char * path, char * buf, int bufsiz)
{
    return -EPERM;
}


/* ---------- LFS-64 ----------- */
#if !defined(__alpha__) && !defined(__ia64__) && !defined(__mips64) && !defined(__x86_64__)

static long cp_new_stat64(struct inode * inode, struct stat64 * statbuf)
{
    struct stat64 tmp;
    unsigned int blocks, indirect;

    memset(&tmp, 0, sizeof(tmp));
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
    return copy_to_user(statbuf,&tmp,sizeof(tmp)) ? -EFAULT : 0;
}

asmlinkage long sys_stat64(char * filename, struct stat64 * statbuf, long flags)
{
    struct nameidata nd;
    int error = 0;

    int fd = sys_open(filename, 0, 0);
    void * f = fget(fd);
    extern int mount_root_vfs;
    if (mount_root_vfs==0 && f==0)
    {
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
}

asmlinkage long sys_lstat64(char * filename, struct stat64 * statbuf, long flags)
{
    struct nameidata nd;
    int error;

    error = 0;
    return error;
}

asmlinkage long sys_fstat64(unsigned long fd, struct stat64 * statbuf, long flags)
{
#if 0
    extern int mount_root_vfs;
    if (mount_root_vfs==0)
    {
        return 0;
    }
#endif
    if (fd<3)
    {
        statbuf->st_mode = 0020000 | 0620;
        statbuf->st_rdev = (4 << 8) + 1;
        return 0;
    }
    struct file * f;
    int err = -EBADF;

    f = fget(fd);
    if (f)
    {
        err = cp_new_stat64(f, statbuf);
    }
    return err;
}

#endif /* LFS-64 */
