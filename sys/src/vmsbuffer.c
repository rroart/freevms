// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004. Based on buffer.c.

/*
 *  linux/fs/buffer.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/*
 *  'buffer.c' implements the buffer-cache functions. Race-conditions have
 * been avoided by NEVER letting an interrupt change a buffer (except for the
 * data, of course), but instead letting the caller do it.
 */

/* Start bdflush() with kernel_thread not syscall - Paul Gortmaker, 12/95 */

/* Removed a lot of unnecessary code and simplified things now that
 * the buffer cache isn't our primary cache - Andrew Tridgell 12/96
 */

/* Speed up hash, lru, and free list operations.  Use gfp() for allocating
 * hash table, use SLAB cache for buffer heads. -DaveM
 */

/* Added 32k buffer block sizes - these are required older ARM systems.
 * - RMK
 */

/* Thread it... -DaveM */

/* async buffer flushing, 1999 Andrea Arcangeli <andrea@suse.de> */

#include <linux/config.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/locks.h>
#include <linux/errno.h>
#include <linux/smp_lock.h>
#include <linux/vmalloc.h>
#include <linux/blkdev.h>
#include <linux/sysrq.h>
#include <linux/file.h>
#include <linux/init.h>
#include <linux/quotaops.h>
#include <linux/iobuf.h>
#include <linux/highmem.h>
#include <linux/module.h>
#include <linux/completion.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/bitops.h>
#include <asm/mmu_context.h>
#include <asm/hw_irq.h>

#include <fcbdef.h>
#include <pridef.h>
#include <iodef.h>
#include <misc.h>
#include <rvtdef.h>
#include <vcbdef.h>
#include <ucbdef.h>
#include <linux/ext2_fs.h>

#include <misc_routines.h>
#include "../../ext2/src/x2p.h"

#define EXT2_EF 30

#define MAX_BUF_PER_PAGE (PAGE_CACHE_SIZE / 512)
#define NR_RESERVED (10*MAX_BUF_PER_PAGE)
#define MAX_UNUSED_BUFFERS NR_RESERVED+20 /* don't ever have more than this 
					     number of unused buffer heads */

/* Anti-deadlock ordering:
 *	lru_list_lock > hash_table_lock > unused_list_lock
 */

#define BH_ENTRY(list) list_entry((list), struct buffer_head, b_inode_buffers)

/*
 * Hash table gook..
 */
#if 0
static unsigned int bh_hash_mask;
static unsigned int bh_hash_shift;
static struct buffer_head **hash_table;
static rwlock_t hash_table_lock = RW_LOCK_UNLOCKED;

static struct buffer_head *lru_list[NR_LIST];
static spinlock_t lru_list_lock __cacheline_aligned_in_smp = SPIN_LOCK_UNLOCKED;
static int nr_buffers_type[NR_LIST];
static unsigned long size_buffers_type[NR_LIST];

static struct buffer_head * unused_list;
static int nr_unused_buffer_heads;
static spinlock_t unused_list_lock = SPIN_LOCK_UNLOCKED;
static DECLARE_WAIT_QUEUE_HEAD(buffer_wait);
#endif

static int grow_buffers(kdev_t dev, unsigned long block, int size);
static void __refile_buffer(struct buffer_head *);

/* This is used by some architectures to estimate available memory. */
atomic_t buffermem_pages = ATOMIC_INIT(0);

/* Here is the parameter block for the bdflush process. If you add or
 * remove any of the parameters, make sure to update kernel/sysctl.c
 * and the documentation at linux/Documentation/sysctl/vm.txt.
 */

#define N_PARAM 9

/* The dummy values in this structure are left in there for compatibility
 * with old programs that play with the /proc entries.
 */
union bdflush_param {
	struct {
		int nfract;	/* Percentage of buffer cache dirty to 
				   activate bdflush */
		int dummy1;	/* old "ndirty" */
		int dummy2;	/* old "nrefill" */
		int dummy3;	/* unused */
		int interval;	/* jiffies delay between kupdate flushes */
		int age_buffer;	/* Time for normal buffer to age before we flush it */
		int nfract_sync;/* Percentage of buffer cache dirty to 
				   activate bdflush synchronously */
		int dummy4;	/* unused */
		int dummy5;	/* unused */
	} b_un;
	unsigned int data[N_PARAM];
} bdf_prm = {{40, 0, 0, 0, 5*HZ, 30*HZ, 60, 0, 0}};

/* These are the min and max parameter values that we will allow to be assigned */
int bdflush_min[N_PARAM] = {  0,  10,    5,   25,  0,   1*HZ,   0, 0, 0};
int bdflush_max[N_PARAM] = {100,50000, 20000, 20000,10000*HZ, 6000*HZ, 100, 0, 0};

void fastcall unlock_buffer(struct buffer_head *bh)
{
	clear_bit(BH_Wait_IO, &bh->b_state);
	clear_bit(BH_launder, &bh->b_state);
	clear_bit(BH_Lock, &bh->b_state);
	smp_mb__after_clear_bit();
	if (waitqueue_active(&bh->b_wait))
		wake_up(&bh->b_wait);
	//		wake_up2(&bh->b_wait,PRI$_IOCOM);
}

/*
 * Rewrote the wait-routines to use the "new" wait-queue functionality,
 * and getting rid of the cli-sti pairs. The wait-queue routines still
 * need cli-sti, but now it's just a couple of 386 instructions or so.
 *
 * Note that the real wait_on_buffer() is an inline function that checks
 * if 'b_wait' is set before calling this, so that the queues aren't set
 * up unnecessarily.
 */
void __wait_on_buffer(struct buffer_head * bh)
{
	struct task_struct *tsk = current;
	DECLARE_WAITQUEUE(wait, tsk);

	get_bh(bh);
	add_wait_queue(&bh->b_wait, &wait);
	do {
	  //run_task_queue(&tq_disk);
		set_task_state(tsk, TASK_UNINTERRUPTIBLE);
		if (!buffer_locked(bh))
			break;
		schedule();
	} while (buffer_locked(bh));
	tsk->state = TASK_RUNNING;
	remove_wait_queue(&bh->b_wait, &wait);
	put_bh(bh);
}

/*
 * Default synchronous end-of-IO handler..  Just mark it up-to-date and
 * unlock the buffer. This is what ll_rw_block uses too.
 */
void end_buffer_io_sync(struct buffer_head *bh, int uptodate)
{
	mark_buffer_uptodate(bh, uptodate);
	unlock_buffer(bh);
	put_bh(bh);
}

/* Call sync_buffers with wait!=0 to ensure that the call does not
 * return until all buffer writes have completed.  Sync() may return
 * before the writes have finished; fsync() may not.
 */

/* Godamity-damn.  Some buffers (bitmaps for filesystems)
 * spontaneously dirty themselves without ever brelse being called.
 * We will ultimately want to put these in a separate list, but for
 * now we search all of the lists for dirty buffers.
 */
int sync_buffers(kdev_t dev, int wait)
{
	int err = 0;

	/* One pass for no-wait, three for wait:
	 * 0) write out all dirty, unlocked buffers;
	 * 1) wait for all dirty locked buffers;
	 * 2) write out all dirty, unlocked buffers;
	 * 2) wait for completion by waiting for all buffers to unlock.
	 */
#if 0
	write_unlocked_buffers(dev);
	if (wait) {
		err = wait_for_locked_buffers(dev, BUF_DIRTY, 0);
		write_unlocked_buffers(dev);
		err |= wait_for_locked_buffers(dev, BUF_LOCKED, 1);
	}
#endif
	return err;
}

int fsync_super(struct super_block *sb)
{
#if 0
	kdev_t dev = sb->s_dev;
	sync_buffers(dev, 0);

	lock_kernel();
	sync_inodes_sb(sb);
	DQUOT_SYNC(dev);
	lock_super(sb);
	if (sb->s_dirt && sb->s_op && sb->s_op->write_super)
		sb->s_op->write_super(sb);
	unlock_super(sb);
	unlock_kernel();

	return sync_buffers(dev, 1);
#else
	return 0;
#endif
}

int fsync_no_super(kdev_t dev)
{
	sync_buffers(dev, 0);
	return sync_buffers(dev, 1);
}

int fsync_dev(kdev_t dev)
{
#if 0
	sync_buffers(dev, 0);

	lock_kernel();
	sync_inodes(dev);
	DQUOT_SYNC(dev);
	sync_supers(dev);
	unlock_kernel();

	return sync_buffers(dev, 1);
#else
	return 0;
#endif
}

/*
 * There's no real reason to pretend we should
 * ever do anything differently
 */
void sync_dev(kdev_t dev)
{
	fsync_dev(dev);
}

asmlinkage long sys_sync(void)
{
	fsync_dev(0);
	return 0;
}

/*
 *	filp may be NULL if called via the msync of a vma.
 */
 
int file_fsync(struct file *filp, struct dentry *dentry, int datasync)
{
#if 0
  // not yet
	struct _fcb * inode = dentry->d_inode;
	struct super_block * sb;
	kdev_t dev;
	int ret;

	lock_kernel();
	/* sync the inode to buffers */
	write_inode_now(inode, 0);

	/* sync the superblock to buffers */
	sb = inode->i_sb;
	lock_super(sb);
	if (sb->s_op && sb->s_op->write_super)
		sb->s_op->write_super(sb);
	unlock_super(sb);

	/* .. finally sync the buffers to disk */
	dev = FCB_DEV(inode);
	ret = sync_buffers(dev, 1);
	unlock_kernel();
	return ret;
#endif
	return 0;
}

asmlinkage long sys_fsync(unsigned int fd)
{
	struct file * file;
	struct dentry * dentry;
	struct _fcb * inode;
	int ret, err;

	ret = -EBADF;
	file = fget(fd);
	if (!file)
		goto out;

	dentry = file->f_dentry;
	inode = dentry->d_inode;

	ret = -EINVAL;
	if (!file->f_op || !file->f_op->fsync) {
		/* Why?  We can still call filemap_fdatasync */
		goto out_putf;
	}

	/* We need to protect against concurrent writers.. */
#if 0
	// not yet
	down(&inode->i_sem);
	ret = filemap_fdatasync(inode->i_mapping);
	err = file->f_op->fsync(file, dentry, 0);
	if (err && !ret)
		ret = err;
	err = filemap_fdatawait(inode->i_mapping);
	if (err && !ret)
		ret = err;
	up(&inode->i_sem);
#endif

out_putf:
	fput(file);
out:
	return ret;
}

asmlinkage long sys_fdatasync(unsigned int fd)
{
	struct file * file;
	struct dentry * dentry;
	struct _fcb * inode;
	int ret, err;

	ret = -EBADF;
	file = fget(fd);
	if (!file)
		goto out;

	dentry = file->f_dentry;
	inode = dentry->d_inode;

	ret = -EINVAL;
	if (!file->f_op || !file->f_op->fsync)
		goto out_putf;

#if 0
	// not yet
	down(&inode->i_sem);
	ret = filemap_fdatasync(inode->i_mapping);
	err = file->f_op->fsync(file, dentry, 1);
	if (err && !ret)
		ret = err;
	err = filemap_fdatawait(inode->i_mapping);
	if (err && !ret)
		ret = err;
	up(&inode->i_sem);
#endif

out_putf:
	fput(file);
out:
	return ret;
}

void init_buffer(struct buffer_head *bh, bh_end_io_t *handler, void *private)
{
	bh->b_list = BUF_CLEAN;
	bh->b_end_io = handler;
	bh->b_private = private;
}

/*
 * Ok, this is getblk, and it isn't very clear, again to hinder
 * race-conditions. Most of the code is seldom used, (ie repeating),
 * so it should be much more efficient than it looks.
 *
 * The algorithm is changed: hopefully better, and an elusive bug removed.
 *
 * 14.02.92: changed it to sync dirty buffers a bit: better performance
 * when the filesystem starts to get full of dirty blocks (I hope).
 */
struct buffer_head * getblk(kdev_t dev, int block, int size)
{
  struct buffer_head * bh;
	 bh = kmalloc (sizeof(struct buffer_head), GFP_KERNEL);
	 init_buffer(bh, NULL, NULL);
	 init_waitqueue_head(&bh->b_wait);
	 get_bh(bh);

	 bh -> b_dev = dev;
	 bh -> b_data = kmalloc(size, GFP_KERNEL);
	 bh -> b_blocknr = (long)block;
	 bh -> b_size = (long)size;
	 return bh;
}

/*
 * if a new dirty buffer is created we need to balance bdflush.
 *
 * in the future we might want to make bdflush aware of different
 * pressures on different devices - thus the (currently unused)
 * 'dev' parameter.
 */
void balance_dirty(void)
{
  int state = 0;//balance_dirty_state();

	if (state < 0)
		return;

	/* If we're getting into imbalance, start write-out */
#if 0
	spin_lock(&lru_list_lock);
	write_some_buffers(NODEV);
#endif

	/*
	 * And if we're _really_ out of balance, wait for
	 * some of the dirty/locked buffers ourselves and
	 * start bdflush.
	 * This will throttle heavy writers.
	 */
#if 0
	if (state > 0) {
		wait_for_some_buffers(NODEV);
		wakeup_bdflush();
	}
#endif
}

inline void fastcall __mark_dirty(struct buffer_head *bh)
{
	bh->b_flushtime = jiffies + bdf_prm.b_un.age_buffer;
	//refile_buffer(bh);
}

/* atomic version, the user must call balance_dirty() by hand
   as soon as it become possible to block */
void fastcall __mark_buffer_dirty(struct buffer_head *bh)
{
	if (!atomic_set_buffer_dirty(bh))
		__mark_dirty(bh);
}

void fastcall mark_buffer_dirty(struct buffer_head *bh)
{
	if (!atomic_set_buffer_dirty(bh)) {
		__mark_dirty(bh);
		balance_dirty();
	}
}

void set_buffer_flushtime(struct buffer_head *bh)
{
	bh->b_flushtime = jiffies + bdf_prm.b_un.age_buffer;
}
EXPORT_SYMBOL(set_buffer_flushtime);

/*
 * Release a buffer head
 */
void __brelse(struct buffer_head * buf)
{
	if (atomic_read(&buf->b_count)) {
		put_bh(buf);
#if 1
		 if (atomic_read(&buf->b_count)==0)
		   kfree(buf);
#endif
		return;
	}
	printk(KERN_ERR "VFS: brelse: Trying to free free buffer\n");
}

/*
 * bforget() is like brelse(), except it discards any
 * potentially dirty data.
 */
void __bforget(struct buffer_head * buf)
{
  //mark_buffer_clean(buf);
	__brelse(buf);
}

/**
 *	bread() - reads a specified block and returns the bh
 *	@block: number of block
 *	@size: size (in bytes) to read
 * 
 *	Reads a specified block, and returns buffer head that
 *	contains it. It returns NULL if the block was unreadable.
 */

struct buffer_head * bread(kdev_t dev, int block, int size)
{
	 struct buffer_head * bh;
	 struct _iosb iosb;
	 int sts;

	 bh = kmalloc (sizeof(struct buffer_head), GFP_KERNEL);
	 init_buffer(bh, NULL, NULL);
	 init_waitqueue_head(&bh->b_wait);
	 get_bh(bh);

	 bh -> b_dev = dev;
	 bh -> b_data = kmalloc(size, GFP_KERNEL);
	 bh -> b_size = (long)size;
	 bh -> b_blocknr = (long)block;

	 //printk(KERN_INFO "qiow2 %x,%x,%x,%x | ",dev,dev2chan(dev),size,block);
	 //printk(KERN_INFO "q2 %lx,%lx|",dev,block);

	 sts = exe$qiow(EXT2_EF,dev2chan(dev),IO$_READPBLK,&iosb,0,0,
			bh -> b_data,size,block,MINOR(dev)&31,0,0);

	 set_bit(BH_Uptodate, &bh->b_state);
	 return bh;
}

/**
 * try_to_release_page - release old fs-specific metadata on a page
 *
 */

int try_to_release_page(struct page * page, int gfp_mask)
{
	return 0;
}

/*
 * We don't have to release all buffers here, but
 * we have to be sure that no dirty buffer is left
 * and no IO is going on (no buffer is locked), because
 * we have truncated the file and are going to free the
 * blocks on-disk..
 */
int discard_bh_page(struct page *page, unsigned long offset, int drop_pagecache)
{
	return 1;
}

/*
 * block_write_full_page() is SMP threaded - the kernel lock is not held.
 */
static int __block_write_full_page2(struct _fcb *inode, struct page *page, unsigned long pageno)
{
	struct _fcb * fcb=e2_search_fcb(inode);
	unsigned long iblock, lblock;
	int err, i;
	unsigned long block;
	int need_unlock;
	int sts;
	struct _iosb iosb;
	int turns=0;
	signed int blocknr;
	unsigned long blocksize;
	struct _vcb * vcb = exttwo_get_current_vcb();
	struct ext2_super_block * sb = vcb->vcb$l_cache;
	int i_blkbits = EXT2_BLOCK_SIZE_BITS(sb);

	blocksize = 1 << i_blkbits;

	block = pageno << (PAGE_CACHE_SHIFT - i_blkbits);
	iblock = pageno << (PAGE_CACHE_SHIFT - i_blkbits);

	i = 0;

	/* Stage 1: make sure we have all the buffers mapped! */
	do {
		/*
		 * If the buffer isn't up-to-date, we can't be sure
		 * that the buffer has been initialized with the proper
		 * block number information etc..
		 *
		 * Leave it to the low-level FS to make all those
		 * decisions (block #0 may actually be a valid block)
		 */
	  if (fcb)
	    blocknr=e2_map_vbn(fcb,iblock);
	  else
	    blocknr=iblock;
	  if (blocknr==-1) {
	    err=ext2_get_block(vcb, inode, iblock, &blocknr, 1, fcb);
	    if (err)
	      goto out;
	  }
	  struct _ucb * ucb;
	  if (vcb->vcb$l_rvn) {
	    struct _rvt * rvt = vcb->vcb$l_rvt;
	    struct _ucb ** ucblst = rvt->rvt$l_ucblst;
	    ucb = ucblst[fcb->fcb$b_fid_rvn - 1];
	  } else 
	    ucb = vcb->vcb$l_rvt;
	  sts = exe$qiow(EXT2_EF,(unsigned short)x2p->io_channel,IO$_WRITEPBLK,&iosb,0,0,
			 page_address(page)+turns*blocksize,blocksize, blocknr*vms_block_factor(i_blkbits),ucb->ucb$w_fill_0,0,0);

	  turns++;
	  block++;
	  iblock++;
	} while (turns<(PAGE_SIZE/blocksize));

	/* Done - end_buffer_io_async will unlock */
#if 0
	SetPageUptodate(page);
#endif
	return 0;

out:
	/*
	 * ENOSPC, or some other error.  We may already have added some
	 * blocks to the file, so we need to write these out to avoid
	 * exposing stale data.
	 */
#if 0
	ClearPageUptodate(page);
#endif
	return err;
}

static int __block_prepare_write(struct _fcb *inode, struct page *page,
		unsigned from, unsigned to, unsigned long pageno)
{
	struct _fcb * fcb=e2_search_fcb(inode);
	unsigned block_start, block_end;
	unsigned long block;
	int err = 0;
	unsigned blocksize, bbits;
	struct buffer_head *bh, *head, *wait[2], **wait_bh=wait, *arr[MAX_BUF_PER_PAGE];
	char *kaddr = kmap(page);
	int turns = 0;
	int sts;
	struct _iosb iosb;
	int blocknr;

	struct _vcb * vcb = exttwo_get_current_vcb();
	struct ext2_super_block * sb = vcb->vcb$l_cache;
	int i_blkbits = EXT2_BLOCK_SIZE_BITS(sb);
	blocksize = 1 << i_blkbits;

	bbits = i_blkbits;
	block = pageno << (PAGE_CACHE_SHIFT - bbits);

	for(block_start = 0; turns<(PAGE_SIZE/blocksize);
	    block++, block_start=block_end, turns++) {

	   block_end = block_start+blocksize;
	   if (block_end <= from)
	     continue;
	   if (block_start >= to)
	     break;

	   if (fcb)
	     blocknr=e2_map_vbn(fcb,block);
	   else
	     blocknr=block;
	   if (blocknr==-1) {
	     err=ext2_get_block(vcb, inode, block, &blocknr, 1, fcb);
	     if (err)
	       goto out;
	   }
	   if (block_end > to)
	     memset(kaddr+to, 0, block_end-to);
	   if (block_start < from)
	     memset(kaddr+block_start, 0, from-block_start);
	   if (block_end > to || block_start < from)
	     flush_dcache_page(page);
	   if ((block_start < from || block_end > to)) {
	     struct _ucb * ucb;
	     if (vcb->vcb$l_rvn) {
	       struct _rvt * rvt = vcb->vcb$l_rvt;
	       struct _ucb ** ucblst = rvt->rvt$l_ucblst;
	       ucb = ucblst[fcb->fcb$b_fid_rvn - 1];
	     } else 
	       ucb = vcb->vcb$l_rvt;
	     sts = exe$qiow(EXT2_EF,(unsigned short)x2p->io_channel,IO$_READPBLK,&iosb,0,0,
			    kaddr+turns*blocksize,blocksize, blocknr*vms_block_factor(i_blkbits),ucb->ucb$w_fill_0,0,0);
	   }
	}
	return 0;
out:
	/*
	 * Zero out any newly allocated blocks to avoid exposing stale
	 * data.  If BH_New is set, we know that the block was newly
	 * allocated in the above loop.
	 */

	return err;
}

static int __block_commit_write(struct _fcb *inode, struct page *page,
		unsigned from, unsigned to, unsigned long pageno)
{
	struct _fcb * fcb=e2_search_fcb(inode);
	unsigned block_start, block_end;
	int partial = 0, need_balance_dirty = 0;
	unsigned blocksize;
	struct buffer_head *bh, **arr;
	int turns=0;
	int sts;
	struct _iosb iosb;
	signed long blocknr, block;
	int bbits;

	struct _vcb * vcb = exttwo_get_current_vcb();
	struct ext2_super_block * sb = vcb->vcb$l_cache;
	int i_blkbits = EXT2_BLOCK_SIZE_BITS(sb);
	blocksize = 1 << i_blkbits;

	bbits = i_blkbits;
	block = pageno << (PAGE_CACHE_SHIFT - bbits);

	for(block_start = 0;
	    turns<(PAGE_SIZE/blocksize);
	    block_start=block_end, turns++, block++) {
	  blocknr=e2_map_vbn(fcb,block);
		block_end = block_start + blocksize;
		if (block_end <= from || block_start >= to) {
		  partial = 1;
		} else {
		  struct _ucb * ucb;
		  if (vcb->vcb$l_rvn) {
		    struct _rvt * rvt = vcb->vcb$l_rvt;
		    struct _ucb ** ucblst = rvt->rvt$l_ucblst;
		    ucb = ucblst[fcb->fcb$b_fid_rvn - 1];
		  } else 
		    ucb = vcb->vcb$l_rvt;
		  sts = exe$qiow(EXT2_EF,(unsigned short)x2p->io_channel,IO$_WRITEPBLK,&iosb,0,0,
				 page_address(page)+turns*blocksize,blocksize, blocknr*vms_block_factor(i_blkbits),ucb->ucb$w_fill_0,0,0);
		}
	}

	/*
	 * is this a partial write that happened to make all buffers
	 * uptodate then we can optimize away a bogus readpage() for
	 * the next read(). Here we 'discover' wether the page went
	 * uptodate as a result of this (potentially partial) write.
	 */
#if 0
	if (!partial)
		SetPageUptodate(page);
#endif
	return 0;
}

/*
 * Generic "read page" function for block devices that have the normal
 * get_block functionality. This is most of the block device filesystems.
 * Reads the page asynchronously --- the unlock_buffer() and
 * mark_buffer_uptodate() functions propagate buffer state into the
 * page struct once IO has completed.
 */
int block_read_full_page2(struct _fcb *inode,struct page *page, unsigned long pageno)
{
	 struct _fcb * fcb=e2_search_fcb(inode);
	 unsigned long iblock, lblock;
	 unsigned int blocksize, blocks;
	 int nr, i;
	 int sts;
	 struct _iosb iosb;
	 int turns;
	 unsigned long blocknr;

	 struct _vcb * vcb = exttwo_get_current_vcb();
	 struct ext2_super_block * sb = vcb->vcb$l_cache;
	 int i_blkbits = EXT2_BLOCK_SIZE_BITS(sb);
	 blocksize = 1 << i_blkbits;

	 blocks = PAGE_CACHE_SIZE >> i_blkbits;
	 iblock = pageno << (PAGE_CACHE_SHIFT - i_blkbits);
	 lblock = (inode->fcb$l_filesize+blocksize-1) >> i_blkbits;

	 nr = 0;
	 i = 0;
	 turns = 0;

	 do {
	   if (iblock < lblock) {
	     if (fcb)
	       blocknr=e2_map_vbn(fcb,iblock);
	     else
	       blocknr=iblock;
	   } else {
	     continue;
	   }
#if 0
	   // file holes not supported yet
	   memset(kmap(page) + i*blocksize, 0, blocksize);
	   flush_dcache_page(page);
	   kunmap(page);
	   continue;
#endif

	   nr++;

	   struct _ucb * ucb;
	   if (vcb->vcb$l_rvn) {
	     struct _rvt * rvt = vcb->vcb$l_rvt;
	     struct _ucb ** ucblst = rvt->rvt$l_ucblst;
	     ucb = ucblst[fcb->fcb$b_fid_rvn - 1];
	   } else 
	     ucb = vcb->vcb$l_rvt;
	   sts = exe$qiow(EXT2_EF,(unsigned short)x2p->io_channel,IO$_READPBLK,&iosb,0,0,
			  page_address(page) + i*blocksize,blocksize, blocknr*vms_block_factor(i_blkbits),ucb->ucb$w_fill_0,0,0);

	 } while (i++, iblock++, turns++, turns<(PAGE_SIZE/blocksize));

#if 0
	 SetPageUptodate(page);
	 //	 UnlockPage(page);
#endif

	 return 0;
}

int block_read_full_page3(struct _fcb * fcb,struct page *page, unsigned long pageno)
{
	 unsigned long iblock, lblock;
	 unsigned int blocksize, blocks;
	 int nr, i;
	 int sts;
	 struct _iosb iosb;
	 int turns;
	 unsigned long blocknr;

	 struct _vcb * vcb = exttwo_get_current_vcb();
	 struct ext2_super_block * sb = vcb->vcb$l_cache;
	 int i_blkbits = EXT2_BLOCK_SIZE_BITS(sb);
	 blocksize = 1 << i_blkbits;

	 blocks = PAGE_CACHE_SIZE >> i_blkbits;
	 iblock = pageno << (PAGE_CACHE_SHIFT - i_blkbits);
#if 0
	 lblock = (inode->fcb$l_filesize+blocksize-1) >> i_blkbits;
#else
	 lblock = fcb->fcb$l_efblk; // check
#endif

	 nr = 0;
	 i = 0;
	 turns = 0;

	 do {
	   if (iblock < lblock) {
	     if (fcb) {
	       blocknr=e2_map_vbn(fcb,iblock);
	       if ((blocknr+1)==0) {
		 // temp fix before fixing block size check 
		 printk("ebl %lx %lx %lx\n",fcb, iblock, blocknr);
		 continue;
	       }
	     }
	     else
	       blocknr=iblock;
	   } else {
	     continue;
	   }
	   //printk("b %lx %lx %lx %lx\n",fcb,blocknr,iblock,lblock);
#if 0
	   // file holes not supported yet
	   memset(kmap(page) + i*blocksize, 0, blocksize);
	   flush_dcache_page(page);
	   kunmap(page);
	   continue;
#endif

	   nr++;

	   //printk("p3 %lx %lx %lx %lx %lx %lx\n",page_address(page) + i*blocksize,blocksize, blocknr*vms_block_factor(i_blkbits),vcb->vcb$l_rvt->rvt$l_ucblst[fcb->fcb$b_fid_rvn]->ucb$w_fill_0,0,0);
	   struct _ucb * ucb;
	   if (vcb->vcb$l_rvn) {
	     struct _rvt * rvt = vcb->vcb$l_rvt;
	     struct _ucb ** ucblst = rvt->rvt$l_ucblst;
	     ucb = ucblst[fcb->fcb$b_fid_rvn - 1];
	   } else 
	     ucb = vcb->vcb$l_rvt;
	   sts = exe$qiow(EXT2_EF,(unsigned short)x2p->io_channel,IO$_READPBLK,&iosb,0,0,
			  page_address(page) + i*blocksize,blocksize, blocknr*vms_block_factor(i_blkbits),ucb->ucb$w_fill_0,0,0);

	 } while (i++, iblock++, turns++, turns<(PAGE_SIZE/blocksize));

#if 0
	 SetPageUptodate(page);
	 //	 UnlockPage(page);
#endif

	 return 0;
}

// From ext2/src/super.c
static loff_t ext2_max_size(int bits)
{
  loff_t res = EXT2_NDIR_BLOCKS;
  res += 1LL << (bits-2);
  res += 1LL << (2*(bits-2));
  res += 1LL << (3*(bits-2));
  res <<= bits;
  if (res > (512LL << 32) - (1 << bits))
    res = (512LL << 32) - (1 << bits);
  return res;
}

/* utility function for filesystems that need to do work on expanding
 * truncates.  Uses prepare/commit_write to allow the filesystem to
 * deal with the hole.  
 */
int generic_cont_expand(struct _fcb *inode, loff_t size)
{
	struct page *page;
	unsigned long index, offset, limit;
	int err;

	err = -EFBIG;
        limit = current->rlim[RLIMIT_FSIZE].rlim_cur;
	if (limit != RLIM_INFINITY && size > (loff_t)limit) {
		send_sig(SIGXFSZ, current, 0);
		goto out;
	}
	struct _vcb * vcb = exttwo_get_current_vcb();
	struct ext2_super_block * sb = vcb->vcb$l_cache;
	if (size > ext2_max_size(EXT2_BLOCK_SIZE_BITS(sb)))
		goto out;

	offset = (size & (PAGE_CACHE_SIZE-1)); /* Within page */

	/* ugh.  in prepare/commit_write, if from==to==start of block, we 
	** skip the prepare.  make sure we never send an offset for the start
	** of a block
	*/
	if ((offset & (EXT2_BLOCK_SIZE(sb) - 1)) == 0) {
		offset++;
	}
	index = size >> PAGE_CACHE_SHIFT;
	err = -ENOMEM;
	page = alloc_pages(GFP_KERNEL, 0);//grab_cache_page(mapping, index);
	if (!page)
		goto out;
	err = block_prepare_write2(inode, page, offset, offset, index);
	if (!err) {
		err = block_commit_write2(inode, page, offset, offset, index);
	}
#if 0
	UnlockPage(page);
#endif
	page_cache_release(page);
	if (err > 0)
		err = 0;
out:
	return err;
}

/*
 * For moronic filesystems that do not allow holes in file.
 * We may have to extend the file.
 */

int cont_prepare_write(struct page *page, unsigned offset, unsigned to, get_block_t *get_block, unsigned long *bytes)
{
#if 0
	struct address_space *mapping = page->mapping;
	struct _fcb *inode = mapping->host;
	struct page *new_page;
	unsigned long pgpos;
	long status;
	unsigned zerofrom;
	struct _vcb * vcb = exttwo_get_current_vcb();
	struct ext2_super_block * sb = vcb->vcb$l_cache;
	int i_blkbits = EXT2_BLOCK_SIZE_BITS(sb);
	unsigned blocksize = 1 << i_blkbits;
	char *kaddr;

	while(page->index > (pgpos = *bytes>>PAGE_CACHE_SHIFT)) {
		status = -ENOMEM;
		new_page = alloc_pages(GFP_KERNEL, 0);//grab_cache_page(mapping, pgpos);
		if (!new_page)
			goto out;
		/* we might sleep */
		if (*bytes>>PAGE_CACHE_SHIFT != pgpos) {
#if 0
			UnlockPage(new_page);
#endif
			page_cache_release(new_page);
			continue;
		}
		zerofrom = *bytes & ~PAGE_CACHE_MASK;
		if (zerofrom & (blocksize-1)) {
			*bytes |= (blocksize-1);
			(*bytes)++;
		}
		status = __block_prepare_write(inode, new_page, zerofrom,
						PAGE_CACHE_SIZE, get_block);
		if (status)
			goto out_unmap;
		kaddr = page_address(new_page);
		memset(kaddr+zerofrom, 0, PAGE_CACHE_SIZE-zerofrom);
		flush_dcache_page(new_page);
		__block_commit_write(inode, new_page, zerofrom, PAGE_CACHE_SIZE,pageno);
		kunmap(new_page);
#if 0
		UnlockPage(new_page);
#endif
		page_cache_release(new_page);
	}

	if (page->index < pgpos) {
		/* completely inside the area */
		zerofrom = offset;
	} else {
		/* page covers the boundary, find the boundary offset */
		zerofrom = *bytes & ~PAGE_CACHE_MASK;

		/* if we will expand the thing last block will be filled */
		if (to > zerofrom && (zerofrom & (blocksize-1))) {
			*bytes |= (blocksize-1);
			(*bytes)++;
		}

		/* starting below the boundary? Nothing to zero out */
		if (offset <= zerofrom)
			zerofrom = offset;
	}
	status = __block_prepare_write(inode, page, zerofrom, to, get_block);
	if (status)
		goto out1;
	kaddr = page_address(page);
	if (zerofrom < offset) {
		memset(kaddr+zerofrom, 0, offset-zerofrom);
		flush_dcache_page(page);
		__block_commit_write(inode, page, zerofrom, offset);
	}
	return 0;
out1:
#if 0
	ClearPageUptodate(page);
#endif
	kunmap(page);
	return status;

out_unmap:
#if 0
	ClearPageUptodate(new_page);
#endif
	kunmap(new_page);
#if 0
	UnlockPage(new_page);
#endif
	page_cache_release(new_page);
out:
	return status;
#endif
}

int block_prepare_write2(struct _fcb *inode, struct page *page, unsigned from, unsigned to, unsigned long pageno)
{
	int err = __block_prepare_write(inode, page, from, to, pageno);
	if (err) {
#if 0
		ClearPageUptodate(page);
#endif
		kunmap(page);
	}
	return err;
}

int block_commit_write2(struct _fcb * inode, struct page *page, unsigned from, unsigned to, unsigned long pageno)
{
	__block_commit_write(inode,page,from,to,pageno);
	kunmap(page);
	return 0;
}

int generic_commit_write2(struct _fcb * inode, struct page *page,
		unsigned from, unsigned to, unsigned long pageno)
{
	loff_t pos = ((loff_t)pageno << PAGE_CACHE_SHIFT) + to;
	__block_commit_write(inode,page,from,to,pageno);
	kunmap(page);
	if (pos > inode->fcb$l_filesize) {
		inode->fcb$l_filesize = pos;
		struct _vcb * vcb = exttwo_get_current_vcb();
		ext2_sync_inode(vcb, inode);
	}
	return 0;
}

int block_truncate_page(struct address_space *mapping, loff_t from,get_block_t *get_block)
{
  // mapping is really inode
  struct _fcb * inode = (void*) mapping;
	struct _fcb * fcb=e2_search_fcb(inode);
	unsigned long index = from >> PAGE_CACHE_SHIFT;
	unsigned offset = from & (PAGE_CACHE_SIZE-1);
	unsigned blocksize, iblock, length, pos;
	struct page *page;
	int err;
	int lbn;

	struct _vcb * vcb = exttwo_get_current_vcb();
	struct ext2_super_block * sb = vcb->vcb$l_cache;
	int i_blkbits = EXT2_BLOCK_SIZE_BITS(sb);
	blocksize = 1 << i_blkbits;
	length = offset & (blocksize - 1);

	/* Block boundary? Nothing to do */
	if (!length)
		return 0;

	length = blocksize - length;
	iblock = index << (PAGE_CACHE_SHIFT - i_blkbits);
	
	page = alloc_pages(GFP_KERNEL, 0);// was grab_cache_page(mapping, index);
	err = -ENOMEM;
	if (!page)
		goto out;

	/* Find the buffer that contains "offset" */
	pos = blocksize;
	while (offset >= pos) {
		iblock++;
		pos += blocksize;
	}

	err = 0;

	lbn = e2_map_vbn(fcb,iblock);

	myqio(READ, page_address(page)+pos,blocksize,lbn,0,vms_block_factor(i_blkbits));

	memset(kmap(page) + offset, 0, length);
	flush_dcache_page(page);
	kunmap(page);

	err = 0;

unlock:
#if 0
	UnlockPage(page);
#endif
	page_cache_release(page);
out:
	return err;
}

int block_write_full_page2(struct _fcb *inode, struct page *page, unsigned long pageno)
{
	struct _fcb * fcb=e2_search_fcb(inode);
	unsigned long end_index = inode->fcb$l_filesize >> PAGE_CACHE_SHIFT;
	unsigned offset;
	int err;

	/* easy case */
	if (pageno < end_index)
		return __block_write_full_page2(inode, page, pageno);

	/* things got complicated... */
	offset = inode->fcb$l_filesize & (PAGE_CACHE_SIZE-1);
	/* OK, are we completely out? */
	if (pageno >= end_index+1 || !offset) {
#if 0
		UnlockPage(page);
#endif
		return -EIO;
	}

	/* Sigh... will have to work, then... */
	err = __block_prepare_write(inode, page, 0, offset, pageno);
	if (!err) {
		memset(page_address(page) + offset, 0, PAGE_CACHE_SIZE - offset);
		flush_dcache_page(page);
		__block_commit_write(inode,page,0,offset,pageno);
done:
		kunmap(page);
#if 0
		UnlockPage(page);
#endif
		return err;
	}
#if 0
	ClearPageUptodate(page);
#endif
	goto done;
}

int block_write_full_page3(struct _fcb * fcb, struct page *page, unsigned long pageno)
{
	struct _fcb * inode=fcb->fcb$l_primfcb;
	unsigned long end_index = inode->fcb$l_filesize >> PAGE_CACHE_SHIFT;
	unsigned offset;
	int err;

	/* easy case */
	if (pageno < end_index)
		return __block_write_full_page2(inode, page, pageno);

	/* things got complicated... */
	offset = inode->fcb$l_filesize & (PAGE_CACHE_SIZE-1);
	/* OK, are we completely out? */
	if (pageno >= end_index+1 || !offset) {
#if 0
		UnlockPage(page);
#endif
		return -EIO;
	}

	/* Sigh... will have to work, then... */
	err = __block_prepare_write(inode, page, 0, offset, pageno);
	if (!err) {
		memset(page_address(page) + offset, 0, PAGE_CACHE_SIZE - offset);
		flush_dcache_page(page);
		__block_commit_write(inode,page,0,offset,pageno);
done:
		kunmap(page);
#if 0
		UnlockPage(page);
#endif
		return err;
	}
#if 0
	ClearPageUptodate(page);
#endif
	goto done;
}

int generic_block_bmap(struct address_space *mapping, long block, get_block_t *get_block)
{
	struct buffer_head tmp;
	struct _fcb *inode = mapping->host;
	tmp.b_state = 0;
	tmp.b_blocknr = 0;
	struct _vcb * vcb = exttwo_get_current_vcb();
	get_block(vcb, inode, block, &tmp, 0);
	return tmp.b_blocknr;
}

int generic_direct_IO(int rw, struct _fcb * inode, struct kiobuf * iobuf, unsigned long blocknr, int blocksize, get_block_t * get_block)
{
	struct _fcb * fcb=e2_search_fcb(inode);
	int i, nr_blocks, retval=0;
	unsigned long * blocks = iobuf->blocks;
	int length;
	int sts;
	int type;
	struct _iosb iosb;
	unsigned long iblock;

	length = iobuf->length;
	nr_blocks = length / blocksize;
	struct _vcb * vcb = exttwo_get_current_vcb();
	struct ext2_super_block * sb = vcb->vcb$l_cache;
	int i_blkbits = EXT2_BLOCK_SIZE_BITS(sb);
	iblock = blocknr << (PAGE_CACHE_SHIFT - i_blkbits);
	/* build the blocklist */
	for (i = 0; i < nr_blocks; i++, blocknr++) {
		struct buffer_head bh;

		bh.b_state = 0;
		bh.b_dev = 0; // FCB_DEV(inode);
		bh.b_size = blocksize;

			if (fcb)
			  blocknr=e2_map_vbn(fcb,iblock);
			else
			  blocknr=iblock;
			if (blocknr==-1) {
			  if (rw!=WRITE) goto out;
			  retval=ext2_get_block(vcb, inode, iblock, &blocknr, 1, fcb);
			}

		if (retval) {
			if (!i)
				/* report error to userspace */
				goto out;
			else
				/* do short I/O utill 'i' */
				break;
		}

		if (rw == READ) {
			if (buffer_new(&bh))
				BUG();
			if (!buffer_mapped(&bh)) {
				/* there was an hole in the filesystem */
				blocks[i] = -1UL;
				continue;
			}
		} else {
#if 0
			if (buffer_new(&bh))
				unmap_underlying_metadata(&bh);
#endif
			if (!buffer_mapped(&bh))
				BUG();
		}
		if (rw==READ)
		  type=IO$_READPBLK;
		else
		  type=IO$_WRITEPBLK;
		struct _ucb * ucb;
		if (vcb->vcb$l_rvn) {
		  struct _rvt * rvt = vcb->vcb$l_rvt;
		  struct _ucb ** ucblst = rvt->rvt$l_ucblst;
		  ucb = ucblst[fcb->fcb$b_fid_rvn - 1];
		} else 
		  ucb = vcb->vcb$l_rvt;
		sts = exe$qiow(EXT2_EF,(unsigned short)x2p->io_channel,type,&iosb,0,0,
			       bh.b_data,blocksize, bh.b_blocknr*vms_block_factor(i_blkbits),ucb->ucb$w_fill_0,0,0);
		//		blocks[i] = bh.b_blocknr;
	}

	/* patch length to handle short I/O */
	iobuf->length = i * blocksize;
	//	retval = brw_kiovec(rw, 1, &iobuf, FCB_DEV(inode), iobuf->blocks, blocksize);
	/* restore orig length */
	iobuf->length = length;
 out:

	return retval;
}

/*
 * IO completion routine for a buffer_head being used for kiobuf IO: we
 * can't dispatch the kiobuf callback until io_count reaches 0.  
 */

static void end_buffer_io_kiobuf(struct buffer_head *bh, int uptodate)
{
	struct kiobuf *kiobuf;
	
	mark_buffer_uptodate(bh, uptodate);

	kiobuf = bh->b_private;
	unlock_buffer(bh);
	end_kio_request(kiobuf, uptodate);
}

/*
 * For brw_kiovec: submit a set of buffer_head temporary IOs and wait
 * for them to complete.  Clean up the buffer_heads afterwards.  
 */

static int wait_kio(int rw, int nr, struct buffer_head *bh[], int size)
{
	int iosize, err;
	int i;
	struct buffer_head *tmp;

	iosize = 0;
	err = 0;

	for (i = nr; --i >= 0; ) {
		iosize += size;
		tmp = bh[i];
		if (buffer_locked(tmp)) {
		  //wait_on_buffer(tmp);
		}
		
		if (!buffer_uptodate(tmp)) {
			/* We are traversing bh'es in reverse order so
                           clearing iosize on error calculates the
                           amount of IO before the first error. */
			iosize = 0;
			err = -EIO;
		}
	}
	
	if (iosize)
		return iosize;
	return err;
}

/*
 * Start I/O on a physical range of kernel memory, defined by a vector
 * of kiobuf structs (much like a user-space iovec list).
 *
 * The kiobuf must already be locked for IO.  IO is submitted
 * asynchronously: you need to check page->locked, page->uptodate, and
 * maybe wait on page->wait.
 *
 * It is up to the caller to make sure that there are enough blocks
 * passed in to completely map the iobufs to disk.
 */

int brw_kiovec(int rw, int nr, struct kiobuf *iovec[], 
	       kdev_t dev, unsigned long b[], int size)
{
	int		err;
	int		length;
	int		transferred;
	int		i;
	int		bufind;
	int		pageind;
	int		bhind;
	int		offset;
	unsigned long	blocknr;
	struct kiobuf *	iobuf = NULL;
	struct page *	map;
	struct buffer_head *tmp, **bhs = NULL;

	if (!nr)
		return 0;
	
	/* 
	 * First, do some alignment and validity checks 
	 */
	for (i = 0; i < nr; i++) {
		iobuf = iovec[i];
		if ((iobuf->offset & (size-1)) ||
		    (iobuf->length & (size-1)))
			return -EINVAL;
		if (!iobuf->nr_pages)
			panic("brw_kiovec: iobuf not initialised");
	}

	/* 
	 * OK to walk down the iovec doing page IO on each page we find. 
	 */
	bufind = bhind = transferred = err = 0;
	for (i = 0; i < nr; i++) {
		iobuf = iovec[i];
		offset = iobuf->offset;
		length = iobuf->length;
		iobuf->errno = 0;
		if (!bhs)
			bhs = iobuf->bh;
		
		for (pageind = 0; pageind < iobuf->nr_pages; pageind++) {
			map  = iobuf->maplist[pageind];
			if (!map) {
				err = -EFAULT;
				goto finished;
			}
			
			while (length > 0) {
				blocknr = b[bufind++];
				if (blocknr == -1UL) {
					if (rw == READ) {
						/* there was an hole in the filesystem */
						memset(kmap(map) + offset, 0, size);
						flush_dcache_page(map);
						kunmap(map);

						transferred += size;
						goto skip_block;
					} else
						BUG();
				}
				tmp = bhs[bhind++];

				tmp->b_size = size;
#if 0
				set_bh_page(tmp, map, offset);
#endif
				tmp->b_this_page = tmp;

				init_buffer(tmp, end_buffer_io_kiobuf, iobuf);
				tmp->b_dev = dev;
				tmp->b_blocknr = blocknr;
				tmp->b_state = (1 << BH_Mapped) | (1 << BH_Lock) | (1 << BH_Req);

				if (rw == WRITE) {
					set_bit(BH_Uptodate, &tmp->b_state);
					clear_bit(BH_Dirty, &tmp->b_state);
				} else
					set_bit(BH_Uptodate, &tmp->b_state);

				atomic_inc(&iobuf->io_count);
				vms_submit_bh(rw, tmp);
				/* 
				 * Wait for IO if we have got too much 
				 */
				if (bhind >= KIO_MAX_SECTORS) {
					kiobuf_wait_for_io(iobuf); /* wake-one */
					err = wait_kio(rw, bhind, bhs, size);
					if (err >= 0)
						transferred += err;
					else
						goto finished;
					bhind = 0;
				}

			skip_block:
				length -= size;
				offset += size;

				if (offset >= PAGE_SIZE) {
					offset = 0;
					break;
				}
			} /* End of block loop */
		} /* End of page loop */		
	} /* End of iovec loop */

	/* Is there any IO still left to submit? */
	if (bhind) {
		kiobuf_wait_for_io(iobuf); /* wake-one */
		err = wait_kio(rw, bhind, bhs, size);
		if (err >= 0)
			transferred += err;
		else
			goto finished;
	}

 finished:
	if (transferred)
		return transferred;
	return err;
}

/*
 * Start I/O on a page.
 * This function expects the page to be locked and may return
 * before I/O is complete. You then have to check page->locked,
 * page->uptodate, and maybe wait on page->wait.
 *
 * brw_page() is SMP-safe, although it's being called with the
 * kernel lock held - but the code is ready.
 *
 * FIXME: we need a swapper_inode->get_block function to remove
 *        some of the bmap kludges and interface ugliness here.
 */
int brw_page(int rw, struct page *page, kdev_t dev, int b[], int size)
{
  printk("warning, brw_page does nothing\n");
  return 0;
}

int block_symlink(struct _fcb *inode, const char *symname, int len)
{
	struct page *page = alloc_pages(GFP_KERNEL, 0); //was grab_cache_page
	int err = -ENOMEM;
	char *kaddr;

	if (!page)
		goto fail;
	err = block_prepare_write2(inode, page, 0, len-1, 0);
	if (err)
		goto fail_map;
	kaddr = page_address(page);
	memcpy(kaddr, symname, len-1);
	generic_commit_write2(inode, page, 0, len-1, 0);
	/*
	 * Notice that we are _not_ going to block here - end of page is
	 * unmapped, so this will only try to map the rest of page, see
	 * that it is unmapped (typically even will not look into inode -
	 * ->fcb$l_filesize will be enough for everything) and zero it out.
	 * OTOH it's obviously correct and should make the page up-to-date.
	 */
	err = block_read_full_page2(inode, page, 0);
	//	wait_on_page(page);
	page_cache_release(page);
	if (err < 0)
		goto fail;
	struct _vcb * vcb = exttwo_get_current_vcb();
	ext2_sync_inode(vcb, inode);
	return 0;
fail_map:
#if 0
	UnlockPage(page);
#endif
	page_cache_release(page);
fail:
	return err;
}

/*
 * Can the buffer be thrown out?
 */
#define BUFFER_BUSY_BITS	((1<<BH_Dirty) | (1<<BH_Lock))
#define buffer_busy(bh)		(atomic_read(&(bh)->b_count) | ((bh)->b_state & BUFFER_BUSY_BITS))

/* ================== Debugging =================== */

void show_buffers(void)
{
#ifdef CONFIG_SMP
	struct buffer_head * bh;
	int found = 0, locked = 0, dirty = 0, used = 0, lastused = 0;
	int nlist;
	static char *buf_types[NR_LIST] = { "CLEAN", "LOCKED", "DIRTY", };
#endif

#if 0
	printk("Buffer memory:   %6dkB\n",
			atomic_read(&buffermem_pages) << (PAGE_SHIFT-10));

	printk("Cache memory:   %6dkB\n",
			(atomic_read(&page_cache_size)- atomic_read(&buffermem_pages)) << (PAGE_SHIFT-10));
#endif

#ifdef CONFIG_SMP /* trylock does nothing on UP and so we could deadlock */
#if 0
	// not yet
	if (!spin_trylock(&lru_list_lock))
		return;
	for(nlist = 0; nlist < NR_LIST; nlist++) {
		found = locked = dirty = used = lastused = 0;
		bh = lru_list[nlist];
		if(!bh) continue;

		do {
			found++;
			if (buffer_locked(bh))
				locked++;
			if (buffer_dirty(bh))
				dirty++;
			if (atomic_read(&bh->b_count))
				used++, lastused = found;
			bh = bh->b_next_free;
		} while (bh != lru_list[nlist]);
		{
			int tmp = nr_buffers_type[nlist];
			if (found != tmp)
				printk("%9s: BUG -> found %d, reported %d\n",
				       buf_types[nlist], found, tmp);
		}
		printk("%9s: %d buffers, %lu kbyte, %d used (last=%d), "
		       "%d locked, %d dirty\n",
		       buf_types[nlist], found, size_buffers_type[nlist]>>10,
		       used, lastused, locked, dirty);
	}
	spin_unlock(&lru_list_lock);
#endif
#endif
}

/* ===================== Init ======================= */

/*
 * allocate the hash table and init the free list
 * Use gfp() for the hash table to decrease TLB misses, use
 * SLAB cache for buffer heads.
 */
void __init buffer_init(unsigned long mempages)
{
	printk("%%KERNEL-I-ISNOMORE, Linux Buffer-cache is no longer used\n");
}

/* ====================== bdflush support =================== */

/* This is a simple kernel daemon, whose job it is to provide a dynamic
 * response to dirty buffers.  Once this process is activated, we write back
 * a limited number of buffers to the disks and then go back to sleep again.
 */

DECLARE_WAIT_QUEUE_HEAD(bdflush_wait);

int block_sync_page(struct page *page)
{
	return 0;
}

/* This is the interface to bdflush.  As we get more sophisticated, we can
 * pass tuning parameters to this "process", to adjust how it behaves. 
 * We would want to verify each parameter, however, to make sure that it 
 * is reasonable. */

asmlinkage long sys_bdflush(int func, long data)
{
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (func == 1) {
		/* do_exit directly and let kupdate to do its work alone. */
		do_exit(0);
#if 0 /* left here as it's the only example of lazy-mm-stuff used from
	 a syscall that doesn't care about the current mm context. */
		int error;
		struct mm_struct *user_mm;

		/*
		 * bdflush will spend all of it's time in kernel-space,
		 * without touching user-space, so we can switch it into
		 * 'lazy TLB mode' to reduce the cost of context-switches
		 * to and from bdflush.
		 */
		user_mm = start_lazy_tlb();
#if 0
		error = sync_old_buffers();
#endif
		end_lazy_tlb(user_mm);
		return error;
#endif
	}

	/* Basically func 1 means read param 1, 2 means write param 1, etc */
	if (func >= 2) {
		int i = (func-2) >> 1;
		if (i >= 0 && i < N_PARAM) {
			if ((func & 1) == 0)
				return put_user(bdf_prm.data[i], (int*)data);

			if (data >= bdflush_min[i] && data <= bdflush_max[i]) {
				bdf_prm.data[i] = data;
				return 0;
			}
		}
		return -EINVAL;
	}

	/* Having func 0 used to launch the actual bdflush and then never
	 * return (unless explicitly killed). We return zero here to 
	 * remain semi-compatible with present update(8) programs.
	 */
	return 0;
}

