// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>
#include<phddef.h>
#include<sysgen.h>
#include<system_data_cells.h>
#include<va_rangedef.h>
#include<wsldef.h>

int exe$adjwsl(signed int pagcnt, unsigned int *wsetlm) {
  struct _pcb * p=smp$gl_cpu_data[smp_processor_id()]->cpu$l_curpcb;
  int newsize;
  //or ctl$gl_phd?
  // do all here, have no priv image in setwork
  p->pcb$l_phd->phd$l_wssize+=pagcnt;
  p->pcb$l_phd->phd$l_wsquota+=pagcnt;
  p->pcb$l_phd->phd$l_wsextent+=pagcnt;
  newsize=p->pcb$l_phd->phd$l_wssize;
  if (pagcnt>0) {
    if (newsize>WSMAX) newsize=WSMAX;
    if (newsize>p->pcb$l_phd->phd$l_wsauthext) newsize=p->pcb$l_phd->phd$l_wsauthext;
    //check with general mem resources too
  }
}
