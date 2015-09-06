// $Id$
// $Locker$

// Author. Roar Thron�s.
// Modified Linux source file, 2001-2004.

#ifndef __LINUX_COMPILER_H
#define __LINUX_COMPILER_H

#define likely(x)   __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

#define __attribute_used__  __attribute((__used__))

#define __attribute_const__ __attribute__((__const__))

#endif /* __LINUX_COMPILER_H */
