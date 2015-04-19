// $Id$
// $Locker$

// Author. Roar Thron�s.
/**
   \file comdrvsub.c
   \author Roar Thron�s
 */

#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <asm/hw_irq.h>
#include <vms_drivers.h>
#include <cpudef.h>
#include <ipldef.h>
#include <pridef.h>
#include <irpdef.h>
#include <acbdef.h>
#include <tastdef.h>
#include <ttyucbdef.h>
#include <com_routines.h>
#include <sch_routines.h>
#include <smp_routines.h>
#include <ipl.h>
#include <queue.h>

#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/sched.h>

#include <internals.h>
#include <system_data_cells.h>

void com_std$delattnastp (struct _acb **acb_lh, struct _ucb *ucb, int ipid);

void com$post(struct _irp * i, struct _ucb * u)
{
    int savipl = getipl();
    if (savipl<IPL$_IOPOST)
        setipl(IPL$_IOPOST);
    vmslock(&SPIN_IOPOST, -1);
    insqti(i,&ioc$gq_postiq);
    vmsunlock(&SPIN_IOPOST, -1);
    if (ctl$gl_pcb->pcb$l_cpu_id != smp$gl_primid)
        smp_send_work(CPU$M_IOPOST, 0);
    else
        SOFTINT_IOPOST_VECTOR;
    setipl(savipl);
}

/**
   \brief delivery of attention asts - see 5.2 7.9.2
   \param acb pointer to acb list head
   \param ucb
*/

void  com_std$delattnast (struct _acb **acb_lh, struct _ucb *ucb)
{
    com_std$delattnastp(acb_lh, ucb, 0);
}

void fork_routine(fr3,fr4,fr5)
{
    struct _acb * a = fr5;
    long * exp = &a->acb$l_kast;

    a->acb$l_ast=a->acb$l_kast;
    a->acb$l_kast=0;
    a->acb$l_astprm=exp[1];
    a->acb$b_rmod=0;
    a->acb$l_pid=exp[3];
    return sch$qast(a->acb$l_pid,PRI$_IOCOM,a);
}

void dealloc_tast(long arg)
{
    struct _tast * tast = arg;
    tast->tast$v_busy=0;
    if (tast->tast$v_lost)
        return;
    kfree(arg);
    //exe$deanonpaged();
}

void fork_routine2(fr3,fr4,fr5)
{
    struct _acb * a = fr5;
    struct _tast * tast = fr5;

    tast->tast$b_rmod|=ACB$M_PKAST|ACB$M_NODELETE;
    a->acb$l_ast=tast->tast$l_ast;
    a->acb$l_kast=dealloc_tast;
    a->acb$l_astprm=tast->tast$l_astprm;
    a->acb$b_rmod=tast->tast$b_rmod;
    a->acb$l_pid=tast->tast$l_pid;
    return sch$qast(a->acb$l_pid,PRI$_IOCOM,a);
}

/**
   \brief delivery of attention asts - see 5.2 7.9.2
   \param acb pointer to acb list head
   \param ucb
   \param ipid internal pid
*/

void com_std$delattnastp (struct _acb **acb_lh, struct _ucb *ucb, int ipid)
{
    struct _acb * next, * cur = acb_lh;
    /** scan acb/fkb list */
    while (cur->acb$l_astqfl)
    {
        next=cur->acb$l_astqfl;
        long * exp=&next->acb$l_kast;
        /** compare pid if requested, that is non-zero */
        if ((ipid == 0) || (ipid == exp[3]))
        {
            /** remove from list */
            cur->acb$l_astqfl=next->acb$l_astqfl;
            /** fork it */
            fork(fork_routine,0,0,next);
        }
        else
        {
            cur=next;
        }
    }
}

/**
   \brief delivery of out-of-band attention asts - see 5.2 7.9.5.3
   \param acb pointer to acb list head
   \param ucb
   \param ipid internal pid
   \param matchchar
   \param inclchar_p
*/

void com_std$delctrlastp (struct _acb **acb_lh, struct _ucb *ucb, int ipid, int matchchar, int *inclchar_p)
{
    /** already entered with device lock and ipl */
    struct _acb * next, * cur = acb_lh;
    /** scan tast list */
    while (cur->acb$l_astqfl)
    {
        next=cur->acb$l_astqfl;
        struct _tast * tast = next;
        /** check for tast mask - MISSING */
        if (tast->tast$l_mask)
        {
            // not yet implemented
            // skip
        }
        /** if pid nonzero, compare with tast */
        if ((ipid == 0) || (ipid == tast->tast$l_pid))
        {
            /** skip if busy */
            if (tast->tast$v_busy)
                continue;
            /** mark as busy */
            tast->tast$v_busy=1; // bbss?
            cur->acb$l_astqfl=next->acb$l_astqfl;
            next->acb$b_rmod=SPL$C_QUEUEAST;
            /** mark matchchar in ast param */
            tast->tast$l_astprm=matchchar;
            /** do fork process */
            fork(fork_routine2,0,0,next);
        }
        else
        {
            cur=next;
        }
    }
}

void com_std$delctrlast (struct _acb **acb_lh, struct _ucb *ucb, int matchchar, int *inclchar_p)
{
    com_std$delctrlastp(acb_lh, ucb, 0, matchchar, inclchar_p);
}

void com_std$drvdealmem (void *ptr)
{
    kfree(ptr); // maybe do more checks
}

/**
   \brief flushing an attention ast list - see 5.2 7.9.3
   \param pcb
   \param ucb
   \param chan i/o channel
   \param acb pointer to acb list head
*/

int com_std$flushattns (struct _pcb *pcb, struct _ucb *ucb, int chan, struct _acb **acb_lh)
{
    /** set ipl and device lock - MISSING */
    setipl(ucb->ucb$b_dipl);
    long ipid=pcb->pcb$l_pid;
    struct _acb * next, * cur = acb_lh;
    /** scan acb/fkb list */
    while (cur->acb$l_astqfl)
    {
        next=cur->acb$l_astqfl;
        long * exp=&next->acb$l_kast;
        /** if pid matches */
        if ((ipid == 0) || (ipid == exp[3]))
        {
            /** remove from queue */
            cur->acb$l_astqfl=next->acb$l_astqfl;
            kfree(next);
            /** increment ast quota - MISSING */
        }
        else
        {
            cur=next;
        }
    }
    /** set ipl and device unlock - MISSING */
    setipl(0);
    return SS$_NORMAL;
}

/**
   \brief flushing an attention ast list - see 5.2 7.9.5.4
   \param pcb
   \param ucb
   \param chan i/o channel
   \param acb pointer to acb list head
   \param mask
*/

int com_std$flushctrls (struct _pcb *pcb, struct _ucb *ucb, int chan, struct _acb **acb_lh, int *mask_p)
{
    struct _tast * tast = 0;
    /** device lock - MISSING */

    /** scan list */
    int tmp_mask=0;
    int did_modify=0;
    struct _acb * next, * cur = acb_lh;
    while (cur->acb$l_astqfl)
    {
        next=cur->acb$l_astqfl;
        struct _tast * t=((long)next)-((long)(&tast->tast$l_flink-(long)tast));
        /** compare pid */
        if (t->tast$l_pid == pcb->pcb$l_pid && t->tast$w_chan == chan)
        {
            /** remove from list */
            cur->acb$l_astqfl=next->acb$l_astqfl;
            if (t->tast$v_busy)
            {
                /** if bust mark as lost */
                t->tast$v_lost=1;
            }
            else
            {
                //kfree(next);
                /** dealloc if not busy */
                com_std$drvdealmem(t);
                /** return ast quota - MISSING */
            }
        }
        else
        {
            /** build summary mask if no pid match */
            tmp_mask|=t->tast$l_mask;
        }
        cur=next;
    }

    /** unlock device - lower down? - MISSING */

    struct _ltrm_ucb * tty = ucb;
    /** replace summary mask with the one built */
    tty->ucb$l_tl_outband=tmp_mask;

    return SS$_NORMAL;
}

void com_std$post (struct _irp *irp, struct _ucb *ucb)
{
    com$post(irp,ucb);
}

void com_std$post_nocnt (struct _irp *irp);

/**
   \brief set attention ast mechanism - see 5.2 7.9.1
   \param irp
   \param pcb
   \param ucb
   \param ccb
   \param acb pointer to acb list head
*/

int com_std$setattnast (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, struct _acb **acb_lh)
{
    short int chan=ccb-&ctl$gl_ccbbase[0];
    long * exp;
    /** if qio p1 is zero, then it is flush */
    if (irp->irp$l_qio_p1==0)
        return com_std$flushattns(pcb,ucb,chan,acb_lh);
    /** allocate expanded acb, TODO: size */
    struct _acb * acb = kmalloc(sizeof(struct _acb)+100,GFP_KERNEL);
    memset(acb, 0, sizeof(struct _acb));
    exp=&acb->acb$l_kast;
    /** copy data */
    exp[0]=irp->irp$l_qio_p1;
    exp[1]=irp->irp$l_qio_p2;
    exp[2]=chan;
    exp[3]=pcb->pcb$l_pid;
    /** device lock and set ipl - MISSING lock */
    setipl(ucb->ucb$b_dipl);
    acb->acb$b_rmod=IPL$_QUEUEAST; // really fkb$b_flck
    /** insert into list */
    acb->acb$l_astqfl=*acb_lh;
    *acb_lh=acb;
    /** device unlock and set ipl - MISSING unlock */
    setipl(0);
    return SS$_NORMAL;
}

/**
   \brief set out-of-band attention ast mechanism - see 5.2 7.9.5.2
   \param irp
   \param pcb
   \param ucb
   \param ccb
   \param acb pointer to acb list head
   \param tast_p terminal ast list
*/

int com_std$setctrlast (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, struct _acb **acb_lh,int mask, struct _tast **tast_p)
{
    struct _acb * a;
    struct _tast * tast;
    /** if qio p1 or p2 is zero, then it is flush */
    if (irp->irp$l_qio_p1==0 || irp->irp$l_qio_p2==0)
        return com_std$flushctrls (pcb, ucb, ccb->ccb$l_chan, acb_lh, mask);
    // proc_addr is p1 and mask is p2
    a = kmalloc(sizeof(struct _tast), GFP_KERNEL);
    memset(a, 0, sizeof(struct _tast));
    tast = a;
    /** set ipl device lock - MISSING */

    /** scan list */
    /** find pid and channel */
    int tmp_mask=0;
    int did_modify=0;
    struct _acb * next, * cur = acb_lh;
    while (cur->acb$l_astqfl)
    {
        next=cur->acb$l_astqfl;
        struct _tast * t=((long)next)-((long)(&tast->tast$l_flink-(long)tast));
        if (t->tast$l_pid == pcb->pcb$l_pid && t->tast$w_chan == ccb->ccb$w_chan)
        {
            if (t->tast$v_busy)
            {
                /** if tast in use mark it as lost and remove from list */
                cur->acb$l_astqfl=next->acb$l_astqfl;
                t->tast$v_lost=1;
                goto use_allocated;
            }
            else
            {
                //kfree(next);
                /** else allocate, initialize a new terminal ast */
                com_std$drvdealmem(a);
                tast=t;
                tast->tast$l_ast=irp->irp$l_qio_p1;
                tast->tast$l_astprm=irp->irp$l_qio_p2;
                tast->tast$l_mask=mask;
                did_modify=1;
                /** charge ast quota - MISSING */
            }
        }
        tmp_mask|=t->tast$l_mask;
        cur=next;
    }

use_allocated:
    if (!did_modify)
    {
        /** copy irp data etc */
        tast->tast$l_pid=pcb->pcb$l_pid;
        tast->tast$w_chan=ccb->ccb$w_chan;
        tast->tast$l_ast=irp->irp$l_qio_p1;
        tast->tast$l_astprm=irp->irp$l_qio_p2;
        tast->tast$l_mask=mask;

        /** insert tast */
        struct _acb * acb=tast;
        acb->acb$l_astqfl=*acb_lh;
        *acb_lh=acb;
    }

    /** set ipl device unlock - MISSING */

    struct _ltrm_ucb * tty = ucb;
    /** set outband */
    tty->ucb$l_tl_outband=tmp_mask;

    *tast_p=tast;

    return SS$_NORMAL;
}
