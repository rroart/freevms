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

int mmg$purgwsscn(int acmode, void * va, struct _pcb * p, signed int pagedirection, struct _rde * rde, unsigned long newpte) {
#ifdef CONFIG_VMS
  // acquire mmg spin
  unsigned long pfn=__pa(va)>>PAGE_SHIFT;
  struct page * page=&mem_map[pfn];
  struct _phd * phd=p->pcb$l_phd;
  struct _wsl * wsle, *wsl=phd->phd$l_wslist;

  wsle=&wsl[page->pfn$l_wslx_qw];

  if (wsle->wsl$v_valid==0)
    return SS$_NORMAL;

  if (wsle->wsl$v_pfnlock)
    return SS$_NORMAL;

  if ((((unsigned long)wsle->wsl$pq_va)&WSL$M_PAGTYP)>=WSL$C_PPGTBL)
    return SS$_NORMAL;

  mmg$frewslx(p, va, findpte(p,va), page->pfn$l_wslx_qw);

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
