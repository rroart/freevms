#include <linux/linkage.h>
#include "../../freevms/sys/src/system_data_cells.h"
#include "../../freevms/starlet/src/ssdef.h"
#include "../../freevms/lib/src/pridef.h"
#include "../../freevms/lib/src/acbdef.h"
#include "../../freevms/lib/src/evtdef.h"
#include "../../freevms/lib/src/statedef.h"
#include <linux/sched.h>
#include <linux/smp.h>

int sch$qast(unsigned long pid, int priclass, struct _acb * a) {
  struct _pcb * p=find_process_by_pid(pid);
  int status;
  if (!p) {
    return SS$_NONEXPR;
  }
  /* lck */
  insque(a,p->pcb$l_astqfl);
  /* just simple insert , no pris */
  if (p->pcb$w_state!=SCH$C_CUR)
    status=sch$rse(p, priclass, EVT$_AST);
  /* unlock */
  return status;
}

asmlinkage void sch$astdel(void) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=cpu->cpu$l_curpcb;
  struct _acb * dummy, *acb;
 more:
  /*lock*/
  /* { int i;
  printk("here ast\n");
  for (i=0; i<1000000; i++) ;
  } */
  if (aqempty(&p->pcb$l_astqfl)) return;
  /* { int i,j;
  printk("here ast2 %x %x %x\n",p->pid,p->pcb$l_astqfl,&p->pcb$l_astqfl);
  for (j=0; j<20; j++) for (i=0; i<1000000000; i++) ;
  } */
  acb=remque(p->pcb$l_astqfl,dummy);
  if (acb->acb$b_rmod & ACB$V_KAST) {
    acb->acb$b_rmod&=~ACB$V_KAST;
    /* unlock */
    acb->acb$l_kast();
    goto more;
  }
  acb->acb$l_ast(); /* ? */
  /*unlock*/
}


