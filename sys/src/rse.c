// $Id$
// $Locker$

#include<linux/sched.h>
#include<asm/hw_irq.h>

#include"../../freevms/sys/src/sysgen.h"
#include"../../freevms/sys/src/rse.h"
#include"../../freevms/lib/src/pridef.h"
#include"../../freevms/lib/src/evtdef.h"
#include"../../freevms/lib/src/statedef.h"
#include"../../freevms/lib/src/cpudef.h"

/* don't want to recompile all yet */
#define        PCB$M_INQUAN    0x8
#define PCB$M_WAKEPEN   0x1000

extern int mydebug;

int sch$qend(struct _pcb * p) {
  int cpuid=smp_processor_id();
  struct _cpu * cpu=smp$gl_cpu_data[cpuid];

  p->pcb$l_pixhist=p->pcb$l_pixhist << 1;
  p->phd$w_quant = -QUANTUM/10;
  p->pcb$l_onqtime=exe$gl_abstim_tics;
  p->pcb$l_sts=p->pcb$l_sts & ( ~ PCB$M_INQUAN );
  if (ffs(sch$gl_comoqs)) {
    p->pcb$b_pri=p->pcb$b_prib;
    /* cpu->cpu$b_cur_pri=
       not quite finished
     */
  }
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
	//SOFTINT_RESCHED_VECTOR;
	 p->need_resched = 1;       
	 /* lacks some sch stuff */
    }
    else
      {
	p->need_resched=1;
	//	SOFTINT_RESCHED_VECTOR; a bit too early. get scheduling in interrupt and crash
      }
  }
  return 1;
}

int sch$pixscan(void) {
  unsigned long tmp;
  struct _pcb *tmp2;
  unsigned long comqs=sch$gl_comqs & 0x7fff0000;
  int tmppri,i,scanned=sgn$gw_pixscan;

  //printk("pixscan 1\n");

  if (!sgn$gw_pixscan) return;
  /* get sched spinlock */
  if (!((sch$gl_comqs & 0x7fff0000) || (sch$gl_comoqs & 0x7fff0000))) goto out;
  tmppri=ffs(comqs);
  tmppri--;
  
  //printk("pri %x|",tmppri);

  for(i=30;i>tmppri,scanned;i--) {
    tmp=sch$aq_comh[i];
    if(*(unsigned long *)tmp == tmp) {; } else {
      tmp2=tmp;
      do {
	//printk("tmp2 %x|",tmp2);
	if (!(tmp2>=&sch$aq_comh[0] && tmp2<=&sch$aq_comh[33])) {
	  if (tmp2->pcb$l_onqtime+100*sch$gw_dormantwait<exe$gl_abstim_tics) {
	    tmp2->pcb$l_pixhist|=1;
	    sch$chsep(tmp2,tmppri);
	    scanned--;
	    if (!scanned) goto out;
	  } else {
	    goto out;
	  }
	}
	tmp2=tmp2->pcb$l_sqfl;
      } while (tmp2!=tmp);
    }
  }
  out:
    /* release spinlock */
  //printk("\n2\n");
    return;
}

// need more statequeues before sch$chse*
  void sch$chse(struct _pcb * p, unsigned char class) {
    unsigned char pri=0,newpri;

    switch (class) {
    case PRI$_NULL:
      pri=0;
      break;
    case PRI$_IOCOM:
      pri=2;
      break;
    case PRI$_TIMER:
    case PRI$_RESAVL:
      pri=3;
      break;
    case PRI$_TOCOM:
      pri=4;
      break;
    case PRI$_TICOM:
      pri=6;
      break;
    }
    newpri=p->pcb$b_prib-pri;
    if (newpri<p->pcb$b_pri) p->pcb$b_pri=newpri;
    if (p->pcb$b_pri<16) p->pcb$b_pri=16;
  }

  void sch$chsep(struct _pcb * p,unsigned char newpri) {
    p->pcb$b_pri=newpri;
    if (p->pcb$b_pri<16) p->pcb$b_pri=16;
  }

void sch$wake(unsigned long pid) {
  struct _pcb * p=find_process_by_pid(pid);
  p->pcb$l_sts &= PCB$M_WAKEPEN;
  sch$rse(p,PRI$_RESAVL,EVT$_WAKE);
}

void sch$swpwake(void) {
  if (sch$gl_comoqs) goto wake;

  return;
 wake:
  sch$wake(sch$gl_swppid); /*not yet set*/
  return;
}

void sch$rse(struct _pcb * p, unsigned char class, unsigned char event) {
  unsigned long dummy;
  struct _pcb *p2;

  switch (event) {
  case EVT$_WAKE:
    switch (p->pcb$w_state) {
    case SCH$C_HIB:
    case SCH$C_HIBO:
      goto wake;
    }     
  case EVT$_SWPOUT:
    switch (p->pcb$w_state) {
    case SCH$C_COM:
    case SCH$C_HIB:
    case SCH$C_LEF:
    case SCH$C_SUSP:
      goto swpout;
    }
  case EVT$_AST:
    switch (p->pcb$w_state) {
    case SCH$C_SUSP:
    case SCH$C_SUSPO:
    case SCH$C_COM:
    case SCH$C_COMO:
    case SCH$C_CUR:
      return;
    default:
      goto ast;
    }
  }
  return;

 wake:
  return;

 swpout:
  if (p->pcb$w_state!=SCH$C_COM) {
    p2=remque(p,dummy);
    sch$aq_wqhdr[p->pcb$w_state].wqh$l_wqcnt--;
    (p->pcb$w_state)++;
    insque(p,sch$aq_wqhdr[p->pcb$w_state].wqh$l_wqfl);
    sch$aq_wqhdr[p->pcb$w_state].wqh$l_wqcnt++;
  } else {
    int tmppri=p->pcb$b_pri;
    unsigned long qhead;

    p2=remque(p,dummy);
    if (sch$aq_comh[tmppri]==((struct _pcb *) sch$aq_comh[tmppri])->pcb$l_sqfl)
      sch$gl_comqs=sch$gl_comqs & (~(1 << tmppri));
    (p->pcb$w_state)++;
    sch$gl_comoqs=sch$gl_comoqs | (1 << tmppri);
    qhead=sch$aq_comot[tmppri];
    insque(p,qhead);
  }
  return;

 ast:
  return;

}

void sch$change_cur_priority(struct _pcb *p, unsigned char newpri) {
  /* lacks sch$al_cpu etc stuff */
  int tmppri;
  struct _pcb * p2;
  int cpuid = smp_processor_id();
  struct _cpu * cpu=smp$gl_cpu_data[cpuid];
  cpu->cpu$b_cur_pri=newpri;
  p2=cpu->cpu$l_curpcb;
  p2->pcb$b_pri=newpri;
  /* lacks sch$al_cpu etc stuff */
  tmppri=ffs(sch$gl_comqs);
  tmppri--;
  if (newpri>=tmppri) return;
  SOFTINT_RESCHED_VECTOR; /* or set need_resched if interrupt probs */
  // interprocessor interrupt not implemented?
}

void sch$one_sec(void) {
  sch$pixscan();
    }
