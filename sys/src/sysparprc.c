// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>
#include<asm/hw_irq.h>
#include<system_data_cells.h>
#include<ipldef.h>
#include<phddef.h>

asmlinkage void exe$resched(void) {
  int ipl=getipl();
  struct _pcb * p;
  setipl(IPL$_SCHED);
  spin_lock(&SPIN_SCHED);

  p=smp$gl_cpu_data[0]->cpu$l_curpcb;
  p->pcb$l_onqtime=exe$gl_abstim_tics;

  sch$change_cur_priority(p, p->pcb$b_prib);

  SOFTINT_RESCHED_VECTOR;

  spin_unlock(&SPIN_SCHED);
  setipl(ipl);

}
