// $Id$
// $Locker$

// Author. Roar Thron�s.
// Author. Linux people.

#include <linux/config.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/swapctl.h>
#include <linux/interrupt.h>
#include <linux/pagemap.h>
#include <linux/bootmem.h>
#include <linux/slab.h>
#include <linux/compiler.h>

#include <ssdef.h>
#include <system_data_cells.h>

struct mymap {
  unsigned long flink;
  unsigned long size;
};

zone_t thezone = {
  lock: SPIN_LOCK_UNLOCKED
};

int exe$alophycntg(unsigned long * va, unsigned long num) {
  signed long firstpfn;

  firstpfn=mmg$allocontig_align(num);
  if (firstpfn<0) {
    return SS$_INSFMEM;
  }

  
}

int nr_swap_pages;
int nr_active_pages;
int nr_inactive_pages;
struct list_head inactive_list;
struct list_head active_list;
pg_data_t *pgdat_list;

#define memlist_init(x) INIT_LIST_HEAD(x)
#define memlist_add_head list_add
#define memlist_add_tail list_add_tail
#define memlist_del list_del
#define memlist_entry list_entry
#define memlist_next(x) ((x)->next)
#define memlist_prev(x) ((x)->prev)

extern int in_free_all_bootmem_core;

int memalcdeb=0;

static void FASTCALL(__free_pages_ok (struct page *page, unsigned int order));
static void __free_pages_ok (struct page *page, unsigned int order)
{
	unsigned long index, page_idx, mask, flags;
	free_area_t *area;
	struct page *base;
	zone_t *zone;
	unsigned long i,tmp;

	zone = &thezone;

	/* Yes, think what happens when other parts of the kernel take 
	 * a reference to a page in order to pin it for io. -ben
	 */
#if 0
	if (PageLRU(page))
		lru_cache_del(page);
#endif

#if 0
	if (page->buffers)
		BUG();
	if (page->mapping)
		BUG();
#endif
	if (!VALID_PAGE(page))
		BUG();
#if 0
	if (PageSwapCache(page))
		BUG();
	if (PageLocked(page))
		BUG();
#endif
	if (PageLRU(page))
		BUG();
	if (PageActive(page))
		BUG();
	page->pfn$l_page_state &= ~((1<<PG_referenced) | (1<<PG_dirty));

	mask = (~0UL) << order;
	page_idx = page - mem_map;
	base = mem_map;

	spin_lock_irqsave(&zone->lock, flags);

#if 0
	while (mask + (1 << (MAX_ORDER-1))) {
	  struct page *buddy1;
	  if (memalcdeb) printk("memlistdel at %x %x %x\n",page_idx,mask,(page_idx ^ -mask));
	  buddy1 = base + (page_idx ^ -mask);
	  memlist_del(&buddy1->list);
	  mask <<= 1;
	  page_idx &= mask;
	}
	if (memalcdeb) printk("\n");
#endif

	if (!in_free_all_bootmem_core)
	  for(i=0;i<(1 << order);i++,page++) {
	    mmg$dallocpfn(page);
	    //memlist_del(&page->list);
	  }

	spin_unlock_irqrestore(&zone->lock, flags);

}

#ifndef CONFIG_DISCONTIGMEM
struct page *_alloc_pages(unsigned int gfp_mask, unsigned int order)
{
	return __alloc_pages(gfp_mask, order,
		contig_page_data.node_zonelists+(gfp_mask & GFP_ZONEMASK));
}
#endif

int inallocpfn=0;

/*
 * This is the 'heart' of the zoned buddy allocator:
 */
struct page * __alloc_pages(unsigned int gfp_mask, unsigned int order, zonelist_t *zonelist)
{
        unsigned long flags;
	unsigned long min;
	zone_t *zone, * classzone;
	struct page * page, *tmp;
	int freed;
	signed long pfn;
	unsigned long i;

	zone = &thezone;

	spin_lock_irqsave(&zone->lock, flags);
	if (inallocpfn++) panic("mooo\n");
	if (order)
	  pfn=mmg$allocontig_align(1 << order);
	else
	  pfn=mmg$allocpfn();
	inallocpfn--;
	spin_unlock_irqrestore(&zone->lock, flags);

	//	printk("allocated pfn %x %x\n",pfn,1<<order);

	if (pfn>=0) {
	  page=&mem_map[pfn];
	  for(i=0,tmp=page;i<(1<<order);i++,tmp++)
	    set_page_count(tmp, 1);
	  return page;
	}

	printk("should not be here now\n");

	mb();
	if (waitqueue_active(&kswapd_wait))
		wake_up_interruptible(&kswapd_wait);

rebalance:
	try_to_free_pages(classzone, gfp_mask, order);

	spin_lock_irqsave(&zone->lock, flags);
	if (inallocpfn++) panic("mooo\n");
	if (order)
	  pfn=mmg$allocontig(1 << order);
	else
	  pfn=mmg$allocpfn();
	inallocpfn--;
	spin_unlock_irqrestore(&zone->lock, flags);

	if (pfn>=0) {
	  page=&mem_map[pfn];
	  for(i=0,tmp=page;i<(1<<order);i++,tmp++)
	    set_page_count(tmp, 1);
	  return page;
	}

	current->need_resched=1;
	__set_current_state(TASK_RUNNING);
	schedule();
	goto rebalance;
}

/*
 * Common helper functions.
 */
unsigned long __get_free_pages(unsigned int gfp_mask, unsigned int order)
{
	struct page * page;

	page = alloc_pages(gfp_mask, order);
	if (!page)
		return 0;
	return (unsigned long) page_address(page);
}

unsigned long get_zeroed_page(unsigned int gfp_mask)
{
	struct page * page;

	page = alloc_pages(gfp_mask, 0);
	if (page) {
		void *address = page_address(page);
		clear_page(address);
		return (unsigned long) address;
	}
	return 0;
}

void __free_pages(struct page *page, unsigned int order)
{
	if (!PageReserved(page) && put_page_testzero(page))
		__free_pages_ok(page, order);
}

void free_pages(unsigned long addr, unsigned int order)
{
	if (addr != 0)
		__free_pages(virt_to_page(addr), order);
}

/*
 * Total amount of free (allocatable) RAM:
 */
unsigned int nr_free_pages (void)
{
	unsigned int sum;
	zone_t *zone;
	pg_data_t *pgdat = pgdat_list;

	sum = sch$gl_freecnt;
	return sum;
}

/*
 * Amount of free RAM allocatable as buffer memory:
 */
unsigned int nr_free_buffer_pages (void)
{
	pg_data_t *pgdat = pgdat_list;
	unsigned int sum = 0;

	return sum;
}

#define K(x) ((x) << (PAGE_SHIFT-10))

/*
 * Show free area list (used inside shift_scroll-lock stuff)
 * We also calculate the percentage fragmentation. We do this by counting the
 * memory on each free list with the exception of the first item on the list.
 */
void show_free_areas_core(pg_data_t *pgdat)
{
 	unsigned int order;
	unsigned type;
	pg_data_t *tmpdat = pgdat;

	printk("Free pages:      %6dkB (%6dkB HighMem)\n",
		K(nr_free_pages()),
		K(nr_free_highpages()));

	while (tmpdat) {
		zone_t *zone;
		for (zone = tmpdat->node_zones;
			       	zone < tmpdat->node_zones + MAX_NR_ZONES; zone++)
			printk("Zone:%s freepages:%6lukB min:%6lukB low:%6lukB " 
				       "high:%6lukB\n", 
					zone->name,
					K(zone->free_pages),
					K(zone->pages_min),
					K(zone->pages_low),
					K(zone->pages_high));
			
		tmpdat = tmpdat->node_next;
	}

	printk("( Active: %d, inactive: %d, free: %d )\n",
	       nr_active_pages,
	       nr_inactive_pages,
	       nr_free_pages());

	for (type = 0; type < MAX_NR_ZONES; type++) {
		struct list_head *head, *curr;
		zone_t *zone = pgdat->node_zones + type;
 		unsigned long nr, total, flags;

		total = 0;
		if (zone->size) {
			spin_lock_irqsave(&zone->lock, flags);
		 	for (order = 0; order < MAX_ORDER; order++) {
				head = &(zone->free_area + order)->free_list;
				curr = head;
				nr = 0;
				for (;;) {
					curr = memlist_next(curr);
					if (curr == head)
						break;
					nr++;
				}
				total += nr * (1 << order);
				printk("%lu*%lukB ", nr, K(1UL) << order);
			}
			spin_unlock_irqrestore(&zone->lock, flags);
		}
		printk("= %lukB)\n", K(total));
	}

#ifdef SWAP_CACHE_INFO
	show_swap_cache_info();
#endif	
}

void show_free_areas(void)
{
	show_free_areas_core(pgdat_list);
}

#define LONG_ALIGN(x) (((x)+(sizeof(long))-1)&~((sizeof(long))-1))

/*
 * Set up the zone data structures:
 *   - mark all pages reserved
 *   - mark all memory queues empty
 *   - clear the memory bitmaps
 */
void __init free_area_init_core(int nid, pg_data_t *pgdat, struct page **gmap,
	unsigned long *zones_size, unsigned long zone_start_paddr, 
	unsigned long *zholes_size, struct page *lmem_map)
{
	struct page *p;
	unsigned long i, j;
	unsigned long map_size;
	unsigned long totalpages, offset, realtotalpages;
	const unsigned long zone_required_alignment = 1UL << (MAX_ORDER-1);
	struct mymap * mytmp;

	if (zone_start_paddr & ~PAGE_MASK)
		BUG();

	totalpages = 0;
	for (i = 0; i < MAX_NR_ZONES; i++) {
		unsigned long size = zones_size[i];
		totalpages += size;
	}
	realtotalpages = totalpages;
	INIT_LIST_HEAD(&active_list);
	INIT_LIST_HEAD(&inactive_list);

	/*
	 * Some architectures (with lots of mem and discontinous memory
	 * maps) have to search for a good mem_map area:
	 * For discontigmem, the conceptual mem map array starts from 
	 * PAGE_OFFSET, we need to align the actual array onto a mem map 
	 * boundary, so that MAP_NR works.
	 */
	map_size = (totalpages + 1)*sizeof(struct page);
	if (lmem_map == (struct page *)0) {
		lmem_map = (struct page *) alloc_bootmem_node(pgdat, map_size);
		lmem_map = (struct page *)(PAGE_OFFSET + 
			MAP_ALIGN((unsigned long)lmem_map - PAGE_OFFSET));
	}
	*gmap = pgdat->node_mem_map = lmem_map;
	pgdat->node_size = totalpages;
	pgdat->node_start_paddr = zone_start_paddr;
	pgdat->node_start_mapnr = (lmem_map - mem_map);
	pgdat->nr_zones = 0;

	mmg$gl_npagedyn=alloc_bootmem_node(pgdat,sgn$gl_npagedyn);
	mmg$gl_npagedyn = (struct page *)(PAGE_OFFSET + 
	  MAP_ALIGN((unsigned long)mmg$gl_npagedyn - PAGE_OFFSET));
	exe$gl_npagedyn=mmg$gl_npagedyn;
	mytmp=exe$gl_npagedyn;
	mytmp->flink=0; /* ? */
	mytmp->size=sgn$gl_npagedyn;

	/*
	 * Initially all pages are reserved - free ones are freed
	 * up by free_all_bootmem() once the early boot process is
	 * done.
	 */
	for (p = lmem_map; p < lmem_map + totalpages; p++) {
		set_page_count(p, 0);
		SetPageReserved(p);
		init_waitqueue_head(&p->wait);
		//memlist_init(&p->list);
	}

	offset = lmem_map - mem_map;	
	for (i = 0; i < totalpages; i++) {
	  struct page *page = mem_map + offset + i;
	  page->virtual = __va(zone_start_paddr);
	  zone_start_paddr += PAGE_SIZE;
	}
}

void __init free_area_init(unsigned long *zones_size)
{
	free_area_init_core(0, &contig_page_data, &mem_map, zones_size, 0, 0, 0);
}

static int __init setup_mem_frac(char *str)
{
	int j = 0;

	printk("setup_mem_frac not done\n");
	return 1;
}

__setup("memfrac=", setup_mem_frac);
