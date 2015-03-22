#ifndef _ASM_GENERIC_PGTABLE_H
#define _ASM_GENERIC_PGTABLE_H

#ifndef __HAVE_ARCH_PTEP_TEST_AND_CLEAR_YOUNG
static inline int ptep_test_and_clear_young(pte_t *ptep)
{
    pte_t pte = *ptep;
    if (!pte_young(pte))
        return 0;
    set_pte(ptep, pte_mkold(pte));
    return 1;
}
#endif

#ifndef __HAVE_ARCH_PTEP_TEST_AND_CLEAR_DIRTY
static inline int ptep_test_and_clear_dirty(pte_t *ptep)
{
    pte_t pte = *ptep;
    if (!pte_dirty(pte))
        return 0;
    set_pte(ptep, pte_mkclean(pte));
    return 1;
}
#endif

#ifndef __HAVE_ARCH_PTEP_GET_AND_CLEAR
static inline pte_t ptep_get_and_clear(pte_t *ptep)
{
    pte_t pte = *ptep;
    pte_clear(42, 42, ptep); // check
    return pte;
}
#endif

#ifndef __HAVE_ARCH_PTEP_SET_WRPROTECT
static inline void ptep_set_wrprotect(pte_t *ptep)
{
    pte_t old_pte = *ptep;
    set_pte(ptep, pte_wrprotect(old_pte));
}
#endif

#ifndef __ASSEMBLY__
static inline void ptep_mkdirty(pte_t *ptep)
{
    pte_t old_pte = *ptep;
    set_pte(ptep, pte_mkdirty(old_pte));
}
#endif

#ifndef __HAVE_ARCH_PTE_SAME
#define pte_same(A,B)   (pte_val(A) == pte_val(B))
#endif

/*
 * When walking page tables, get the address of the next boundary,
 * or the end address of the range if that comes earlier.  Although no
 * vma end wraps to 0, rounded up __boundary may wrap to 0 throughout.
 */

#define pgd_addr_end(addr, end)                     \
({  unsigned long __boundary = ((addr) + PGDIR_SIZE) & PGDIR_MASK;  \
    (__boundary - 1 < (end) - 1)? __boundary: (end);        \
})

#ifndef pud_addr_end
#define pud_addr_end(addr, end)                     \
({  unsigned long __boundary = ((addr) + PUD_SIZE) & PUD_MASK;  \
    (__boundary - 1 < (end) - 1)? __boundary: (end);        \
})
#endif

#ifndef pmd_addr_end
#define pmd_addr_end(addr, end)                     \
({  unsigned long __boundary = ((addr) + PMD_SIZE) & PMD_MASK;  \
    (__boundary - 1 < (end) - 1)? __boundary: (end);        \
})
#endif

#ifndef __ASSEMBLY__
/*
 * When walking page tables, we usually want to skip any p?d_none entries;
 * and any p?d_bad entries - reporting the error before resetting to none.
 * Do the tests inline, but report and clear the bad entry in mm/memory.c.
 */
void pgd_clear_bad(pgd_t *);
void pud_clear_bad(pud_t *);
void pmd_clear_bad(pmd_t *);

static inline int pgd_none_or_clear_bad(pgd_t *pgd)
{
    if (pgd_none(*pgd))
        return 1;
    if (unlikely(pgd_bad(*pgd)))
    {
        pgd_clear_bad(pgd);
        return 1;
    }
    return 0;
}

static inline int pud_none_or_clear_bad(pud_t *pud)
{
    if (pud_none(*pud))
        return 1;
    if (unlikely(pud_bad(*pud)))
    {
        pud_clear_bad(pud);
        return 1;
    }
    return 0;
}

static inline int pmd_none_or_clear_bad(pmd_t *pmd)
{
    if (pmd_none(*pmd))
        return 1;
    if (unlikely(pmd_bad(*pmd)))
    {
        pmd_clear_bad(pmd);
        return 1;
    }
    return 0;
}
#endif /* !__ASSEMBLY__ */

#endif /* _ASM_GENERIC_PGTABLE_H */
