// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004.

/*
 *  linux/fs/block_dev.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *  Copyright (C) 2001  Andrea Arcangeli <andrea@suse.de> SuSE
 */

#include <linux/config.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/locks.h>
#include <linux/fcntl.h>
#include <linux/slab.h>
#include <linux/kmod.h>
#include <linux/major.h>
#include <linux/smp_lock.h>
#include <linux/iobuf.h>
#include <linux/highmem.h>
#include <linux/blkdev.h>
#include <linux/module.h>

#include <asm/uaccess.h>

#include <queue.h>

#define MAX_BUF_PER_PAGE (PAGE_CACHE_SIZE / 512)

static unsigned long max_block(kdev_t dev)
{
    unsigned int retval = ~0U;
    int major = MAJOR(dev);

    if (blk_size[major])
    {
        int minor = MINOR(dev);
        unsigned int blocks = blk_size[major][minor];
        if (blocks)
        {
            unsigned int size = block_size(dev);
            unsigned int sizebits = blksize_bits(size);
            blocks += (size-1) >> BLOCK_SIZE_BITS;
            retval = blocks << (BLOCK_SIZE_BITS - sizebits);
            if (sizebits > BLOCK_SIZE_BITS)
                retval = blocks >> (sizebits - BLOCK_SIZE_BITS);
        }
    }
    return retval;
}

static loff_t blkdev_size(kdev_t dev)
{
    unsigned int blocks = ~0U;
    int major = MAJOR(dev);

    if (blk_size[major])
    {
        int minor = MINOR(dev);
        blocks = blk_size[major][minor];
    }
    return (loff_t) blocks << BLOCK_SIZE_BITS;
}

/* Kill _all_ buffers, dirty or not.. */
static void kill_bdev(struct block_device *bdev)
{
    truncate_inode_pages(bdev->bd_inode->i_mapping, 0);
}

int set_blocksize(kdev_t dev, int size)
{
    int oldsize;
    struct block_device *bdev;

    /* Size must be a power of two, and between 512 and PAGE_SIZE */
    if (size > PAGE_SIZE || size < 512 || (size & (size-1)))
        return -EINVAL;

    /* Size cannot be smaller than the size supported by the device */
    if (size < get_hardsect_size(dev))
        return -EINVAL;

    /* No blocksize array? Implies hardcoded BLOCK_SIZE */
    if (!blksize_size[MAJOR(dev)])
    {
        if (size == BLOCK_SIZE)
            return 0;
        return -EINVAL;
    }

    oldsize = blksize_size[MAJOR(dev)][MINOR(dev)];
    if (oldsize == size)
        return 0;

    if (!oldsize && size == BLOCK_SIZE)
    {
        blksize_size[MAJOR(dev)][MINOR(dev)] = size;
        return 0;
    }

    /* Ok, we're actually changing the blocksize.. */
    bdev = bdget(dev);
    sync_buffers(dev, 2);
    blksize_size[MAJOR(dev)][MINOR(dev)] = size;
    bdev->bd_inode->i_blkbits = blksize_bits(size);
    kill_bdev(bdev);
    bdput(bdev);
    return 0;
}

static int blkdev_get_block(struct inode * inode, long iblock, struct buffer_head * bh, int create)
{
    if (iblock >= max_block(inode->i_rdev))
        return -EIO;

    bh->b_dev = inode->i_rdev;
    bh->b_blocknr = iblock;
    bh->b_state |= 1UL << BH_Mapped;
    return 0;
}

static int blkdev_direct_IO(int rw, struct inode * inode, struct kiobuf * iobuf, unsigned long blocknr, int blocksize)
{
    return generic_direct_IO(rw, inode, iobuf, blocknr, blocksize, blkdev_get_block);
}

static int blkdev_writepage(struct page * page)
{
    panic("ai blkdev wr\n");
}

static int blkdev_readpage(struct file * file, struct page * page)
{
    panic("ai blkdev rd\n");
}

static int blkdev_prepare_write(struct file *file, struct page *page, unsigned from, unsigned to)
{
    panic("uh-oh. prep write\n");
}

static int blkdev_commit_write(struct file *file, struct page *page, unsigned from, unsigned to)
{
    printk("uh-oh. commit write\n");
}

/*
 * private llseek:
 * for a block special file file->f_dentry->d_inode->i_size is zero
 * so we compute the size by hand (just as in block_read/write above)
 */
static loff_t block_llseek(struct file *file, loff_t offset, int origin)
{
    /* ewww */
    loff_t size = file->f_dentry->d_inode->i_bdev->bd_inode->i_size;
    loff_t retval;

    switch (origin)
    {
    case 2:
        offset += size;
        break;
    case 1:
        offset += file->f_pos;
    }
    retval = -EINVAL;
    if (offset >= 0 && offset <= size)
    {
        if (offset != file->f_pos)
        {
            file->f_pos = offset;
            file->f_reada = 0;
            file->f_version = ++event;
        }
        retval = offset;
    }
    return retval;
}


static int __block_fsync(struct inode * inode)
{
    int ret, err;

    ret = filemap_fdatasync(inode->i_mapping);
    err = sync_buffers(inode->i_rdev, 1);
    if (err && !ret)
        ret = err;
    err = filemap_fdatawait(inode->i_mapping);
    if (err && !ret)
        ret = err;

    return ret;
}

/*
 *	Filp may be NULL when we are called by an msync of a vma
 *	since the vma has no handle.
 */

static int block_fsync(struct file *filp, struct dentry *dentry, int datasync)
{
    struct inode * inode = dentry->d_inode;

    return __block_fsync(inode);
}

/*
 * pseudo-fs
 */

static struct super_block *bd_read_super(struct super_block *sb, void *data, int silent)
{
    return 0;
}

static DECLARE_FSTYPE(bd_type, "bdev", bd_read_super, FS_NOMOUNT);

static struct vfsmount *bd_mnt;

/*
 * bdev cache handling - shamelessly stolen from inode.c
 * We use smaller hashtable, though.
 */

#define HASH_BITS	6
#define HASH_SIZE	(1UL << HASH_BITS)
#define HASH_MASK	(HASH_SIZE-1)
static struct list_head bdev_hashtable[HASH_SIZE];
static spinlock_t bdev_lock __cacheline_aligned_in_smp = SPIN_LOCK_UNLOCKED;
static kmem_cache_t * bdev_cachep;

//#define alloc_bdev() \
((struct block_device *) kmem_cache_alloc(bdev_cachep, SLAB_KERNEL))
#define destroy_bdev(bdev) kfree(bdev)

static void init_once(void * foo, kmem_cache_t * cachep, unsigned long flags)
{
    struct block_device * bdev = (struct block_device *) foo;

    if ((flags & (SLAB_CTOR_VERIFY|SLAB_CTOR_CONSTRUCTOR)) ==
            SLAB_CTOR_CONSTRUCTOR)
    {
        memset(bdev, 0, sizeof(*bdev));
        sema_init(&bdev->bd_sem, 1);
        INIT_LIST_HEAD(&bdev->bd_inodes);
    }
}

/*
 * Most likely _very_ bad one - but then it's hardly critical for small
 * /dev and can be fixed when somebody will need really large one.
 */
static inline unsigned long hash(dev_t dev)
{
    unsigned long tmp = dev;
    tmp = tmp + (tmp >> HASH_BITS) + (tmp >> HASH_BITS*2);
    return tmp & HASH_MASK;
}

struct block_device *bdget(dev_t dev)
{
    struct list_head * head = bdev_hashtable + hash(dev);
    struct block_device *bdev, *new_bdev;
    spin_lock(&bdev_lock);
    bdev = 0;
    spin_unlock(&bdev_lock);
    if (bdev)
        return bdev;
    new_bdev = kmalloc(sizeof(struct block_device), GFP_KERNEL);
    memset(new_bdev, 0, sizeof(struct block_device));
    if (new_bdev)
    {
        struct inode *inode = kmalloc(sizeof(struct inode), GFP_KERNEL);
        memset(inode, 0, sizeof(struct inode));
        if (inode)
        {
            kdev_t kdev = to_kdev_t(dev);
            atomic_set(&new_bdev->bd_count,1);
            new_bdev->bd_dev = dev;
            new_bdev->bd_op = NULL;
            new_bdev->bd_inode = inode;
            inode->i_rdev = kdev;
            inode->i_dev = kdev;
            inode->i_bdev = new_bdev;
            inode->i_data.a_ops = 0;
            inode->i_data.gfp_mask = GFP_USER;
            inode->i_mode = S_IFBLK;
            spin_lock(&bdev_lock);
#if 0
            bdev = kmalloc(sizeof(struct block_device), GFP_KERNEL);
            memset(bdev, 0, sizeof(struct block_device));
            qhead_init(bdev);
#else
            bdev = 0;
            qhead_init(new_bdev);
            qhead_init(&inode->i_devices);
            qhead_init(&new_bdev->bd_inodes);
#endif
            if (!bdev)
            {
                spin_unlock(&bdev_lock);
                return new_bdev;
            }
            spin_unlock(&bdev_lock);
        }
        destroy_bdev(new_bdev);
    }
    return bdev;
}

static inline void __bd_forget(struct inode *inode)
{
    list_del_init(&inode->i_devices);
    inode->i_bdev = NULL;
    inode->i_mapping = &inode->i_data;
}

void bdput(struct block_device *bdev)
{
    if (atomic_dec_and_lock(&bdev->bd_count, &bdev_lock))
    {
        struct list_head *p;
        if (bdev->bd_openers)
            BUG();
        list_del(&bdev->bd_hash);
        while ( (p = bdev->bd_inodes.next) != &bdev->bd_inodes )
        {
            __bd_forget(list_entry(p, struct inode, i_devices));
        }
        spin_unlock(&bdev_lock);
        destroy_bdev(bdev);
    }
}

static struct
{
    const char *name;
    struct block_device_operations *bdops;
} blkdevs[MAX_BLKDEV];

int get_blkdev_list(char * p)
{
    int i;
    int len;

    len = sprintf(p, "\nBlock devices:\n");
    for (i = 0; i < MAX_BLKDEV ; i++)
    {
        if (blkdevs[i].bdops)
        {
            len += sprintf(p+len, "%3d %s\n", i, blkdevs[i].name);
        }
    }
    return len;
}

/*
	Return the function table of a device.
	Load the driver if needed.
*/
const struct block_device_operations * get_blkfops(unsigned int major)
{
    const struct block_device_operations *ret = NULL;

    /* major 0 is used for non-device mounts */
    if (major && major < MAX_BLKDEV)
    {
#ifdef CONFIG_KMOD
        if (!blkdevs[major].bdops)
        {
            char name[20];
            sprintf(name, "block-major-%d", major);
            request_module(name);
        }
#endif
        ret = blkdevs[major].bdops;
    }
    return ret;
}

/*
 * This routine checks whether a removable media has been changed,
 * and invalidates all buffer-cache-entries in that case. This
 * is a relatively slow routine, so we have to try to minimize using
 * it. Thus it is called only upon a 'mount' or 'open'. This
 * is the best way of combining speed and utility, I think.
 * People changing diskettes in the middle of an operation deserve
 * to lose :-)
 */
int check_disk_change(kdev_t dev)
{
    int i;
    const struct block_device_operations * bdops = NULL;

    i = MAJOR(dev);
    if (i < MAX_BLKDEV)
        bdops = blkdevs[i].bdops;
    if (bdops == NULL)
        return 0;
    if (bdops->check_media_change == NULL)
        return 0;
    if (!bdops->check_media_change(dev))
        return 0;

    printk(KERN_DEBUG "VFS: Disk change detected on device %s\n",
           bdevname(dev));

    if (bdops->revalidate)
        bdops->revalidate(dev);
    return 1;
}

int ioctl_by_bdev(struct block_device *bdev, unsigned cmd, unsigned long arg)
{
    int res;
    mm_segment_t old_fs = get_fs();

    if (!bdev->bd_op->ioctl)
        return -EINVAL;
    set_fs(KERNEL_DS);
    res = bdev->bd_op->ioctl(bdev->bd_inode, NULL, cmd, arg);
    set_fs(old_fs);
    return res;
}

static int do_open(struct block_device *bdev, struct inode *inode, struct file *file)
{
    int ret = -ENXIO;
    kdev_t dev = to_kdev_t(bdev->bd_dev);

    down(&bdev->bd_sem);
    lock_kernel();
    if (!bdev->bd_op)
        bdev->bd_op = get_blkfops(MAJOR(dev));
    if (bdev->bd_op)
    {
        ret = 0;
        if (bdev->bd_op->owner)
            __MOD_INC_USE_COUNT(bdev->bd_op->owner);
        if (bdev->bd_op->open)
            ret = bdev->bd_op->open(inode, file);
        if (!ret)
        {
            bdev->bd_openers++;
            bdev->bd_inode->i_size = blkdev_size(dev);
            bdev->bd_inode->i_blkbits = blksize_bits(block_size(dev));
        }
        else
        {
            if (bdev->bd_op->owner)
                __MOD_DEC_USE_COUNT(bdev->bd_op->owner);
            if (!bdev->bd_openers)
                bdev->bd_op = NULL;
        }
    }
    unlock_kernel();
    up(&bdev->bd_sem);
    if (ret)
        bdput(bdev);
    return ret;
}

int blkdev_get(struct block_device *bdev, mode_t mode, unsigned flags, int kind)
{
    /*
     * This crockload is due to bad choice of ->open() type.
     * It will go away.
     * For now, block device ->open() routine must _not_
     * examine anything in 'inode' argument except ->i_rdev.
     */
    struct file fake_file = {};
    struct dentry fake_dentry = {};
    fake_file.f_mode = mode;
    fake_file.f_flags = flags;
    fake_file.f_dentry = &fake_dentry;
    fake_dentry.d_inode = bdev->bd_inode;

    return do_open(bdev, bdev->bd_inode, &fake_file);
}

int blkdev_put(struct block_device *bdev, int kind)
{
    int ret = 0;
    kdev_t rdev = to_kdev_t(bdev->bd_dev); /* this should become bdev */
    struct inode *bd_inode = bdev->bd_inode;

    down(&bdev->bd_sem);
    lock_kernel();
    if (kind == BDEV_FILE)
        __block_fsync(bd_inode);
    else if (kind == BDEV_FS)
        fsync_no_super(rdev);
    if (!--bdev->bd_openers)
        kill_bdev(bdev);
    if (bdev->bd_op->release)
        ret = bdev->bd_op->release(bd_inode, NULL);
    if (bdev->bd_op->owner)
        __MOD_DEC_USE_COUNT(bdev->bd_op->owner);
    if (!bdev->bd_openers)
        bdev->bd_op = NULL;
    unlock_kernel();
    up(&bdev->bd_sem);
    bdput(bdev);
    return ret;
}

static int blkdev_ioctl(struct inode *inode, struct file *file, unsigned cmd,
                        unsigned long arg)
{
    if (inode->i_bdev->bd_op->ioctl)
        return inode->i_bdev->bd_op->ioctl(inode, file, cmd, arg);
    return -EINVAL;
}

const char * bdevname(kdev_t dev)
{
    static char buffer[32];
    const char * name = blkdevs[MAJOR(dev)].name;

    if (!name)
        name = "unknown-block";

    sprintf(buffer, "%s(%d,%d)", name, MAJOR(dev), MINOR(dev));
    return buffer;
}
