// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>

#include<starlet.h>
#include<jpidef.h>
#include <ssdef.h>
#include <misc.h>

// this behaves like getjpiw for now

asmlinkage int exe$getjpi(unsigned int efn, unsigned int *pidadr, void * prcnam, void *itmlst, struct _iosb *iosb, void (*astadr)(), unsigned long long astprm) {
  struct _pcb * p;
  int sts;
  struct item_list_3 * it=itmlst;
  sts=exe$pscan_next_id(&p);
  if (sts==0)
    return SS$_NOMOREPROC;
  while (it->item_code) {
    switch (it->item_code) {
    case JPI$_PRCNAM:
      bcopy(&p->pcb$t_lname,it->bufaddr,15);
      break;

    case JPI$_PID:
      bcopy(&p->pcb$l_epid,it->bufaddr,4);
      break;

    case JPI$_MASTER_PID:
      bcopy(&p->pid,it->bufaddr,4);
      break;

    }
    it++;
  }
  return 1;
}

asmlinkage int exe$getjpi_wrap(struct struct_getjpi *s) {
  return exe$getjpi(s->efn,s->pidadr,s->prcnam,s->itmlst,s->iosb,s->astadr,s->astprm);
}

