#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include "../../freevms/lib/src/cebdef.h"
#include "../../freevms/lib/src/pridef.h"
#include "../../freevms/lib/src/cpudef.h"
#include "../../freevms/starlet/src/ssdef.h"
#include "../../freevms/sys/src/system_data_cells.h"

asmlinkage int exe$clref(unsigned int efn) {
  int retval;
  struct _pcb * p=smp$gl_cpu_data[smp_processor_id()]->cpu$l_curpcb;
  int efncluster=(efn&224)>>5;
  unsigned long * clusteraddr;
  clusteraddr=getefc(p,efn);
  retval=test_and_clear_bit(efn&31,clusteraddr);
  /* do a bbcci instead */
  if (retval)
    return SS$_WASSET;
  else
    return SS$_WASCLR;
}

asmlinkage int exe$readef(unsigned int efn, unsigned int *state) {
  struct _pcb * p=current;
  int efncluster=(efn&224)>>5;
  unsigned long * clusteraddr;
  clusteraddr=getefc(p,efn);
  *state=*clusteraddr;
}

asmlinkage int exe$setef(unsigned int efn) {
  struct _pcb * p=current;
  return sch$postef(p->pid,PRI$_IOCOM,efn);
}

inline unsigned long * getefcp(struct _pcb * p, unsigned long efn) {
  return &p->pcb$l_efcs + ((efn&96)>>5);
}

inline unsigned long * getefc(struct _pcb * p, unsigned long efn) {
  unsigned long * retval = &p->pcb$l_efcs + ((efn&96)>>5);
  if (efn<64)
    return retval;
  if (!retval) 
    return 0;
  return &((struct _ceb *)(*retval))->ceb$l_efc;
}

inline unsigned long * getefcno(unsigned long efn) {
  return ((efn&96)>>5);
}

