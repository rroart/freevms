// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file postef.c
   \brief post event flag
   \author Roar Thronæs
 */

#include<linux/sched.h>
#include<asm/bitops.h>
#include<ssdef.h>
#include<cebdef.h>
#include<evtdef.h>
#include<ipldef.h>
#include<system_data_cells.h>
#include<ipl.h>
#include<internals.h>
#include <exe_routines.h>
#include <sch_routines.h>
#include <misc_routines.h>

#undef MYDEB_EFC
#define MYDEB_EFC

/**
   \brief check whether this flag satisfies a wait request for this process
   \details for single flags < 64
   \param p the process pcb
   \param priclass the priority calss
   \param efp event flag pointer
   \param clusteraddr address of the cluster
 */

int waitcheck(struct _pcb *p, unsigned long priclass, unsigned long * efp, unsigned long * clusteraddr) {
  unsigned long tmp;
  if (efp!=clusteraddr) return;
  tmp=(*clusteraddr)&~p->pcb$l_efwm;
#if 0
  *clusteraddr&=~p->pcb$l_efwm;
#endif
  if (!tmp) return;
  // tmp=~(p->pcb$l_sts&(~PCB$M_WALL)); // what?
  // if (!tmp && tmp!=p->pcb$l_efwm) return; // what?
  /** if wait satisfied invoke sch$rse */
  sch$rse(p,priclass,EVT$_EVENT);
}

/**
   \brief check whether this flag satisfies a wait request for this process
   \details for common flags >= 64
   \param p the process pcb
   \param priclass the priority calss
   \param efp event flag pointer
   \param clusteraddr address of the cluster
 */

int waitcheck2(struct _pcb *p, unsigned long priclass, unsigned long * efp, unsigned long * clusteraddr) {
  unsigned long tmp;
  //  if (efp!=clusteraddr) return;
  tmp=(*clusteraddr)&~p->pcb$l_efwm;
  //*clusteraddr&=~p->pcb$l_efwm;
  if (tmp==0) return;
  if (p->pcb$l_sts&PCB$M_WALL) {
    if (p->pcb$l_efwm!=(~tmp))
      return;
  }
  // if (!tmp && tmp!=p->pcb$l_efwm) return; // what?
  /** if wait satisfied invoke sch$rse */
  sch$rse(p,priclass,EVT$_EVENT);
}

#ifdef MYDEB_EFC
long efc[32*1024];
long efcc[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#endif

/**
   \brief setting an event flag - see 5.2 9.7
   \param ipid internal process id
   \param priclass at which priority class
   \param efn event flag number
 */

int sch$postef(unsigned long ipid, unsigned long priclass, unsigned long efn) {
  /** sched spinlock */
  int savipl=vmslock(&SPIN_SCHED,IPL$_SYNCH);
#ifdef MYDEB_EFC
  {
    int pid=ipid&31;
    efc[1024*pid+efcc[pid]]=efn;
    efcc[pid]++;
    long addr = &ipid;
    addr-=4;
    efc[1024*pid+efcc[pid]]=*(long*)addr;
    efcc[pid]++;
    if (efcc[pid]>1000)
      efcc[pid]=0;
  }
#endif
  struct _pcb * p, * first;
  int efncluster=getefcno(efn);
  int retval;
  unsigned long * clusteraddr;
  /* sched spinlock */
  p=exe$ipid_to_pcb(ipid);
  /** does process still exist */
  if (!p) {
    retval = SS$_NONEXPR;
    goto end;
  }
  /** test event flag legality */
  if (efn>127) {
    retval = SS$_ILLEFC;
    goto end;
  }
  clusteraddr=getefc(p,efn);
  /** if non-common event flag */
  /** test if set */
  if (efncluster<2) {
    if (test_and_set_bit(efn&31,clusteraddr)) // bbssi
      retval=SS$_WASSET;
    else
      retval=SS$_WASCLR;
    /** wait satisfied? */
    waitcheck(p,priclass,&p->pcb$l_efcs+p->pcb$b_wefc,clusteraddr);
  } else {
    /** common event flag */
    struct _ceb * c=*(unsigned long *)getefcp(p,efn);
    struct _pcb * tmp, *next;
    if (!c) {
      retval=SS$_UNASEFC;
      goto end;
    }

    if (test_and_set_bit(efn&31,clusteraddr)) // bbssi
      retval=SS$_WASSET;
    else
      retval=SS$_WASCLR;

    /** locate ceb */
    if (retval==SS$_WASCLR) {
      first=&c->ceb$l_wqfl;
      tmp=first->pcb$l_sqfl;
      while (first!=tmp) {
	clusteraddr=getefc(tmp,efn);
	next=tmp->pcb$l_sqfl;
	/** wait satisfied? */
	waitcheck2(tmp,priclass,&tmp->pcb$l_efcs+tmp->pcb$b_wefc,clusteraddr);
	tmp=next;
      }
    }
  }
 end:
  /** unlock sched spinlock */
  vmsunlock(&SPIN_SCHED,savipl);
  return retval;
}
