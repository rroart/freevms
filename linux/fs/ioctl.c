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
