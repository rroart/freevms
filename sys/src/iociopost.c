#include<linux/sched.h>
#include"../../freevms/lib/src/irpdef.h"
#include"../../freevms/lib/src/acbdef.h"
#include"../../freevms/lib/src/ipldef.h"
#include"../../freevms/lib/src/pridef.h"
#include"../../freevms/pal/src/ipl.h"
#include "../../freevms/sys/src/system_data_cells.h"

dirpost(void) {
  printk("doing dirpost\n");
}

bufpost(void) {
  printk("doing bufpost\n");
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

  if (i->irp$w_sts & IRP$M_BUFIO) goto bufio;

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

