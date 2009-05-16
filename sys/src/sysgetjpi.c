// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file sysgetjpi.c
   \brief system get job and process information
   \author Roar Thronæs
*/

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

/**
   \brief system service get job or process info - see 5.2 13.2.3
   \details make multiuser version
*/

asmlinkage int exe$getjpi(unsigned int efn, unsigned int *pidadr, void * prcnam, void *itmlst, struct _iosb *iosb, void (*astadr)(), unsigned long long astprm) {
  struct _pcb * p;
  int sts;
  struct item_list_3 * it=itmlst;
  /** test list item entries */
  /** use pidadr - MISSING */
  /** clear event flag */
  exe$clref(efn);
  /** find next pcb */
  sts=exe$pscan_next_id(&p);
  if (sts==0)
    return SS$_NOMOREPROC;
  /** invoke nampid - MISSING */
  /** if other node, cwps - MISSING */
  /** iosb writecheck - MISSING */
  /** if ast, check ast quota and charge - MISSING */
  /** writetest for buffer descriptors - MISSING */
  /** gather some usual informastion - TODO still more remains */
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
      *(long long *)it->bufaddr=p->pcb$l_phd->phd$l_cputim;
      break;

    case JPI$_DIRIO:
      *(unsigned int *)it->bufaddr=p->pcb$l_phd->phd$l_diocnt;
      break;

    case JPI$_BUFIO:
      *(unsigned int *)it->bufaddr=p->pcb$l_phd->phd$l_biocnt;
      break;

    }
    it++;
  }

  /** post event flag */
  struct _pcb * pcb = ctl$gl_pcb;
  sch$postef(pcb->pcb$l_pid, PRI$_NULL, efn);

  /** eventual ast queue - MISSING */

  /** eventual iosb write */
  if (iosb)
    iosb->iosb$w_status=SS$_NORMAL;

  return SS$_NORMAL;

  /** handle target process, kast etc - MISSING */
  /** handle target process, status, state - MISSING */

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

