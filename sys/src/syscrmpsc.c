// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>
#include <linux/vmalloc.h>
#include <system_data_cells.h>
#include<ipldef.h>
#include<phddef.h>
#include<rdedef.h>
#include<secdef.h>
#include<va_rangedef.h>
#include<wsldef.h>

asmlinkage int exe$mgblsc(void) {
}

/* no short int yet*/
asmlinkage int exe$crmpsc(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode, unsigned int flags, void *gsdnam, unsigned int relpag, unsigned /*short*/ int chan, unsigned int pagcnt, unsigned int vbn, unsigned int prot,unsigned int pfc) {
  // we will just have to pretend this fd is chan and offset i vbn (mmap)?
  // fd -> file, have a version with file = fget(fd);

  struct _secdef *sec, *pstl;
  void * first=inadr->va_range$ps_start_va;
  void * last=inadr->va_range$ps_end_va;
  struct _pcb * p=smp$gl_cpu_data[smp_processor_id()]->cpu$l_curpcb;
  unsigned long prot_pte = 0xc00; // TYP1 and TYP0
  struct _rde * rde;
  int savipl=getipl();
  prot_pte|=p->pcb$l_phd->phd$l_pst_free<<12;

  mmg$vfysecflg();
  // check channel
  // skip wcb for some time?
  setipl(IPL$_ASTDEL);
  mmg$dalcstxscn();

#if 0
  sec=vmalloc(sizeof(struct _secdef));
  bzero(sec,sizeof(struct _secdef));
#endif

  pstl=p->pcb$l_phd->phd$l_pst_base_offset;
  sec=&pstl[p->pcb$l_phd->phd$l_pst_free++];
  sec->sec$l_flags=flags;
  sec->sec$l_ccb=chan;
  sec->sec$l_vbn=vbn;
  sec->sec$l_unit_cnt=pagcnt;

#if 0
  rde=vmalloc(sizeof(struct _rde));
  bzero(rde,sizeof(struct _rde));
  rde->rde$pq_start_va=first;
  rde->rde$q_region_size=last-first;
  rde->rde$l_flags=pfc;
  rde->rde$r_regprot.regprt$l_region_prot = prot;
  insrde(rde,&p->pcb$l_phd->phd$ps_p0_va_list_flink);
#endif

  mmg$fast_create(p, 0, first, last, (last-first)>>12, prot_pte);

  setipl(savipl);

}
