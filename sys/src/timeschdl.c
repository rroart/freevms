// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004. Based on timer.c. 

#include<queue.h>
#include <asmlink.h>
#include <pridef.h>
#include<ipldef.h>
#include<lkbdef.h>
#include<phddef.h>
#include<ipl.h>
#include <linux/linkage.h>
#include <linux/sched.h>

#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/param.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/smp.h>

#include <asm/io.h>
#include <asm/smp.h>
#include <asm/irq.h>
#include <asm/msr.h>
#include <asm/delay.h>
#include <asm/mpspec.h>
#include <asm/uaccess.h>
#include <asm/processor.h>

#include <linux/mc146818rtc.h>
#include <linux/timex.h>
#include <linux/config.h>

#include <asm/fixmap.h>
#include <asm/cobalt.h>

/*
 * for x86_do_profile()
 */
#include <linux/irq.h>

#include <linux/kernel_stat.h>

#include <asm/uaccess.h>
#include <asm/hw_irq.h>
#include <sysgen.h>
#include <rse.h>
#include <system_data_cells.h>
#include <internals.h>

extern int pid1count; /* Will be removed in the future */
extern int pid0count; /* Will be removed in the future */

extern int countme2;

#define TICK_SIZE tick

extern rwlock_t xtime_lock;

extern spinlock_t i8253_lock;

extern unsigned long last_tsc_low;

extern int mydebug5,mydebug6,in_idle_while;

extern int delay_at_last_interrupt;

extern long last_rtc_update;

extern int use_tsc;

int exetimeout=0;

void exe$timeout(void) {
  /* do sch$swpwake() if appropiate ???? */
  sch$swpwake();
  exe$gl_abstim++;
  /* do erl$wake() if appropiate ???? */
  /* ecc$reenable() */
  /* scan i/o db etc */
  /* scan crbs */
  /* if running monitor... */
  /* scan fork and wait queue. soon to be implemented */
  /* scan lock mgr etc */
  if (lck$gq_timoutq)
    if (exe$gl_abstim>=((struct _lkb *)lck$gq_timoutq)->lkb$l_duetime)
      lck$searchdlck();
  sch$one_sec();
  /* sch$ravail() */
#ifdef __i386__
  if (exetimeout) printk("exe$timeout %x %x %x\n",cur_task->pcb$l_pid,current->pcb$l_pid,current->thread.eip);
#else
  if (exetimeout) printk("exe$timeout %x\n",current->pcb$l_pid);
#endif
}

extern int vmstimerconf;

void printtq(struct _tqe * t) {
  printk("%x %x %x %x\n",t,t->tqe$l_tqfl,t->tqe$q_delta,t->tqe$q_time);
}

asmlinkage void exe$swtimint(void) {
  static signed int times=0;

  if (intr_blocked(IPL$_TIMERFORK))
      return;

  regtrap(REG_INTR, IPL$_TIMERFORK);

  setipl(IPL$_TIMERFORK);

  times++;
  if (current->pcb$w_quant>=0 && current->pcb$w_quant<128) 
    sch$qend(current);

  vmslock(&SPIN_TIMER,IPL$_TIMER);
  vmslock(&SPIN_HWCLK,IPL$_HWCLK);

  //  printk(".");
  /* check tqe from EXE$GL_TQFL */
  if (smp_processor_id()==0) {
    if (vmstimerconf && exe$gq_systime>=exe$gq_1st_time) {
      struct _tqe * t, * dummy=0;
      //    printk(".");
      //if (times>=0 && times<5)   printtq(exe$gl_tqfl);
      //if (times>=0 && times<5)   printtq(exe$gl_tqfl->tqe$l_tqfl);
      t=remque(exe$gl_tqfl->tqe$l_tqfl,dummy);
      //if (times>=0 && times<5)   printtq(t);
      //if (times>=0 && times<5)   printtq(exe$gl_tqfl);
      if ((t->tqe$b_rqtype & TQE$M_TQTYPE) == TQE$C_TMSNGL) {
	sch$postef(t->tqe$l_pid,PRI$_TIMER,t->tqe$b_efn);
	sch$qast(t->tqe$l_pid,PRI$_TIMER,(struct _acb *) t);
	//	void (*f)(void);
	//	f=t->tqe$l_fpc;
	//	if (f) f();
	//	printk("herexxx\n");
	//		printk(KERN_EMERG "before f %x %x %x %x\n",f,t->tqe$l_fpc,exe$timeout,&exe$timeout);
      }
      if ((t->tqe$b_rqtype & TQE$M_TQTYPE) == TQE$C_SSSNGL) {
	void (*f)(void);
	f=t->tqe$l_fpc;
	//		printk(KERN_EMERG "before f %x %x %x %x\n",f,t->tqe$l_fpc,exe$timeout,&exe$timeout);
	if (f) f();
	//		printk(KERN_EMERG "after f %x %x %x %x\n",f,t->tqe$l_fpc,exe$timeout,&exe$timeout);
      }
      if ((t->tqe$b_rqtype & TQE$M_TQTYPE) == TQE$C_WKSNGL) {
	sch$wake(t->tqe$l_pid);
	//	void (*f)(void);
	//	f=t->tqe$l_fpc;
	//	printk(KERN_EMERG "xbefore f %x %x %x %x\n",f,t->tqe$l_fpc,exe$timeout,&exe$timeout);
      }
      if (t->tqe$b_rqtype & TQE$M_REPEAT) {
	t->tqe$q_time=exe$gq_systime+t->tqe$q_delta;
	//	if (times>=0 && times<5) printk("i\n");
	//      if (times>=0 && times<5)   printtq(t);
	//if (times>=0 && times<5)   printtq(exe$gl_tqfl);
	//	printk(":");
	if (0 && times>1000 && times<1200) {
	  printtq(t);
	  printtq(exe$gl_tqfl);
	  printtq(exe$gl_tqfl->tqe$l_tqfl);
	  printtq(exe$gl_tqfl->tqe$l_tqfl->tqe$l_tqfl);
	}
	exe$instimq(t);
	if (0 && times>1000 && times<1200) {
	  printtq(t);
	  printtq(exe$gl_tqfl);
	  printtq(exe$gl_tqfl->tqe$l_tqfl);
	  printtq(exe$gl_tqfl->tqe$l_tqfl->tqe$l_tqfl);
	  printk("i");
	}
	//printk(";");
	//if (times>=0 && times<5)   printtq(t);
	//if (times>=0 && times<5)   printtq(exe$gl_tqfl);
	//if (times>=0 && times<5) printk("i\n");
	//	insque(t,exe$gl_tqfl); /* move this to a sort */
      }
      exe$gq_1st_time=exe$gl_tqfl->tqe$l_tqfl->tqe$q_time;
    }
  }
  //	if (times==4) { unsigned long long i=1;
  //	cli();
  //	for(i=1;i!=0;i++) ;}
  //printk(",");
  vmsunlock(&SPIN_HWCLK,IPL$_TIMER);
  vmsunlock(&SPIN_TIMER,-1);
}

/* vax has 100 Hz clock interrupts. Quantum is in those 10 ns units */
#ifdef __arch_um__
#define QUANTADD 1
#endif
#ifdef __i386
/* 100Hz interrupts here */
#define QUANTADD 1
#endif

#ifdef __arch_um__
extern int hwclkdone;
#endif

void exe$hwclkint(int irq, void *dev_id, struct pt_regs *regs) {
  /* reset pr$_iccs */
  /* smp sanity timer */

#ifdef __arch_um__
int hwclkdone=1;
#endif

#if 0
// this did not go well, for some reason
  if (intr_blocked(IPL$_HWCLK))
      return;

  regtrap(REG_INTR, IPL$_HWCLK);

  setipl(IPL$_HWCLK);
#endif

  // on UP or primary SMP cpu:
  if (smp_processor_id()==0) {
  
    int count;

    write_lock(&xtime_lock);

#ifndef __arch_um__
    if (use_tsc)
      {
	rdtscl(last_tsc_low);

	spin_lock(&i8253_lock);
	outb_p(0x00, 0x43);

	count = inb_p(0x40);
	count |= inb(0x40) << 8;
	spin_unlock(&i8253_lock);

	count = ((LATCH-1) - count) * TICK_SIZE;
	delay_at_last_interrupt = (count + LATCH/2) / LATCH;
      }
 
#ifdef CONFIG_X86_IO_APIC
    if (timer_ack) {
      spin_lock(&i8259A_lock);
      outb(0x0c, 0x20);
      inb(0x20);
      spin_unlock(&i8259A_lock);
    }
#endif
#endif /* __arch_um__ */

    exe$gq_systime+=exe$gl_ticklength;

    (*(unsigned long *)&jiffies)++;
    exe$gl_abstim_tics=jiffies;

    if (exe$gq_systime>=exe$gq_1st_time) 
      SOFTINT_TIMERFORK_VECTOR;

#ifndef CONFIG_SMP
    {
      int user_tick=user_mode(regs);
      struct task_struct *p = current;
      int cpu = smp_processor_id(), system = user_tick ^ 1;
      //	if (mydebug6 && in_idle_while)
      //  printk(".");
      if (mydebug5 && !countme2--) { 
	countme2=500; printk(",");
	printk("timer %x %x %x\n",p->pcb$l_pid,p->pcb$w_quant,p->pcb$b_pri);
      }
      //  printk(":");
      //	if (p->pcb$l_pid==2) { int i; for(i=0;i<1000000;i++) ; }
      // { int i; for (i=0; i<1000000; i++ ) ; }}
      update_one_process(p, user_tick, system, cpu);
      if (p->pcb$l_pid==0) { if (++pid0count>5) { pid0count=0; p->need_resched=1;}}  /* Will be removed in the future */
      if (p->pcb$l_pid==INIT_PID) { if (++pid1count>5) { pid1count=0; p->need_resched=1;}}  /* Will be removed in the future */
      if (p->pcb$l_pid) {
	p->pcb$l_phd->phd$l_cputim++;
	p->pcb$w_quant+=QUANTADD;
	if (++p->pcb$w_quant  >= 0 ) {
	  if (p->pcb$w_quant<128) {
	    SOFTINT_TIMERFORK_VECTOR;
	    //		    sch$resched();
	  }
	}
	if (p->pcb$b_prib == 31)
	  kstat.per_cpu_nice[cpu] += user_tick;
	else
	  kstat.per_cpu_user[cpu] += user_tick;
	kstat.per_cpu_system[cpu] += system;
      } else if (local_bh_count(cpu) || local_irq_count(cpu) > 1)
	kstat.per_cpu_system[cpu] += system;
    }
#endif

#ifndef CONFIG_VMS
    mark_bh(TIMER_BH);
    if (TQ_ACTIVE(tq_timer))
      mark_bh(TQUEUE_BH);
#endif

#ifdef __i386__
#ifndef CONFIG_X86_LOCAL_APIC
    if (!user_mode(regs))
      x86_do_profile(regs->eip);
#else
    if (!using_apic_timer)
      smp_local_timer_interrupt(regs);
#endif
#endif

    if ((time_status & STA_UNSYNC) == 0 &&
#ifndef __arch_um__
	xtime.tv_sec > last_rtc_update + 660 &&
#endif
	xtime.tv_usec >= 500000 - ((unsigned) tick) / 2 &&
	xtime.tv_usec <= 500000 + ((unsigned) tick) / 2) {
#ifndef __arch_um__
      if (set_rtc_mmss(xtime.tv_sec) == 0)
	last_rtc_update = xtime.tv_sec;
      else
	last_rtc_update = xtime.tv_sec - 600;
#endif
    }

    write_unlock(&xtime_lock);
  }  
}
