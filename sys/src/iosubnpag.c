#include <linux/kernel.h>
#include "../../freevms/sys/src/asmlink.h"
#include <linux/smp.h>
#include <asm/hw_irq.h>
#include <asm/current.h>
#include "../../freevms/sys/src/system_data_cells.h"
#include"../../freevms/pal/src/queue.h"
#include"../../freevms/lib/src/ddtdef.h"
#include"../../freevms/lib/src/ucbdef.h"
#include"../../freevms/lib/src/irpdef.h"

void ioc$initiate(struct _irp * i, struct _ucb * u) {
  struct _ddt *d; 
  void (*f)(void *,void *);
  /* no  smp affinity check yet */
  u->ucb$l_irp=i;
  u->ucb$l_svapte=i->irp$l_svapte;
  u->ucb$w_boff=i->irp$w_boff;
  u->ucb$w_bcnt=i->irp$l_bcnt;
  u->ucb$l_sts&=~(UCB$M_TIMOUT|UCB$M_CANCEL);
  /* no diagnostic buf */
  d=u->ucb$l_ddt;
  f=d->ddt$l_start;
  f(i,u);
  
}

extern int exetimeout;

void ioc$reqcom(int iosb1, int iosb2, struct _ucb * u) {
  int qemp;
  struct _irp * i=u->ucb$l_irp;
  i->irp$l_iost1=iosb1; /* ok? */
  i->irp$l_iost2=iosb2;
  //  exetimeout=0;
  //    { int i,j; for(j=0;j<20;j++) for(i=0;i<1000000000;i++); }
  printk("reqcom %x\n",i);
  //if (i==0) { int i,j; for(j=0;j<200;j++) for(i=0;i<10000000;i++); }
  if (i==0) {
    printk("i should not be 0\n");
    goto end;
  }
  qemp=rqempty(&ioc$gq_postiq);
  insqti(i,&ioc$gq_postiq);

  if (smp_processor_id()==0) {
    SOFTINT_IOPOST_VECTOR;
  } else {
    /* request interprocessor interrupt */
    if (!qemp) goto notempty;
  notempty:
  }
  qemp=aqempty(u->ucb$l_ioqfl);
  if (qemp) goto end;
  printk("ioq %x %x",i,u->ucb$l_ioqfl);
  i=remque(u->ucb$l_ioqfl,i);
  printk("ioq %x %x",i,u->ucb$l_ioqfl);
  ioc$initiate(i,u);
 end:
  if (aqempty(u->ucb$l_ioqfl))
    u->ucb$l_sts&=~UCB$M_BSY;
  printk("end of reqcom\n");
}

void ioc$wfikpch(void * nextfunc, void * timeoutfunc, struct _irp * i, unsigned long fr4, struct _ucb * u, int timeout, int oldipl) {
  /* should also have had return addresses caller and caller's caller as params? */
  /* must have them... */
  u->ucb$l_sts|=UCB$M_INT|UCB$M_TIM;
  u->ucb$l_duetim=exe$gl_abstim+timeout;
  u->ucb$l_fr3=i;
  u->ucb$l_fr4=fr4;
  u->ucb$l_fpc=nextfunc;
  u->ucb$l_sts&=~UCB$M_TIMOUT;
  /* release spinlock */
}

