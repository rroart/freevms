// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <ssdef.h>
#include <irpdef.h>
#include <system_data_cells.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <sch_routines.h>

int   exe_std$writechk (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, void *buf, int bufsiz) {
  // not fully implemented. lacks actual check?
  if (bufsiz < 0) return SS$_BADPARAM;
  irp->irp$l_bcnt=bufsiz;
  return SS$_NORMAL;
}

int   exe_std$readchk (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, void *buf, int bufsiz) {
  // not fully implemented. lacks actual check?
  if (bufsiz < 0) return SS$_BADPARAM;
  irp->irp$l_bcnt=bufsiz;
  return SS$_NORMAL;
}

int exe_std$iorsnwait (struct _irp * irp, struct _pcb * pcb, struct _ucb * ucb, struct _ccb * ccb, int qio_sts, int rsn) {
  kfree(irp);
  pcb->pcb$l_efwm=rsn;
  sch$gl_resmask|=(1<<rsn);
  return sch$wait(pcb,sch$gq_mwait);
}
