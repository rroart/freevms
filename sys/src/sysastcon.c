// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <system_data_cells.h>
#include <acbdef.h>
#include <pridef.h>
#include <cpudef.h>
#include <psldef.h>
#include <ssdef.h>
#include <linux/smp.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>

int exe$setast(char enbflg) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=ctl$gl_pcb;
  int retval;
  if (p->pcb$b_asten&(1<<p->psl_prv_mod))
    retval=SS$_WASSET;
  else
    retval=SS$_WASCLR;
  if (p->oldpsl_prv_mod!=PSL$C_USER) {
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

int exe$dclast(void (*astadr)(__unknown_params), unsigned long astprm, unsigned int acmode) {
  struct _cpu * cpu=smp$gl_cpu_data[smp_processor_id()];
  struct _pcb * p=ctl$gl_pcb;
  struct _acb * a=kmalloc(sizeof(struct _acb),GFP_KERNEL);
  bzero(a,sizeof(struct _acb));
  a->acb$l_pid=p->pcb$l_pid;
  a->acb$l_ast=astadr;
  a->acb$l_astprm=astprm;
  sch$qast(p->pcb$l_pid,PRI$_NULL,a);
  return SS$_NORMAL;
}
