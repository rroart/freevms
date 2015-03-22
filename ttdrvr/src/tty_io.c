// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004

/*
 *  linux/drivers/char/tty_io.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/*
 * 'tty_io.c' gives an orthogonal feeling to tty's, be they consoles
 * or rs-channels. It also implements echoing, cooked mode etc.
 *
 * Kill-line thanks to John T Kohl, who also corrected VMIN = VTIME = 0.
 *
 * Modified by Theodore Ts'o, 9/14/92, to dynamically allocate the
 * tty_struct and tty_queue structures.  Previously there was an array
 * of 256 tty_struct's which was statically allocated, and the
 * tty_queue structures were allocated at boot time.  Both are now
 * dynamically allocated only when the tty is open.
 *
 * Also restructured routines so that there is more of a separation
 * between the high-level tty routines (tty_io.c and tty_ioctl.c) and
 * the low-level tty routines (serial.c, pty.c, console.c).  This
 * makes for cleaner and more compact code.  -TYT, 9/17/92
 *
 * Modified by Fred N. van Kempen, 01/29/93, to add line disciplines
 * which can be dynamically activated and de-activated by the line
 * discipline handling modules (like SLIP).
 *
 * NOTE: pay no attention to the line discipline code (yet); its
 * interface is still subject to change in this version...
 * -- TYT, 1/31/92
 *
 * Added functionality to the OPOST tty handling.  No delays, but all
 * other bits should be there.
 *  -- Nick Holloway <alfie@dcs.warwick.ac.uk>, 27th May 1993.
 *
 * Rewrote canonical mode and added more termios flags.
 *  -- julian@uhunix.uhcc.hawaii.edu (J. Cowley), 13Jan94
 *
 * Reorganized FASYNC support so mouse code can share it.
 *  -- ctm@ardi.com, 9Sep95
 *
 * New TIOCLINUX variants added.
 *  -- mj@k332.feld.cvut.cz, 19-Nov-95
 *
 * Restrict vt switching via ioctl()
 *      -- grif@cs.ucr.edu, 5-Dec-95
 *
 * Move console and virtual terminal code to more appropriate files,
 * implement CONFIG_VT and generalize console device interface.
 *  -- Marko Kohtala <Marko.Kohtala@hut.fi>, March 97
 *
 * Rewrote init_dev and release_dev to eliminate races.
 *  -- Bill Hawes <whawes@star.net>, June 97
 *
 * Added devfs support.
 *      -- C. Scott Ananian <cananian@alumni.princeton.edu>, 13-Jan-1998
 *
 * Added support for a Unix98-style ptmx device.
 *      -- C. Scott Ananian <cananian@alumni.princeton.edu>, 14-Jan-1998
 *
 * Reduced memory usage for older ARM systems
 *      -- Russell King <rmk@arm.linux.org.uk>
 *
 * Move do_SAK() into process context.  Less stack use in devfs functions.
 * alloc_tty_struct() always uses kmalloc() -- Andrew Morton <andrewm@uow.edu.eu> 17Mar01
 */

#include <linux/config.h>
#include <linux/types.h>
#include <linux/major.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/fcntl.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/file.h>
#include <linux/console.h>
#include <linux/timer.h>
#include <linux/ctype.h>
#include <linux/kd.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/smp_lock.h>

#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/bitops.h>

#include <linux/kbd_kern.h>
#include <linux/vt_kern.h>
#include <linux/selection.h>

#include <linux/kmod.h>

#include <iosbdef.h>
#include <pridef.h>
#include <iodef.h>
#include <misc.h>
#include <descrip.h>
#include <ucbdef.h>
#include <system_data_cells.h>
#include <ccbdef.h>
#include <exe_routines.h>
#include <misc_routines.h>

#ifdef CONFIG_VT
#if 1
extern void con_init_devfs (void);
#endif
#endif

#define CONSOLE_DEV MKDEV(TTY_MAJOR,0)
#define TTY_DEV MKDEV(TTYAUX_MAJOR,0)
#define SYSCONS_DEV MKDEV(TTYAUX_MAJOR,1)
#define PTMX_DEV MKDEV(TTYAUX_MAJOR,2)

#undef TTY_DEBUG_HANGUP

#define TTY_PARANOIA_CHECK 1
#define CHECK_TTY_COUNT 1

struct termios tty_std_termios;     /* for the benefit of tty drivers  */
struct tty_driver *tty_drivers;     /* linked list of tty drivers */
struct tty_ldisc ldiscs[NR_LDISCS]; /* line disc dispatch table */

#ifdef CONFIG_UNIX98_PTYS
extern struct tty_driver ptm_driver[];  /* Unix98 pty masters; for /dev/ptmx */
extern struct tty_driver pts_driver[];  /* Unix98 pty slaves;  for /dev/ptmx */
#endif

/*
 * redirect is the pseudo-tty that console output
 * is redirected to if asked by TIOCCONS.
 */
struct tty_struct * redirect;

static void initialize_tty_struct(struct tty_struct *tty);

static ssize_t tty_read(struct file *, char *, size_t, loff_t *);
static ssize_t tty_write(struct file *, const char *, size_t, loff_t *);
static unsigned int tty_poll(struct file *, poll_table *);
static int tty_open(struct inode *, struct file *);
static int tty_release(struct inode *, struct file *);
int tty_ioctl(struct inode * inode, struct file * file,
              unsigned int cmd, unsigned long arg);
static int tty_fasync(int fd, struct file * filp, int on);
extern int vme_scc_init (void);
extern long vme_scc_console_init(void);
extern int serial167_init(void);
extern long serial167_console_init(void);
extern void console_8xx_init(void);
extern int rs_8xx_init(void);
extern void mac_scc_console_init(void);
extern void hwc_console_init(void);
extern void hwc_tty_init(void);
extern void con3215_init(void);
extern void tty3215_init(void);
extern void tub3270_con_init(void);
extern void tub3270_init(void);
extern void rs285_console_init(void);
extern void sa1100_rs_console_init(void);
extern void sgi_serial_console_init(void);
extern void sci_console_init(void);
extern void tx3912_console_init(void);
extern void tx3912_rs_init(void);

#ifndef MIN
#define MIN(a,b)    ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b)    ((a) < (b) ? (b) : (a))
#endif

static struct tty_struct *alloc_tty_struct(void)
{
    struct tty_struct *tty;

    tty = kmalloc(sizeof(struct tty_struct), GFP_KERNEL);
    if (tty)
        memset(tty, 0, sizeof(struct tty_struct));
    return tty;
}

static inline void free_tty_struct(struct tty_struct *tty)
{
    kfree(tty);
}

/*
 * This routine returns the name of tty.
 */
static char *
_tty_make_name(struct tty_struct *tty, const char *name, char *buf)
{
    int idx = (tty)?MINOR(tty->device) - tty->driver.minor_start:0;

    if (!tty) /* Hmm.  NULL pointer.  That's fun. */
        strcpy(buf, "NULL tty");
    else
        sprintf(buf, name,
                idx + tty->driver.name_base);

    return buf;
}

#define TTY_NUMBER(tty) (MINOR((tty)->device) - (tty)->driver.minor_start + \
             (tty)->driver.name_base)

char *tty_name(struct tty_struct *tty, char *buf)
{
    return _tty_make_name(tty, (tty)?tty->driver.name:NULL, buf);
}

inline int tty_paranoia_check(struct tty_struct *tty, kdev_t device,
                              const char *routine)
{
#ifdef TTY_PARANOIA_CHECK
    static const char badmagic[] = KERN_WARNING
                                   "Warning: bad magic number for tty struct (%x) in %s\n";
    static const char badtty[] = KERN_WARNING
                                 "Warning: null TTY for (%x) in %s\n";

    if (!tty)
    {
        printk(badtty, device, routine);
        return 1;
    }
    if (tty->magic != TTY_MAGIC)
    {
        printk(badmagic, device, routine);
        return 1;
    }
#endif
    return 0;
}

static int check_tty_count(struct tty_struct *tty, const char *routine)
{
#ifdef CHECK_TTY_COUNT
    struct list_head *p;
    int count = 0;

    file_list_lock();
    for(p = tty->tty_files.next; p != &tty->tty_files; p = p->next)
    {
        if(list_entry(p, struct file, f_list)->private_data == tty)
            count++;
    }
    file_list_unlock();
    if (tty->driver.type == TTY_DRIVER_TYPE_PTY &&
            tty->driver.subtype == PTY_TYPE_SLAVE &&
            tty->link && tty->link->count)
        count++;
    if (tty->count != count)
    {
        printk(KERN_WARNING "Warning: dev (%s) tty->count(%d) "
               "!= #fd's(%d) in %s\n",
               tty->device, tty->count, count, routine);
        return count;
    }
#endif
    return 0;
}

int tty_register_ldisc(int disc, struct tty_ldisc *new_ldisc)
{
    if (disc < N_TTY || disc >= NR_LDISCS)
        return -EINVAL;

    if (new_ldisc)
    {
        ldiscs[disc] = *new_ldisc;
        ldiscs[disc].flags |= LDISC_FLAG_DEFINED;
        ldiscs[disc].num = disc;
    }
    else
        memset(&ldiscs[disc], 0, sizeof(struct tty_ldisc));

    return 0;
}

EXPORT_SYMBOL(tty_register_ldisc);

/* Set the discipline of a tty line. */
static int tty_set_ldisc(struct tty_struct *tty, int ldisc)
{
    int retval = 0;
    struct  tty_ldisc o_ldisc;
    char buf[64];

    if ((ldisc < N_TTY) || (ldisc >= NR_LDISCS))
        return -EINVAL;
    /* Eduardo Blanco <ejbs@cs.cs.com.uy> */
    /* Cyrus Durgin <cider@speakeasy.org> */
    if (!(ldiscs[ldisc].flags & LDISC_FLAG_DEFINED))
    {
        char modname [20];
        sprintf(modname, "tty-ldisc-%d", ldisc);
        request_module (modname);
    }
    if (!(ldiscs[ldisc].flags & LDISC_FLAG_DEFINED))
        return -EINVAL;

    if (tty->ldisc.num == ldisc)
        return 0;   /* We are already in the desired discipline */
    o_ldisc = tty->ldisc;

#if 0
    tty_wait_until_sent(tty, 0);
#endif

    /* Shutdown the current discipline. */
    if (tty->ldisc.close)
        (tty->ldisc.close)(tty);

    /* Now set up the new line discipline. */
    tty->ldisc = ldiscs[ldisc];
    tty->termios->c_line = ldisc;
    if (tty->ldisc.open)
        retval = (tty->ldisc.open)(tty);
    if (retval < 0)
    {
        tty->ldisc = o_ldisc;
        tty->termios->c_line = tty->ldisc.num;
        if (tty->ldisc.open && (tty->ldisc.open(tty) < 0))
        {
            tty->ldisc = ldiscs[N_TTY];
            tty->termios->c_line = N_TTY;
            if (tty->ldisc.open)
            {
                int r = tty->ldisc.open(tty);

                if (r < 0)
                    panic("Couldn't open N_TTY ldisc for "
                          "%s --- error %d.",
                          tty_name(tty, buf), r);
            }
        }
    }
    if (tty->ldisc.num != o_ldisc.num && tty->driver.set_ldisc)
        tty->driver.set_ldisc(tty);
    return retval;
}

/*
 * This routine returns a tty driver structure, given a device number
 */
struct tty_driver *get_tty_driver(kdev_t device)
{
    int major, minor;
    struct tty_driver *p;

    minor = MINOR(device);
    major = MAJOR(device);

    for (p = tty_drivers; p; p = p->next)
    {
        if (p->major != major)
            continue;
        if (minor < p->minor_start)
            continue;
        if (minor >= p->minor_start + p->num)
            continue;
        return p;
    }
    return NULL;
}

/*
 * If we try to write to, or set the state of, a terminal and we're
 * not in the foreground, send a SIGTTOU.  If the signal is blocked or
 * ignored, go ahead and perform the operation.  (POSIX 7.2)
 */
int tty_check_change(struct tty_struct * tty)
{
    if (current->tty != tty)
        return 0;
    if (tty->pgrp <= 0)
    {
        printk(KERN_WARNING "tty_check_change: tty->pgrp <= 0!\n");
        return 0;
    }
    if (current->pgrp == tty->pgrp)
        return 0;
#if 0
    if (is_ignored(SIGTTOU))
        return 0;
#endif
    if (is_orphaned_pgrp(current->pgrp))
        return -EIO;
    (void) kill_pg(current->pgrp,SIGTTOU,1);
    return -ERESTARTSYS;
}

static struct file_operations tty_fops =
{
llseek:
    no_llseek,
read:
    tty_read,
write:
    tty_write,
poll:
    tty_poll,
ioctl:
    tty_ioctl,
open:
    tty_open,
release:
    tty_release,
fasync:
    tty_fasync,
};

void tty_vhangup(struct tty_struct * tty)
{
#ifdef TTY_DEBUG_HANGUP
    char    buf[64];

    printk(KERN_DEBUG "%s vhangup...\n", tty_name(tty, buf));
#endif
#if 0
    do_tty_hangup((void *) tty);
#endif
}

/*
 * This function is typically called only by the session leader, when
 * it wants to disassociate itself from its controlling tty.
 *
 * It performs the following functions:
 *  (1)  Sends a SIGHUP and SIGCONT to the foreground process group
 *  (2)  Clears the tty from being controlling the session
 *  (3)  Clears the controlling tty for all processes in the
 *      session group.
 *
 * The argument on_exit is set to 1 if called when a process is
 * exiting; it is 0 if called by the ioctl TIOCNOTTY.
 */
void disassociate_ctty(int on_exit)
{
    struct tty_struct *tty = current->tty;
    struct task_struct *p;
    int tty_pgrp = -1;

    if (tty)
    {
        tty_pgrp = tty->pgrp;
        if (on_exit && tty->driver.type != TTY_DRIVER_TYPE_PTY)
            tty_vhangup(tty);
    }
    else
    {
        if (current->tty_old_pgrp)
        {
            kill_pg(current->tty_old_pgrp, SIGHUP, on_exit);
            kill_pg(current->tty_old_pgrp, SIGCONT, on_exit);
        }
        return;
    }
    if (tty_pgrp > 0)
    {
        kill_pg(tty_pgrp, SIGHUP, on_exit);
        if (!on_exit)
            kill_pg(tty_pgrp, SIGCONT, on_exit);
    }

    current->tty_old_pgrp = 0;
    tty->session = 0;
    tty->pgrp = -1;

    read_lock(&tasklist_lock);
    for_each_task_pre1(p)
    if (p->session == current->session)
        p->tty = NULL;
    for_each_task_post1(p);
    read_unlock(&tasklist_lock);
}

void wait_for_keypress(void)
{
    struct console *c = console_drivers;
    if (c) c->wait_key(c);
}

void stop_tty(struct tty_struct *tty)
{
    if (tty->stopped)
        return;
    tty->stopped = 1;
    if (tty->link && tty->link->packet)
    {
        tty->ctrl_status &= ~TIOCPKT_START;
        tty->ctrl_status |= TIOCPKT_STOP;
        wake_up_interruptible(&tty->link->read_wait);
        //      wake_up_interruptible2(&tty->link->read_wait,PRI$_TICOM);
    }
    if (tty->driver.stop)
        (tty->driver.stop)(tty);
}

void start_tty(struct tty_struct *tty)
{
    if (!tty->stopped || tty->flow_stopped)
        return;
    tty->stopped = 0;
    if (tty->link && tty->link->packet)
    {
        tty->ctrl_status &= ~TIOCPKT_STOP;
        tty->ctrl_status |= TIOCPKT_START;
        wake_up_interruptible(&tty->link->read_wait);
        //      wake_up_interruptible2(&tty->link->read_wait,PRI$_TICOM);
    }
    if (tty->driver.start)
        (tty->driver.start)(tty);
    if ((test_bit(TTY_DO_WRITE_WAKEUP, &tty->flags)) &&
            tty->ldisc.write_wakeup)
        (tty->ldisc.write_wakeup)(tty);
    wake_up_interruptible(&tty->write_wait);
    //  wake_up_interruptible2(&tty->write_wait,PRI$_TOCOM);
}

static ssize_t tty_read(struct file * file, char * buf, size_t count,
                        loff_t *ppos)
{
    int i;
    struct tty_struct * tty;
    struct inode *inode;
    int sts;
    struct _iosb iosb;

    short int chan = 0;
    struct _pcb * p = ctl$gl_pcb;
    struct dsc$descriptor d;
    d.dsc$a_pointer=&p->pcb$t_terminal;
    d.dsc$w_length=strlen(p->pcb$t_terminal);
    sts=exe$assign(&d,&chan,0,0,0);
    struct _ucb *u=ctl$gl_ccbbase[chan].ccb$l_ucb;

    lock_kernel();

    // remember to add terminator stuff in here
    sts = exe$qiow(28,chan,IO$_READPBLK,&iosb,0,0,
                   buf,count,0,0,0,0);
    i = iosb.iosb$w_bcnt;

    unlock_kernel();

    exe$dassgn(chan);
    return i;
}

static ssize_t tty_write(struct file * file, const char * buf, size_t count,
                         loff_t *ppos)
{
    int is_console;
    struct tty_struct * tty;
    struct inode *inode;
    int sts;
    struct _iosb iosb;

    short int chan = 0;
    struct _pcb * p = ctl$gl_pcb;
    struct dsc$descriptor d;
    d.dsc$a_pointer=&p->pcb$t_terminal;
    d.dsc$w_length=strlen(p->pcb$t_terminal);
    sts=exe$assign(&d,&chan,0,0,0);
    struct _ucb *u=ctl$gl_ccbbase[chan].ccb$l_ucb;

    //  sts = exe$qio(0,(unsigned short)dev2chan(con_redirect(inode->i_rdev)),IO$_WRITEPBLK,0/*&iosb*/,0,0,
    sts = exe$qio(28,chan,IO$_WRITEPBLK,0/*&iosb*/,0,0,
                  buf,count,0,0,0,0);

    exe$dassgn(chan);
    return count;
}

/* Semaphore to protect creating and releasing a tty */
static DECLARE_MUTEX(tty_sem);

static void down_tty_sem(int index)
{
    down(&tty_sem);
}

static void up_tty_sem(int index)
{
    up(&tty_sem);
}

static void release_mem(struct tty_struct *tty, int idx);

/*
 * WSH 06/09/97: Rewritten to remove races and properly clean up after a
 * failed open.  The new code protects the open with a semaphore, so it's
 * really quite straightforward.  The semaphore locking can probably be
 * relaxed for the (most common) case of reopening a tty.
 */
static int init_dev(kdev_t device, struct tty_struct **ret_tty)
{
    struct tty_struct *tty, *o_tty;
    struct termios *tp, **tp_loc, *o_tp, **o_tp_loc;
    struct termios *ltp, **ltp_loc, *o_ltp, **o_ltp_loc;
    struct tty_driver *driver;
    int retval=0;
    int idx;

    driver = get_tty_driver(device);
    if (!driver)
        return -ENODEV;

    idx = MINOR(device) - driver->minor_start;

    /*
     * Check whether we need to acquire the tty semaphore to avoid
     * race conditions.  For now, play it safe.
     */
    down_tty_sem(idx);

    /* check whether we're reopening an existing tty */
    tty = driver->table[idx];
    if (tty) goto fast_track;

    /*
     * First time open is complex, especially for PTY devices.
     * This code guarantees that either everything succeeds and the
     * TTY is ready for operation, or else the table slots are vacated
     * and the allocated memory released.  (Except that the termios
     * and locked termios may be retained.)
     */

    o_tty = NULL;
    tp = o_tp = NULL;
    ltp = o_ltp = NULL;

    tty = alloc_tty_struct();
    if(!tty)
        goto fail_no_mem;
    initialize_tty_struct(tty);
    tty->device = device;
    tty->driver = *driver;

    tp_loc = &driver->termios[idx];
    if (!*tp_loc)
    {
        tp = (struct termios *) kmalloc(sizeof(struct termios),
                                        GFP_KERNEL);
        if (!tp)
            goto free_mem_out;
        *tp = driver->init_termios;
    }

    ltp_loc = &driver->termios_locked[idx];
    if (!*ltp_loc)
    {
        ltp = (struct termios *) kmalloc(sizeof(struct termios),
                                         GFP_KERNEL);
        if (!ltp)
            goto free_mem_out;
        memset(ltp, 0, sizeof(struct termios));
    }

    if (driver->type == TTY_DRIVER_TYPE_PTY)
    {
        o_tty = alloc_tty_struct();
        if (!o_tty)
            goto free_mem_out;
        initialize_tty_struct(o_tty);
        o_tty->device = (kdev_t) MKDEV(driver->other->major,
                                       driver->other->minor_start + idx);
        o_tty->driver = *driver->other;

        o_tp_loc  = &driver->other->termios[idx];
        if (!*o_tp_loc)
        {
            o_tp = (struct termios *)
                   kmalloc(sizeof(struct termios), GFP_KERNEL);
            if (!o_tp)
                goto free_mem_out;
            *o_tp = driver->other->init_termios;
        }

        o_ltp_loc = &driver->other->termios_locked[idx];
        if (!*o_ltp_loc)
        {
            o_ltp = (struct termios *)
                    kmalloc(sizeof(struct termios), GFP_KERNEL);
            if (!o_ltp)
                goto free_mem_out;
            memset(o_ltp, 0, sizeof(struct termios));
        }

        /*
         * Everything allocated ... set up the o_tty structure.
         */
        driver->other->table[idx] = o_tty;
        if (!*o_tp_loc)
            *o_tp_loc = o_tp;
        if (!*o_ltp_loc)
            *o_ltp_loc = o_ltp;
        o_tty->termios = *o_tp_loc;
        o_tty->termios_locked = *o_ltp_loc;
        (*driver->other->refcount)++;
        if (driver->subtype == PTY_TYPE_MASTER)
            o_tty->count++;

        /* Establish the links in both directions */
        tty->link   = o_tty;
        o_tty->link = tty;
    }

    /*
     * All structures have been allocated, so now we install them.
     * Failures after this point use release_mem to clean up, so
     * there's no need to null out the local pointers.
     */
    driver->table[idx] = tty;

    if (!*tp_loc)
        *tp_loc = tp;
    if (!*ltp_loc)
        *ltp_loc = ltp;
    tty->termios = *tp_loc;
    tty->termios_locked = *ltp_loc;
    (*driver->refcount)++;
    tty->count++;

    /*
     * Structures all installed ... call the ldisc open routines.
     * If we fail here just call release_mem to clean up.  No need
     * to decrement the use counts, as release_mem doesn't care.
     */
    if (tty->ldisc.open)
    {
        retval = (tty->ldisc.open)(tty);
        if (retval)
            goto release_mem_out;
    }
    if (o_tty && o_tty->ldisc.open)
    {
        retval = (o_tty->ldisc.open)(o_tty);
        if (retval)
        {
            if (tty->ldisc.close)
                (tty->ldisc.close)(tty);
            goto release_mem_out;
        }
    }
    goto success;

    /*
     * This fast open can be used if the tty is already open.
     * No memory is allocated, and the only failures are from
     * attempting to open a closing tty or attempting multiple
     * opens on a pty master.
     */
fast_track:
    if (test_bit(TTY_CLOSING, &tty->flags))
    {
        retval = -EIO;
        goto end_init;
    }
    if (driver->type == TTY_DRIVER_TYPE_PTY &&
            driver->subtype == PTY_TYPE_MASTER)
    {
        /*
         * special case for PTY masters: only one open permitted,
         * and the slave side open count is incremented as well.
         */
        if (tty->count)
        {
            retval = -EIO;
            goto end_init;
        }
        tty->link->count++;
    }
    tty->count++;
    tty->driver = *driver; /* N.B. why do this every time?? */

success:
    *ret_tty = tty;

    /* All paths come through here to release the semaphore */
end_init:
    up_tty_sem(idx);
    return retval;

    /* Release locally allocated memory ... nothing placed in slots */
free_mem_out:
    if (o_tp)
        kfree(o_tp);
    if (o_tty)
        free_tty_struct(o_tty);
    if (ltp)
        kfree(ltp);
    if (tp)
        kfree(tp);
    free_tty_struct(tty);

fail_no_mem:
    retval = -ENOMEM;
    goto end_init;

    /* call the tty release_mem routine to clean out this slot */
release_mem_out:
    printk(KERN_INFO "init_dev: ldisc open failed, "
           "clearing slot %d\n", idx);
    release_mem(tty, idx);
    goto end_init;
}

int init_dev2(kdev_t device, struct tty_struct **ret_tty)
{
    return init_dev(device, ret_tty);
}

/*
 * Releases memory associated with a tty structure, and clears out the
 * driver table slots.
 */
static void release_mem(struct tty_struct *tty, int idx)
{
    struct tty_struct *o_tty;
    struct termios *tp;

    if ((o_tty = tty->link) != NULL)
    {
        o_tty->driver.table[idx] = NULL;
        if (o_tty->driver.flags & TTY_DRIVER_RESET_TERMIOS)
        {
            tp = o_tty->driver.termios[idx];
            o_tty->driver.termios[idx] = NULL;
            kfree(tp);
        }
        o_tty->magic = 0;
        (*o_tty->driver.refcount)--;
        list_del(&o_tty->tty_files);
        free_tty_struct(o_tty);
    }

    tty->driver.table[idx] = NULL;
    if (tty->driver.flags & TTY_DRIVER_RESET_TERMIOS)
    {
        tp = tty->driver.termios[idx];
        tty->driver.termios[idx] = NULL;
        kfree(tp);
    }
    tty->magic = 0;
    (*tty->driver.refcount)--;
    list_del(&tty->tty_files);
    free_tty_struct(tty);
}

/*
 * Even releasing the tty structures is a tricky business.. We have
 * to be very careful that the structures are all released at the
 * same time, as interrupts might otherwise get the wrong pointers.
 *
 * WSH 09/09/97: rewritten to avoid some nasty race conditions that could
 * lead to double frees or releasing memory still in use.
 */
static void release_dev(struct file * filp)
{
    struct tty_struct *tty, *o_tty;
    int pty_master, tty_closing, o_tty_closing, do_sleep;
    int idx;
    char    buf[64];

    tty = (struct tty_struct *)filp->private_data;
    if (tty_paranoia_check(tty, filp->f_dentry->d_inode->i_rdev, "release_dev"))
        return;

    check_tty_count(tty, "release_dev");

    tty_fasync(-1, filp, 0);

    idx = MINOR(tty->device) - tty->driver.minor_start;
    pty_master = (tty->driver.type == TTY_DRIVER_TYPE_PTY &&
                  tty->driver.subtype == PTY_TYPE_MASTER);
    o_tty = tty->link;

#ifdef TTY_PARANOIA_CHECK
    if (idx < 0 || idx >= tty->driver.num)
    {
        printk(KERN_DEBUG "release_dev: bad idx when trying to "
               "free (%s)\n", tty->device);
        return;
    }
    if (tty != tty->driver.table[idx])
    {
        printk(KERN_DEBUG "release_dev: driver.table[%d] not tty "
               "for (%s)\n", idx, tty->device);
        return;
    }
    if (tty->termios != tty->driver.termios[idx])
    {
        printk(KERN_DEBUG "release_dev: driver.termios[%d] not termios "
               "for (%s)\n",
               idx, tty->device);
        return;
    }
    if (tty->termios_locked != tty->driver.termios_locked[idx])
    {
        printk(KERN_DEBUG "release_dev: driver.termios_locked[%d] not "
               "termios_locked for (%s)\n",
               idx, tty->device);
        return;
    }
#endif

#ifdef TTY_DEBUG_HANGUP
    printk(KERN_DEBUG "release_dev of %s (tty count=%d)...",
           tty_name(tty, buf), tty->count);
#endif

#ifdef TTY_PARANOIA_CHECK
    if (tty->driver.other)
    {
        if (o_tty != tty->driver.other->table[idx])
        {
            printk(KERN_DEBUG "release_dev: other->table[%d] "
                   "not o_tty for (%s)\n",
                   idx, tty->device);
            return;
        }
        if (o_tty->termios != tty->driver.other->termios[idx])
        {
            printk(KERN_DEBUG "release_dev: other->termios[%d] "
                   "not o_termios for (%s)\n",
                   idx, tty->device);
            return;
        }
        if (o_tty->termios_locked !=
                tty->driver.other->termios_locked[idx])
        {
            printk(KERN_DEBUG "release_dev: other->termios_locked["
                   "%d] not o_termios_locked for (%s)\n",
                   idx, tty->device);
            return;
        }
        if (o_tty->link != tty)
        {
            printk(KERN_DEBUG "release_dev: bad pty pointers\n");
            return;
        }
    }
#endif

    if (tty->driver.close)
        tty->driver.close(tty, filp);

    /*
     * Sanity check: if tty->count is going to zero, there shouldn't be
     * any waiters on tty->read_wait or tty->write_wait.  We test the
     * wait queues and kick everyone out _before_ actually starting to
     * close.  This ensures that we won't block while releasing the tty
     * structure.
     *
     * The test for the o_tty closing is necessary, since the master and
     * slave sides may close in any order.  If the slave side closes out
     * first, its count will be one, since the master side holds an open.
     * Thus this test wouldn't be triggered at the time the slave closes,
     * so we do it now.
     *
     * Note that it's possible for the tty to be opened again while we're
     * flushing out waiters.  By recalculating the closing flags before
     * each iteration we avoid any problems.
     */
    while (1)
    {
        tty_closing = tty->count <= 1;
        o_tty_closing = o_tty &&
                        (o_tty->count <= (pty_master ? 1 : 0));
        do_sleep = 0;

        if (tty_closing)
        {
            if (waitqueue_active(&tty->read_wait))
            {
                wake_up(&tty->read_wait);
                do_sleep++;
            }
            if (waitqueue_active(&tty->write_wait))
            {
                wake_up(&tty->write_wait);
                do_sleep++;
            }
        }
        if (o_tty_closing)
        {
            if (waitqueue_active(&o_tty->read_wait))
            {
                wake_up(&o_tty->read_wait);
                do_sleep++;
            }
            if (waitqueue_active(&o_tty->write_wait))
            {
                wake_up(&o_tty->write_wait);
                do_sleep++;
            }
        }
        if (!do_sleep)
            break;

        printk(KERN_WARNING "release_dev: %s: read/write wait queue "
               "active!\n", tty_name(tty, buf));
        schedule();
    }

    /*
     * The closing flags are now consistent with the open counts on
     * both sides, and we've completed the last operation that could
     * block, so it's safe to proceed with closing.
     */
    if (pty_master)
    {
        if (--o_tty->count < 0)
        {
            printk(KERN_WARNING "release_dev: bad pty slave count "
                   "(%d) for %s\n",
                   o_tty->count, tty_name(o_tty, buf));
            o_tty->count = 0;
        }
    }
    if (--tty->count < 0)
    {
        printk(KERN_WARNING "release_dev: bad tty->count (%d) for %s\n",
               tty->count, tty_name(tty, buf));
        tty->count = 0;
    }

    /*
     * We've decremented tty->count, so we should zero out
     * filp->private_data, to break the link between the tty and
     * the file descriptor.  Otherwise if filp_close() blocks before
     * the file descriptor is removed from the inuse_filp
     * list, check_tty_count() could observe a discrepancy and
     * printk a warning message to the user.
     */
    filp->private_data = 0;

    /*
     * Perform some housekeeping before deciding whether to return.
     *
     * Set the TTY_CLOSING flag if this was the last open.  In the
     * case of a pty we may have to wait around for the other side
     * to close, and TTY_CLOSING makes sure we can't be reopened.
     */
    if(tty_closing)
        set_bit(TTY_CLOSING, &tty->flags);
    if(o_tty_closing)
        set_bit(TTY_CLOSING, &o_tty->flags);

    /*
     * If _either_ side is closing, make sure there aren't any
     * processes that still think tty or o_tty is their controlling
     * tty.  Also, clear redirect if it points to either tty.
     */
    if (tty_closing || o_tty_closing)
    {
        struct task_struct *p;

        read_lock(&tasklist_lock);
        for_each_task_pre1(p)
        {
            if (p->tty == tty || (o_tty && p->tty == o_tty))
                p->tty = NULL;
        }
        for_each_task_post1(p);
        read_unlock(&tasklist_lock);

        if (redirect == tty || (o_tty && redirect == o_tty))
            redirect = NULL;
    }

    /* check whether both sides are closing ... */
    if (!tty_closing || (o_tty && !o_tty_closing))
        return;

#ifdef TTY_DEBUG_HANGUP
    printk(KERN_DEBUG "freeing tty structure...");
#endif

    /*
     * Shutdown the current line discipline, and reset it to N_TTY.
     * N.B. why reset ldisc when we're releasing the memory??
     */
    if (tty->ldisc.close)
        (tty->ldisc.close)(tty);
    tty->ldisc = ldiscs[N_TTY];
    tty->termios->c_line = N_TTY;
    if (o_tty)
    {
        if (o_tty->ldisc.close)
            (o_tty->ldisc.close)(o_tty);
        o_tty->ldisc = ldiscs[N_TTY];
    }

    /*
     * Make sure that the tty's task queue isn't activated.
     */

    /*
     * The release_mem function takes care of the details of clearing
     * the slots and preserving the termios structure.
     */
    release_mem(tty, idx);
}

is_tty_fops(struct file * f)
{
    return f->f_op==&tty_fops;
}

open_tty()
{
    int fd=get_unused_fd();
    struct file * f=get_empty_filp();
    f->f_op=&tty_fops;
    f->f_mode=FMODE_READ|FMODE_WRITE;
    fd_install(fd, f);
}

/*
 * tty_open and tty_release keep up the tty count that contains the
 * number of opens done on a tty. We cannot use the inode-count, as
 * different inodes might point to the same tty.
 *
 * Open-counting is needed for pty masters, as well as for keeping
 * track of serial lines: DTR is dropped when the last close happens.
 * (This is not done solely through tty->count, now.  - Ted 1/27/92)
 *
 * The termios state of a pty is reset on first open so that
 * settings don't persist across reuse.
 */
static int tty_open(struct inode * inode, struct file * filp)
{
    struct tty_struct *tty;
    int noctty, retval;
    kdev_t device;
    unsigned short saved_flags;
    char    buf[64];

    saved_flags = filp->f_flags;
retry_open:
    noctty = filp->f_flags & O_NOCTTY;
    device = inode->i_rdev;
    if (device == TTY_DEV)
    {
        if (!current->tty)
            return -ENXIO;
        device = current->tty->device;
        filp->f_flags |= O_NONBLOCK; /* Don't let /dev/tty block */
        /* noctty = 1; */
    }
#ifdef CONFIG_VT
    if (device == CONSOLE_DEV)
    {
        extern int fg_console;
        device = MKDEV(TTY_MAJOR, fg_console + 1);
        noctty = 1;
    }
#endif
    if (device == SYSCONS_DEV)
    {
        struct console *c = console_drivers;
        while(c && !c->device)
            c = c->next;
        if (!c)
            return -ENODEV;
        device = c->device(c);
        filp->f_flags |= O_NONBLOCK; /* Don't let /dev/console block */
        noctty = 1;
    }

    if (device == PTMX_DEV)
    {
#ifdef CONFIG_UNIX98_PTYS

        /* find a free pty. */
        int major, minor;
        struct tty_driver *driver;

        /* find a device that is not in use. */
        retval = -1;
        for ( major = 0 ; major < UNIX98_NR_MAJORS ; major++ )
        {
            driver = &ptm_driver[major];
            for (minor = driver->minor_start ;
                    minor < driver->minor_start + driver->num ;
                    minor++)
            {
                device = MKDEV(driver->major, minor);
                if (!init_dev(device, &tty)) goto ptmx_found; /* ok! */
            }
        }
        return -EIO; /* no free ptys */
ptmx_found:
        set_bit(TTY_PTY_LOCK, &tty->flags); /* LOCK THE SLAVE */
        minor -= driver->minor_start;
#if 0
        devpts_pty_new(driver->other->name_base + minor, MKDEV(driver->other->major, minor + driver->other->minor_start));
#else
        printk("no new pts\n");
#endif
#if 0
        tty_register_devfs(&pts_driver[major], DEVFS_FL_DEFAULT,
                           pts_driver[major].minor_start + minor);
#endif
        noctty = 1;
        goto init_dev_done;

#else   /* CONFIG_UNIX_98_PTYS */

        return -ENODEV;

#endif  /* CONFIG_UNIX_98_PTYS */
    }

    retval = init_dev(device, &tty);
    if (retval)
        return retval;

#ifdef CONFIG_UNIX98_PTYS
init_dev_done:
#endif
    filp->private_data = tty;
    file_move(filp, &tty->tty_files);
    check_tty_count(tty, "tty_open");
    if (tty->driver.type == TTY_DRIVER_TYPE_PTY &&
            tty->driver.subtype == PTY_TYPE_MASTER)
        noctty = 1;
#ifdef TTY_DEBUG_HANGUP
    printk(KERN_DEBUG "opening %s...", tty_name(tty, buf));
#endif
    if (tty->driver.open)
        retval = tty->driver.open(tty, filp);
    else
        retval = -ENODEV;
    filp->f_flags = saved_flags;

    if (!retval && test_bit(TTY_EXCLUSIVE, &tty->flags) && !suser())
        retval = -EBUSY;

    if (retval)
    {
#ifdef TTY_DEBUG_HANGUP
        printk(KERN_DEBUG "error %d in opening %s...", retval,
               tty_name(tty, buf));
#endif

        release_dev(filp);
        if (retval != -ERESTARTSYS)
            return retval;
        if (signal_pending(current))
            return retval;
        schedule();
        /*
         * Need to reset f_op in case a hangup happened.
         */
        filp->f_op = &tty_fops;
        goto retry_open;
    }
    if (!noctty &&
            current->leader &&
            !current->tty &&
            tty->session == 0)
    {
        task_lock(current);
        current->tty = tty;
        task_unlock(current);
        current->tty_old_pgrp = 0;
        tty->session = current->session;
        tty->pgrp = current->pgrp;
    }
    if ((tty->driver.type == TTY_DRIVER_TYPE_SERIAL) &&
            (tty->driver.subtype == SERIAL_TYPE_CALLOUT) &&
            (tty->count == 1))
    {
        static int nr_warns;
        if (nr_warns < 5)
        {
            printk(KERN_WARNING "tty_io.c: "
                   "process %d (%s) used obsolete /dev/%s - "
                   "update software to use /dev/ttyS%d\n",
                   current->pcb$l_pid, current->pcb$t_lname,
                   tty_name(tty, buf), TTY_NUMBER(tty));
            nr_warns++;
        }
    }
    return 0;
}

static int tty_release(struct inode * inode, struct file * filp)
{
    lock_kernel();
    release_dev(filp);
    unlock_kernel();
    return 0;
}

/* No kernel lock held - fine */
static unsigned int tty_poll(struct file * filp, poll_table * wait)
{
    struct tty_struct * tty;

    tty = (struct tty_struct *)filp->private_data;
    if (tty_paranoia_check(tty, filp->f_dentry->d_inode->i_rdev, "tty_poll"))
        return 0;

    if (tty->ldisc.poll)
        return (tty->ldisc.poll)(tty, filp, wait);
    return 0;
}

static int tty_fasync(int fd, struct file * filp, int on)
{
    struct tty_struct * tty;
    int retval;

    tty = (struct tty_struct *)filp->private_data;
    if (tty_paranoia_check(tty, filp->f_dentry->d_inode->i_rdev, "tty_fasync"))
        return 0;

    retval = fasync_helper(fd, filp, on, &tty->fasync);
    if (retval <= 0)
        return retval;

    if (on)
    {
        if (!waitqueue_active(&tty->read_wait))
            tty->minimum_to_wake = 1;
        if (filp->f_owner.pid == 0)
        {
            filp->f_owner.pid = (-tty->pgrp) ? : current->pcb$l_pid;
            filp->f_owner.uid = current->uid;
            filp->f_owner.euid = current->euid;
        }
    }
    else
    {
        if (!tty->fasync && !waitqueue_active(&tty->read_wait))
            tty->minimum_to_wake = N_TTY_BUF_SIZE;
    }
    return 0;
}

static int tiocsti(struct tty_struct *tty, char * arg)
{
    char ch, mbz = 0;

    if ((current->tty != tty) && !suser())
        return -EPERM;
    if (get_user(ch, arg))
        return -EFAULT;
    tty->ldisc.receive_buf(tty, &ch, &mbz, 1);
    return 0;
}

static int tiocgwinsz(struct tty_struct *tty, struct winsize * arg)
{
    if (copy_to_user(arg, &tty->winsize, sizeof(*arg)))
        return -EFAULT;
    return 0;
}

static int tiocswinsz(struct tty_struct *tty, struct tty_struct *real_tty,
                      struct winsize * arg)
{
    struct winsize tmp_ws;

    if (copy_from_user(&tmp_ws, arg, sizeof(*arg)))
        return -EFAULT;
    if (!memcmp(&tmp_ws, &tty->winsize, sizeof(*arg)))
        return 0;
    if (tty->pgrp > 0)
        kill_pg(tty->pgrp, SIGWINCH, 1);
    if ((real_tty->pgrp != tty->pgrp) && (real_tty->pgrp > 0))
        kill_pg(real_tty->pgrp, SIGWINCH, 1);
    tty->winsize = tmp_ws;
    real_tty->winsize = tmp_ws;
    return 0;
}

static int tioccons(struct inode *inode,
                    struct tty_struct *tty, struct tty_struct *real_tty)
{
    if (inode->i_rdev == SYSCONS_DEV ||
            inode->i_rdev == CONSOLE_DEV)
    {
        if (!suser())
            return -EPERM;
        redirect = NULL;
        return 0;
    }
    if (redirect)
        return -EBUSY;
    redirect = real_tty;
    return 0;
}


static int fionbio(struct file *file, int *arg)
{
    int nonblock;

    if (get_user(nonblock, arg))
        return -EFAULT;

    if (nonblock)
        file->f_flags |= O_NONBLOCK;
    else
        file->f_flags &= ~O_NONBLOCK;
    return 0;
}

static int tiocsctty(struct tty_struct *tty, int arg)
{
    if (current->leader &&
            (current->session == tty->session))
        return 0;
    /*
     * The process must be a session leader and
     * not have a controlling tty already.
     */
    if (!current->leader || current->tty)
        return -EPERM;
    if (tty->session > 0)
    {
        /*
         * This tty is already the controlling
         * tty for another session group!
         */
        if ((arg == 1) && suser())
        {
            /*
             * Steal it away
             */
            struct task_struct *p;

            read_lock(&tasklist_lock);
            for_each_task_pre1(p)
            if (p->tty == tty)
                p->tty = NULL;
            for_each_task_post1(p);
            read_unlock(&tasklist_lock);
        }
        else
            return -EPERM;
    }
    task_lock(current);
    current->tty = tty;
    task_unlock(current);
    current->tty_old_pgrp = 0;
    tty->session = current->session;
    tty->pgrp = current->pgrp;
    return 0;
}

static int tiocgpgrp(struct tty_struct *tty, struct tty_struct *real_tty, pid_t *arg)
{
    /*
     * (tty == real_tty) is a cheap way of
     * testing if the tty is NOT a master pty.
     */
    if (tty == real_tty && current->tty != real_tty)
        return -ENOTTY;
    return put_user(real_tty->pgrp, arg);
}

static int tiocspgrp(struct tty_struct *tty, struct tty_struct *real_tty, pid_t *arg)
{
    pid_t pgrp;
    int retval = tty_check_change(real_tty);

    if (retval == -EIO)
        return -ENOTTY;
    if (retval)
        return retval;
    if (!current->tty ||
            (current->tty != real_tty) ||
            (real_tty->session != current->session))
        return -ENOTTY;
    if (get_user(pgrp, (pid_t *) arg))
        return -EFAULT;
    if (pgrp < 0)
        return -EINVAL;
    if (session_of_pgrp(pgrp) != current->session)
        return -EPERM;
    real_tty->pgrp = pgrp;
    return 0;
}

static int tiocgsid(struct tty_struct *tty, struct tty_struct *real_tty, pid_t *arg)
{
    /*
     * (tty == real_tty) is a cheap way of
     * testing if the tty is NOT a master pty.
    */
    if (tty == real_tty && current->tty != real_tty)
        return -ENOTTY;
    if (real_tty->session <= 0)
        return -ENOTTY;
    return put_user(real_tty->session, arg);
}

static int tiocttygstruct(struct tty_struct *tty, struct tty_struct *arg)
{
    if (copy_to_user(arg, tty, sizeof(*arg)))
        return -EFAULT;
    return 0;
}

static int tiocsetd(struct tty_struct *tty, int *arg)
{
    int ldisc;

    if (get_user(ldisc, arg))
        return -EFAULT;
    return tty_set_ldisc(tty, ldisc);
}

static int send_break(struct tty_struct *tty, int duration)
{
    set_current_state(TASK_INTERRUPTIBLE);

    tty->driver.break_ctl(tty, -1);
    if (!signal_pending(current))
        schedule_timeout(duration);
    tty->driver.break_ctl(tty, 0);
    if (signal_pending(current))
        return -EINTR;
    return 0;
}

/*
 * Split this up, as gcc can choke on it otherwise..
 */
int tty_ioctl(struct inode * inode, struct file * file,
              unsigned int cmd, unsigned long arg)
{
    struct tty_struct *tty, *real_tty;
    int retval;

    tty = (struct tty_struct *)file->private_data;
    if (tty_paranoia_check(tty, inode->i_rdev, "tty_ioctl"))
        return -EINVAL;

    real_tty = tty;
    if (tty->driver.type == TTY_DRIVER_TYPE_PTY &&
            tty->driver.subtype == PTY_TYPE_MASTER)
        real_tty = tty->link;

    /*
     * Break handling by driver
     */
    if (!tty->driver.break_ctl)
    {
        switch(cmd)
        {
        case TIOCSBRK:
        case TIOCCBRK:
            if (tty->driver.ioctl)
                return tty->driver.ioctl(tty, file, cmd, arg);
            return -EINVAL;

            /* These two ioctl's always return success; even if */
            /* the driver doesn't support them. */
        case TCSBRK:
        case TCSBRKP:
            if (!tty->driver.ioctl)
                return 0;
            retval = tty->driver.ioctl(tty, file, cmd, arg);
            if (retval == -ENOIOCTLCMD)
                retval = 0;
            return retval;
        }
    }

    /*
     * Factor out some common prep work
     */
    switch (cmd)
    {
    case TIOCSETD:
    case TIOCSBRK:
    case TIOCCBRK:
    case TCSBRK:
    case TCSBRKP:
        retval = tty_check_change(tty);
        if (retval)
            return retval;
        if (cmd != TIOCCBRK)
        {
#if 0
            tty_wait_until_sent(tty, 0);
#endif
            if (signal_pending(current))
                return -EINTR;
        }
        break;
    }

    switch (cmd)
    {
    case TIOCSTI:
        return tiocsti(tty, (char *)arg);
    case TIOCGWINSZ:
        return tiocgwinsz(tty, (struct winsize *) arg);
    case TIOCSWINSZ:
        return tiocswinsz(tty, real_tty, (struct winsize *) arg);
    case TIOCCONS:
        return tioccons(inode, tty, real_tty);
    case FIONBIO:
        return fionbio(file, (int *) arg);
    case TIOCEXCL:
        set_bit(TTY_EXCLUSIVE, &tty->flags);
        return 0;
    case TIOCNXCL:
        clear_bit(TTY_EXCLUSIVE, &tty->flags);
        return 0;
    case TIOCNOTTY:
        if (current->tty != tty)
            return -ENOTTY;
        if (current->leader)
            disassociate_ctty(0);
        task_lock(current);
        current->tty = NULL;
        task_unlock(current);
        return 0;
    case TIOCSCTTY:
        return tiocsctty(tty, arg);
    case TIOCGPGRP:
        return tiocgpgrp(tty, real_tty, (pid_t *) arg);
    case TIOCSPGRP:
        return tiocspgrp(tty, real_tty, (pid_t *) arg);
    case TIOCGSID:
        return tiocgsid(tty, real_tty, (pid_t *) arg);
    case TIOCGETD:
        return put_user(tty->ldisc.num, (int *) arg);
    case TIOCSETD:
        return tiocsetd(tty, (int *) arg);
#ifdef CONFIG_VT
    case TIOCLINUX:
        return tioclinux(tty, arg);
#endif
    case TIOCTTYGSTRUCT:
        return tiocttygstruct(tty, (struct tty_struct *) arg);

        /*
         * Break handling
         */
    case TIOCSBRK:  /* Turn break on, unconditionally */
        tty->driver.break_ctl(tty, -1);
        return 0;

    case TIOCCBRK:  /* Turn break off, unconditionally */
        tty->driver.break_ctl(tty, 0);
        return 0;
    case TCSBRK:   /* SVID version: non-zero arg --> no break */
        /*
         * XXX is the above comment correct, or the
         * code below correct?  Is this ioctl used at
         * all by anyone?
         */
        if (!arg)
            return send_break(tty, HZ/4);
        return 0;
    case TCSBRKP:   /* support for POSIX tcsendbreak() */
        return send_break(tty, arg ? arg*(HZ/10) : HZ/4);
    }
    if (tty->driver.ioctl)
    {
        int retval = (tty->driver.ioctl)(tty, file, cmd, arg);
        if (retval != -ENOIOCTLCMD)
            return retval;
    }
    if (tty->ldisc.ioctl)
    {
        int retval = (tty->ldisc.ioctl)(tty, file, cmd, arg);
        if (retval != -ENOIOCTLCMD)
            return retval;
    }
    return -EINVAL;
}

/*
 * Routine which returns the baud rate of the tty
 *
 * Note that the baud_table needs to be kept in sync with the
 * include/asm/termbits.h file.
 */
static int baud_table[] =
{
    0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800,
    9600, 19200, 38400, 57600, 115200, 230400, 460800,
#ifdef __sparc__
    76800, 153600, 307200, 614400, 921600
#else
    500000, 576000, 921600, 1000000, 1152000, 1500000, 2000000,
    2500000, 3000000, 3500000, 4000000
#endif
};

/*
 * This subroutine initializes a tty structure.
 */
static void initialize_tty_struct(struct tty_struct *tty)
{
    memset(tty, 0, sizeof(struct tty_struct));
    tty->magic = TTY_MAGIC;
    tty->ldisc = ldiscs[N_TTY];
    tty->pgrp = -1;
    tty->flip.char_buf_ptr = tty->flip.char_buf;
    tty->flip.flag_buf_ptr = tty->flip.flag_buf;
    tty->flip.tqueue.routine = 0;
    tty->flip.tqueue.data = tty;
    init_MUTEX(&tty->flip.pty_sem);
    init_waitqueue_head(&tty->write_wait);
    init_waitqueue_head(&tty->read_wait);
    tty->tq_hangup.routine = 0;
    tty->tq_hangup.data = tty;
    sema_init(&tty->atomic_read, 1);
    sema_init(&tty->atomic_write, 1);
    spin_lock_init(&tty->read_lock);
    INIT_LIST_HEAD(&tty->tty_files);
    INIT_TQUEUE(&tty->SAK_tq, 0, 0);
}

/*
 * The default put_char routine if the driver did not define one.
 */
void tty_default_put_char(struct tty_struct *tty, unsigned char ch)
{
    tty->driver.write(tty, 0, &ch, 1);
}

/*
 * Register a tty device described by <driver>, with minor number <minor>.
 */
void tty_register_devfs (struct tty_driver *driver, unsigned int flags, unsigned minor)
{
#ifdef CONFIG_DEVFS_FS
    umode_t mode = S_IFCHR | S_IRUSR | S_IWUSR;
    kdev_t device = MKDEV (driver->major, minor);
    int idx = minor - driver->minor_start;
    char buf[32];

    switch (device)
    {
    case TTY_DEV:
    case PTMX_DEV:
        mode |= S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
        break;
    default:
        if (driver->major == PTY_MASTER_MAJOR)
            mode |= S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
        break;
    }
    if ( (minor <  driver->minor_start) ||
            (minor >= driver->minor_start + driver->num) )
    {
        printk(KERN_ERR "Attempt to register invalid minor number "
               "with devfs (%d:%d).\n", (int)driver->major,(int)minor);
        return;
    }
#  ifdef CONFIG_UNIX98_PTYS
    if ( (driver->major >= UNIX98_PTY_SLAVE_MAJOR) &&
            (driver->major < UNIX98_PTY_SLAVE_MAJOR + UNIX98_NR_MAJORS) )
        flags |= DEVFS_FL_CURRENT_OWNER;
#  endif
    sprintf(buf, driver->name, idx + driver->name_base);
    devfs_register (NULL, buf, flags | DEVFS_FL_DEFAULT,
                    driver->major, minor, mode, &tty_fops, NULL);
#endif /* CONFIG_DEVFS_FS */
}

void tty_unregister_devfs (struct tty_driver *driver, unsigned minor)
{
#ifdef CONFIG_DEVFS_FS
    void * handle;
    int idx = minor - driver->minor_start;
    char buf[32];

    sprintf(buf, driver->name, idx + driver->name_base);
    handle = devfs_find_handle (NULL, buf, driver->major, minor,
                                DEVFS_SPECIAL_CHR, 0);
    devfs_unregister (handle);
#endif /* CONFIG_DEVFS_FS */
}

EXPORT_SYMBOL(tty_register_devfs);
EXPORT_SYMBOL(tty_unregister_devfs);

/*
 * Called by a tty driver to register itself.
 */
int tty_register_driver(struct tty_driver *driver)
{
    int error = 0;
    int i;

    if (driver->flags & TTY_DRIVER_INSTALLED)
        return 0;

#if 0
    error = devfs_register_chrdev(driver->major, driver->name, &tty_fops);
#endif
    if (error < 0)
        return error;
    else if(driver->major == 0)
        driver->major = error;

    if (!driver->put_char)
        driver->put_char = tty_default_put_char;

    driver->prev = 0;
    driver->next = tty_drivers;
    if (tty_drivers) tty_drivers->prev = driver;
    tty_drivers = driver;

#if 0
    if ( !(driver->flags & TTY_DRIVER_NO_DEVFS) )
    {
        for(i = 0; i < driver->num; i++)
            tty_register_devfs(driver, 0, driver->minor_start + i);
    }
    proc_tty_register_driver(driver);
#endif
    return error;
}

/*
 * Called by a tty driver to unregister itself.
 */
int tty_unregister_driver(struct tty_driver *driver)
{
    int retval;
    struct tty_driver *p;
    int i, found = 0;
    struct termios *tp;
    const char *othername = NULL;

    if (*driver->refcount)
        return -EBUSY;

    for (p = tty_drivers; p; p = p->next)
    {
        if (p == driver)
            found++;
        else if (p->major == driver->major)
            othername = p->name;
    }

    if (!found)
        return -ENOENT;

#if 0
    if (othername == NULL)
    {
        retval = devfs_unregister_chrdev(driver->major, driver->name);
        if (retval)
            return retval;
    }
    else
        devfs_register_chrdev(driver->major, othername, &tty_fops);
#endif

    if (driver->prev)
        driver->prev->next = driver->next;
    else
        tty_drivers = driver->next;

    if (driver->next)
        driver->next->prev = driver->prev;

    /*
     * Free the termios and termios_locked structures because
     * we don't want to get memory leaks when modular tty
     * drivers are removed from the kernel.
     */
    for (i = 0; i < driver->num; i++)
    {
        tp = driver->termios[i];
        if (tp)
        {
            driver->termios[i] = NULL;
            kfree(tp);
        }
        tp = driver->termios_locked[i];
        if (tp)
        {
            driver->termios_locked[i] = NULL;
            kfree(tp);
        }
#if 0
        tty_unregister_devfs(driver, driver->minor_start + i);
#endif
    }
#if 0
    proc_tty_unregister_driver(driver);
#endif
    return 0;
}


/*
 * Initialize the console device. This is called *early*, so
 * we can't necessarily depend on lots of kernel help here.
 * Just do some early initializations, and do the complex setup
 * later.
 */
void __init console_init(void)
{
    /* Setup the default TTY line discipline. */
    memset(ldiscs, 0, sizeof(ldiscs));

    /*
     * Set up the standard termios.  Individual tty drivers may
     * deviate from this; this is used as a template.
     */
    memset(&tty_std_termios, 0, sizeof(struct termios));
    memcpy(tty_std_termios.c_cc, INIT_C_CC, NCCS);
    tty_std_termios.c_iflag = ICRNL | IXON;
    tty_std_termios.c_oflag = OPOST | ONLCR;
    tty_std_termios.c_cflag = B38400 | CS8 | CREAD | HUPCL;
    tty_std_termios.c_lflag = ISIG | ICANON | ECHO | ECHOE | ECHOK |
                              ECHOCTL | ECHOKE | IEXTEN;

    /*
     * set up the console device so that later boot sequences can
     * inform about problems etc..
     */
#ifdef CONFIG_VT
    con_init();
#else
#endif
#ifdef CONFIG_SERIAL_CONSOLE
#if defined(CONFIG_SERIAL)
#if 0
    serial_console_init();
#endif
#endif /* CONFIG_8xx */
#endif
#ifdef CONFIG_STDIO_CONSOLE
    stdio_console_init();
#endif
}

static struct tty_driver dev_tty_driver, dev_syscons_driver;
#ifdef CONFIG_UNIX98_PTYS
static struct tty_driver dev_ptmx_driver;
#endif

/*
 * Ok, now we can initialize the rest of the tty devices and can count
 * on memory allocations, interrupts etc..
 */
void __init tty_init(void)
{
    /*
     * dev_tty_driver and dev_console_driver are actually magic
     * devices which get redirected at open time.  Nevertheless,
     * we register them so that register_chrdev is called
     * appropriately.
     */
    memset(&dev_tty_driver, 0, sizeof(struct tty_driver));
    dev_tty_driver.magic = TTY_DRIVER_MAGIC;
    dev_tty_driver.driver_name = "/dev/tty";
    dev_tty_driver.name = dev_tty_driver.driver_name + 5;
    dev_tty_driver.name_base = 0;
    dev_tty_driver.major = TTYAUX_MAJOR;
    dev_tty_driver.minor_start = 0;
    dev_tty_driver.num = 1;
    dev_tty_driver.type = TTY_DRIVER_TYPE_SYSTEM;
    dev_tty_driver.subtype = SYSTEM_TYPE_TTY;

    if (tty_register_driver(&dev_tty_driver))
        panic("Couldn't register /dev/tty driver\n");

    dev_syscons_driver = dev_tty_driver;
    dev_syscons_driver.driver_name = "/dev/console";
    dev_syscons_driver.name = dev_syscons_driver.driver_name + 5;
    dev_syscons_driver.major = TTYAUX_MAJOR;
    dev_syscons_driver.minor_start = 1;
    dev_syscons_driver.type = TTY_DRIVER_TYPE_SYSTEM;
    dev_syscons_driver.subtype = SYSTEM_TYPE_SYSCONS;

    if (tty_register_driver(&dev_syscons_driver))
        panic("Couldn't register /dev/console driver\n");

    /* console calls tty_register_driver() before kmalloc() works.
     * Thus, we can't devfs_register() then.  Do so now, instead.
     */
#ifdef CONFIG_VT
    con_init_devfs();
#endif

    con_vmsinit();

    kbd_vmsinit();

#ifdef CONFIG_UNIX98_PTYS
    dev_ptmx_driver = dev_tty_driver;
    dev_ptmx_driver.driver_name = "/dev/ptmx";
    dev_ptmx_driver.name = dev_ptmx_driver.driver_name + 5;
    dev_ptmx_driver.major= MAJOR(PTMX_DEV);
    dev_ptmx_driver.minor_start = MINOR(PTMX_DEV);
    dev_ptmx_driver.type = TTY_DRIVER_TYPE_SYSTEM;
    dev_ptmx_driver.subtype = SYSTEM_TYPE_SYSPTMX;

    if (tty_register_driver(&dev_ptmx_driver))
        panic("Couldn't register /dev/ptmx driver\n");
#endif

    kbd_init();
}

#if 1
__initcall(tty_init);
#endif
