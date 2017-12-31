// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>
#include<phddef.h>
#include<system_data_cells.h>
#include<va_rangedef.h>
#include<wsldef.h>

int mm_in_range(struct _va_range *inadr, void *address)
{
    address=((unsigned long)address)&0xfffffe00;
    if (address>=inadr->va_range$ps_start_va && address<=inadr->va_range$ps_end_va) return 1;
    else
        return 0;
}

int mm_common_lock(struct _va_range *inadr,int mask,int set)
{
    struct _pcb * p=ctl$gl_pcb;
    struct _wsl * wsl=p->pcb$l_phd->phd$l_wslist;
    int i;
    if (set)
    {
        for(i=0; i<512; i++)
            if (mm_in_range(inadr,wsl[i].wsl$pq_va)) wsl[i].wsl$pq_va=(unsigned long)wsl[i].wsl$pq_va|mask;
    }
    else
    {
        for(i=0; i<512; i++)
            if (mm_in_range(inadr,wsl[i].wsl$pq_va)) wsl[i].wsl$pq_va=(unsigned long)wsl[i].wsl$pq_va&~mask;
    }
    return 1;
}

asmlinkage int exe$lkwset(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode)
{
    return mm_common_lock(inadr, WSL$M_WSLOCK, 1);
}

asmlinkage int exe$lckpag(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode)
{
    return mm_common_lock(inadr, WSL$M_PFNLOCK, 1);
}

asmlinkage int exe$ulwset(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode)
{
    return mm_common_lock(inadr, WSL$M_WSLOCK, 0);
}

asmlinkage int exe$ulkpag(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode)
{
    return mm_common_lock(inadr, WSL$M_PFNLOCK, 0);
}
