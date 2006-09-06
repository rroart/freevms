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
#include<pridef.h>
#include <exe_routines.h>
#include <sch_routines.h>

// this behaves like getjpiw for now

asmlinkage int exe$getjpi(unsigned int efn, unsigned int *pidadr, void * prcnam, void *itmlst, struct _iosb *iosb, void (*astadr)(), unsigned long long astprm) {
  struct _pcb * p;
  int sts;
  struct item_list_3 * it=itmlst;
  exe$clref(efn);
  sts=exe$pscan_next_id(&p);
  if (sts==0)
    return SS$_NOMOREPROC;
  while (it->item_code) {
    switch (it->item_code) {
    case JPI$_PRCNAM:
      memcpy(it->bufaddr,&p->pcb$t_lname,15);
      break;

    case JPI$_PID:
      memcpy(it->bufaddr,&p->pcb$l_epid,4);
      break;

    case JPI$_MASTER_PID:
      memcpy(it->bufaddr,&p->pcb$l_pid,4);
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

    case JPI$_BIOCNT:
      *(unsigned short *)it->bufaddr=p->pcb$w_biocnt;
      break;

    case JPI$_BIOLM:
      *(unsigned short *)it->bufaddr=p->pcb$w_biolm;
      break;

    case JPI$_DIOCNT:
      *(unsigned short *)it->bufaddr=p->pcb$w_diocnt;
      break;

    case JPI$_DIOLM:
      *(unsigned short *)it->bufaddr=p->pcb$w_diolm;
      break;

    case JPI$_CPUTIM:
      *(unsigned int *)it->bufaddr=p->pcb$l_cputim;
      break;

    }
    it++;
  }

  struct _pcb * pcb = ctl$gl_pcb;
  sch$postef(pcb->pcb$l_pid, PRI$_NULL, efn);

  if (iosb)
    iosb->iosb$w_status=SS$_NORMAL;

  return SS$_NORMAL;
}

asmlinkage int exe$getjpiw(unsigned int efn, unsigned int *pidadr, void * prcnam, void *itmlst, struct _iosb *iosb, void (*astadr)(), unsigned long long astprm) {

  /* I think this is about it */

  int status=exe$getjpi(efn,pidadr,prcnam,itmlst,iosb,astadr,astprm);
  if ((status&1)==0) return status;
  return exe$synch(efn,iosb);

}

asmlinkage int exe$getjpi_wrap(struct struct_getjpi *s) {
  return exe$getjpi(s->efn,s->pidadr,s->prcnam,s->itmlst,s->iosb,s->astadr,s->astprm);
}

asmlinkage int exe$getjpiw_wrap(struct struct_getjpi *s) {
  return exe$getjpiw(s->efn,s->pidadr,s->prcnam,s->itmlst,s->iosb,s->astadr,s->astprm);
}

