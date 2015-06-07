/*
 * Definitions for diskquota-operations. When diskquota is configured these
 * macros expand to the right source-code.
 *
 * Author:  Marco van Wieringen <mvw@planets.elm.net>
 *
 * Version: $Id$
 *
 */
#ifndef _LINUX_QUOTAOPS_
#define _LINUX_QUOTAOPS_

#include <linux/smp_lock.h>

/*
 * NO-OP when quota not configured.
 */
#define DQUOT_INIT(inode)           do { } while(0)
#define DQUOT_DROP(inode)           do { } while(0)
#define DQUOT_ALLOC_INODE(inode)        (0)
#define DQUOT_FREE_INODE(inode)         do { } while(0)
#define DQUOT_SYNC(dev)             do { } while(0)
#define DQUOT_OFF(sb)               do { } while(0)
#define DQUOT_TRANSFER(inode, iattr)        (0)
extern __inline__ int DQUOT_PREALLOC_BLOCK_NODIRTY(struct inode *inode, int nr)
{
    lock_kernel();
    inode->i_blocks += nr << (inode->i_sb->s_blocksize_bits - 9);
    unlock_kernel();
    return 0;
}

extern __inline__ int DQUOT_PREALLOC_BLOCK(struct inode *inode, int nr)
{
    DQUOT_PREALLOC_BLOCK_NODIRTY(inode, nr);
    mark_inode_dirty(inode);
    return 0;
}

extern __inline__ int DQUOT_ALLOC_BLOCK_NODIRTY(struct inode *inode, int nr)
{
    lock_kernel();
    inode->i_blocks += nr << (inode->i_sb->s_blocksize_bits - 9);
    unlock_kernel();
    return 0;
}

extern __inline__ int DQUOT_ALLOC_BLOCK(struct inode *inode, int nr)
{
    DQUOT_ALLOC_BLOCK_NODIRTY(inode, nr);
    mark_inode_dirty(inode);
    return 0;
}

extern __inline__ void DQUOT_FREE_BLOCK_NODIRTY(struct inode *inode, int nr)
{
    lock_kernel();
    inode->i_blocks -= nr << (inode->i_sb->s_blocksize_bits - 9);
    unlock_kernel();
}

extern __inline__ void DQUOT_FREE_BLOCK(struct inode *inode, int nr)
{
    DQUOT_FREE_BLOCK_NODIRTY(inode, nr);
    mark_inode_dirty(inode);
}

#endif /* _LINUX_QUOTAOPS_ */
