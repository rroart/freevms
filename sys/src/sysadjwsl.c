// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>
#include<sysgen.h>
#include<system_data_cells.h>
#include<va_rangedef.h>
#include<wsldef.h>

int exe$adjwsl(int pagcnt, unsigned int *wsetlm) {
  struct _pcb * p=smp$gl_cpu_data[smp_processor_id()]->cpu$l_curpcb;
  //or ctl$gl_phd?
  int newsize=p->pcb$l_phd->phd$l_wssize+pagcnt;
  if (pagcnt>0) {
    if (newsize>WSMAX) newsize=WSMAX;
    if (newsize>p->pcb$l_phd->phd$l_wsauthext) newsize=p->pcb$l_phd->phd$l_wsauthext;
    //check with general mem resources too
  }
}
