// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/unistd.h>
#include<linux/linkage.h>
#include<linux/sched.h>
#include<starlet.h>
#include<ssdef.h>
#include<evtdef.h>
#include<pridef.h>

/* Author: Roar Thronæs */

asmlinkage int exe$setpri_wrap(struct struct_setpri *s) {
  return exe$setpri(s->pidadr,s->prcnam,s->pri,s->prvpri,s->pol,s->prvpol);
}

asmlinkage int exe$setpri(unsigned int *pidadr, void *prcnam, unsigned int pri, unsigned int *prvpri, unsigned int*pol, unsigned int *prvpol) {
  struct _pcb *p;
  struct _pcb * retpcb;
  unsigned long ipid, epid;
  int sts;
  sts=exe$nampid(current,pidadr,prcnam,&retpcb,&ipid,&epid);
  p=retpcb;

  if (!p) p=current;

  p->pcb$b_prib=31-pri;

  if (p==current)
    sch$change_cur_priority(p, p->pcb$b_prib);

  sch$rse(p,PRI$_IOCOM,EVT$_SETPRI);

  return SS$_NORMAL;
}
