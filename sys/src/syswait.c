// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file syswait.c
   \brief wait for event
   \author Roar Thronæs
 */

#include <linux/sched.h>
#include <linux/smp.h>
#include <cebdef.h>
#include <cpudef.h>
#include <ipldef.h>
#include <ssdef.h>
#include <system_data_cells.h>
#include <misc_routines.h>
#include <sch_routines.h>
#include <ipl.h>
#include <internals.h>
#include <queue.h>

/**
   \brief waiting for av event flag - See 5.2 9.6
   \details Most is implemented, but not some waitqueues, due to some forgotten problems.
   Must verify that common event flags works properly.
   Also something in the top of exe$wait modifies the return stack.
   \param efn event flag number
   \param mask event flag mask
   \param waitallflag should wait for all set in mask
   \param dummy for the stack return address change
 */

int exe$wait(unsigned int efn, unsigned int mask, int waitallflag, void * dummy)
{
#ifdef __x86_64__
    __asm__ __volatile__ ("movq %%rsp,%0; ":"=r" (dummy) );
    dummy+=0x20;
#endif
    struct _pcb * p=ctl$gl_pcb;
    int efncluster=(efn&224)>>5;
    unsigned long * clusteraddr;
    struct _wqh * wq;
    /** set ipl 2 */
    setipl(IPL$_ASTDEL);
    /** legal efn check */
    if (efn>127)
        return SS$_ILLEFC;
    /** determine event flag cluster */
    clusteraddr=getefc(p,efn);
    p->pcb$b_wefc=efncluster;
    /** test if common event flag and existance */
    if (efncluster>1 && !clusteraddr)
        return SS$_UNASEFC;

    /** sched spinlock */
    vmslock(&SPIN_SCHED,IPL$_SCHED);

#if 0
    if (efncluster==0 || efncluster==1) goto notcommon;
    /* not impl */
    printk("in a wait non impl routine %x %x %x %x\n",efn,mask,clusteraddr,efncluster);
    return;
notcommon:
#endif

    /** find waitqueue */
    if (efncluster<2)
    {
        wq=sch$gq_lefwq;
        /* do */
    }
    else
    {
        unsigned long * l=getefcp(p,efn);
        wq=&((struct _ceb *)(*l))->ceb$l_wqfl;
    }

    /** if the wait condition is not satisfied */
    if (!waitallflag && (mask & *clusteraddr))
    {
out:
        /** unlock sched */
        vmsunlock(&SPIN_SCHED,-1);
        {}
#if 0
        void * dummy = &efn;
        char ** pc = dummy+0x38;
        (*pc)+=7;
#endif
        return SS$_NORMAL;
    }

    /** if the wait condition is satisfied */
    if (waitallflag && ((mask & *clusteraddr) == mask))
        goto out;

    /** handle waitall flag */
    /** TODO: check if waitall and mask is handled right */
    if (waitallflag && ((mask & *clusteraddr)))
    {
        mask &= ~(*clusteraddr);
    }

    /** handle waitall set sts flag */
    if (waitallflag)
        p->pcb$l_sts|=PCB$M_WALL; // maybe reset if this is not the case?
    p->pcb$l_efwm=~mask;

    /** TODO: check if this insque still has problems. should it be here */
#if 0
    insque(p,&wq->wqh$l_wqfl); // temporary... see about corruption in rse.c
#endif
    /** if unsatisfied, fix the return address */
    fixup_hib_pc(dummy);
    /** jump sch$wait */
    sch$wait(p,wq);
    // check. no unlock here? done in sch$sched
    return SS$_NORMAL;
}

int exe$waitfr(unsigned int efn)
{
    return exe$wait(efn,1<<(efn&31),0,&efn);
}

int exe$wflor(unsigned int efn, unsigned int mask)
{
    return exe$wait(efn,mask,0,&efn);
}

int exe$wfland(unsigned int efn, unsigned int mask)
{
    return exe$wait(efn,mask,1,&efn);
}
