/* 
 * Copyright (C) 2000, 2001, 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include "linux/config.h"
#include "linux/kernel.h"
#include "linux/sched.h"
#include "linux/interrupt.h"
#include "linux/mm.h"
#include "linux/slab.h"
#include "linux/utsname.h"
#include "linux/fs.h"
#include "linux/utime.h"
#include "linux/smp_lock.h"
#include "linux/module.h"
#include "linux/init.h"
#include "linux/capability.h"
#include "asm/unistd.h"
#include "asm/mman.h"
#include "asm/segment.h"
#include "asm/stat.h"
#include "asm/pgtable.h"
#include "asm/processor.h"
#include "asm/pgalloc.h"
#include "asm/spinlock.h"
#include "asm/uaccess.h"
#include "asm/user.h"
#include "user_util.h"
#include "kern_util.h"
#include "kern.h"
#include "signal_kern.h"
#include "signal_user.h"
#include "init.h"
#include "irq_user.h"
#include "mem_user.h"
#include "tlb.h"
#include "frame.h"
#include "sigcontext.h"
#include "2_5compat.h"

static inline struct task_struct * get_cur_task(void)
{
        struct task_struct *cur_task;
        __asm__("andl %%esp,%0; ":"=r" (cur_task) : "0" (~8191UL));
        return cur_task;
}
 
//#define cur_task get_cur_task()
#define cur_task ({ int dummy; (struct task_struct *) CURRENT_TASK(dummy); })

struct cpu_task cpu_tasks[NR_CPUS] = { [0 ... NR_CPUS - 1] = { -1, NULL } };

int external_pid(void *t)
{
	struct task_struct *task = t ? t : current;

	return(task->thread.extern_pid);
}

int pid_to_processor_id(int pid)
{
	int i;

	for(i = 0; i < smp_num_cpus; i++){
		if(cpu_tasks[i].pid == pid) return(i);
	}
	return(-1);
}

void free_stack(unsigned long stack)
{
	free_page(stack);
}

void set_init_pid(int pid)
{
	init_task.thread.extern_pid = pid;
	if(pipe(init_task.thread.switch_pipe) < 0)
		panic("Can't create switch pipe for init_task");
}

int set_user_mode(void *t, int protect_mem)
{
	struct task_struct *task;

	task = t ? t : current;
	if(task->thread.tracing) return(1);
	task->thread.request.op = OP_TRACE_ON;
	if(protect_mem) protect_kernel_mem(1);
	usr1_pid(getpid());
	return(0);
}

void set_tracing(void *task, int tracing)
{
	((struct task_struct *) task)->thread.tracing = tracing;
}

int is_tracing(void *t)
{
	return (((struct task_struct *) t)->thread.tracing);
}

unsigned long alloc_stack(void)
{
	unsigned long page;

	if((page = __get_free_page(GFP_KERNEL)) == 0)
		return(0);
	stack_protections(page);
	return(page);
}

extern void schedule_tail(struct task_struct *prev);

static void new_thread_handler(int sig)
{
	int (*fn)(void *);
	void *arg;

	fn = current->thread.request.u.thread.proc;
	arg = current->thread.request.u.thread.arg;
	suspend_new_thread(current->thread.switch_pipe[0]);

	free_page(current->thread.temp_stack);
	set_cmdline("(kernel thread)");
	force_flush_all();
	if(current->thread.prev_sched != NULL)
		schedule_tail(current->thread.prev_sched);
	current->thread.prev_sched = NULL;
	current->thread.regs.regs.sc = (void *) (&sig + 1);
	change_sig(SIGUSR1, 1);
	unblock_signals();
	if(!run_kernel_thread(fn, arg, &current->thread.jmp))
		do_exit(0);
}

static int new_thread_proc(void *stack)
{
	block_signals();
	init_new_thread(stack, new_thread_handler);
	usr1_pid(getpid());
}

int kernel_thread(int (*fn)(void *), void * arg, unsigned long flags)
{
	int pid;

	current->thread.request.u.thread.proc = fn;
	current->thread.request.u.thread.arg = arg;
	pid = do_fork(CLONE_VM | flags, 0, NULL, 0);
	if(pid < 0) panic("do_fork failed in kernel_thread");
	return(pid);
}

void switch_mm(struct mm_struct *prev, struct mm_struct *next, 
	       struct task_struct *tsk, unsigned cpu)
{
	if (prev != next) 
		clear_bit(cpu, &prev->cpu_vm_mask);
	set_bit(cpu, &next->cpu_vm_mask);
}

void set_current(void *t)
{
	// the original bugs with vms mm?
	struct task_struct *task = t;
	int cpu=CPU(task);
	cpu_tasks[cpu].pid= task->thread.extern_pid;
	cpu_tasks[cpu].task= task;
//        cpu_tasks[CPU(task)] = ((struct cpu_task) 
//              { task->thread.extern_pid, task });

}

void *_switch_to(void *prev, void *next)
{
	struct task_struct *from, *to;
	int vtalrm, alrm, err;
	char c;

	from = prev;
	to = next;

	to->thread.prev_sched = from;

	if(CPU(from) == 0) forward_interrupts(to->thread.extern_pid);
	forward_ipi(cpu_data[CPU(from)].ipi_pipe[0], to->thread.extern_pid);
	block_signals();

	vtalrm = change_sig(SIGVTALRM, 0);
	alrm = change_sig(SIGALRM, 0);

	c = 0;
	set_current(to);
//	printk("user_write %x\n",to->thread.switch_pipe[1]);
	err = user_write(to->thread.switch_pipe[1], &c, sizeof(c));
//	printk("user_write %x\n",to->thread.switch_pipe[1]);
	if(err != sizeof(c))
		panic("write of switch_pipe failed, errno = %d", -err);

	if(from->state == TASK_ZOMBIE) kill_pid(getpid());
//	printk("user_read %x\n",to->thread.switch_pipe[0]);
	err = user_read(from->thread.switch_pipe[0], &c, sizeof(c));
//	printk("user_read %x\n",to->thread.switch_pipe[0]);
	if(err != sizeof(c))
		panic("read of switch_pipe failed, errno = %d", -err);

	change_sig(SIGVTALRM, vtalrm);
	change_sig(SIGALRM, alrm);

	flush_tlb_all();
	unblock_signals();

	return(cur_task->thread.prev_sched);
}

void ret_from_sys_call(void)
{
	if(current->need_resched) schedule();
	if(current->sigpending != 0) do_signal(0);
}

void release_thread(struct task_struct *task)
{
	close(task->thread.switch_pipe[0]);
	close(task->thread.switch_pipe[1]);
	kill_pid(task->thread.extern_pid);
}

void exit_thread(void)
{
	unprotect_stack((unsigned long) current);
}

void finish_fork_handler(int sig)
{
	current->thread.regs.regs.sc = (void *) (&sig + 1);
	suspend_new_thread(current->thread.switch_pipe[0]);
	
	force_flush_all();
	if(current->mm != current->p_pptr->mm)
		protect(uml_physmem, high_physmem - uml_physmem, 1, 1, 0, 1);
	task_protections((unsigned long) current);
	if(current->thread.prev_sched != NULL)
		schedule_tail(current->thread.prev_sched);
	current->thread.prev_sched = NULL;

	free_page(current->thread.temp_stack);
	set_user_mode(current, 1);
}

void *get_current(void)
{
	return(current);
}

/* This sigusr1 business works around a bug in gcc's -pg support.  
 * Normally a procedure's mcount call comes after esp has been copied to 
 * ebp and the new frame is constructed.  With procedures with no locals,
 * the mcount comes before, as the first thing that the procedure does.
 * When that procedure is main for a thread, ebp comes in as NULL.  So,
 * when mcount dereferences it, it segfaults.  So, UML works around this
 * by adding a non-optimizable local to the various trampolines, fork_tramp
 * and outer_tramp below, and exec_tramp.
 */

static int sigusr1 = SIGUSR1;

int fork_tramp(void *stack)
{
	int sig = sigusr1;

	block_signals();
	init_new_thread(stack, finish_fork_handler);

	kill(getpid(), sig);
	return(0);
}

int copy_thread(int nr, unsigned long clone_flags, unsigned long sp,
		unsigned long stack_top, struct task_struct * p, 
		struct pt_regs *regs)
{
	int new_pid;
	unsigned long stack;
	int (*tramp)(void *);

	p->thread = (struct thread_struct) INIT_THREAD;
	p->thread.kernel_stack = (unsigned long) p + 2 * PAGE_SIZE;

	if(current->thread.forking)
		tramp = fork_tramp;
	else {
		tramp = new_thread_proc;
		p->thread.request.u.thread = current->thread.request.u.thread;
	}

	if(pipe(p->thread.switch_pipe) < 0)
		panic("copy_thread : pipe failed");

	stack = alloc_stack();
	if(stack == 0){
		printk(KERN_ERR "copy_thread : failed to allocate "
		       "temporary stack\n");
		return(-ENOMEM);
	}

	clone_flags &= CLONE_VM;
	p->thread.temp_stack = stack;
	new_pid = start_fork_tramp((void *) p->thread.kernel_stack, stack,
				   clone_flags, tramp);
	if(new_pid < 0){
		printk(KERN_ERR "copy_thread : clone failed - errno = %d\n", 
		       -new_pid);
		return(new_pid);
	}

	if(current->thread.forking){
		sc_to_sc(p->thread.regs.regs.sc, current->thread.regs.regs.sc);
		PT_REGS_SET_SYSCALL_RETURN(&p->thread.regs, 0);
		if(sp != 0) PT_REGS_SP(&p->thread.regs) = sp;
	}
	else {
		p->mm = NULL;
		p->active_mm = NULL;
	}
	p->thread.extern_pid = new_pid;

	current->thread.request.op = OP_FORK;
	current->thread.request.u.fork.pid = new_pid;
	usr1_pid(getpid());
	return(0);
}

void tracing_reboot(void)
{
	current->thread.request.op = OP_REBOOT;
	usr1_pid(getpid());
}

void tracing_halt(void)
{
	current->thread.request.op = OP_HALT;
	usr1_pid(getpid());
}

void tracing_cb(void (*proc)(void *), void *arg)
{
	if(getpid() == tracing_pid){
		(*proc)(arg);
	}
	else {
		current->thread.request.op = OP_CB;
		current->thread.request.u.cb.proc = proc;
		current->thread.request.u.cb.arg = arg;
		usr1_pid(getpid());
	}
}

int do_proc_op(void *t, int proc_id)
{
	struct task_struct *task;
	struct thread_struct *thread;
	int op, pid;

	task = t;
	thread = &task->thread;
	op = thread->request.op;
	switch(op){
	case OP_NONE:
	case OP_TRACE_ON:
		break;
	case OP_EXEC:
		pid = thread->request.u.exec.pid;
		do_exec(thread->extern_pid, pid);
		thread->extern_pid = pid;
		cpu_tasks[CPU(task)].pid = pid;
		break;
	case OP_FORK:
		attach_process(thread->request.u.fork.pid);
		break;
	case OP_CB:
		(*thread->request.u.cb.proc)(thread->request.u.cb.arg);
		break;
	case OP_REBOOT:
	case OP_HALT:
		break;
	default:
		tracer_panic("Bad op in do_proc_op");
		break;
	}
	thread->request.op = OP_NONE;
	return(op);
}

unsigned long stack_sp(unsigned long page)
{
	return(page + PAGE_SIZE - sizeof(void *));
}

int current_pid(void)
{
	return(current->pid);
}

extern int done_init_idle;
int in_idle_while;
unsigned long round_and_round;
int hwclkdone=0;

void cpu_idle(void)
{
	init_idle();
	printk("id %x\n",current->pid);	
	printk("idle %x %x %x\n",done_init_idle,current,&init_task);
  printk("pid 0 here again%x %x\n",init_task.pcb$l_astqfl,&init_task.pcb$l_astqfl); 
	{ int i; for(i=0;i<10000000;i++) ; }
	if (current->pid==0) { /* just to be sure */
	  	  current->pcb$b_prib  = 24;
	  	  current->pcb$b_pri   = 24;
	  current->pcb$b_prib  = 31;
	  current->pcb$b_pri   = 31;
	  current->pcb$w_quant = 0;
	} /* we might not need these settings */

 	if(CPU(current) == 0) idle_timer();

	atomic_inc(&init_mm.mm_count);
	current->mm = &init_mm;
	current->active_mm = &init_mm;

	printk("bef while\n");
	while(1){
		/* endless idle loop with no priority at all */
		SET_PRI(current);

		/*
		 * although we are an idle CPU, we do not want to
		 * get into the scheduler unnecessarily.
		 */
		if (!hwclkdone) schedule();
		if (current->need_resched) {
			schedule();
			check_pgt_cache();
		}
		idle_sleep(10);
	}
	printk("aft while\n");
}

int page_size(void)
{
	return(PAGE_SIZE);
}

int page_mask(void)
{
	return(PAGE_MASK);
}

unsigned long um_virt_to_phys(void *t, unsigned long addr)
{
	struct task_struct *task;
	pgd_t *pgd;
	pmd_t *pmd;
	pte_t *pte;

	task = t;
	if(task->mm == NULL) return(0xffffffff);
	pgd = pgd_offset(task->mm, addr);
	pmd = pmd_offset(pgd, addr);
	if(!pmd_present(*pmd)) return(0xffffffff);
	pte = pte_offset(pmd, addr);
	if(!pte_present(*pte)) return(0xffffffff);
	return((pte_val(*pte) & PAGE_MASK) + (addr & ~PAGE_MASK));
}

char *current_cmd(void)
{
#ifdef CONFIG_SMP
	return("(Unknown)");
#else
	unsigned long addr;

	if((addr = um_virt_to_phys(current, 
				   current->mm->arg_start)) == 0xffffffff) 
		return("(Unknown)");
	else return((char *) addr);
#endif
}

void force_sigbus(void)
{
	printk(KERN_ERR "Killing pid %d because of a lack of memory\n", 
	       current->pid);
	lock_kernel();
	sigaddset(&current->pending.signal, SIGBUS);
	recalc_sigpending(current);
	current->flags |= PF_SIGNALED;
	do_exit(SIGBUS | 0x80);
}

void dump_thread(struct pt_regs *regs, struct user *u)
{
}

void enable_hlt(void)
{
	panic("enable_hlt");
}

void disable_hlt(void)
{
	panic("disable_hlt");
}

extern int signal_frame_size;

void interrupt_end(void)
{
	if(current->need_resched) schedule();
	do_signal(0);
}

void *um_kmalloc(int size)
{
	return(kmalloc(size, GFP_KERNEL));
}

unsigned long get_fault_addr(void)
{
	return((unsigned long) current->thread.fault_addr);
}

EXPORT_SYMBOL(get_fault_addr);

int singlestepping(void *t)
{
	struct task_struct *task;
	int ret;

	task = (struct task_struct *) t;
	ret = (task->ptrace & PT_DTRACE);
	task->ptrace &= ~PT_DTRACE;
	return(ret);
}

void not_implemented(void)
{
	printk(KERN_DEBUG "Something isn't implemented in here\n");
}

EXPORT_SYMBOL(not_implemented);

int user_context(unsigned long sp)
{
	return((sp & (PAGE_MASK << 1)) != current->thread.kernel_stack);
}

extern void remove_umid_dir(void);
__uml_exitcall(remove_umid_dir);

extern exitcall_t __uml_exitcall_begin, __uml_exitcall_end;

void do_uml_exitcalls(void)
{
	exitcall_t *call;

	call = &__uml_exitcall_end;
	while (--call >= &__uml_exitcall_begin)
		(*call)();
}

extern exitcall_t __exitcall_begin, __exitcall_end;

void do_exitcalls(void)
{
	exitcall_t *call;

	call = &__exitcall_end;
	while (--call >= &__exitcall_begin)
		(*call)();
	do_uml_exitcalls();
}

void *round_up(unsigned long addr)
{
	return(ROUND_UP(addr));
}

void *round_down(unsigned long addr)
{
	return(ROUND_DOWN(addr));
}

char *uml_strdup(char *string)
{
	char *new;

	new = kmalloc(strlen(string) + 1, GFP_KERNEL);
	if(new == NULL) return(NULL);
	strcpy(new, string);
	return(new);
}

int jail = 0;

int __init jail_setup(char *line, int *add)
{
	int ok = 1;

	if(jail) return(0);
#ifdef CONFIG_SMP
	printf("'jail' may not used used in a kernel with CONFIG_SMP "
	       "enabled\n");
	ok = 0;
#endif
#ifdef CONFIG_HOSTFS
	printf("'jail' may not used used in a kernel with CONFIG_HOSTFS "
	       "enabled\n");
	ok = 0;
#endif
#ifdef CONFIG_MODULES
	printf("'jail' may not used used in a kernel with CONFIG_MODULES "
	       "enabled\n");
	ok = 0;
#endif	
	if(!ok) exit(1);

	/* CAP_SYS_RAWIO controls the ability to open /dev/mem and /dev/kmem.
	 * Removing it from the bounding set eliminates the ability of anything
	 * to acquire it, and thus read or write kernel memory.
	 */
	cap_lower(cap_bset, CAP_SYS_RAWIO);
	jail = 1;
	return(0);
}

__uml_setup("jail", jail_setup,
"jail\n"
"    Enables the protection of kernel memory from processes.\n\n"
);

static void mprotect_kernel_mem(int w, int delay_signals)
{
	unsigned long start, end, flags = 0;
	int alrm = 0, vtalrm = 0;

	if(!jail || (current == &init_task)) return;

	if(delay_signals){
		local_irq_save(flags);
		alrm = change_sig(SIGALRM, 0);
		vtalrm = change_sig(SIGVTALRM, 0);
	}

	start = (unsigned long) current + PAGE_SIZE;
	end = (unsigned long) current + PAGE_SIZE * 4;
	protect(uml_physmem, start - uml_physmem, 1, w, 1, 1);
	protect(end, high_physmem - end, 1, w, 1, 1);

	start = (unsigned long) ROUND_DOWN(&_stext);
	end = (unsigned long) ROUND_UP(&_etext);
	protect(start, end - start, 1, w, 1, 1);

	start = (unsigned long) ROUND_DOWN(&_unprotected_end);
	end = (unsigned long) ROUND_UP(&_edata);
	protect(start, end - start, 1, w, 1, 1);

	start = (unsigned long) ROUND_DOWN(&__bss_start);
	end = (unsigned long) ROUND_UP(brk_start);
	protect(start, end - start, 1, w, 1, 1);

	mprotect_kernel_vm(w);

	if(delay_signals){
		local_irq_restore(flags);
		change_sig(SIGALRM, alrm);
		change_sig(SIGVTALRM, vtalrm);
	}
}

void unprotect_kernel_mem(int delay_signals)
{
	mprotect_kernel_mem(1, delay_signals);
}

void protect_kernel_mem(int delay_signals)
{
	mprotect_kernel_mem(0, delay_signals);
}

void *get_init_task(void)
{
	return(&init_task_union.task);
}

int copy_to_user_proc(void *to, void *from, int size)
{
	return(copy_to_user(to, from, size));
}

int copy_from_user_proc(void *to, void *from, int size)
{
	return(copy_from_user(to, from, size));
}

void set_thread_sc(void *sc)
{
	current->thread.regs.regs.sc = sc;
}

int smp_sigio_handler(void)
{
#ifdef CONFIG_SMP
	IPI_handler(hard_smp_processor_id());
	if (hard_smp_processor_id() != 0) return(1);
#endif
	return(0);
}

/*
 * Overrides for Emacs so that we follow Linus's tabbing style.
 * Emacs will notice this stuff at the end of the file and automatically
 * adjust the settings for this buffer only.  This must remain at the end
 * of the file.
 * ---------------------------------------------------------------------------
 * Local variables:
 * c-file-style: "linux"
 * End:
 */
