// $Id$ 
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004
/**
   \file sched.c
   \brief Scheduling - TODO still more doc
   \author Roar Thronæs
*/

/*
 *  linux/kernel/sched.c
 *
 *  Kernel scheduler and related syscalls
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  1996-12-23  Modified by Dave Grothe to fix bugs in semaphores and
 *              make semaphores SMP safe
 *  1998-11-19	Implemented schedule_timeout() and related stuff
 *		by Andrea Arcangeli
 *  1998-12-28  Implemented better SMP scheduling by Ingo Molnar
 */

/* sch$resched, sch$sched and more by Roar Thronæs */

/*
 * 'sched.c' is the main kernel file. It contains scheduling primitives
 * (sleep_on, wakeup, schedule etc) as well as a number of simple system
 * call functions (type getpid()), which just extract a field from
 * current-task
 */

#include <system_data_cells.h>
#include <dyndef.h>
#include <phddef.h>

#include <linux/config.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/smp_lock.h>
#include <linux/nmi.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/completion.h>
#include <linux/prefetch.h>
#include <linux/compiler.h>
#include <asm/uaccess.h>
#include <asm/mmu_context.h>
#include <sysgen.h>
#include <ipldef.h>
#include <ipl.h>
#include <statedef.h>
#include <queue.h>
#include<system_service_setup.h>
#include <internals.h>
#include <exe_routines.h>
#include <sch_routines.h>
#include <misc_routines.h>
#include <cpbdef.h>
#include <rsndef.h>
#include <statedef.h>

#undef DEBUG_SCHED

#define DEBUG_SCHED

/* try to cut out wake_up_process schedule_timeout __wake_up __wake_up_sync 
   complete wait_for_completion interruptible_sleep_on 
   interruptible_sleep_on_timeout sleep_on sleep_on_timeout 
*/ 

extern void timer_bh(void);
extern void tqueue_bh(void);
extern void immediate_bh(void);

int done_init_idle=0; 

/*
 * scheduler variables
 */

unsigned securebits = SECUREBITS_DEFAULT; /* systemwide security settings */

extern void mem_use(void);

int mydebug4 = 0;
int mydebug5 = 0;
int mydebug6 = 1;

/**
   \brief check content ok
*/

mycheckaddr(unsigned int ctl){
#if 1
  int nr0=nr_running;
  int i,n=0,m=0;
  struct _pcb  *tmp2 = 0;
  unsigned long tmp;
  int pan=0;
  for(i=0;i<32;i++) {
    tmp=&sch$aq_comh[i];
    if(*(unsigned long *)tmp == tmp) {
      if (test_bit(i,&sch$gl_comqs))
	panic("testbit %x %x\n",i,sch$gl_comqs);
    }
    if(*(unsigned long *)tmp == tmp) {; } else {
      tmp2=tmp;
      do {
	n++;
	if (tmp2!=tmp && i!=tmp2->pcb$b_pri)
	  panic("wrong pri in q %x %x %x\n",tmp2,i,tmp2->pcb$b_pri);
	if (tmp2!=tmp && tmp2->pcb$b_type!=DYN$C_PCB)
	  panic("wrong type in q %x %x %x\n",tmp2,i,tmp2->pcb$b_type);
	if (ctl<42 && ctl$gl_pcb==tmp2)
	  panic("ctl$gl_pcb on comq\n");
	if (ctl>42 && ctl==tmp2)
	  panic("next ctl$gl_pcb on comq\n");
	if (tmp2!=(tmp2->pcb$l_sqfl->pcb$l_sqbl)) { pan=1; goto mypanic; }
	if (tmp2!=(tmp2->pcb$l_sqbl->pcb$l_sqfl)) { pan=2; goto mypanic; }
	tmp2=tmp2->pcb$l_sqfl;
      } while (tmp2!=tmp);
      n--;
    }
  }
  for(i=0;i<32;i++) {
    tmp=&sch$aq_comh[i];
    if(*(unsigned long *)tmp == tmp) {; } else {
      tmp2=tmp;
      do {
	m++;
	tmp2=tmp2->pcb$l_sqbl;
	if (tmp2!=(tmp2->pcb$l_sqfl->pcb$l_sqbl)) { pan=3; goto mypanic; }
	if (tmp2!=(tmp2->pcb$l_sqbl->pcb$l_sqfl)) { pan=4; goto mypanic; }
      } while (tmp2!=tmp);
      m--;
    }
  }
  if (nr0!=nr_running) { /*printk("nr0 changed\n");*/ return; }
  if (n!=m) { pan=5|(n<<4)|(m<<8); goto mypanic; }
#if 0
  unsigned long * f=&sch$aq_comh[31];
  unsigned long * b=&sch$aq_comt[31];
  if (*f==f && *b!=f)
    { pan=6; goto mypanic; }
  if (*b==f && *f!=f)
    { pan=7; goto mypanic; }
  if (nr_running<2 && *b!=*f)
    {pan=8; goto mypanic; }
  printk("mypanic pan %x %x %x %x %x %x\n",pan,nr_running,f,b,*f,*b);
#endif
  return;
 mypanic:
  printk("mypanic pan %x %x %x %x %x %x\n",pan,i,n,m,tmp,tmp2);
  printk("mypanic %x %x %x %x %x\n",tmp2->pcb$l_sqfl,tmp2->pcb$l_sqfl->pcb$l_sqbl,tmp2->pcb$l_sqbl,tmp2->pcb$l_sqbl->pcb$l_sqfl,42);
  cli();
#ifdef __i386__
  sickinsque(0x11111111,0x22222222);
#endif
  while(1) {; };
#endif 
}

int numproc(void) {
  int i,n=0;
  struct _pcb  *tmp2;
  unsigned long tmp;
  for(i=0;i<32;i++) {
    tmp=&sch$aq_comh[i];
    if(*(unsigned long *)tmp == tmp) {; } else {
      tmp2=tmp;
      do {
	n++;
	tmp2=tmp2->pcb$l_sqfl;
	//	printk("%x %x %x & ",tmp2,tmp2->pcb$l_pid,tmp2->pcb$b_pri);
      } while (tmp2!=tmp);
      n--;
      //      printk("\n");
    }
  }
  return n;
}

void printcom(void) {
  int i;
  struct _pcb  *tmp2;
  unsigned long tmp;
  printk(KERN_EMERG "cpusch %x\n",sch$gl_comqs);
  for(i=16;i<32;i++) {
    tmp=&sch$aq_comh[i];
    if(*(unsigned long *)tmp == tmp) {; } else {
      tmp2=tmp;
      printk(KERN_EMERG "com %x ",i);
      do {
	printk(KERN_EMERG "%x %x %x %x| ",tmp2,tmp2->pcb$l_sqfl,tmp2->pcb$l_pid,tmp2->pcb$b_pri);
	tmp2=tmp2->pcb$l_sqfl;
      } while (tmp2!=tmp);
      printk(KERN_EMERG "\n");
    }
  }
  printk(KERN_EMERG "\n");
}

void printcom2(void) {
  int i;
  struct _pcb  *tmp2;
  unsigned long tmp;
  printk(KERN_EMERG "cpusch %x\n",sch$gl_comqs);
  for(i=16;i<32;i++) {
#ifdef __i386__
    tmp=&sch$aq_comt[i];
#else
    tmp=&sch$aq_comh[i][1];
#endif
    if(*(unsigned long *)tmp == tmp) {; } else {
      tmp2=tmp;
      printk(KERN_EMERG "com %x ",i);
      do {
	printk(KERN_EMERG "%x %x %x %x| ",tmp2,tmp2->pcb$l_sqbl,tmp2->pcb$l_pid,tmp2->pcb$b_pri);
	tmp2=tmp2->pcb$l_sqbl;
      } while (tmp2!=tmp);
      printk(KERN_EMERG "\n");
    }
  }
  printk(KERN_EMERG "\n");
}

/*
 * Scheduling quanta.
 *
 * NOTE! The unix "nice" value influences how long a process
 * gets. The nice value ranges from -20 to +19, where a -20
 * is a "high-priority" task, and a "+10" is a low-priority
 * task.
 *
 * We want the time-slice to be around 50ms or so, so this
 * calculation depends on the value of HZ.
 */
#if HZ < 200
#define TICK_SCALE(x)	((x) >> 2)
#elif HZ < 400
#define TICK_SCALE(x)	((x) >> 1)
#elif HZ < 800
#define TICK_SCALE(x)	(x)
#elif HZ < 1600
#define TICK_SCALE(x)	((x) << 1)
#else
#define TICK_SCALE(x)	((x) << 2)
#endif

#define NICE_TO_TICKS(nice)	(TICK_SCALE(20-(nice))+1)


/*
 *	Init task must be ok at boot for the ix86 as we will check its signals
 *	via the SMP irq return path.
 */
 
struct task_struct * init_tasks[NR_CPUS] = {&init_task, };

/*
 * The tasklist_lock protects the linked list of processes.
 *
 * The runqueue_lock locks the parts that actually access
 * and change the run-queues, and have to be interrupt-safe.
 *
 * If both locks are to be concurrently held, the runqueue_lock
 * nests inside the tasklist_lock.
 *
 * task->alloc_lock nests inside tasklist_lock.
 */
spinlock_t runqueue_lock __cacheline_aligned = SPIN_LOCK_UNLOCKED;  /* inner */
rwlock_t tasklist_lock __cacheline_aligned = RW_LOCK_UNLOCKED;	/* outer */

struct kernel_stat kstat;
extern struct task_struct *child_reaper;

#ifdef CONFIG_SMP

#define idle_task(cpu) (init_tasks[cpu_number_map(cpu)])
#define can_schedule(p,cpu) \
	((p)->cpus_runnable & (p)->cpus_allowed & (1 << cpu))

#else

#define idle_task(cpu) (&init_task)
#define can_schedule(p,cpu) (1)

#endif

void scheduling_functions_start_here(void) { }

/*
 * This is ugly, but reschedule_idle() is very timing-critical.
 * We are called with the runqueue spinlock held and we must
 * not claim the tasklist_lock.
 */
static FASTCALL(void reschedule_idle(struct task_struct * p));

static void fastcall reschedule_idle(struct task_struct * p)
{
#ifdef CONFIG_SMP
#if 0
  // not yet?
	int this_cpu = smp_processor_id();
	struct task_struct *tsk, *target_tsk;
	int cpu, best_cpu, i, max_prio;
	cycles_t oldest_idle;

	/*
	 * shortcut if the woken up task's last CPU is
	 * idle now.
	 */
	best_cpu = p->pcb$l_cpu_id;
	if (can_schedule(p, best_cpu)) {
		tsk = idle_task(best_cpu);
		if (cpu_curr(best_cpu) == tsk) {
			int need_resched;
send_now_idle:
			/*
			 * If need_resched == -1 then we can skip sending
			 * the IPI altogether, tsk->need_resched is
			 * actively watched by the idle thread.
			 */
			need_resched = tsk->need_resched;
			tsk->need_resched = 1;
			if ((best_cpu != this_cpu) && !need_resched)
				smp_send_reschedule(best_cpu);
			return;
		}
	}

	/*
	 * We know that the preferred CPU has a cache-affine current
	 * process, lets try to find a new idle CPU for the woken-up
	 * process. Select the least recently active idle CPU. (that
	 * one will have the least active cache context.) Also find
	 * the executing process which has the least priority.
	 */
	oldest_idle = (cycles_t) -1;
	target_tsk = NULL;
	max_prio = 0;

	for (i = 0; i < smp_num_cpus; i++) {
		cpu = cpu_logical_map(i);
		if (!can_schedule(p, cpu))
			continue;
		tsk = cpu_curr(cpu);
		/*
		 * We use the first available idle CPU. This creates
		 * a priority list between idle CPUs, but this is not
		 * a problem.
		 */
		if (tsk == idle_task(cpu)) {
			if (last_schedule(cpu) < oldest_idle) {
				oldest_idle = last_schedule(cpu);
				target_tsk = tsk;
			}
		} else {
			if (oldest_idle == -1ULL) {
				int prio = preemption_goodness(tsk, p, cpu);

				if (prio > max_prio) {
					max_prio = prio;
					target_tsk = tsk;
				}
			}
		}
	}
	tsk = target_tsk;
	if (tsk) {
		if (oldest_idle != -1ULL) {
			best_cpu = tsk->pcb$l_cpu_id;
			goto send_now_idle;
		}
		tsk->need_resched = 1;
		if (tsk->pcb$l_cpu_id != this_cpu)
			smp_send_reschedule(tsk->pcb$l_cpu_id);
	}
	return;
		
#endif
#else /* UP */
	int this_cpu = smp_processor_id();
	struct task_struct *tsk;

	tsk = ctl$gl_pcb;
	if (p->pcb$b_pri >= tsk->pcb$b_pri) /* previous was meaningless */
		tsk->need_resched = 1;
#endif
}

/**
   \brief is the task on comqueue?
*/

int task_on_comqueue(struct _pcb *p) {
  int i,found=0;
  struct _pcb  *tmp2;
  struct _pcb * tmp;
  for(i=0;i<32;i++) {
    tmp=&sch$aq_comh[i];
    if(*(unsigned long *)tmp == tmp) {; } else {
      tmp=tmp->pcb$l_sqfl;
      tmp2=tmp;
      do {
	//	if (tmp2 == p) found=1;
	// wrong/why?	if (tmp2 == p && tmp2->pcb$l_sqfl!=tmp) found=1;
	if (tmp2 == p) found=1;
	tmp2=tmp2->pcb$l_sqfl;
      } while (tmp2!=tmp);
    }
  }
  return found;
}

/*
 * Wake up a process. Put it on the run-queue if it's not
 * already there.  The "current" process is always on the
 * run-queue (except when the actual re-schedule is in
 * progress), and as such you're allowed to do the simpler
 * "current->state = TASK_RUNNING" to mark yourself runnable
 * without the overhead of this.
 */
static inline int try_to_wake_up(struct task_struct * p, int synchronous)
{
  unsigned long flags;
  int success = 0;
  unsigned long qhead;
  int curpri;
  int before,after;
  int cpuid = smp_processor_id();
  struct _cpu * cpu=smp$gl_cpu_data[cpuid];

  spin_lock_irqsave(&runqueue_lock, flags);
  p->state = TASK_RUNNING;
  p->pcb$w_state = SCH$C_COM;
  if (p==ctl$gl_pcb)
    goto out;
  if (task_on_comqueue(p)) /*  argh! */
    goto out;

  //  p->pcb$b_pri=p->pcb$b_prib-3;  /* boost */ /* not here */
  curpri=p->pcb$b_pri;
  if (mydebug4) printk("add tyr %x %x\n",p->pcb$l_pid,curpri);
#ifdef __i386__
  qhead=*(unsigned long *)&sch$aq_comt[curpri];
#else
  qhead=*(unsigned long *)&sch$aq_comh[curpri][1];
#endif
  if (mydebug4) printk("eq qhead %x %x %x %x\n",
		       (unsigned long *)qhead,(unsigned long *)(qhead+4),
		       *(unsigned long *)qhead,*(unsigned long *)(qhead+4));
  if (mydebug4) printk("p %x %x %x\n",qhead,*(void**)qhead,p);
  if (mydebug4) printk("iq3 %x\n",sch$aq_comh[curpri]);

  before=numproc();
  //    printcom();
  //    printcom2();
  mycheckaddr(0);
  insque(p,qhead);
  nr_running++;
  after=numproc();
  if(after-before!=1) {
    printcom();
    printcom2();
    panic("insq1 %x %x %x %x\n",p,p->pcb$l_pid,before,after);
  }

  if (mydebug4) printk("p %x %x %x\n",qhead,*(void**)qhead,p);
  if (mydebug4) printk("iq3 %x\n",sch$aq_comh[curpri]);
  if (mydebug4) printk("eq qhead %x %x %x %x\n",
		       (unsigned long *)qhead,(unsigned long *)(qhead+4),
		       *(unsigned long *)qhead,*(unsigned long *)(qhead+4));

  if (mydebug4) printk("comq1 %x %x\n",curpri,sch$gl_comqs);

  sch$gl_comqs=sch$gl_comqs | (1 << curpri);

  mycheckaddr(0);

  if (mydebug4) printk("comq1 %x %x\n",curpri,sch$gl_comqs);
  if (mydebug4) {
    int i;
    struct _pcb  *tmp2,*tmp3=qhead;
    unsigned long tmp;
    printk("p %x\n",p);
    printk("%x %x %x %x\n",tmp3,tmp3->pcb$l_sqfl,tmp3->pcb$l_sqfl->pcb$l_sqfl,tmp3->pcb$l_sqfl->pcb$l_sqfl->pcb$l_sqfl);
    for(i=0;i<32;i++) {
      tmp=&sch$aq_comh[i];
      //      printk("i %x %x i",i,tmp);
      if(*(unsigned long *)tmp == tmp) {; } else {
	tmp2=((struct _pcb *)tmp)->pcb$l_sqfl->pcb$l_sqfl;
	do {
	  printk("com2 %x %x %x %x\n",tmp2,tmp2->pcb$l_pid,tmp2->pcb$b_pri,i);
	  tmp2=tmp2->pcb$l_sqfl;
	} while (tmp2!=tmp);
      }
    }
  }
  if (!synchronous || !(p->cpus_allowed & (1 << smp_processor_id())))
    reschedule_idle(p);
  success = 1;

 out:
  spin_unlock_irqrestore(&runqueue_lock, flags);
  return success;
}

inline int fastcall wake_up_process(struct task_struct * p)
{
	return try_to_wake_up(p, 0);
}

static void process_timeout(unsigned long __data)
{
	struct task_struct * p = (struct task_struct *) __data;

	wake_up_process(p);
}

/**
 * schedule_timeout - sleep until timeout
 * @timeout: timeout value in jiffies
 *
 * Make the current task sleep until @timeout jiffies have
 * elapsed. The routine will return immediately unless
 * the current task state has been set (see set_current_state()).
 *
 * You can set the task state as follows -
 *
 * %TASK_UNINTERRUPTIBLE - at least @timeout jiffies are guaranteed to
 * pass before the routine returns. The routine will return 0
 *
 * %TASK_INTERRUPTIBLE - the routine may return early if a signal is
 * delivered to the current task. In this case the remaining time
 * in jiffies will be returned, or 0 if the timer expired in time
 *
 * The current task state is guaranteed to be TASK_RUNNING when this 
 * routine returns.
 *
 * Specifying a @timeout value of %MAX_SCHEDULE_TIMEOUT will schedule
 * the CPU away without a bound on the timeout. In this case the return
 * value will be %MAX_SCHEDULE_TIMEOUT.
 *
 * In all cases the return value is guaranteed to be non-negative.
 */
signed long fastcall schedule_timeout(signed long timeout)
{
	struct timer_list timer;
	unsigned long expire;

	switch (timeout)
	{
	case MAX_SCHEDULE_TIMEOUT:
		/*
		 * These two special cases are useful to be comfortable
		 * in the caller. Nothing more. We could take
		 * MAX_SCHEDULE_TIMEOUT from one of the negative value
		 * but I' d like to return a valid offset (>=0) to allow
		 * the caller to do everything it want with the retval.
		 */
		schedule();
		goto out;
	default:
		/*
		 * Another bit of PARANOID. Note that the retval will be
		 * 0 since no piece of kernel is supposed to do a check
		 * for a negative retval of schedule_timeout() (since it
		 * should never happens anyway). You just have the printk()
		 * that will tell you if something is gone wrong and where.
		 */
		if (timeout < 0)
		{
			printk(KERN_ERR "schedule_timeout: wrong timeout "
			       "value %lx from %p\n", timeout,
			       __builtin_return_address(0));
			current->state = TASK_RUNNING;
			current->pcb$w_state = SCH$C_CUR;
			goto out;
		}
	}

	expire = timeout + jiffies;

	init_timer(&timer);
	timer.expires = expire;
	timer.data = (unsigned long) current;
	timer.function = process_timeout;

	add_timer(&timer);
	schedule();
	del_timer_sync(&timer);

	timeout = expire - jiffies;

 out:
	return timeout < 0 ? 0 : timeout;
}

/* should be straight from the internals */
/* this is not in use, should have some more things like sch$ and cpu$ globals
   implemented first */
asmlinkage void sch$sched(int);

extern int fix_init_thread;

int countme=500;

/**
   \brief rescheduling - see 5.2 12.6.4
*/

asmlinkage void sch$resched(void) {
  int cpuid = smp_processor_id();
  struct _cpu * cpu=smp$gl_cpu_data[cpuid]; 
  struct _pcb * curpcb;
  unsigned long curpri;
  unsigned long qhead;
  int before,after;

  // lock sched db, soon
  //if (spl(IPL$_SCHED)) return;
  //    old=spl(IPL$_SCHED);
  // svpctx, do not think we need to do this here

#ifdef __x86_64__
  if (intr_blocked(IPL$_RESCHED))
    return;
  regtrap(REG_INTR,IPL$_RESCHED);
#endif

  /** spinlock sched and set ipl */
  setipl(IPL$_SCHED);
  vmslock(&SPIN_SCHED,-1);

  spin_lock_irq(&runqueue_lock); /* eventually change to sched? */

  /** get current pcb and priority */

  curpcb=cpu->cpu$l_curpcb;

  release_kernel_lock(curpcb, cpuid);

  curpri=cpu->cpu$b_cur_pri;

  /** clear bit in cpu_priority table */

  sch$al_cpu_priority[curpri]=sch$al_cpu_priority[curpri] & (~ cpu->cpu$l_cpuid_mask );

  /** if no process with this pri on any cpu, clear bit in active_priority table */

  if (!sch$al_cpu_priority[curpri])
    sch$gl_active_priority=sch$gl_active_priority & (~ (1 << (31-curpri)));

  /** now some if's remaining from linux - TODO: check if still needed */

  if (curpcb == idle_task(curpcb->pcb$l_cpu_id))
    goto out;

  if (curpcb->state==TASK_INTERRUPTIBLE)
    if (signal_pending(curpcb)) {
      curpcb->state = TASK_RUNNING;
      curpcb->pcb$w_state = SCH$C_CUR;
    }

#if 0
  if (curpcb->state!=TASK_RUNNING) {
    curpcb->pcb$w_state=SCH$C_LEF; // use here temporarily
  }
#endif

#if 0
  if (curpcb->state==TASK_RUNNING) {
#endif
#ifdef DEBUG_SCHED
    before=numproc();
    //    printcom();
    //if (curpcb==0xa018c000 && qhead==0xa018c000)
    //  panic("aieeeeeh\n");
    mycheckaddr(0);
    //if (curpcb==qhead) panic(" a panic\n");
#endif

    /** set pri bit in comqs */

    sch$gl_comqs=sch$gl_comqs | (1 << curpri);
    //    curpcb->state=TASK_INTERRUPTIBLE; /* soon SCH$C_COM ? */

    /** set state of cur pcb to COM */

    curpcb->pcb$w_state=SCH$C_COM;

    /** insert pcb at tail of comqueue */

#ifdef __i386__    
    qhead=*(unsigned long *)&sch$aq_comt[curpri];
#else
    qhead=*(unsigned long *)&sch$aq_comh[curpri][1];
#endif
    if (!task_on_comqueue(curpcb)) {
      if (curpcb==qhead) panic(" a panic\n");
      insque(curpcb,qhead);
    } else {
      panic("something\n");
    }
#ifdef DEBUG_SCHED
    mycheckaddr(42);
#endif
    /** linux leftover */
    nr_running++;
#ifdef DEBUG_SCHED
    after=numproc();
    if(after-before!=1) {
      //printk("entry qhead %x %x\n",curpcb,qhead);
      printcom();
      panic("insq2 %x %x\n",before,after);
    }
#endif

  out:
    /** clear idle_cpus to signal all idle cpus to try to reschedule */
    sch$gl_idle_cpus=0;
#if 0
  }
#endif
  /** go some intro sch$sched */
  sch$sched(1);
}

/*
 *  'schedule()' is the scheduler function. It's a very simple and nice
 * scheduler: it's not perfect, but certainly works for most things.
 *
 * The goto is "interesting".
 *
 *   NOTE!!  Task 0 is the 'idle' task, which gets called when no other
 * tasks can run. It can not be killed, and it cannot sleep. The 'state'
 * information in task[0] is never used.
 */

//int mdthreadpid=0;

int mydebug=0;
int mydebug2=0;
int mycount=0;

/* should be straight from the internals
   aboutish implemented */

asmlinkage void schedule(void) {
  SOFTINT_RESCHED_VECTOR;
  //__asm__ __volatile__ ("int $0x93\n");
  //  sch$resched();
}

asmlinkage void sch$sched(int from_sch$resched) {
  int cpuid = smp_processor_id();
  struct _cpu * cpu=smp$gl_cpu_data[cpuid]; 
  struct _pcb *next = 0, *curpcb;
  int curpri, affinity;
  unsigned char tmppri;
  unsigned long qhead = 0;
  int after, before;

  curpcb=cpu->cpu$l_curpcb;
  curpri=cpu->cpu$b_cur_pri;

  //  if (!countme--) { countme=500; printk("."); }

  if (from_sch$resched == 1)
    goto skip_lock;

#if 0

  // NOT YET??? nope,not an interrupt. pushpsl+setipl/vmslock instead?

  if (intr_blocked(IPL$_SCHED))
    return;

  regtrap(REG_INTR,IPL$_SCHED);
#endif

  int ipl = getipl();
  if (ipl != 8 || SPIN_SCHED.spl$l_spinlock == 0)
    panic("schsch\n");

#if 0
  // temp workaround
  // must avoid nesting, since I do not know how to get out of it
  setipl(IPL$_SCHED);
  vmslock(&SPIN_SCHED,-1);
#endif

  /** clear cpu_priority for current pri bit - TODO: where did this come from? */

  sch$al_cpu_priority[curpri]=sch$al_cpu_priority[curpri] & (~ cpu->cpu$l_cpuid_mask );

  /** skip if ... TODO: from where? */

  if (sch$al_cpu_priority[curpri]) 
    goto skip_lock;

  /** clear active_priority for current pri bit - TODO: where did this come from? */

  sch$gl_active_priority=sch$gl_active_priority & (~ (1 << (31-curpri)));

  //if (spl(IPL$_SCHED)) return;
  //  old=spl(IPL$_SCHED);

  /** now 4 linux leftovers */

  spin_lock_prefetch(&runqueue_lock);

  if (!curpcb->active_mm) BUG();

  release_kernel_lock(curpcb, cpuid);

  spin_lock_irq(&runqueue_lock);

 skip_lock:

  /** reset cpu affinity TODO: from where? */

  affinity=0;
  struct _pcb * aff_next = 0;

  /** find highest pri comqueue */

  tmppri=ffs(sch$gl_comqs);
#ifdef DEBUG_SCHED
  if (mydebug5)
    printk("ffs %x %x\n",tmppri,sch$gl_comqs);
#endif

  if (!tmppri) {
    /** if none found, idle */
#if 0
    // spot for more vms sched
    goto sch$idle;
#endif
  go_idle:
    /** set bit in idle_cpus */
    sch$gl_idle_cpus=sch$gl_idle_cpus | (cpu->cpu$l_cpuid_mask);
    /** store null pcb and -1 pri: MISSING check why */
    /** necessary idle_task line from linux */
    next=idle_task(cpuid);
    goto skip_cap;
  } else {
    /** if pcb in queue */
    tmppri--;
  gethead:
    qhead=*(unsigned long *)&sch$aq_comh[tmppri];
#ifdef DEBUG_SCHED
    if (mydebug4) printk("eq qhead %x %x %x %x\n",
			 (unsigned long *)qhead,(unsigned long *)(qhead+4),
			 *(unsigned long *)qhead,*(unsigned long *)(qhead+4));

    if (mydebug) printcom();

    if (mydebug4) printk("next %x %x %x %x\n",qhead,*(void**)qhead,next,sch$aq_comh[tmppri]);
    before=numproc();
#endif
#ifdef DEBUG_SCHED
    mycheckaddr(42);
#endif

    /** get new pcb from selected comqueue */
    next=(struct _pcb *) remque(qhead,next);

    /** zero next pcb queue header TODO: check if still needed */
    qhead_init(next); // temp measure since we don't insert into wqs

    /** linux pcb count leftover */
    nr_running--;
#ifdef DEBUG_SCHED
    after=numproc();
    if(before-after!=1) {
      int i;
      for (i=0;i<2000000000;i++) ;
      panic("remq1 %x %x\n",before,after);
    }
    if (mydebug4) printk("next %x %x %x %x\n",qhead,*(void**)qhead,next,sch$aq_comh[tmppri]);
    if (mydebug4) printk("comh %x %x\n",sch$aq_comh[tmppri],((struct _pcb *) sch$aq_comh[tmppri])->pcb$l_sqfl);
#endif
    //if (sch$aq_comh[tmppri]==((struct _pcb *) sch$aq_comh[tmppri])->pcb$l_sqfl)

    /** if last in queue clear comqs bit */
    if (sch$aq_comh[tmppri]==&sch$aq_comh[tmppri])
      sch$gl_comqs=sch$gl_comqs & (~(1 << tmppri));
#ifdef DEBUG_SCHED
    mycheckaddr(next);
#endif
    //    if(*(unsigned long *)qhead == qhead)
    //  if(*(unsigned long *)qhead == 0)
    if (mydebug5) printk("comq3 %x %x %x\n",tmppri,sch$gl_comqs,(~(1 << tmppri)));
    //	  printk("sch %x\n",sch$gl_comqs);
  }

  if(next==0) 	  { int j; printk("qel0\n"); for(j=0;j<1000000000;j++) ; } 

  /* bvs qempty not needed, it might panic on gen prot? */

  if (mydebug4) printk("eq qhead %x %x %x %x\n",
		       (unsigned long *)qhead,(unsigned long *)(qhead+4),
		       *(unsigned long *)qhead,*(unsigned long *)(qhead+4));
  /* No DYN check yet */
 check_pcb_ok:
#if 0
  if (next->pcb$b_type != DYN$C_PCB)
    panic("DYN$C_PCB\n");
#endif
  /** test process capabilities */
  if ((next->pcb$l_capability & cpu->cpu$l_capability) != next->pcb$l_capability) {
    if (next->pcb$l_capability & CPB$M_IMPLICIT_AFFINITY) {
      if (next->pcb$l_affinity == cpu->cpu$l_phy_cpuid)
	goto skip_cap;
      next->pcb$b_affinity_skip--;
      if (next->pcb$b_affinity_skip == 0) {
	next->pcb$b_affinity_skip = sch$gl_affinity_skip;
	next->pcb$l_affinity = cpu->cpu$l_phy_cpuid;
	// because, according to internals (5.2/1.5), implicit does nothing
	goto skip_cap;
      }
      if (!affinity) {
	affinity = 1;
	aff_next = next;
      } 
      insque(next, ((struct _pcb *)qhead)->pcb$l_sqbl); // check
      sch$gl_comqs |= 1 << tmppri;
      if (next != ((struct _pcb *)qhead)->pcb$l_sqfl) { // queue rounded
	next=(struct _pcb *) remque(qhead,next); // check
	printk("go check_pcb_ok\n");
	goto check_pcb_ok;
      }
    nextqueue:
      // need to have find_next_set_bit
      for(tmppri++; tmppri<32; tmppri++) {
	if (sch$aq_comh[tmppri]!=&sch$aq_comh[tmppri])
	  break;
      }
      if (tmppri == 32) {
	if (affinity == 0)
	  goto go_idle;
      } else {
	if (affinity == 0)
	  goto gethead;
      }
      // replace with sch$aq_preempt_mask and sysgen priority_offset
      if (tmppri<=aff_next->pcb$b_pri) { // check. with =?
	printk("gethead\n");
	goto gethead;
      }
      next = aff_next->pcb$l_sqfl;
      remque(aff_next, 0);
      if (aqempty(aff_next))
	sch$gl_comqs=sch$gl_comqs & (~(1 << next->pcb$b_pri)); // check original programming error?
      next->pcb$b_affinity_skip = sch$gl_affinity_skip;
      next->pcb$l_affinity = cpu->cpu$l_phy_cpuid;
      // because, according to internals (5.2/1.5), implicit does nothing
    } else {
      if (sch$gl_capability_sequence != next->pcb$l_capability_seq)
	next->pcb$l_current_affinity = sch$calculate_affinity(next->pcb$l_affinity);
      if ((next->pcb$l_current_affinity & (1 << next->pcb$l_cpu_id)) == 0) {
	panic("rwait for capability\n");
	sch$gl_resmask |= RSN$_CPUCAP;
	next->pcb$l_efwm = RSN$_CPUCAP;
	next->pcb$w_state = SCH$C_MWAIT;
	// set wqcnt
	struct _pcb * pcb = next->pcb$l_sqfl;
	insque(next,sch$gq_mwait);
	((struct _wqh *)sch$gq_mwait)->wqh$l_wqcnt++;
	if (aqempty(pcb))
	  goto nextqueue;
	next = pcb;
	remque(next, 0);
	goto check_pcb_ok;
      }
    }
  }
 skip_cap:

  /** if the capabilities match, store new pcb in per-cpu db */
  cpu->cpu$l_curpcb=next;
  cpu->cpu$b_ipl=next->psl_ipl;

  /** change new pcb to CUR state */
  next->state=TASK_RUNNING;
  next->pcb$w_state = SCH$C_CUR;

  /** store cpu id in pcb */
  next->pcb$l_cpu_id=cpu->cpu$l_phy_cpuid;

  /** modify new pcb pri */
  if (next->pcb$b_pri<next->pcb$b_prib) next->pcb$b_pri++;

  /** copy new pri in cpu pri db */
  cpu->cpu$b_cur_pri=next->pcb$b_pri;

  /** clear idle_cpus mask TODO: check setting cpuid_mask */
  sch$gl_idle_cpus=sch$gl_idle_cpus & (~ cpu->cpu$l_cpuid_mask);

  /** set bit it cpu_priority table to cpuid_mask */
  sch$al_cpu_priority[cpu->cpu$b_cur_pri]=sch$al_cpu_priority[cpu->cpu$b_cur_pri] | (cpu->cpu$l_cpuid_mask);
  
  /** set bit in active_priority */
  sch$gl_active_priority=sch$gl_active_priority | (1 << (31-cpu->cpu$b_cur_pri));

#ifdef DEBUG_SCHED
  if (mydebug5) { 
    printk("pri %x %x %x %x %x %x\n",curpcb,curpcb->pcb$l_pid,curpcb->pcb$b_pri,next,next->pcb$l_pid,next->pcb$b_pri);
    printk("cpusch %x %x\n",cpu->cpu$b_cur_pri,sch$gl_comqs);
    printcom();
  }
#endif

#if 0
  curpcb->need_resched = 0;
#endif

#ifdef DEBUG_SCHED
  if (mydebug4) { int i; for(i=0;i<100000000;i++) ; }
#endif
  /** this if is a linux leftover */
  if (next == curpcb) { /* does not belong in vms, but must be here */
    // handles idle_task
    spin_unlock_irq(&runqueue_lock);
    vmsunlock(&SPIN_SCHED,-1);
    reacquire_kernel_lock(curpcb);
    //splret();
    goto return_a_reimac;
    return;
  } 

  /** sets pcb cpu_id again TODO: not needed? */
  next->pcb$l_cpu_id = curpcb->pcb$l_cpu_id;

  /** spinunlock */
  spin_unlock_irq(&runqueue_lock);
  vmsunlock(&SPIN_SCHED,-1);

  //      if (mydebug5) { int j; for(j=0;j<1000000000;j++) ; }

  if (mydebug6)
    if (next>=&sch$aq_comh[0] && next<=&sch$aq_comh[33]) {
      panic("ga!\n");
      printk("ga!\n");
      { int j; for(j=0;j<1000000000;j++) ; }
    }

  /** linux count line */
  kstat.context_swtch++;
  /*
   * there are 3 processes which are affected by a context switch:
   *
   * curpcb == .... ==> (last => next)
   *
   * It's the 'much more previous' 'curpcb' that is on next's stack,
   * but curpcb is set to (the just run) 'last' process by switch_to().
   * This might sound slightly confusing but makes tons of sense.
   */
  /** from here, prepare_to_switch() and switch_to() are from linux,
      due to missing LDPCTX and SVPCTX etc */
  prepare_to_switch();
  {
    struct mm_struct *mm = next->mm;
    struct mm_struct *oldmm = curpcb->active_mm;
    if (!mm) {
      if (next->active_mm) { printk("bu %x %x %x\n",next,next->pcb$l_pid,next->pcb$b_pri); { int j; for(j=0;j<1000000000;j++) ; }; BUG(); }
      next->active_mm = oldmm;
      atomic_inc(&oldmm->mm_count);
      enter_lazy_tlb(oldmm, next, cpuid);
    } else {
      if (next->active_mm != mm) BUG();
      switch_mm(oldmm, mm, next, cpuid);
    }

    if (!curpcb->mm) {
      curpcb->active_mm = NULL;
      mmdrop(oldmm);
    }
  }

  /*
   * This just switches the register state and the
   * stack.
   */

#ifdef DEBUG_SCHED
  if (mydebug4) printk("bef swto\n");
  //  if (mydebug6) printk("bef swto %x %x\n",curpcb,next);
#endif

  /** need to set astlvl */
  next->pr_astlvl=next->phd$b_astlvl;
  if (from_sch$resched==0) {
    // myrei();
  }
#ifdef __i386__
  switch_to(curpcb, next, curpcb);
#else
  long tmp1 = 0; // read_pda(level4_pgt);
  long tmp2 = __pa(next->mm->pgd);
  switch_to(curpcb, next, curpcb, tmp1, tmp2);
#endif

  /* does not get here */

  //splret();
  goto return_a_reimac;
  return;
 qempty:
  panic("qempty");
  return;
 sch$idle:
  //	printk("sch$idle\n");
  sch$gl_idle_cpus=sch$gl_idle_cpus | (cpu->cpu$l_cpuid_mask);
  // cpu->cpu$l_curpcb = sch$ar_nullpcb;
  cpu->cpu$l_curpcb = idle_task(cpuid);
  cpu->cpu$b_cur_pri=-1;
  vmsunlock(&SPIN_SCHED,IPL$_RESCHED);
  for (; sch$gl_idle_cpus & (1<<cpuid); ) nop();
    //safe_halt();
  vmslock(&SPIN_SCHED,IPL$_SCHED);
#if 0
  cpu->cpu$q_sched_flags |= CPU$M_SCHED;
#endif
  goto skip_lock;
  return;
 return_a_reimac:
  if (from_sch$resched==0) {
    // myrei();
  }
}

/*
 * The core wakeup function.  Non-exclusive wakeups (nr_exclusive == 0) just wake everything
 * up.  If it's an exclusive wakeup (nr_exclusive == small +ve number) then we wake all the
 * non-exclusive tasks and one exclusive task.
 *
 * There are circumstances in which we can try to wake a task which has already
 * started to run but is not in state TASK_RUNNING.  try_to_wake_up() returns zero
 * in this (rare) case, and we handle it by contonuing to scan the queue.
 */
static inline void __wake_up_common (wait_queue_head_t *q, unsigned int mode,
			 	     int nr_exclusive, const int sync)
{
	struct list_head *tmp;
	struct task_struct *p;

	CHECK_MAGIC_WQHEAD(q);
	WQ_CHECK_LIST_HEAD(&q->task_list);

#ifdef __i386__	
	list_for_each(tmp,&q->task_list) {
		unsigned int state;
                wait_queue_t *curr = list_entry(tmp, wait_queue_t, task_list);

		CHECK_MAGIC(curr->__magic);
		p = curr->task;
		state = p->state;
		if (state & mode) {
			WQ_NOTE_WAKER(curr);
			if (try_to_wake_up(p, sync) && (curr->flags&WQ_FLAG_EXCLUSIVE) && !--nr_exclusive)
				break;
		}
	}
#endif
}

void fastcall __wake_up(wait_queue_head_t *q, unsigned int mode, int nr)
{
}

void fastcall __wake_up_sync(wait_queue_head_t *q, unsigned int mode, int nr)
{
}

void fastcall complete(struct completion *x)
{
	unsigned long flags;

	spin_lock_irqsave(&x->wait.lock, flags);
	x->done++;
	__wake_up_common(&x->wait, TASK_UNINTERRUPTIBLE | TASK_INTERRUPTIBLE, 1, 0);
	spin_unlock_irqrestore(&x->wait.lock, flags);
}

void fastcall wait_for_completion(struct completion *x)
{
}

#define	SLEEP_ON_VAR				\
	unsigned long flags;			\
	wait_queue_t wait;			\
	init_waitqueue_entry(&wait, current);

#define	SLEEP_ON_HEAD					\
	wq_write_lock_irqsave(&q->lock,flags);		\
	__add_wait_queue(q, &wait);			\
	wq_write_unlock(&q->lock);

#define	SLEEP_ON_TAIL						\
	wq_write_lock_irq(&q->lock);				\
	__remove_wait_queue(q, &wait);				\
	wq_write_unlock_irqrestore(&q->lock,flags);

void fastcall interruptible_sleep_on(wait_queue_head_t *q)
{
	SLEEP_ON_VAR

	current->state = TASK_INTERRUPTIBLE;
	current->pcb$w_state = 0;

	SLEEP_ON_HEAD
	schedule();
	SLEEP_ON_TAIL
}

long fastcall interruptible_sleep_on_timeout(wait_queue_head_t *q, long timeout)
{
	SLEEP_ON_VAR

	current->state = TASK_INTERRUPTIBLE;
	current->pcb$w_state = 0;

	SLEEP_ON_HEAD
	timeout = schedule_timeout(timeout);
	SLEEP_ON_TAIL

	return timeout;
}

void fastcall sleep_on(wait_queue_head_t *q)
{
	SLEEP_ON_VAR
	
	current->state = TASK_UNINTERRUPTIBLE;
	current->pcb$w_state = 0;

	SLEEP_ON_HEAD
	schedule();
	SLEEP_ON_TAIL
}

long fastcall sleep_on_timeout(wait_queue_head_t *q, long timeout)
{
	SLEEP_ON_VAR
	
	current->state = TASK_UNINTERRUPTIBLE;
	current->pcb$w_state = 0;

	SLEEP_ON_HEAD
	timeout = schedule_timeout(timeout);
	SLEEP_ON_TAIL

	return timeout;
}

void scheduling_functions_end_here(void) { }

#ifndef __alpha__

/*
 * This has been replaced by sys_setpriority.  Maybe it should be
 * moved into the arch dependent tree for those ports that require
 * it for backward compatibility?
 */

asmlinkage long sys_nice(int increment)
{
	long newprio;

	/*
	 *	Setpriority might change our priority at the same moment.
	 *	We don't have to worry. Conceptually one call occurs first
	 *	and we have a single winner.
	 */
	if (increment < 0) {
		if (!capable(CAP_SYS_NICE))
			return -EPERM;
		if (increment < -40)
			increment = -40;
	}
	if (increment > 40)
		increment = 40;

	newprio = current->pcb$b_prib + increment;
	if (newprio < -20)
		newprio = -20;
	if (newprio > 19)
		newprio = 19;
	current->pcb$b_prib = newprio;
	return 0;
}

#endif

inline struct task_struct *find_process_by_pid(pid_t pid)
{
	struct task_struct *tsk = current;

	if (pid)
		tsk = find_task_by_pid(pid);
	if (tsk)
	return tsk;
        long *x=&tsk;
        printk(KERN_EMERG "FIND %d %x %x %x %x %x %x %x %x %x %x\n",pid,pid,x[1],x[2],x[3],x[4],x[5],x[6],x[7],x[8]);
	return 0;
}

static int setscheduler(pid_t pid, int policy, 
			struct sched_param *param)
{
	struct sched_param lp;
	struct task_struct *p;
	int retval;

	retval = -EINVAL;
	if (!param || pid < 0)
		goto out_nounlock;

	retval = -EFAULT;
	if (copy_from_user(&lp, param, sizeof(struct sched_param)))
		goto out_nounlock;

	/*
	 * We play safe to avoid deadlocks.
	 */
	read_lock_irq(&tasklist_lock);
	spin_lock(&runqueue_lock);

	p = find_process_by_pid(pid);

	retval = -ESRCH;
	if (!p)
		goto out_unlock;
			
	if (policy < 0)
		policy = p->pcb$l_sched_policy;
	else {
		retval = -EINVAL;
		if (policy != PCB$K_SCHED_FIFO && policy != PCB$K_SCHED_RR &&
				policy != PCB$K_SCHED_OTHER)
			goto out_unlock;
	}
	
	/*
	 * Valid priorities for PCB$K_SCHED_FIFO and PCB$K_SCHED_RR are 1..99, valid
	 * priority for PCB$K_SCHED_OTHER is 0.
	 */
	retval = -EINVAL;
	if (lp.sched_priority < 0 || lp.sched_priority > 99)
		goto out_unlock;
	if ((policy == PCB$K_SCHED_OTHER) != (lp.sched_priority == 0))
		goto out_unlock;

	retval = -EPERM;
	if ((policy == PCB$K_SCHED_FIFO || policy == PCB$K_SCHED_RR) && 
	    !capable(CAP_SYS_NICE))
		goto out_unlock;
	if ((current->euid != p->euid) && (current->euid != p->uid) &&
	    !capable(CAP_SYS_NICE))
		goto out_unlock;

	retval = 0;
	p->pcb$l_sched_policy = policy;
	//	p->rt_priority = lp.sched_priority;
	//	if (task_on_runqueue(p))
	//		move_first_runqueue(p);

#if 0
	current->need_resched = 1;
#endif

out_unlock:
	spin_unlock(&runqueue_lock);
	read_unlock_irq(&tasklist_lock);

out_nounlock:
	return retval;
}

asmlinkage long sys_sched_setscheduler(pid_t pid, int policy, 
				      struct sched_param *param)
{
	if (pid>0)
	  pid = exe$epid_to_ipid(pid);
	return setscheduler(pid, policy, param);
}

asmlinkage long sys_sched_setparam(pid_t pid, struct sched_param *param)
{
	if (pid>0)
	  pid = exe$epid_to_ipid(pid);
	return setscheduler(pid, -1, param);
}

asmlinkage long sys_sched_getscheduler(pid_t pid)
{
	struct task_struct *p;
	int retval;

	if (pid>0)
	  pid = exe$epid_to_ipid(pid);

	retval = -EINVAL;
	if (pid < 0)
		goto out_nounlock;

	retval = -ESRCH;
	read_lock(&tasklist_lock);
	p = find_process_by_pid(pid);
	if (p)
		retval = p->pcb$l_sched_policy & ~SCHED_YIELD;
	read_unlock(&tasklist_lock);

out_nounlock:
	return retval;
}

asmlinkage long sys_sched_getparam(pid_t pid, struct sched_param *param)
{
	struct task_struct *p;
	struct sched_param lp;
	int retval;

	if (pid>0)
	  pid = exe$epid_to_ipid(pid);

	retval = -EINVAL;
	if (!param || pid < 0)
		goto out_nounlock;

	read_lock(&tasklist_lock);
	p = find_process_by_pid(pid);
	retval = -ESRCH;
	if (!p)
		goto out_unlock;
	//	lp.sched_priority = p->rt_priority;
	read_unlock(&tasklist_lock);

	/*
	 * This one might sleep, we cannot do it with a spinlock held ...
	 */
	retval = copy_to_user(param, &lp, sizeof(*param)) ? -EFAULT : 0;

out_nounlock:
	return retval;

out_unlock:
	read_unlock(&tasklist_lock);
	return retval;
}

asmlinkage long sys_sched_yield(void)
{
	/*
	 * Trick. sched_yield() first counts the number of truly 
	 * 'pending' runnable processes, then returns if it's
	 * only the current processes. (This test does not have
	 * to be atomic.) In threaded applications this optimization
	 * gets triggered quite often.
	 */

	int nr_pending = nr_running;

#if CONFIG_SMP
#if 0
	// not yet?
	int i;

	// Subtract non-idle processes running on other CPUs.
	for (i = 0; i < smp_num_cpus; i++) {
		int cpu = cpu_logical_map(i);
		if (aligned_data[cpu].schedule_data.curr != idle_task(cpu))
			nr_pending--;
	}
#endif
#else
	// on UP this process is on the runqueue as well
	nr_pending--;
#endif
	if (nr_pending) {
		/*
		 * This process can only be rescheduled by us,
		 * so this is safe without any locking.
		 */
	  //		if (current->pcb$l_sched_policy == PCB$K_SCHED_OTHER)
	  //			current->pcb$l_sched_policy |= SCHED_YIELD;
#if 0
		current->need_resched = 1;
#endif

		spin_lock_irq(&runqueue_lock);
		//		move_last_runqueue(current);
		spin_unlock_irq(&runqueue_lock);
	}
	return 0;
}

asmlinkage long sys_sched_get_priority_max(int policy)
{
	int ret = -EINVAL;

	switch (policy) {
	case PCB$K_SCHED_FIFO:
	case PCB$K_SCHED_RR:
		ret = 99;
		break;
	case PCB$K_SCHED_OTHER:
		ret = 0;
		break;
	}
	return ret;
}

asmlinkage long sys_sched_get_priority_min(int policy)
{
	int ret = -EINVAL;

	switch (policy) {
	case PCB$K_SCHED_FIFO:
	case PCB$K_SCHED_RR:
		ret = 1;
		break;
	case PCB$K_SCHED_OTHER:
		ret = 0;
	}
	return ret;
}

asmlinkage long sys_sched_rr_get_interval(pid_t pid, struct timespec *interval)
{
	struct timespec t;
	struct task_struct *p;
	int retval = -EINVAL;

	if (pid>0)
	  pid = exe$epid_to_ipid(pid);

	if (pid < 0)
		goto out_nounlock;

	retval = -ESRCH;
	read_lock(&tasklist_lock);
	p = find_process_by_pid(pid);
	if (p)
		jiffies_to_timespec(p->pcb$l_sched_policy & PCB$K_SCHED_FIFO ? 0 : NICE_TO_TICKS(p->pcb$b_prib), &t);
	read_unlock(&tasklist_lock);
	if (p)
		retval = copy_to_user(interval, &t, sizeof(t)) ? -EFAULT : 0;
out_nounlock:
	return retval;
out_unlock:
	read_unlock(&tasklist_lock);
	return retval;
}

static void show_task(struct task_struct * p)
{
	unsigned long free = 0;
	int state;
	static const char * stat_nam[] = { "R", "S", "D", "Z", "T", "W" };

	printk(KERN_EMERG "%-13.13s ", p->pcb$t_lname);
	state = p->state ? ffz(~p->state) + 1 : 0;
	if (((unsigned) state) < sizeof(stat_nam)/sizeof(char *))
		printk(stat_nam[state]);
	else
		printk(KERN_EMERG " ");
#if (BITS_PER_LONG == 32)
	if (p == current)
		printk(KERN_EMERG " current  ");
	else
		printk(KERN_EMERG " %08lX ", thread_saved_pc(&p->thread));
#else
	if (p == current)
		printk(KERN_EMERG "   current task   ");
	else
		printk(KERN_EMERG " %016lx ", thread_saved_pc(&p->thread));
#endif
	{
		unsigned long * n = (unsigned long *) (p+1);
		while (!*n)
			n++;
		free = (unsigned long) n - (unsigned long)(p+1);
	}
	printk(KERN_EMERG "%5lu %5d %6d ", free, p->pcb$l_pid, p->p_pptr->pcb$l_pid);
	if (p->p_cptr)
		printk(KERN_EMERG "%5d ", p->p_cptr->pcb$l_pid);
	else
		printk(KERN_EMERG "      ");
	if (!p->mm)
		printk(KERN_EMERG " (L-TLB)\n");
	else
		printk(KERN_EMERG " (NOTLB)\n");

	{
		extern void show_trace_task(struct task_struct *tsk);
		show_trace_task(p);
	}
}

char * render_sigset_t(sigset_t *set, char *buffer)
{
	int i = _NSIG, x;
	do {
		i -= 4, x = 0;
		if (sigismember(set, i+1)) x |= 1;
		if (sigismember(set, i+2)) x |= 2;
		if (sigismember(set, i+3)) x |= 4;
		if (sigismember(set, i+4)) x |= 8;
		*buffer++ = (x < 10 ? '0' : 'a' - 10) + x;
	} while (i >= 4);
	*buffer = 0;
	return buffer;
}

void show_state(void)
{
	struct task_struct *p;

#if (BITS_PER_LONG == 32)
	printk(KERN_EMERG "\n"
	       "                         free                        sibling\n");
	printk(KERN_EMERG "  task             PC    stack   pid father child younger older\n");
#else
	printk(KERN_EMERG "\n"
	       "                                 free                        sibling\n");
	printk(KERN_EMERG "  task                 PC        stack   pid father child younger older\n");
#endif
	read_lock(&tasklist_lock);
	for_each_task_pre1(p) {
		/*
		 * reset the NMI-timeout, listing all files on a slow
		 * console might take alot of time:
		 */
		touch_nmi_watchdog();
		show_task(p);
	}
	for_each_task_post1(p);
	read_unlock(&tasklist_lock);
}

/**
 * reparent_to_init() - Reparent the calling kernel thread to the init task.
 *
 * If a kernel thread is launched as a result of a system call, or if
 * it ever exits, it should generally reparent itself to init so that
 * it is correctly cleaned up on exit.
 *
 * The various task state such as scheduling policy and priority may have
 * been inherited fro a user process, so we reset them to sane values here.
 *
 * NOTE that reparent_to_init() gives the caller full capabilities.
 */
void reparent_to_init(void)
{
	struct task_struct *this_task = current;

	write_lock_irq(&tasklist_lock);

	/* Reparent to init */
	REMOVE_LINKS(this_task);
	this_task->p_pptr = child_reaper;
	this_task->p_opptr = child_reaper;
	SET_LINKS(this_task);

	/* Set the exit signal to SIGCHLD so we signal init on exit */
	this_task->exit_signal = SIGCHLD;

	/* We also take the runqueue_lock while altering task fields
	 * which affect scheduling decisions */
	spin_lock(&runqueue_lock);

	this_task->ptrace = 0;
	this_task->pcb$b_prib = DEFPRI;
	this_task->pcb$l_sched_policy = PCB$K_SCHED_OTHER;
	/* cpus_allowed? */
	/* rt_priority? */
	/* signals? */
	this_task->cap_effective = CAP_INIT_EFF_SET;
	this_task->cap_inheritable = CAP_INIT_INH_SET;
	this_task->cap_permitted = CAP_FULL_SET;
	this_task->keep_capabilities = 0;
	memcpy(this_task->rlim, init_task.rlim, sizeof(*(this_task->rlim)));
	this_task->user = INIT_USER;

	spin_unlock(&runqueue_lock);
	write_unlock_irq(&tasklist_lock);
}

/*
 *	Put all the gunge required to become a kernel thread without
 *	attached user resources in one place where it belongs.
 */

void daemonize(void)
{
	struct fs_struct *fs;


	/*
	 * If we were started as result of loading a module, close all of the
	 * user space pages.  We don't need them, and if we didn't close them
	 * they would be locked into memory.
	 */
	exit_mm(current);

	current->session = 1;
	current->pgrp = 1;
	current->tty = NULL;

	/* Become as one with the init task */

	exit_fs(current);	/* current->fs->count--; */
	fs = init_task.fs;
	current->fs = fs;
	atomic_inc(&fs->count);
 	exit_files(current);
	current->files = init_task.files;
	atomic_inc(&current->files->count);
}

extern unsigned long wait_init_idle;

extern struct _phd system_phd;

void __init init_idle(void)
{
  struct _pcb * cur = smp$gl_cpu_data[smp_processor_id()]->cpu$l_curpcb;
  //	if (current != &init_task && task_on_runqueue(current)) {
  //		printk("UGH! (%d:%d) was on the runqueue, removing.\n",
  //			smp_processor_id(), current->pcb$l_pid);
  //		del_from_runqueue(current);
  //	}
	//sched_data->curr = current;
	//	sched_data->last_schedule = get_cycles();
	clear_bit(cur->pcb$l_cpu_id, &wait_init_idle);
	cur->psl=0;
	cur->pslindex=0;
	cur->pcb$b_asten=15;
	cur->phd$b_astlvl=4;
	cur->pr_astlvl=4;
	cur->pcb$l_phd=&system_phd;
	// current->pcb$l_phd->phd$q_ptbr=current->mm->pgd; // same wait
	printk("done init_idle\n");
	done_init_idle=1;
}

extern void init_timervecs (void);

extern unsigned long pcbvec[];
extern unsigned long seqvec[];

void __init sched_init(void)
{
  /*
   * We have to do a little magic to get the first
   * process right in SMP mode.
	 */
  int cpuid = smp_processor_id();
  int nr;
  struct _cpu * cpu;

  init_task.pcb$l_cpu_id = cpuid;
  cpu=smp$gl_cpu_data[cpuid];

  init_task.pcb$b_type = DYN$C_PCB;

  init_task.pcb$b_pri=31;
  init_task.pcb$b_prib=31;
  qhead_init(&init_task.pcb$l_astqfl);
  cpu->cpu$l_curpcb=&init_task;
  cpu->cpu$b_cur_pri=31;

  sch$gl_pcbvec=pcbvec;
  sch$gl_seqvec=seqvec;
  memset(sch$gl_pcbvec,0,(unsigned long)MAXPROCESSCNT*sizeof(unsigned long));
  memset(sch$gl_seqvec,0,(unsigned long)MAXPROCESSCNT*sizeof(unsigned long));

  printk("pid 0 here %x %x\n",init_task.pcb$l_astqfl,&init_task.pcb$l_astqfl); 
  //	{ int i,j; for(j=0;j<2;j++) for(i=0;i<1000000000;i++); }

  init_timervecs();

	/*
	 * The boot idle thread does lazy MMU switching as well:
	 */
  atomic_inc(&init_mm.mm_count);
  enter_lazy_tlb(&init_mm, current, cpuid);
}

