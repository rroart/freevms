// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/smp.h>
#include<linux/kernel.h>
#include<asm/hw_irq.h>
#include<linux/sched.h>
#include<irpdef.h>
#include<acbdef.h>
#include<ipldef.h>
#include<pridef.h>
#include<ipl.h>
#include <system_data_cells.h>
#include <internals.h>

dirpost(struct _irp * i) {
  printk("doing dirpost\n");
}

bufpost(struct _irp * i) {
  struct _acb * a=(struct _acb *) i;
  //printk("doing bufpost\n");
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

  //printk("iopost %x %x %x %x\n",&ioc$gq_postiq,ioc$gq_postiq,current->pid,ioc$gq_postiq>>32); //,&ioc$gq_postiq,ioc$gq_postiq &ioc$gq_postiq,ioc$gq_postiq);
 again:
  if (!rqempty(&ioc$gq_postiq)) {
    i=remqhi(&ioc$gq_postiq,i);
  } else {
    if (!aqempty(&smp$gl_cpu_data[smp_processor_id()]->cpu$l_psfl)) {
      i=remque(&smp$gl_cpu_data[smp_processor_id()]->cpu$l_psfl,i);
    } else {
      return;
    }
  }
  p=exe$ipid_to_pcb(i->irp$l_pid);

#if 0
  // these two sch should not be here permanently
  sch$postef(p->pcb$l_pid,PRI$_IOCOM,i->irp$b_efn);
  sch$qast(p->pcb$l_pid,PRI$_IOCOM,i);

  return; // the rest is not finished
#endif

  if (i->irp$l_sts & IRP$M_BUFIO) goto bufio;

 dirio:
  i->irp$b_rmod|=ACB$M_KAST;
  ((struct _acb *) i)->acb$l_kast=dirpost;
  ((struct _acb *) i)->acb$l_astprm=i;
  /* find other class than 1 */
  sch$postef(p->pcb$l_pid,PRI$_IOCOM,i->irp$b_efn);
  sch$qast(p->pcb$l_pid,PRI$_IOCOM,i);
  goto again;

 bufio:
  i->irp$b_rmod|=ACB$M_KAST;

  // put ioc$bufpost here?
  ((struct _acb *) i)->acb$l_kast=bufpost;
  ((struct _acb *) i)->acb$l_astprm=i;
  /* find other class than 1 */
  sch$postef(p->pcb$l_pid,PRI$_IOCOM,i->irp$b_efn);
  sch$qast(p->pcb$l_pid,PRI$_IOCOM,i);
  goto again;

  ioc$bufpost();
}

ioc$bufpost(struct _irp * i){
  i->irp$b_rmod|=ACB$M_KAST; // think this belongs here too
  i->irp$l_wind=bufpost; // really acb$l_kast;
  i->irp$l_astprm=i; // think this belongs here too
}

void ioc$myiopost(struct _pcb * p,unsigned long priclass) {
  sch$postef(p,priclass);
}

