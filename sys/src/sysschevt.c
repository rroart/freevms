#include<linux/linkage.h>
#include<linux/vmalloc.h>
#include<linux/smp.h>
#include<linux/sched.h>
#include"../../freevms/starlet/src/ssdef.h"
#include"../../freevms/lib/src/tqedef.h"
#include"../../freevms/lib/src/cpudef.h"
#include"../../freevms/sys/src/system_data_cells.h"
#include "../../freevms/lib/src/statedef.h"

asmlinkage int exe$setimr  (unsigned int efn, signed long long *daytim,
		 void (*astadr)(long), unsigned
		 long reqidt, unsigned int flags) {
  unsigned long long time;
  struct _tqe * t=vmalloc(sizeof(struct _tqe));
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  bzero(t,sizeof(struct _tqe));
  if ( efn ) exe$clref(efn);
  if (flags) t->tqe$b_type=TQE$M_CHK_CPUTIM;
  if (!daytim) return SS$_ACCVIO;
  if (*daytim<0) {
    time=exe$gq_systime+*daytim;
  } else {
    time=*daytim;
  }
  printk("astadr %x\n",astadr);
  t->tqe$l_ast=astadr;
  //  printk("astadr2 %x\n",t->tqe$l_fpc);
  t->tqe$l_astprm=reqidt;
  t->tqe$q_time=time;
  t->tqe$b_rqtype=TQE$C_TMSNGL;
  t->tqe$l_pid=cpu->cpu$l_curpcb->pid;
  t->tqe$b_efn=efn;
  printast((struct _acb * )t);
  cpu->cpu$l_curpcb->pcb$w_state=SCH$C_LEF; /* put it here until ... */
  exe$instimq(t);
}
