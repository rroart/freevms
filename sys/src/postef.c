#include<linux/sched.h>
#include"../../freevms/starlet/src/ssdef.h"
#include"../../freevms/lib/src/evtdef.h"

int waitcheck(struct _pcb *p, unsigned long priclass, unsigned long * efp, unsigned long * clusteraddr) {
  unsigned long tmp;
  if (efp!=clusteraddr) return;
  tmp=(*clusteraddr)&~p->pcb$l_efwm;
  *clusteraddr&=~p->pcb$l_efwm;
  if (!tmp) return;
  tmp=~(p->pcb$l_sts&(~PCB$M_WALL));
  if (!tmp && tmp!=p->pcb$l_efwm) return;
  sch$rse(p,priclass,EVT$_EVENT);
}

int sch$postef(unsigned long ipid, unsigned long priclass, unsigned long efn) {
  struct _pcb * p;
  int efncluster=(efn&224)>>5;
  int retval;
  unsigned long * clusteraddr;
  /* sched spinlock */
  p=find_process_by_pid(ipid);
  if (!p) return SS$_NONEXPR;
  clusteraddr=&p->pcb$l_efcs+efncluster;
  if ((*clusteraddr)&(~(1<<(efn&31))))
    retval=SS$_WASSET;
  else
    retval=SS$_WASCLR;
  (*clusteraddr)|=(1<<(efn&31));
  waitcheck(p,priclass,&p->pcb$l_efcs+4*p->pcb$b_wefc,clusteraddr);
  return retval;
}
