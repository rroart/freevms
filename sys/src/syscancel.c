// $Id$
// $Locker$

// Author. Roar Thron�s.
/**
   \file syscancel.c
   \brief QIO cancel
   \author Roar Thron�s
*/

#include <linux/config.h>

#include <starlet.h>
#include <ddtdef.h>
#include <iodef.h>
#include <ipldef.h>
#include <ssdef.h>
#include <misc.h>
#include <irpdef.h>
#include <ucbdef.h>
#include <ccbdef.h>
#include <system_data_cells.h>
#include <queue.h>
#include <ipl.h>
#include <linux/linkage.h>
#include <linux/sched.h>
#include <exe_routines.h>

/**
   \brief cancel i/o on channel
   \param chan i/o channel
*/

asmlinkage int exe$cancel(unsigned short int chan)
{
    /** verifies channel legality - MISSING */
    /** handle affinity - MISSING */
    /** set ipl */
    setipl(IPL$_ASTDEL);
    struct _ucb * ucb;
    struct _ccb * ccb;
    /** pagefault ccb into memory */
    ccb = &ctl$ga_ccb_table[chan];
    /** higher set ipl - MISSING */
    /** forklock - MISSING */
    ucb = ccb->ccb$l_ucb;
    struct _pcb * pcb = ctl$gl_pcb;
    struct _irp * irph;
    irph = &ucb->ucb$l_ioqfl;
    struct _irp * irp;
    irp = irph->irp$l_ioqfl;
    /** scan irps */
    while (irp!=irph)
    {
        struct _irp * next = irp->irp$l_ioqfl;
        /** if pids and channels match */
        /** and if virtual clear - MISSING */
        if (irp->irp$l_pid==pcb->pcb$l_pid && irp->irp$w_chan==chan)
        {
            /** remove irp from queue */
            remque(irp, 0);
            /** clear buffer read bit - MISSING */
            irp->irp$l_iost1 = SS$_CANCEL;
            /** SS$_CANCEL status */
            /** check. beware movbuf */
            /** insert at postprocessing queue */
            com$post(irp, ucb);
            /** iopost softint - MISSING */
        }
        irp = next;
    }
    /** invoke driver cancel routine */
    if ((irp = ucb->ucb$l_irp))
    {
        // check this later
        //    ucb->ucb$l_irp = 0;
        struct _ddt *ddt;
        void (*f)(void *,void *, void* ,void *);
        ddt=ucb->ucb$l_ddt;
        f=ddt->ddt$l_cancel;
        f(irp,pcb,ucb,ccb);
        // cancel is according to the book going to get the code from $canceln, but how?
    }
    /** unlock forklock - MISSING */
    /** relinquish affinity - MISSING */
    /** set ipl 0 */
    setipl(0);
    /** no oustanding i/o ccb ioc 0, wind 0, return - MISSING */
    /** device mounted or foreign mount, return - MISSING */
    /** has process section, return - MISSING */
    /** NOTFCP set, return - MISSING */
    /** if no resources etc to allocate irp, return or sch$rwait - MISSING */
    /** irp with acpcontrol etc - MISSING */
    /** charge buffer quota - MISSING */
    /** release forklock and affinity - MISSING */
    /** invoke qioacppkt - MISSING */
    return SS$_NORMAL;
}
