// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<system_data_cells.h>
#include<linux/kernel.h>
#include <asm/hw_irq.h>
#include <vms_drivers.h>
#include <ipldef.h>
#include <pridef.h>
#include <irpdef.h>
#include <acbdef.h>

#include <linux/mm.h>

void  com_std$delattnastp (struct _acb **acb_lh, struct _ucb *ucb, int ipid);

void com$post(struct _irp * i, struct _ucb * u) {
  insqti(i,&ioc$gq_postiq);
  SOFTINT_IOPOST_VECTOR;
}

void  com_std$delattnast (struct _acb **acb_lh, struct _ucb *ucb) {
  com_std$delattnastp(acb_lh, ucb, 0);
}

void fork_routine(fr3,fr4,fr5) {
  struct _acb * a = fr5;
  long * exp = &a->acb$l_kast;

  a->acb$l_ast=a->acb$l_kast;
  a->acb$l_kast=0;
  a->acb$l_astprm=exp[1];
  a->acb$b_rmod=0;
  a->acb$l_pid=exp[3];
  return sch$qast(a->acb$l_pid,PRI$_IOCOM,a);
}

void  com_std$delattnastp (struct _acb **acb_lh, struct _ucb *ucb, int ipid) {
  struct _acb * next, * cur = acb_lh;
  while (cur->acb$l_astqfl) {
    next=cur->acb$l_astqfl;
    long * exp=&next->acb$l_kast;
    if ((ipid == 0) || (ipid == exp[3])) {
      cur->acb$l_astqfl=next->acb$l_astqfl;
      fork(fork_routine,0,0,next);
    } else {
      cur=next;
    }
  }
}

void  com_std$delctrlast (struct _acb **acb_lh, struct _ucb *ucb, int matchchar, int *inclchar_p);

void  com_std$delctrlastp (struct _acb **acb_lh, struct _ucb *ucb, int ipid, int matchchar, int *inclchar_p);

void  com_std$drvdealmem (void *ptr) {
  kfree(ptr); // maybe do more checks
}

int   com_std$flushattns (struct _pcb *pcb, struct _ucb *ucb, int chan, struct _acb **acb_lh) {
  // missing device lock
  setipl(ucb->ucb$b_dipl);
  long ipid=pcb->pcb$l_pid;
  struct _acb * next, * cur = acb_lh;
  while (cur->acb$l_astqfl) {
    next=cur->acb$l_astqfl;
    long * exp=&next->acb$l_kast;
    if ((ipid == 0) || (ipid == exp[3])) {
      kfree(next);
      cur->acb$l_astqfl=next->acb$l_astqfl;
    } else {
      cur=next;
    }
  }
  setipl(0);
  return SS$_NORMAL;
}

int   com_std$flushctrls (struct _pcb *pcb, struct _ucb *ucb, int chan, struct _acb **acb_lh, int *mask_p);

void  com_std$post (struct _irp *irp, struct _ucb *ucb) {
  com$post(irp,ucb);
}

void  com_std$post_nocnt (struct _irp *irp);

int   com_std$setattnast (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, struct _acb **acb_lh) {
  short int chan=ccb-&ctl$gl_ccbbase[0];
  long * exp;
  if (irp->irp$l_qio_p1==0) 
    return com_std$flushattns(pcb,ucb,chan,acb_lh);
  struct _acb * acb = kmalloc(sizeof(struct _acb)+100,GFP_KERNEL);
  memset(acb, 0, sizeof(struct _acb));
  exp=&acb->acb$l_kast;
  exp[0]=irp->irp$l_qio_p1;
  exp[1]=irp->irp$l_qio_p2;
  exp[2]=chan;
  exp[3]=pcb->pcb$l_pid;
  setipl(ucb->ucb$b_dipl);
  acb->acb$b_rmod=IPL$_QUEUEAST; // really fkb$b_flck
  acb->acb$l_astqfl=*acb_lh;
  *acb_lh=acb;
  setipl(0);
  return SS$_NORMAL;
}

int   com_std$setctrlast (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, struct _acb **acb_lh,int mask, struct _tast **tast_p);
