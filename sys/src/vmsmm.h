// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004. Based on mm.h.

#ifndef _LINUX_MM_H
#define _LINUX_MM_H

#include <linux/sched.h>
#include <linux/errno.h>

#ifdef __KERNEL__

#include <linux/config.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/mmzone.h>
#include <linux/rbtree.h>

extern unsigned long max_mapnr;
extern unsigned long num_physpages;
extern unsigned long num_mappedpages;
extern void * high_memory;
extern int page_cluster;
/* The inactive_clean lists are per zone. */
extern struct list_head active_list;
extern struct list_head inactive_list;

#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/atomic.h>

#include <pfndef.h>
#include <rdedef.h>

/*
 * Linux kernel virtual memory manager primitives.
 * The idea being to have a "virtual" mm in the same way
 * we have a virtual fs - giving a cleaner interface to the
 * mm details, and allowing different kinds of memory mappings
 * (from shared memory to executable loading to arbitrary
 * mmap() functions).
 */

#define vm_area_struct _rde

// vm_flags will be 8 bits left to coexist with rde stuff

/*
 * vm_flags..
 */
#define VM_READ     0x00000100  /* currently active flags */
#define VM_WRITE    0x00000200
#define VM_EXEC     0x00000400
#define VM_SHARED   0x00000800

#define VM_MAYREAD  0x00001000  /* limits for mprotect() etc */
#define VM_MAYWRITE 0x00002000
#define VM_MAYEXEC  0x00004000
#define VM_MAYSHARE 0x00008000

#define VM_GROWSDOWN    0x00010000  /* general info on the segment */
#define VM_GROWSUP  0x00020000
#define VM_SHM      0x00000000  /* shared memory area, don't swap out */
#define VM_PFNMAP   0x00040000  /* Page-ranges managed without "struct page", just pure PFN */
#define VM_DENYWRITE    0x00080000  /* ETXTBSY on write attempts.. */

#define VM_EXECUTABLE   0x00100000
#define VM_LOCKED   0x00200000
#define VM_IO           0x00400000  /* Memory mapped I/O or similar */

/* Used by sys_madvise() */
#define VM_SEQ_READ 0x00800000  /* App will access data sequentially */
#define VM_RAND_READ    0x01000000  /* App will not benefit from clustered reads */

#define VM_DONTCOPY 0x02000000      /* Do not copy this vma on fork */
#define VM_DONTEXPAND   0x04000000  /* Cannot expand with mremap() */
#define VM_RESERVED 0x08000000  /* Don't unmap it from swap_out */

#define VM_STACK_FLAGS  0x00017700

#define VM_READHINTMASK         (VM_SEQ_READ | VM_RAND_READ)
#define VM_ClearReadHint(v)     (v)->rde$l_flags &= ~VM_READHINTMASK
#define VM_NormalReadHint(v)        (!((v)->rde$l_flags & VM_READHINTMASK))
#define VM_SequentialReadHint(v)    ((v)->rde$l_flags & VM_SEQ_READ)
#define VM_RandomReadHint(v)        ((v)->rde$l_flags & VM_RAND_READ)

/* read ahead limits */
extern int vm_min_readahead;
extern int vm_max_readahead;

/*
 * mapping from the currently active rde$l_flags protection bits (the
 * low four bits) to a page protection mask..
 */
extern pgprot_t protection_map[16];


/*
 * These are the virtual MM functions - opening of an area, closing and
 * unmapping it (needed to keep files on disk up-to-date etc), pointer
 * to the functions called when a no-page or a wp-page exception occurs.
 */
struct vm_operations_struct
{
    void (*open)(struct _rde * area);
    void (*close)(struct _rde * area);
    struct page * (*nopage)(struct _rde * area, unsigned long address, int unused);
};

/*
 * Methods to modify the page usage count.
 *
 * What counts for a page usage:
 * - cache mapping   (page->mapping)
 * - disk mapping    (page->buffers)
 * - page mapped in a task's page tables, each mapping
 *   is counted separately
 *
 * Also, many kernel routines increase the page count before a critical
 * routine so they can be sure the page doesn't go away from under them.
 */
#define get_page(p)     atomic_inc(&(p)->count)
#define put_page(p)     __free_page(p)
#define put_page_testzero(p)    atomic_dec_and_test(&(p)->count)
#define page_count(p)       atomic_read(&(p)->count)
#define set_page_count(p,v)     atomic_set(&(p)->count, v)

/*
 * Various page->flags bits:
 *
 * PG_reserved is set for special pages, which can never be swapped
 * out. Some of them might not even exist (eg empty_bad_page)...
 *
 * Multiple processes may "see" the same page. E.g. for untouched
 * mappings of /dev/null, all processes see the same page full of
 * zeroes, and text pages of executables and shared libraries have
 * only one copy in memory, at most, normally.
 *
 * For the non-reserved pages, page->count denotes a reference count.
 *   page->count == 0 means the page is free.
 *   page->count == 1 means the page is used for exactly one purpose
 *   (e.g. a private data page of one process).
 *
 * A page may be used for kmalloc() or anyone else who does a
 * __get_free_page(). In this case the page->count is at least 1, and
 * all other fields are unused but should be 0 or NULL. The
 * management of this page is the responsibility of the one who uses
 * it.
 *
 * The other pages (we may call them "process pages") are completely
 * managed by the Linux memory manager: I/O, buffers, swapping etc.
 * The following discussion applies only to them.
 *
 * A page may belong to an inode's memory mapping. In this case,
 * page->mapping is the pointer to the inode, and page->index is the
 * file offset of the page, in units of PAGE_CACHE_SIZE.
 *
 * A page may have buffers allocated to it. In this case,
 * page->buffers is a circular list of these buffer heads. Else,
 * page->buffers == NULL.
 *
 * For pages belonging to inodes, the page->count is the number of
 * attaches, plus 1 if buffers are allocated to the page, plus one
 * for the page cache itself.
 *
 * All pages belonging to an inode are in these doubly linked lists:
 * mapping->clean_pages, mapping->dirty_pages and mapping->locked_pages;
 * using the page->list list_head. These fields are also used for
 * freelist managemet (when page->count==0).
 *
 * There is also a hash table mapping (mapping,index) to the page
 * in memory if present. The lists for this hash table use the fields
 * page->next_hash and page->pprev_hash.
 *
 * All process pages can do I/O:
 * - inode pages may need to be read from disk,
 * - inode pages which have been modified and are MAP_SHARED may need
 *   to be written to disk,
 * - private pages which have been modified may need to be swapped out
 *   to swap space and (later) to be read back into memory.
 * During disk I/O, PG_locked is used. This bit is set before I/O
 * and reset when I/O completes. page->wait is a wait queue of all
 * tasks waiting for the I/O on this page to complete.
 * PG_uptodate tells whether the page's contents is valid.
 * When a read completes, the page becomes uptodate, unless a disk I/O
 * error happened.
 *
 * For choosing which pages to swap out, inode pages carry a
 * PG_referenced bit, which is set any time the system accesses
 * that page through the (mapping,index) hash table. This referenced
 * bit, together with the referenced bit in the page tables, is used
 * to manipulate page->age and move the page across the active,
 * inactive_dirty and inactive_clean lists.
 *
 * Note that the referenced bit, the page->lru list_head and the
 * active, inactive_dirty and inactive_clean lists are protected by
 * the pagemap_lru_lock, and *NOT* by the usual PG_locked bit!
 *
 * PG_skip is used on sparc/sparc64 architectures to "skip" certain
 * parts of the address space.
 *
 * PG_error is set to indicate that an I/O error occurred on this page.
 *
 * PG_arch_1 is an architecture specific page state bit.  The generic
 * code guarantees that this bit is cleared for a page when it first
 * is entered into the page cache.
 *
 * PG_highmem pages are not permanently mapped into the kernel virtual
 * address space, they need to be kmapped separately for doing IO on
 * the pages. The struct page (these bits with information) are always
 * mapped into kernel address space...
 */
#if 0
#define PG_locked        3  /* Page is locked. Don't touch. */
#define PG_error        20
#define PG_referenced       21
#define PG_uptodate     22
#define PG_dirty        13
#define PG_unused       23
#define PG_lru          24
#define PG_active       25
#endif
#define PG_slab         26
#if 0
#define PG_skip         27
//#define PG_highmem        11
#define PG_checked      28  /* kill me in 2.5.<early>. */
#define PG_arch_1       29
#endif
#define PG_reserved     30
#if 0
#define PG_launder      31  /* written out by VM pressure.. */

/* Make it prettier to test the above... */
#define UnlockPage(page)    unlock_page(page)
#define Page_Uptodate(page) test_bit(PG_uptodate, &(page)->pfn$l_page_state)
#define SetPageUptodate(page)   set_bit(PG_uptodate, &(page)->pfn$l_page_state)
#define ClearPageUptodate(page) clear_bit(PG_uptodate, &(page)->pfn$l_page_state)
#define PageDirty(page)     test_bit(PG_dirty, &(page)->pfn$l_page_state)
#define SetPageDirty(page)  set_bit(PG_dirty, &(page)->pfn$l_page_state)
#define ClearPageDirty(page)    clear_bit(PG_dirty, &(page)->pfn$l_page_state)
#define PageLocked(page)    test_bit(PG_locked, &(page)->pfn$l_page_state)
#define LockPage(page)      set_bit(PG_locked, &(page)->pfn$l_page_state)
#define TryLockPage(page)   test_and_set_bit(PG_locked, &(page)->pfn$l_page_state)
#define PageChecked(page)   test_bit(PG_checked, &(page)->pfn$l_page_state)
#define SetPageChecked(page)    set_bit(PG_checked, &(page)->pfn$l_page_state)
#define PageLaunder(page)   test_bit(PG_launder, &(page)->pfn$l_page_state)
#define SetPageLaunder(page)    set_bit(PG_launder, &(page)->pfn$l_page_state)

extern void FASTCALL(set_page_dirty(struct page *));

/*
 * The first mb is necessary to safely close the critical section opened by the
 * TryLockPage(), the second mb is necessary to enforce ordering between
 * the clear_bit and the read of the waitqueue (to avoid SMP races with a
 * parallel wait_on_page).
 */
#define PageError(page)     test_bit(PG_error, &(page)->pfn$l_page_state)
#define SetPageError(page)  set_bit(PG_error, &(page)->pfn$l_page_state)
#define ClearPageError(page)    clear_bit(PG_error, &(page)->pfn$l_page_state)
#define PageReferenced(page)    test_bit(PG_referenced, &(page)->pfn$l_page_state)
#define SetPageReferenced(page) set_bit(PG_referenced, &(page)->pfn$l_page_state)
#define ClearPageReferenced(page)   clear_bit(PG_referenced, &(page)->pfn$l_page_state)
#define PageTestandClearReferenced(page)    test_and_clear_bit(PG_referenced, &(page)->pfn$l_page_state)
#else
#define PageLocked(x) (0)
#endif
#define PageSlab(page)      test_bit(PG_slab, &(page)->pfn$l_page_state)
#define PageSetSlab(page)   set_bit(PG_slab, &(page)->pfn$l_page_state)
#define PageClearSlab(page) clear_bit(PG_slab, &(page)->pfn$l_page_state)
#define PageReserved(page)  test_bit(PG_reserved, &(page)->pfn$l_page_state)
#if 0

#define PageActive(page)    test_bit(PG_active, &(page)->pfn$l_page_state)
#define SetPageActive(page) set_bit(PG_active, &(page)->pfn$l_page_state)
#define ClearPageActive(page)   clear_bit(PG_active, &(page)->pfn$l_page_state)

#define PageLRU(page)       test_bit(PG_lru, &(page)->pfn$l_page_state)
#define TestSetPageLRU(page)    test_and_set_bit(PG_lru, &(page)->pfn$l_page_state)
#define TestClearPageLRU(page)  test_and_clear_bit(PG_lru, &(page)->pfn$l_page_state)

#ifdef CONFIG_HIGHMEM
#define PageHighMem(page)       test_bit(PG_highmem, &(page)->pfn$l_page_state)
#else
#define PageHighMem(page)       0 /* needed to optimize away at compile time */
#endif

#endif
#define SetPageReserved(page)       set_bit(PG_reserved, &(page)->pfn$l_page_state)
#define ClearPageReserved(page)     clear_bit(PG_reserved, &(page)->pfn$l_page_state)

/*
 * Error return values for the *_nopage functions
 */
#define NOPAGE_SIGBUS   (NULL)
#define NOPAGE_OOM  ((struct page *) (-1))

/* The array of struct pages */
extern mem_map_t * mem_map;

#define __page_address(page) ({ PAGE_OFFSET + (((page) - mem_map) << PAGE_SHIFT); })
#define page_address(page) __page_address(page)

/*
 * There is only one page-allocator function, and two main namespaces to
 * it. The alloc_page*() variants return 'struct page *' and as such
 * can allocate highmem pages, the *get*page*() variants return
 * virtual kernel addresses to the allocated page(s).
 */
extern struct page * FASTCALL(_alloc_pages(unsigned int gfp_mask, unsigned int order));
extern struct page * FASTCALL(__alloc_pages(unsigned int gfp_mask, unsigned int order, zonelist_t *zonelist));
extern struct page * alloc_pages_node(int nid, unsigned int gfp_mask, unsigned int order);

static inline struct page * alloc_pages(unsigned int gfp_mask, unsigned int order)
{
    /*
     * Gets optimized away by the compiler.
     */
    if (order >= MAX_ORDER)
        return NULL;
    return _alloc_pages(gfp_mask, order);
}

#define alloc_page(gfp_mask) alloc_pages(gfp_mask, 0)

extern unsigned long FASTCALL(__get_free_pages(unsigned int gfp_mask, unsigned int order));
extern unsigned long FASTCALL(get_zeroed_page(unsigned int gfp_mask));

#define __get_free_page(gfp_mask) \
        __get_free_pages((gfp_mask),0)

#define __get_dma_pages(gfp_mask, order) \
        __get_free_pages((gfp_mask) | GFP_DMA,(order))

/*
 * The old interface name will be removed in 2.5:
 */
#define get_free_page get_zeroed_page

/*
 * There is only one 'core' page-freeing function.
 */
extern void FASTCALL(__free_pages(struct page *page, unsigned int order));
extern void FASTCALL(free_pages(unsigned long addr, unsigned int order));

#define __free_page(page) __free_pages((page), 0)
#define free_page(addr) free_pages((addr),0)

extern void show_free_areas(void);
extern void show_free_areas_node(pg_data_t *pgdat);

extern void clear_page_tables(struct mm_struct *, unsigned long, int);

extern int fail_writepage(struct page *);
struct page * shmem_nopage(struct _rde * vma, unsigned long address, int unused);
struct file *shmem_file_setup(char * name, loff_t size);
extern void shmem_lock(struct file * file, int lock);
extern int shmem_zero_setup(struct _rde *);

extern void zap_page_range(struct mm_struct *mm, unsigned long address, unsigned long size);
extern int copy_page_range(struct mm_struct *dst, struct mm_struct *src, struct _rde *vma);
extern int remap_page_range(unsigned long from, unsigned long to, unsigned long size, pgprot_t prot);
extern int zeromap_page_range(unsigned long from, unsigned long size, pgprot_t prot);

extern int vmtruncate(struct _fcb * inode, loff_t offset);
extern pud_t *FASTCALL(__pud_alloc(struct mm_struct *mm, pgd_t *pgd, unsigned long address));
extern pmd_t *FASTCALL(__pmd_alloc(struct mm_struct *mm, pud_t *pud, unsigned long address));
extern pte_t *FASTCALL(pte_alloc(struct mm_struct *mm, pmd_t *pmd, unsigned long address));
extern int handle_mm_fault(struct mm_struct *mm,struct _rde *vma, unsigned long address, int write_access);
extern int make_pages_present(unsigned long addr, unsigned long end);
extern int access_process_vm(struct task_struct *tsk, unsigned long addr, void *buf, int len, int write);
extern int ptrace_readdata(struct task_struct *tsk, unsigned long src, char *dst, int len);
extern int ptrace_writedata(struct task_struct *tsk, char * src, unsigned long dst, int len);
extern int ptrace_attach(struct task_struct *tsk);
extern int ptrace_detach(struct task_struct *, unsigned int);
extern void ptrace_disable(struct task_struct *);
extern int ptrace_check_attach(struct task_struct *task, int kill);

int get_user_pages(struct task_struct *tsk, struct mm_struct *mm, unsigned long start,
                   int len, int write, int force, struct page **pages, struct _rde **vmas);
void print_bad_pte(struct vm_area_struct *, pte_t, unsigned long);

/*
 * On a two-level page table, this ends up being trivial. Thus the
 * inlining and the symmetry break with pte_alloc() that does all
 * of this out-of-line.
 */
static inline pmd_t *pmd_alloc(struct mm_struct *mm, pud_t *pud, unsigned long address)
{
    if (pud_none(*pud))
        return __pmd_alloc(mm, pud, address);
    return pmd_offset(pud, address);
}

/*
 * On a two-level page table, this ends up being trivial. Thus the
 * inlining and the symmetry break with pte_alloc() that does all
 * of this out-of-line.
 */
static inline pmd_t *pud_alloc(struct mm_struct *mm, pgd_t *pgd, unsigned long address)
{
    if (pgd_none(*pgd))
        return __pud_alloc(mm, pgd, address);
    return pud_offset(pgd, address);
}

extern int pgt_cache_water[2];
extern int check_pgt_cache(void);

/*
 * We use mm->page_table_lock to guard all pagetable pages of the mm.
 */
#define pte_lock_init(page) do {} while (0)
#define pte_lock_deinit(page)   do {} while (0)
#define pte_lockptr(mm, pmd)    ({(void)(pmd); &(mm)->page_table_lock;})

#define pte_offset_map_lock(mm, pmd, address, ptlp) \
({                          \
    spinlock_t *__ptl = pte_lockptr(mm, pmd);   \
    pte_t *__pte = pte_offset_map(pmd, address);    \
    *(ptlp) = __ptl;                \
    spin_lock(__ptl);               \
    __pte;                      \
})

#define pte_unmap_unlock(pte, ptl)  do {        \
    spin_unlock(ptl);               \
    pte_unmap(pte);                 \
} while (0)

#define pte_alloc_map(mm, pmd, address)         \
    ((unlikely(!pmd_present(*(pmd))) && __pte_alloc(mm, pmd, address))? \
        NULL: pte_offset_map(pmd, address))

#define pte_alloc_map_lock(mm, pmd, address, ptlp)  \
    ((unlikely(!pmd_present(*(pmd))) && __pte_alloc(mm, pmd, address))? \
        NULL: pte_offset_map_lock(mm, pmd, address, ptlp))

#define pte_alloc_kernel(pmd, address)          \
    ((unlikely(!pmd_present(*(pmd))) && __pte_alloc_kernel(pmd, address))? \
        NULL: pte_offset_kernel(pmd, address))

extern void free_area_init(unsigned long * zones_size);
extern void free_area_init_node(int nid, pg_data_t *pgdat, struct page *pmap,
                                unsigned long * zones_size, unsigned long zone_start_paddr,
                                unsigned long *zholes_size);
extern void mem_init(void);
extern void show_mem(void);
extern void si_meminfo(struct sysinfo * val);
extern void swapin_readahead(swp_entry_t);

extern struct address_space swapper_space;
#define PageSwapCache(page) ((page)->mapping == &swapper_space)

static inline int is_page_cache_freeable(struct page * page)
{
    return page_count(page) /*- !!page->buffers*/ == 1;
}

extern int can_share_swap_page(struct page *);
extern int remove_exclusive_swap_page(struct page *);

extern void __free_pte(pte_t);

/* mmap.c */
extern void lock_vma_mappings(struct _rde *);
extern void unlock_vma_mappings(struct _rde *);
extern void insert_vm_struct(struct mm_struct *, struct _rde *);
extern void __insert_vm_struct(struct mm_struct *, struct _rde *);
extern void build_mmap_rb(struct mm_struct *);
extern void exit_mmap(struct mm_struct *);

extern unsigned long get_unmapped_area(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);

extern unsigned long do_mmap_pgoff(struct file *file, unsigned long addr,
                                   unsigned long len, unsigned long prot,
                                   unsigned long flag, unsigned long pgoff);

static inline unsigned long do_mmap(struct file *file, unsigned long addr,
                                    unsigned long len, unsigned long prot,
                                    unsigned long flag, unsigned long offset)
{
    unsigned long ret = -EINVAL;
    if ((offset + PAGE_ALIGN(len)) < offset)
        goto out;
    if (!(offset & ~PAGE_MASK))
        ret = do_mmap_pgoff(file, addr, len, prot, flag, offset >> PAGE_SHIFT);
out:
    return ret;
}

extern int do_munmap(struct mm_struct *, unsigned long, size_t);

extern unsigned long do_brk(unsigned long, unsigned long);

#if 0
static inline void __vma_unlink(struct mm_struct * mm, struct _rde * vma, struct _rde * prev)
{
    prev->vm_next = vma->vm_next;
    rb_erase(&vma->vm_rb, &mm->mm_rb);
    if (mm->mmap_cache == vma)
        mm->mmap_cache = prev;
}
#endif

static inline int can_vma_merge(struct _rde * vma, unsigned long rde$l_flags)
{
#if 0
    if (!vma->vm_file && vma->rde$l_flags == rde$l_flags)
        return 1;
    else
        return 0;
#endif
    return 0;
}

struct zone_t;
/* filemap.c */
extern void remove_inode_page(struct page *);
extern unsigned long page_unuse(struct page *);
extern void truncate_inode_pages(struct address_space *, loff_t);

/* generic vm_area_ops exported for stackable file systems */
extern int filemap_sync(struct _rde *, unsigned long,   size_t, unsigned int);
extern struct page *filemap_nopage(struct _rde *, unsigned long, int);

/*
 * GFP bitmasks..
 */
/* Zone modifiers in GFP_ZONEMASK (see linux/mmzone.h - low four bits) */
#define __GFP_DMA   0x01
#define __GFP_HIGHMEM   0x02

/* Action modifiers - doesn't change the zoning */
#define __GFP_WAIT  0x10    /* Can wait and reschedule? */
#define __GFP_HIGH  0x20    /* Should access emergency pools? */
#define __GFP_IO    0x40    /* Can start low memory physical IO? */
#define __GFP_HIGHIO    0x80    /* Can start high mem physical IO? */
#define __GFP_FS    0x100   /* Can call down to low-level FS? */

#define GFP_NOHIGHIO    (__GFP_HIGH | __GFP_WAIT | __GFP_IO)
#define GFP_NOIO    (__GFP_HIGH | __GFP_WAIT)
#define GFP_NOFS    (__GFP_HIGH | __GFP_WAIT | __GFP_IO | __GFP_HIGHIO)
#define GFP_ATOMIC  (__GFP_HIGH)
#define GFP_USER    (             __GFP_WAIT | __GFP_IO | __GFP_HIGHIO | __GFP_FS)
#define GFP_HIGHUSER    (             __GFP_WAIT | __GFP_IO | __GFP_HIGHIO | __GFP_FS | __GFP_HIGHMEM)
#define GFP_KERNEL  (__GFP_HIGH | __GFP_WAIT | __GFP_IO | __GFP_HIGHIO | __GFP_FS)
#define GFP_NFS     (__GFP_HIGH | __GFP_WAIT | __GFP_IO | __GFP_HIGHIO | __GFP_FS)
#define GFP_KSWAPD  (             __GFP_WAIT | __GFP_IO | __GFP_HIGHIO | __GFP_FS)

/* Flag - indicates that the buffer will be suitable for DMA.  Ignored on some
   platforms, used as appropriate on others */

#define GFP_DMA     __GFP_DMA

static inline unsigned int pf_gfp_mask(unsigned int gfp_mask)
{
    /* avoid all memory balancing I/O methods if this task cannot block on I/O */
    if (current->flags & PF_NOIO)
        gfp_mask &= ~(__GFP_IO | __GFP_HIGHIO | __GFP_FS);

    return gfp_mask;
}

/* vma is the first one with  address < vma->vm_end,
 * and even  address < vma->rde$pq_start_va. Have to extend vma. */
static inline int expand_stack(struct _rde * vma, unsigned long address)
{
    unsigned long grow;

    /*
     * vma->rde$pq_start_va/vm_end cannot change under us because the caller is required
     * to hold the mmap_sem in write mode. We need to get the spinlock only
     * before relocating the vma range ourself.
     */
    address &= PAGE_MASK;
    //spin_lock(&vma->vm_mm->page_table_lock);
    grow = (unsigned long)(vma->rde$pq_start_va - address) >> PAGE_SHIFT;
    if ((vma->rde$pq_start_va + vma->rde$q_region_size) - address > current->rlim[RLIMIT_STACK].rlim_cur /* ||
                                                        ((vma->vm_mm->total_vm + grow) << PAGE_SHIFT) > current->rlim[RLIMIT_AS].rlim_cur*/)
    {
        //spin_unlock(&vma->vm_mm->page_table_lock);
        return -ENOMEM;
    }
    vma->rde$pq_start_va = address;
    //vma->vm_pgoff -= grow;
    //vma->vm_mm->total_vm += grow;
    //if (vma->rde$l_flags & VM_LOCKED)
    //vma->vm_mm->locked_vm += grow;
    //spin_unlock(&vma->vm_mm->page_table_lock);
    return 0;
}

/* Look up the first VMA which satisfies  addr < vm_end,  NULL if none. */
extern struct _rde * find_vma(struct mm_struct * mm, unsigned long addr);
extern struct _rde * find_vma_prev(struct mm_struct * mm, unsigned long addr,
                                   struct _rde **pprev);

/* Look up the first VMA which intersects the interval start_addr..end_addr-1,
   NULL if none.  Assume start_addr < end_addr. */
static inline struct _rde * find_vma_intersection(struct mm_struct * mm, unsigned long start_addr, unsigned long end_addr)
{
    struct _rde * vma = find_vma(mm,start_addr);

    if (vma && end_addr <= vma->rde$pq_start_va)
        vma = NULL;
    return vma;
}

extern struct _rde *find_extend_vma(struct mm_struct *mm, unsigned long addr);

#endif /* __KERNEL__ */

#endif
