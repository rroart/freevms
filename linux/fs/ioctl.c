/*
 *  linux/fs/ioctl.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <linux/mm.h>
#include <linux/smp_lock.h>
#include <linux/file.h>

#include <asm/uaccess.h>
#include <asm/ioctls.h>

#include <vfddef.h>

static int file_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
    int error;
    int block;
    struct inode * inode = filp->f_dentry->d_inode;

    switch (cmd)
    {
    case FIBMAP:
    {
        struct address_space *mapping = inode->i_mapping;
        int res;
        /* do we support this mess? */
        if (!mapping->a_ops->bmap)
            return -EINVAL;
        if (!capable(CAP_SYS_RAWIO))
            return -EPERM;
        if ((error = get_user(block, (int *) arg)) != 0)
            return error;

        res = mapping->a_ops->bmap(mapping, block);
        return put_user(res, (int *) arg);
    }
    case FIGETBSZ:
        if (inode->i_sb == NULL)
            return -EBADF;
        return put_user(inode->i_sb->s_blocksize, (int *) arg);
    case FIONREAD:
        return put_user(inode->i_size - filp->f_pos, (int *) arg);
    }
    if (filp->f_op && filp->f_op->ioctl)
        return filp->f_op->ioctl(inode, filp, cmd, arg);
    return -ENOTTY;
}


asmlinkage long sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg)
{
    struct vms_fd * vms_fd = fget(fd);
    int cmu_ioctl(int, int, long);
    if (vms_fd->vfd$l_is_cmu)
        return cmu_ioctl(fd, cmd, arg);

    // temp workaround for bash
    int * a = arg;
    if (cmd == TIOCGPGRP)
        *a = current->pgrp;
    if (cmd == TIOCSPGRP)
        current->pgrp = *a;
    if (cmd == TCGETS)
        ((struct termios *)a)->c_lflag = ECHOCTL; // check
    return 0;
}
