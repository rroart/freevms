#include<linux/linkage.h>
#include"../../freevms/sys/src/system_data_cells.h"
#include"../../freevms/lib/src/ucbdef.h"
#include"../../freevms/lib/src/irpdef.h"
#include"../../freevms/lib/src/fkbdef.h"
#include"../../freevms/lib/src/ipldef.h"
#include"../../freevms/pal/src/ipl.h"
#include <linux/smp.h>
#include <asm/current.h>

asmlinkage void exe$forkdspth(void) {
  if (intr_blocked(IPL$_QUEUEAST))
    return;
  regtrap(REG_INTR, IPL$_QUEUEAST);
  setipl(IPL$_TIMER);
}

void exe$iofork(struct _ucb * u, struct _irp * i) {
  int curipl;
  int newipl;
  int isempty;
  struct _fkb * f;
  /* need caller and caller's caller address of return again */
  u->ucb$l_sts&=~UCB$M_TIM;
  u->ucb$l_fr3=i;
  u->ucb$l_fr4=current;
  newipl=u->ucb$b_dipl;
  f=smp$gl_cpu_data[smp_processor_id()]->cpu$q_swiqfl[newipl-6];
  isempty=aqempty(f);
  insque(u,f->fkb$l_fqbl);
  if (f) {
    /* do SOFTINTS */
  }
}
