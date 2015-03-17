// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file capability.c
   \brief capabilities and affinities
   \author Roar Thronæs
*/

#include <far_pointers.h>
#include <linux/sched.h>
#include <ssdef.h>
#include <cpbdef.h>
#include <ipldef.h>
#include <statedef.h>
#include <internals.h>
#include <smp_routines.h>
#include <asm/hw_irq.h>
#include <prvdef.h>
#include <exe_routines.h>
#include <starlet.h>
#include <capdef.h>

int sch$calculate_affinity(int mask)
{
    int i;
    int new_mask = 0;
    for(i=0; i<32; i++)
    {
        if ((mask & smp$gl_cpu_data[i]->cpu$l_capability) == mask)
            new_mask |= 1 << i;
    }
    return new_mask;
}

int sch$clear_affinity(unsigned int cpu_mask, struct _pcb *pcb, unsigned int flags, UINT64_PQ prev_mask_p)
{
    if (prev_mask_p)
        *prev_mask_p = pcb->pcb$l_affinity;
    pcb->pcb$l_affinity &= ~cpu_mask;
#if 0
    if (flags & CPB$M_FLAG_PERMANENT)
        pcb->pcb$l_permanent_cpu_affinity = pcb->pcb$l_affinity;
#endif
    pcb->pcb$l_current_affinity = sch$calculate_affinity(pcb->pcb$l_affinity);
    return SS$_NORMAL;
}

/**
   \brief add cpu capabilities - see 7.0 2.5
*/

int sch$add_cpu_cap(unsigned int cpu_id, unsigned int mask, UINT64_PQ prev_mask_p)
{
    if (prev_mask_p)
        *prev_mask_p = smp$gl_cpu_data[cpu_id]->cpu$l_capability;
    smp$gl_cpu_data[cpu_id]->cpu$l_capability |= mask;
    sch$al_cpu_cap[cpu_id]=smp$gl_cpu_data[cpu_id]->cpu$l_capability;
    /** increase capability sequence */
    sch$gl_capability_sequence++;
    return SS$_NORMAL;
}

/**
   \brief set process capability - see 7.0 2.5
*/

int sch$clear_capability(struct _pcb *pcb, unsigned int mask, unsigned int cpu_id, unsigned int flags, UINT64_PQ prev_mask_p)
{
    if (prev_mask_p)
        *prev_mask_p = pcb->pcb$l_capability;
    int mask2 = 1 << cpu_id;
    pcb->pcb$l_capability &= ~mask2; // check order
    return SS$_NORMAL;
}

/**
   \brief set affinity - see 7.0 2.5.2
*/

int sch$set_affinity(unsigned int cpu_mask, struct _pcb *pcb, unsigned int flags, UINT64_PQ prev_mask_p)
{
    if (prev_mask_p)
        *prev_mask_p = pcb->pcb$l_affinity;
    /** store a new value if pcb affinity (7.0 ktb affinity) */
    pcb->pcb$l_affinity |= cpu_mask;
#if 0
    if (flags & CPB$M_FLAG_PERMANENT)
        pcb->pcb$l_permanent_cpu_affinity = pcb->pcb$l_affinity;
#endif
    pcb->pcb$l_current_affinity = sch$calculate_affinity(pcb->pcb$l_affinity);
    return SS$_NORMAL;
}

/**
   \brief remove cpu capabilities - see 5.2 12.5.4 or 7.0 2.5
   \details as add, but in reverse
   \details maybe it has need for rescheduling too
*/

sch$remove_cpu_cap(unsigned int cpu_id, unsigned int mask, UINT64_PQ prev_mask_p)
{
    if (prev_mask_p)
        *prev_mask_p = smp$gl_cpu_data[cpu_id]->cpu$l_capability;
    /** clear cpu-data cpu capability bit */
    smp$gl_cpu_data[cpu_id]->cpu$l_capability &= ~mask;
    /** and clear the corresponding bit in cpu_cap table */
    sch$al_cpu_cap[cpu_id]=smp$gl_cpu_data[cpu_id]->cpu$l_capability;
    /** increment capability sequence to indicate a change */
    sch$gl_capability_sequence++;
    struct _pcb * pcb = smp$gl_cpu_data[cpu_id]->cpu$l_curpcb;
    /** get current pcb and compare its mask */
    if ((pcb->pcb$l_capability & smp$gl_cpu_data[cpu_id]->cpu$l_capability) != pcb->pcb$l_capability)
    {
        /** do resched interrupt or send one to other cpu */
        if (ctl$gl_pcb->pcb$l_cpu_id!=cpu_id)
            smp_send_work(CPU$M_RESCHED, cpu_id);
        else
            SOFTINT_RESCHED_VECTOR;
    }
    return SS$_NORMAL;
}

/**
   \brief set process capability - see 7.0 2.5
*/

int sch$set_capability(struct _pcb *pcb, unsigned int mask, unsigned int cpu_id, unsigned int flags, UINT64_PQ prev_mask_p)
{
    if (prev_mask_p)
        *prev_mask_p = pcb->pcb$l_capability;
    int mask2 = 1 << cpu_id;
    pcb->pcb$l_capability |= mask2; // check order
    return SS$_NORMAL;
}

/**
   \brief acquire affinity - see 5.2 12.5.4 or 7.0 2.5.4
 */

int sch$acquire_affinity(struct _pcb *pcb, int obsolete, int cpu_id)   // check implicit name
{
    /** use affinity_skip from global (non-7.0 member) TODO check if used */
    pcb->pcb$b_affinity_skip = sch$gl_affinity_skip;
    /** note that pcb capability is not 7.0 */
    /** set implicit affinity flag */
    pcb->pcb$l_capability |= CPB$M_IMPLICIT_AFFINITY;
    /** store cpu_id in ktb bias_cell, probably pcb affinity, TODO both? */
    pcb->pcb$l_affinity = cpu_id;
    /** if current pcb */
    if (pcb->pcb$w_state == SCH$C_CUR)
    {
        /** do resched interrupt or send one to other cpu */
        if (pcb->pcb$l_affinity != pcb->pcb$l_cpu_id)
        {
            if (ctl$gl_pcb->pcb$l_cpu_id!=pcb->pcb$l_cpu_id)
                smp_send_work(CPU$M_RESCHED, pcb->pcb$l_cpu_id);
            else
                SOFTINT_RESCHED_VECTOR;
        }
    }
    return SS$_NORMAL;
}

/**
   \brief release (acquire in reverse) affinity - see 7.0 2.5.4
 */

int sch$release_affinity(struct _pcb *pcb)   // check _implicit name
{
    /** note that pcb capability is not 7.0 */
    /** clear implicit affinity flag */
    pcb->pcb$l_capability &= ~CPB$M_IMPLICIT_AFFINITY; // check. enough?
    return SS$_NORMAL;
}

int scs_std$change_affinity( struct _ucb *ucb_p );

int sch$release_capability(struct _pcb *pcb, unsigned int mask, unsigned int cpu_id, unsigned int flags, UINT64_PQ prev_mask_p)
{
    pcb->pcb$l_capability &= ~mask;
    pcb->pcb$l_current_affinity = sch$calculate_affinity(pcb->pcb$l_capability);
    return SS$_NORMAL;
}

/**
   \brief remove cpu capabilities - see 5.2 12.5.4
*/

int sch$require_capability(struct _pcb *pcb, unsigned int mask, unsigned int cpu_id, unsigned int flags, UINT64_PQ prev_mask_p)
{
    /** acquire spinlock sched */
    int oldipl = vmslock(&SPIN_SCHED, IPL$_SCHED);
    /** save prev mask */
    if (prev_mask_p)
        *prev_mask_p = pcb->pcb$l_capability;
    /** sets the new or-ed capability */
    pcb->pcb$l_capability |= mask; // check order
#if 0
    if (*prev_mask_p != pcb->pcb$l_capability)
    {
        smp$gl_cpu_data[pcb->pcb$l_affinity]->cpu$l_hardaff++;
        smp$gl_cpu_data[cpu_id]->cpu$l_hardaff++;
    }
#endif
    pcb->pcb$l_affinity = cpu_id;
    /** if wanted, set pcb$l_permanant_capability */
    if (flags & CPB$M_FLAG_PERMANENT)
        pcb->pcb$l_permanent_cpu_affinity = pcb->pcb$l_affinity;
    /** calculate affinity */
    pcb->pcb$l_current_affinity = sch$calculate_affinity(pcb->pcb$l_capability);
    /** if running pcb */
    if (ctl$gl_pcb == pcb)
    {
        if ((pcb->pcb$l_current_affinity & (1 << pcb->pcb$l_cpu_id)) == 0)
        {
            /** if current affinity is not the cpu_id */
            if (pcb->pcb$l_affinity != pcb->pcb$l_cpu_id)
            {
                /** do resched interrupt or send one to other cpu */
                if (ctl$gl_pcb->pcb$l_cpu_id!=pcb->pcb$l_cpu_id)
                    smp_send_work(CPU$M_RESCHED, pcb->pcb$l_cpu_id);
                else
                    SOFTINT_RESCHED_VECTOR;
            }
        }
    }
    /** release spinlock sched */
    vmsunlock(&SPIN_SCHED, oldipl);
    return SS$_NORMAL;
}

static int change_bits (int bits, int select_mask, int modify_mask)
{
    int other = bits & ~select_mask;
    bits = bits & select_mask;
    bits = bits | modify_mask;
    return bits | other;
}

/**
   \brief modify cpu capabilities - see 7.0 2.5
*/

// later: CAP$K_ALL_ACTIVE_CPUS and CAP$M_FLAG_CHECK_CPU. checks.
asmlinkage int exe$cpu_capabilities (int cpu_id, struct _generic_64 *select_mask, struct _generic_64 *modify_mask, struct _generic_64 *prev_mask, struct _generic_64 *flags)
{
    int flag = 0;
    if (flags)
        flag = *(int*)flags;
    int sts;
    int priv = ctl$gl_pcb->pcb$l_priv;
    if ((select_mask || modify_mask) && (priv & PRV$M_WORLD) == 0)
        return SS$_NOPRIV;
    // check right?
    if ((select_mask || modify_mask) && (priv & PRV$M_ALTPRI) == 0)
        return SS$_NOPRIV;
    if ((select_mask || modify_mask))
    {
        int mask;
        if (flag & CAP$M_FLAG_DEFAULT_ONLY)
        {
            if (prev_mask)
                *(int*)prev_mask = sch$gl_default_cpu_cap;
            sch$gl_default_cpu_cap = change_bits (sch$gl_default_cpu_cap, *(int*)select_mask, *(int*)modify_mask);
        }
        else
        {
            int prev = smp$gl_cpu_data[cpu_id]->cpu$l_capability;
            int mask;
            mask = *(int*)select_mask & (*(int*)modify_mask);
            sch$add_cpu_cap(cpu_id, mask, 0 /*prev_mask*/);
            mask = *(int*)select_mask & ~(*(int*)modify_mask);
            sch$remove_cpu_cap(cpu_id, mask, 0 /*prev_mask*/);
            if (prev_mask)
                *(int*)prev_mask = prev;
        }
    }
    else
    {
        if (prev_mask)
            *(int*)prev_mask = smp$gl_cpu_data[cpu_id]->cpu$l_capability;
    }
    vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
    return SS$_NORMAL;
}

/**
   \brief modify process user capabilities - see 7.0 2.5
*/

// later: CAP$M_FLAG_PERMANENT
asmlinkage int exe$process_capabilities (unsigned int *pidadr, void *prcnam, struct _generic_64 *select_mask, struct _generic_64 *modify_mask, struct _generic_64 *prev_mask, struct _generic_64 *flags)
{
    int flag = 0;
    if (flags)
        flag = *(int*)flags;
    struct _pcb * retpcb;
    unsigned long ipid, epid;
    int sts;
    sts=exe$nampid(current,pidadr,prcnam,&retpcb,&ipid,&epid);
    if ((sts & 1) == 0)
        return sts;
    struct _pcb * pcb;
    pcb = retpcb;
    int priv = ctl$gl_pcb->pcb$l_priv;
#if 0
    // done in nampid
    if (pcb != ctl$gl_pcb && (select_mask || modify_mask) && (priv & PRV$M_WORLD) == 0)
        return SS$_NOPRIV;
#endif
    // check right?
    if (pcb != ctl$gl_pcb && (select_mask || modify_mask) && (priv & PRV$M_ALTPRI) == 0)
        return SS$_NOPRIV;
    if ((select_mask || modify_mask))
    {
        int mask;
        if (flag & CAP$M_FLAG_DEFAULT_ONLY)
        {
            if (prev_mask)
                *(int*)prev_mask = sch$gl_default_process_cap;
            sch$gl_default_process_cap = change_bits (sch$gl_default_process_cap, *(int*)select_mask, *(int*)modify_mask);
        }
        else
        {
            int prev = pcb->pcb$l_capability;
            int mask;
            mask = *(int*)select_mask & ~(*(int*)modify_mask);
#if 0
            sch$release_capability(pcb, mask, 0, 0 /*prev_mask*/);
#else
            /** call clear capability */
            sch$clear_capability (pcb, mask, pcb->pcb$l_cpu_id, flags, 0);
#endif
            mask = *(int*)select_mask & (*(int*)modify_mask);
#if 0
            sch$require_capability(pcb, mask, 0, 0 /*prev_mask*/);
#else
            /** call set capability? TODO check and fix typo */
            sch$clear_capability (pcb, mask, pcb->pcb$l_cpu_id, flags, 0);
#endif
            if (prev_mask)
                *(int*)prev_mask = prev;
        }
    }
    else
    {
        if (prev_mask)
            *(int*)prev_mask = pcb->pcb$l_capability;
    }
    vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
    return SS$_NORMAL;
}

asmlinkage int exe$process_capabilities_wrap (struct struct_args * s)
{
    return exe$process_capabilities (s->s1, s->s2, s->s3, s->s4, s->s5, s->s6);
}
