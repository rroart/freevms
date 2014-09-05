// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004. Based on swapfile.c.

/*
 *  linux/mm/swapfile.c
 *
 *  Copyright (C) 1991, 1992, 1993, 1994  Linus Torvalds
 *  Swap reorganised 29.12.95, Stephen Tweedie
 */

#include <linux/config.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/kernel_stat.h>
#include <linux/blkdev.h> /* for blk_size */
#include <linux/vmalloc.h>
#include <linux/pagemap.h>
#include <linux/shm.h>
#include <linux/compiler.h>

#include <asm/pgtable.h>

#include <dyndef.h>
#include <pfldef.h>
#include <system_data_cells.h>

spinlock_t swaplock = SPIN_LOCK_UNLOCKED;
//unsigned int mmg$gl_maxpfidx;
int total_swap_pages;
static int swap_overflow;

static const char Bad_file[] = "Bad swap file entry ";
static const char Unused_file[] = "Unused swap file entry ";
static const char Bad_offset[] = "Bad swap offset entry ";
static const char Unused_offset[] = "Unused swap offset entry ";

#define MAX_SWAPFILES 32
struct _pfl swap_info_pfl[MAX_SWAPFILES];

#define SWAPFILE_CLUSTER 256
#undef SWAPFILE_CLUSTER
#define SWAPFILE_CLUSTER 1

/*
 * Check if we're the only user of a swap page,
 * when the page is locked.
 */
static int exclusive_swap_page(struct page *page)
{
  return  0;
#if 0
	int retval = 0;
	struct _pfl * p;
	swp_entry_t entry;

	entry.val = page->index;
	p = swap_info_get(entry);
	if (p) {
		/* Is the only swap cache user the cache itself? */
		if (p->pfl$l_bitmap[SWP_OFFSET(entry)] == 1) {
			/* Recheck the page count with the pagecache lock held.. */
			if (page_count(page) - !!page->buffers == 2)
				retval = 1;
		}
		swap_info_put(p);
	}
	return retval;
#endif
}

/*
 * We can use this swap cache entry directly
 * if there are no other references to it.
 *
 * Here "exclusive_swap_page()" does the real
 * work, but we opportunistically check whether
 * we need to get all the locks first..
 */
int can_share_swap_page(struct page *page)
{
	int retval = 0;

#if 0
	if (!PageLocked(page))
		BUG();
#endif
	switch (page_count(page)) {
	case 3:
#if 0
		if (!page->buffers)
			break;
#endif
		/* Fallthrough */
	case 2:
#if 0
		if (!PageSwapCache(page))
			break;
#endif
		retval = exclusive_swap_page(page);
		break;
	case 1:
		if (PageReserved(page))
			break;
		retval = 1;
	}
	return retval;
}

asmlinkage long sys_swapoff(const char * specialfile)
{
}

int get_swaparea_info(char *buf)
{
}

/*
 * Written 01/25/92 by Simmule Turner, heavily changed by Linus.
 *
 * The swapon system call
 */
asmlinkage long sys_swapon(const char * specialfile, int swap_flags)
{
  // use stuff from kswapd for this

}

void si_swapinfo(struct sysinfo *val)
{
}

struct address_space swapper_space;

void swap_setup(void){
  printk("swap setup does nothing\n");
}

int page_cluster;


