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
#include "../../freevms/sys/src/sysgen.h"
#include "../../freevms/sys/src/rse.h"

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

void exe$timeout(void) {
  /* do sch$swpwake() if appropiate ???? */
  exe$gl_abstim++;
  /* do erl$wake() if appropiate ???? */
  /* ecc$reenable() */
  /* scan i/o db etc */
  /* scan crbs */
  /* if running monitor... */
  /* scan fork and wait queue. soon to be implemented */
  /* scan lock mgr etc */
  sch$one_sec();
  /* sch$ravail() */
}

asmlinkage void exe$swtimint(void) {
  if (current->phd$w_quant>=0 && current->phd$w_quant<128) 
    sch$qend(current);
  /* check tqe from EXE$GL_TQFL */
  if (smp_processor_id()==0) {
    if (exe$gl_abstim_tics>=exe$gq_1st_time) {
      struct _tqe * t, * dummy;
      t=remque(*exe$gl_tqfl,dummy);
      if ((t->tqe$b_rqtype & TQE$M_TQTYPE) == TQE$C_TMSNGL) {
      }
      if ((t->tqe$b_rqtype & TQE$M_TQTYPE) == TQE$C_SSSNGL) {
	void (*f)(void);
	f=t->tqe$l_pc;
	//	printk(KERN_EMERG "before f %x %x %x %x\n",f,t->tqe$l_pc,exe$timeout,&exe$timeout);
	f();
	//	printk(KERN_EMERG "after f %x %x %x %x\n",f,t->tqe$l_pc,exe$timeout,&exe$timeout);
      }
      if ((t->tqe$b_rqtype & TQE$M_TQTYPE) == TQE$C_WKSNGL) {
      }
      if (t->tqe$b_rqtype & TQE$M_REPEAT)
	insque(t,*exe$gl_tqfl);
      
    }
  }
}

void exe$hwclkint(int irq, void *dev_id, struct pt_regs *regs) {
  /* reset pr$_iccs */
  /* smp sanity timer */

  // on UP or primary SMP cpu:
  if (smp_processor_id()==0) {
  
    int count;

    write_lock(&xtime_lock);

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

    exe$gq_systime+=exe$gl_ticklength;

    (*(unsigned long *)&jiffies)++;
    exe$gl_abstim_tics=jiffies;

    if (exe$gl_abstim_tics>=exe$gq_1st_time) exe$swtimint();
    /* SOFTINT_TIMER_VECTOR; */

#ifndef CONFIG_SMP
    {
      int user_tick=user_mode(regs);
      struct task_struct *p = current;
      int cpu = smp_processor_id(), system = user_tick ^ 1;
      //	if (mydebug6 && in_idle_while)
      //  printk(".");
      if (mydebug5 && !countme2--) { 
	countme2=500; printk(",");
	printk("timer %x %x %x\n",p->pid,p->phd$w_quant,p->pcb$b_pri);
      }
      //  printk(":");
      //	if (p->pid==2) { int i; for(i=0;i<1000000;i++) ; }
      // { int i; for (i=0; i<1000000; i++ ) ; }}
      update_one_process(p, user_tick, system, cpu);
      if (p->pid==0) { if (++pid0count>5) { pid0count=0; p->need_resched=1;}}  /* Will be removed in the future */
      if (p->pid==1) { if (++pid1count>5) { pid1count=0; p->need_resched=1;}}  /* Will be removed in the future */
      if (p->pid) {
	p->phd$l_cputim++;
	if (++p->phd$w_quant  >= 0 ) {
	  if (p->phd$w_quant<128) {
	    //		    SOFTINT_TIMER_VECTOR;
	    exe$swtimint();
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

    mark_bh(TIMER_BH);
    if (TQ_ACTIVE(tq_timer))
      mark_bh(TQUEUE_BH);

#ifndef CONFIG_X86_LOCAL_APIC
    if (!user_mode(regs))
      x86_do_profile(regs->eip);
#else
    if (!using_apic_timer)
      smp_local_timer_interrupt(regs);
#endif

    if ((time_status & STA_UNSYNC) == 0 &&
	xtime.tv_sec > last_rtc_update + 660 &&
	xtime.tv_usec >= 500000 - ((unsigned) tick) / 2 &&
	xtime.tv_usec <= 500000 + ((unsigned) tick) / 2) {
      if (set_rtc_mmss(xtime.tv_sec) == 0)
	last_rtc_update = xtime.tv_sec;
      else
	last_rtc_update = xtime.tv_sec - 600;
    }

    write_unlock(&xtime_lock);
  }  
}
