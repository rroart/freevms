// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified User Mode Linux source file, 2001-2004.

#ifndef __UM_TIMEX_H
#define __UM_TIMEX_H

#include "linux/time.h"
#include "asm-i386/timex.h"

#if 0
typedef unsigned long cycles_t;
#endif

#define cacheflush_time (0)

#if 0
static inline cycles_t get_cycles (void)
{
	return 0;
}
#endif

#endif
