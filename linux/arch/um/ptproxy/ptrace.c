/**********************************************************************
ptrace.c

Copyright (C) 1999 Lars Brinkhoff.  See the file COPYING for licensing
terms and conditions.

Jeff Dike (jdike@karaya.com) : Modified for integration into uml
**********************************************************************/

#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <asm/ptrace.h>

#include "ptproxy.h"
#include "debug.h"
#include "user_util.h"
#include "ptrace_user.h"

long proxy_ptrace(struct debugger *debugger, int arg1, pid_t arg2,
		  long arg3, long arg4, pid_t child, int *ret)
{
	long result;
	int status;

	*ret = 0;
	if(debugger->debugee->died) return(-ESRCH);

	switch(arg1){
	case PTRACE_ATTACH:
		if(debugger->debugee->traced) return(-EPERM);

		debugger->debugee->pid = arg2;
		debugger->debugee->traced = 1;
		if(debugger->debugee->stopped) 
			child_proxy(child, W_STOPCODE(SIGSTOP));
		else kill(child, SIGSTOP);
		return(0);

	case PTRACE_CONT:
		*ret = PTRACE_CONT;
		return(ptrace(PTRACE_CONT, child, arg3, arg4));

	case PTRACE_DETACH:
		if(!debugger->debugee->traced) return(-EPERM);
		
		debugger->debugee->traced = 0;
		kill(child, SIGCONT);
		return(0);

#ifdef UM_HAVE_GETFPREGS
	case PTRACE_GETFPREGS:
	{
		long regs[FP_FRAME_SIZE];
		int i, result;

		result = ptrace(PTRACE_GETFPREGS, child, 0, regs);
		if(result == -1) return(-errno);
		
		for (i = 0; i < sizeof(regs)/sizeof(regs[0]); i++)
			ptrace(PTRACE_POKEDATA, debugger->pid, arg4 + 4 * i,
			       regs[i]);
		return(result);
	}
#endif

#ifdef UM_HAVE_GETFPXREGS
	case PTRACE_GETFPXREGS:
	{
		long regs[FPX_FRAME_SIZE];
		int i, result;

		result = ptrace(PTRACE_GETFPXREGS, child, 0, regs);
		if(result == -1) return(-errno);
		
		for (i = 0; i < sizeof(regs)/sizeof(regs[0]); i++)
			ptrace(PTRACE_POKEDATA, debugger->pid, arg4 + 4 * i,
			       regs[i]);
		return(result);
	}
#endif

#ifdef UM_HAVE_GETREGS
	case PTRACE_GETREGS:
	{
		long regs[FRAME_SIZE];
		int i, result;

		result = ptrace(PTRACE_GETREGS, child, 0, regs);
		if(result == -1) return(-errno);

		for (i = 0; i < sizeof(regs)/sizeof(regs[0]); i++)
			ptrace (PTRACE_POKEDATA, debugger->pid,
				arg4 + 4 * i, regs[i]);
		return(result);
	}
	break;
#endif

	case PTRACE_KILL:
		result = ptrace(PTRACE_KILL, child, arg3, arg4);
		if(result == -1) return(-errno);

		return(result);

	case PTRACE_PEEKDATA:
	case PTRACE_PEEKTEXT:
	case PTRACE_PEEKUSER:
		/* The value being read out could be -1, so we have to 
		 * check errno to see if there's an error, and zero it
		 * beforehand so we're not faked out by an old error
		 */

		errno = 0;
		result = ptrace(arg1, child, arg3, 0);
		if((result == -1) && (errno != 0)) return(-errno);

		result = ptrace(PTRACE_POKEDATA, debugger->pid, arg4, result);
		if(result == -1) return(-errno);
			
		return(result);

	case PTRACE_POKEDATA:
	case PTRACE_POKETEXT:
	case PTRACE_POKEUSER:
		result = ptrace(arg1, child, arg3, arg4);
		if(result == -1) return(-errno);

		return(result);

#ifdef UM_HAVE_SETFPREGS
	case PTRACE_SETFPREGS:
	{
		long regs[FP_FRAME_SIZE];
		int i;

		for (i = 0; i < sizeof(regs)/sizeof(regs[0]); i++)
			regs[i] = ptrace (PTRACE_PEEKDATA, debugger->pid,
					  arg4 + 4 * i, 0);
		result = ptrace(PTRACE_SETFPREGS, child, 0, regs);
		if(result == -1) return(-errno);

		return(result);
	}
#endif

#ifdef UM_HAVE_SETFPXREGS
	case PTRACE_SETFPXREGS:
	{
		long regs[FPX_FRAME_SIZE];
		int i;

		for (i = 0; i < sizeof(regs)/sizeof(regs[0]); i++)
			regs[i] = ptrace (PTRACE_PEEKDATA, debugger->pid,
					  arg4 + 4 * i, 0);
		result = ptrace(PTRACE_SETFPXREGS, child, 0, regs);
		if(result == -1) return(-errno);

		return(result);
	}
#endif

#ifdef UM_HAVE_SETREGS
	case PTRACE_SETREGS:
	{
		long regs[FRAME_SIZE];
		int i;

		for (i = 0; i < sizeof(regs)/sizeof(regs[0]); i++)
			regs[i] = ptrace(PTRACE_PEEKDATA, debugger->pid,
					 arg4 + 4 * i, 0);
		result = ptrace(PTRACE_SETREGS, child, 0, regs);
		if(result == -1) return(-errno);

		return(result);
	}
#endif

	case PTRACE_SINGLESTEP:
		result = ptrace(PTRACE_SINGLESTEP, child, arg3, arg4);
		if(result == -1) return(-errno);
		
		status = wait_for_stop(child, SIGTRAP, PTRACE_SINGLESTEP);
		child_proxy(child, status);
		return(result);

	case PTRACE_SYSCALL:
		result = ptrace(PTRACE_SYSCALL, child, arg3, arg4);
		if(result == -1) return(-errno);

		*ret = PTRACE_SYSCALL;
		return(result);

	case PTRACE_TRACEME:
	default:
		return(-EINVAL);
	}
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
