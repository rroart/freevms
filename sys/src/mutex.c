// $Id$
// $Locker$

// Author. Roar Thron�s.
/**
   \file mutex.c
   \brief mutexes
   \author Roar Thron�s
*/

#include <system_data_cells.h>
#include <ipl.h>
#include <internals.h>
#include <pridef.h>
#include <wqhdef.h>
#include <linux/sched.h>
#include <ipldef.h>
#include <sch_routines.h>
#include <linux/spinlock.h>
#include <mtxdef.h>
#include <asm/bitops.h>
#include <statedef.h>
#include <queue.h>
#include <sch_routines.h>

#define ioc$gq_mutex ioc_spin
static struct _mtx ioc_spin = { 65535 /* mtx$w_sts : 1, mtx$w_owncnt : -1 */ };

// definitely needs reimplementation. this implemantion, or the bad old one? */

/**
   \brief stall process when waiting for mutex - see 5.2 8.5.3
   \param p pcb
   \param m mutex
*/

void mutexwait(struct _pcb * p, struct _mtx * m)
{
    /** store mutex address in efwm */
    p->pcb$l_efwm = m; // check. 32 bit problem
    /** new pcb state MWAIT */
    p->pcb$w_state = SCH$C_MWAIT;
    /** insert into MWAIT scheduling queue */
    insque(p,sch$aq_wqhdr[SCH$C_MWAIT].wqh$l_wqfl);
    int ipl=getipl();
    /** put on wait */
    sch$waitl(p, &sch$aq_wqhdr[SCH$C_MWAIT]);
    setipl(ipl);
}

/**
   \brief mutex for writing - see 5.2 8.5.2
   \param m mutex
*/

void sch$lockw(struct _mtx * m)
{
    struct _pcb * p = ctl$gl_pcb;
again:
    {}
    /** spinlock sched */
    int ipl = vmslock(&SPIN_SCHED, IPL$_SCHED);
    /** test and set mutex write flag */
    if (test_and_set_bit(16, m))
    {
        /** if set, no further readers or writers allowed, stall process */
        mutexwait(p, m);
        goto again;
    }
    else
    {
        /** if no other owners */
        if (m->mtx$w_owncnt==65535 /* not yet: -1 */ )
        {
            /** own it is done by incrementing owncnt and mtxcnt */
            m->mtx$w_owncnt++;
            p->pcb$w_mtxcnt++;
            /** if it is the first to be locked */
            if (p->pcb$w_mtxcnt==1)
            {
                /** then record current priorities in pcb */
                /** TODO check if saved right with regard to realtime */
                p->pcb$b_prisav = p->pcb$b_pri;
                p->pcb$b_pribsav = p->pcb$b_prib;
                /** if not realtime process, boost priority */
                if (16>=p->pcb$b_pri)
                {
                    p->pcb$b_prib = 15;
                    sch$change_cur_priority(p, 15);
                }
            }
        }
        else
        {
            /** if other owners, stall */
            mutexwait(p, m);
            goto again;
        }
    }
    /** release spinlock */
    ipl = IPL$_ASTDEL;
    vmsunlock(&SPIN_SCHED, ipl);
}

/**
   \brief mutex for reading - see 5.2 8.5.1
   \param m mutex
*/

void sch$lockr(struct _mtx * m)
{
    struct _pcb * p = ctl$gl_pcb;
again:
    {}
    /** spinlock sched */
    int ipl = vmslock(&SPIN_SCHED, IPL$_SCHED);
    /** test mutex write flag */
    if (test_bit(16, m))
    {
        /** if set, stall process until available */
        mutexwait(p, m);
        goto again;
    }
    else
    {
        /** if not set, grant readership */
        /** TODO check owncnt use here. copied from lockw? */
        if (m->mtx$w_owncnt==65535 /* not yet: -1 */ )
        {
            /** own it is done by incrementing owncnt and mtxcnt */
            m->mtx$w_owncnt++;
            p->pcb$w_mtxcnt++;
            /** if it is the first to be locked */
            if (p->pcb$w_mtxcnt==1)
            {
                /** then record current priorities in pcb */
                /** TODO check if saved right with regard to realtime */
                p->pcb$b_prisav = p->pcb$b_pri;
                p->pcb$b_pribsav = p->pcb$b_prib;
                /** if not realtime process, boost priority */
                if (16>=p->pcb$b_pri)
                {
                    p->pcb$b_prib = 15;
                    sch$change_cur_priority(p, 15);
                }
            }
        }
        else
        {
            mutexwait(p, m);
            goto again;
        }
    }
    /** release spinlock */
    ipl = IPL$_ASTDEL;
    vmsunlock(&SPIN_SCHED, ipl);
}

void sch$iolockw(void)
{
    // probably wrong?
    sch$lockw(&ioc$gq_mutex);
}

/**
   \brief unlocking a mutex - see 5.2 8.5.4
   \param m mutex
*/

void sch$unlock(struct _mtx * m)
{
    struct _pcb * p = ctl$gl_pcb;
    /** spinlock sched */
    int ipl = vmslock(&SPIN_SCHED, IPL$_SCHED);
    /** decrement mtxcnt */
    p->pcb$w_mtxcnt--;
    /** if process does not own anymore, restore pris and change pri */
    /** TODO check if all saved are restore, and TODO check realtime */
    if (p->pcb$w_mtxcnt==0)
    {
        p->pcb$b_prib = p->pcb$b_pribsav;
        sch$change_cur_priority(p, p->pcb$b_pri);
    }
    /** decrement owncnt */
    m->mtx$w_owncnt--;
    if (m->mtx$w_owncnt==65535 /* not yet: -1 */ )
    {
        /** if no other owners */
        /** bbcci, test and clear write bit */
        if (test_and_clear_bit(16, m))
        {
            /** if a writer had it */
            int rsn = m; // check 32 bit
            // pasted from ravail
            struct _wqh * wq=sch$gq_mwait;
            struct _pcb * p=wq->wqh$l_wqfl;
            struct _pcb * next;
            /** scan misc wait queue */
            for (; p!=wq; p=next)
            {
                next = p->pcb$l_sqfl;
                /** search for those waiting for the now unlocked mutex */
                if (p->pcb$l_efwm==rsn)
                {
                    /** when found */
                    /** decrease wqueue count */
                    wq->wqh$l_wqcnt--;
                    /** schedule that process */
                    sch$chse(p,PRI$_RESAVL);
                }
            }
        }
    }

out:
    /** release spinlock */
    vmsunlock(&SPIN_SCHED, ipl);
}

void sch$unlockw(struct _mtx * m)
{
    sch$unlock(m);
}

void sch$iounlockw(void)
{
    // probably wrong?
    sch$unlockw(&ioc$gq_mutex);
}

void sch$iounlock(void)
{
    // probably wrong?
    sch$unlockw(&ioc$gq_mutex);
}

/**
   \brief resource available
*/

void sch_std$ravail(int rsn)   // check. still 32 bit prob
{
    int retval=test_and_clear_bit(rsn,&sch$gl_resmask);
    if (retval==0)
        return;
    int savipl=vmslock(&SPIN_SCHED,IPL$_MAILBOX);
    struct _wqh * wq=sch$gq_mwait;
    struct _pcb * p=wq->wqh$l_wqfl;
    for (; p!=wq; p=p->pcb$l_sqfl)
    {
        if (p->pcb$l_efwm==rsn)
        {
            wq->wqh$l_wqcnt--;
            sch$chse(p,PRI$_RESAVL);
        }
    }
    vmsunlock(&SPIN_SCHED,savipl);
}

void sch$rwait(int rsn)
{
    test_and_set_bit(rsn,&sch$gl_resmask);
    return mutexwait(ctl$gl_pcb, rsn);
}
