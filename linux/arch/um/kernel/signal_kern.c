/* 
 * Copyright (C) 2000, 2001, 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include "linux/config.h"
#include "linux/stddef.h"
#include "linux/sys.h"
#include "linux/sched.h"
#include "linux/wait.h"
#include "linux/kernel.h"
#include "linux/smp_lock.h"
#include "linux/module.h"
#include "linux/slab.h"
#include "asm/signal.h"
#include "asm/uaccess.h"
#include "user_util.h"
#include "kern_util.h"
#include "signal_kern.h"
#include "signal_user.h"
#include "kern.h"
#include "frame_kern.h"
#include "frame.h"
#include "sigcontext.h"

EXPORT_SYMBOL(block_signals);
EXPORT_SYMBOL(unblock_signals);

int probe_stack(unsigned long sp, int delta)
{
	int n;

	if((get_user(n, (int *) sp) != 0) ||
	   (put_user(n, (int *) sp) != 0) ||
	   (get_user(n, (int *) (sp - delta)) != 0) ||
	   (put_user(n, (int *) (sp - delta)) != 0))
		return(-EFAULT);
	return(0);
}

static void force_segv(int sig)
{
	if(sig == SIGSEGV){
		struct k_sigaction *ka;

		ka = &current->sig->action[SIGSEGV - 1];
		ka->sa.sa_handler = SIG_DFL;
	}
	force_sig(SIGSEGV, current);
}

#define _S(nr) (1<<((nr)-1))

#define _BLOCKABLE (~(_S(SIGKILL) | _S(SIGSTOP)))

/*
 * OK, we're invoking a handler
 */	
static int handle_signal(struct pt_regs *regs, unsigned long signr, 
			 struct k_sigaction *ka, siginfo_t *info, 
			 sigset_t *oldset, int error)
{
	struct signal_context *context;
        __sighandler_t handler;
	void (*restorer)(void);
	unsigned long sp;
	sigset_t save;
	int err, ret;

	ret = 0;
	switch(error){
	case -ERESTARTNOHAND:
		ret = -EINTR;
		break;

	case -ERESTARTSYS:
		if (!(ka->sa.sa_flags & SA_RESTART)) {
			ret = -EINTR;
			break;
		}
		/* fallthrough */
	case -ERESTARTNOINTR:
		PT_REGS_RESTART_SYSCALL(regs);
		PT_REGS_ORIG_SYSCALL(regs) = PT_REGS_SYSCALL_NR(regs);

		/* This is because of the UM_SET_SYSCALL_RETURN and the fact
		 * that on i386 the system call number and return value are
		 * in the same register.  When the system call restarts, %eax
		 * had better have the system call number in it.  Since the
		 * return value doesn't matter (except that it shouldn't be
		 * -ERESTART*), we'll stick the system call number there.
		 */
		ret = PT_REGS_SYSCALL_NR(regs);
		break;
	}

	handler = ka->sa.sa_handler;
	save = *oldset;

	if (ka->sa.sa_flags & SA_ONESHOT)
		ka->sa.sa_handler = SIG_DFL;

	if (!(ka->sa.sa_flags & SA_NODEFER)) {
		spin_lock_irq(&current->sigmask_lock);
		sigorsets(&current->blocked, &current->blocked, 
			  &ka->sa.sa_mask);
		sigaddset(&current->blocked, signr);
		recalc_sigpending(current);
		spin_unlock_irq(&current->sigmask_lock);
	}

	sp = PT_REGS_SP(regs);

	if((ka->sa.sa_flags & SA_ONSTACK) && (sas_ss_flags(sp) == 0))
		sp = current->sas_ss_sp + current->sas_ss_size;
	
	sp -= 4 * sizeof(void *) + sizeof(*context);
	context = (struct signal_context *) sp;

	if(error != 0) PT_REGS_SET_SYSCALL_RETURN(regs, ret);

	if(copy_to_user(&context->sigs, &save, sizeof(save)) ||
	   copy_to_user(&context->prev, &current->thread.signal_context,
			sizeof(current->thread.signal_context))){
		force_segv(signr);
		return(1);
	}
	
	current->thread.signal_context = context;
	sp -= 4 * sizeof(void *);

	if (ka->sa.sa_flags & SA_RESTORER) restorer = ka->sa.sa_restorer;
	else restorer = NULL;

	if(ka->sa.sa_flags & SA_SIGINFO)
		err = setup_signal_stack_si(sp, signr, (unsigned long) handler,
					    restorer, regs,
					    &context->sc, info);
	else 
		err = setup_signal_stack_sc(sp, signr, (unsigned long) handler,
					    restorer, regs, &context->sc);
	if(err) goto segv;

	return(0);
 segv:
	force_segv(signr);
	return(1);
}

/*
 * Note that 'init' is a special process: it doesn't get signals it doesn't
 * want to handle. Thus you cannot kill init even with a SIGKILL even by
 * mistake.
 */

static int kern_do_signal(struct pt_regs *regs, sigset_t *oldset, int error)
{
	siginfo_t info;
	struct k_sigaction *ka;
	int err;

	if (!oldset)
		oldset = &current->blocked;

	for (;;) {
		unsigned long signr;

		spin_lock_irq(&current->sigmask_lock);
		signr = dequeue_signal(&current->blocked, &info);
		spin_unlock_irq(&current->sigmask_lock);

		if (!signr)
			break;

		if ((current->ptrace & PT_PTRACED) && signr != SIGKILL) {
			/* Let the debugger run.  */
			current->exit_code = signr;
			current->state = TASK_STOPPED;
			notify_parent(current, SIGCHLD);
			schedule();

			/* We're back.  Did the debugger cancel the sig?  */
			if (!(signr = current->exit_code))
				continue;
			current->exit_code = 0;

			/* The debugger continued.  Ignore SIGSTOP.  */
			if (signr == SIGSTOP)
				continue;

			/* Update the siginfo structure.  Is this good?  */
			if (signr != info.si_signo) {
				info.si_signo = signr;
				info.si_errno = 0;
				info.si_code = SI_USER;
				info.si_pid = current->p_pptr->pcb$l_pid;
				info.si_uid = current->p_pptr->uid;
			}

			/* If the (new) signal is now blocked, requeue it.  */
			if (sigismember(&current->blocked, signr)) {
				send_sig_info(signr, &info, current);
				continue;
			}
		}

		ka = &current->sig->action[signr-1];
		if (ka->sa.sa_handler == SIG_IGN) {
			if (signr != SIGCHLD)
				continue;
			/* Check for SIGCHLD: it's special.  */
			while (sys_wait4(-1, NULL, WNOHANG, NULL) > 0)
				/* nothing */;
			continue;
		}

		if (ka->sa.sa_handler == SIG_DFL) {
			int exit_code = signr;

			/* Init gets no signals it doesn't want.  */
			if (current->pcb$l_pid == INIT_PID)
				continue;

			switch (signr) {
			case SIGCONT: case SIGCHLD: case SIGWINCH:
				continue;

			case SIGTSTP: case SIGTTIN: case SIGTTOU:
				if (is_orphaned_pgrp(current->pgrp))
					continue;
				/* FALLTHRU */

                        case SIGSTOP: {
                                struct signal_struct *sig;
				current->state = TASK_STOPPED;
				current->exit_code = signr;
                                sig = current->p_pptr->sig;
                                if (sig && !(sig->action[SIGCHLD-1].sa.sa_flags & SA_NOCLDSTOP))
					notify_parent(current, SIGCHLD);
				schedule();
				continue;
			}
			case SIGQUIT: case SIGILL: case SIGTRAP:
			case SIGABRT: case SIGFPE: case SIGSEGV:
			case SIGBUS: case SIGSYS: case SIGXCPU: case SIGXFSZ:
				if (do_coredump(signr, &current->thread.regs))
					exit_code |= 0x80;
				/* FALLTHRU */

			default:
				sigaddset(&current->pending.signal, signr);
				recalc_sigpending(current);
				current->flags |= PF_SIGNALED;
				do_exit(exit_code);
				/* NOTREACHED */
			}
		}

		/* Whee!  Actually deliver the signal.  */
		err = handle_signal(regs, signr, ka, &info, oldset, error);
		if(!err) return(1);
	}

	/* Did we come from a system call? */
	if(PT_REGS_SYSCALL_NR(regs) >= 0){
		/* Restart the system call - no handlers present */
		if(PT_REGS_SYSCALL_RET(regs) == -ERESTARTNOHAND ||
		   PT_REGS_SYSCALL_RET(regs) == -ERESTARTSYS ||
		   PT_REGS_SYSCALL_RET(regs) == -ERESTARTNOINTR){
			PT_REGS_ORIG_SYSCALL(regs) = PT_REGS_SYSCALL_NR(regs);
			PT_REGS_RESTART_SYSCALL(regs);
		}
	}
	return(0);
}

int do_signal(int error)
{
	return(kern_do_signal(&current->thread.regs, NULL, error));
}

/*
 * Atomically swap in the new signal mask, and wait for a signal.
 */
int sys_sigsuspend(int history0, int history1, old_sigset_t mask)
{
	sigset_t saveset;

	mask &= _BLOCKABLE;
	spin_lock_irq(&current->sigmask_lock);
	saveset = current->blocked;
	siginitset(&current->blocked, mask);
	recalc_sigpending(current);
	spin_unlock_irq(&current->sigmask_lock);

	while (1) {
		current->state = TASK_INTERRUPTIBLE;
		schedule();
		if(kern_do_signal(&current->thread.regs, &saveset, -EINTR))
			return(-EINTR);
	}
}

int sys_rt_sigsuspend(sigset_t *unewset, size_t sigsetsize)
{
	sigset_t saveset, newset;

	/* XXX: Don't preclude handling different sized sigset_t's.  */
	if (sigsetsize != sizeof(sigset_t))
		return -EINVAL;

	if (copy_from_user(&newset, unewset, sizeof(newset)))
		return -EFAULT;
	sigdelsetmask(&newset, ~_BLOCKABLE);

	spin_lock_irq(&current->sigmask_lock);
	saveset = current->blocked;
	current->blocked = newset;
	recalc_sigpending(current);
	spin_unlock_irq(&current->sigmask_lock);

	while (1) {
		current->state = TASK_INTERRUPTIBLE;
		schedule();
		if (kern_do_signal(&current->thread.regs, &saveset, -EINTR))
			return(-EINTR);
	}
}

int sys_sigreturn(struct pt_regs regs)
{
	struct signal_context *context = current->thread.signal_context;

	sigdelsetmask(&context->sigs, ~_BLOCKABLE);
	spin_lock_irq(&current->sigmask_lock);
	current->blocked = context->sigs;
	recalc_sigpending(current);
	spin_unlock_irq(&current->sigmask_lock);
	current->thread.signal_context = context->prev;
	if(context->sc != NULL){
		copy_sc_from_user(current->thread.regs.regs.sc, context->sc,
				  &signal_frame_sc.arch);
	}
	return(PT_REGS_SYSCALL_RET(&current->thread.regs));
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
