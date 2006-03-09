// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>
#include<ssdef.h>
#include<tqedef.h>
#include<cpudef.h>
#include<dyndef.h>
#include<system_data_cells.h>
#include <ipl.h>
#include<internals.h>
#include <statedef.h>
#include<linux/vmalloc.h>
#include<linux/smp.h>
#include <linux/slab.h>
#include <exe_routines.h>

asmlinkage int exe$setimr  (unsigned int efn, signed long long *daytim,
		 void (*astadr)(long), unsigned
		 long reqidt, unsigned int flags) {
  unsigned long long time;
  struct _tqe * t=kmalloc(sizeof(struct _tqe),GFP_KERNEL);
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  memset(t,0,sizeof(struct _tqe));
  t->tqe$b_type=DYN$C_TQE;
  exe$clref(efn);
  if (flags) t->tqe$b_rqtype=TQE$M_CHK_CPUTIM;
  if (!daytim) return SS$_ACCVIO;
  if (*daytim<0) {
    time=exe$gq_systime-*daytim;
  } else {
    time=*daytim;
  }
  //printk("astadr %x\n",astadr);
  t->tqe$l_ast=astadr;
  //  printk("astadr2 %x\n",t->tqe$l_fpc);
  t->tqe$l_astprm=reqidt;
  t->tqe$q_time=time;
  t->tqe$b_rqtype=TQE$C_TMSNGL;
  t->tqe$l_pid=ctl$gl_pcb->pcb$l_pid;
  t->tqe$b_efn=efn;
  //printast((struct _acb * )t);
  ctl$gl_pcb->pcb$w_state=SCH$C_LEF; /* put it here until ... */
  exe$instimq(t);
}

asmlinkage int exe$schdwk(unsigned int *pidadr, void *prcnam, signed long long * daytim, signed long long * reptim) {
  struct _pcb * retpcb, *p;
  unsigned long retipid, retepid;
  int sts=exe$nampid(current,pidadr,prcnam,&retpcb,&retipid,&retepid);
  p=retpcb;
  vmsunlock(&SPIN_SCHED,0);
  /* no cwps here either. snif. */
  if (!p) return;
  {
  unsigned long long time;
  struct _tqe * t=kmalloc(sizeof(struct _tqe),GFP_KERNEL);
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  memset(t,0,sizeof(struct _tqe));
  t->tqe$b_type=DYN$C_TQE;
  if (*daytim<0) {
    time=exe$gq_systime-*daytim;
  } else {
    time=*daytim;
  }
  t->tqe$l_pid=p->pcb$l_pid;
  t->tqe$q_time=time;
  if (reptim) t->tqe$q_delta=-(*reptim);
  t->tqe$b_rqtype=TQE$C_WKSNGL;
  if (reptim) t->tqe$b_rqtype|=TQE$M_REPEAT;
  //printast((struct _acb * )t);
  exe$instimq(t);
  }
}


