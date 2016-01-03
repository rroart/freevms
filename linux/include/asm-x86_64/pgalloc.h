#ifndef _X86_64_PGALLOC_H
#define _X86_64_PGALLOC_H

#include <linux/config.h>
#include <asm/processor.h>
#include <asm/fixmap.h>
#include <asm/pda.h>
#include <linux/threads.h>
#include <linux/mm.h>
#include <asm/page.h>

#define inc_pgcache_size() add_pda(pgtable_cache_sz,1UL)
#define dec_pgcache_size() sub_pda(pgtable_cache_sz,1UL)

#define pmd_populate_kernel(mm, pmd, pte) \
        set_pmd(pmd, __pmd(_PAGE_TABLE | __pa(pte)))
#define pud_populate(mm, pud, pmd) \
        set_pud(pud, __pud(_PAGE_TABLE | __pa(pmd)))
#define pmd_populate(mm, pmd, pte) \
        set_pmd(pmd, __pmd(_PAGE_TABLE | __pa(pte)))
#define pgd_populate(mm, pgd, pud) \
        set_pgd(pgd, __pgd(_PAGE_TABLE | __pa(pud)))

extern __inline__ void pmd_free(pmd_t *pmd)
{
    *(unsigned long *)pmd = (unsigned long) read_pda(pmd_quick);
    write_pda(pmd_quick,(unsigned long *) pmd);
    inc_pgcache_size();
}

extern __inline__ void pmd_free_slow(pmd_t *pmd)
{
    if ((unsigned long)pmd & (PAGE_SIZE-1))
        out_of_line_bug();
    free_page((unsigned long)pmd);
}

static inline pmd_t *pmd_alloc_one_fast (struct mm_struct *mm, unsigned long addr)
{
    unsigned long *ret = (unsigned long *)read_pda(pmd_quick);

    if (ret != NULL)
    {
        write_pda(pmd_quick, (unsigned long *)(*ret));
        ret[0] = 0;
        dec_pgcache_size();
    }
    return (pmd_t *)ret;
}

static inline pmd_t *pmd_alloc_one (struct mm_struct *mm, unsigned long addr)
{
    return (pmd_t *)get_zeroed_page(GFP_KERNEL);
}

static inline pud_t *pud_alloc_one_fast (struct mm_struct *mm, unsigned long addr)
{
    unsigned long *ret = (unsigned long *)read_pda(pud_quick);

    if (ret != NULL)
    {
        write_pda(pud_quick, (unsigned long *)(*ret));
        ret[0] = 0;
        dec_pgcache_size();
    }
    return (pud_t *)ret;
}

static inline pud_t *pud_alloc_one (struct mm_struct *mm, unsigned long addr)
{
    return (pud_t *)get_zeroed_page(GFP_KERNEL);
}

static inline void pud_free (pud_t *pud)
{
    BUG_ON((unsigned long)pud & (PAGE_SIZE-1));
    free_page((unsigned long)pud);
}

static inline pgd_t *pgd_alloc_one_fast (void)
{
    unsigned long *ret = read_pda(pgd_quick);

    if (ret)
    {
        write_pda(pgd_quick,(unsigned long *)(*ret));
        ret[0] = 0;
        dec_pgcache_size();
    }
    return (pgd_t *) ret;
}

#if 0
static inline pgd_t *pgd_alloc (struct mm_struct *mm)
{
    /* the VM system never calls pgd_alloc_one_fast(), so we do it here. */
    pgd_t *pgd = pgd_alloc_one_fast();

    if (pgd == NULL)
        pgd = (pgd_t *)get_zeroed_page(GFP_KERNEL);
    return pgd;
}
#else
static inline pgd_t *pgd_alloc(struct mm_struct *mm)
{
    unsigned boundary;
    pgd_t *pgd = (pgd_t *)__get_free_page(GFP_KERNEL/*|__GFP_REPEAT*/);
    if (!pgd)
        return NULL;
    /*
     * Copy kernel pointers in from init.
     * Could keep a freelist or slab cache of those because the kernel
     * part never changes.
     */
    boundary = pgd_index(__PAGE_OFFSET);
    memset(pgd, 0, boundary * sizeof(pgd_t));
    memcpy(pgd + boundary,
           init_level4_pgt + boundary,
           (PTRS_PER_PGD - boundary) * sizeof(pgd_t));
    return pgd;
}
#endif

#if 0
static inline void pgd_free (pgd_t *pgd)
{
    *(unsigned long *)pgd = (unsigned long) read_pda(pgd_quick);
    write_pda(pgd_quick,(unsigned long *) pgd);
    inc_pgcache_size();
}
#else
static inline void pgd_free (pgd_t *pgd)
{
#if 0
    BUG_ON((unsigned long)pgd & (PAGE_SIZE-1));
#endif
    free_page((unsigned long)pgd);
}
#endif

static inline void pgd_free_slow (pgd_t *pgd)
{
    if ((unsigned long)pgd & (PAGE_SIZE-1))
        out_of_line_bug();
    free_page((unsigned long)pgd);
}

#if 0
static inline pte_t *pte_alloc_one(struct mm_struct *mm, unsigned long address)
{
    return (pte_t *)get_zeroed_page(GFP_KERNEL);
}
#endif

extern __inline__ pte_t *pte_alloc_one_fast(struct mm_struct *mm, unsigned long address)
{
    unsigned long *ret;

    if ((ret = read_pda(pte_quick)) != NULL)
    {
        write_pda(pte_quick, (unsigned long *)(*ret));
        ret[0] = ret[1];
        dec_pgcache_size();
    }
    return (pte_t *)ret;
}

/* Should really implement gc for free page table pages. This could be done with
   a reference count in struct page. */

#if 0
extern __inline__ void pte_free(pte_t *pte)
{
    *(unsigned long *)pte = (unsigned long) read_pda(pte_quick);
    write_pda(pte_quick, (unsigned long *) pte);
    inc_pgcache_size();
}
#endif

extern __inline__ void pte_free_slow(pte_t *pte)
{
    if ((unsigned long)pte & (PAGE_SIZE-1))
        out_of_line_bug();
    free_page((unsigned long)pte);
}


extern int do_check_pgt_cache(int, int);

/*
 * TLB flushing:
 *
 *  - flush_tlb() flushes the current mm struct TLBs
 *  - flush_tlb_all() flushes all processes TLBs
 *  - flush_tlb_mm(mm) flushes the specified mm context TLB's
 *  - flush_tlb_page(vma, vmaddr) flushes one page
 *  - flush_tlb_range(mm, start, end) flushes a range of pages
 *  - flush_tlb_pgtables(mm, start, end) flushes a range of page tables
 */

#ifndef CONFIG_SMP

#define flush_tlb() __flush_tlb()
#define flush_tlb_all() __flush_tlb_all()
#define local_flush_tlb() __flush_tlb()

static inline void flush_tlb_mm(struct mm_struct *mm)
{
    if (mm == current->active_mm)
        __flush_tlb();
}

static inline void flush_tlb_page2(struct mm_struct *mm,
                                   unsigned long addr)
{
    if (mm == current->active_mm)
        __flush_tlb_one(addr);
}

static inline void flush_tlb_range(struct mm_struct *mm,
                                   unsigned long start, unsigned long end)
{
    if (mm == current->active_mm)
        __flush_tlb();
}

#else

#include <asm/smp.h>

#define local_flush_tlb() \
    __flush_tlb()

extern void flush_tlb_all(void);
extern void flush_tlb_current_task(void);
extern void flush_tlb_mm(struct mm_struct *);
extern void flush_tlb_page(struct vm_area_struct *, unsigned long);
extern void flush_tlb_page2(struct mm_struct *, unsigned long);

#define flush_tlb() flush_tlb_current_task()

static inline void flush_tlb_range(struct mm_struct * mm, unsigned long start, unsigned long end)
{
    flush_tlb_mm(mm);
}

#define TLBSTATE_OK 1
#define TLBSTATE_LAZY   2

struct tlb_state
{
    struct mm_struct *active_mm;
    int state;
} ____cacheline_aligned;
extern struct tlb_state cpu_tlbstate[NR_CPUS];


#endif

extern inline void flush_tlb_pgtables(struct mm_struct *mm,
                                      unsigned long start, unsigned long end)
{
    flush_tlb_mm(mm);
}

#if 1
#define __GFP_REPEAT 0
#endif

static inline pte_t *pte_alloc_one_kernel(struct mm_struct *mm, unsigned long address)
{
    return (pte_t *)get_zeroed_page(GFP_KERNEL|__GFP_REPEAT);
}

static inline struct page *pte_alloc_one(struct mm_struct *mm, unsigned long address)
{
    void *p = (void *)get_zeroed_page(GFP_KERNEL|__GFP_REPEAT);
    if (!p)
        return NULL;
    return p;
    return virt_to_page(p);
}

/* Should really implement gc for free page table pages. This could be
   done with a reference count in struct page. */

static inline void pte_free_kernel(pte_t *pte)
{
    BUG_ON((unsigned long)pte & (PAGE_SIZE-1));
    free_page((unsigned long)pte);
}

static inline void pte_free(struct page *pte)
{
    __free_page(pte);
}

#define __pte_free_tlb(tlb,pte) tlb_remove_page((tlb),(pte))

#define __pmd_free_tlb(tlb,x)   tlb_remove_page((tlb),virt_to_page(x))
#define __pud_free_tlb(tlb,x)   tlb_remove_page((tlb),virt_to_page(x))

#endif /* _X86_64_PGALLOC_H */
