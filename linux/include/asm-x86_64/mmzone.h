/*
 * Written by Kanoj Sarcar (kanoj@sgi.com) Aug 99
 * Adapted for K8/x86-64 Jul 2002 by Andi Kleen.
 */
#ifndef _ASM_MMZONE_H_
#define _ASM_MMZONE_H_

#include <linux/config.h>

typedef struct plat_pglist_data
{
    pg_data_t   gendata;
    unsigned long   start_pfn, end_pfn;
} plat_pg_data_t;

struct bootmem_data_t;

/*
 * Following are macros that are specific to this numa platform.
 *
 * XXX check what the compiler generates for all this
 */

extern plat_pg_data_t *plat_node_data[];

#define MAXNODE 8
#define MAX_NUMNODES MAXNODE
#define NODEMAPSIZE 0xff

/* Simple perfect hash to map physical addresses to node numbers */
extern int memnode_shift;
extern u8  memnodemap[NODEMAPSIZE];
extern int maxnode;

#if 0
#define VIRTUAL_BUG_ON(x) do { if (x) out_of_line_bug(); } while(0)
#else
#define VIRTUAL_BUG_ON(x) do {} while (0)
#endif

/* VALID_PAGE below hardcodes the same algorithm*/
static inline int phys_to_nid(unsigned long addr)
{
    int nid;
    VIRTUAL_BUG_ON((addr >> memnode_shift) >= NODEMAPSIZE);
    nid = memnodemap[addr >> memnode_shift];
    VIRTUAL_BUG_ON(nid > maxnode);
    return nid;
}

#define PLAT_NODE_DATA(n)       (plat_node_data[(n)])
#define PLAT_NODE_DATA_STARTNR(n)   \
    (PLAT_NODE_DATA(n)->gendata.node_start_mapnr)
#define PLAT_NODE_DATA_SIZE(n)      (PLAT_NODE_DATA(n)->gendata.node_size)

#define PLAT_NODE_DATA_LOCALNR(p, n)    \
    (((p) - PLAT_NODE_DATA(n)->gendata.node_start_paddr) >> PAGE_SHIFT)

#endif /* _ASM_MMZONE_H_ */
