// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file sysevtsrv.c
   \brief reading and clearing event flags
   \author Roar Thronæs
 */

#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include <cebdef.h>
#include <pridef.h>
#include <cpudef.h>
#include <ssdef.h>
#include <system_data_cells.h>
#include <misc_routines.h>
#include <sch_routines.h>

#undef MYDEB_EFC
#define MYDEB_EFC

/**
   \brief Returns a pointer to the event flag give a PCB and the event flag number.
   \param p PCB pointer
   \param efn event flag number
*/

inline unsigned long * getefcp(struct _pcb * p, unsigned long efn) {
  return &p->pcb$l_efcs + ((efn&96)>>5);
}

inline unsigned long * getefc(struct _pcb * p, unsigned long efn) {
  unsigned long * retval = &p->pcb$l_efcs + ((efn&96)>>5);
  if (efn<64)
    return retval;
  if (!retval) 
    return 0;
  return &((struct _ceb *)(*retval))->ceb$l_efc;
}

inline unsigned long * getefcno(unsigned long efn) {
  return ((efn&96)>>5);
}

#ifdef MYDEB_EFC
extern long efc[], efcc[];
#endif

/**
   \brief clear event flag - see 5.2 9.8
   \param efn event flag number
 */

asmlinkage int exe$clref(unsigned int efn) {
  int retval;
  struct _pcb * p=ctl$gl_pcb;
#ifdef MYDEB_EFC
  {
    int pid=p->pcb$l_pid&31;
    efc[1024*pid+efcc[pid]]=0x80000000|efn;
    efcc[pid]++;
    long addr = &efn;
    addr-=4;
    efc[1024*pid+efcc[pid]]=*(long*)addr;
    efcc[pid]++;
    if (efcc[pid]>1000)
      efcc[pid]=0;
  }
#endif
  int efncluster=(efn&224)>>5;
  unsigned long * clusteraddr;
  clusteraddr=getefc(p,efn);
  retval=test_and_clear_bit(efn&31,clusteraddr);
  /* do a bbcci instead */
  if (retval)
    return SS$_WASSET;
  else
    return SS$_WASCLR;
}

/**
   \brief read event flag - see 5.2 9.8
   \param efn event flag number
   \param state return here
 */

asmlinkage int exe$readef(unsigned int efn, unsigned int *state) {
  struct _pcb * p=current;
  int efncluster=(efn&224)>>5;
  unsigned long * clusteraddr;
  clusteraddr=getefc(p,efn);
  *state=*clusteraddr;
}

/**
   \brief set event flag - see 5.2 9.8
   \param efn event flag number
 */

asmlinkage int exe$setef(unsigned int efn) {
  struct _pcb * p=current;
  return sch$postef(p->pcb$l_pid,PRI$_IOCOM,efn);
}

