/*
 * This file contains the procedures for the handling of select and poll
 *
 * Created for Linux based loosely upon Mathius Lattner's minix
 * patches by Peter MacDonald. Heavily edited by Linus.
 *
 *  4 February 1994
 *     COFF/ELF binary emulation. If the process has the STICKY_TIMEOUTS
 *     flag set in its personality we do *not* modify the given timeout
 *     parameter to reflect time remaining.
 *
 *  24 January 2000
 *     Changed sys_poll()/do_poll() to use PAGE_SIZE chunk-based allocation
 *     of fds to overcome nfds < 16390 descriptors limit (Tigran Aivazian).
 */

#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/poll.h>
#include <linux/personality.h> /* for STICKY_TIMEOUTS */
#include <linux/file.h>

#include <asm/uaccess.h>

#define ROUND_UP(x,y) (((x)+(y)-1)/(y))
#define DEFAULT_POLLMASK (POLLIN | POLLOUT | POLLRDNORM | POLLWRNORM)

struct poll_table_entry
{
    struct file * filp;
    wait_queue_t wait;
    wait_queue_head_t * wait_address;
};

struct poll_table_page
{
    struct poll_table_page * next;
    struct poll_table_entry * entry;
    struct poll_table_entry entries[0];
};

#define POLL_TABLE_FULL(table) \
    ((unsigned long)((table)->entry+1) > PAGE_SIZE + (unsigned long)(table))

/*
 * Ok, Peter made a complicated, but straightforward multiple_wait() function.
 * I have rewritten this, taking some shortcuts: This code may not be easy to
 * follow, but it should be free of race-conditions, and it's practical. If you
 * understand what I'm doing here, then you understand how the linux
 * sleep/wakeup mechanism works.
 *
 * Two very simple procedures, poll_wait() and poll_freewait() make all the
 * work.  poll_wait() is an inline-function defined in <linux/poll.h>,
 * as all select/poll functions have to call it to add an entry to the
 * poll table.
 */

void poll_freewait(poll_table* pt)
{
    struct poll_table_page * p = pt->table;
    while (p)
    {
        struct poll_table_entry * entry;
        struct poll_table_page *old;

        entry = p->entry;
        do
        {
            entry--;
            remove_wait_queue(entry->wait_address,&entry->wait);
            fput(entry->filp);
        }
        while (entry > p->entries);
        old = p;
        p = p->next;
        free_page((unsigned long) old);
    }
}

void __pollwait(struct file * filp, wait_queue_head_t * wait_address, poll_table *p)
{
    struct poll_table_page *table = p->table;

    if (!table || POLL_TABLE_FULL(table))
    {
        struct poll_table_page *new_table;

        new_table = (struct poll_table_page *) __get_free_page(GFP_KERNEL);
        if (!new_table)
        {
            p->error = -ENOMEM;
            __set_current_state(TASK_RUNNING);
            return;
        }
        new_table->entry = new_table->entries;
        new_table->next = table;
        p->table = new_table;
        table = new_table;
    }

    /* Add a new entry */
    {
        struct poll_table_entry * entry = table->entry;
        table->entry = entry+1;
        get_file(filp);
        entry->filp = filp;
        entry->wait_address = wait_address;
        init_waitqueue_entry(&entry->wait, current);
        add_wait_queue(wait_address,&entry->wait);
    }
}

#define __IN(fds, n)        (fds->in + n)
#define __OUT(fds, n)       (fds->out + n)
#define __EX(fds, n)        (fds->ex + n)
#define __RES_IN(fds, n)    (fds->res_in + n)
#define __RES_OUT(fds, n)   (fds->res_out + n)
#define __RES_EX(fds, n)    (fds->res_ex + n)

#define BITS(fds, n)        (*__IN(fds, n)|*__OUT(fds, n)|*__EX(fds, n))

static int max_select_fd(unsigned long n, fd_set_bits *fds)
{
    unsigned long *open_fds;
    unsigned long set;
    int max;

    /* handle last in-complete long-word first */
    set = ~(~0UL << (n & (__NFDBITS-1)));
    n /= __NFDBITS;
    open_fds = current->files->open_fds->fds_bits+n;
    max = 0;
    if (set)
    {
        set &= BITS(fds, n);
        if (set)
        {
            if (!(set & ~*open_fds))
                goto get_max;
            return -EBADF;
        }
    }
    while (n)
    {
        open_fds--;
        n--;
        set = BITS(fds, n);
        if (!set)
            continue;
        if (set & ~*open_fds)
            return -EBADF;
        if (max)
            continue;
get_max:
        do
        {
            max++;
            set >>= 1;
        }
        while (set);
        max += n * __NFDBITS;
    }

    return max;
}

#define BIT(i)      (1UL << ((i)&(__NFDBITS-1)))
#define MEM(i,m)    ((m)+(unsigned)(i)/__NFDBITS)
#define ISSET(i,m)  (((i)&*(m)) != 0)
#define SET(i,m)    (*(m) |= (i))

#define POLLIN_SET (POLLRDNORM | POLLRDBAND | POLLIN | POLLHUP | POLLERR)
#define POLLOUT_SET (POLLWRBAND | POLLWRNORM | POLLOUT | POLLERR)
#define POLLEX_SET (POLLPRI)

int do_select(int n, fd_set_bits *fds, long *timeout)
{
    poll_table table, *wait;
    int retval, i, off;
    long __timeout = *timeout;

    read_lock(&current->files->file_lock);
    retval = max_select_fd(n, fds);
    read_unlock(&current->files->file_lock);

    if (retval < 0)
        return retval;
    n = retval;

    poll_initwait(&table);
    wait = &table;
    if (!__timeout)
        wait = NULL;
    retval = 0;
    for (;;)
    {
        set_current_state(TASK_INTERRUPTIBLE);
        for (i = 0 ; i < n; i++)
        {
            unsigned long bit = BIT(i);
            unsigned long mask;
            struct file *file;

            off = i / __NFDBITS;
            if (!(bit & BITS(fds, off)))
                continue;
            file = fget(i);
            mask = POLLNVAL;
            if (file)
            {
                mask = DEFAULT_POLLMASK;
                if (file->f_op && file->f_op->poll)
                    mask = file->f_op->poll(file, wait);
                fput(file);
            }
            if ((mask & POLLIN_SET) && ISSET(bit, __IN(fds,off)))
            {
                SET(bit, __RES_IN(fds,off));
                retval++;
                wait = NULL;
            }
            if ((mask & POLLOUT_SET) && ISSET(bit, __OUT(fds,off)))
            {
                SET(bit, __RES_OUT(fds,off));
                retval++;
                wait = NULL;
            }
            if ((mask & POLLEX_SET) && ISSET(bit, __EX(fds,off)))
            {
                SET(bit, __RES_EX(fds,off));
                retval++;
                wait = NULL;
            }
        }
        wait = NULL;
        if (retval || !__timeout || signal_pending(current))
            break;
        if(table.error)
        {
            retval = table.error;
            break;
        }
        __timeout = schedule_timeout(__timeout);
    }
    current->state = TASK_RUNNING;

    poll_freewait(&table);

    /*
     * Up-to-date the caller timeout.
     */
    *timeout = __timeout;
    return retval;
}

asmlinkage long sys_select(int n, fd_set *inp, fd_set *outp, fd_set *exp, struct timeval *tvp)
{
    printk("no select\n");
    return -EINVAL;
}

#define POLLFD_PER_PAGE  ((PAGE_SIZE) / sizeof(struct pollfd))

static void do_pollfd(unsigned int num, struct pollfd * fdpage,
                      poll_table ** pwait, int *count)
{
    int i;

    for (i = 0; i < num; i++)
    {
        int fd;
        unsigned int mask;
        struct pollfd *fdp;

        mask = 0;
        fdp = fdpage+i;
        fd = fdp->fd;
        if (fd >= 0)
        {
            struct file * file = fget(fd);
            mask = POLLNVAL;
            if (file != NULL)
            {
                mask = DEFAULT_POLLMASK;
                if (file->f_op && file->f_op->poll)
                    mask = file->f_op->poll(file, *pwait);
                mask &= fdp->events | POLLERR | POLLHUP;
                fput(file);
            }
            if (mask)
            {
                *pwait = NULL;
                (*count)++;
            }
        }
        fdp->revents = mask;
    }
}

static int do_poll(unsigned int nfds, unsigned int nchunks, unsigned int nleft,
                   struct pollfd *fds[], poll_table *wait, long timeout)
{
    int count;
    poll_table* pt = wait;

    for (;;)
    {
        unsigned int i;

        set_current_state(TASK_INTERRUPTIBLE);
        count = 0;
        for (i=0; i < nchunks; i++)
            do_pollfd(POLLFD_PER_PAGE, fds[i], &pt, &count);
        if (nleft)
            do_pollfd(nleft, fds[nchunks], &pt, &count);
        pt = NULL;
        if (count || !timeout || signal_pending(current))
            break;
        count = wait->error;
        if (count)
            break;
        timeout = schedule_timeout(timeout);
    }
    current->state = TASK_RUNNING;
    return count;
}

asmlinkage long sys_poll(struct pollfd * ufds, unsigned int nfds, long timeout)
{
    printk("no select\n");
    return -EINVAL;
}
