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

#include <stdio.h>
#include <linux/string.h>
//#include <unistd.h>
#include <fcntl.h>

//#include "phyio.h"
//#include "ssdef.h"
#include "../../freevms/starlet/src/ssdef.h"

struct phyio_info {
  unsigned status;
  unsigned sectors;
  unsigned sectorsize;
};

#if defined(__digital__) && defined(__unix__)
#define DEV_PREFIX "/devices/rdisk/%s"
#else
#ifdef sun
#define DEV_PREFIX "/dev/dsk/%s"
#else
#define DEV_PREFIX "/dev/%s"
#endif
#endif

unsigned init_count = 0;
unsigned read_count = 0;
unsigned write_count = 0;

void phyio_show(void)
{
    printk("PHYIO_SHOW Initializations: %d Reads: %d Writes: %d\n",
           init_count,read_count,write_count);
}

unsigned phyio_init(int devlen,char *devnam,unsigned *handle,struct phyio_info *info)
{
    int vmsfd;
    char *cp,devbuf[200];
    init_count++;
    info->status = 0;           /* We don't know anything about this device! */
    info->sectors = 0;
    info->sectorsize = 0;
    sprintf(devbuf,DEV_PREFIX,devnam);
    cp = strchr(devbuf,':');
    if (cp != NULL) *cp = '\0';
    vmsfd = sys_open(devbuf,O_RDWR);
    if (vmsfd < 0) vmsfd = sys_open(devbuf,O_RDONLY);
    if (vmsfd < 0) return SS$_NOSUCHDEV;
    *handle = vmsfd;
    return SS$_NORMAL;
}

unsigned phyio_close(unsigned handle)
{
    sys_close(handle);
    return SS$_NORMAL;
}


unsigned phyio_read(unsigned handle,unsigned block,unsigned length,char *buffer)
{
    int res;
#ifdef DEBUG
    printk("Phyio read block: %d into %x (%d bytes)\n",block,buffer,length);
#endif
    read_count++;
    if ((res = sys_lseek(handle,block*512,0)) < 0) {
        printk("lseek ");
	printk("sys_lseek failed %d\n",res);
        return SS$_PARITY;
    }
    if ((res = sys_read(handle,buffer,length)) != length) {
        printk("read ");
	printk("read failed %d\n",res);
        return SS$_PARITY;
    }
    return SS$_NORMAL;
}


unsigned phyio_write(unsigned handle,unsigned block,unsigned length,char *buffer)
{
#ifdef DEBUG
    printk("Phyio write block: %d from %x (%d bytes)\n",block,buffer,length);
#endif
    write_count++;
    if (sys_lseek(handle,block*512,0) < 0) return SS$_PARITY;
    if (sys_write(handle,buffer,length) != length) return SS$_PARITY;
    return SS$_NORMAL;
}
