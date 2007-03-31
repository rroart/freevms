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
#include <linux/spinlock.h>
#include <mtxdef.h>
#include <asm/bitops.h>
#include <statedef.h>
#include <queue.h>
#include <sch_routines.h>

#define ioc$gq_mutex ioc_spin
static struct _mtx ioc_spin = { 65535 /* mtx$w_sts : 1, mtx$w_owncnt : -1 */ };

// definitely needs reimplementation

void mutexwait(struct _pcb * p, struct _mtx * m) {
  p->pcb$l_efwm = m; // check. 32 bit problem
  p->pcb$w_state = SCH$C_MWAIT;
  insque(p,sch$aq_wqhdr[SCH$C_MWAIT].wqh$l_wqfl);
  int ipl=getipl();
  sch$waitl(p, &sch$aq_wqhdr[SCH$C_MWAIT]);
  setipl(ipl);
}  

void sch$lockw(struct _mtx * m) {
  struct _pcb * p = ctl$gl_pcb;
 again:
  {}
  int ipl = vmslock(&SPIN_SCHED, IPL$_SCHED);
  if (test_and_set_bit(16, m)) {
    mutexwait(p, m);
    goto again;
  } else {
    if (m->mtx$w_owncnt==65535 /* not yet: -1 */ ) {
      m->mtx$w_owncnt++;
      p->pcb$w_mtxcnt++;
      if (p->pcb$w_mtxcnt==1) {
	p->pcb$b_prisav = p->pcb$b_pri;
	p->pcb$b_pribsav = p->pcb$b_prib;
	if (16>=p->pcb$b_pri) { 
	  p->pcb$b_prib = 15;
	  sch$change_cur_priority(p, 15);
	}
      }
    } else {
      mutexwait(p, m);
      goto again;
    }
  }
  ipl = IPL$_ASTDEL;
  vmsunlock(&SPIN_SCHED, ipl);
}

void sch$lockr(struct _mtx * m) {
  struct _pcb * p = ctl$gl_pcb;
 again:
  {}
  int ipl = vmslock(&SPIN_SCHED, IPL$_SCHED);
  if (test_bit(16, m)) {
    mutexwait(p, m);
    goto again;
  } else {
    if (m->mtx$w_owncnt==65535 /* not yet: -1 */ ) {
      m->mtx$w_owncnt++;
      p->pcb$w_mtxcnt++;
      if (p->pcb$w_mtxcnt==1) {
	p->pcb$b_prisav = p->pcb$b_pri;
	p->pcb$b_pribsav = p->pcb$b_prib;
	if (16>=p->pcb$b_pri) { 
	  p->pcb$b_prib = 15;
	  sch$change_cur_priority(p, 15);
	}
      }
    } else {
      mutexwait(p, m);
      goto again;
    }
  }
  ipl = IPL$_ASTDEL;
  vmsunlock(&SPIN_SCHED, ipl);
}

void sch$iolockw(void) {
  // probably wrong?
  sch$lockw(&ioc$gq_mutex);
}

void sch$unlock(struct _mtx * m) {
  struct _pcb * p = ctl$gl_pcb;
  int ipl = vmslock(&SPIN_SCHED, IPL$_SCHED);
  p->pcb$w_mtxcnt--;
  if (p->pcb$w_mtxcnt==0) {
    p->pcb$b_prib = p->pcb$b_pribsav;
    sch$change_cur_priority(p, p->pcb$b_pri);
  }
  m->mtx$w_owncnt--;
  if (m->mtx$w_owncnt==65535 /* not yet: -1 */ ) {
    if (test_and_clear_bit(16, m)) {
      int rsn = m; // check 32 bit
      // pasted from ravail
      struct _wqh * wq=sch$gq_mwait;
      struct _pcb * p=wq->wqh$l_wqfl;
      struct _pcb * next;
      for (; p!=wq; p=next) {
	next = p->pcb$l_sqfl;
	if (p->pcb$l_efwm==rsn) {
	  wq->wqh$l_wqcnt--;
	  sch$chse(p,PRI$_RESAVL);
	}
      }
    }
  }

 out:
  vmsunlock(&SPIN_SCHED, ipl);
}

void sch$unlockw(struct _mtx * m) {
  sch$unlock(m);
}

void sch$iounlockw(void) {
  // probably wrong?
  sch$unlockw(&ioc$gq_mutex);
}

void sch$iounlock(void) {
  // probably wrong?
  sch$unlockw(&ioc$gq_mutex);
}

void sch_std$ravail(int rsn) { // check. still 32 bit prob
  int retval=test_and_clear_bit(rsn,&sch$gl_resmask);
  if (retval==0)
    return;
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

void sch$rwait(int rsn) {
  test_and_set_bit(rsn,&sch$gl_resmask);
  return mutexwait(ctl$gl_pcb, rsn);
}
