/*
 * Written by Kanoj Sarcar, SGI, Aug 1999
 */

#include <linux/mm.h>
#include <linux/init.h>
#include <linux/bootmem.h>
#include <linux/mmzone.h>
#include <linux/spinlock.h>

int numnodes = 1;   /* Initialized for UMA platforms */

static bootmem_data_t contig_bootmem_data;
pg_data_t contig_page_data = { bdata:
                               &contig_bootmem_data
                             };

/*
 * This is meant to be invoked by platforms whose physical memory starts
 * at a considerably higher value than 0. Examples are Super-H, ARM, m68k.
 * Should be invoked with paramters (0, 0, unsigned long *[], start_paddr).
 */
void __init free_area_init_node(int nid, pg_data_t *pgdat, struct page *pmap,
                                unsigned long *zones_size, unsigned long zone_start_paddr,
                                unsigned long *zholes_size)
{
    free_area_init_core(0, &contig_page_data, &mem_map, zones_size,
                        zone_start_paddr, zholes_size, pmap);
}

struct page * alloc_pages_node(int nid, unsigned int gfp_mask, unsigned int order)
{
    return alloc_pages(gfp_mask, order);
}
