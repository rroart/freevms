// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>
#include<linux/vmalloc.h>
#include<acbdef.h>
#include<pridef.h>
#include<ipldef.h>
#include<internals.h>
#include<system_data_cells.h>

asmlinkage int exe$exit(unsigned int code);

asmlinkage int exe$forcex(unsigned int *pidadr, void *prcnam, unsigned int code){
  struct _pcb *p;
  struct _acb *a;
  p=exe$nampid2(current,pidadr,prcnam);
  vmsunlock(&SPIN_SCHED,0);
  if (!p) return 0;
  p->pcb$l_sts|=PCB$M_FORCPEN;
  a=vmalloc(sizeof(struct _acb));
  bzero(a,sizeof(struct _acb));
  a->acb$l_pid=p->pid;
  a->acb$l_ast=&exe$exit;
  a->acb$l_astprm=code;
  return sch$qast(p->pid,PRI$_RESAVL,a);

  /*no cwps*/
}
