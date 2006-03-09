/*
 *  linux/lib/errno.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#ifdef __i386__
int errno;
#else
int kernel_errno;
#endif
