/**********************************************************************
proxy.c

Copyright (C) 1999 Lars Brinkhoff.  See the file COPYING for licensing
terms and conditions.

Jeff Dike (jdike@karaya.com) : Modified for integration into uml
**********************************************************************/

/* XXX This file shouldn't refer to CONFIG_* */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/ioctl.h>
#include <asm/unistd.h>

#include "ptproxy.h"
#include "sysdep.h"
#include "wait.h"

#include "user_util.h"
#include "user.h"

/*
 * Handle debugger trap, i.e. syscall.
 */

int debugger_syscall (debugger_state *debugger, pid_t child)
{
	long arg1, arg2, arg3, arg4, arg5, result;
	int syscall, ret = 0;

	syscall = get_syscall(debugger->pid, &arg1, &arg2, &arg3, &arg4, 
			      &arg5);

	switch(syscall){
	case __NR_execve:
		/* execve never returns */
		debugger->handle_trace = debugger_syscall; 
		break;

	case __NR_ptrace:
		if(debugger->debugee->pid != 0) arg2 = debugger->debugee->pid;
		result = proxy_ptrace(debugger, arg1, arg2, arg3, arg4, child,
				      &ret);
		syscall_cancel(debugger->pid, result);
		debugger->handle_trace = debugger_syscall;
		return(ret);

	case __NR_waitpid:
	case __NR_wait4:
		debugger->wait_status_ptr = (int *) arg2;
		debugger->wait_options = arg3;
		if(debugger->debugee->event){
			syscall_continue(debugger->pid);
			wait_for_stop(debugger->pid, SIGTRAP, PTRACE_SYSCALL);
			proxy_wait_return(debugger, -1);
			return(0);
		}
		else if(debugger->wait_options & WNOHANG){
			syscall_cancel(debugger->pid, 0);
			debugger->handle_trace = debugger_syscall;
			return(0);
		}
		else {
			syscall_pause(debugger->pid);
			debugger->handle_trace = proxy_wait_return;
			debugger->waiting = 1;
		}
		break;

	case __NR_kill:
		if(arg1 == debugger->debugee->pid){
			result = kill(child, arg2);
			syscall_cancel(debugger->pid, result);
			debugger->handle_trace = debugger_syscall;
			return(0);
		}
		else debugger->handle_trace = debugger_normal_return;
		break;

	default:
		debugger->handle_trace = debugger_normal_return;
	}

	syscall_continue (debugger->pid);
	return(ret);
}

int debugger_normal_return(debugger_state *debugger, pid_t unused)
{
	debugger->handle_trace = debugger_syscall;
	syscall_continue(debugger->pid);
	return(0);
}

void debugger_cancelled_return(debugger_state *debugger, int result)
{
	debugger->handle_trace = debugger_syscall;
	syscall_set_result(debugger->pid, result);
	syscall_continue(debugger->pid);
}

#ifdef CONFIG_SMP
#error need to make these arrays
#endif

static debugger_state debugger;
static debugee_state debugee;

void init_proxy (pid_t debugger_pid, int stopped, int status)
{
	debugger.pid = debugger_pid;
	debugger.handle_trace = debugger_syscall;
	debugger.debugee = &debugee;
	debugger.waiting = 0;

	debugee.pid = 0;
	debugee.traced = 0;
	debugee.stopped = stopped;
	debugee.event = 0;
	debugee.zombie = 0;
	debugee.died = 0;
	debugee.wait_status = status;
}

int debugger_proxy(int status, int pid)
{
	int ret = 0;

	if(WIFSTOPPED(status)){
		if (WSTOPSIG (status) == SIGTRAP)
			ret = (*debugger.handle_trace)(&debugger, pid);
		else ptrace(PTRACE_SYSCALL, debugger.pid, 0, WSTOPSIG(status));
	}
	else if(WIFEXITED(status)){
		tracer_panic("debugger (pid %d) exited with status %d", 
			     debugger.pid, WEXITSTATUS(status));
	}
	else if(WIFSIGNALED(status)){
		tracer_panic("debugger (pid %d) exited with signal %d", 
			     debugger.pid, WTERMSIG(status));
	}
	else {
		tracer_panic("proxy got unknown status (0x%x) on debugger "
			     "(pid %d)", status, debugger.pid);
	}
	return(ret);
}

void child_proxy(pid_t pid, int status)
{
	debugee.event = 1;
	debugee.wait_status = status;

	if(WIFSTOPPED(status)){
		debugee.stopped = 1;
		kill(debugger.pid, SIGCHLD);
	}
	else if(WIFEXITED(status) || WIFSIGNALED(status)){
		debugee.zombie = 1;
		kill(debugger.pid, SIGCHLD);
	}
	else panic("proxy got unknown status (0x%x) on child (pid %d)", 
		   status, pid);
}

void fake_child_exit(void)
{
	int status, pid;

	child_proxy(1, W_EXITCODE(0, 0));
	while(debugger.waiting == 1){
		pid = waitpid(debugger.pid, &status, WUNTRACED);
		if(pid != debugger.pid){
			printk("fake_child_exit - waitpid failed, "
			       "errno = %d\n", errno);
			return;
		}
		debugger_proxy(status, debugger.pid);
	}
	pid = waitpid(debugger.pid, &status, WUNTRACED);
	if(pid != debugger.pid){
		printk("fake_child_exit - waitpid failed, "
		       "errno = %d\n", errno);
		return;
	}
	if(ptrace(PTRACE_DETACH, debugger.pid, 0, SIGCONT) < 0)
		printk("fake_child_exit - PTRACE_DETACH failed, errno = %d\n",
		       errno);
}

char gdb_init_string[] = 
"att 1\nb panic\nb stop\nhandle SIGWINCH nostop noprint pass\n";

int start_debugger(char *prog, int startup, int stop, int *fd_out)
{
	int slave, child;

	slave = open_gdb_chan();
	if((child = fork()) == 0){
		char *tempname = NULL;
		int fd;

	        if(setsid() < 0) perror("setsid");
		if((dup2(slave, 0) < 0) || (dup2(slave, 1) < 0) || 
		   (dup2(slave, 2) < 0)){
			printk("start_debugger : dup2 failed, errno = %d\n",
			       errno);
			exit(1);
		}
		if(ioctl(0, TIOCSCTTY, 0) < 0){
			printk("start_debugger : TIOCSCTTY failed, "
			       "errno = %d\n", errno);
			exit(1);
		}
		if(tcsetpgrp (1, getpid()) < 0){
			printk("start_debugger : tcsetpgrp failed, "
			       "errno = %d\n", errno);
#ifdef notdef
			exit(1);
#endif
		}
		if((fd = make_tempfile("/tmp/gdb_init-XXXXXX", &tempname, 0)) < 0){
			printk("start_debugger : make_tempfile failed, errno = %d\n",
			       errno);
			exit(1);
		}
		write(fd, gdb_init_string, sizeof(gdb_init_string) - 1);
		if(startup){
			if(stop){
				write(fd, "b start_kernel\n",
				      strlen("b start_kernel\n"));
			}
			write(fd, "c\n", strlen("c\n"));
		}
		if(ptrace(PTRACE_TRACEME, 0, 0, 0) < 0){
			printk("start_debugger :  PTRACE_TRACEME failed, "
			       "errno = %d\n", errno);
			exit(1);
		}
		execlp("gdb", "gdb", "--command", tempname, prog, NULL);
		printk("start_debugger : exec of gdb failed, errno = %d\n",
		       errno);
	}
	if(child < 0){
		printk("start_debugger : fork for gdb failed, errno = %d\n",
		       errno);
		return(-1);
	}
	*fd_out = slave;
	return(child);
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
