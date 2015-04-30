// $Id$
// $Locker$

// Author. Roar Thron�s.
/**
   \file sysparprc.c
   \brief system service for rescheduling
   \author Roar Thron�s
*/

#include <linux/linkage.h>
#include <linux/sched.h>
#include <asm/hw_irq.h>
#include <system_data_cells.h>
#include <ipldef.h>
#include <ipl.h>
#include <phddef.h>
#include <sch_routines.h>
#include <internals.h>

/**
   \brief reschedule system service - see 5.2 13.4.2
*/

asmlinkage void exe$resched(void)
{
    int ipl=getipl();
    struct _pcb * p;
    setipl(IPL$_SCHED);
    /** acquire sched spinlock */
    vmslock(&SPIN_SCHED,-1);

    /** record onqtime */
    p=ctl$gl_pcb;
    p->pcb$l_onqtime=exe$gl_abstim_tics;

    /** call change_cur_priority */
    sch$change_cur_priority(p, p->pcb$b_prib);

    /** rescheduling interrupt */
    SOFTINT_RESCHED_VECTOR;

    /** release sched spinlock */
    vmsunlock(&SPIN_SCHED,-1);
    setipl(ipl);
    /** return value - MISSING */
}
