/* 
 * Copyright (C) 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef __UM_SIGNAL_H
#define __UM_SIGNAL_H

#include "sysdep/ptrace.h"
#include "asm/arch/signal.h"

struct signal_context {
	void *sc;
	sigset_t sigs;
	struct signal_context *prev;
};

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
