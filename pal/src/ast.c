#include<linux/linkage.h>
#include"../../freevms/lib/src/acbdef.h"
#include"../../freevms/lib/src/cpudef.h"
#include<linux/sched.h>
#include "../../freevms/sys/src/asmlink.h"
#include<asm/hw_irq.h>

asmlinkage void sw_ast(void) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=cpu->cpu$l_curpcb;
  //printk("sw_ast\n");
  //{ int i; for (i=0;i<10000000;i++) ; }
  //return;
  // varm stuff here REI
  // should reaaly be if (cpu->cpu$b_cur_mod > p->phd$b_astlvl)
  if (p->phd$b_astlvl<4)
    SOFTINT_ASTDEL_VECTOR;
  /* check sw interrupts */
}
