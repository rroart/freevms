// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/spinlock.h>
#include<asm/unistd.h>

#include<acbdef.h>
#include<aqbdef.h>
#include<cdrpdef.h>
#include<ddtdef.h>
#include<dyndef.h>
#include<fdtdef.h>
#include<internals.h>
#include<iodef.h>
#include<ipldef.h>
#include<irpdef.h>
#include<pridef.h>
#include<ssdef.h>
#include<starlet.h>
#include<system_data_cells.h>
#include<ucbdef.h>
#include<vcbdef.h>

#include<linux/vmalloc.h>
#include<linux/linkage.h>
#include<linux/kernel.h>
#include<asm/hw_irq.h>
#include "../../starlet/src/sysdep.h"

// temporary stuff with syscalls

// extern int kernel_errno;

void exe$insertirp(void * u, struct _irp * i) {
  struct _irp *tmp=((struct _irp *)u)->irp$l_ioqfl;
  while (tmp!=u && i->irp$b_pri>tmp->irp$b_pri)
    tmp=tmp->irp$l_ioqfl;
  insque(i,tmp);
}

void exe_std$abortio(struct _irp * i, struct _pcb * p, struct _ucb * u, unsigned long s) {
}

void exe$altqueuepkt (void) {
}

void exe$finishio (long long * iosb, struct _irp * i, struct _pcb * p, struct _ucb * u) {
  /* do iosb stuff */
  forklock(u->ucb$b_flck,-1);
  insque(i,&smp$gl_cpu_data[smp_processor_id()]->cpu$l_psbl);
  SOFTINT_IOPOST_VECTOR;
  forkunlock(u->ucb$b_flck,-1);
}

void exe$finishioc (long long * iosb, struct _irp * i, struct _pcb * p, struct _ucb * u) {
  *iosb&=0xffffffff00000000;
  exe$finishio(iosb,i,p,u);
}

int exe$insioq (struct _irp * i, struct _ucb * u) {
  /* raise ipl */
  int savipl=forklock(u->ucb$b_flck,u->ucb$b_flck);
  if (u->ucb$l_sts & UCB$M_BSY)
    exe$insertirp(&u->ucb$l_ioqfl,i);
  else {
    u->ucb$l_sts|=UCB$M_BSY;
    ioc$initiate(i,u);
  }
  /* restore ipl */
  /* release fork lock */
  forkunlock(u->ucb$b_flck,savipl);
  return;
}

asmlinkage int exe_qiow(unsigned int efn, unsigned short int chan,unsigned int func, struct _iosb *iosb, void(*astadr)(__unknown_params), long  astprm, void*p1, long p2, long  p3, long p4, long p5, long p6) {
  struct struct_qio s;
  //printk("in exe_qiow %x %x %x %x %x %x %x %x %x %x %x\n",efn,chan,func,iosb,astadr,astprm,p1,p2,p3,p4);
  s.efn=efn;
  s.chan=chan;
  s.func=func;
  s.iosb=iosb;
  s.astadr=astadr;
  s.astprm=astprm;
  s.p1=p1;
  s.p2=p2;
  s.p3=p3;
  s.p4=p4;
  s.p5=p5;
  s.p6=p6;
  return exe$qiow(&s);
}

asmlinkage int exe$qiow (struct struct_qio * q) {

  /* I think this is about it */

  int status=sys$qio(q->efn,q->chan,q->func,q->iosb,q->astadr,q->astprm,q->p1,q->p2,q->p3,q->p4,q->p5,q->p6);
  if ((status&1)==0) return status;
  return exe$synch(q->efn,q->iosb);

}

/* put this into a struct */
asmlinkage int exe$qio (struct struct_qio * q) {
  int func;
  unsigned int c, d;
  struct _pcb * p=current;
  struct _irp * i;
  exe$clref(q->efn);
  if (q->chan<0 || q->chan>ctl$gl_chindx) return SS$_IVCHAN;
  /*ccb$l_wind stuff*/
  func=q->func;
  /*spooled?*/
  /*no access check yet*/
  /*check fdt mask*/
  /*check func code*/
  /*check iosb*/
  if (q->iosb) *((unsigned long long *)q->iosb)=0;
  setipl(IPL$_ASTDEL);
  /*check proc quota*/
  i=kmalloc(sizeof(struct _irp),GFP_KERNEL);
  bzero(i,sizeof(struct _irp));
  i->irp$b_type=DYN$C_IRP;
  i->irp$b_efn=q->efn;
  i->irp$l_ast=q->astadr;
  i->irp$l_astprm=q->astprm;
  i->irp$l_iosb=q->iosb;
  i->irp$w_chan=q->chan;
  i->irp$l_func=q->func;
  i->irp$b_pri=p->pcb$b_pri;
  i->irp$l_qio_p1=q->p1;
  i->irp$l_qio_p2=q->p2;
  i->irp$l_qio_p3=q->p3;
  i->irp$l_qio_p4=q->p4;
  i->irp$l_qio_p5=q->p5;
  i->irp$l_qio_p6=q->p6;
  i->irp$l_ucb=ctl$gl_ccbbase[q->chan].ccb$l_ucb;
  i->irp$l_pid=current->pcb$l_pid;
  i->irp$l_sts|=IRP$M_BUFIO; /* no DIRIO because of no mmg$svaptechk */
  /* do preprocessing */
  /* does it do one or more functions */
  //  for(c=0,d=1;c<64;c++,d=d<1) /* right order? */
  //  if (d&func) {
  c=i->irp$v_fcode;
  d=i->irp$v_fmod;
  return ctl$ga_ccb_table[q->chan].ccb$l_ucb->ucb$l_ddt->ddt$l_fdt->fdt$ps_func_rtn[i->irp$v_fcode](i,p,i->irp$l_ucb,&ctl$gl_ccbbase[q->chan]); // a real beauty, isn't it :)
      //  }
 earlyerror:
  setipl(0);
  sch$postef(current->pcb$l_pid,PRI$_NULL,q->efn);
  return 0;		   
}

int exe$qioacppkt (struct _irp * i, struct _pcb * p, struct _ucb * u) {
  int wasempty;
  struct _vcb * v=u->ucb$l_vcb;
  struct _aqb * a=v->vcb$l_aqb;
  wasempty=aqempty(&a->aqb$l_acpqfl);
  insque(i,&a->aqb$l_acpqfl);
  if (!wasempty) return SS$_NORMAL;
  if (a->aqb$l_acppid==0) {
    exe$qioqxqppkt(p,i);
    return SS$_NORMAL;
  }
  sch$wake(a->aqb$l_acppid);
  setipl(0);
  return SS$_NORMAL;
}

int exe$qiodrvpkt (struct _irp * i, struct _pcb * p, struct _ucb * u) {

  exe$insioq(i,u);
  setipl(0);
  return SS$_NORMAL;
}

int exe_std$qiodrvpkt (struct _irp * i, struct _ucb * u) {
  return exe$qiodrvpkt(i,0,u);
}

extern f11b$dispatch();

void exe$qioqxqppkt (struct _pcb * p, struct _irp * i) {
  struct _acb *a=&i->irp$l_fqfl;
  //  struct _f11b * f=ctl$gl_f11bxqp;

  a->acb$l_pid=p->pcb$l_pid;
  a->acb$l_ast=f11b$dispatch;
  a->acb$l_astprm=i;
  a->acb$b_rmod|=ACB$M_NODELETE; // bad idea to free this
  remque(i,0); // got to get rid of this somewhere, why not here?
  sch$qast(p->pcb$l_pid,PRI$_RESAVL,a);
}


