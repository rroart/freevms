#ifndef __UM_PGTABLE_H
#define __UM_PGTABLE_H

#include "linux/sched.h"
#include "asm/processor.h"
#include "asm/page.h"
#include "asm/fixmap.h"

extern pgd_t swapper_pg_dir[1024];

#define flush_cache_all() do ; while (0)
#define flush_cache_mm(mm) do ; while (0)
#define flush_cache_range(mm, start, end) do ; while (0)
#define flush_cache_page(vma, vmaddr) do ; while (0)
#define flush_page_to_ram(page) do ; while (0)
#define flush_dcache_page(page)	do ; while (0)
#define flush_icache_range(from, to) do ; while (0)
#define flush_icache_page(vma,pg) do ; while (0)

extern void pte_free(pte_t *pte);

extern void pgd_free(pgd_t *pgd);

extern int do_check_pgt_cache(int, int);

/* zero page used for uninitialized stuff */
extern unsigned long *empty_zero_page;

#define pgtable_cache_init() do ; while (0)

/* PMD_SHIFT determines the size of the area a second-level page table can map */
#define PMD_SHIFT	22
#define PMD_SIZE	(1UL << PMD_SHIFT)
#define PMD_MASK	(~(PMD_SIZE-1))

/* PGDIR_SHIFT determines what a third-level page table entry can map */
#define PGDIR_SHIFT	22
#define PGDIR_SIZE	(1UL << PGDIR_SHIFT)
#define PGDIR_MASK	(~(PGDIR_SIZE-1))

/*
 * entries per page directory level: the i386 is two-level, so
 * we don't really have any PMD directory physically.
 */
#define PTRS_PER_PTE	1024
#define PTRS_PER_PMD	1
#define PTRS_PER_PGD	1024
#define USER_PTRS_PER_PGD	(TASK_SIZE/PGDIR_SIZE)
#define FIRST_USER_PGD_NR       0

#define pte_ERROR(e) \
        printk("%s:%d: bad pte %08lx.\n", __FILE__, __LINE__, pte_val(e))
#define pmd_ERROR(e) \
        printk("%s:%d: bad pmd %08lx.\n", __FILE__, __LINE__, pmd_val(e))
#define pgd_ERROR(e) \
        printk("%s:%d: bad pgd %08lx.\n", __FILE__, __LINE__, pgd_val(e))

/*
 * pgd entries used up by user/kernel:
 */

#define USER_PGD_PTRS (TASK_SIZE >> PGDIR_SHIFT)
#define KERNEL_PGD_PTRS (PTRS_PER_PGD-USER_PGD_PTRS)

#ifndef __ASSEMBLY__
/* Just any arbitrary offset to the start of the vmalloc VM area: the
 * current 8MB value just means that there will be a 8MB "hole" after the
 * physical memory until the kernel virtual memory starts.  That means that
 * any out-of-bounds memory accesses will hopefully be caught.
 * The vmalloc() routines leaves a hole of 4kB between each vmalloced
 * area for the same reason. ;)
 */
extern unsigned long high_physmem;

#define VMALLOC_OFFSET	(__va_space)
#define VMALLOC_START	(((unsigned long) high_physmem + VMALLOC_OFFSET) & ~(VMALLOC_OFFSET-1))
#define VMALLOC_VMADDR(x) ((unsigned long)(x))
#define VMALLOC_END	(FIXADDR_START)

/*
 * The 4MB page is guessing..  Detailed in the infamous "Chapter H"
 * of the Pentium details, but assuming intel did the straightforward
 * thing, this bit set in the page directory entry just means that
 * the page directory entry points directly to a 4MB-aligned block of
 * memory. 
 */
#define _PAGE_PRESENT	0x001
#define _PAGE_NEWPAGE	0x002
#define _PAGE_PROTNONE	0x004	/* If not present */
#define _PAGE_RW	0x008
#define _PAGE_USER	0x010
#define _PAGE_PCD       0x020
#define _PAGE_ACCESSED	0x040
#define _PAGE_DIRTY	0x080
#define _PAGE_NEWPROT   0x100


#define _PAGE_TABLE	(_PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_ACCESSED | _PAGE_DIRTY)
#define _KERNPG_TABLE	(_PAGE_PRESENT | _PAGE_RW | _PAGE_ACCESSED | _PAGE_DIRTY)
#define _PAGE_CHG_MASK	(PAGE_MASK | _PAGE_ACCESSED | _PAGE_DIRTY)

#define PAGE_NONE	__pgprot(_PAGE_PROTNONE | _PAGE_ACCESSED)
#define PAGE_SHARED	__pgprot(_PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_ACCESSED)
#define PAGE_COPY	__pgprot(_PAGE_PRESENT | _PAGE_USER | _PAGE_ACCESSED)
#define PAGE_READONLY	__pgprot(_PAGE_PRESENT | _PAGE_USER | _PAGE_ACCESSED)
#define PAGE_KERNEL	__pgprot(_PAGE_PRESENT | _PAGE_RW | _PAGE_DIRTY | _PAGE_ACCESSED)
#define PAGE_KERNEL_RO	__pgprot(_PAGE_PRESENT | _PAGE_DIRTY | _PAGE_ACCESSED)

/*
 * The i386 can't do page protection for execute, and considers that the same are read.
 * Also, write permissions imply read permissions. This is the closest we can get..
 */
#define __P000	PAGE_NONE
#define __P001	PAGE_READONLY
#define __P010	PAGE_COPY
#define __P011	PAGE_COPY
#define __P100	PAGE_READONLY
#define __P101	PAGE_READONLY
#define __P110	PAGE_COPY
#define __P111	PAGE_COPY

#define __S000	PAGE_NONE
#define __S001	PAGE_READONLY
#define __S010	PAGE_SHARED
#define __S011	PAGE_SHARED
#define __S100	PAGE_READONLY
#define __S101	PAGE_READONLY
#define __S110	PAGE_SHARED
#define __S111	PAGE_SHARED

/*
 * Define this if things work differently on an i386 and an i486:
 * it will (on an i486) warn about kernel memory accesses that are
 * done without a 'verify_area(VERIFY_WRITE,..)'
 */
#undef TEST_VERIFY_AREA

/* page table for 0-4MB for everybody */
extern unsigned long pg0[1024];

/*
 * BAD_PAGETABLE is used when we need a bogus page-table, while
 * BAD_PAGE is used for a bogus page.
 *
 * ZERO_PAGE is a global shared page that is always zero: used
 * for zero-mapped memory areas etc..
 */
extern pte_t __bad_page(void);
extern pte_t * __bad_pagetable(void);

#define BAD_PAGETABLE __bad_pagetable()
#define BAD_PAGE __bad_page()
#define ZERO_PAGE(vaddr) (virt_to_page(empty_zero_page))

/* number of bits that fit into a memory pointer */
#define BITS_PER_PTR			(8*sizeof(unsigned long))

/* to align the pointer to a pointer address */
#define PTR_MASK			(~(sizeof(void*)-1))

/* sizeof(void*)==1<<SIZEOF_PTR_LOG2 */
/* 64-bit machines, beware!  SRB. */
#define SIZEOF_PTR_LOG2			2

/* to find an entry in a page-table */
#define PAGE_PTR(address) \
((unsigned long)(address)>>(PAGE_SHIFT-SIZEOF_PTR_LOG2)&PTR_MASK&~PAGE_MASK)

#define pte_none(x)	!(pte_val(x) & ~_PAGE_NEWPAGE)
#define pte_present(x)	(pte_val(x) & (_PAGE_PRESENT | _PAGE_PROTNONE))

#define pte_clear(xp)	do { pte_val(*(xp)) = _PAGE_NEWPAGE; } while (0)

#define pmd_none(x)	(!(pmd_val(x) & ~_PAGE_NEWPAGE))
#define	pmd_bad(x)	((pmd_val(x) & (~PAGE_MASK & ~_PAGE_USER)) != _KERNPG_TABLE)
#define pmd_present(x)	(pmd_val(x) & _PAGE_PRESENT)
#define pmd_clear(xp)	do { pmd_val(*(xp)) = _PAGE_NEWPAGE; } while (0)

#define pmd_newpage(x)  (pmd_val(x) & _PAGE_NEWPAGE)
#define pmd_mkuptodate(x) (pmd_val(x) &= ~_PAGE_NEWPAGE)

/*
 * The "pgd_xxx()" functions here are trivial for a folded two-level
 * setup: the pgd is never bad, and a pmd always exists (as it's folded
 * into the pgd entry)
 */
static inline int pgd_none(pgd_t pgd)		{ return 0; }
static inline int pgd_bad(pgd_t pgd)		{ return 0; }
static inline int pgd_present(pgd_t pgd)	{ return 1; }
static inline void pgd_clear(pgd_t * pgdp)	{ }


/*
 * Permanent address of a page. Obviously must never be
 * called on a highmem page.
 */
#define __page_address(page) ({ PAGE_OFFSET + (((page) - mem_map) << PAGE_SHIFT); })
#define page_address(page) __page_address(page)
#define pages_to_mb(x) ((x) >> (20-PAGE_SHIFT))
#define pte_page(x) \
    (mem_map+((unsigned long)((__pa(pte_val(x)) >> PAGE_SHIFT))))
#define pte_address(x) ((void *) ((unsigned long) pte_val(x) & PAGE_MASK))

static inline pte_t pte_mknewprot(pte_t pte)
{
 	pte_val(pte) |= _PAGE_NEWPROT;
	return(pte);
}

static inline pte_t pte_mknewpage(pte_t pte)
{
	pte_val(pte) |= _PAGE_NEWPAGE;
	return(pte);
}

static inline void set_pte(pte_t *pteptr, pte_t pteval)
{
	/* If it's a swap entry, it needs to be marked _PAGE_NEWPAGE so
	 * fix_range knows to unmap it.  _PAGE_NEWPROT is specific to
	 * mapped pages.
	 */
	*pteptr = pte_mknewpage(pteval);
	if(pte_present(*pteptr)) *pteptr = pte_mknewprot(*pteptr);
}

/*
 * (pmds are folded into pgds so this doesnt get actually called,
 * but the define is needed for a generic inline function.)
 */
#define set_pmd(pmdptr, pmdval) (*(pmdptr) = pmdval)
#define set_pgd(pgdptr, pgdval) (*(pgdptr) = pgdval)

/*
 * The following only work if pte_present() is true.
 * Undefined behaviour if not..
 */
static inline int pte_read(pte_t pte)	{ return pte_val(pte) & _PAGE_USER; }
static inline int pte_exec(pte_t pte)	{ return pte_val(pte) & _PAGE_USER; }
static inline int pte_dirty(pte_t pte)	{ return pte_val(pte) & _PAGE_DIRTY; }
static inline int pte_young(pte_t pte)	{ return pte_val(pte) & _PAGE_ACCESSED; }
static inline int pte_write(pte_t pte)	{ return pte_val(pte) & _PAGE_RW; }
static inline int pte_newpage(pte_t pte) { return pte_val(pte) & _PAGE_NEWPAGE; }
static inline int pte_newprot(pte_t pte) { return pte_val(pte) & _PAGE_NEWPROT; }

static inline pte_t pte_rdprotect(pte_t pte)
{ 
	pte_val(pte) &= ~_PAGE_USER; 
	return(pte_mknewprot(pte));
}

static inline pte_t pte_exprotect(pte_t pte)
{ 
	pte_val(pte) &= ~_PAGE_USER;
	return(pte_mknewprot(pte));
}

static inline pte_t pte_mkclean(pte_t pte)
{
	pte_val(pte) &= ~_PAGE_DIRTY; 
	return(pte);
}

static inline pte_t pte_mkold(pte_t pte)	
{ 
	pte_val(pte) &= ~_PAGE_ACCESSED; 
	return(pte);
}

static inline pte_t pte_wrprotect(pte_t pte)
{ 
	pte_val(pte) &= ~_PAGE_RW; 
	return(pte_mknewprot(pte)); 
}

static inline pte_t pte_mkread(pte_t pte)
{ 
	pte_val(pte) |= _PAGE_USER; 
	return(pte_mknewprot(pte)); 
}

static inline pte_t pte_mkexec(pte_t pte)
{ 
	pte_val(pte) |= _PAGE_USER; 
	return(pte_mknewprot(pte)); 
}

static inline pte_t pte_mkdirty(pte_t pte)
{ 
	pte_val(pte) |= _PAGE_DIRTY; 
	return(pte);
}

static inline pte_t pte_mkyoung(pte_t pte)
{
	pte_val(pte) |= _PAGE_ACCESSED; 
	return(pte);
}

static inline pte_t pte_mkwrite(pte_t pte)	
{
	pte_val(pte) |= _PAGE_RW; 
	return(pte_mknewprot(pte)); 
}

static inline pte_t pte_mkuptodate(pte_t pte)	
{
	pte_val(pte) &= ~(_PAGE_NEWPROT | _PAGE_NEWPAGE);
	return(pte); 
}

/*
 * Conversion functions: convert a page and protection to a page entry,
 * and a page entry and page directory to the page they refer to.
 */

#define mk_pte(page, pgprot) \
({					\
	pte_t __pte;                    \
                                        \
	pte_val(__pte) = ((unsigned long) __va((page-mem_map)*(unsigned long)PAGE_SIZE + pgprot_val(pgprot)));         \
	if(pte_present(__pte)) pte_mknewprot(pte_mknewpage(__pte)); \
	__pte;                          \
})

/* This takes a physical page address that is used by the remapping functions */
#define mk_pte_phys(physpage, pgprot) \
({ pte_t __pte; pte_val(__pte) = physpage + pgprot_val(pgprot); __pte; })

static inline pte_t pte_modify(pte_t pte, pgprot_t newprot)
{
	pte_val(pte) = (pte_val(pte) & _PAGE_CHG_MASK) | pgprot_val(newprot);
	if(pte_present(pte)) pte = pte_mknewpage(pte_mknewprot(pte));
	return pte; 
}

#define pmd_page(pmd) \
(pmd_val(pmd) & PAGE_MASK)

/* to find an entry in a page-table-directory. */
#define pgd_index(address) ((address >> PGDIR_SHIFT) & (PTRS_PER_PGD-1))

/* to find an entry in a page-table-directory */
#define pgd_offset(mm, address) \
((mm)->pgd + ((address) >> PGDIR_SHIFT))

/* to find an entry in a kernel page-table-directory */
#define pgd_offset_k(address) pgd_offset(&init_mm, address)

/* Find an entry in the second-level page table.. */
static inline pmd_t * pmd_offset(pgd_t * dir, unsigned long address)
{
	return (pmd_t *) dir;
}

/* Find an entry in the third-level page table.. */ 
#define pte_offset(pmd, address) \
((pte_t *) (pmd_page(*pmd) + ((address>>10) & ((PTRS_PER_PTE-1)<<2))))

#define update_mmu_cache(vma,address,pte) do ; while (0)

/* Encode and de-code a swap entry */
#define SWP_TYPE(x)			(((x).val >> 3) & 0x7f)
#define SWP_OFFSET(x)			((x).val >> 10)

#define SWP_ENTRY(type, offset) \
	((swp_entry_t) { ((type) << 3) | ((offset) << 10) })
#define pte_to_swp_entry(pte) \
	((swp_entry_t) { pte_val(pte_mkuptodate(pte)) })
#define swp_entry_to_pte(x)		((pte_t) { (x).val })

#define PageSkip(x) (0)
#define kern_addr_valid(addr) (1)

#include <asm-generic/pgtable.h>

#endif

#endif
/*
 * Overrides for Emacs so that we follow Linus's tabbing style.
 * Emacs will notice this stuff at the end of the file and automatically
 * adjust the settings for this buffer only.  This must remain at the end
 * of the file.
 * ---------------------------------------------------------------------------
 * Local variables:
 * c-file-style: "linux"
 * End:
 */
