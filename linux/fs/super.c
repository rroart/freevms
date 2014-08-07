// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004  

/*
 *  linux/fs/super.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  super.c contains code to handle: - mount structures
 *                                   - super-block tables
 *                                   - filesystem drivers list
 *                                   - mount system call
 *                                   - umount system call
 *                                   - ustat system call
 *
 * GK 2/5/95  -  Changed to support mounting the root fs via NFS
 *
 *  Added kerneld support: Jacques Gelinas and Bjorn Ekwall
 *  Added change_root: Werner Almesberger & Hans Lermen, Feb '96
 *  Added options to /proc/mounts:
 *    Torbjörn Lindh (torbjorn.lindh@gopta.se), April 14, 1996.
 *  Added devfs support: Richard Gooch <rgooch@atnf.csiro.au>, 13-JAN-1998
 *  Heavily rewritten for 'one fs - one tree' dcache architecture. AV, Mar 2000
 */

#include <linux/config.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/locks.h>
#include <linux/smp_lock.h>
#include <linux/devfs_fs_kernel.h>
#include <linux/fd.h>
#include <linux/init.h>
#include <linux/major.h>
#include <linux/quotaops.h>
#include <linux/acct.h>

#include <asm/uaccess.h>

#include <linux/nfs_fs.h>
#include <linux/nfs_fs_sb.h>
#include <linux/nfs_mount.h>

#include <linux/kmod.h>
#define __NO_VERSION__
#include <linux/module.h>

extern void wait_for_keypress(void);

extern int root_mountflags;

int do_remount_sb(struct super_block *sb, int flags, void * data);

/* this is initialized in init/main.c */
kdev_t ROOT_DEV;

LIST_HEAD(super_blocks);
spinlock_t sb_lock = SPIN_LOCK_UNLOCKED;

asmlinkage long sys_sysfs(int option, unsigned long arg1, unsigned long arg2)
{
	int retval = -EINVAL;
	return retval;
}

asmlinkage long sys_ustat(dev_t dev, struct ustat * ubuf)
{
	int err = -EINVAL;
	return err;
}
