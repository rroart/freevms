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

void sch$newlvl(struct _pcb *p);

int sch$qast(unsigned long pid, int priclass, struct _acb * a) {
  int savipl;
  struct _pcb * p=exe$ipid_to_pcb(pid);
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
    struct _pcb * curp=ctl$gl_pcb;
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
  printk("acb %x %x %x %x %x\n",acb,acb->acb$l_pid,acb->acb$l_ast,acb->acb$l_astprm,acb->acb$l_kast);
}

int astdeb=0;

asmlinkage void sch$astdel(void) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=ctl$gl_pcb;
  struct _acb * dummy = 0, *acb;

  /*lock*/
  if (intr_blocked(IPL$_ASTDEL))
    return;

  regtrap(REG_INTR, IPL$_ASTDEL);

  spin_lock(&SPIN_SCHED);
 more:
  setipl(IPL$_SYNCH); // also IPL$_SCHED

  /* { int i;
     //printk("here ast\n");
     for (i=0; i<1000000; i++) ;
     } */
  if (aqempty(&p->pcb$l_astqfl)) {
    sch$newlvl(p);
    spin_unlock(&SPIN_SCHED);
    return;
  }
  /* { int i,j;
     //printk("here ast2 %x %x %x\n",p->pid,p->pcb$l_astqfl,&p->pcb$l_astqfl);
     for (j=0; j<20; j++) for (i=0; i<1000000000; i++) ;
     } */
  acb=remque(p->pcb$l_astqfl,dummy);
  //printk("here ast2 %x %x %x %x\n",p->pid,p->pcb$l_astqfl,&p->pcb$l_astqfl,acb);
#ifdef __i386__
  if (0 && astdeb /*&& ( ((unsigned long)acb->acb$l_ast)>0xd0000000 || ((unsigned long)acb->acb$l_kast)>0xd0000000)*/)   printast(acb);
  //printk(" a %x %x ",p->pcb$l_astqfl,&p->pcb$l_astqfl);
#endif
  //  printast(acb);
  //  mydebug5=1;
  //  printk(KERN_EMERG "astdel %x\n",acb);
  if (acb->acb$b_rmod & ACB$M_KAST) {
    acb->acb$b_rmod&=~ACB$M_KAST;
    /* unlock */
    //printk("astdel1 %x \n",acb->acb$l_kast);
    setipl(IPL$_ASTDEL);
    //p->pcb$b_astact=1;
    if (((unsigned long)acb->acb$l_kast<0x80000000)||((unsigned long)acb->acb$l_kast>0xb0000000)) {
      int i;
      printk("kast %x\n",acb->acb$l_kast);
      for(i=0;i<2000000000;i++) ;
    }
#ifdef __i386__
    if (0 && astdeb)
      printk(" a %x ",acb->acb$l_kast);
  if (0 && astdeb)
    printk(" a %x",*(unsigned long*)acb->acb$l_kast);
#endif
#ifdef __i386__
  //      printk("a1 ");
#endif
    acb->acb$l_kast(acb);
#ifdef __i386__
    //      printk("a2 ");
#endif
    //p->pcb$b_astact=0;
    // do not do this? if ((acb->acb$b_rmod&ACB$M_NODELETE)==0) kfree(acb);
    goto more;
  }
  //printk("astdel2 %x %x \n",acb->acb$l_ast,acb->acb$l_astprm);
  if (p->pcb$b_asten!=15 || p->pcb$b_astact) { // 15 because no modes yet
    insque(acb,p->pcb$l_astqfl);
    p->phd$b_astlvl=p->pr_astlvl=(acb->acb$b_rmod & 3) + 1;
    spin_unlock(&SPIN_SCHED);
    return;
  }
  setipl(IPL$_ASTDEL);
  p->pcb$b_astact=0; // 1; wait with this until we get modes
  setipl(0); // for kernel mode, I think. everything is in kernelmode yet.
  if (((unsigned long)acb->acb$l_ast<0x80000000)&&((unsigned long)acb->acb$l_ast>0xb0000000)) {
    int i;
    printk("kast %x\n",acb->acb$l_ast);
    for(i=0;i<2000000000;i++) ;
  }
#ifdef __i386__
  if (0 && astdeb)
    printk(" a %x",acb->acb$l_ast);
  if (0 && astdeb)
    printk(" a %x",*(unsigned long*)acb->acb$l_ast);
#endif
#ifdef __i386__
  //  printk("a3 ");
#endif
  if (acb->acb$b_rmod&ACB$M_PKAST) {
    acb->acb$b_rmod&=~ACB$M_PKAST;
    if(acb->acb$l_kast) acb->acb$l_kast(acb->acb$l_astprm); /* ? */
  }
#ifdef __arch_um__
  if (((unsigned long)acb->acb$l_ast<0x80000000)) {
    // funny, this printk resulted in tracing too
    // printk("no user ast, avoiding tracing against myself. ast astprm pid %x %x %x %x\n",acb->acb$l_ast,acb->acb$l_astprm,p->pcb$l_pid,p->pid);
  } else {
    if(acb->acb$l_ast) acb->acb$l_ast(acb->acb$l_astprm); /* ? */
  }
#else
  if(acb->acb$l_ast) acb->acb$l_ast(acb->acb$l_astprm); /* ? */
#endif
#ifdef __i386__
  //      printk("a4 ");
#endif
  p->pcb$b_astact=0;
  if ((acb->acb$b_rmod&ACB$M_NODELETE)==0) kfree(acb);
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


