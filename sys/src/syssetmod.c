// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/kernel.h>
#include<linux/sched.h>
#include<internals.h>
#include<ipldef.h>
#include<pcbdef.h>
#include<prvdef.h>
#include<ssdef.h>
#include<system_data_cells.h>

asmlinkage int exe$setrwm(int flags)
{
    printk("setrwm not implemented\n");
    return SS$_NORMAL;
}

asmlinkage int exe$setswm(int flags)
{
    int priv = ctl$gl_pcb->pcb$l_priv;
    if ((priv & PRV$M_PSWAPM) == 0)
        return SS$_NOPRIV; // check
    int oldipl = vmslock(&SPIN_SCHED, IPL$_SCHED);
    if (flags)
        ctl$gl_pcb->pcb$l_sts |= PCB$M_PSWAPM;
    else
        ctl$gl_pcb->pcb$l_sts &= ~PCB$M_PSWAPM;
    vmsunlock(&SPIN_SCHED, oldipl);
    return SS$_NORMAL;
}

