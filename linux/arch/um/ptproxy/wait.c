/**********************************************************************
wait.c

Copyright (C) 1999 Lars Brinkhoff.  See the file COPYING for licensing
terms and conditions.

**********************************************************************/

#include <errno.h>
#include <sys/wait.h>
#include <sys/ptrace.h>

#include "ptproxy.h"
#include "sysdep.h"
#include "wait.h"

#include <asm/ptrace.h>
#include "sysdep/ptrace.h"

int proxy_wait_return (struct debugger *debugger, pid_t unused)
{
	debugger->waiting = 0;

	if(debugger->debugee->died || (debugger->wait_options & __WCLONE)){
		debugger_cancelled_return(debugger, -ECHILD);
		return(0);
	}

	if(debugger->debugee->zombie && debugger->debugee->event)
		debugger->debugee->died = 1;

	if(debugger->debugee->event){
		debugger->debugee->event = 0;
		ptrace(PTRACE_POKEDATA, debugger->pid,
		       debugger->wait_status_ptr, 
		       debugger->debugee->wait_status);
		/* if (wait4)
		   ptrace (PTRACE_POKEDATA, pid, rusage_ptr, ...); */
		debugger_cancelled_return(debugger, debugger->debugee->pid);
		return(0);
	}

	/* pause will return -EINTR, which happens to be right for wait */
	debugger_normal_return(debugger, -1);
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
