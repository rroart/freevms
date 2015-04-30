// $Id$
// $Locker$

// Author. Roar Thron�s.
/**
   \file iosubrams.c
   \brief QIO subroutines
   \author Roar Thron�s
*/

#include <linux/kernel.h>
#include <asmlink.h>
#include <linux/smp.h>
#include <linux/sched.h>
#include <asm/hw_irq.h>
#include <asm/current.h>
#include <system_data_cells.h>
#include <internals.h>
#include <queue.h>
#include <ddtdef.h>
#include <ucbdef.h>
#include <irpdef.h>
#include <ipldef.h>
#include <wcbdef.h>
#include <ssdef.h>

/**
   \brief map virtual block to logical block - see 5.2 21.8.2
   \param vbn virtual block number
   \param numbytes yet unused
   \param wcb window control block
   \param irp yet unused
   \param ucb yet unused
   \param lbn_p return logical block value
   \param notmapped_p yet unused
   \param new_ucb_p yet unused
*/

int ioc_std$mapvblk (unsigned int vbn, unsigned int numbytes, struct _wcb *wcb, struct _irp *irp, struct _ucb *ucb, unsigned int *lbn_p, unsigned *notmapped_p, struct _ucb **new_ucb_p)
{
    struct _wcb * head=wcb;
    struct _wcb * tmp=head->wcb$l_wlfl;
    while (tmp!=head)
    {
        if (vbn>=tmp->wcb$l_stvbn && vbn<(tmp->wcb$l_stvbn+tmp->wcb$l_p1_count))
        {
            *lbn_p=tmp->wcb$l_p1_lbn+(vbn-tmp->wcb$l_stvbn);
            return SS$_NORMAL;
        }
        tmp=tmp->wcb$l_wlfl;
    }
    return 0;
}
