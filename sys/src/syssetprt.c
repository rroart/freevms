// $Id$
// $Locker$

// Author. Roar Thronæs.

// everything is untested

#include<linux/linkage.h>
#include<linux/sched.h>
#include<linux/slab.h>
#include<asm/pgalloc.h>
#include<asm/mmu_context.h>
#include<internals.h>
#include<ipldef.h>
#include<mmg_functions.h>
#include<mmg_routines.h>
#include<rdedef.h>
#include<ssdef.h>
#include<system_data_cells.h>
#include<va_rangedef.h>
#include<vmspte.h>

int mmg$pteref_64 (void * va, struct _rde *rde, void ** va_pte);

int mmg$setprtpag (int acmode, void * va, struct _pcb * p, signed int pagedirection, struct _rde * rde, unsigned long newpte) {
 step1:
  {}
  long * pte;
  int sts = mmg$pteref_64 (va, rde, &pte);
  if ((sts&1) == 0)
    return sts;
  if (pte == 0) {
    vmsunlock (&SPIN_MMG, -1); // check. ipl level
    return SS$_NOSUCHPAG;
  }

  struct _mypte * mypte = pte;
  struct _mypte * newptep = &newpte;

  // check. duplicated from pagefault. clean up both some time

  // different forms of invalid ptes?
  // 0 valid bit, and...?
  // how to differentiate between
  // demandzeropage, transitionpage, invalidglobalpage, pageinpfile or
  //   in image file
  // Use bit 9-11, they are available for os use.
  // 11 and 10 reflect vax pte 26 22  (and 9 for 21)
  // 0 0 pfn=0 demand zero page
  // 0 0 pfn!=0 page in transition
  // 0 1 pfn=gpti invalid global page
  // 1 0 pfn=misc page is in page file
  // 1 1 page is in image file

  if ((*(unsigned long *)pte)&1) goto valid; // valid bit set

  if ((*(unsigned long *)pte)&_PAGE_TYP1) { // page or image file
    if ((*(unsigned long *)pte)&_PAGE_TYP0) { // image file
      if (mypte->pte$v_wrt && newptep->pte$v_wrt == 0)
	goto step5;
      // not yet. check if writeable from some mode or copy-on-reference
      // not yet. read to write and copy-on-reference
    } else { // page file
    }
  }

  if (!((*(unsigned long *)pte)&_PAGE_TYP1)) { // zero transition or global
    if (!((*(unsigned long *)pte)&_PAGE_TYP0)) {
      if ((*(unsigned long *)pte)&0xffffffff) { // transition
	if (newptep->pte$v_wrt == 0) {
	  vmsunlock (&SPIN_MMG, IPL$_ASTDEL); // check ipl
	  *(long *)pte&=1; // check. touch to make it valid?
	  goto step1;
	}
      } else { // zero page demand?
	if (newptep->pte$v_wrt == 0) {
	  vmsunlock (&SPIN_MMG, IPL$_ASTDEL); // check ipl
	  *(long *)pte&=1; // check. touch to make it valid?
	  goto step1;
	} else
	  goto step5;
      }
    } else {
    }
  }

  goto step5;

 valid:

  // check. postponed implementation

  if ((*(unsigned long *)pte)&_PAGE_TYP1) { // page or image file
    if ((*(unsigned long *)pte)&_PAGE_TYP0) { // image file
    } else { // page file
    }
  }

  if (!((*(unsigned long *)pte)&_PAGE_TYP1)) { // zero transition or global
    if (!((*(unsigned long *)pte)&_PAGE_TYP0)) {
      if ((*(unsigned long *)pte)&0xffffffff) { // transition
      } else { // zero page demand?
      }
    } else {
    }
  }

 step5:
  {}

  struct _pcb * pcb = p;
  long page = va;
  *(long *)pte = newpte;
  flush_tlb_range(pcb->mm, page, page + PAGE_SIZE);

  vmsunlock (&SPIN_MMG, IPL$_ASTDEL);

  return SS$_NORMAL;
}

asmlinkage int exe$setprt  (struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode, unsigned int prot, unsigned char *prvprt) {
  // no prot transformation yet
  // no scratch space yet
  // no inadr access test yet
  // no acmode maximization yet
  int ipl = getipl();
  setipl(IPL$_ASTDEL);
  struct _rde * rde = mmg$lookup_rde_va (inadr->va_range$ps_start_va, ctl$gl_pcb->pcb$l_phd, LOOKUP_RDE_EXACT, IPL$_ASTDEL);
  void * first=inadr->va_range$ps_start_va;
  void * last=inadr->va_range$ps_end_va;
  struct _pcb * p=ctl$gl_pcb;
  unsigned long numpages=(last-first)/PAGE_SIZE;
  int ret = mmg$credel(acmode, first, last, mmg$setprtpag, inadr, retadr, acmode, p, numpages);
  if (ret == SS$_PAGTYPVIO)
    ret = SS$_NOPRIV;
  if (ret == SS$_NOSUCHPAG)
    ret = SS$_ACCVIO;
  setipl(ipl);
  return ret;
}

asmlinkage int exe$fault_page(void) {

}

