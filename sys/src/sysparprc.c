// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>
#include<asm/hw_irq.h>
#include<system_data_cells.h>
#include<ipldef.h>
#include<ipl.h>
#include<phddef.h>
#include <sch_routines.h>
#include<internals.h>

asmlinkage void exe$resched(void) {
  int ipl=getipl();
  struct _pcb * p;
  setipl(IPL$_SCHED);
  vmslock(&SPIN_SCHED,-1);

  p=ctl$gl_pcb;
  p->pcb$l_onqtime=exe$gl_abstim_tics;

  sch$change_cur_priority(p, p->pcb$b_prib);

  SOFTINT_RESCHED_VECTOR;

  vmsunlock(&SPIN_SCHED,-1);
  setipl(ipl);

}
