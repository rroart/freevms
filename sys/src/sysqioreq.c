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

void exe$qioqe2ppkt (struct _pcb * p, struct _irp * i);
void exe$qioqxqppkt (struct _pcb * p, struct _irp * i);

// temporary stuff with syscalls

// extern int kernel_errno;

void exe$insertirp(struct _ucb * u, struct _irp * i) {
  //  struct _irp *tmp=((struct _irp *)u)->irp$l_ioqfl; // change to ucb$l_ioqfl
  struct _irp *head=&u->ucb$l_ioqfl;
  struct _irp *tmp=u->ucb$l_ioqfl;
  while (tmp!=head && i->irp$b_pri>tmp->irp$b_pri)
    tmp=tmp->irp$l_ioqfl;
  insque(i,tmp);
}

int exe_std$abortio(struct _irp * i, struct _pcb * p, struct _ucb * u, unsigned long s) {
  // acquire forklock etc
  forklock(u->ucb$b_flck,-1);
#if 0
  if (i->irp$l_iosb)
    bzero(i->irp$l_iosb,8);
#endif
  i->irp$l_iosb=0;
  // inc process ast quota if v_quota was set
  i->irp$b_rmod&=~ACB$M_QUOTA;
  
  insque(i,smp$gl_cpu_data[smp_processor_id()]->cpu$l_psbl);
  SOFTINT_IOPOST_VECTOR;

  forkunlock(u->ucb$b_flck,-1);
  return s;
}

void exe$altqueuepkt (void) {}

int exe$altquepkt (struct _irp * i, struct _pcb * p, struct _ucb * u) {
  struct _ddt *d; 
  void (*f)(void *,void *);
  int savipl=forklock(u->ucb$b_flck,u->ucb$b_flck);
  /* no smp stuff yet */

  d=u->ucb$l_ddt;
  f=d->ddt$l_altstart;
  f(i,u);

  forkunlock(u->ucb$b_flck,savipl);
  return SS$_NORMAL;
}

int exe$finishio (long status1, long status2, struct _irp * i, struct _pcb * p, struct _ucb * u) {
  /* do iosb stuff */
  i->irp$l_iost1=status1;
  i->irp$l_iost2=status2;
  forklock(u->ucb$b_flck,-1);
  insque(i,smp$gl_cpu_data[smp_processor_id()]->cpu$l_psbl);
  SOFTINT_IOPOST_VECTOR;
  forkunlock(u->ucb$b_flck,-1);
  setipl(0);
  return SS$_NORMAL;
}

int exe$finishioc (long status, struct _irp * i, struct _pcb * p, struct _ucb * u) {
  return exe$finishio(status,0,i,p,u);
}

int exe$insioq (struct _irp * i, struct _ucb * u) {
  /* raise ipl */
  int savipl=forklock(u->ucb$b_flck,u->ucb$b_flck);
  if (u->ucb$l_sts & UCB$M_BSY)
    exe$insertirp(u,i); // should be &u->ucb$l_ioqfl ?
  else {
    u->ucb$l_sts|=UCB$M_BSY;
    ioc$initiate(i,u);
  }
  /* restore ipl */
  /* release fork lock */
  forkunlock(u->ucb$b_flck,savipl);
  return SS$_NORMAL;
}

#if 0
asmlinkage int exe_qiow_not(unsigned int efn, unsigned short int chan,unsigned int func, struct _iosb *iosb, void(*astadr)(__unknown_params), long  astprm, void*p1, long p2, long  p3, long p4, long p5, long p6) {
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
#endif

extern int sys$qio(unsigned int efn, unsigned short int chan,unsigned int func, struct _iosb *iosb, void(*astadr)(__unknown_params), long  astprm, void*p1, long p2, long  p3, long p4, long p5, long p6);

asmlinkage int exe$qiow (unsigned int efn, unsigned short int chan,unsigned int func, struct _iosb *iosb, void(*astadr)(__unknown_params), long  astprm, void*p1, long p2, long  p3, long p4, long p5, long p6) {

  /* I think this is about it */

  int status=sys$qio(efn,chan,func,iosb,astadr,astprm,p1,p2,p3,p4,p5,p6);
  if ((status&1)==0) return status;
  return exe$synch(efn,iosb);

}

/* put this into a struct */
asmlinkage int exe$qio (unsigned int efn, unsigned short int chan,unsigned int func, struct _iosb *iosb, void(*astadr)(__unknown_params), long  astprm, void*p1, long p2, long  p3, long p4, long p5, long p6) {
  int retval = 0;
  unsigned int c, d;
  struct _pcb * p=current;
  struct _irp * i;
  exe$clref(efn);
  if (chan<0 || chan>ctl$gl_chindx) return SS$_IVCHAN;
  /*ccb$l_wind stuff*/

  /*spooled?*/
  /*no access check yet*/
  /*check fdt mask*/
  /*check func code*/
  /*check iosb*/
  if (iosb) *((unsigned long long *)iosb)=0;
  int oldipl=getipl(); // unstandard and temporary measure
  setipl(IPL$_ASTDEL);
  /*check proc quota*/
  i=kmalloc(sizeof(struct _irp),GFP_KERNEL);
  bzero(i,sizeof(struct _irp));
  i->irp$b_type=DYN$C_IRP;
  i->irp$b_efn=efn;
  i->irp$l_ast=astadr;
  i->irp$l_astprm=astprm;
  i->irp$l_iosb=iosb;
  i->irp$w_chan=chan;
  i->irp$l_func=func;
  i->irp$b_pri=p->pcb$b_pri;
  i->irp$l_qio_p1=p1;
  i->irp$l_qio_p2=p2;
  i->irp$l_qio_p3=p3;
  i->irp$l_qio_p4=p4;
  i->irp$l_qio_p5=p5;
  i->irp$l_qio_p6=p6;
  i->irp$l_ucb=ctl$gl_ccbbase[chan].ccb$l_ucb;
  i->irp$l_pid=current->pcb$l_pid;
  i->irp$l_sts|=IRP$M_BUFIO; /* no DIRIO because of no mmg$svaptechk */
  /* do preprocessing */
  /* does it do one or more functions */
  //  for(c=0,d=1;c<64;c++,d=d<1) /* right order? */
  //  if (d&func) {
  c=i->irp$v_fcode;
  d=i->irp$v_fmod;
  if (0==ctl$ga_ccb_table[chan].ccb$l_ucb->ucb$l_ddt->ddt$l_fdt->fdt$ps_func_rtn[i->irp$v_fcode]) {
    retval = SS$_ILLIOFUNC;
    goto earlyerror;
  }
  retval = ctl$ga_ccb_table[chan].ccb$l_ucb->ucb$l_ddt->ddt$l_fdt->fdt$ps_func_rtn[i->irp$v_fcode](i,p,i->irp$l_ucb,&ctl$gl_ccbbase[chan]); // a real beauty, isn't it :)
  setipl(oldipl); // temporary measure. should be 0 again?
  return retval;
      //  }
 earlyerror:
  setipl(oldipl);
  sch$postef(current->pcb$l_pid,PRI$_NULL,efn);
  return retval;
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
#ifdef CONFIG_VMS
  if (a->aqb$l_acppid==1) {
    exe$qioqe2ppkt(p,i);
    return SS$_NORMAL;
  }
#endif
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

extern exttwo$dispatch();

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

#ifdef CONFIG_VMS
void exe$qioqe2ppkt (struct _pcb * p, struct _irp * i) {
  struct _acb *a=&i->irp$l_fqfl;
  //  struct _f11b * f=ctl$gl_f11bxqp;

  a->acb$l_pid=p->pcb$l_pid;
  a->acb$l_ast=exttwo$dispatch;
  a->acb$l_astprm=i;
  a->acb$b_rmod|=ACB$M_NODELETE; // bad idea to free this
  remque(i,0); // got to get rid of this somewhere, why not here?
  sch$qast(p->pcb$l_pid,PRI$_RESAVL,a);
}
#endif

asmlinkage int exe$qiow_wrap(struct struct_qio * s) {
  return exe$qiow(s->efn,s->chan,s->func,s->iosb,s->astadr,s->astprm,s->p1,s->p2,s->p3,s->p4,s->p5,s->p6);
}

asmlinkage int exe$qio_wrap(struct struct_qio * s) {
  return exe$qio(s->efn,s->chan,s->func,s->iosb,s->astadr,s->astprm,s->p1,s->p2,s->p3,s->p4,s->p5,s->p6);
}

