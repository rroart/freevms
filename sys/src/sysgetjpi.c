// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>

#include<starlet.h>
#include<jpidef.h>
#include <ssdef.h>
#include <misc.h>
#include <phddef.h>

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

    case JPI$_STATE:
      *(unsigned long *)it->bufaddr=p->pcb$w_state;
      break;

    case JPI$_PRI:
      *(unsigned long *)it->bufaddr=p->pcb$b_pri;
      break;

    case JPI$_PAGEFLTS:
      *(unsigned long *)it->bufaddr=p->pcb$l_phd->phd$l_pageflts;
      break;

    case JPI$_PPGCNT:
      *(unsigned long *)it->bufaddr=p->pcb$l_ppgcnt;
      break;

    case JPI$_GPGCNT:
      *(unsigned long *)it->bufaddr=p->pcb$l_gpgcnt;
      break;

    case JPI$_WSAUTH:
      *(unsigned long *)it->bufaddr=p->pcb$l_phd->phd$l_wsauth;
      break;

    case JPI$_WSQUOTA:
      *(unsigned long *)it->bufaddr=p->pcb$l_phd->phd$l_wsquota;
      break;

    case JPI$_WSSIZE:
      *(unsigned long *)it->bufaddr=p->pcb$l_phd->phd$l_wssize;
      break;

    case JPI$_WSAUTHEXT:
      *(unsigned long *)it->bufaddr=p->pcb$l_phd->phd$l_wsauthext;
      break;

    case JPI$_WSEXTENT:
      *(unsigned long *)it->bufaddr=p->pcb$l_phd->phd$l_wsextent;
      break;

    }
    it++;
  }
  return SS$_NORMAL;
}

asmlinkage int exe$getjpi_wrap(struct struct_getjpi *s) {
  return exe$getjpi(s->efn,s->pidadr,s->prcnam,s->itmlst,s->iosb,s->astadr,s->astprm);
}

