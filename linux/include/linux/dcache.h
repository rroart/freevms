#ifndef __LINUX_DCACHE_H
#define __LINUX_DCACHE_H

#ifdef __KERNEL__

#include <asm/atomic.h>
#include <linux/mount.h>

/*
 * linux/include/linux/dcache.h
 *
 * Dirent cache data structures
 *
 * (C) Copyright 1997 Thomas Schoebel-Theuer,
 * with heavy changes by Linus Torvalds
 */

#define IS_ROOT(x) ((x) == (x)->d_parent)

/*
 * "quick string" -- eases parameter passing, but more importantly
 * saves "metadata" about the string (ie length and the hash).
 */
struct qstr
{
    const unsigned char * name;
    unsigned int len;
    unsigned int hash;
};

struct dentry_stat_t
{
    int nr_dentry;
    int nr_unused;
    int age_limit;          /* age in seconds */
    int want_pages;         /* pages requested by system */
    int dummy[2];
};
extern struct dentry_stat_t dentry_stat;

#define DNAME_INLINE_LEN 16

struct dentry
{
    atomic_t d_count;
    unsigned int d_flags;
    struct inode  * d_inode;    /* Where the name belongs to - NULL is negative */
    struct dentry * d_parent;   /* parent directory */
    struct list_head d_hash;    /* lookup hash list */
    struct list_head d_lru;     /* d_count = 0 LRU list */
    struct list_head d_child;   /* child of parent list */
    struct list_head d_subdirs; /* our children */
    struct list_head d_alias;   /* inode alias list */
    int d_mounted;
    struct qstr d_name;
    unsigned long d_time;       /* used by d_revalidate */
    struct dentry_operations  *d_op;
    struct super_block * d_sb;  /* The root of the dentry tree */
    unsigned long d_vfs_flags;
    void * d_fsdata;        /* fs-specific data */
    unsigned char d_iname[DNAME_INLINE_LEN]; /* small names */
};

struct dentry_operations
{
    int (*d_revalidate)(struct dentry *, int);
    int (*d_hash) (struct dentry *, struct qstr *);
    int (*d_compare) (struct dentry *, struct qstr *, struct qstr *);
    int (*d_delete)(struct dentry *);
    void (*d_release)(struct dentry *);
    void (*d_iput)(struct dentry *, struct inode *);
};

/* the dentry parameter passed to d_hash and d_compare is the parent
 * directory of the entries to be compared. It is used in case these
 * functions need any directory specific information for determining
 * equivalency classes.  Using the dentry itself might not work, as it
 * might be a negative dentry which has no information associated with
 * it */

/*
locking rules:
        big lock    dcache_lock may block
d_revalidate:   no      no      yes
d_hash      no      no      yes
d_compare:  no      yes     no
d_delete:   no      yes     no
d_release:  no      no      yes
d_iput:     no      no      yes
 */

/* d_flags entries */
#define DCACHE_AUTOFS_PENDING 0x0001    /* autofs: "under construction" */
#define DCACHE_NFSFS_RENAMED  0x0002    /* this dentry has been "silly
* renamed" and has to be
* deleted on the last dput()
*/
#define DCACHE_NFSD_DISCONNECTED 0x0004 /* This dentry is not currently connected to the
* dcache tree. Its parent will either be itself,
* or will have this flag as well.
* If this dentry points to a directory, then
* s_nfsd_free_path semaphore will be down
*/
#define DCACHE_REFERENCED   0x0008  /* Recently used, don't discard. */

/**
 *   dget, dget_locked   -   get a reference to a dentry
 *   @dentry: dentry to get a reference to
 *
 *   Given a dentry or %NULL pointer increment the reference count
 *   if appropriate and return the dentry. A dentry will not be
 *   destroyed when it has references. dget() should never be
 *   called for dentries with zero reference counter. For these cases
 *   (preferably none, functions in dcache.c are sufficient for normal
 *   needs and they take necessary precautions) you should hold dcache_lock
 *   and call dget_locked() instead of dget().
 */

static __inline__ struct dentry * dget(struct dentry *dentry)
{
    if (dentry)
    {
        if (!atomic_read(&dentry->d_count))
            BUG();
        atomic_inc(&dentry->d_count);
    }
    return dentry;
}

extern void dput(struct dentry *);

#endif /* __KERNEL__ */

#endif  /* __LINUX_DCACHE_H */
