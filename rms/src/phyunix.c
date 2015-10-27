// $Id$
// $Locker$

// Author. Paul Nankervis.
// Author. Roar Thron�s.

/* PHYVMS.c v1.3    Physical I/O module for Unix */

/*
 This is part of ODS2 written by Paul Nankervis,
 email address:  Paulnank@au1.ibm.com

 ODS2 is distributed freely for all members of the
 VMS community to use. However all derived works
 must maintain comments in their source to acknowledge
 the contibution of the original author.
 */

/*
 If the user mounts  cd0   we open up /dev/cd0 for access.
 */

#include <linux/config.h>
#include <linux/errno.h>
#include <linux/kernel.h>

#include <asm/uaccess.h>

#include <phyio.h>
#include <ssdef.h>
#include <ucbdef.h>

#define DEV_PREFIX "/dev/%s"

unsigned init_count = 0;
unsigned read_count = 0;
unsigned write_count = 0;

void phyio_show(void)
{
    printk("PHYIO_SHOW Initializations: %d Reads: %d Writes: %d\n", init_count, read_count, write_count);
}

int phyio_init(int devlen, char *devnam, struct file **handle, struct phyio_info *info, struct _dt_ucb * ucb)
{
    struct file * vmsfd;
    char *cp, devbuf[200];
    char *tmpname;
    init_count++;
    //    sprintf(devbuf,DEV_PREFIX,devnam);
    sprintf(devbuf, "%s", devnam);
    cp = strchr(devbuf, ':');
    if (cp != NULL)
        *cp = '\0';
    //    tmpname=getname(devnam);
    panic("no %s\n", devbuf);
    vmsfd = -1;
    //    putname(tmpname);
    if (IS_ERR(vmsfd))
        return SS$_NOSUCHDEV;
    *handle = vmsfd;
    ucb->ucb$l_maxblock = vmsfd->f_dentry->d_inode->i_size >> 9; // block size
    return SS$_NORMAL;
}

int phyio_close(struct file * handle)
{
    filp_close(handle, NULL);
    return SS$_NORMAL;
}

int phyio_read(struct file * handle, unsigned block, unsigned length, char *buffer)
{
    mm_segment_t fs;
#ifdef DEBUG
    printk("Phyio read block: %d into %x (%d bytes)\n",block,buffer,length);
#endif
    read_count++;

    fs = get_fs();
    set_fs(KERNEL_DS);
    if (generic_file_llseek(handle, block * 512, 0) < 0)
        goto error;
    if (handle->f_op->read(handle, buffer, length, &handle->f_pos) != length)
        goto error;
    set_fs(fs);

    return SS$_NORMAL;
    error: set_fs(fs);
    return SS$_PARITY;
}

int phyio_write(struct file * handle, unsigned block, unsigned length, char *buffer)
{
    mm_segment_t fs;
#ifdef DEBUG
    printk("Phyio write block: %d from %x (%d bytes)\n",block,buffer,length);
#endif
    write_count++;
    //    if (sys_lseek(handle,block*512,0) < 0) return SS$_PARITY;
    //    if (sys_write(handle,buffer,length) != length) return SS$_PARITY;

    fs = get_fs();
    set_fs(KERNEL_DS);
    if (generic_file_llseek(handle, block * 512, 0) < 0)
        goto error;
    if (handle->f_op->write(handle, buffer, length, &handle->f_pos) != length)
        goto error;
    set_fs(fs);

    return SS$_NORMAL;
    error: set_fs(fs);
    return SS$_PARITY;
}
