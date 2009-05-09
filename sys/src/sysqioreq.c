// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file sysqioreq.c
   \brief QIO - TODO still more doc
   \author Roar Thronæs
*/

#include<linux/spinlock.h>
#include<asm/unistd.h>

#include<acbdef.h>
#include<aqbdef.h>
#include<cdrpdef.h>
#include<ddtdef.h>
#include<dyndef.h>
#include<fdtdef.h>
#include<ipl.h>
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
#include <exe_routines.h>
#include <misc_routines.h>
#include <sch_routines.h>
#include <ioc_routines.h>
#include <queue.h>
#include <linux/slab.h>

#define MYDEB_IRP
#undef MYDEB_IRP

void exe$qioqe2ppkt (struct _pcb * p, struct _irp * i);
void exe$qioqxqppkt (struct _pcb * p, struct _irp * i);

// temporary stuff with syscalls

// extern int kernel_errno;

/**
   \brief insert irp in units queue - see 5.2 22.2.1
   \param u unit cb
   \param i irp
*/

void exe$insertirp(struct _ucb * u, struct _irp * i) {
  //  struct _irp *tmp=((struct _irp *)u)->irp$l_ioqfl; // change to ucb$l_ioqfl
  struct _irp *head=&u->ucb$l_ioqfl;
  struct _irp *tmp=u->ucb$l_ioqfl;
  /** find right pri */
  while (tmp!=head && i->irp$b_pri>tmp->irp$b_pri)
    tmp=tmp->irp$l_ioqfl;
  /** insert at that pri */
  insque(i,tmp);
}

/**
   \brief abort i/o operation - see 5.2 21.6.3.2
   \param i irp
   \param p pcb
   \param u ucb
   \param qio_sts
*/

int exe_std$abortio(struct _irp * i, struct _pcb * p, struct _ucb * u, int qio_sts) {
  /** acquire forklock etc */
  int savipl = forklock(u->ucb$b_flck, u->ucb$b_flck);
#if 0
  if (i->irp$l_iosb)
    bzero(i->irp$l_iosb,8);
#endif
  /** clear iosb */
  i->irp$l_iosb=0;
  /** inc process ast quota if v_quota was set - MISSING */
  i->irp$b_rmod&=~ACB$M_QUOTA;

  /** insert irp in cpu's postprocessing queue */
  insque(i,smp$gl_cpu_data[smp$gl_primid /* smp_processor_id() */]->cpu$l_psbl); // check. bug?
  /** request iopost softint */
  SOFTINT_IOPOST_VECTOR;

  /** release forklock */
  forkunlock(u->ucb$b_flck, savipl);
  /** set ipl 0 - MISSING */
  return qio_sts;
}

void exe$altqueuepkt (void) {}

/**
   \brief entering the driver's alternate start i/o routine - see 5.2 22.2.2
   \param i irp
   \param p pcb
   \param u ucb
*/

int exe$altquepkt (struct _irp * i, struct _pcb * p, struct _ucb * u) {
  struct _ddt *d; 
  void (*f)(void *,void *);
  /** forklock */
  int savipl=forklock(u->ucb$b_flck,u->ucb$b_flck);
  /** smp stuff - MISSING */

  /** locate ddt$l_altstart */
  d=u->ucb$l_ddt;
  f=d->ddt$l_altstart;
  /** call it */
  f(i,u);

  /** unlock fork */
  forkunlock(u->ucb$b_flck,savipl);
  return SS$_NORMAL;
}

int exe$finishio (long status1, long status2, struct _irp * i, struct _pcb * p, struct _ucb * u) {
  /* do iosb stuff */
  i->irp$l_iost1=status1;
  i->irp$l_iost2=status2;
  int savipl = forklock(u->ucb$b_flck, u->ucb$b_flck);
  insque(i,smp$gl_cpu_data[smp$gl_primid /* smp_processor_id() */]->cpu$l_psbl); // check. bug?
  SOFTINT_IOPOST_VECTOR;
  forkunlock(u->ucb$b_flck, savipl);
  setipl(0);
  return SS$_NORMAL;
}

int exe$finishioc (long status, struct _irp * i, struct _pcb * p, struct _ucb * u) {
  return exe$finishio(status,0,i,p,u);
}

/**
   \brief insert irp in units queue - see 5.2 22.2.1
   \param i irp
   \param u ucb unit
*/

int exe$insioq (struct _irp * i, struct _ucb * u) {
  /** forklock and raise ipl */
  int savipl=forklock(u->ucb$b_flck, u->ucb$b_flck);

  /** if unit busy, just insert */
  if (u->ucb$l_sts & UCB$M_BSY)
    exe$insertirp(u,i); // should be &u->ucb$l_ioqfl ?
  else {
    /** else mark busy and initiate i/o */
    u->ucb$l_sts|=UCB$M_BSY;
    ioc$initiate(i,u);
  }
  /** restore ipl */
  /** release fork lock */
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

#ifdef MYDEB_IRP
long lastirp[32];
long lastirp1[32*20];
long lastirp2[32*6];
#if 1
long myirp[1024*6];
long myirpc=0;
#endif

static int qcnt = 0;
#endif

/* put this into a struct */

/**
   \brief initiating device action - see 5.2 21.6.1
   \param efn event flag
   \param chan i/o channel
   \param func function
   \param iosb i/o status block
   \param astadr ast routine
   \param astprm ast parameter
   \param p1-p6
*/

asmlinkage int exe$qio (unsigned int efn, unsigned short int chan,unsigned int func, struct _iosb *iosb, void(*astadr)(__unknown_params), long  astprm, void*p1, long p2, long  p3, long p4, long p5, long p6) {
  int retval = 0;
#if 0
  if (chan>1000)
    panic("chan 1000 %x\n",chan);
#endif
  unsigned int c, d;
  struct _pcb * p=current;
  struct _irp * i;
  /** clear event flag */
  exe$clref(efn);

  /** verify channel number TODO redo */
  if (chan<0 || chan>ctl$gl_chindx) return SS$_IVCHAN;

  /** access mode check - MISSING */

  /** access or deaccess pending in ccb$l_wind stuff - MISSING */

  /** device spooled stuff - MISSING */

  /** access check verify - MISSING */

  /** check fdt mask to get buffer or whatever type - MISSING TODO check */

  /** check func code legality - MISSING */

  /** some check for offline device - MISSING */

  /** if iosb nonzero check iosb writeable - TODO redo */
  if (iosb) *((unsigned long long *)iosb)=0; // check. use sizeof later

  /** raise ipl */
  int oldipl=getipl(); // unstandard and temporary measure
  setipl(IPL$_ASTDEL);

  /** check proc quota - MISSING */

  /** allocate irp */
  i=kmalloc(sizeof(struct _irp),GFP_KERNEL);
  memset(i,0,sizeof(struct _irp));

  /** initialize irp */
  i->irp$b_type=DYN$C_IRP;
  i->irp$b_efn=efn;
  i->irp$b_rmod=p->psl_prv_mod;
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
  /** do not set bufio if function code does not say buffered - MISSING */
#ifdef MYDEB_IRP
  i->irp$w_empty=qcnt++;
  lastirp[i->irp$l_pid&31]=i;
#if 1
  memcpy(&lastirp1[20*(i->irp$l_pid&31)],i,20*4);
  memcpy(&lastirp2[6*(i->irp$l_pid&31)],&i->irp$l_qio_p1,6*4);
  memcpy(&lastirp2[6*(i->irp$l_pid&31)+5],&exe$gl_abstim_tics,4);
#endif
#if 1
  memcpy(&myirp[myirpc],i,20*4);
  myirpc+=20;
  memcpy(&myirp[myirpc],&i->irp$l_qio_p1,6*4);
  memcpy(&myirp[myirpc+5],&exe$gl_abstim_tics,4);
  myirpc+=8;
  if (myirpc>1024) myirpc=0;
#endif
#endif
  /** set quota bit - MISSING */
  /** if astadr nonzero charge quota - MISSING */
  /** if ccb$l_wind nonzero store in irp - MISSING */
  /** ccb increase outstanding i/o count - MISSING */
  /** some more device access checks - MISSING */
  /** diagnostic buffer - MISSING */
  /** do preprocessing - MISSING TODO check it */
  /** TODO does it do one or more functions */
  //  for(c=0,d=1;c<64;c++,d=d<1) /* right order? */
  //  if (d&func) {
  c=i->irp$v_fcode;
  d=i->irp$v_fmod;
  if (0==ctl$ga_ccb_table[chan].ccb$l_ucb->ucb$l_ddt->ddt$l_fdt->fdt$ps_func_rtn[i->irp$v_fcode]) {
    retval = SS$_ILLIOFUNC;
    goto earlyerror;
  }
  /** call i/o function */
  retval = ctl$ga_ccb_table[chan].ccb$l_ucb->ucb$l_ddt->ddt$l_fdt->fdt$ps_func_rtn[i->irp$v_fcode](i,p,i->irp$l_ucb,&ctl$gl_ccbbase[chan]); // a real beauty, isn't it :)
  setipl(oldipl); // temporary measure. should be 0 again?
  return retval;
      //  }
 earlyerror:
  setipl(oldipl);
  /** invoke event flag poster */
  sch$postef(current->pcb$l_pid,PRI$_NULL,efn);
  return retval;
  /** if error, do abortio - MISSING */
}

/**
   \brief initiating file system i/o - see 5.2 22.2.3
   \param i irp
   \param p pcb
   \param u ucb unit
*/

int exe$qioacppkt (struct _irp * i, struct _pcb * p, struct _ucb * u) {
  int wasempty;
  /** locate vcb */
  struct _vcb * v=u->ucb$l_vcb;
  /** locate aqb */
  struct _aqb * a=v->vcb$l_aqb;
  /** if pid zero use XQP */
  if (a->aqb$l_acppid==0) {
    exe$qioqxqppkt(p,i);
    return SS$_NORMAL;
  }
  /** one temporary FreeVMS hacks in the if for ext2 */
#ifdef CONFIG_VMS
  if (a->aqb$l_acppid==1) {
    exe$qioqe2ppkt(p,i);
    return SS$_NORMAL;
  }
#endif
  wasempty=aqempty(&a->aqb$l_acpqfl);
  /** insert irp into aqbs i/o queue */
  insque(i,&a->aqb$l_acpqfl);
  /** return if it was not empty before insert */
  if (!wasempty) return SS$_NORMAL;
  /** spinlock and unlock TODO specified where? */
  vmslock(&SPIN_SCHED,IPL$_SCHED);
  /** invoke wakeup */
  sch$wake(a->aqb$l_acppid);
  vmsunlock(&SPIN_SCHED,-1);
  /** TODO check ipl 0 */
  setipl(0);
  /** TODO return val from sch$wake */
  return SS$_NORMAL;
}

/**
   \brief entering the driver's start i/o routine - see 5.2 22.2.1
   \param i irp
   \param p pcb
   \param u ucb
*/

int exe$qiodrvpkt (struct _irp * i, struct _pcb * p, struct _ucb * u) {
  /** insert irp in ucb */
  exe$insioq(i,u);
  /** setipl 0 */
  setipl(0);
  return SS$_NORMAL;
}

int exe_std$qiodrvpkt (struct _irp * i, struct _ucb * u) {
  return exe$qiodrvpkt(i,0,u);
}

extern f11b$dispatch();

extern exttwo$dispatch();

/**
   \brief initiating file system i/o for XQP ODS2 - see 5.2 22.2.3
   \param i irp
   \param p pcb
*/

void exe$qioqxqppkt (struct _pcb * p, struct _irp * i) {
  struct _acb *a=&i->irp$l_fqfl;
  /** getting the xqp data area is not used anymore? */
  //  struct _f11b * f=ctl$gl_f11bxqp;

  /** get address of dispatch routine and fill out acb */
  a->acb$l_pid=p->pcb$l_pid;
  a->acb$l_ast=f11b$dispatch;
  a->acb$l_astprm=i;
  a->acb$b_rmod|=ACB$M_NODELETE; // bad idea to free this
#if 0
  remque(i,0); // got to get rid of this somewhere, why not here?
#endif
  /** call sch$qast to queue ast */
  sch$qast(p->pcb$l_pid,PRI$_RESAVL,a);
  /** set ipl 0 */
  setipl(0);
}

/**
   \brief initiating file system i/o for XQP(?) EXT2
   \param i irp
   \param p pcb
*/

#ifdef CONFIG_VMS
void exe$qioqe2ppkt (struct _pcb * p, struct _irp * i) {
  struct _acb *a=&i->irp$l_fqfl;
  /** getting the xqp data area is not used anymore? */
  //  struct _f11b * f=ctl$gl_f11bxqp;

  /** get address of dispatch routine and fill out acb */
  a->acb$l_pid=p->pcb$l_pid;
  a->acb$l_ast=exttwo$dispatch;
  a->acb$l_astprm=i;
  a->acb$b_rmod|=ACB$M_NODELETE; // bad idea to free this
#if 0
  remque(i,0); // got to get rid of this somewhere, why not here?
#endif
  /** call sch$qast to queue ast */
  sch$qast(p->pcb$l_pid,PRI$_RESAVL,a);
  /** set ipl 0 - MISSING? TODO or why not? */
}
#endif

asmlinkage int exe$qiow_wrap(struct struct_qio * s) {
  return exe$qiow(s->efn,s->chan,s->func,s->iosb,s->astadr,s->astprm,s->p1,s->p2,s->p3,s->p4,s->p5,s->p6);
}

asmlinkage int exe$qio_wrap(struct struct_qio * s) {
  return exe$qio(s->efn,s->chan,s->func,s->iosb,s->astadr,s->astprm,s->p1,s->p2,s->p3,s->p4,s->p5,s->p6);
}

