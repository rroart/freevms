// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/config.h>

#include<starlet.h>
#include<ddtdef.h>
#include<iodef.h>
#include<ipldef.h>
#include<ssdef.h>
#include<misc.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ccbdef.h>
#include<system_data_cells.h>
#include<linux/linkage.h>
#include<linux/sched.h>

asmlinkage int exe$cancel(unsigned short int chan) {
  setipl(IPL$_ASTDEL);
  struct _ucb * ucb;
  struct _ccb * ccb;
  ccb = &ctl$ga_ccb_table[chan];
  ucb = ccb->ccb$l_ucb;
  struct _pcb * pcb = ctl$gl_pcb;
  struct _irp * irph;
  irph = &ucb->ucb$l_ioqfl;
  struct _irp * irp;
  irp = irph->irp$l_ioqfl;
  while (irp!=irph) {
    struct _irp * next = irp->irp$l_ioqfl;
    if (irp->irp$l_pid==pcb->pcb$l_pid && irp->irp$w_chan==chan) {
      remque(irp, 0);
      irp->irp$l_iost1 = SS$_CANCEL;
      // check. beware movbuf
      com$post(irp, ucb);
    }
    irp = next;
  }
  if ((irp = ucb->ucb$l_irp)) {
    // check this later
    //    ucb->ucb$l_irp = 0;
    struct _ddt *ddt; 
    void (*f)(void *,void *, void* ,void *);
    ddt=ucb->ucb$l_ddt;
    f=ddt->ddt$l_cancel;
    f(irp,pcb,ucb,ccb);
  // cancel is according to the book going to get the code from $canceln, but how?
  }
  setipl(0);
  return SS$_NORMAL;
}
