/**********************************************************************
ptproxy.h

Copyright (C) 1999 Lars Brinkhoff.  See the file COPYING for licensing
terms and conditions.
**********************************************************************/

#ifndef __PTPROXY_H
#define __PTPROXY_H

#include <sys/types.h>

typedef struct debugger debugger_state;
typedef struct debugee debugee_state;

struct debugger
{
	pid_t pid;
	int wait_options;
	int *wait_status_ptr;
	unsigned int waiting : 1;
	int (*handle_trace) (debugger_state *, pid_t);

	debugee_state *debugee;
};

struct debugee
{
	pid_t pid;
	int wait_status;
	unsigned died : 1;
	unsigned event : 1;
	unsigned stopped : 1;
	unsigned trace_singlestep : 1;
	unsigned trace_syscall : 1;
	unsigned traced : 1;
	unsigned zombie : 1;
};

extern int debugger_syscall(debugger_state *debugger, pid_t pid);
extern int debugger_normal_return (debugger_state *debugger, pid_t unused);

extern long proxy_ptrace (struct debugger *, int, pid_t, long, long, pid_t,
			  int *strace_out);
extern void debugger_cancelled_return(debugger_state *debugger, int result);

#endif

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
