#include <linux/sched.h>
#include <linux/smp.h>
#include "../../freevms/lib/src/cpudef.h"
#include "../../freevms/starlet/src/ssdef.h"

int exe$clref(unsigned int efn) {
  struct _pcb * p=smp$gl_cpu_data[smp_processor_id()]->cpu$l_curpcb;
  int efncluster=(efn&224)>>5;
  unsigned long * clusteraddr;
  clusteraddr=&p->pcb$l_efcs+efncluster;
  (*clusteraddr)&=~(1<<(efn&31));
}
