// $Id$
// $Locker$

#include<linux/sched.h>
#include<asm/hw_irq.h>

#include"../../freevms/sys/src/sysgen.h"
#include"../../freevms/sys/src/rse.h"

extern int mydebug;

int sch$qend(struct _pcb * p) {
  p->phd$w_quant = -QUANTUM/10;
  //  p->need_resched = 1;
  if (mydebug) printk("quend %x %x\n",p->pid,p->need_resched);
  {
    //    struct list_head * tmp;
    struct _pcb * e, * next;
    unsigned char c;
    int tmppri;
    //    list_for_each(tmp, &runqueue_head) {
    //      e = list_entry(tmp, struct task_struct, run_list);
    //      if (e->pcb$b_pri <= c) c=e->pcb$b_pri, next=e;
    //    }
    //SOFTINT_RESCHED_VECTOR;
    // return;
    tmppri=ffs(sch$gl_comqs);
    tmppri--;
    if (tmppri<=p->pcb$b_pri) {
	if (p->pcb$b_pri != p->pcb$b_prib) ++p->pcb$b_pri;
	//	 sch$resched(); /*no interrupt yet*/ /*did not work*/
	SOFTINT_RESCHED_VECTOR;
	 p->need_resched = 1;       }
    else
      {
	p->need_resched=1;
	//	SOFTINT_RESCHED_VECTOR; a bit too early. get scheduling in interrupt and crash
      }
  }
  return 1;
}
