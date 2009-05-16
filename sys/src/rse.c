// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file rse.c
   \brief report system event
   \author Roar Thronæs
*/

#include<linux/sched.h>
#include <asmlink.h>
#include<asm/hw_irq.h>

#include<sysgen.h>
#include<rse.h>
#include<pridef.h>
#include<evtdef.h>
#include<statedef.h>
#include<cpudef.h>
#include<system_data_cells.h>
#include<ipl.h>
#include<internals.h>
#include <exe_routines.h>
#include <sch_routines.h>
#include <smp_routines.h>
#include <misc_routines.h>
#include<queue.h>
#include<ipldef.h>
#include<cpbdef.h>

extern int mydebug;

void sch$unwait(struct _pcb * p);
void sch$rse(struct _pcb * p, unsigned char class, unsigned char event);
void sch$swpwake(void);
void sch$chsep(struct _pcb * p,unsigned char newpri);
void sch$change_cur_priority(struct _pcb *p, unsigned char newpri);

int sch$qend(struct _pcb * p) {
  int cpuid=smp_processor_id();
  struct _cpu * cpu=smp$gl_cpu_data[cpuid];
  vmslock(&SPIN_SCHED,IPL$_SCHED);

  p->pcb$l_pixhist=p->pcb$l_pixhist << 1;
  p->pcb$w_quant = -QUANTUM;
  p->pcb$l_onqtime=exe$gl_abstim_tics;
  p->pcb$l_sts=p->pcb$l_sts & ( ~ PCB$M_INQUAN );
#if 0
// not yet
  if (ffs(sch$gl_comoqs)) {
    p->pcb$b_pri=p->pcb$b_prib;
    /* cpu->cpu$b_cur_pri=
       not quite finished
     */
  }
#endif
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
	if (p->pcb$b_pri != p->pcb$b_prib)
	  sch$change_cur_priority(p,p->pcb$b_pri+1);
	//	 sch$resched(); /*no interrupt yet*/ /*did not work*/
	//SOFTINT_RESCHED_VECTOR;
	// p->need_resched = 1;       
	 /* lacks some sch stuff */
    }
    else
      {
	//p->need_resched=1;
#if 1
	SOFTINT_RESCHED_VECTOR; // a bit too early. get scheduling in interrupt and crash
#else
	sch$gl_idle_cpus &= ~(1<<p->pcb$l_cpu_id);
#endif
      }
  }
  vmsunlock(&SPIN_SCHED,-1);
  return 1;
}

/**
   \brief pixscan priority boots - see 5.2 12.5.6
*/

int sch$pixscan(void) {
  unsigned long tmp;
  struct _pcb *tmp2;
  unsigned long comqs=sch$gl_comqs & 0x7fff0000;
  int tmppri,i,scanned=sgn$gw_pixscan;

  //printk("pixscan 1\n");

  /** test sgn pixscan and maybe return */
  if (!sgn$gw_pixscan) return;
  /** get sched spinlock */
  vmslock(&SPIN_SCHED,-1);
  /** determine if any processes eligible for boost, use comq and comoq */
  /** if none, return */
  if (!((sch$gl_comqs & 0x7fff0000) || (sch$gl_comoqs & 0x7fff0000))) goto out;
  tmppri=ffs(comqs);
  tmppri--;
  /** compute highest pri of these two - MISSING */
  
  //printk("pri %x|",tmppri);
  goto out; /** not implement this yet - MISSING actually partially in place */
  /** use abstim bit as coin - MISSING */
  /** scan queues */
  /** continue until max reached or max boosts reached */
  /** and DORMANTWAIT reached */
  for(i=30;i>tmppri && scanned;i--) {
    tmp=*(unsigned long *)&sch$aq_comh[i];
    if(*(unsigned long *)tmp == tmp) {; } else {
      tmp2=tmp;
      do {
	//printk("tmp2 %x|",tmp2);
	if (!(tmp2>=&sch$aq_comh[0] && tmp2<=&sch$aq_comh[33])) {
	  /** compare onqtime dormantwait abstim_tics */
	  if (tmp2->pcb$l_onqtime+100*sch$gw_dormantwait<exe$gl_abstim_tics) {
	    /** set loworder bits in pixhist if found */
	    tmp2->pcb$l_pixhist|=1;
	    /** if found, boost */
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
  /** release spinlock */
  //printk("\n2\n");
  vmsunlock(&SPIN_SCHED,-1);
  return;
}

int myp3, myp2, mycomq, mypri, sqfl;

/**
   \brief change pri state etc according to priclass etc - see 5.2 12.5.5
*/

void sch$chsep(struct _pcb * p,unsigned char newpri) {
  int ipl = getipl();
  if (ipl != 8 || SPIN_SCHED.spl$l_spinlock == 0)
    panic("rse %x %x\n",ipl,SPIN_SCHED.spl$l_spinlock);
  struct _pcb * p2, *p3 , *dummy = 0;
  p3=p->pcb$l_sqfl;
  myp3=p3;
  mycheckaddr(0);
  p2=remque(p,dummy);
  myp2=p2;
  mycomq=sch$gl_comqs;
  mypri=p2->pcb$b_pri;
  sqfl=p3->pcb$l_sqfl;
  /** set comqs or comoqs bits */
  if (p3==p3->pcb$l_sqfl) {
      if (p2->pcb$w_state==SCH$C_COM) {
	sch$gl_comqs&=(~(1 << p2->pcb$b_pri));
      } else if (p2->pcb$w_state==SCH$C_COMO) {
	sch$gl_comoqs&=(~(1 << p2->pcb$b_pri));
      }
  }
  mycheckaddr(0);
  p->pcb$b_pri=newpri;
  /** if now computable process is outswapped... - MISSING */
  //  if (!(p->pcb$l_sts & PCB$M_RES)) { not yet used
  if (0) {
    sch$swpwake();
    p2->pcb$w_state=SCH$C_COMO;
    p2->state=TASK_RUNNING;
    mycheckaddr(0);
#ifdef __i386__
    insque(p2,*(unsigned long *)&sch$aq_comot[newpri]);
#else
    insque(p2,*(unsigned long *)&sch$aq_comoh[newpri][1]);
#endif
    sch$gl_comoqs|=(1 << newpri);
    mycheckaddr(0);
    return;
  }
  /** if resident, change state to COM */
  p2->pcb$w_state=SCH$C_COM;
  p2->state=TASK_RUNNING;
  mycheckaddr(0);
  if (task_on_comqueue(p2))
    panic("t\n");
#if 0
  if (p2==ctl$gl_pcb)  // another temp workaround
    panic("t2\n");
#endif
  /** do affinities */
  if (sch$gl_idle_cpus || sch$gl_capability_sequence != p2->pcb$l_capability_seq) {
    // any one idle?
    // recompute caps if necessary
    if (sch$gl_capability_sequence != p2->pcb$l_capability_seq)
      p2->pcb$l_current_affinity = sch$calculate_affinity(p2->pcb$l_affinity);
    // right cpu idle
    if (p2->pcb$l_current_affinity & sch$gl_idle_cpus) {
      if (p2->pcb$l_capability & CPB$M_IMPLICIT_AFFINITY) {
	int isset = sch$gl_idle_cpus & (1<<p2->pcb$l_affinity);
	sch$gl_idle_cpus &= ~(1<<p2->pcb$l_affinity);
	if (isset)
	  goto do_return;
      }
      sch$gl_idle_cpus &= ~p2->pcb$l_current_affinity;
      goto do_return;
    }
    // change to aq_preempt_mask later
    // anyone at all to preempt?
    if (sch$gl_active_priority & ((1<<(31-newpri))-1) & (1<<(31-mypri)-1)) {
      int lowpri=ffs(sch$gl_active_priority);
      lowpri--;
      int cpus=sch$al_cpu_priority[31-lowpri];
      if ((cpus & p2->pcb$l_current_affinity) == 0) {
	do {
	  int tmp = (sch$gl_active_priority & ((1<<(31-newpri))-1));
	  tmp &= ~(1<<lowpri);
	  lowpri=ffs(tmp);
	  if(lowpri == 0)
	    goto do_return;
	  lowpri--;
	  int cpus=sch$al_cpu_priority[31-lowpri];
	} while ((cpus & p2->pcb$l_current_affinity) == 0);
      }
      int wanted;
      if (p2->pcb$l_capability & CPB$M_IMPLICIT_AFFINITY)
	wanted = p2->pcb$l_affinity;
      else
	wanted = ctl$gl_pcb->pcb$l_cpu_id;
#if 1
      if (wanted != smp_processor_id())
	smp_send_work(CPU$M_RESCHED, wanted);
      else
	SOFTINT_RESCHED_VECTOR; /* or set need_resched if interrupt probs */
#else
      sch$gl_idle_cpus &= ~(1<<p->pcb$l_cpu_id);
#endif
      goto do_return;
    }
  }
#if 0
 do_resched:
  mycheckaddr(0);
  // and a ipint resched here too, if needed
#if 1
  SOFTINT_RESCHED_VECTOR; // not yet?
#else
  sch$gl_idle_cpus &= ~(1<<p2->pcb$l_cpu_id);
#endif
#endif
  /** insert into comq */
 do_return:
#ifdef __i386__
  insque(p2,*(unsigned long *)&sch$aq_comt[newpri]);
#else
  insque(p2,*(unsigned long *)&sch$aq_comh[newpri][1]);
#endif
  sch$gl_comqs|=(1 << newpri);
#if 0
  p->pcb$l_sts&=~PCB$M_WAKEPEN; // got to have this somewhere, here works
#endif
}

/**
   \brief change pri state etc according to priclass etc - see 5.2 12.5.5
*/

// need more statequeues before sch$chse*
  void sch$chse(struct _pcb * p, unsigned char class) {
    unsigned char pri=0,newpri;

    /** do pri adjustments */

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
    if (newpri>p->pcb$b_pri) newpri=p->pcb$b_pri;
    if (p->pcb$b_pri<16) newpri=p->pcb$b_prib;
    sch$chsep(p,newpri);
  }

/**
   \brief set wake and report it - see 5.2 13.3.1
*/

void sch$wake(unsigned long pid) {
  struct _pcb * p=exe$ipid_to_pcb(pid);
  if (!p) return;
  /** set pending wake flag */
  p->pcb$l_sts |= PCB$M_WAKEPEN;
  /** report wake scheduling event */
  sch$rse(p,PRI$_RESAVL,EVT$_WAKE);
}

void sch$swpwake(void) {
  if (sch$gl_comoqs) goto wake;

  if (sch$gl_mfycnt>0) goto wake;

  return;
 wake:
  sch$wake(sch$gl_swppid); /*not yet set*/
  return;
}

/**
   \brief event reporting - see 5.2 12.5.5
   \param p pcb
   \param class priority class
   \param event type of event
*/

// called sch$report_event in 1.5
void sch$rse(struct _pcb * p, unsigned char class, unsigned char event) {
  unsigned long dummy = 0;
  struct _pcb *p2;
  int ipl = getipl();
  if (ipl != 8 || SPIN_SCHED.spl$l_spinlock == 0)
    panic("rse %x %x\n",ipl,SPIN_SCHED.spl$l_spinlock);

  /** based on current pcb state, check if event is significant for the process */

  /** AST events are signicant to almost all states except SUSPs, COMs, CUR */
  /** result in COM or COMO */

  switch (event) {
  case EVT$_AST:
    switch (p->pcb$w_state) {
    case SCH$C_SUSP:
    case SCH$C_SUSPO:
    case SCH$C_COM:
    case SCH$C_COMO:
    case SCH$C_CUR:
      break;
    default:
      goto ast;
    }

  case EVT$_EVENT: {
    if (p->pcb$w_state==SCH$C_LEF) goto event;
    if (p->pcb$w_state==SCH$C_CEF) goto cef;
    if (p->pcb$w_state==SCH$C_LEFO) goto lefo;
  }

  case EVT$_CEF: {
    if (p->pcb$w_state==SCH$C_CEF) goto cef;
    if (p->pcb$w_state==SCH$C_LEFO) goto lefo;
  }

  case EVT$_LEFO:
    if (p->pcb$w_state==SCH$C_LEFO) goto lefo;

  case EVT$_FPGA:
    if (p->pcb$w_state==SCH$C_FPG) goto fpga;

    /** wake is only signicant for HIBs */

  case EVT$_WAKE:
    switch (p->pcb$w_state) {
    case SCH$C_HIB:
    case SCH$C_HIBO:
      goto wake;
    }     

  case EVT$_RESUME:
    switch (p->pcb$w_state) {
    case SCH$C_SUSP:
    case SCH$C_SUSPO:
      goto resume;
    }

  case EVT$_PFCOM:
    if (p->pcb$w_state==SCH$C_PFW) goto pfcom;

  case EVT$_SETPRI:
    switch (p->pcb$w_state) {
    case SCH$C_COM:
    case SCH$C_COMO:
      goto setpri;
    }

    /** swapout is only significant for COM HIB LEF SUSP */

  case EVT$_SWPOUT:
    switch (p->pcb$w_state) {
    case SCH$C_COM:
    case SCH$C_HIB:
    case SCH$C_LEF:
    case SCH$C_SUSP:
      goto swpout;
    }
  }

  /* bugcheck */
  /** else ignore and return */
  return;

 cef:
  /** test for cef resident */
  if (!(p->pcb$l_sts & PCB$M_RES)) goto lefo;
 event:
  //p->pcb$b_reserved_b1 == SS$_NORMAL; // temp place. iosb will need this
  /* add stuff to pc no use here */
  goto lefo;

 lefo:
  goto common;

 ast:
 colpga:
 fpga:
 wake:
 resume:
 pfcom:
 setpri:

  /** for the most */
 common:
  /** if to be computable - MISSING */
  /** remove from waitqueue - MISSING actually commented out for some earlier(?) serious reason */
  // p2=remque(p,dummy);
  /** adjust waitqueue count */
  if (p->pcb$w_state < SCH$C_COM)
    sch$aq_wqhdr[p->pcb$w_state].wqh$l_wqcnt--;
  /** adjust pcb onqtime */
  p->pcb$l_onqtime+=(exe$gl_abstim_tics-p->pcb$l_waitime);
  sch$unwait(p);
  /** do priority adjustment, set to COM etc */
  sch$chse(p,class);
  return;

  /** for swapout */
 swpout:
  if (p->pcb$w_state!=SCH$C_COM) {
    /** for non-COM */
    /** remove from resident wait queue */
    p2=remque(p,dummy);
    /** adjust waitqueue count */
    sch$aq_wqhdr[p->pcb$w_state].wqh$l_wqcnt--;
    /** insert into outswapped wait queue */
    /** set state */
    (p->pcb$w_state)++;
    insque(p,sch$aq_wqhdr[p->pcb$w_state].wqh$l_wqfl);
    /** adjust waitqueue count */
    sch$aq_wqhdr[p->pcb$w_state].wqh$l_wqcnt++;
  } else {
    /** for COM */
    int tmppri=p->pcb$b_pri;
    unsigned long qhead;

    mycheckaddr(0);
    /** remove from COM pri queue */
    p2=remque(p,dummy);
    /** if com pri queue empty, clear sch$gl_comqs bit */
    if (sch$aq_comh[tmppri]==&sch$aq_comh[tmppri])
      sch$gl_comqs=sch$gl_comqs & (~(1 << tmppri));
    /** set state */
    (p->pcb$w_state)++;
    mycheckaddr(0);
    /** set bit in sch$gl_comoqs */
    sch$gl_comoqs=sch$gl_comoqs | (1 << tmppri);
    /** insert into COMO wait queue */
#ifdef __i386__
    qhead=*(unsigned long *)&sch$aq_comot[tmppri];
#else
    qhead=*(unsigned long *)&sch$aq_comoh[tmppri][1];
#endif
    insque(p,qhead);
    mycheckaddr(0);
  }
  return;
}

/**
   \brief change process priority - see 5.2 12.5.3
*/

void sch$change_cur_priority(struct _pcb *p, unsigned char newpri) {
  /* lacks sch$al_cpu etc stuff */
  int tmppri;
  struct _pcb * p2 = p;
  int cpuid = p2->pcb$l_cpu_id;
  struct _cpu * cpu=smp$gl_cpu_data[cpuid];
  /* lacks sch$al_cpu etc stuff */
  /** clear old pri bit in cpu_priority */
  sch$al_cpu_priority[cpu->cpu$b_cur_pri]=sch$al_cpu_priority[cpu->cpu$b_cur_pri] & (~cpu->cpu$l_cpuid_mask);
  /** if no other at same pri, clear bit in active_priority */
  if (sch$al_cpu_priority[cpu->cpu$b_cur_pri] == 0)
    sch$gl_active_priority &= ~(1<<(31-cpu->cpu$b_cur_pri));
  /** copy new pri to cur_pri and pri */
  cpu->cpu$b_cur_pri=newpri;
  p2->pcb$b_pri=newpri;
  /** set new pri bit in cpu_priority */
  sch$al_cpu_priority[cpu->cpu$b_cur_pri]=sch$al_cpu_priority[cpu->cpu$b_cur_pri] & (~cpu->cpu$l_cpuid_mask);
  /** set corresponding bit in active_priority */
  if (sch$al_cpu_priority[cpu->cpu$b_cur_pri] == 0)
    sch$gl_active_priority &= ~(1<<(31-cpu->cpu$b_cur_pri));
  /** find lsb in comqs */
  tmppri=ffs(sch$gl_comqs);
  tmppri--;
  /** compare with newpri, and return if bigger */
  if (newpri>=tmppri) return;
#if 1
  /** otherwise, request resched softint, locally or by intercpu interrupt */
  if (cpuid != smp_processor_id())
    smp_send_work(CPU$M_RESCHED, cpuid);
  else
    SOFTINT_RESCHED_VECTOR; /* or set need_resched if interrupt probs */
#else
  sch$gl_idle_cpus &= ~(1<<p->pcb$l_cpu_id);
#endif
  // interprocessor interrupt not implemented?
}

/**
   \brief run once in a sec
*/

void sch$one_sec(void) {
  /** mainly run pixscan */
  sch$pixscan();
    }

int sch$waitm(struct _pcb * p, struct _wqh * wq) {
  /* do no phd stuff yet */
  p->pcb$l_waitime=exe$gl_abstim_tics;
  /* check queued ast */
  if (p->phd$b_astlvl==4) {
    sch$sched(0);
    return;
  }
  // sch$sched(0); /* do it anyway until asts are implemented */
  /* check psl and ast stuff */
  if (!(p->psl_cur_mod >= p->pr_astlvl)) { // change to phd?
    sch$sched(0);
    return;
  }
  //  if (p->psl_ipl!=0) 
  {
    union {
      struct {
	unsigned psl_cur_mod:2;
	unsigned psl_prv_mod:2;
	unsigned psl_ipl:5;
	unsigned psl_is:1;
	unsigned psl_intr:1;
      };
      unsigned short psl;
    } savpsl;
    savpsl.psl=p->pslstk[p->pslindex-1];
    if (savpsl.psl_ipl) {
      sch$sched(0);
      return;
    }
  }
  sch$rse(p,PRI$_NULL,EVT$_AST);
  sch$sched(0);
}

int sch$waitl(struct _pcb * p, struct _wqh * wq) {
  /* do a svpctx ? */
  return sch$waitm(p,wq);
}

int sch$waitk(struct _pcb * p, struct _wqh * wq) {
  p->pcb$w_state=wq->wqh$l_wqstate;
  p->state=TASK_UNINTERRUPTIBLE;
#if 0
  if (!task_on_comqueue(p)) {  // scheduling bug fix that may be removed sometime
    panic("toc\n");
  }
#endif
#if 0
  // check
  // check
  {
    struct _pcb * p3=p->pcb$l_sqfl;
    remque(p,0);
    qhead_init(p); // temp measure since we don't insert into wqs
    if (p3==p3->pcb$l_sqfl) {
      sch$gl_comqs&=(~(1 << p->pcb$b_pri));
    }
  }
#endif
  insque(p,&wq->wqh$l_wqfl); // better wait with waitqs? had pcb queue corruption
  // and: insque for ceb waiting etc is now down in syswait.c
  wq->wqh$l_wqcnt++;
  return sch$waitl(p,wq);
}

int sch$wait(struct _pcb * p, struct _wqh * wq) {
  /* remove call frame etc ? */
  return sch$waitk(p,wq);
}

void sch$chsep2(struct _pcb * p,unsigned char newpri) {
  int ipl = getipl();
  if (ipl != 8 || SPIN_SCHED.spl$l_spinlock == 0)
    panic("rse %x %x\n",ipl,SPIN_SCHED.spl$l_spinlock);
  struct _pcb * p2, *p3 , *dummy;
  p3=p->pcb$l_sqfl;
  p2=p;
  /*
  if (p3==p3->pcb$l_sqfl) {
      if (p2->state==TASK_RUNNING) {
	sch$gl_comqs&=(~(1 << p2->pcb$b_pri));
      } else if (p2->pcb$w_state==SCH$C_COMO) {
	sch$gl_comoqs&=(~(1 << p2->pcb$b_pri));
      }
  }
  */
  p->pcb$b_pri=newpri;
  /*
  if (!(p->pcb$l_sts & PCB$M_RES)) {
    sch$swpwake();
    p2->pcb$w_state=SCH$C_COMO;
    p2->state=TASK_RUNNING;
    p2->pcb$w_state = SCH$C_CUR;
    mycheckaddr(0);
#ifdef __i386__
    insque(p2,*(unsigned long *)&sch$aq_comot[newpri]);
#else
    insque(p2,*(unsigned long *)&sch$aq_comoh[newpri][1]);
#endif
    sch$gl_comoqs|=(1 << newpri);
    mycheckaddr(0);
    return;
  }
  */
  //SOFTINT_RESCHED_VECTOR;
  p2->state=TASK_RUNNING;
  p2->pcb$w_state = SCH$C_CUR;
  mycheckaddr(0);
#ifdef __i386__
  insque(p2,*(unsigned long *)&sch$aq_comt[newpri]);
#else
  insque(p2,*(unsigned long *)&sch$aq_comh[newpri][1]);
#endif
  sch$gl_comqs|=(1 << newpri);
  mycheckaddr(0);
}

// need more statequeues before sch$chse*
  void sch$chse2(struct _pcb * p, unsigned char class) {
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
    if (newpri>p->pcb$b_pri) newpri=p->pcb$b_pri;
    if (p->pcb$b_pri<16) p->pcb$b_pri=p->pcb$b_prib;
    sch$chsep2(p,newpri);
  }

void sch$unwait(struct _pcb * p) {

}
