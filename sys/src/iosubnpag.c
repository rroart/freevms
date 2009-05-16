// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file iosubnpag.c
   \brief QIO - TODO still more doc
   \author Roar Thronæs
*/

#include <linux/kernel.h>
#include <asmlink.h>
#include <linux/smp.h>
#include <linux/sched.h>
#include <asm/hw_irq.h>
#include <asm/current.h>
#include <system_data_cells.h>
#include <ipl.h>
#include <internals.h>
#include<queue.h>
#include<ddtdef.h>
#include<ucbdef.h>
#include<irpdef.h>
#include<ipldef.h>
#include<ddbdef.h>
#include <ioc_routines.h>
#include <smp_routines.h>

#include <linux/sched.h>

#undef MYDEB_REQCOM
#define MYDEB_REQCOM

/**
   \brief initiate i/o for unit's irp - see 5.2 22.2.1
   \param i irp
   \param u unit
*/

void ioc$initiate(struct _irp * i, struct _ucb * u) {
  struct _ddt *d; 
  void (*f)(void *,void *);
  /** smp affinity check yet - MISSING */
  /** save irp address in unit */
  u->ucb$l_irp=i;

  /** copy some irp content to unit */
  u->ucb$l_svapte=i->irp$l_svapte;
  u->ucb$l_boff=i->irp$l_boff;
  u->ucb$l_bcnt=i->irp$l_bcnt;

  /** clear TIMOUT and CANCEL */
  u->ucb$l_sts&=~(UCB$M_TIMOUT|UCB$M_CANCEL);

  /** diagnostic buf - MISSING */

  /** locate ddt$l_start */
  d=u->ucb$l_ddt;
  f=d->ddt$l_start;
  /** call it */
  f(i,u);
  
}

void ioc_std$initiate(struct _irp * i, struct _ucb * u) {
  ioc$initiate(i,u);
}

extern int exetimeout;

#ifdef MYDEB_REQCOM
long reqcom[32*1024];
long reqcomc[32]={ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#endif

/**
   \brief i/o request complete - see 5.2 22.3.4
   \param iosb1 value status
   \param iosb2 value status
   \param u ucb
*/

void ioc$reqcom(int iosb1, int iosb2, struct _ucb * u) {
  int qemp;
  int savipl;
  struct _irp * i=u->ucb$l_irp;
#ifdef MYDEB_REQCOM
  {
    long addr = &iosb1;
    addr -= 0x4;
    int pid=i->irp$l_pid&31;
    reqcom[1024*pid+reqcomc[pid]]=i;
    reqcomc[pid]++;
    reqcom[1024*pid+reqcomc[pid]]=i->irp$w_empty; //->irp$l_qio_p3;
    reqcomc[pid]++;
    reqcom[1024*pid+reqcomc[pid]]=((long*)addr)[0];
    reqcomc[pid]++;
    reqcom[1024*pid+reqcomc[pid]]=exe$gl_abstim_tics;// |(myindex_<<16); //i;
    reqcomc[pid]++;
    if (reqcomc[pid]>1000)
      reqcomc[pid]=0;
  }
#endif
  /** if error log do error stuff and erl$releasemb - MISSING */

  /** increase ucb i/o count - MISSING */

  /** store final i/o status values */
  i->irp$l_iost1=iosb1; /* ok? */
  i->irp$l_iost2=iosb2;

  /** if tape device ... - MISSING */

  /** if error completion and is disk or tape, check mount verification - MISSING */

  /** some more work if tape errors - MISSING */

  //  exetimeout=0;
  //    { int i,j; for(j=0;j<20;j++) for(i=0;i<1000000000;i++); }
  //printk("reqcom %x\n",i);
  //if (i==0) { int i,j; for(j=0;j<200;j++) for(i=0;i<10000000;i++); }
  if (i==0) {
    printk("i should not be 0\n");
    goto end;
  }

  /** test and save current ipl, and maybe raise ipl */

  savipl=getipl();
  if (savipl<IPL$_IOPOST)
    setipl(IPL$_IOPOST);
  
  /** spinlock, interlocking */

  vmslock(&SPIN_IOPOST, -1);

  /** insert irp in the interlocked systemwide i/o postprocessing queue */
  qemp=rqempty(&ioc$gq_postiq);
  insqti(i,&ioc$gq_postiq);

  /** spinunlock */

  vmsunlock(&SPIN_IOPOST, -1);
#if 0
  long * l2=&ioc$gq_postiq;
  long l=((long)l2)+(*l2);
  if (i==l)
    goto skip_it;
  insqti(i,&ioc$gq_postiq);
 skip_it:
#endif
  if (!qemp) goto notempty;

  /** if first element, do */

  /** if primary cpu, request iopost softint */
  if (smp_processor_id() == smp$gl_primid) {
    SOFTINT_IOPOST_VECTOR;
  } else {
    /** send interprocessor interrupt to do it on primary cpu */
    smp_send_work(CPU$M_IOPOST, 0);
    /* request interprocessor interrupt */
  }
 notempty:
  {
  }

  /** restore saved ipl */
  setipl(savipl);

  /** if mount verification ... - MISSING */

  /** if the unit has pending i/o requests */
  qemp=aqempty(u->ucb$l_ioqfl);
  if (qemp) goto end;
  //printk("ioq %x %x",i,u->ucb$l_ioqfl);
  /** remove one from queue */
  i=remque(u->ucb$l_ioqfl,i);
  //printk("ioq %x %x",i,u->ucb$l_ioqfl);
  /** initiate i/o on it */
  return ioc$initiate(i,u);
 end:
  /** if no pending i/o request, clear bsy flag */
  if (aqempty(u->ucb$l_ioqfl))
    u->ucb$l_sts&=~UCB$M_BSY;
  /** enter ioc$relchan - MISSING */
  //printk("end of reqcom\n");
}

void ioc_std$reqcom(int iosb1, int iosb2, struct _ucb * u) {
  return(ioc$reqcom(iosb1,iosb2,u));
}

/**
   \brief waiting for the device interrupt - see 5.2 22.3.2
   \param nextfunc
   \param timeoutfunc
   \param i irp
   \param fr4
   \param u
   \param timeout
   \param oldipl
*/

void ioc$wfikpch(void * nextfunc, void * timeoutfunc, struct _irp * i, unsigned long fr4, struct _ucb * u, int timeout, int oldipl) {
  /* should also have had return addresses caller and caller's caller as params? */
  /* must have them... */
  u->ucb$l_sts|=UCB$M_INT|UCB$M_TIM;
  /** store timout + abstim in duetim */
  u->ucb$l_duetim=exe$gl_abstim+timeout;
  /** store in fr3 and fr4 TODO: check if right */
  u->ucb$l_fr3=i;
  u->ucb$l_fr4=fr4;
  /** store return address nextfunc in fpc, no need to adjust */
  u->ucb$l_fpc=nextfunc;
  /** clear timout */
  u->ucb$l_sts&=~UCB$M_TIMOUT;
  /** release device lock - MISSING TODO check if need be here */
}

int ioc$scan_iodb_usrctx(struct _ddb **d) {
  if ((*d)->ddb$ps_link) 
    (*d)=(*d)->ddb$ps_link;
  else
    *d=0;
}

int ioc$cvt_devnam() {

}

int exe$match_name(struct _ddb * d, char * s) {

}

void ioc_std$cancelio (signed int chan, struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb) {
  if (ucb->ucb$l_sts&UCB$M_BSY && irp->irp$l_pid==pcb->pcb$l_pid && (-chan)==irp->irp$w_chan) {
    ucb->ucb$l_sts|=UCB$M_CANCEL; // check. had to add extra - before chan
  }
}

int ioc$ctrl_init (struct _crb *crb, struct _ddb *ddb) {
  struct _ddt * ddt = ddb->ddb$l_ddt;
  int (*ctrlinit_2)() = ddt->ddt$ps_ctrlinit_2;
  if (ctrlinit_2)
    ctrlinit_2(0 /* idb */, ddb, crb);
}

int ioc$unit_init (struct _ucb *ucb) {
  struct _ddt * ddt = ucb->ucb$l_ddt;
  int (*unitinit)() = ddt->ddt$l_unitinit;
  if (unitinit)
    unitinit(0, ucb);
  struct _ddb * ddb = ucb->ucb$l_ddb;
  struct _dpt * dpt = ddb->ddb$ps_dpt;
  void (*init)() = dpt->dpt$ps_init_pd;
  void (*reinit)() = dpt->dpt$ps_reinit_pd;
  long orb = 0;
  long idb = 0;
  long crb = 0;
  struct _ddb * dbb = ucb->ucb$l_ddb;
  if (init)
    init(crb, ddb, idb, orb, ucb);
  if (reinit)
    reinit(crb, ddb, idb, orb, ucb);
}
