// $Id$
// $Locker$

// Author. Roar Thronæs.

#include "../../freevms/sys/src/system_data_cells.h"
#include "../../freevms/lib/src/acbdef.h"
#include "../../freevms/lib/src/pridef.h"
#include "../../freevms/lib/src/cpudef.h"
#include "../../freevms/starlet/src/psldef.h"
#include "../../freevms/starlet/src/ssdef.h"
#include <linux/smp.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>

int exe$setast(char enbflg) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=cpu->cpu$l_curpcb;
  int retval;
  if (p->pcb$b_asten&(1<<p->psl_cur_mod))
    retval=SS$_WASSET;
  else
    retval=SS$_WASCLR;
  if (p->oldpsl_prv_mod!=PSL$C_USER) {
  back:
    if (enbflg) 
      p->pcb$b_asten|=(1<<p->psl_cur_mod);
    else
      p->pcb$b_asten&=~(1<<p->psl_cur_mod);
    sch$newlvl(p);
    return retval;
  }
  goto back; // the same for now
  return retval;
}

int exe$dclast(void (*astadr)(__unknown_params), unsigned long astprm, unsigned int acmode) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=cpu->cpu$l_curpcb;
  struct _acb * a=vmalloc(sizeof(struct _acb));
  bzero(a,sizeof(struct _acb));
  a->acb$l_pid=p->pid;
  a->acb$l_ast=astadr;
  a->acb$l_astprm=astprm;
  sch$qast(p->pid,PRI$_NULL,a);
}
