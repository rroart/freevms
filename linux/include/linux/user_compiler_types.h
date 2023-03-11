#ifndef __USER_LINUX_COMPILER_TYPES_H
#define __USER_LINUX_COMPILER_TYPES_H

#ifdef __KERNEL__
#include "compiler_types.h"
#else
#define __KERNEL__
#include "compiler_types.h"
#undef __KERNEL__
#endif

#define __inline __inline__

#endif
