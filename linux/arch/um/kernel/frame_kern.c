/* 
 * Copyright (C) 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include "asm/ptrace.h"
#include "asm/uaccess.h"
#include "frame.h"
#include "frame_kern.h"
#include "sigcontext.h"
#include "sysdep/ptrace.h"

int copy_siginfo_to_user(siginfo_t *to, siginfo_t *from)
{
	if (!access_ok (VERIFY_WRITE, to, sizeof(siginfo_t)))
		return -EFAULT;
	if (from->si_code < 0)
		return __copy_to_user(to, from, sizeof(siginfo_t));
	else {
		int err;

		/* If you change siginfo_t structure, please be sure
		   this code is fixed accordingly.
		   It should never copy any pad contained in the structure
		   to avoid security leaks, but must copy the generic
		   3 ints plus the relevant union member.  */
		err = __put_user(from->si_signo, &to->si_signo);
		err |= __put_user(from->si_errno, &to->si_errno);
		err |= __put_user((short)from->si_code, &to->si_code);
		/* First 32bits of unions are always present.  */
		err |= __put_user(from->si_pid, &to->si_pid);
		switch (from->si_code >> 16) {
		case __SI_FAULT >> 16:
			break;
		case __SI_CHLD >> 16:
			err |= __put_user(from->si_utime, &to->si_utime);
			err |= __put_user(from->si_stime, &to->si_stime);
			err |= __put_user(from->si_status, &to->si_status);
		default:
			err |= __put_user(from->si_uid, &to->si_uid);
			break;
		}
		return err;
	}
}

static int copy_restorer(void (*restorer)(void), unsigned long start, 
			 unsigned long sr_index, int sr_relative)
{
	if(restorer != 0){
		if(copy_to_user((void *) (start + sr_index), &restorer, 
				sizeof(restorer)))
			return(1);
	}
	else if(sr_relative){
		unsigned long *sr = (unsigned long *) (start + sr_index);
		*sr += (unsigned long) sr;
	}
	return(0);
}

int setup_signal_stack_si(unsigned long stack_top, int sig, 
			  unsigned long handler, void (*restorer)(void), 
			  struct pt_regs *regs, void *context_sc, 
			  siginfo_t *info)
{
	unsigned long start, sc;
	void *sip;

	start = stack_top - signal_frame_si.len - 
		sc_size(&signal_frame_sc.arch);
	sip = (void *) (start + signal_frame_si.si_index);
	sc = start + signal_frame_si.len;
	if(copy_sc_to_user((void *) sc, regs->regs.sc, 
			   &signal_frame_sc.arch) ||
	   copy_to_user(context_sc, (void *) sc, sizeof(context_sc)) ||
	   copy_to_user((void *) start, signal_frame_si.data,
			signal_frame_si.len) ||
	   copy_to_user((void *) (start + signal_frame_si.sig_index), &sig, 
				 sizeof(sig)) ||
	   copy_siginfo_to_user(sip, info) ||
	   copy_to_user((void *) (start + signal_frame_si.sip_index), &sip,
			sizeof(sip)) ||
	   copy_restorer(restorer, start, signal_frame_si.sr_index,
			 signal_frame_si.sr_relative))
		return(1);
	
	PT_REGS_IP(regs) = handler;
	PT_REGS_SP(regs) = start + signal_frame_sc.sp_index;
	return(0);
}

int setup_signal_stack_sc(unsigned long stack_top, int sig, 
			  unsigned long handler, void (*restorer)(void), 
			  struct pt_regs *regs, void *context_sc)
{
	unsigned long start = stack_top - signal_frame_sc.len;
	void *user_sc = (void *) (start + signal_frame_sc.sc_index);
	
	if(copy_to_user((void *) start, signal_frame_sc.data, 
			signal_frame_sc.len) ||
	   copy_to_user((void *) (start + signal_frame_sc.sig_index), &sig,
			sizeof(sig)) ||
	   copy_sc_to_user(user_sc, regs->regs.sc, &signal_frame_sc.arch) ||
	   copy_to_user(context_sc, &user_sc, sizeof(user_sc)) ||
	   copy_restorer(restorer, start, signal_frame_sc.sr_index,
			 signal_frame_sc.sr_relative))
		return(1);

	PT_REGS_IP(regs) = handler;
	PT_REGS_SP(regs) = start + signal_frame_sc.sp_index;

	set_sc_ip_sp(regs->regs.sc, handler, start + signal_frame_sc.sp_index);
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
