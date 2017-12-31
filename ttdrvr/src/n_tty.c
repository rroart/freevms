/*
 * n_tty.c --- implements the N_TTY line discipline.
 *
 * This code used to be in tty_io.c, but things are getting hairy
 * enough that it made sense to split things off.  (The N_TTY
 * processing has changed so much that it's hardly recognizable,
 * anyway...)
 *
 * Note that the open routine for N_TTY is guaranteed never to return
 * an error.  This is because Linux will fall back to setting a line
 * to N_TTY if it can not switch to any other line discipline.
 *
 * Written by Theodore Ts'o, Copyright 1994.
 *
 * This file also contains code originally written by Linus Torvalds,
 * Copyright 1991, 1992, 1993, and by Julian Cowley, Copyright 1994.
 *
 * This file may be redistributed under the terms of the GNU General Public
 * License.
 *
 * Reduced memory usage for older ARM systems  - Russell King.
 *
 * 2000/01/20   Fixed SMP locking on put_tty_queue using bits of
 *		the patch by Andrew J. Kroll <ag784@freenet.buffalo.edu>
 *		who actually finally proved there really was a race.
 */

#include <linux/types.h>
#include <linux/major.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/fcntl.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/tty.h>
#include <linux/timer.h>
#include <linux/ctype.h>
#include <linux/kd.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/poll.h>

#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/bitops.h>

#include <ssdef.h>
#include <system_data_cells.h>
#include <ccbdef.h>
#include <ucbdef.h>
#include <irpdef.h>

#define CONSOLE_DEV MKDEV(TTY_MAJOR,0)
#define SYSCONS_DEV  MKDEV(TTYAUX_MAJOR,1)

#ifndef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif

/* number of characters left in xmit buffer before select has we have room */
#define WAKEUP_CHARS 256

/*
 * This defines the low- and high-watermarks for throttling and
 * unthrottling the TTY driver.  These watermarks are used for
 * controlling the space in the read buffer.
 */
#define TTY_THRESHOLD_THROTTLE		128 /* now based on remaining room */
#define TTY_THRESHOLD_UNTHROTTLE 	128

int is_ignored(int sig)
{
    return (sigismember(&current->blocked, sig) ||
            current->sig->action[sig-1].sa.sa_handler == SIG_IGN);
}

struct tty_ldisc tty_ldisc_N_TTY =
{
    TTY_LDISC_MAGIC,	/* magic */
    "n_tty",		/* name */
    0,			/* num */
    0,			/* flags */
    0,		/* open */
    0,		/* close */
    0,	/* flush_buffer */
    0,	/* chars_in_buffer */
    0,		/* read */
    0,		/* write */
    0,		/* ioctl */
    0,	/* set_termios */
    0,		/* poll */
    0,	/* receive_buf */
    0,	/* receive_room */
    0			/* write_wakeup */
};

