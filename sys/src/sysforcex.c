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
#include<cwpsdef.h>
#include<cdrpdef.h>
#include<rddef.h>
#include<ssdef.h>

asmlinkage int exe$exit(unsigned int code);

asmlinkage int exe$forcex(unsigned int *pidadr, void *prcnam, unsigned int code){
  struct _pcb *p;
  struct _acb *a;
  struct _pcb * retpcb;
  unsigned long ipid, epid;
  int sts;
#if 0
  if (pidadr && ((*pidadr)&0x80000000)) 
    return cwps$forcex(pidadr,prcnam,code);
#endif
  sts=exe$nampid(current,pidadr,prcnam,&retpcb,&ipid,&epid);
  p=retpcb;
  if (sts==SS$_REMOTE_PROC)
    return cwps$forcex(pidadr,prcnam,code);
  vmsunlock(&SPIN_SCHED,0);
  if (!p) return 0;
  p->pcb$l_sts|=PCB$M_FORCPEN;
  a=kmalloc(sizeof(struct _acb),GFP_KERNEL);
  bzero(a,sizeof(struct _acb));
  a->acb$l_pid=p->pcb$l_pid;
  a->acb$l_ast=&exe$exit;
  a->acb$l_astprm=code;
  return sch$qast(p->pcb$l_pid,PRI$_RESAVL,a);
  
  /*no cwps*/
}

forcex_msg(){}
forcex_dg(){}
forcex_err(){}

int cwps$forcex(unsigned int *pidadr, void *prcnam, unsigned int code){
  struct _cdrp * c=vmalloc(sizeof(struct _cdrp));
  struct _scs_rd *r;
  struct _cdt * cdt;
  struct _cwpssrv * cwpssrv=vmalloc(sizeof(struct _cwpssrv)+100);
  struct _cwpsfex * cwpsfex=((unsigned long)cwpssrv)+sizeof(struct _cwpssrv);
  cwpssrv->cwpssrv$b_subtype=CWPSSRV$K_FORCEX;
  if (pidadr) cwpssrv->cwpssrv$l_sought_epid=*pidadr&0x7fffffff;
  cwpsfex->cwpsfex$l_code=code;
  bzero(c,sizeof(struct _cdrp));
  c->cdrp$l_rwcptr=0;
  c->cdrp$l_rspid=scs_std$alloc_rspid(0,0,c,0);
  scs_std$find_rdte( c->cdrp$l_rspid, &r);
  r->rd$l_cdrp=c;
  //printk("cluster forcex temporary broke?\n");
  cdt=find_a_cdt(forcex_msg);
  if (cdt==0) {
    printk("no forcex_msg\n");
    return 0;
  }

  c->cdrp$l_cdt=cdt;
  c->cdrp$w_cdrpsize=sizeof(struct _cwpssrv);
  c->cdrp$l_msg_buf=cwpssrv;
  c->cdrp$l_xct_len=100+sizeof(struct _cwpssrv);
  scs_std$senddg(0,100+sizeof(struct _cwpssrv),c);
  vfree(c);
  vfree(cwpssrv);
  return 0;
}
