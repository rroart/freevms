/* 
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include "linux/sched.h"
#include "asm/ptrace.h"
#include "sysdep/sc.h"

/* determines which flags the user has access to. */
/* 1 = access 0 = no access */
#define FLAG_MASK 0x00044dd5

int putreg(struct task_struct *child, int regno, unsigned long value)
{
	regno >>= 2;
	switch (regno) {
	case FS:
		if (value && (value & 3) != 3)
			return -EIO;
		PT_REGS_FS(&child->thread.regs) = value;
		return 0;
	case GS:
		if (value && (value & 3) != 3)
			return -EIO;
		PT_REGS_GS(&child->thread.regs) = value;
		return 0;
	case DS:
	case ES:
		if (value && (value & 3) != 3)
			return -EIO;
		value &= 0xffff;
		break;
	case SS:
	case CS:
		if ((value & 3) != 3)
			return -EIO;
		value &= 0xffff;
		break;
	case EFL:
		value &= FLAG_MASK;
		value |= PT_REGS_EFLAGS(&child->thread.regs);
		break;
	}
	PT_REGS_SET(&child->thread.regs, regno, value);
	return 0;
}

unsigned long getreg(struct task_struct *child, int regno)
{
	unsigned long retval = ~0UL;

	regno >>= 2;
	switch (regno) {
	case FS:
	case GS:
	case DS:
	case ES:
	case SS:
	case CS:
		retval = 0xffff;
		/* fall through */
	default:
		retval &= PT_REG(&child->thread.regs, regno);
	}
	return retval;
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
