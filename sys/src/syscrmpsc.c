// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>
#include <linux/vmalloc.h>
#include <system_data_cells.h>
#include<ipldef.h>
#include<phddef.h>
#include<secdef.h>
#include<va_rangedef.h>
#include<wsldef.h>

asmlinkage int exe$crmpsc(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode, unsigned int flags, void *gsdnam, unsigned int relpag, unsigned short int chan, unsigned int pagcnt, unsigned int vbn, unsigned int prot,unsigned int pfc) {
  // we will just have to pretend this fd is chan and offset i vbn (mmap)?
  // fd -> file

  struct _secdef *sec, *pstl;
  struct _pcb * p=smp$gl_cpu_data[smp_processor_id()]->cpu$l_curpcb;
  int savipl=getipl();

  mmg$vfysecflg();
  // check channel
  // skip wcb for some time?
  setipl(IPL$_ASTDEL);
  mmg$dalcstxscn();

#if 0
  sec=vmalloc(sizeof(struct _secdef));
  bzero(sec,sizeof(struct _secdef));
#endif

  pstl=&p->pcb$l_phd->phd$l_pst_base_offset;
  sec=&pstl[p->pcb$l_phd->phd$l_pst_free++];
  sec->sec$l_flags=flags;
  sec->sec$l_ccb=chan;
  sec->sec$l_vbn=vbn;

  //  mmg$fast_create(); //not yet
  exe$cretva(inadr,retadr,acmode);

  setipl(savipl);

}
