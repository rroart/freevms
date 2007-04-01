// $Id$
// $Locker$

// Author. Roar Thronæs.

#if 0
#include <stdio.h> 
#endif
#include <ssdef.h> 
#include <descrip.h> 
#include <starlet.h>
#include <misc.h>

#define __KERNEL__
#include <linux/config.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/posix_types.h>
#include <linux/slab.h>
#include <asm/pgalloc.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <system_data_cells.h>
#undef __KERNEL__

int show$address(int mask) {
#ifdef __i386__
  int retlen;
  $DESCRIPTOR(p, "p1");
  char c[80];
  struct dsc$descriptor o;
  o.dsc$a_pointer=c;
  o.dsc$w_length=80;
  memset (c, 0, 80);
  int sts = cli$present(&p);
  if (sts&1) {
    sts = cli$get_value(&p, &o, &retlen);
    o.dsc$w_length=retlen;
  }

  long addr = strtol (c, 0, 16);
  long isnot_addr = addr;
  if (addr == 0) 
    sts = sda_find_addr (c, &addr);
  if ((sts&1)==0)
    return sts;
  long size = sizeof(long);
  struct _pcb * pcb;
  struct mm_struct mm;
  long page = addr & PAGE_MASK;
  pgd_t *pgd = 0, *pgd_k;
  pud_t *pud = 0, *pud_k;
  pmd_t *pmd = 0, *pmd_k;
  pte_t *pte = 0, *pte_k;
  long phys = 0;
  sda$getmemlong(ctl$gl_pcb, &pcb);
  sda$getmem(pcb->mm, &mm, sizeof (mm));
  pgd_k = pgd_offset (&mm, page);
  sda$getmemlong(pgd_k, &pgd);
  if (pgd_none_or_clear_bad(pgd))
    goto out;
  pud_k = pud_offset(pgd, page);
  sda$getmemlong(pud_k, &pud);
  if (pud_none_or_clear_bad(pud))
    goto out;
  pmd_k = pmd_offset(pud, page);
  sda$getmemlong(pmd_k, &pmd);
  if (pmd_none_or_clear_bad(pmd))
    goto out;
  pte_k = pte_offset(pmd, page);
  sda$getmemlong(pte_k, &pte);
  if (pte_none/*_or_clear_bad*/(*pte))
    goto out;
  phys = *(long *)pte & PAGE_MASK;
 out:
  printf("\t%lx is an xx address\n\n");
  printf("\tMapped by Level-4 PTE at: %lx : %lx\n", pte_k, *pte);
  printf("\tMapped by Level-3 PTE at: %lx : %lx\n", pmd_k, *pmd);
  printf("\tMapped by Level-2 PTE at: %lx : %lx\n", pud_k, *pud);
  printf("\tMapped by Level-1 PTE at: %lx : %lx\n", pgd_k, *pgd);
  printf("\tMapped by Level-0 PGD at: %lx\n", mm.pgd);

  // printf("\tAlso mapped in SPT window at: %lx\n\n", 0);

  printf("\tMapped to physical address %lx\n\n", phys);
#endif
}
