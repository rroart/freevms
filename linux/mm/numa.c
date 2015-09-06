/*
 * Written by Kanoj Sarcar, SGI, Aug 1999
 */

#include <linux/mm.h>
#include <linux/bootmem.h>

static bootmem_data_t contig_bootmem_data;
pg_data_t contig_page_data = { bdata:
                               &contig_bootmem_data
                             };
