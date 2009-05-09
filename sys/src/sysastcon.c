// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file system ast control.c
   \brief ast controlling
   \author Roar Thronæs
 */


#include <system_data_cells.h>
#include <acbdef.h>
#include <pridef.h>
#include <cpudef.h>
#include <psldef.h>
#include <ssdef.h>
#include <sch_routines.h>
#include <linux/smp.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>

/**
   \brief enabling/disabling ast delivery according to callers mode - see 5.2 7.6
   \param enbflg enable flag
   \details does not set reenable_asts and soft_ast_disable
   \details TODO: check implementation
*/

int exe$setast(char enbflg) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=ctl$gl_pcb;
  int retval;
  /** check if already set or unset */
  if (p->pcb$b_asten&(1<<p->psl_prv_mod))
    retval=SS$_WASSET;
  else
    retval=SS$_WASCLR;
  /** set accoring to priv level and recompute */
  if (p->oldpsl_prv_mod!=PSL$C_USER) { // check
  back:
    if (enbflg) 
      p->pcb$b_asten|=(1<<p->psl_prv_mod);
    else
      p->pcb$b_asten&=~(1<<p->psl_prv_mod);
    sch$newlvl(p);
    return retval;
  }
  goto back; // the same for now
  return retval;
}

/**
   \brief creating an ast - see 5.2 7.3
   \param astadr routine
   \param astprm parameter
   \param acmode access mode
*/

int exe$dclast(void (*astadr)(__unknown_params), unsigned long astprm, unsigned int acmode) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=ctl$gl_pcb;
  /** allocate acb and fill it */
  struct _acb * a=kmalloc(sizeof(struct _acb),GFP_KERNEL);
  memset(a,0,sizeof(struct _acb));
  a->acb$l_pid=p->pcb$l_pid;
  a->acb$l_ast=astadr;
  a->acb$l_astprm=astprm;
  if (p->psl_prv_mod > acmode)
    acmode = p->psl_prv_mod;
  a->acb$b_rmod=acmode;
  /** call sch$qast */
  sch$qast(p->pcb$l_pid,PRI$_NULL,a);
  return SS$_NORMAL;
}
