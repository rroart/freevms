#include "../../freevms/sys/src/system_data_cells.h"
#include "../../freevms/lib/src/acbdef.h"
#include "../../freevms/lib/src/pridef.h"
#include "../../freevms/lib/src/cpudef.h"
#include <linux/smp.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>

int exe$setast(char enbflg) {

}

int exe$dclast(void (*astadr)(__unknown_params), unsigned long astprm, unsigned int acmode) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=cpu->cpu$l_curpcb;
  struct _acb * a=vmalloc(sizeof(struct _acb));
  a->acb$l_pid=p->pid;
  a->acb$l_ast=astadr;
  a->acb$l_astprm=astprm;
  sch$qast(p->pid,PRI$_NULL,a);
}
