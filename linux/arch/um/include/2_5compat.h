// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified User Mode Linux source file, 2001-2004.

/* 
 * Copyright (C) 2001 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef __2_5_COMPAT_H__
#define __2_5_COMPAT_H__

#include "linux/version.h"
#include "asm-i386/hw_irq.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 0)

#define major(dev) MAJOR(dev)
#define minor(dev) MINOR(dev)
#define kdev_same(dev1, dev2) ((dev1) == (dev2))
#define mk_kdev(maj, min) MKDEV(maj, min)
#define INIT_CONSOLE(dev_name, write_proc, device_proc, setup_proc, f) { \
	name :		dev_name, \
	write :		write_proc, \
	read :		NULL, \
	device :	device_proc, \
	wait_key :	NULL, \
	unblank :	NULL, \
	setup :		setup_proc, \
	flags :		f, \
	index :		-1, \
	cflag :		0, \
	next :		NULL \
}

#define INIT_GENDISK(maj, name, parts, bsizes, max, blops) { \
	major :		maj, \
	major_name : 	name, \
	minor_shift :	0, \
	max_p : 	1, \
	part : 		parts, \
	sizes : 	bsizes, \
	nr_real : 	max, \
	real_devices : 	NULL, \
	next : 		NULL, \
	fops : 		blops, \
	de_arr : 	NULL, \
	flags : 	0 \
}

#define INIT_QUEUE(queue, request, lock) blk_init_queue(queue, request)

#define ELV_NOOP ELEVATOR_NOOP
#define INIT_ELV(queue, elv) elevator_init(elv, ELV_NOOP)

#define REQUEST_LOCK io_request_lock

#define INIT_HARDSECT(arr, maj, sizes) arr[maj] = sizes

#define IS_WRITE(req) ((req)->cmd == WRITE)
#define IS_READ(req) ((req)->cmd == READ)

#define CPU(task) ((task)->pcb$l_cpu_id)

#define yield() do { SOFTINT_RESCHED_VECTOR; } while(0)

#define SET_PRI(task) \
	do { (task)->pcb$b_prib = 31; (task)->pcb$b_pri = 31; } while(0);

#else

#define INIT_CONSOLE(dev_name, write_proc, device_proc, setup_proc, f) { \
	name :		dev_name, \
	write :		write_proc, \
	read :		NULL, \
	device :	device_proc, \
	setup :		setup_proc, \
	flags :		f, \
	index :		-1, \
	cflag :		0, \
	next :		NULL \
}

#define INIT_GENDISK(maj, name, parts, bsizes, max, blops) { \
	major :		maj, \
	major_name : 	name, \
	minor_shift :	0, \
	part : 		parts, \
	sizes : 	bsizes, \
	nr_real : 	max, \
	next : 		NULL, \
	fops : 		blops, \
	de_arr : 	NULL, \
	flags : 	0 \
}

#define INIT_QUEUE(queue, request, lock) blk_init_queue(queue, request, lock)

#define ELV_NOOP elevator_noop
#define INIT_ELV(queue, elv) elevator_init(queue, elv, ELV_NOOP)

#define REQUEST_LOCK ubd_lock

#define INIT_HARDSECT(arr, maj, sizes)

#define IS_WRITE(req) (rq_data_dir(req) == WRITE)
#define IS_READ(req) (rq_data_dir(req) == READ)

#define CPU(task) ((task)->cpu)

#define SET_PRI(task) do ; while(0)

#endif

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
