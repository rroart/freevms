#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include "../../freevms/lib/src/pridef.h"
#include "../../freevms/lib/src/cpudef.h"
#include "../../freevms/starlet/src/ssdef.h"

asmlinkage int exe$clref(unsigned int efn) {
  struct _pcb * p=smp$gl_cpu_data[smp_processor_id()]->cpu$l_curpcb;
  int efncluster=(efn&224)>>5;
  unsigned long * clusteraddr;
  clusteraddr=&p->pcb$l_efcs+efncluster;
  (*clusteraddr)&=~(1<<(efn&31));
}

asmlinkage int exe$readef(unsigned int efn, unsigned int *state) {
  struct _pcb * p=current;
  int efncluster=(efn&224)>>5;
  unsigned long * clusteraddr;
  clusteraddr=&p->pcb$l_efcs+efncluster;
  *state=*clusteraddr;
}

asmlinkage int exe$setef(unsigned int efn) {
  struct _pcb * p=current;
  int efncluster=(efn&224)>>5;
  unsigned long * clusteraddr;
  clusteraddr=&p->pcb$l_efcs+efncluster;
  (*clusteraddr)|=(1<<(efn&31));
  sch$postef(p->pid,PRI$_IOCOM,efn);
}
