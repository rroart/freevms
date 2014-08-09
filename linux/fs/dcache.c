// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004.

/*
 * fs/dcache.c
 *
 * Complete reimplementation
 * (C) 1997 Thomas Schoebel-Theuer,
 * with heavy changes by Linus Torvalds
 */

/*
 * Notes on the allocation strategy:
 *
 * The dcache is a master of the icache - whenever a dcache entry
 * exists, the inode will always exist. "iput()" is done either when
 * the dcache entry is deleted or garbage collected.
 */

#include <linux/config.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/smp_lock.h>
#include <linux/cache.h>
#include <linux/module.h>

#include <asm/uaccess.h>

#include <misc_routines.h>

asmlinkage long sys_getcwd(char *buf, unsigned long size)
{
  // check. quick and dirty. fix later
	int error = 0;
	extern char default_buffer[];
	memcpy(buf, default_buffer, size);
	return error;
}
