// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <ssdef.h>
#include <irpdef.h>

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

