#include<linux/sched.h>
#include"../../freevms/lib/src/irpdef.h"
#include"../../freevms/lib/src/acbdef.h"
#include"../../freevms/lib/src/ipldef.h"
#include"../../freevms/lib/src/pridef.h"
#include"../../freevms/pal/src/ipl.h"
#include "../../freevms/sys/src/system_data_cells.h"

dirpost(struct _irp * i) {
  printk("doing dirpost\n");
}

bufpost(struct _irp * i) {
  struct _acb * a=(struct _acb *) i;
  printk("doing bufpost\n");
  /* do iosb soon? */
  if (i->irp$l_iosb) {
    bcopy(&i->irp$l_iost1,i->irp$l_iosb,8);
  }
  if (i->irp$l_bcnt) {
    bcopy(i->irp$l_svapte,i->useraddress,i->irp$l_bcnt);
  }
  if (a->acb$l_ast) {
    a->acb$b_rmod&=~ACB$M_KAST;
    sch$qast(i->irp$l_pid,PRI$_NULL,i);
  }
}

asmlinkage void ioc$iopost(void) {
  struct _irp * i;
  struct _pcb * p;
  
  if (intr_blocked(IPL$_IOPOST))
    return;
  
  regtrap(REG_INTR, IPL$_IOPOST);

  setipl(IPL$_IOPOST);

  printk("iopost %x %x %x %x\n",&ioc$gq_postiq,ioc$gq_postiq,current->pid,ioc$gq_postiq>>32); //,&ioc$gq_postiq,ioc$gq_postiq &ioc$gq_postiq,ioc$gq_postiq);
 again:
  if (!rqempty(&ioc$gq_postiq)) {
    i=remqhi(&ioc$gq_postiq,i);
  } else {
    return;
  }
  p=find_process_by_pid(i->irp$l_pid);

#if 0
  // these two sch should not be here permanently
  sch$postef(p->pid,PRI$_IOCOM,i->irp$b_efn);
  sch$qast(p->pid,PRI$_IOCOM,i);

  return; // the rest is not finished
#endif

  if (i->irp$l_sts & IRP$M_BUFIO) goto bufio;

 dirio:
  i->irp$b_rmod|=ACB$M_KAST;
  ((struct _acb *) i)->acb$l_kast=dirpost;
  ((struct _acb *) i)->acb$l_astprm=i;
  /* find other class than 1 */
  sch$postef(p->pid,PRI$_IOCOM,i->irp$b_efn);
  sch$qast(p->pid,PRI$_IOCOM,i);
  goto again;

 bufio:

  i->irp$b_rmod|=ACB$M_KAST;
  ((struct _acb *) i)->acb$l_kast=bufpost;
  ((struct _acb *) i)->acb$l_astprm=i;
  /* find other class than 1 */
  sch$postef(p->pid,PRI$_IOCOM,i->irp$b_efn);
  sch$qast(p->pid,PRI$_IOCOM,i);
  goto again;
}

void ioc$myiopost(struct _pcb * p,unsigned long priclass) {
  sch$postef(p,priclass);
}

