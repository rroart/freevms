// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/smp.h>
#include<linux/kernel.h>
#include<asm/hw_irq.h>
#include<linux/sched.h>
#include<cxbdef.h>
#include<bufiodef.h>
#include<dyndef.h>
#include<irpdef.h>
#include<acbdef.h>
#include<ipldef.h>
#include<pridef.h>
#include<ipl.h>
#include<phddef.h>
#include <system_data_cells.h>
#include <internals.h>
#include <rsndef.h>

kfreebuf(void * d) {
  struct _bufio * bd = d;
  struct _cxb * cx = d;
  switch (bd->bufio$b_type) {
  case DYN$C_BUFIO:

    //    kfree(bd->bufio$ps_pktdata); not necessary?
    kfree(bd);
    break;

  case DYN$C_CXB:
    while (cx) {
      kfree(cx->cxb$ps_pktdata);
      kfree(cx);
      cx=cx->cxb$l_link;
    }
    break;

  default:
    panic("kfreebuf\n");
  }
}

movbuf(struct _irp * i) {
  // still skipping access checks and such
  struct _bufio * bd = i->irp$l_svapte;
  struct _cxb * cx = bd;

  if (bd==0) return;
  if (i->irp$l_bcnt==0) goto end;

  switch (bd->bufio$b_type) {
  case DYN$C_BUFIO:

    if (bd->bufio$w_size==0) goto end;
    if (bd->bufio$ps_uva32==0) goto end;
    memcpy(bd->bufio$ps_uva32,bd->bufio$ps_pktdata,i->irp$l_bcnt);
    break;

  case DYN$C_CXB:
    while (cx) {
      if (cx->cxb$w_length==0) goto skip;
      if (cx->cxb$ps_uva32==0) goto skip;
      memcpy(cx->cxb$ps_uva32,cx->cxb$ps_pktdata,cx->cxb$w_length);
    skip:
      cx=cx->cxb$l_link;
    }
    break;

  default:
    panic("movbuf\n");
  }

 end:
  kfreebuf(bd);
}

dirpost(struct _irp * i) {
  printk("doing dirpost\n");
}

bufpost(struct _irp * i) {
  struct _acb * a=(struct _acb *) i;
  struct _pcb * pcb = smp$gl_cpu_data[0]->cpu$l_curpcb;
  struct _phd * phd = pcb->pcb$l_phd;
  //printk("doing bufpost\n");
  /* do iosb soon? */

  movbuf(i);

  if (i->irp$l_sts&IRP$M_MBXIO)
    sch_std$ravail(RSN$_MAILBOX);

  // dirpost to begin here

  // should be either of these
  phd->phd$l_biocnt++;
  phd->phd$l_diocnt++;

  // copy diagnostic

  // decr ccb$w_ioc

  if (i->irp$l_iosb) {
    bcopy(&i->irp$l_iost1,i->irp$l_iosb,8);
  }

  // do an eventually setting of common event flag

  if (a->acb$l_ast) {
    a->acb$b_rmod&=~ACB$M_KAST;
    a->acb$b_rmod&=~ACB$M_NODELETE;
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
  if (i->irp$l_ast)
    i->irp$b_rmod|=ACB$M_NODELETE;
  ((struct _acb *) i)->acb$l_kast=dirpost;
  // not this? ((struct _acb *) i)->acb$l_astprm=i;
  /* find other class than 1 */
  sch$postef(p->pcb$l_pid,PRI$_IOCOM,i->irp$b_efn);
  sch$qast(p->pcb$l_pid,PRI$_IOCOM,i);
  goto again;

 bufio:
  i->irp$b_rmod|=ACB$M_KAST;
  if (i->irp$l_ast)
    i->irp$b_rmod|=ACB$M_NODELETE;

  // put ioc$bufpost here?
  ((struct _acb *) i)->acb$l_kast=bufpost;
  // not this?  ((struct _acb *) i)->acb$l_astprm=i;
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
  sch$postef(p->pcb$l_pid,priclass);
}

