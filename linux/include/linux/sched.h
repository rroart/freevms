// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004  

#ifndef _LINUX_SCHED_H
#define _LINUX_SCHED_H

#include <asm/param.h>	/* for HZ */

extern unsigned long event;

#include <linux/config.h>
#include <linux/binfmts.h>
#include <linux/threads.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/times.h>
#include <linux/timex.h>
#include <linux/rbtree.h>

#include <asm/system.h>
#include <asm/semaphore.h>
#include <asm/page.h>
#include <asm/ptrace.h>
#include <asm/mmu.h>

#include <linux/smp.h>
#include <linux/tty.h>
#include <linux/sem.h>
#include <linux/signal.h>
#include <linux/securebits.h>
#include <linux/fs_struct.h>

#include <pcbdef.h>
#include <asmlink.h>

#include <system_data_cells.h>
#include <sysgen.h>

struct exec_domain;

/*
 * cloning flags:
 */
#define CSIGNAL		0x000000ff	/* signal mask to be sent at exit */
#define CLONE_VM	0x00000100	/* set if VM shared between processes */
#define CLONE_FS	0x00000200	/* set if fs info shared between processes */
#define CLONE_FILES	0x00000400	/* set if open files shared between processes */
#define CLONE_SIGHAND	0x00000800	/* set if signal handlers and blocked signals shared */
#define CLONE_PID	0x00001000	/* set if pid shared */
#define CLONE_PTRACE	0x00002000	/* set if we want to let tracing continue on the child too */
#define CLONE_VFORK	0x00004000	/* set if the parent wants the child to wake it up on mm_release */
#define CLONE_PARENT	0x00008000	/* set if we want to have the same parent as the cloner */
#define CLONE_THREAD	0x00010000	/* Same thread group? */

#define CLONE_SIGNAL	(CLONE_SIGHAND | CLONE_THREAD)

/*
 * These are the constant used to fake the fixed-point load-average
 * counting. Some notes:
 *  - 11 bit fractions expand to 22 bits by the multiplies: this gives
 *    a load-average precision of 10 bits integer + 11 bits fractional
 *  - if you want to count load-averages more often, you need more
 *    precision, or rounding will get you. With 2-second counting freq,
 *    the EXP_n values would be 1981, 2034 and 2043 if still using only
 *    11 bit fractions.
 */
extern unsigned long avenrun[];		/* Load averages */

#define FSHIFT		11		/* nr of bits of precision */
#define FIXED_1		(1<<FSHIFT)	/* 1.0 as fixed-point */
#define LOAD_FREQ	(5*HZ)		/* 5 sec intervals */
#define EXP_1		1884		/* 1/exp(5sec/1min) as fixed-point */
#define EXP_5		2014		/* 1/exp(5sec/5min) */
#define EXP_15		2037		/* 1/exp(5sec/15min) */

#define CALC_LOAD(load,exp,n) \
	load *= exp; \
	load += n*(FIXED_1-exp); \
	load >>= FSHIFT;

#define CT_TO_SECS(x)	((x) / HZ)
#define CT_TO_USECS(x)	(((x) % HZ) * 1000000/HZ)

extern int nr_running, nr_threads;
extern int last_pid;

#include <linux/fs.h>
#include <linux/time.h>
#include <linux/param.h>
#include <linux/resource.h>
#include <linux/timer.h>

#include <asm/processor.h>

#define TASK_RUNNING		0
#define TASK_INTERRUPTIBLE	1
#define TASK_UNINTERRUPTIBLE	2
#define TASK_ZOMBIE		4
#define TASK_STOPPED		8

#define __set_task_state(tsk, state_value)		\
	do { (tsk)->state = (state_value); } while (0)
#ifdef CONFIG_SMP
#define set_task_state(tsk, state_value)		\
	set_mb((tsk)->state, (state_value))
#else
#define set_task_state(tsk, state_value)		\
	__set_task_state((tsk), (state_value))
#endif

#define __set_current_state(state_value)			\
	do { current->state = (state_value); } while (0)
#ifdef CONFIG_SMP
#define set_current_state(state_value)		\
	set_mb(current->state, (state_value))
#else
#define set_current_state(state_value)		\
	__set_current_state(state_value)
#endif

/*
 * Scheduling policies
 */
#define SCHED_OTHER		0
#define SCHED_FIFO		1
#define SCHED_RR		2

#define	PCB$K_SCHED_OTHER	0
#define	PCB$K_SCHED_FIFO	1
#define	PCB$K_SCHED_RR	        2

#define	KTB$K_SCHED_OTHER	0
#define	KTB$K_SCHED_FIFO	1
#define	KTB$K_SCHED_RR	        2

/*
 * This is an additional bit set when we want to
 * yield the CPU for one re-schedule..
 */
#define SCHED_YIELD		0x10

struct sched_param {
	int sched_priority;
};

struct completion;

#ifdef __KERNEL__

#include <linux/spinlock.h>

/*
 * This serializes "schedule()" and also protects
 * the run-queue from deletions/modifications (but
 * _adding_ to the beginning of the run-queue has
 * a separate lock).
 */
extern rwlock_t tasklist_lock;
extern spinlock_t runqueue_lock;
extern spinlock_t mmlist_lock;

extern void sched_init(void);
extern void init_idle(void);
extern void show_state(void);
extern void cpu_init (void);
extern void trap_init(void);
extern void update_process_times(int user);
extern void update_one_process(struct task_struct *p, unsigned long user,
			       unsigned long system, int cpu);

#define	MAX_SCHEDULE_TIMEOUT	LONG_MAX
extern signed long FASTCALL(schedule_timeout(signed long timeout));
asmlinkage void schedule(void);

extern int schedule_task(struct tq_struct *task);
extern void flush_scheduled_tasks(void);
extern int start_context_thread(void);
extern int current_is_keventd(void);

/*
 * The default fd array needs to be at least BITS_PER_LONG,
 * as this is the granularity returned by copy_fdset().
 */
#define NR_OPEN_DEFAULT BITS_PER_LONG

/*
 * Open file table structure
 */
struct files_struct {
	atomic_t count;
	rwlock_t file_lock;	/* Protects all the below members.  Nests inside tsk->alloc_lock */
	int max_fds;
	int max_fdset;
	int next_fd;
	struct file ** fd;	/* current fd array */
	fd_set *close_on_exec;
	fd_set *open_fds;
	fd_set close_on_exec_init;
	fd_set open_fds_init;
	struct file * fd_array[NR_OPEN_DEFAULT];
};

#define INIT_FILES \
{ 							\
	count:		ATOMIC_INIT(1), 		\
	file_lock:	RW_LOCK_UNLOCKED, 		\
	max_fds:	NR_OPEN_DEFAULT, 		\
	max_fdset:	__FD_SETSIZE, 			\
	next_fd:	0, 				\
	fd:		&init_files.fd_array[0], 	\
	close_on_exec:	&init_files.close_on_exec_init, \
	open_fds:	&init_files.open_fds_init, 	\
	close_on_exec_init: { { 0, } }, 		\
	open_fds_init:	{ { 0, } }, 			\
	fd_array:	{ NULL, } 			\
}

/* Maximum number of active map areas.. This is a random (large) number */
#define MAX_MAP_COUNT	(65536)

struct mm_struct {
	struct vm_area_struct * mmap;		/* list of VMAs */
	rb_root_t mm_rb;
	struct vm_area_struct * mmap_cache;	/* last find_vma result */
	pgd_t * pgd;
	atomic_t mm_users;			/* How many users with user space? */
	atomic_t mm_count;			/* How many references to "struct mm_struct" (users count as 1) */
	int map_count;				/* number of VMAs */
	struct rw_semaphore mmap_sem;
	spinlock_t page_table_lock;		/* Protects task page tables and mm->rss */

	struct list_head mmlist;		/* List of all active mm's.  These are globally strung
						 * together off init_mm.mmlist, and are protected
						 * by mmlist_lock
						 */

	unsigned long start_code, end_code, start_data, end_data;
	unsigned long start_brk, brk, start_stack;
	unsigned long arg_start, arg_end, env_start, env_end;
	unsigned long rss, total_vm, locked_vm;
	unsigned long def_flags;
	unsigned long cpu_vm_mask;
	unsigned long swap_address;

	unsigned dumpable:1;

	/* Architecture-specific MM context */
	mm_context_t context;
};

extern int mmlist_nr;

#define INIT_MM(name) \
{			 				\
	mm_rb:		RB_ROOT,			\
	pgd:		swapper_pg_dir, 		\
	mm_users:	ATOMIC_INIT(2), 		\
	mm_count:	ATOMIC_INIT(1), 		\
	mmap_sem:	__RWSEM_INITIALIZER(name.mmap_sem), \
	page_table_lock: SPIN_LOCK_UNLOCKED, 		\
	mmlist:		LIST_HEAD_INIT(name.mmlist),	\
}

struct signal_struct {
	atomic_t		count;
	struct k_sigaction	action[_NSIG];
	spinlock_t		siglock;
};


#define INIT_SIGNALS {	\
	count:		ATOMIC_INIT(1), 		\
	action:		{ {{0,}}, }, 			\
	siglock:	SPIN_LOCK_UNLOCKED 		\
}

/*
 * Some day this will be a full-fledged user tracking system..
 */
struct user_struct {
	atomic_t __count;	/* reference count */
	atomic_t processes;	/* How many processes does this user have? */
	atomic_t files;		/* How many open files does this user have? */

	/* Hash table maintenance information */
	struct user_struct *next, **pprev;
	uid_t uid;
};

#define get_current_user() ({ 				\
	struct user_struct *__user = current->user;	\
	atomic_inc(&__user->__count);			\
	__user; })

extern struct user_struct root_user;
#define INIT_USER (&root_user)

#define _pcb task_struct
#define _ktb task_struct
struct task_struct {
  struct _pcb *pcb$l_sqfl;
  struct _pcb *pcb$l_sqbl;            
  unsigned short pcb$w_size;
  unsigned char pcb$b_type;
  unsigned char pcb$b_wefc;

	/*
	 * offsets of these are hardcoded elsewhere - touch with care
	 * meaning entry.S and getuser.S -roar this is offset 12
	 */
	volatile long state;	/* -1 unrunnable, 0 runnable, >0 stopped */
	unsigned long flags;	/* per process flags, defined below */
	int sigpending;
	mm_segment_t addr_limit;	/* thread address space:
					 	0-0xBFFFFFFF for user-thead
						0-0xFFFFFFFF for kernel-thread
					 */
	struct exec_domain *exec_domain;
	volatile long need_resched;
	unsigned long ptrace;

	int lock_depth;		/* Lock depth */

/*
 * offset 32 begins here on 32-bit platforms. We keep
 * all fields in a single cacheline that are needed for
 * the goodness() loop in schedule().
 */
	struct mm_struct *mm;
	int processor_not;
	/*
	 * cpus_runnable is ~0 if the process is not running on any
	 * CPU. It's (1 << cpu) if it's running on a CPU. This mask
	 * is updated under the runqueue lock.
	 *
	 * To determine whether a process might run on a CPU, this
	 * mask is AND-ed with cpus_allowed.
	 */
	unsigned long cpus_runnable, cpus_allowed;
	/*
	 * (only the 'next' pointer fits into the cacheline, but
	 * that's just fine.)
	 */
	struct list_head run_list;
	unsigned long sleep_time;

	struct task_struct *next_task, *prev_task; // remember to remove these
	struct mm_struct *active_mm;
	struct list_head local_pages;
	unsigned int allocation_order, nr_local_pages;

/* task state */
	struct linux_binfmt *binfmt;
	int exit_code, exit_signal;
	int pdeath_signal;  /*  The signal sent when the parent dies  */
	/* ??? */
	unsigned long personality;
	int did_exec:1;
  //pid_t pid;
	pid_t pgrp;
	pid_t tty_old_pgrp;
	pid_t session;
  //pid_t tgid;
	/* boolean value for session group leader */
	int leader;
	/* 
	 * pointers to (original) parent process, youngest child, younger sibling,
	 * older sibling, respectively.  (p->father can be replaced with 
	 * p->p_pptr->pid)
	 */
	struct task_struct *p_opptr, *p_pptr, *p_cptr, *p_ysptr, *p_osptr;
	struct list_head thread_group;

	/* PID hash table linkage. */
	struct task_struct *pidhash_next;
	struct task_struct **pidhash_pprev;

	wait_queue_head_t wait_chldexit;	/* for wait4() */
	struct completion *vfork_done;		/* for vfork() */
	unsigned long rt_priority_not;
	unsigned long it_real_value, it_prof_value, it_virt_value;
	unsigned long it_real_incr, it_prof_incr, it_virt_incr;
	struct timer_list real_timer;
	struct tms times;
	unsigned long start_time;
	long per_cpu_utime[NR_CPUS], per_cpu_stime[NR_CPUS];
/* mm fault and swap info: this can arguably be seen as either mm-specific or thread-specific */
	unsigned long min_flt, maj_flt, nswap, cmin_flt, cmaj_flt, cnswap;
	int swappable:1;
/* process credentials */
	uid_t uid,euid,suid,fsuid;
	gid_t gid,egid,sgid,fsgid;
	int ngroups;
	gid_t	groups[NGROUPS];
	kernel_cap_t   cap_effective, cap_inheritable, cap_permitted;
	int keep_capabilities:1;
	struct user_struct *user;
/* limits */
	struct rlimit rlim[RLIM_NLIMITS];
	unsigned short used_math;
  //	char comm[16];
/* file system info */
	int link_count, total_link_count;
	struct tty_struct *tty; /* NULL if no tty */
	unsigned int locks; /* How many file locks are being held */
/* ipc stuff */
	struct sem_undo *semundo;
	struct sem_queue *semsleeping;
/* CPU-specific state of this task */
	struct thread_struct thread;
/* filesystem information */
	struct fs_struct *fs;
/* open file information */
	struct files_struct *files;
/* signal handlers */
	spinlock_t sigmask_lock;	/* Protects signal and blocked */
	struct signal_struct *sig;

	sigset_t blocked;
	struct sigpending pending;

	unsigned long sas_ss_sp;
	size_t sas_ss_size;
	int (*notifier)(void *priv);
	void *notifier_data;
	sigset_t *notifier_mask;
	
/* Thread group tracking */
   	u32 parent_exec_id;
   	u32 self_exec_id;
/* Protection of (de-)allocation: mm, files, fs, tty */
	spinlock_t alloc_lock;

/* journalling filesystem info */
	void *journal_info;

  unsigned long pcb$l_phypcb;
  struct _acb * pcb$l_astqfl;
  struct _acb * pcb$l_astqbl;
  unsigned char pcb$b_astact;
  unsigned char pcb$b_asten;
  unsigned short pcb$w_state;
  unsigned char pcb$b_pri;             
  unsigned char pcb$b_prib;
  unsigned char pcb$b_reserved_b1;
  unsigned char pcb$b_affinity_skip;
  unsigned long pcb$l_owner;
  unsigned long pcb$l_sts;
  unsigned long pcb$l_sts2;
  unsigned long pcb$l_wtime;         
  unsigned long pcb$l_onqtime;         
  unsigned long pcb$l_waitime;         
  unsigned short pcb$w_astcnt;          
  unsigned short pcb$w_biocnt;          
  unsigned short pcb$w_biolm;           
  short pcb$w_diocnt;                   
  short pcb$w_diolm;                    
  unsigned short pcb$w_prccnt;          
  char pcb$t_terminal [8];            
  unsigned long pcb$l_efwm;            
  unsigned long pcb$l_efcs;            
  unsigned long pcb$l_efcu;            
  unsigned long pcb$l_efc2p;            
  unsigned long pcb$l_efc3p;            
  /* why do the next 3 overlap with efc? */
  unsigned short pcb$w_pgflchar;
  unsigned char pcb$b_pgflindex;
  unsigned char pcb$reserved1;
  unsigned long pcb$l_pid;             
  unsigned long pcb$l_epid;             
  unsigned long pcb$l_eowner;
  // not ready for this yet? 
  struct _phd * pcb$l_phd;          //note overlapping stuff. move it.

  unsigned short pcb$w_aptcnt;
  unsigned short pcb$w_mtxcnt;
  unsigned long pcb$l_gpgcnt;
  unsigned long pcb$l_ppgcnt;
  unsigned long pcb$l_jib;
  unsigned long pcb$l_wsswp;
  unsigned long pcb$l_swapsize;

  unsigned long pcb$l_priv;
  unsigned long pcb$l_arb;
  unsigned char pcb$t_res1[48];
unsigned long pcb$l_uic;
  unsigned char pcb$t_res2[60];
unsigned long pcb$l_orb;
  unsigned short pcb$w_res3;
  unsigned short pcb$w_tmbu;
struct _lkb * pcb$l_lockqfl;
struct _lkb * pcb$l_lockqbl;
unsigned long pcb$l_dlckpri;
unsigned long pcb$l_ipast;
unsigned long pcb$l_defprot;
unsigned long pcb$l_pmb;
unsigned long pcb$l_affinity;
unsigned long pcb$l_sched_spare;
unsigned long pcb$l_capability;
unsigned long pcb$l_cpu_id;
unsigned long pcb$l_cputim;
  unsigned char pcb$t_lname[16];
unsigned long pcb$l_prcpdb;
unsigned long pcb$l_pixhist;
unsigned long pcb$l_ns_reserved_q1;
unsigned long pcb$l_affinity_callback;
unsigned long pcb$l_permanent_capability;
unsigned long pcb$l_permanent_cpu_affinity;
unsigned long pcb$l_cwpssrv_queue;
unsigned long pcb$l_current_affinity;
unsigned long pcb$l_capability_seq;
  unsigned char pcb$b_prisav;
  unsigned char pcb$b_pribsav;
  unsigned char pcb$nothing;
  unsigned char pcb$b_authpri;

/*	long counter; temp */
/*	long nice; temp */
  unsigned long pcb$l_sched_policy;
  // more phd

  signed short pcb$w_quant; /* really belongs in phd, but seems to have moved. ktb stuff? */
  unsigned short phd$w_prclm; /* ditto? */

  unsigned long pdg$l_pcb;
  unsigned char phd$b_astlvl; /* some cpus lack something */
  unsigned char pr_astlvl; /* some cpus lack something */

  union {
    struct {
      unsigned psl_cur_mod:2;
      unsigned psl_prv_mod:2;
      unsigned psl_ipl:5;
      unsigned psl_is:1;
      unsigned psl_intr:1;
    };
    unsigned short psl;
  };
  union {
    struct {
      unsigned oldpsl_cur_mod:2;
      unsigned oldpsl_prv_mod:2;
      unsigned oldpsl_ipl:5;
      unsigned oldpsl_is:1;
      unsigned oldpsl_intr:1;
    };
    unsigned short oldpsl;
  };
  unsigned short pslstk[32];
  unsigned char pslindex;
  long pcb$l_pqb;
  long ipr_sp[4];
};
/* will need a PCB. PCB is a null PCB placeholder */

/*
 * Per process flags
 */
#define PF_ALIGNWARN	0x00000001	/* Print alignment warning msgs */
					/* Not implemented yet, only for 486*/
#define PF_STARTING	0x00000002	/* being created */
#define PF_EXITING	0x00000004	/* getting shut down */
#define PF_FORKNOEXEC	0x00000040	/* forked but didn't exec */
#define PF_SUPERPRIV	0x00000100	/* used super-user privileges */
#define PF_DUMPCORE	0x00000200	/* dumped core */
#define PF_SIGNALED	0x00000400	/* killed by a signal */
#define PF_MEMALLOC	0x00000800	/* Allocating memory */
#define PF_MEMDIE	0x00001000	/* Killed for out-of-memory */
#define PF_FREE_PAGES	0x00002000	/* per process page freeing */
#define PF_NOIO		0x00004000	/* avoid generating further I/O */

#define PF_USEDFPU	0x00100000	/* task used FPU this quantum (SMP) */

/*
 * Ptrace flags
 */

#define PT_PTRACED	0x00000001
#define PT_TRACESYS	0x00000002
#define PT_DTRACE	0x00000004	/* delayed trace (used on m68k, i386) */
#define PT_TRACESYSGOOD	0x00000008
#define PT_PTRACE_CAP	0x00000010	/* ptracer can follow suid-exec */

/*
 * Limit the stack by to some sane default: root can always
 * increase this limit if needed..  8MB seems reasonable.
 */
#define _STK_LIM	(8*1024*1024)

#define DEF_COUNTER	(10*HZ/100)	/* 100 ms time slice */
#define MAX_COUNTER	(20*HZ/100)
#define DEF_NICE	(0)


/*
 * The default (Linux) execution domain.
 */
extern struct exec_domain	default_exec_domain;

/*
 *  INIT_TASK is used to set up the first task table, touch at
 * your own risk!. Base=0, limit=0x1fffff (=2MB)
 */
/*    counter:		DEF_COUNTER,					
    nice:		DEF_NICE,          */
#define INIT_TASK(tsk)	\
{									\
    state:		0,						\
    flags:		0,						\
    sigpending:		0,						\
    addr_limit:		KERNEL_DS,					\
    exec_domain:	&default_exec_domain,				\
    lock_depth:		-1,						\
    pcb$l_sched_policy:		SCHED_OTHER,					\
    mm:			&init_mm,						\
    active_mm:		&init_mm,					\
    cpus_runnable:	-1,						\
    cpus_allowed:	-1,						\
    run_list:		LIST_HEAD_INIT(tsk.run_list),			\
    p_opptr:		&tsk,						\
    p_pptr:		&tsk,						\
    thread_group:	LIST_HEAD_INIT(tsk.thread_group),		\
    wait_chldexit:	__WAIT_QUEUE_HEAD_INITIALIZER(tsk.wait_chldexit),\
    real_timer:		{						\
	function:		it_real_fn				\
    },									\
    cap_effective:	CAP_INIT_EFF_SET,				\
    cap_inheritable:	CAP_INIT_INH_SET,				\
    cap_permitted:	CAP_FULL_SET,					\
    keep_capabilities:	0,						\
    rlim:		INIT_RLIMITS,					\
    user:		INIT_USER,					\
    pcb$t_lname:		"swapper",					\
    thread:		INIT_THREAD,					\
    fs:			&init_fs,					\
    files:		&init_files,					\
    sigmask_lock:	SPIN_LOCK_UNLOCKED,				\
    sig:		&init_signals,					\
    pending:		{ NULL, &tsk.pending.head, {{0}}},		\
    blocked:		{{0}},						\
    alloc_lock:		SPIN_LOCK_UNLOCKED,				\
    journal_info:	NULL,						\
    pcb$l_priv:		-1,						\
    pcb$l_uic:		0x40001,						\
}


#ifndef INIT_TASK_SIZE
# define INIT_TASK_SIZE	2048*sizeof(long)
#endif

union task_union {
	struct task_struct task;
	unsigned long stack[INIT_TASK_SIZE/sizeof(long)];
};

extern union task_union init_task_union;

extern struct   mm_struct init_mm;
extern struct task_struct *init_tasks[NR_CPUS];

#define task_has_cpu(tsk) ((tsk)->cpus_runnable != ~0UL)

/* per-UID process charging. */
extern struct user_struct * alloc_uid(uid_t);
extern void free_uid(struct user_struct *);

#include <asm/current.h>

extern unsigned long volatile jiffies;
extern unsigned long itimer_ticks;
extern unsigned long itimer_next;
extern struct timeval xtime;
extern void do_timer(struct pt_regs *);

extern unsigned int * prof_buffer;
extern unsigned long prof_len;
extern unsigned long prof_shift;

#define CURRENT_TIME (xtime.tv_sec)

extern void FASTCALL(__wake_up(wait_queue_head_t *q, unsigned int mode, int nr));
extern void FASTCALL(__wake_up_sync(wait_queue_head_t *q, unsigned int mode, int nr));
extern void FASTCALL(sleep_on(wait_queue_head_t *q));
extern long FASTCALL(sleep_on_timeout(wait_queue_head_t *q,
				      signed long timeout));
extern void FASTCALL(interruptible_sleep_on(wait_queue_head_t *q));
extern long FASTCALL(interruptible_sleep_on_timeout(wait_queue_head_t *q,
						    signed long timeout));
extern int FASTCALL(wake_up_process(struct task_struct * tsk));

extern void FASTCALL(__wake_up2(wait_queue_head_t *q, unsigned int mode, int nr,int priclass));
extern void FASTCALL(__wake_up_sync2(wait_queue_head_t *q, unsigned int mode, int nr, int priclass));
extern void FASTCALL(sleep_on(wait_queue_head_t *q));
extern long FASTCALL(sleep_on_timeout(wait_queue_head_t *q,
				      signed long timeout));
extern void FASTCALL(interruptible_sleep_on(wait_queue_head_t *q));
extern long FASTCALL(interruptible_sleep_on_timeout(wait_queue_head_t *q,
						    signed long timeout));
extern int FASTCALL(wake_up_process2(struct task_struct * tsk,int priclass));

#define wake_up(x)			__wake_up((x),TASK_UNINTERRUPTIBLE | TASK_INTERRUPTIBLE, 1)
#define wake_up_nr(x, nr)		__wake_up((x),TASK_UNINTERRUPTIBLE | TASK_INTERRUPTIBLE, nr)
#define wake_up_all(x)			__wake_up((x),TASK_UNINTERRUPTIBLE | TASK_INTERRUPTIBLE, 0)
#define wake_up_sync(x)			__wake_up_sync((x),TASK_UNINTERRUPTIBLE | TASK_INTERRUPTIBLE, 1)
#define wake_up_sync_nr(x, nr)		__wake_up_sync((x),TASK_UNINTERRUPTIBLE | TASK_INTERRUPTIBLE, nr)
#define wake_up_interruptible(x)	__wake_up((x),TASK_INTERRUPTIBLE, 1)
#define wake_up_interruptible_nr(x, nr)	__wake_up((x),TASK_INTERRUPTIBLE, nr)
#define wake_up_interruptible_all(x)	__wake_up((x),TASK_INTERRUPTIBLE, 0)
#define wake_up_interruptible_sync(x)	__wake_up_sync((x),TASK_INTERRUPTIBLE, 1)
#define wake_up_interruptible_sync_nr(x) __wake_up_sync((x),TASK_INTERRUPTIBLE,  nr)

#define wake_up2(x,priclass)			__wake_up2((x),TASK_UNINTERRUPTIBLE | TASK_INTERRUPTIBLE, 1,priclass)
#define wake_up_nr2(x, nr,priclass)		__wake_up2((x),TASK_UNINTERRUPTIBLE | TASK_INTERRUPTIBLE, nr,priclass)
#define wake_up_all2(x,priclass)			__wake_up2((x),TASK_UNINTERRUPTIBLE | TASK_INTERRUPTIBLE, 0,priclass)
#define wake_up_sync2(x,priclass)			__wake_up_sync2((x),TASK_UNINTERRUPTIBLE | TASK_INTERRUPTIBLE, 1,priclass)
#define wake_up_sync_nr2(x, nr,priclass)		__wake_up_sync2((x),TASK_UNINTERRUPTIBLE | TASK_INTERRUPTIBLE, nr,priclass)
#define wake_up_interruptible2(x,priclass)	__wake_up2((x),TASK_INTERRUPTIBLE, 1,priclass)
#define wake_up_interruptible_nr2(x, nr,priclass)	__wake_up2((x),TASK_INTERRUPTIBLE, nr,priclass)
#define wake_up_interruptible_all2(x,priclass)	__wake_up2((x),TASK_INTERRUPTIBLE, 0,priclass)
#define wake_up_interruptible_sync2(x,priclass)	__wake_up_sync2((x),TASK_INTERRUPTIBLE, 1,priclass)
#define wake_up_interruptible_sync_nr2(x,priclass) __wake_up_sync2((x),TASK_INTERRUPTIBLE,  nr,priclass)

asmlinkage long sys_wait4(pid_t pid,unsigned int * stat_addr, int options, struct rusage * ru);

extern int in_group_p(gid_t);
extern int in_egroup_p(gid_t);

extern void proc_caches_init(void);
extern void flush_signals(struct task_struct *);
extern void flush_signal_handlers(struct task_struct *);
extern int dequeue_signal(sigset_t *, siginfo_t *);
extern void block_all_signals(int (*notifier)(void *priv), void *priv,
			      sigset_t *mask);
extern void unblock_all_signals(void);
extern int send_sig_info(int, struct siginfo *, struct task_struct *);
extern int force_sig_info(int, struct siginfo *, struct task_struct *);
extern int kill_pg_info(int, struct siginfo *, pid_t);
extern int kill_sl_info(int, struct siginfo *, pid_t);
extern int kill_proc_info(int, struct siginfo *, pid_t);
extern void notify_parent(struct task_struct *, int);
extern void do_notify_parent(struct task_struct *, int);
extern void force_sig(int, struct task_struct *);
extern int send_sig(int, struct task_struct *, int);
extern int kill_pg(pid_t, int, int);
extern int kill_sl(pid_t, int, int);
extern int kill_proc(pid_t, int, int);
extern int do_sigaction(int, const struct k_sigaction *, struct k_sigaction *);
extern int do_sigaltstack(const stack_t *, stack_t *, unsigned long);

static inline int signal_pending(struct task_struct *p)
{
	return (p->sigpending != 0);
}

/*
 * Re-calculate pending state from the set of locally pending
 * signals, globally pending signals, and blocked signals.
 */
static inline int has_pending_signals(sigset_t *signal, sigset_t *blocked)
{
	unsigned long ready;
	long i;

	switch (_NSIG_WORDS) {
	default:
		for (i = _NSIG_WORDS, ready = 0; --i >= 0 ;)
			ready |= signal->sig[i] &~ blocked->sig[i];
		break;

	case 4: ready  = signal->sig[3] &~ blocked->sig[3];
		ready |= signal->sig[2] &~ blocked->sig[2];
		ready |= signal->sig[1] &~ blocked->sig[1];
		ready |= signal->sig[0] &~ blocked->sig[0];
		break;

	case 2: ready  = signal->sig[1] &~ blocked->sig[1];
		ready |= signal->sig[0] &~ blocked->sig[0];
		break;

	case 1: ready  = signal->sig[0] &~ blocked->sig[0];
	}
	return ready !=	0;
}

/* Reevaluate whether the task has signals pending delivery.
   This is required every time the blocked sigset_t changes.
   All callers should have t->sigmask_lock.  */

static inline void recalc_sigpending(struct task_struct *t)
{
	t->sigpending = has_pending_signals(&t->pending.signal, &t->blocked);
}

/* True if we are on the alternate signal stack.  */

static inline int on_sig_stack(unsigned long sp)
{
	return (sp - current->sas_ss_sp < current->sas_ss_size);
}

static inline int sas_ss_flags(unsigned long sp)
{
	return (current->sas_ss_size == 0 ? SS_DISABLE
		: on_sig_stack(sp) ? SS_ONSTACK : 0);
}

extern int request_irq(unsigned int,
		       void (*handler)(int, void *, struct pt_regs *),
		       unsigned long, const char *, void *);
extern void free_irq(unsigned int, void *);

/*
 * This has now become a routine instead of a macro, it sets a flag if
 * it returns true (to do BSD-style accounting where the process is flagged
 * if it uses root privs). The implication of this is that you should do
 * normal permissions checks first, and check suser() last.
 *
 * [Dec 1997 -- Chris Evans]
 * For correctness, the above considerations need to be extended to
 * fsuser(). This is done, along with moving fsuser() checks to be
 * last.
 *
 * These will be removed, but in the mean time, when the SECURE_NOROOT 
 * flag is set, uids don't grant privilege.
 */
static inline int suser(void)
{
	if (!issecure(SECURE_NOROOT) && current->euid == 0) { 
		current->flags |= PF_SUPERPRIV;
		return 1;
	}
	return 0;
}

static inline int fsuser(void)
{
	if (!issecure(SECURE_NOROOT) && current->fsuid == 0) {
		current->flags |= PF_SUPERPRIV;
		return 1;
	}
	return 0;
}

/*
 * capable() checks for a particular capability.  
 * New privilege checks should use this interface, rather than suser() or
 * fsuser(). See include/linux/capability.h for defined capabilities.
 */

static inline int capable(int cap)
{
#if 1 /* ok now */
	if (cap_raised(current->cap_effective, cap))
#else
	if (cap_is_fs_cap(cap) ? current->fsuid == 0 : current->euid == 0)
#endif
	{
		current->flags |= PF_SUPERPRIV;
		return 1;
	}
	return 0;
}

/*
 * Routines for handling mm_structs
 */
extern struct mm_struct * mm_alloc(void);

extern struct mm_struct * start_lazy_tlb(void);
extern void end_lazy_tlb(struct mm_struct *mm);

/* mmdrop drops the mm and the page tables */
extern inline void FASTCALL(__mmdrop(struct mm_struct *));
static inline void mmdrop(struct mm_struct * mm)
{
	if (atomic_dec_and_test(&mm->mm_count))
		__mmdrop(mm);
}

/* mmput gets rid of the mappings and all user-space */
extern void mmput(struct mm_struct *);
/* Remove the current tasks stale references to the old mm_struct */
extern void mm_release(void);

/*
 * Routines for handling the fd arrays
 */
extern struct file ** alloc_fd_array(int);
extern int expand_fd_array(struct files_struct *, int nr);
extern void free_fd_array(struct file **, int);

extern fd_set *alloc_fdset(int);
extern int expand_fdset(struct files_struct *, int nr);
extern void free_fdset(fd_set *, int);

extern int  copy_thread(int, unsigned long, unsigned long, unsigned long, struct task_struct *, struct pt_regs *);
extern void flush_thread(void);
extern void exit_thread(void);

extern void exit_mm(struct task_struct *);
extern void exit_files(struct task_struct *);
extern void exit_sighand(struct task_struct *);

extern void reparent_to_init(void);
extern void daemonize(void);

extern int do_execve(char *, char **, char **, struct pt_regs *);
extern int do_fork(unsigned long, unsigned long, struct pt_regs *, unsigned long);

extern void FASTCALL(add_wait_queue(wait_queue_head_t *q, wait_queue_t * wait));
extern void FASTCALL(add_wait_queue_exclusive(wait_queue_head_t *q, wait_queue_t * wait));
extern void FASTCALL(remove_wait_queue(wait_queue_head_t *q, wait_queue_t * wait));

#define __wait_event(wq, condition) 					\
do {									\
	wait_queue_t __wait;						\
	init_waitqueue_entry(&__wait, current);				\
									\
	add_wait_queue(&wq, &__wait);					\
	for (;;) {							\
		set_current_state(TASK_UNINTERRUPTIBLE);		\
		if (condition)						\
			break;						\
		schedule();						\
	}								\
	current->state = TASK_RUNNING;					\
	remove_wait_queue(&wq, &__wait);				\
} while (0)

#define wait_event(wq, condition) 					\
do {									\
	if (condition)	 						\
		break;							\
	__wait_event(wq, condition);					\
} while (0)

#define __wait_event_interruptible(wq, condition, ret)			\
do {									\
	wait_queue_t __wait;						\
	init_waitqueue_entry(&__wait, current);				\
									\
	add_wait_queue(&wq, &__wait);					\
	for (;;) {							\
		set_current_state(TASK_INTERRUPTIBLE);			\
		if (condition)						\
			break;						\
		if (!signal_pending(current)) {				\
			schedule();					\
			continue;					\
		}							\
		ret = -ERESTARTSYS;					\
		break;							\
	}								\
	current->state = TASK_RUNNING;					\
	remove_wait_queue(&wq, &__wait);				\
} while (0)
	
#define wait_event_interruptible(wq, condition)				\
({									\
	int __ret = 0;							\
	if (!(condition))						\
		__wait_event_interruptible(wq, condition, __ret);	\
	__ret;								\
})

#define REMOVE_LINKS(p) do { \
	if ((p)->p_osptr) \
		(p)->p_osptr->p_ysptr = (p)->p_ysptr; \
	if ((p)->p_ysptr) \
		(p)->p_ysptr->p_osptr = (p)->p_osptr; \
	else \
		(p)->p_pptr->p_cptr = (p)->p_osptr; \
	} while (0)

#define SET_LINKS(p) do { \
	(p)->p_ysptr = NULL; \
	if (((p)->p_osptr = (p)->p_pptr->p_cptr) != NULL) \
		(p)->p_osptr->p_ysptr = p; \
	(p)->p_pptr->p_cptr = p; \
	} while (0)

extern unsigned long pcbvec[];

#define for_each_task_pre1(p) \
	{ \
	int cnt; \
	for(cnt=0;cnt<MAXPROCESSCNT;cnt++) { \
	p=pcbvec[cnt]; \
	if (p==0) continue;

#define for_each_task_post1(p) \
	} }

#define for_each_task_pre(p) \
	{ \
	unsigned long i; \
	struct _pcb *head; \
	for(i=0;i<32;i++) { \
	head=&sch$aq_comh[i]; \
	p=sch$aq_comh[i]; \
	while (head!=p) {

#define for_each_task_post(p) \
	p=p->pcb$l_sqfl; \
	} } }

#define next_thread(p) \
	list_entry((p)->thread_group.next, struct task_struct, thread_group)

/* Protects ->fs, ->files, ->mm, and synchronises with wait4().  Nests inside tasklist_lock */
static inline void task_lock(struct task_struct *p)
{
	spin_lock(&p->alloc_lock);
}

static inline void task_unlock(struct task_struct *p)
{
	spin_unlock(&p->alloc_lock);
}

/* write full pathname into buffer and return start of pathname */
static inline char * d_path(struct dentry *dentry, struct vfsmount *vfsmnt,
				char *buf, int buflen)
{
	char *res;
	struct vfsmount *rootmnt;
	struct dentry *root;
	read_lock(&current->fs->lock);
	rootmnt = mntget(current->fs->rootmnt);
	root = dget(current->fs->root);
	read_unlock(&current->fs->lock);
	spin_lock(&dcache_lock);
	res = __d_path(dentry, vfsmnt, root, rootmnt, buf, buflen);
	spin_unlock(&dcache_lock);
	dput(root);
	mntput(rootmnt);
	return res;
}

#endif /* __KERNEL__ */

#define INIT_PID 2
#define INIT_PID_REAL 1

#endif
