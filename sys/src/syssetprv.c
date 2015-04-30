// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <linux/linkage.h>
#include <linux/sched.h>

#include <ssdef.h>
#include <system_data_cells.h>

asmlinkage int exe$setprv(char enbflg, struct _generic_64 *prvadr, char prmflg, struct _generic_64 *prvprv)
{
    // also do setprv check and more
    // need also change pcb$l_priv, but will not recompile now
    struct _pcb * pcb = ctl$gl_pcb;
    long long * p = prvprv;
    if (p)
        *p=pcb->pcb$l_priv;
    long long * l = *(long long*)prvadr;
    p = prvadr;
    if (enbflg)
        pcb->pcb$l_priv|=*p;
    else
        pcb->pcb$l_priv&=~*p;

    return SS$_NORMAL;
}
