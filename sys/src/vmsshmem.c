// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004. Based on shmem.c.

/*
 * Resizable virtual memory filesystem for Linux.
 *
 * Copyright (C) 2000 Linus Torvalds.
 *		 2000 Transmeta Corp.
 *		 2000-2001 Christoph Rohland
 *		 2000-2001 SAP AG
 * 
 * This file is released under the GPL.
 */

/*
 * This virtual memory filesystem is heavily based on the ramfs. It
 * extends ramfs by the ability to use swap and honor resource limits
 * which makes it a completely usable filesystem.
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/devfs_fs_kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/file.h>
#include <linux/swap.h>
#include <linux/pagemap.h>
#include <linux/string.h>
#include <linux/locks.h>
#include <linux/smp_lock.h>

#include <asm/uaccess.h>

/* This magic number is used in glibc for posix shared memory */
#define TMPFS_MAGIC	0x01021994

#define ENTRIES_PER_PAGE (PAGE_CACHE_SIZE/sizeof(unsigned long))

#define SHMEM_SB(sb) (&sb->u.shmem_sb)

static struct super_operations shmem_ops;
static struct address_space_operations shmem_aops;
static struct file_operations shmem_file_operations;
static struct inode_operations shmem_inode_operations;
static struct file_operations shmem_dir_operations;
static struct inode_operations shmem_dir_inode_operations;
static struct vm_operations_struct shmem_vm_ops;

LIST_HEAD (shmem_inodes);
static spinlock_t shmem_ilock = SPIN_LOCK_UNLOCKED;
atomic_t shmem_nrpages = ATOMIC_INIT(0); /* Not used right now */

#define BLOCKS_PER_PAGE (PAGE_CACHE_SIZE/512)

/*
 * shmem_recalc_inode - recalculate the size of an inode
 *
 * @inode: inode to recalc
 * @swap:  additional swap pages freed externally
 *
 * We have to calculate the free blocks since the mm can drop pages
 * behind our back
 *
 * But we know that normally
 * inodes->i_blocks/BLOCKS_PER_PAGE == 
 * 			inode->i_mapping->nrpages + info->swapped
 *
 * So the mm freed 
 * inodes->i_blocks/BLOCKS_PER_PAGE - 
 * 			(inode->i_mapping->nrpages + info->swapped)
 *
 * It has to be called with the spinlock held.
 */

static void shmem_recalc_inode(struct inode * inode)
{
	unsigned long freed;

	freed = (inode->i_blocks/BLOCKS_PER_PAGE) -
		(inode->i_mapping->nrpages + SHMEM_I(inode)->swapped);
	if (freed){
		struct shmem_sb_info * sbinfo = SHMEM_SB(inode->i_sb);
		inode->i_blocks -= freed*BLOCKS_PER_PAGE;
		spin_lock (&sbinfo->stat_lock);
		sbinfo->free_blocks += freed;
		spin_unlock (&sbinfo->stat_lock);
	}
}

/*
 * shmem_swp_entry - find the swap vector position in the info structure
 *
 * @info:  info structure for the inode
 * @index: index of the page to find
 * @page:  optional page to add to the structure. Has to be preset to
 *         all zeros
 *
 * If there is no space allocated yet it will return -ENOMEM when
 * page == 0 else it will use the page for the needed block.
 *
 * returns -EFBIG if the index is too big.
 *
 *
 * The swap vector is organized the following way:
 *
 * There are SHMEM_NR_DIRECT entries directly stored in the
 * shmem_inode_info structure. So small files do not need an addional
 * allocation.
 *
 * For pages with index > SHMEM_NR_DIRECT there is the pointer
 * i_indirect which points to a page which holds in the first half
 * doubly indirect blocks, in the second half triple indirect blocks:
 *
 * For an artificial ENTRIES_PER_PAGE = 4 this would lead to the
 * following layout (for SHMEM_NR_DIRECT == 16):
 *
 * i_indirect -> dir --> 16-19
 * 	      |	     +-> 20-23
 * 	      |
 * 	      +-->dir2 --> 24-27
 * 	      |	       +-> 28-31
 * 	      |	       +-> 32-35
 * 	      |	       +-> 36-39
 * 	      |
 * 	      +-->dir3 --> 40-43
 * 	       	       +-> 44-47
 * 	      	       +-> 48-51
 * 	      	       +-> 52-55
 */

#define SHMEM_MAX_BLOCKS (SHMEM_NR_DIRECT + ENTRIES_PER_PAGE * ENTRIES_PER_PAGE/2*(ENTRIES_PER_PAGE+1))

static swp_entry_t * shmem_swp_entry (struct shmem_inode_info *info, unsigned long index, unsigned long page) 
{
	unsigned long offset;
	void **dir;

	if (index < SHMEM_NR_DIRECT)
		return info->i_direct+index;

	index -= SHMEM_NR_DIRECT;
	offset = index % ENTRIES_PER_PAGE;
	index /= ENTRIES_PER_PAGE;

	if (!info->i_indirect) {
		info->i_indirect = (void *) page;
		return ERR_PTR(-ENOMEM);
	}

	dir = info->i_indirect + index;
	if (index >= ENTRIES_PER_PAGE/2) {
		index -= ENTRIES_PER_PAGE/2;
		dir = info->i_indirect + ENTRIES_PER_PAGE/2 
			+ index/ENTRIES_PER_PAGE;
		index %= ENTRIES_PER_PAGE;

		if(!*dir) {
			*dir = (void *) page;
			/* We return since we will need another page
                           in the next step */
			return ERR_PTR(-ENOMEM);
		}
		dir = ((void **)*dir) + index;
	}
	if (!*dir) {
		if (!page)
			return ERR_PTR(-ENOMEM);
		*dir = (void *)page;
	}
	return ((swp_entry_t *)*dir) + offset;
}

/*
 * shmem_alloc_entry - get the position of the swap entry for the
 *                     page. If it does not exist allocate the entry
 *
 * @info:	info structure for the inode
 * @index:	index of the page to find
 */
static inline swp_entry_t * shmem_alloc_entry (struct shmem_inode_info *info, unsigned long index)
{
	unsigned long page = 0;
	swp_entry_t * res;

	if (index >= SHMEM_MAX_BLOCKS)
		return ERR_PTR(-EFBIG);

	if (info->next_index <= index)
		info->next_index = index + 1;

	while ((res = shmem_swp_entry(info,index,page)) == ERR_PTR(-ENOMEM)) {
		page = get_zeroed_page(GFP_USER);
		if (!page)
			break;
	}
	return res;
}

/*
 * shmem_free_swp - free some swap entries in a directory
 *
 * @dir:   pointer to the directory
 * @count: number of entries to scan
 */
static int shmem_free_swp(swp_entry_t *dir, unsigned int count)
{
	swp_entry_t *ptr, entry;
	int freed = 0;

	for (ptr = dir; ptr < dir + count; ptr++) {
		if (!ptr->val)
			continue;
		entry = *ptr;
		*ptr = (swp_entry_t){0};
		freed++;
#if 0
		free_swap_and_cache(entry);
#endif
	}
	return freed;
}

/*
 * shmem_truncate_direct - free the swap entries of a whole doubly
 *                         indirect block
 *
 * @dir:	pointer to the pointer to the block
 * @start:	offset to start from (in pages)
 * @len:	how many pages are stored in this block
 *
 * Returns the number of freed swap entries.
 */

static inline unsigned long 
shmem_truncate_direct(swp_entry_t *** dir, unsigned long start, unsigned long len) {
	swp_entry_t **last, **ptr;
	unsigned long off, freed = 0;
 
	if (!*dir)
		return 0;

	last = *dir + (len + ENTRIES_PER_PAGE-1) / ENTRIES_PER_PAGE;
	off = start % ENTRIES_PER_PAGE;

	for (ptr = *dir + start/ENTRIES_PER_PAGE; ptr < last; ptr++) {
		if (!*ptr) {
			off = 0;
			continue;
		}

		if (!off) {
			freed += shmem_free_swp(*ptr, ENTRIES_PER_PAGE);
			free_page ((unsigned long) *ptr);
			*ptr = 0;
		} else {
			freed += shmem_free_swp(*ptr+off,ENTRIES_PER_PAGE-off);
			off = 0;
		}
	}
	
	if (!start) {
		free_page((unsigned long) *dir);
		*dir = 0;
	}
	return freed;
}

/*
 * shmem_truncate_indirect - truncate an inode
 *
 * @info:  the info structure of the inode
 * @index: the index to truncate
 *
 * This function locates the last doubly indirect block and calls
 * then shmem_truncate_direct to do the real work
 */
static inline unsigned long
shmem_truncate_indirect(struct shmem_inode_info *info, unsigned long index)
{
	swp_entry_t ***base;
	unsigned long baseidx, len, start;
	unsigned long max = info->next_index-1;

	if (max < SHMEM_NR_DIRECT) {
		info->next_index = index;
		return shmem_free_swp(info->i_direct + index,
				      SHMEM_NR_DIRECT - index);
	}

	if (max < ENTRIES_PER_PAGE * ENTRIES_PER_PAGE/2 + SHMEM_NR_DIRECT) {
		max -= SHMEM_NR_DIRECT;
		base = (swp_entry_t ***) &info->i_indirect;
		baseidx = SHMEM_NR_DIRECT;
		len = max+1;
	} else {
		max -= ENTRIES_PER_PAGE*ENTRIES_PER_PAGE/2+SHMEM_NR_DIRECT;
		if (max >= ENTRIES_PER_PAGE*ENTRIES_PER_PAGE*ENTRIES_PER_PAGE/2)
			BUG();

		baseidx = max & ~(ENTRIES_PER_PAGE*ENTRIES_PER_PAGE-1);
		base = (swp_entry_t ***) info->i_indirect + ENTRIES_PER_PAGE/2 + baseidx/ENTRIES_PER_PAGE/ENTRIES_PER_PAGE ;
		len = max - baseidx + 1;
		baseidx += ENTRIES_PER_PAGE*ENTRIES_PER_PAGE/2+SHMEM_NR_DIRECT;
	}

	if (index > baseidx) {
		info->next_index = index;
		start = index - baseidx;
	} else {
		info->next_index = baseidx;
		start = 0;
	}
	return shmem_truncate_direct(base, start, len);
}

static void shmem_truncate (struct inode * inode)
{
	unsigned long index;
	unsigned long freed = 0;
	struct shmem_inode_info * info = SHMEM_I(inode);

	down(&info->sem);
	inode->i_ctime = inode->i_mtime = CURRENT_TIME;
	spin_lock (&info->lock);
	index = (inode->i_size + PAGE_CACHE_SIZE - 1) >> PAGE_CACHE_SHIFT;

	while (index < info->next_index) 
		freed += shmem_truncate_indirect(info, index);

	info->swapped -= freed;
	shmem_recalc_inode(inode);
	spin_unlock (&info->lock);
	up(&info->sem);
}

static void shmem_delete_inode(struct inode * inode)
{
	struct shmem_sb_info *sbinfo = SHMEM_SB(inode->i_sb);

	inode->i_size = 0;
	if (inode->i_op->truncate == shmem_truncate){ 
		spin_lock (&shmem_ilock);
		list_del (&SHMEM_I(inode)->list);
		spin_unlock (&shmem_ilock);
		shmem_truncate (inode);
	}
	spin_lock (&sbinfo->stat_lock);
	sbinfo->free_inodes++;
	spin_unlock (&sbinfo->stat_lock);
	clear_inode(inode);
}

static int shmem_clear_swp (swp_entry_t entry, swp_entry_t *ptr, int size) {
	swp_entry_t *test;

	for (test = ptr; test < ptr + size; test++) {
		if (test->val == entry.val) {
#if 0
			swap_free (entry);
#endif
			*test = (swp_entry_t) {0};
			return test - ptr;
		}
	}
	return -1;
}

static int shmem_unuse_inode (struct shmem_inode_info *info, swp_entry_t entry, struct page *page)
{
	swp_entry_t *ptr;
	unsigned long idx;
	int offset;
	
	idx = 0;
	spin_lock (&info->lock);
	offset = shmem_clear_swp (entry, info->i_direct, SHMEM_NR_DIRECT);
	if (offset >= 0)
		goto found;

	for (idx = SHMEM_NR_DIRECT; idx < info->next_index; 
	     idx += ENTRIES_PER_PAGE) {
		ptr = shmem_swp_entry(info, idx, 0);
		if (IS_ERR(ptr))
			continue;
		offset = shmem_clear_swp (entry, ptr, ENTRIES_PER_PAGE);
		if (offset >= 0)
			goto found;
	}
	spin_unlock (&info->lock);
	return 0;
found:
#if 0
	delete_from_swap_cache(page);
	//add_to_page_cache(page, info->inode->i_mapping, offset + idx);
	SetPageDirty(page);
	SetPageUptodate(page);
#endif
	info->swapped--;
	spin_unlock(&info->lock);
	return 1;
}

/*
 * unuse_shmem() search for an eventually swapped out shmem page.
 */
void shmem_unuse(swp_entry_t entry, struct page *page)
{
	struct list_head *p;
	struct shmem_inode_info * info;

	spin_lock (&shmem_ilock);
	list_for_each(p, &shmem_inodes) {
		info = list_entry(p, struct shmem_inode_info, list);

		if (shmem_unuse_inode(info, entry, page))
			break;
	}
	spin_unlock (&shmem_ilock);
}

/*
 * Move the page from the page cache to the swap cache.
 *
 * The page lock prevents multiple occurences of shmem_writepage at
 * once.  We still need to guard against racing with
 * shmem_getpage_locked().  
 */
static int shmem_writepage(struct page * page)
{
	struct shmem_inode_info *info;
	swp_entry_t *entry, swap;
	struct address_space *mapping;
	unsigned long index;
	struct inode *inode;

#if 0
	if (!PageLocked(page))
		BUG();
	if (!PageLaunder(page))
		return fail_writepage(page);
#endif

#if 0
	mapping = page->mapping;
	index = page->index;
#endif
	inode = mapping->host;
	info = SHMEM_I(inode);
#if 0
	if (info->locked)
		return fail_writepage(page);
#endif
getswap:
#if 0
	swap = get_swap_page();
	if (!swap.val)
		return fail_writepage(page);
#endif

	spin_lock(&info->lock);
	entry = shmem_swp_entry(info, index, 0);
	if (IS_ERR(entry))	/* this had been allocated on page allocation */
		BUG();
	shmem_recalc_inode(inode);
	if (entry->val)
		BUG();

	/* Remove it from the page cache */
	//remove_inode_page(page);
	page_cache_release(page);

	/* Add it to the swap cache */
#if 0
	if (add_to_swap_cache(page, swap) != 0) {
		/*
		 * Raced with "speculative" read_swap_cache_async.
		 * Add page back to page cache, unref swap, try again.
		 */
	  //add_to_page_cache_locked(page, mapping, index);
		spin_unlock(&info->lock);
		swap_free(swap);
		goto getswap;
	}
#endif

	*entry = swap;
	info->swapped++;
	spin_unlock(&info->lock);
#if 0
	SetPageUptodate(page);
	set_page_dirty(page);
	UnlockPage(page);
#endif
	return 0;
}

/*
 * shmem_getpage_locked - either get the page from swap or allocate a new one
 *
 * If we allocate a new one we do not mark it dirty. That's up to the
 * vm. If we swap it in we mark it dirty since we also free the swap
 * entry since a page cannot live in both the swap and page cache
 *
 * Called with the inode locked, so it cannot race with itself, but we
 * still need to guard against racing with shm_writepage(), which might
 * be trying to move the page to the swap cache as we run.
 */
static struct page * shmem_getpage_locked(struct shmem_inode_info *info, struct inode * inode, unsigned long idx)
{
	struct address_space * mapping = inode->i_mapping;
	struct shmem_sb_info *sbinfo;
	struct page * page;
	swp_entry_t *entry;

repeat:
	page = 0;// find_lock_page(mapping, idx);
	if (page)
		return page;

	entry = shmem_alloc_entry (info, idx);
	if (IS_ERR(entry))
		return (void *)entry;

	spin_lock (&info->lock);
	
	/* The shmem_alloc_entry() call may have blocked, and
	 * shmem_writepage may have been moving a page between the page
	 * cache and swap cache.  We need to recheck the page cache
	 * under the protection of the info->lock spinlock. */

	page = 0;//find_get_page(mapping, idx);
	if (page) {
#if 0
		if (TryLockPage(page))
			goto wait_retry;
#endif
		spin_unlock (&info->lock);
		return page;
	}
	
	shmem_recalc_inode(inode);
	if (entry->val) {
		unsigned long flags;

		/* Look it up and read it in.. */
		page = 0;//find_get_page(&swapper_space, entry->val);
		if (!page) {
			swp_entry_t swap = *entry;
			spin_unlock (&info->lock);
#if 0
			swapin_readahead(*entry);
			page = read_swap_cache_async(*entry);
#endif
			if (!page) {
				if (entry->val != swap.val)
					goto repeat;
				return ERR_PTR(-ENOMEM);
			}
			//wait_on_page(page);
#if 0
			if (!Page_Uptodate(page) && entry->val == swap.val) {
				page_cache_release(page);
				return ERR_PTR(-EIO);
			}
#endif	
		
			/* Too bad we can't trust this page, because we
			 * dropped the info->lock spinlock */
			page_cache_release(page);
			goto repeat;
		}

		/* We have to this with page locked to prevent races */
#if 0
		if (TryLockPage(page)) 
			goto wait_retry;

		swap_free(*entry);
		*entry = (swp_entry_t) {0};
		delete_from_swap_cache(page);
		flags = page->pfn$l_page_state & ~((1 << PG_uptodate) | (1 << PG_error) | (1 << PG_referenced) | (1 << PG_arch_1));
		page->pfn$l_page_state = flags | (1 << PG_dirty);
#endif
		//add_to_page_cache_locked(page, mapping, idx);
		info->swapped--;
		spin_unlock (&info->lock);
	} else {
		sbinfo = SHMEM_SB(inode->i_sb);
		spin_unlock (&info->lock);
		spin_lock (&sbinfo->stat_lock);
		if (sbinfo->free_blocks == 0)
			goto no_space;
		sbinfo->free_blocks--;
		spin_unlock (&sbinfo->stat_lock);

		/* Ok, get a new page.  We don't have to worry about the
		 * info->lock spinlock here: we cannot race against
		 * shm_writepage because we have already verified that
		 * there is no page present either in memory or in the
		 * swap cache, so we are guaranteed to be populating a
		 * new shm entry.  The inode semaphore we already hold
		 * is enough to make this atomic. */
		page = page_cache_alloc(mapping);
		if (!page)
			return ERR_PTR(-ENOMEM);
		clear_highpage(page);
		inode->i_blocks += BLOCKS_PER_PAGE;
		//add_to_page_cache (page, mapping, idx);
	}

	/* We have the page */
#if 0
	SetPageUptodate(page);
#endif
	return page;
no_space:
	spin_unlock (&sbinfo->stat_lock);
	return ERR_PTR(-ENOSPC);

wait_retry:
	spin_unlock (&info->lock);
	//wait_on_page(page);
	page_cache_release(page);
	goto repeat;
}

static int shmem_getpage(struct inode * inode, unsigned long idx, struct page **ptr)
{
	struct shmem_inode_info *info = SHMEM_I(inode);
	int error;

	down (&info->sem);
	*ptr = ERR_PTR(-EFAULT);
	if (inode->i_size <= (loff_t) idx * PAGE_CACHE_SIZE)
		goto failed;

	*ptr = shmem_getpage_locked(info, inode, idx);
	if (IS_ERR (*ptr))
		goto failed;

#if 0
	UnlockPage(*ptr);
#endif
	up (&info->sem);
	return 0;
failed:
	up (&info->sem);
	error = PTR_ERR(*ptr);
	*ptr = NOPAGE_SIGBUS;
	if (error == -ENOMEM)
		*ptr = NOPAGE_OOM;
	return error;
}

struct page * shmem_nopage(struct vm_area_struct * vma, unsigned long address, int unused)
{
#if 0
	struct page * page;
	unsigned int idx;
	struct inode * inode = vma->vm_file->f_dentry->d_inode;

	idx = (address - vma->vm_start) >> PAGE_CACHE_SHIFT;
	idx += vma->vm_pgoff;

	if (shmem_getpage(inode, idx, &page))
		return page;

	flush_page_to_ram(page);
	return(page);
#endif
	return 0;
}

void shmem_lock(struct file * file, int lock)
{
	struct inode * inode = file->f_dentry->d_inode;
	struct shmem_inode_info * info = SHMEM_I(inode);

	down(&info->sem);
	info->locked = lock;
	up(&info->sem);
}

static int shmem_mmap(struct file * file, struct vm_area_struct * vma)
{
#if 0
	struct vm_operations_struct * ops;
	struct inode *inode = file->f_dentry->d_inode;

	ops = &shmem_vm_ops;
	if (!inode->i_sb || !S_ISREG(inode->i_mode))
		return -EACCES;
	UPDATE_ATIME(inode);
	vma->vm_ops = ops;
#endif
	return 0;
}

struct inode *shmem_get_inode(struct super_block *sb, int mode, int dev)
{
	struct inode * inode;
	struct shmem_inode_info *info;
	struct shmem_sb_info *sbinfo = SHMEM_SB(sb);

	spin_lock (&sbinfo->stat_lock);
	if (!sbinfo->free_inodes) {
		spin_unlock (&sbinfo->stat_lock);
		return NULL;
	}
	sbinfo->free_inodes--;
	spin_unlock (&sbinfo->stat_lock);

	inode = new_inode(sb);
	if (inode) {
		inode->i_mode = mode;
		inode->i_uid = current->fsuid;
		inode->i_gid = current->fsgid;
		inode->i_blksize = PAGE_CACHE_SIZE;
		inode->i_blocks = 0;
		inode->i_rdev = NODEV;
		inode->i_mapping->a_ops = &shmem_aops;
		inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
		info = SHMEM_I(inode);
		info->inode = inode;
		spin_lock_init (&info->lock);
		sema_init (&info->sem, 1);
		switch (mode & S_IFMT) {
		default:
			init_special_inode(inode, mode, dev);
			break;
		case S_IFREG:
			inode->i_op = &shmem_inode_operations;
			inode->i_fop = &shmem_file_operations;
			spin_lock (&shmem_ilock);
			list_add (&SHMEM_I(inode)->list, &shmem_inodes);
			spin_unlock (&shmem_ilock);
			break;
		case S_IFDIR:
			inode->i_nlink++;
			inode->i_op = &shmem_dir_inode_operations;
			inode->i_fop = &shmem_dir_operations;
			break;
		case S_IFLNK:
			break;
		}
	}
	return inode;
}

static int shmem_set_size(struct shmem_sb_info *info,
			  unsigned long max_blocks, unsigned long max_inodes)
{
	int error;
	unsigned long blocks, inodes;

	spin_lock(&info->stat_lock);
	blocks = info->max_blocks - info->free_blocks;
	inodes = info->max_inodes - info->free_inodes;
	error = -EINVAL;
	if (max_blocks < blocks)
		goto out;
	if (max_inodes < inodes)
		goto out;
	error = 0;
	info->max_blocks  = max_blocks;
	info->free_blocks = max_blocks - blocks;
	info->max_inodes  = max_inodes;
	info->free_inodes = max_inodes - inodes;
out:
	spin_unlock(&info->stat_lock);
	return error;
}

static struct super_block *shmem_read_super(struct super_block * sb, void * data, int silent)
{
	struct inode * inode;
	struct dentry * root;
	unsigned long blocks, inodes;
	int mode   = S_IRWXUGO | S_ISVTX;
	uid_t uid = current->fsuid;
	gid_t gid = current->fsgid;
	struct shmem_sb_info *sbinfo = SHMEM_SB(sb);
	struct sysinfo si;

	/*
	 * Per default we only allow half of the physical ram per
	 * tmpfs instance
	 */
	si_meminfo(&si);
	blocks = inodes = si.totalram / 2;

#ifdef CONFIG_TMPFS
	if (shmem_parse_options (data, &mode, &uid, &gid, &blocks, &inodes))
		return NULL;
#endif

	spin_lock_init (&sbinfo->stat_lock);
	sbinfo->max_blocks = blocks;
	sbinfo->free_blocks = blocks;
	sbinfo->max_inodes = inodes;
	sbinfo->free_inodes = inodes;
	sb->s_maxbytes = (unsigned long long) SHMEM_MAX_BLOCKS << PAGE_CACHE_SHIFT;
	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_magic = TMPFS_MAGIC;
	sb->s_op = &shmem_ops;
	inode = shmem_get_inode(sb, S_IFDIR | mode, 0);
	if (!inode)
		return NULL;

	inode->i_uid = uid;
	inode->i_gid = gid;
	root = d_alloc_root(inode);
	if (!root) {
		iput(inode);
		return NULL;
	}
	sb->s_root = root;
	return sb;
}



static struct address_space_operations shmem_aops = {
	writepage:	shmem_writepage,
};

static struct file_operations shmem_file_operations = {
	mmap:	shmem_mmap,
#ifdef CONFIG_TMPFS
	read:	shmem_file_read,
	write:	shmem_file_write,
	fsync:	shmem_sync_file,
#endif
};

static struct inode_operations shmem_inode_operations = {
	truncate:	shmem_truncate,
};

static struct file_operations shmem_dir_operations = {
	read:		generic_read_dir,
	readdir:	dcache_readdir,
#ifdef CONFIG_TMPFS
	fsync:		shmem_sync_file,
#endif
};

static struct inode_operations shmem_dir_inode_operations = {
#ifdef CONFIG_TMPFS
	create:		shmem_create,
	lookup:		shmem_lookup,
	link:		shmem_link,
	unlink:		shmem_unlink,
	symlink:	shmem_symlink,
	mkdir:		shmem_mkdir,
	rmdir:		shmem_rmdir,
	mknod:		shmem_mknod,
	rename:		shmem_rename,
#endif
};

static struct super_operations shmem_ops = {
#ifdef CONFIG_TMPFS
	statfs:		shmem_statfs,
	remount_fs:	shmem_remount_fs,
#endif
	delete_inode:	shmem_delete_inode,
	put_inode:	force_delete,	
};

static struct vm_operations_struct shmem_vm_ops = {
	nopage:	shmem_nopage,
};

#ifdef CONFIG_TMPFS
/* type "shm" will be tagged obsolete in 2.5 */
static DECLARE_FSTYPE(shmem_fs_type, "shm", shmem_read_super, FS_LITTER);
static DECLARE_FSTYPE(tmpfs_fs_type, "tmpfs", shmem_read_super, FS_LITTER);
#else
static DECLARE_FSTYPE(tmpfs_fs_type, "tmpfs", shmem_read_super, FS_LITTER|FS_NOMOUNT);
#endif
static struct vfsmount *shm_mnt;

static int __init init_shmem_fs(void)
{
	int error;
	struct vfsmount * res;

	if ((error = register_filesystem(&tmpfs_fs_type))) {
		printk (KERN_ERR "Could not register tmpfs\n");
		return error;
	}
#ifdef CONFIG_TMPFS
	if ((error = register_filesystem(&shmem_fs_type))) {
		printk (KERN_ERR "Could not register shm fs\n");
		return error;
	}
	devfs_mk_dir (NULL, "shm", NULL);
#endif
	res = kern_mount(&tmpfs_fs_type);
	if (IS_ERR (res)) {
		printk (KERN_ERR "could not kern_mount tmpfs\n");
		unregister_filesystem(&tmpfs_fs_type);
		return PTR_ERR(res);
	}
	shm_mnt = res;

	/* The internal instance should not do size checking */
	if ((error = shmem_set_size(SHMEM_SB(res->mnt_sb), ULONG_MAX, ULONG_MAX)))
		printk (KERN_ERR "could not set limits on internal tmpfs\n");

	return 0;
}

static void __exit exit_shmem_fs(void)
{
#ifdef CONFIG_TMPFS
	unregister_filesystem(&shmem_fs_type);
#endif
	unregister_filesystem(&tmpfs_fs_type);
	mntput(shm_mnt);
}

module_init(init_shmem_fs)
module_exit(exit_shmem_fs)

/*
 * shmem_file_setup - get an unlinked file living in shmem fs
 *
 * @name: name for dentry (to be seen in /proc/<pid>/maps
 * @size: size to be set for the file
 *
 */
struct file *shmem_file_setup(char * name, loff_t size)
{
	int error;
	struct file *file;
	struct inode * inode;
	struct dentry *dentry, *root;
	struct qstr this;
	int vm_enough_memory(long pages);

	if (size > (unsigned long long) SHMEM_MAX_BLOCKS << PAGE_CACHE_SHIFT)
		return ERR_PTR(-EINVAL);

	if (!vm_enough_memory((size) >> PAGE_CACHE_SHIFT))
		return ERR_PTR(-ENOMEM);

	this.name = name;
	this.len = strlen(name);
	this.hash = 0; /* will go */
	root = shm_mnt->mnt_root;
	dentry = d_alloc(root, &this);
	if (!dentry)
		return ERR_PTR(-ENOMEM);

	error = -ENFILE;
	file = get_empty_filp();
	if (!file)
		goto put_dentry;

	error = -ENOSPC;
	inode = shmem_get_inode(root->d_sb, S_IFREG | S_IRWXUGO, 0);
	if (!inode) 
		goto close_file;

	d_instantiate(dentry, inode);
	dentry->d_inode->i_size = size;
	shmem_truncate(inode);
	file->f_vfsmnt = mntget(shm_mnt);
	file->f_dentry = dentry;
	file->f_op = &shmem_file_operations;
	file->f_mode = FMODE_WRITE | FMODE_READ;
	inode->i_nlink = 0;	/* It is unlinked */
	return(file);

close_file:
	put_filp(file);
put_dentry:
	dput (dentry);
	return ERR_PTR(error);	
}
/*
 * shmem_zero_setup - setup a shared anonymous mapping
 *
 * @vma: the vma to be mmapped is prepared by do_mmap_pgoff
 */
int shmem_zero_setup(struct vm_area_struct *vma)
{
	struct file *file;
	loff_t size = vma->rde$q_region_size;
	
	file = shmem_file_setup("dev/zero", size);
	if (IS_ERR(file))
		return PTR_ERR(file);

#if 0
	if (vma->vm_file)
		fput (vma->vm_file);
	vma->vm_file = file;
	vma->vm_ops = &shmem_vm_ops;
#endif
	return 0;
}

EXPORT_SYMBOL(shmem_file_setup);
