#include<linux/linkage.h>
#include"../../freevms/lib/src/acbdef.h"
#include"../../freevms/lib/src/cpudef.h"
#include<linux/sched.h>
#include<asm/hw_irq.h>

asmlinkage void sw_ast(void) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=cpu->cpu$l_curpcb;
  return;
  // varm stuff here REI
  // should reaaly be if (cpu->cpu$b_cur_mod > p->phd$b_astlvl)
  if (p->phd$b_astlvl<4)
    SOFTINT_ASTDEL_VECTOR;
  /* check sw interrupts */
}
