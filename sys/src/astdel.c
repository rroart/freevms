#include "../../freevms/sys/src/system_data_cells.h"

int sch$qast(unsigned long pid, int priclass, struct _acb * a) {
  struct _pcb * p=find_process_by_pid(pid);
  int status;
  if (!p) {
    return SS$_NONEXPR;
  }
  /* lck */
  insque(a,current->pcb$l_astqfl);
  /* just simple insert , no pris */
  if (p->pcb$w_state!=SCH$C_CUR)
    status=sch$rse(p, priclass, EVT$_AST);
  /* unlock */
  return status;
}

sch$astdel(void) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=cpu->cpu$l_curpcb;
  struct _acb * dummy, *acb;
 more:
  /*lock*/
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
