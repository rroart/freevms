// $Id$
// $Locker$

// Author. Roar Thron�s.
/**
 \file sysschevt.c
 \brief system scheduling and timer
 \author Roar Thron�s
 */

#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <linux/smp.h>
#include <linux/slab.h>

#include <exe_routines.h>
#include <ssdef.h>
#include <tqedef.h>
#include <cpudef.h>
#include <dyndef.h>
#include <system_data_cells.h>
#include <ipl.h>
#include <internals.h>
#include <statedef.h>

/**
 \brief set timer - see 5.2 11.6.1
 \param efn event flag
 \param daytim time
 \param astadr ast routine
 \param reqidt param for it
 \param flags
 */

asmlinkage int exe$setimr(unsigned int efn, struct _generic_64 *daytim, void (*astadr)(long), unsigned long reqidt,
        unsigned int flags)
{
    UINT64 time;
    /** test time location - MISSING */
    /** test astcnt - MISSING */
    /** decrement jib tqcnt, test and maybe MWAIT - MISSING */
    /** allocate tqe */
    struct _tqe * t = kmalloc(sizeof(struct _tqe), GFP_KERNEL);
    struct _cpu * cpu = smp$gl_cpu_data[smp_processor_id()];
    memset(t, 0, sizeof(struct _tqe));
    t->tqe$b_type = DYN$C_TQE;
    /** clear event flag */
    exe$clref(efn);
    /** fifth nonzero argument test */
    /** wrong use TODO use chk_cputim field */
    if (flags)
    {
        t->tqe$b_rqtype = TQE$M_CHK_CPUTIM;
    }
    if ((unsigned long) daytim < 4096)
    {
        return SS$_ACCVIO;
    }
    /** if negative, then relative time */
    /** set absolute bit - MISSING */
    if (daytim->gen64$r_quad_overlay.gen64$q_quadword < 0)
    {
        time = exe$gq_systime - daytim->gen64$r_quad_overlay.gen64$q_quadword;
    }
    else
    {
        time = daytim->gen64$r_quad_overlay.gen64$q_quadword;
    }
    //printk("astadr %x\n",astadr);
    /** store access mode in tqe - MISSING */
    /** if no astadr, decrease astcnt and set quota bit - MISSING */
    t->tqe$l_ast = astadr;
    //  printk("astadr2 %x\n",t->tqe$l_fpc);
    t->tqe$l_astprm = reqidt;
    t->tqe$q_time = time;
    t->tqe$b_rqtype = TQE$C_TMSNGL;
    t->tqe$l_pid = ctl$gl_pcb->pcb$l_pid;
    t->tqe$b_efn = efn;
    //printast((struct _acb * )t);
    /** insert in timer queue */
    exe$instimq(t);
    return SS$_NORMAL;
}

/**
 \brief schedule wakeup - see 5.2 11.6.3
 \param pidadr address of pid
 \param prcnam process name
 \param daytim time of day
 \param reptim time to repeat
 */

asmlinkage int exe$schdwk(unsigned int *pidadr, void *prcnam, struct _generic_64 * daytim, signed long long * reptim)
{
    struct _pcb * retpcb, *p;
    unsigned long retipid, retepid;
    /** invoke exe$nampid to find pcb */
    int sts = exe$nampid(current, pidadr, prcnam, &retpcb, &retipid, &retepid);
    p = retpcb;
    /** TODO better handling of sts and vmsunlock */
    vmsunlock(&SPIN_SCHED, 0);
    /** maybe cwps - MISSING */
    if (!p)
    {
        return;
    }
    else
    {
        UINT64 time;
        /** allocate and init tqe */
        struct _tqe * t = kmalloc(sizeof(struct _tqe), GFP_KERNEL);
        struct _cpu * cpu = smp$gl_cpu_data[smp_processor_id()];
        memset(t, 0, sizeof(struct _tqe));
        t->tqe$b_type = DYN$C_TQE;
        /** test if one-time or repeating */
        if (daytim->gen64$r_quad_overlay.gen64$q_quadword < 0)
        {
            /** convert repeat to system time format */
            time = exe$gq_systime - daytim->gen64$r_quad_overlay.gen64$q_quadword;
            /** if relative clear tqe absolute bit - MISSING */
        }
        else
        {
            time = daytim->gen64$r_quad_overlay.gen64$q_quadword;
        }
        t->tqe$l_pid = p->pcb$l_pid;
        t->tqe$q_time = time;
        if (reptim)
            t->tqe$q_delta = -(*reptim);
        t->tqe$b_rqtype = TQE$C_WKSNGL;
        if (reptim)
            t->tqe$b_rqtype |= TQE$M_REPEAT;
        //printast((struct _acb * )t);
        /** decrement astcnt - MISSING */
        /** insert with instimq */
        exe$instimq(t);
    }

    /** return value */
    return SS$_NORMAL;
}

