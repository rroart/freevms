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

void ioc$initiate(struct _ucb * u, struct _irp * i) {
  struct _ddt *d; 
  void (*f)(void);
  /* no  smp affinity check yet */
  u->ucb$l_irp=i;
  u->ucb$l_svapte=i->irp$l_svapte;
  u->ucb$w_boff=i->irp$w_boff;
  u->ucb$w_bcnt=i->irp$w_bcnt;
  u->ucb$l_sts&=~(UCB$M_TIMOUT|UCB$M_CANCEL);
  /* no diagnostic buf */
  d=u->ucb$l_ddt;
  f=d->ddt$l_start;
  f();
  
}

void ioc$reqcom(void) {
  int qemp;

  qemp=rqempty(ioc$gq_postiq);
  insqti(0,ioc$gq_postiq);
  if (!qemp) goto notempty;

  if (smp_processor_id()==0) {
    SOFTINT_IOPOST_VECTOR;
  } else {
    /* request interprocessor interrupt */
  }

 notempty:
}

void ioc$wfikpch(struct _ucb * u, struct _irp *i, int newipl, int timeout) {
  /* should also have had return addresses caller and caller's caller as params? */
  /* must have them... */
  u->ucb$l_sts|=UCB$M_INT|UCB$M_TIM;
  u->ucb$l_duetim=exe$gl_abstim+timeout;
  u->ucb$l_fr3=i;
  u->ucb$l_fr4=current;
  u->ucb$l_sts&=~UCB$M_TIMOUT;
  /* release spinlock */
  
}

