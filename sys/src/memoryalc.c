#include <linux/config.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/swapctl.h>
#include <linux/interrupt.h>
#include <linux/pagemap.h>
#include <linux/bootmem.h>
#include <linux/slab.h>
#include <linux/compiler.h>

#include "../../freevms/starlet/src/ssdef.h"
#include "../../freevms/sys/src/system_data_cells.h"

struct mymap {
  unsigned long flink;
  unsigned long size;
};

int exe$alophycntg(unsigned long * va, unsigned long num) {
  signed long firstpfn;

  firstpfn=mmg$allocontig(num);
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

static char *zone_names[MAX_NR_ZONES] = { "DMA", "Normal", "HighMem" };
static int zone_balance_ratio[MAX_NR_ZONES] __initdata = { 128, 128, 128, };
static int zone_balance_min[MAX_NR_ZONES] __initdata = { 20 , 20, 20, };
static int zone_balance_max[MAX_NR_ZONES] __initdata = { 255 , 255, 255, };

#define memlist_init(x) INIT_LIST_HEAD(x)
#define memlist_add_head list_add
#define memlist_add_tail list_add_tail
#define memlist_del list_del
#define memlist_entry list_entry
#define memlist_next(x) ((x)->next)
#define memlist_prev(x) ((x)->prev)

#define BAD_RANGE(zone,x) (((zone) != (x)->zone) || (((x)-mem_map) < (zone)->zone_start_mapnr) || (((x)-mem_map) >= (zone)->zone_start_mapnr+(zone)->size))

extern int in_free_all_bootmem_core;

static void FASTCALL(__free_pages_ok (struct page *page, unsigned int order));
static void __free_pages_ok (struct page *page, unsigned int order)
{
	unsigned long index, page_idx, mask, flags;
	free_area_t *area;
	struct page *base;
	zone_t *zone;
	unsigned long i,tmp;

	zone = page->zone;

	/* Yes, think what happens when other parts of the kernel take 
	 * a reference to a page in order to pin it for io. -ben
	 */
	if (PageLRU(page))
		lru_cache_del(page);

	if (page->buffers)
		BUG();
	if (page->mapping)
		BUG();
	if (!VALID_PAGE(page))
		BUG();
	if (PageSwapCache(page))
		BUG();
	if (PageLocked(page))
		BUG();
	if (PageLRU(page))
		BUG();
	if (PageActive(page))
		BUG();
	page->flags &= ~((1<<PG_referenced) | (1<<PG_dirty));

	spin_lock_irqsave(&zone->lock, flags);
	if (!in_free_all_bootmem_core)
	  for(i=0,tmp=((page-mem_map)/sizeof(struct _pfn));i<(1 << order);i++,tmp++)
	    mmg$dallocpfn(tmp);

	spin_unlock_irqrestore(&zone->lock, flags);

	page->index = order;
	current->nr_local_pages++;
}

#define MARK_USED(index, order, area) \
	__change_bit((index) >> (1+(order)), (area)->map)

#ifndef CONFIG_DISCONTIGMEM
struct page *_alloc_pages(unsigned int gfp_mask, unsigned int order)
{
	return __alloc_pages(gfp_mask, order,
		contig_page_data.node_zonelists+(gfp_mask & GFP_ZONEMASK));
}
#endif

/*
 * This is the 'heart' of the zoned buddy allocator:
 */
struct page * __alloc_pages(unsigned int gfp_mask, unsigned int order, zonelist_t *zonelist)
{
        unsigned long flags;
	unsigned long min;
	zone_t *zone, * classzone;
	struct page * page;
	int freed;
	signed long pfn;

	zone = zonelist->zones;

	spin_lock_irqsave(&zone->lock, flags);
	if (order)
	  pfn=mmg$allocontig(1 << order);
	else
	  pfn=mmg$allocpfn();
	spin_unlock_irqrestore(&zone->lock, flags);

	if (pfn>=0) {
	  page=&mem_map[pfn];
	  return page;
	}

	classzone->need_balance = 1;
	mb();
	if (waitqueue_active(&kswapd_wait))
		wake_up_interruptible(&kswapd_wait);

rebalance:
	try_to_free_pages(classzone, gfp_mask, order);

	spin_lock_irqsave(zone->lock, flags);
	if (order)
	  pfn=mmg$allocontig(1 << order);
	else
	  pfn=mmg$allocpfn();
	spin_unlock_irqrestore(zone->lock, flags);

	if (pfn>=0) {
	  page=&mem_map[pfn];
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

	sum = 0;
	while (pgdat) {
		for (zone = pgdat->node_zones; zone < pgdat->node_zones + MAX_NR_ZONES; zone++)
			sum += zone->free_pages;
		pgdat = pgdat->node_next;
	}
	return sum;
}

/*
 * Amount of free RAM allocatable as buffer memory:
 */
unsigned int nr_free_buffer_pages (void)
{
	pg_data_t *pgdat = pgdat_list;
	unsigned int sum = 0;

	do {
		zonelist_t *zonelist = pgdat->node_zonelists + (GFP_USER & GFP_ZONEMASK);
		zone_t **zonep = zonelist->zones;
		zone_t *zone;

		for (zone = *zonep++; zone; zone = *zonep++) {
			unsigned long size = zone->size;
			unsigned long high = zone->pages_high;
			if (size > high)
				sum += size - high;
		}

		pgdat = pgdat->node_next;
	} while (pgdat);

	return sum;
}

#if CONFIG_HIGHMEM
unsigned int nr_free_highpages (void)
{
	pg_data_t *pgdat = pgdat_list;
	unsigned int pages = 0;

	while (pgdat) {
		pages += pgdat->node_zones[ZONE_HIGHMEM].free_pages;
		pgdat = pgdat->node_next;
	}
	return pages;
}
#endif

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
	if (zholes_size)
		for (i = 0; i < MAX_NR_ZONES; i++)
			realtotalpages -= zholes_size[i];
			
	printk("On node %d totalpages: %lu\n", nid, realtotalpages);

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
		memlist_init(&p->list);
	}

	offset = lmem_map - mem_map;	
	for (j = 0; j < MAX_NR_ZONES; j++) {
		zone_t *zone = pgdat->node_zones + j;
		unsigned long mask;
		unsigned long size, realsize;

		realsize = size = zones_size[j];
		if (zholes_size)
			realsize -= zholes_size[j];

		printk("zone(%lu): %lu pages.\n", j, size);
		zone->size = size;
		zone->name = zone_names[j];
		zone->lock = SPIN_LOCK_UNLOCKED;
		zone->zone_pgdat = pgdat;
		zone->free_pages = 0;
		zone->need_balance = 0;
		if (!size)
			continue;

		pgdat->nr_zones = j+1;

		mask = (realsize / zone_balance_ratio[j]);
		if (mask < zone_balance_min[j])
			mask = zone_balance_min[j];
		else if (mask > zone_balance_max[j])
			mask = zone_balance_max[j];
		zone->pages_min = mask;
		zone->pages_low = mask*2;
		zone->pages_high = mask*3;

		zone->zone_mem_map = mem_map + offset;
		zone->zone_start_mapnr = offset;
		zone->zone_start_paddr = zone_start_paddr;

		if ((zone_start_paddr >> PAGE_SHIFT) & (zone_required_alignment-1))
			printk("BUG: wrong zone alignment, it will crash\n");

		for (i = 0; i < size; i++) {
			struct page *page = mem_map + offset + i;
			page->zone = zone;
			if (j != ZONE_HIGHMEM)
				page->virtual = __va(zone_start_paddr);
			zone_start_paddr += PAGE_SIZE;
		}

		offset += size;
		for (i = 0; ; i++) {
			unsigned long bitmap_size;

			memlist_init(&zone->free_area[i].free_list);
			if (i == MAX_ORDER-1) {
				zone->free_area[i].map = NULL;
				break;
			}

			/*
			 * Page buddy system uses "index >> (i+1)",
			 * where "index" is at most "size-1".
			 *
			 * The extra "+3" is to round down to byte
			 * size (8 bits per byte assumption). Thus
			 * we get "(size-1) >> (i+4)" as the last byte
			 * we can access.
			 *
			 * The "+1" is because we want to round the
			 * byte allocation up rather than down. So
			 * we should have had a "+7" before we shifted
			 * down by three. Also, we have to add one as
			 * we actually _use_ the last bit (it's [0,n]
			 * inclusive, not [0,n[).
			 *
			 * So we actually had +7+1 before we shift
			 * down by 3. But (n+8) >> 3 == (n >> 3) + 1
			 * (modulo overflows, which we do not have).
			 *
			 * Finally, we LONG_ALIGN because all bitmap
			 * operations are on longs.
			 */
			bitmap_size = (size-1) >> (i+4);
			bitmap_size = LONG_ALIGN(bitmap_size+1);
			zone->free_area[i].map = 
			  (unsigned long *) alloc_bootmem_node(pgdat, bitmap_size);
		}
	}
}

void __init free_area_init(unsigned long *zones_size)
{
	free_area_init_core(0, &contig_page_data, &mem_map, zones_size, 0, 0, 0);
}

static int __init setup_mem_frac(char *str)
{
	int j = 0;

	while (get_option(&str, &zone_balance_ratio[j++]) == 2);
	printk("setup_mem_frac: ");
	for (j = 0; j < MAX_NR_ZONES; j++) printk("%d  ", zone_balance_ratio[j]);
	printk("\n");
	return 1;
}

__setup("memfrac=", setup_mem_frac);
