// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>

#include<linux/mm.h>
#include<linux/sched.h>

#include<asm/current.h>

#include<ipldef.h>
#include<phddef.h>
#include<ssdef.h>
#include<va_rangedef.h>
#include<wsldef.h>
#include<vmspte.h>

int mmg$purgwsscn(int acmode, void * va, struct _pcb * p, signed int pagedirection, struct _rde * rde, unsigned long newpte) {
#ifdef CONFIG_VMS
  // acquire mmg spin
  struct page * page;
  struct _phd * phd;
  struct _wsl * wsl, *wsle;
  unsigned long pfn;
  struct mm_struct * mm = p->mm;
  if (mm==0) 
    mm = p->active_mm; // workaround for do_exit
  struct _mypte *pte = findpte_new(mm,va);
  if (pte->pte$v_valid==0)
    return SS$_NORMAL;
#ifdef __arch_um__
  pfn=__pa(pte->pte$v_pfn<<PAGE_SHIFT)>>PAGE_SHIFT;
#else
  pfn=pte->pte$v_pfn;
#endif
  page=&mem_map[pfn];
  phd=p->pcb$l_phd;
  wsl=phd->phd$l_wslist;

  if ((unsigned long)page->pfn$l_wslx_qw>=0x80000000)
    return SS$_NORMAL;

  wsle=&wsl[page->pfn$l_wslx_qw];

  if (wsle->wsl$v_valid==0)
    return SS$_NORMAL;

  if (wsle->wsl$v_pfnlock)
    return SS$_NORMAL;

  if ((((unsigned long)wsle->wsl$pq_va)&WSL$M_PAGTYP)>=WSL$C_PPGTBL)
    return SS$_NORMAL;

  mmg$frewslx(p, va, findpte_new(mm, va), page->pfn$l_wslx_qw);

  return SS$_NORMAL;
#endif
}

int mmg$purgwspag (int acmode, void * va, struct _pcb * p, signed int pagedirection, struct _rde * rde, unsigned long newpte) {
  return mmg$purgwsscn(acmode,va,p,pagedirection,rde,newpte);
}

asmlinkage int exe$purgws(struct _va_range *inadr) {
  unsigned long start=((unsigned long)inadr->va_range$ps_start_va)&0xfffff000;
  unsigned long end=((unsigned long)inadr->va_range$ps_end_va)&0xfffff000;
  setipl(IPL$_ASTDEL);
  for(;start<end;start+=PAGE_SIZE) {
    mmg$credel(0, start, end, mmg$purgwspag, 0, 0, 0, current, 1);
  }
  return SS$_NORMAL;
}
