// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/linkage.h>
#include <system_data_cells.h>
#include <ssdef.h>
#include <pridef.h>
#include <acbdef.h>
#include <evtdef.h>
#include <statedef.h>
#include <queue.h>
#include <ipldef.h>
#include <ipl.h>
#include <linux/sched.h>
#include <linux/smp.h>

extern int mydebug5;

int sch$qast(unsigned long pid, int priclass, struct _acb * a) {
  int savipl;
  struct _pcb * p=find_process_by_pid(pid);
  int status=SS$_NORMAL;
  int kernelmode;
  if (!p) {
    return SS$_NONEXPR;
  }
  /* lock */
  savipl=getipl();
  spin_lock(&SPIN_SCHED);
  setipl(IPL$_SYNCH);
  insque(a,&p->pcb$l_astqfl);
  if ((a->acb$b_rmod & ACB$M_KAST)==0)
    kernelmode=a->acb$b_rmod & 3;
  else
    kernelmode=0;
  p->phd$b_astlvl=kernelmode;
  /* just simple insert , no pris yet */
  //printk("bef rse\n");
  if (p->pcb$w_state!=SCH$C_CUR)
    status=sch$rse(p, priclass, EVT$_AST);
  else {
    struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
    struct _pcb * curp=cpu->cpu$l_curpcb;
    // if (p==curp) etc // smp not enabled
    p->pr_astlvl=p->phd$b_astlvl;
  }
  //printk("aft rse\n");
  /* unlock */
  spin_unlock(&SPIN_SCHED);
  setipl(savipl);
  return status;
}

printast(struct _acb * acb) {
  printk("acb %x %x %x %x\n",acb,acb->acb$l_pid,acb->acb$l_ast,acb->acb$l_astprm);
}

asmlinkage void sch$astdel(void) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=cpu->cpu$l_curpcb;
  struct _acb * dummy, *acb;

  /*lock*/
  if (intr_blocked(IPL$_ASTDEL))
    return;

  regtrap(REG_INTR, IPL$_ASTDEL);

  spin_lock(&SPIN_SCHED);
 more:
  setipl(IPL$_SYNCH);

  /* { int i;
     printk("here ast\n");
     for (i=0; i<1000000; i++) ;
     } */
  if (aqempty(&p->pcb$l_astqfl)) {
    spin_unlock(&SPIN_SCHED);
    return;
  }
  /* { int i,j;
     printk("here ast2 %x %x %x\n",p->pid,p->pcb$l_astqfl,&p->pcb$l_astqfl);
     for (j=0; j<20; j++) for (i=0; i<1000000000; i++) ;
     } */
  acb=remque(p->pcb$l_astqfl,dummy);
  printk("here ast2 %x %x %x %x\n",p->pid,p->pcb$l_astqfl,&p->pcb$l_astqfl,acb);
  printast(acb);
  //  mydebug5=1;
  //  printk(KERN_EMERG "astdel %x\n",acb);
  if (acb->acb$b_rmod & ACB$M_KAST) {
    acb->acb$b_rmod&=~ACB$M_KAST;
    /* unlock */
    printk("astdel1 %x \n",acb->acb$l_kast);
    setipl(IPL$_ASTDEL);
    p->pcb$b_astact=1;
    acb->acb$l_kast(acb->acb$l_astprm);
    p->pcb$b_astact=0;
    goto more;
  }
  printk("astdel2 %x %x \n",acb->acb$l_ast,acb->acb$l_astprm);
  setipl(IPL$_ASTDEL);
  if (p->pcb$b_asten!=15 || p->pcb$b_astact) { // 15 because no modes yet
    insque(acb,p->pcb$l_astqfl);
    p->phd$b_astlvl=p->pr_astlvl=(acb->acb$b_rmod & 3) + 1;
    spin_unlock(&SPIN_SCHED);
    return;
  }
  p->pcb$b_astact=1;
  if(acb->acb$l_ast) acb->acb$l_ast(acb->acb$l_astprm); /* ? */
  p->pcb$b_astact=0;
  /*unlock*/
  goto more;
}

void sch$newlvl(struct _pcb *p) {
  int newlvl;
  int oldipl=getipl();

  spin_lock(&SPIN_SCHED);
  setipl(IPL$_SYNCH);

  if (aqempty(p->pcb$l_astqfl))
    newlvl=4;
  else {
    if(p->pcb$l_astqfl->acb$b_rmod & ACB$M_KAST)
      newlvl=p->pcb$l_astqfl->acb$b_rmod & 3;
    else
      newlvl=p->phd$b_astlvl; /* ? */
  }
    
  p->phd$b_astlvl=newlvl;
  p->pr_astlvl=newlvl;
  spin_unlock(&SPIN_SCHED);
  setipl(oldipl);
}


