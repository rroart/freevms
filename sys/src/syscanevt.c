// $Id$
// $Locker$

// Author. Roar Thron�s.
/**
   \file syscanevt.c
   \brief system cancel events
   \author Roar Thron�s
*/

#include <ssdef.h>

#include <linux/linkage.h>
#include <linux/sched.h>

#include <system_data_cells.h>

#include <tqedef.h>
#include <exe_routines.h>

/**
   \brief remove timer request id from timer queue - see 5.2 11.6.4
   \param reqid request id
   \param acmode access mode
*/

asmlinkage int exe$cantim(unsigned long reqidt, unsigned int acmode)
{
    exe_std$rmvtimq(acmode,reqidt,0,0);
    return SS$_NORMAL;
}

/**
   \brief cancel wake request id from timer queue - see 5.2 11.6.4
   \param pidadr pid address
   \param prcnam process name
*/

asmlinkage int exe$canwak(unsigned int *pidadr, void *prcnam)
{
    /** TODO also handle prcnam - MISSING */
    int ipid=ctl$gl_pcb->pcb$l_pid;
    exe_std$rmvtimq(0,0,TQE$C_WKSNGL,ipid);
    return SS$_NORMAL;
}
