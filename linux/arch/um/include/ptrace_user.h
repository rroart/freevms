/* 
 * Copyright (C) 2000, 2001, 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef __PTRACE_USER_H__
#define __PTRACE_USER_H__

#include <linux/ptrace.h>
#include "sysdep/ptrace_user.h"

extern int ptrace_getregs(long pid, unsigned long *regs_out);
extern int ptrace_setregs(long pid, unsigned long *regs_in);

#endif
