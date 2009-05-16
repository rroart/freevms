// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file sys_affinity.c
   \brief system service affinities
   \author Roar Thronæs
*/

#include <linux/config.h>
#include <linux/linkage.h>
#include <far_pointers.h>
#include <linux/sched.h>
#include <ssdef.h>
#include <cpbdef.h>
#include <ipldef.h>
#include <statedef.h>
#include <internals.h>
#include <prvdef.h>
#include <sch_routines.h>
#include <exe_routines.h>
#include <capdef.h>
#include <starlet.h>

/**
   \brief system service for explicit affinity mask - see 7.0 2.5.2
   \details lots of flags and checks unimplemented
*/

// lots of flags and checks unimplemented
asmlinkage int exe$process_affinity (unsigned int *pidadr, void *prcnam, struct _generic_64 *select_mask, struct _generic_64 *modify_mask, struct _generic_64 *prev_mask, struct _generic_64 *flags,...) {
  struct _pcb * retpcb;
  unsigned long ipid, epid;
  int sts;
  sts=exe$nampid(current,pidadr,prcnam,&retpcb,&ipid,&epid);
  if ((sts & 1) == 0)
    return sts;
  struct _pcb * pcb;
  pcb = retpcb;
  int aff;
  int priv = ctl$gl_pcb->pcb$l_priv;
  // check right?
  if (pcb != ctl$gl_pcb && (select_mask || modify_mask) && (priv & PRV$M_ALTPRI) == 0)
    return SS$_NOPRIV;
  if ((select_mask || modify_mask)) {
    int prev = pcb->pcb$l_affinity;
    int mask;
    mask = *(int*)select_mask & ~(*(int*)modify_mask);
    sch$clear_affinity(pcb, mask, flags, 0 /*prev_mask*/);
    mask = *(int*)select_mask & (*(int*)modify_mask);
    /** call set_affinity */
    sch$set_affinity(pcb, mask, flags, 0 /*prev_mask*/);
    if (prev_mask)
      *(int*)prev_mask = prev;
  } else {
    if (prev_mask)
      *(int*)prev_mask = pcb->pcb$l_affinity;
  }
  vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
  return SS$_NORMAL;
}

/**
   \brief system service for implicit affinity mask - see 7.0 2.5.4
   \details lots of flags and checks unimplemented
*/

asmlinkage int exe$set_implicit_affinity (unsigned int *pidadr, void *prcnam, struct _generic_64 *state, int cpu_id, struct _generic_64 *prev_mask) {
  int flag = 0;
  if (state)
    flag = *(int*)state;
  struct _pcb * retpcb;
  unsigned long ipid, epid;
  int sts;
  sts=exe$nampid(current,pidadr,prcnam,&retpcb,&ipid,&epid); 
  if ((sts & 1) == 0)
    return sts;
  struct _pcb * pcb;
  pcb = retpcb;
  int priv = ctl$gl_pcb->pcb$l_priv;
  if (flag && (priv & PRV$M_ALTPRI) == 0)
    return SS$_NOPRIV;
  if ((priv & PRV$M_ALTPRI) == 0)
    return SS$_NOPRIV;
  if (flag & CAP$M_IMPLICIT_DEFAULT_ONLY) {
    if (prev_mask)
      *(int*)prev_mask = sch$gl_default_process_cap;
    sch$gl_default_process_cap = state;
  }
  /** maybe do acquire affinity */
  if (flag & CAP$M_IMPLICIT_AFFINITY_SET) {
    if (prev_mask)
      *(int*)prev_mask = pcb->pcb$l_capability;
    sch$acquire_affinity(pcb, 0, cpu_id);
  }
  /** maybe do release affinity */
  if (flag & CAP$M_IMPLICIT_AFFINITY_CLEAR) {
    if (prev_mask)
      *(int*)prev_mask = pcb->pcb$l_capability;
    sch$release_affinity(pcb);
  }
  vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
  return SS$_NORMAL;
}

asmlinkage int exe$process_affinity_wrap (struct struct_args * s) {
  return exe$process_affinity (s->s1, s->s2, s->s3, s->s4, s->s5, s->s6);
}

