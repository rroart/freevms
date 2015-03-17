// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file sysdassgn.c
   \brief QIO deassign channel - TODO still more doc
   \author Roar Thronæs
*/

#include<starlet.h>
#include<iodef.h>
#include<ssdef.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ccbdef.h>
#include<system_data_cells.h>
#include<linux/vmalloc.h>
#include<linux/linkage.h>
#include <exe_routines.h>
#include <ioc_routines.h>
#include <sch_routines.h>

/**
   \brief deassign channel
   \param chan i/o channel
*/

asmlinkage int exe$dassgn(unsigned short int chan)
{
    struct _ccb * ccb;
    /** verify i/o channel */
    int sts = ioc$verify_chan(chan, &ccb);
    if ((sts & 1) == 0)
        return sts;
    /** call exe$canceln, but not implemented, do $cancel instead */
    // cancel is according to the book going to get the code, but how?
    sts = exe$cancel(chan /*, 0 CAN$C_DASSGN*/); // does not read param
    /** call verify_chan again, in case of asts - MISSING */
    /** check ccb$l_wind and close the file - MISSING */
    /** check ccb$w_ioc to see if outstanding i/o - MISSING */
    /** lock i/o db */
    sch$iolockw();
    /** clear ccb amod */
    ccb->ccb$b_amod=0;
    /** disassociate eventual mailbox - MISSING */
    /** dec ucb ref count */
    ccb->ccb$l_ucb->ucb$l_refc--;
    /** checks and stuff to do if refc == 0 - MISSING */
    /** eventually do ioc$last_chan - MISSING */
    /** i/o db unlock */
    sch$iounlock();
    return SS$_NORMAL;
}


