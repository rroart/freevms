// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/hw_irq.h>

#include <cpudef.h>
#include <system_data_cells.h>

#include <smp_routines.h>

void smp_work()
{
    int cpuid = ctl$gl_pcb->pcb$l_cpu_id;
    struct _cpu * cpu = smp$gl_cpu_data[cpuid];
    if (cpu->cpu$l_work_req & CPU$M_IOPOST)
    {
        cpu->cpu$l_work_req &= ~CPU$M_IOPOST; // check. smp non-atom spin
        if (cpuid != smp$gl_primid)
            printk("iopost ipint for non-primary\n");
        else
            SOFTINT_IOPOST_VECTOR;
    }
    if (cpu->cpu$l_work_req & CPU$M_UPDASTSR)
    {
        cpu->cpu$l_work_req &= ~CPU$M_UPDASTSR; // check. smp non-atom spin
        printk("what to do now?\n");
    }
    if (cpu->cpu$l_work_req & CPU$M_RESCHED)
    {
        cpu->cpu$l_work_req &= ~CPU$M_RESCHED; // check. smp non-atom spin
        SOFTINT_RESCHED_VECTOR;
    }
}
