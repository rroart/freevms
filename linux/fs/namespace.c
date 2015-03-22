// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004.

/*
 *  linux/fs/namespace.c
 *
 * (C) Copyright Al Viro 2000, 2001
 *  Released under GPL v2.
 *
 * Based on code from fs/super.c, copyright Linus Torvalds and others.
 * Heavily rewritten.
 */

#include <linux/config.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/init.h>
#include <linux/quotaops.h>
#include <linux/acct.h>
#include <linux/module.h>

#include <asm/uaccess.h>

#include <linux/seq_file.h>

struct vfsmount *do_kern_mount(char *type, int flags, char *name, void *data);
int do_remount_sb(struct super_block *sb, int flags, void * data);
void kill_super(struct super_block *sb);

static struct list_head *mount_hashtable;
static int hash_mask, hash_bits;
static kmem_cache_t *mnt_cache;

static LIST_HEAD(vfsmntlist);
static DECLARE_MUTEX(mount_sem);

/* Will be static */
struct vfsmount *root_vfsmnt;

static inline unsigned long hash(struct vfsmount *mnt, struct dentry *dentry)
{
    unsigned long tmp = ((unsigned long) mnt / L1_CACHE_BYTES);
    tmp += ((unsigned long) dentry / L1_CACHE_BYTES);
    tmp = tmp + (tmp >> hash_bits);
    return tmp & hash_mask;
}

/*
 * Now umount can handle mount points as well as block devices.
 * This is important for filesystems which use unnamed block devices.
 *
 * We now support a flag for forced unmount like the other 'big iron'
 * unixes. Our API is identical to OSF/1 to avoid making a mess of AMD
 */

asmlinkage long sys_umount(char * name, int flags)
{
    return -EPERM;
}

/*
 *  The 2.0 compatible umount. No flags.
 */

asmlinkage long sys_oldumount(char * name)
{
    return sys_umount(name,0);
}

asmlinkage long sys_mount(char * dev_name, char * dir_name, char * type,
                          unsigned long flags, void * data)
{
    return -EPERM;
}

/*
 * Moves the current root to put_root, and sets root/cwd of all processes
 * which had them on the old root to new_root.
 *
 * Note:
 *  - we don't move root/cwd if they are not at the root (reason: if something
 *    cared enough to change them, it's probably wrong to force them elsewhere)
 *  - it's okay to pick a root that isn't the root of a file system, e.g.
 *    /nfs/my_root where /nfs is the mount point. It must be a mountpoint,
 *    though, so you may need to say mount --bind /nfs/my_root /nfs/my_root
 *    first.
 */

asmlinkage long sys_pivot_root(const char *new_root, const char *put_old)
{
    return -EPERM;
}

