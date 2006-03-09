// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<system_data_cells.h>
#include<ipl.h>
#include<internals.h>
#include<pridef.h>
#include<wqhdef.h>
#include<linux/sched.h>
#include<ipldef.h>
#include <sch_routines.h>

// definitely needs reimplementation

void sch$lockw(spinlock_t * s) {
  vmslock(s,-1);
}

void sch$iolockw(void) {
  // probably wrong?
  sch$lockw(&ioc$gq_mutex);
}

void sch$unlockw(spinlock_t * s) {
  vmsunlock(s,-1);
}

void sch$iounlockw(void) {
  // probably wrong?
  sch$unlockw(&ioc$gq_mutex);
}

void sch$iounlock(void) {
  // probably wrong?
  sch$unlockw(&ioc$gq_mutex);
}

void sch_std$ravail(int rsn) {
  int retval=test_and_clear_bit(rsn,&sch$gl_resmask);
  int savipl=vmslock(&SPIN_SCHED,IPL$_MAILBOX);
  struct _wqh * wq=sch$gq_mwait;
  struct _pcb * p=wq->wqh$l_wqfl;
  for (;p!=wq;p=p->pcb$l_sqfl) {
    if (p->pcb$l_efwm==rsn) {
      wq->wqh$l_wqcnt--;
      sch$chse(p,PRI$_RESAVL);
    }
  }
  vmsunlock(&SPIN_SCHED,savipl);
}
