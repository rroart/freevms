// $Id$
// $Locker$

// Author. Roar Thron�s.

// everything is untested

#include <linux/linkage.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <internals.h>
#include <ipldef.h>
#include <rdedef.h>
#include <ssdef.h>

int mmg$pteref_64 (void * va, struct _rde *rde, void ** va_pte)
{
    if (va < rde->rde$ps_start_va || va >= (rde->rde$ps_start_va + rde->rde$l_region_size))
        return SS$_LENVIO;
    struct mm_struct *mm = ctl$gl_pcb->mm;
    long page, address;
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    address = va;
    page = address & PAGE_MASK;
    pgd = pgd_offset(mm, page);
    pud = pud_offset(pgd, page);
    pmd = pmd_offset(pud, page);
    pte = pte_offset(pmd, page);

    int ipl = vmslock(&SPIN_MMG, IPL$_MMG);

    *va_pte = pte;

    return SS$_NORMAL; // with mmg spinlock still held
}

