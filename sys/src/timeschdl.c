// $Id$
// $Locker$

// Author. Roar Thron�s.
// Modified Linux source file, 2001-2004. Based on timer.c.
/**
   \file timeschdl.c
   \brief timer system hardware interrupt
   \author Roar Thron�s
*/

#include <queue.h>
#include <asmlink.h>
#include <pridef.h>
#include <ipldef.h>
#include <lkbdef.h>
#include <phddef.h>
#include <ipl.h>
#include <linux/linkage.h>

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
#include <exe_routines.h>
#include <misc_routines.h>
#include <sch_routines.h>
#include <ddbdef.h>

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

/**
   \brief regular system job - see 5.2 11.8.2
*/

void exe$timeout(void)
{
    /** do sch$swpwake() if appropiate */
    sch$swpwake();
    /** passed one second uptime */
    exe$gl_abstim++;
    /** do erl$wake() if appropiate - MISSING */
    /** ecc$reenable() - MISSING */
    /** scan i/o db etc for timeous */
    {
        struct _ddb * d=ioc$gl_devlist;
        if (d == 0) goto skip;
        do
        {
#if 0
            //    printk("bcmp %s %s\n",d->ddb$t_name,s->dsc$a_pointer);
            struct _sb * sb=d->ddb$ps_sb;
            if ((node==0) && sb)
                goto next;
            if (node && (sb==0))
                goto next;
            if (sb && 0!=strncmp(&sb->sb$t_nodename[1],node,nodelen))
                goto next;
#endif
            struct _ucb * tmp = d->ddb$ps_ucb;
            do
            {
                //printk("unitcmp %x %x\n",unit,tmp->ucb$w_unit);
                if (tmp->ucb$l_duetim && tmp->ucb$l_duetim < exe$gl_abstim)
                {
                    if (tmp->ucb$l_fpc)
                    {
                        /** forklock - MISSING */
                        printk("ucb timeout %x %x\n", tmp, tmp->ucb$l_irp);
                        /** call device timeout routine to restart i/o if needed */
                        tmp->ucb$l_fpc(tmp->ucb$l_fr3, tmp->ucb$l_fr4);
                        /** forkunlock - MISSING */
                    }
                }
                tmp=tmp->ucb$l_link;
            }
            while (tmp && tmp!=d->ddb$ps_ucb);
next:
            d=d->ddb$ps_link;
        }
        while (d && d!=ioc$gl_devlist);
skip:
        {}
    }
    /** scan crbs for more timeouts - MISSING */
    /** if running monitor... scan data - MISSING */
    /** scan fork and wait queue. soon to be implemented - MISSING */
    /** scan lock mgr etc to handle deadlock - MISSING practically */
    if (lck$gq_timoutq)
        if (exe$gl_abstim>=((struct _lkb *)lck$gq_timoutq)->lkb$l_duetime)
            lck$searchdlck();
    /** call one_sec */
    sch$one_sec();
    /** 4 sch$ravail() - MISSING */
    if (exetimeout) printk("exe$timeout %x\n",current->pcb$l_pid);
}

extern int vmstimerconf;

void printtq(struct _tqe * t)
{
    printk("%x %lx %lx %lx\n",t,t->tqe$l_tqfl,t->tqe$q_delta,t->tqe$q_time);
}

/**
   \brief software timer interrupt service routine - see 5.2 11.8
*/

asmlinkage void exe$swtimint(void)
{
    static signed int times=0;

#ifdef __x86_64__
    if (intr_blocked(IPL$_TIMERFORK))
        return;
    regtrap(REG_INTR, IPL$_TIMERFORK);
#endif

    setipl(IPL$_TIMERFORK);

    /** linux leftover or something? */
    times++;

    /** locate process header */
    /** test for quantum end and maybe run sch$qend */
    if (current->pcb$w_quant>=0 && current->pcb$w_quant<128)
        sch$qend(current);
    /** note that the spinlocks are now inside sch$qend */

    //  printk(".");
    /* check tqe from EXE$GL_TQFL */
    /** test if primary cpu */
    if (smp_processor_id()==0)
    {
        /** acquire both TIMER and HWCLK spinlocks */
        vmslock(&SPIN_TIMER,IPL$_TIMER);
        vmslock(&SPIN_HWCLK,IPL$_HWCLK);
        // get rid of duplicated locking with goto label?

        /** iterate */
        /** compare system time with 1st time for timer entry */
        while (vmstimerconf && exe$gq_systime>=exe$gq_1st_time)
        {
            struct _tqe * t, * dummy=0;
            //    printk(".");
            //if (times>=0 && times<5)   printtq(exe$gl_tqfl);
            //if (times>=0 && times<5)   printtq(exe$gl_tqfl->tqe$l_tqfl);
            /** remove first tqe from queue */
            t=remque(exe$gl_tqfl->tqe$l_tqfl,dummy);
            /** release both TIMER and HWCLK spinlocks */
            vmsunlock(&SPIN_HWCLK,IPL$_TIMER);
            vmsunlock(&SPIN_TIMER,-1);
            //if (times>=0 && times<5)   printtq(t);
            //if (times>=0 && times<5)   printtq(exe$gl_tqfl);
            /** 11.8.1 process timer requests */
            /** if TQTYPE CHK_CPUTIM ... - MISSING */
            if ((t->tqe$b_rqtype & TQE$M_TQTYPE) == TQE$C_TMSNGL)
            {
                /** test if process still alive - MISSING */
                /** if not or if illegal efn, deallocate tqe - MISSING */
                sch$postef(t->tqe$l_pid,PRI$_TIMER,t->tqe$b_efn);
                /** increment jib tqcnt - MISSING */
                /** test jib flags to see if someone waits - MISSING */
                /** run sch$chse for each such process - MISSING */
                /** test if acb quota in tqe set, user wants ast - MISSING */
                /** copy some field to ast - MISSING */
                /** call qast */
                sch$qast(t->tqe$l_pid,PRI$_TIMER,(struct _acb *) t);
                //  void (*f)(void);
                //  f=t->tqe$l_fpc;
                //  if (f) f();
                //  printk("herexxx\n");
                //      printk(KERN_EMERG "before f %x %x %x %x\n",f,t->tqe$l_fpc,exe$timeout,&exe$timeout);
            }
            /** 11.8,2 periodic system routine requests TODO check if right */
            if ((t->tqe$b_rqtype & TQE$M_TQTYPE) == TQE$C_SSSNGL)
            {
                void (*f)(void);
                f=t->tqe$l_fpc;
                /** just call it */
                //      printk(KERN_EMERG "before f %x %x %x %x\n",f,t->tqe$l_fpc,exe$timeout,&exe$timeout);
                if (f) f();
                //      printk(KERN_EMERG "after f %x %x %x %x\n",f,t->tqe$l_fpc,exe$timeout,&exe$timeout);
            }
            /** 11.8.3 scheduled wakeup requests */
            if ((t->tqe$b_rqtype & TQE$M_TQTYPE) == TQE$C_WKSNGL)
            {
                /** get sched spinlock TODO says who? */
                vmslock(&SPIN_SCHED,-1);
                /** test if process still alive - MISSING */
                /** if so, and rqpid still exists, increment astcnt - MISSING */
                /** if not deallocate tqe - MISSING */
                /** call sch$wake to wake it up */
                sch$wake(t->tqe$l_pid);
                /** if no repeat, do cleanup - MISSING */
                /** release sched spinlock TODO says who? */
                vmsunlock(&SPIN_SCHED,-1);
                //  void (*f)(void);
                //  f=t->tqe$l_fpc;
                //  printk(KERN_EMERG "xbefore f %x %x %x %x\n",f,t->tqe$l_fpc,exe$timeout,&exe$timeout);
            }
            /** if repeat add tqe delta to tqe time, and do instimq */
            if (t->tqe$b_rqtype & TQE$M_REPEAT)
            {
                t->tqe$q_time=exe$gq_systime+t->tqe$q_delta;
                //  if (times>=0 && times<5) printk("i\n");
                //      if (times>=0 && times<5)   printtq(t);
                //if (times>=0 && times<5)   printtq(exe$gl_tqfl);
                //  printk(":");
                if (0 && times>1000 && times<1200)
                {
                    printtq(t);
                    printtq(exe$gl_tqfl);
                    printtq(exe$gl_tqfl->tqe$l_tqfl);
                    printtq(exe$gl_tqfl->tqe$l_tqfl->tqe$l_tqfl);
                }
                exe$instimq(t);
                if (0 && times>1000 && times<1200)
                {
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
                //  insque(t,exe$gl_tqfl); /* move this to a sort */
            }
            /** acquire both TIMER and HWCLK spinlocks */
            vmslock(&SPIN_TIMER,IPL$_TIMER);
            vmslock(&SPIN_HWCLK,IPL$_HWCLK);
            exe$gq_1st_time=exe$gl_tqfl->tqe$l_tqfl->tqe$q_time;
        }
        vmsunlock(&SPIN_HWCLK,IPL$_TIMER);
        vmsunlock(&SPIN_TIMER,-1);
    }
    //  if (times==4) { unsigned long long i=1;
    //  cli();
    //  for(i=1;i!=0;i++) ;}
    //printk(",");
}

/* vax has 100 Hz clock interrupts. Quantum is in those 10 ns units */
#ifdef __i386
/* 100Hz interrupts here */
#define QUANTADD 1
#endif
#ifdef __x86_64__
/* 100Hz interrupts here? */
#define QUANTADD 1
#endif

/**
   \brief interval timer interrupt service routine - see 5.2 11.7
   \details 386 and x86_64 has overlapping code
*/

#ifdef __i386__
void exe$hwclkint(int irq, void *dev_id, struct pt_regs *regs)
{
    /* reset pr$_iccs */
    /* smp sanity timer */

#if 0
// this did not go well, for some reason
    if (intr_blocked(IPL$_HWCLK))
        return;

    regtrap(REG_INTR, IPL$_HWCLK);

    setipl(IPL$_HWCLK);
#endif

    /** smp and sanity timer mechanism - MISSING */

    /** now some linux parts until gq_systime set */

    // on UP or primary SMP cpu:
    if (smp_processor_id()==0)
    {

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
        extern int timer_ack;
        extern spinlock_t i8259A_lock;
        if (timer_ack)
        {
            spin_lock(&i8259A_lock);
            outb(0x0c, 0x20);
            inb(0x20);
            spin_unlock(&i8259A_lock);
        }
#endif

        /** hwclk spinlock - MISSING */

        /** update gq_systime and abstim ticks */

        exe$gq_systime+=exe$gl_ticklength;

        (*(unsigned long *)&jiffies)++;
        exe$gl_abstim_tics=jiffies;

        /** compare system time with 1st time for timer entry */
        /** if larger, invoke TIMERFORK softint */

        if (exe$gq_systime>=exe$gq_1st_time)
            SOFTINT_TIMERFORK_VECTOR;

        /** uptime time statistics with cpu kernel base - MISSING */

        /** if to be charged, checking busywait and interrupt stack - MISSING */

#ifndef CONFIG_SMP_NOT_NOT
        // check. do this anyway.
        {
            int user_tick=user_mode(regs);
            struct task_struct *p = current;
            int cpu = smp_processor_id(), system = user_tick ^ 1;
            //  if (mydebug6 && in_idle_while)
            //  printk(".");
            if (mydebug5 && !countme2--)
            {
                countme2=500;
                printk(",");
                printk("timer %x %x %x\n",p->pcb$l_pid,p->pcb$w_quant,p->pcb$b_pri);
            }
            //  printk(":");
            //  if (p->pcb$l_pid==2) { int i; for(i=0;i<1000000;i++) ; }
            // { int i; for (i=0; i<1000000; i++ ) ; }}
            /** linux leftover, TODO, check if needed */
            update_one_process(p, user_tick, system, cpu);
            if (p->pcb$l_pid==0)
            {
                if (++pid0count>5)
                {
                    pid0count=0; /*p->need_resched=1;*/   /* Will be removed in the future */
                }
            }
            if (p->pcb$l_pid==INIT_PID)
            {
                if (++pid1count>5)
                {
                    pid1count=0; /*p->need_resched=1;*/   /* Will be removed in the future */
                }
            }
            if (p->pcb$l_pid)
            {
                /** update cputim and quantum, and do TIMERFORK softint if spent */
                p->pcb$l_phd->phd$l_cputim++;
                p->pcb$w_quant+=QUANTADD;
                /** TODO check counting */
                if (++p->pcb$w_quant  >= 0 )
                {
                    if (p->pcb$w_quant<128)
                    {
                        SOFTINT_TIMERFORK_VECTOR;
                        //          sch$resched();
                    }
                }
                /** the rest is linux leftovers */
                if (p->pcb$b_prib == 31)
                    kstat.per_cpu_nice[cpu] += user_tick;
                else
                    kstat.per_cpu_user[cpu] += user_tick;
                kstat.per_cpu_system[cpu] += system;
            }
            else if (local_bh_count(cpu) || local_irq_count(cpu) > 1)
                kstat.per_cpu_system[cpu] += system;
        }
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
                xtime.tv_sec > last_rtc_update + 660 &&
                xtime.tv_usec >= 500000 - ((unsigned) tick) / 2 &&
                xtime.tv_usec <= 500000 + ((unsigned) tick) / 2)
        {
            if (set_rtc_mmss(xtime.tv_sec) == 0)
                last_rtc_update = xtime.tv_sec;
            else
                last_rtc_update = xtime.tv_sec - 600;
        }

        write_unlock(&xtime_lock);
    }
}
#endif

/**
   \brief interval timer interrupt service routine - see 5.2 11.7
   \details 386 and x86_64 has overlapping code
*/

#ifdef __x86_64__
extern unsigned int hpet_tick;                  /* HPET clocks / interrupt */
extern int report_lost_ticks;                   /* command line option */
void timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    static unsigned long rtc_update = 0;

    /*
     * Here we are in the timer irq handler. We have irqs locally disabled (so we
     * don't need spin_lock_irqsave()) but we don't know if the timer_bh is running
     * on the other CPU, so we need a lock. We also need to lock the vsyscall
     * variables, because both do_timer() and us change them -arca+vojtech
     */

#if 0
    // note: this was tried on the 386 version
    // this did not go well, for some reason
    if (intr_blocked(IPL$_HWCLK))
        return;

    regtrap(REG_INTR, IPL$_HWCLK);

    setipl(IPL$_HWCLK);
#endif

    /** smp and sanity timer mechanism - MISSING */

    /** now some linux parts until gq_systime set */

    write_lock(&xtime_lock);
    vxtime_lock();

    {
        long tsc;
        int delay, offset = 0;

        if (hpet_address)
        {

            offset = hpet_readl(HPET_T0_CMP) - hpet_tick;
            delay = hpet_readl(HPET_COUNTER) - offset;

        }
        else
        {

            spin_lock(&i8253_lock);
            outb_p(0x00, 0x43);
            delay = inb_p(0x40);
            delay |= inb(0x40) << 8;
            spin_unlock(&i8253_lock);
            delay = LATCH - 1 - delay;
        }

        rdtscll_sync(&tsc);

        if (vxtime.mode == VXTIME_HPET)
        {

            if (offset - vxtime.last > hpet_tick)
            {
                if (report_lost_ticks)
                    printk(KERN_WARNING "time.c: Lost %d timer tick(s)! (rip %016lx)\n",
                           (offset - vxtime.last) / hpet_tick - 1, regs->rip);
                jiffies += (offset - vxtime.last) / hpet_tick - 1;
            }

            vxtime.last = offset;

        }
        else
        {

            offset = (((tsc - vxtime.last_tsc) * vxtime.tsc_quot) >> 32) - tick;

            if (offset > tick)
            {
                if (report_lost_ticks)
                    printk(KERN_WARNING "time.c: lost %ld tick(s) (rip %016lx)\n",
                           offset / tick, regs->rip);
                jiffies += offset / tick;
                offset %= tick;
            }

            vxtime.last_tsc = tsc - vxtime.quot * delay / vxtime.tsc_quot;

            if ((((tsc - vxtime.last_tsc) * vxtime.tsc_quot) >> 32) < offset)
                vxtime.last_tsc = tsc - (((long)offset << 32) / vxtime.tsc_quot) - 1;

        }
    }

    /** hwclk spinlock - MISSING */

    /** update gq_systime and abstim ticks */

    exe$gq_systime+=exe$gl_ticklength;

    exe$gl_abstim_tics=jiffies;

    /** compare system time with 1st time for timer entry */
    /** if larger, invoke TIMERFORK softint */

    if (exe$gq_systime>=exe$gq_1st_time)
        SOFTINT_TIMERFORK_VECTOR;

    /** uptime time statistics with cpu kernel base - MISSING */

    /** if to be charged, checking busywait and interrupt stack - MISSING */

    int cpu = smp_processor_id();
    struct _pcb * p = ctl$gl_pcb;
    if (p->pcb$l_pid==0)
    {
        if (++pid0count>5)
        {
            pid0count=0; /*p->need_resched=1;*/   /* Will be removed in the future */
        }
    }
    if (p->pcb$l_pid==INIT_PID)
    {
        if (++pid1count>5)
        {
            pid1count=0; /*p->need_resched=1;*/   /* Will be removed in the future */
        }
    }
    if (p->pcb$l_pid)
    {
        /** update cputim and quantum, and do TIMERFORK softint if spent */
        p->pcb$l_phd->phd$l_cputim++;
        p->pcb$w_quant+=QUANTADD;
        /** TODO check counting */
        if (++p->pcb$w_quant  >= 0 )
        {
            if (p->pcb$w_quant<128)
            {
                SOFTINT_TIMERFORK_VECTOR;
                //          sch$resched();
            }
        }
        /** the rest is linux leftovers */
    }
#if 0
    // not yet
    if (p->pcb$b_prib == 31)
        kstat.per_cpu_nice[cpu] += user_tick;
    else
        kstat.per_cpu_user[cpu] += user_tick;
    kstat.per_cpu_system[cpu] += system;
#endif

    /*
     * Do the timer stuff.
     */

    do_timer(regs);

    /*
     * If we have an externally synchronized Linux clock, then update CMOS clock
     * accordingly every ~11 minutes. set_rtc_mmss() will be called in the jiffy
     * closest to exactly 500 ms before the next second. If the update fails, we
     * don'tcare, as it'll be updated on the next turn, and the problem (time way
     * off) isn't likely to go away much sooner anyway.
     */

    if ((~time_status & STA_UNSYNC) && xtime.tv_sec > rtc_update &&
            abs(xtime.tv_usec - 500000) <= tick / 2)
    {
        set_rtc_mmss(xtime.tv_sec);
        rtc_update = xtime.tv_sec + 660;
    }

    vxtime_unlock();
    write_unlock(&xtime_lock);
}
#endif
