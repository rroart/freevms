/* 
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include "linux/sched.h"
#include "user_util.h"
#include "kern_util.h"
#include "kern.h"

static void kill_off_processes(void)
{
	struct task_struct *p;
	int me;

	me = getpid();
	for_each_task_pre1(p){
		if(p->thread.extern_pid != me) kill_pid(p->thread.extern_pid);
	}
	for_each_task_post1(p);
	if(init_task.thread.extern_pid != me) 
		kill_pid(init_task.thread.extern_pid);
}

void uml_cleanup(void)
{
	kill_off_processes();
	do_uml_exitcalls();
}

void machine_restart(char * __unused)
{
	do_exitcalls();
	kill_off_processes();
	tracing_reboot();
	kill_pid(getpid());
}

void machine_power_off(void)
{
	do_exitcalls();
	kill_off_processes();
	tracing_halt();
	kill_pid(getpid());
}

void machine_halt(void)
{
	machine_power_off();
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
