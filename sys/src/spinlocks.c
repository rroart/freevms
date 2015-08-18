#include <linux/kernel.h>
#include <linux/sched.h>
#include <spldef.h>
#include <asm/bitops.h>
#include <system_data_cells.h>

/**
   \file spinlocks.c
   \brief spinlocking
   \author Roar Thron�s
*/

// define SPINDEF in spinlocks_mon?
#undef SPINDEB
#define SPINDEB

#ifdef SPINDEB
static int spinc = 0;
long spin[1024];
#endif

/**
   \brief acquire spinlock - see 5.2 8.3.8
   \param lockname
   \param lockipl -1 if no change
   \details not 100% according to spec
*/

inline int smp$acquire(struct _spl * spl)
{
    /** remember to do a smp enabled check - MISSING */
#ifdef SPINDEB
    long * l = &spl;
    spin[spinc++]=l[-1];
    spin[spinc++]=spl;
    spin[spinc++]=ctl$gl_pcb;
    spin[spinc++]=l;
    spin[spinc++]=smp_processor_id();
    spin[spinc++]=spl->spl$l_own_cnt;
    spin[spinc++]=ctl$gl_pcb->psl_ipl;
    spin[spinc++]=0;
    if(spinc>1000) spinc=0;
#endif
    /** according to spec, this is really acquirel */
    /** ipl is raised before we call this, not accordig to spec */
    /** get cpu id */
again:
    {}
    /** bbssi equivalent, set the spinlock bit */
    int bit = test_and_set_bit(0,&spl->spl$l_spinlock);
    if (bit)
    {
        /** if the bit was set, the spinlock has already been acquired */
        /** compare owner cpu with the cpu id (in spec addr) */
        if (spl->spl$l_own_cpu==smp$gl_cpu_data[ctl$gl_pcb->pcb$l_cpu_id])
        {
            /** if equal, nested acquiring, increase own_cnt */
            spl->spl$l_own_cnt++;
        }
        else
        {
            /** if not loop in lock */
            while (test_bit(0,&spl->spl$l_spinlock))
            {
            }
            goto again;
        }
    }
    else
    {
        /** if the bit was clear, this cpu now owns the lock */
        /** store cpu db id (speced address) in spinlock */
        spl->spl$l_own_cpu=smp$gl_cpu_data[ctl$gl_pcb->pcb$l_cpu_id];
#ifdef SPINDEB
        if (spl->spl$l_own_cpu==0)
            panic("cpu 0\n");
#endif
        /** increment spl own_cnt */
        spl->spl$l_own_cnt++;
#ifdef SPINDEB
        if (spl->spl$l_own_cnt==1)
        {
            panic("cnt 1\n");
#endif
        }
#if 0
        // check where the book wanted these
        unsigned long intr;
        local_irq_save(intr);
        local_irq_disable();
        local_irq_restore(intr);
#endif
    }
    /** use cpu busywait field - MISSING */
    /** use spl timo_int - MISSING */
}

/**
   \brief release spinlock - see 5.2 8.3.8
   \param lockname
   \param lockipl -1 if no change
   \details not 100% according to spec
*/

inline int smp$restore(struct _spl * spl)
{
    /** remember to do a smp enabled check - MISSING */
    /** decrease spinlock owner count */
    spl->spl$l_own_cnt--;
    /** if count is -1, it is now free */
    if (spl->spl$l_own_cnt==-1)
    {
        /** set owner cpu to 0 */
        spl->spl$l_own_cpu = 0;
        /** BBCCI equivalent, clear spinlock bit to free it */
        clear_bit(0,&spl->spl$l_spinlock);
        /** test for bugcheck - MISSING */
    }
}

