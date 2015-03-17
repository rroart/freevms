// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified User Mode Linux source file, 2001-2004.

#include<ccbdef.h>
#include<crbdef.h>
#include<cdtdef.h>
#include<dcdef.h>
#include<ddtdef.h>
#include<dptdef.h>
#include<fdtdef.h>
#include<pdtdef.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ddbdef.h>
#include<ipldef.h>
#include<dyndef.h>
#include<ssdef.h>
#include<iodef.h>
#include<devdef.h>
#include<descrip.h>
#include<system_data_cells.h>
#include<ipl.h>
#include <ioc_routines.h>
#include<linux/vmalloc.h>

//#include<stdio.h>
#define SEEK_SET 0

static void ubd_handler(void);

static struct _irp * globali;
static struct _ucb * globalu;

void  startio3 (struct _irp * i, struct _ucb * u)
{
    ioc$reqcom(SS$_NORMAL,0,u);
    return;
};

void  startio2 (struct _irp * i, struct _ucb * u)
{
    u->ucb$l_fpc=startio3;
    exe$iofork(i,u);
    return;
}

static void ubd_intr2(int irq, void *dev, struct pt_regs *unused)
{
    struct _irp * i;
    struct _ucb * u;
    void (*func)();

    if (intr_blocked(20))
        return;
    regtrap(REG_INTR,20);
    setipl(20);
    ubd_handler();
    /* have to do this until we get things more in order */
    i=globali;
    u=globalu;

    func=u->ucb$l_fpc;
    func(i,u);
    myrei();
}

static struct _fdt ubd$fdt =
{
fdt$q_valid:
    IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
fdt$q_buffered:
    IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

/* more yet undefined dummies */
static void  startio ();
static void  unsolint (void) { };
static void  cancel (void) { };
static void  ioc_std$cancelio (void) { };
static void  regdump (void) { };
static void  diagbuf (void) { };
static void  errorbuf (void) { };
static void  unitinit (void) { };
static void  altstart (void) { };
static void  mntver (void) { };
static void  cloneducb (void) { };
static void  mntv_sssc (void) { };
static void  mntv_for (void) { };
static void  mntv_sqd (void) { };
static void  aux_storage (void) { };
static void  aux_routine (void) { };

struct _ddt ubd$ddt =
{
ddt$l_start:
    startio,
ddt$l_unsolint:
    unsolint,
ddt$l_fdt:
    &ubd$fdt,
ddt$l_cancel:
    cancel,
ddt$l_regdump:
    regdump,
ddt$l_diagbuf:
    diagbuf,
ddt$l_errorbuf:
    errorbuf,
ddt$l_unitinit:
    unitinit,
ddt$l_altstart:
    altstart,
ddt$l_mntver:
    mntver,
ddt$l_cloneducb:
    cloneducb,
    ddt$w_fdtsize: 0,
ddt$ps_mntv_sssc:
    mntv_sssc,
ddt$ps_mntv_for:
    mntv_for,
ddt$ps_mntv_sqd:
    mntv_sqd,
ddt$ps_aux_storage:
    aux_storage,
ddt$ps_aux_routine:
    aux_routine
};

void startio (struct _irp * i, struct _ucb * u)
{
    int m;
    int fd;
    switch (i->irp$v_fcode)
    {

    case IO$_READLBLK :
        fd=u->ucb$l_orb;
        lseek(fd,512*i->irp$l_qio_p3,SEEK_SET);
        read_ubd_fs(fd,i->irp$l_qio_p1,i->irp$l_qio_p2);
        return ioc$reqcom(SS$_NORMAL,0,u);
        break;
    case IO$_READPBLK :
        fd=u->ucb$l_orb;
        lseek(fd,512*i->irp$l_qio_p3,SEEK_SET);
        read_ubd_fs(fd,i->irp$l_qio_p1,i->irp$l_qio_p2);
        return ioc$reqcom(SS$_NORMAL,0,u);
        break;
    case IO$_READVBLK :
        printk("should not be here in startio\n");
        break;
    case IO$_WRITELBLK :
        fd=u->ucb$l_orb;
        lseek(fd,512*i->irp$l_qio_p3,SEEK_SET);
        write_ubd_fs(fd,i->irp$l_qio_p1,i->irp$l_qio_p2);
        return ioc$reqcom(SS$_NORMAL,0,u);
        break;
    case IO$_WRITEPBLK :
        fd=u->ucb$l_orb;
        lseek(fd,512*i->irp$l_qio_p3,SEEK_SET);
        write_ubd_fs(fd,i->irp$l_qio_p1,i->irp$l_qio_p2);
        return ioc$reqcom(SS$_NORMAL,0,u);
        break;
    case IO$_WRITEVBLK :
        printk("should not be here in startio\n");
        break;
    default:
        printk("ubc startio unknown %x\n",i->irp$v_fcode);
        break;
    }
}

int acp_std$access(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$modify(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$mount(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$access(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$deaccess(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$readblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$writeblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

extern void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type);

void ubd$struc_init (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb)
{
    ucb->ucb$b_flck=IPL$_IOLOCK8;
    ucb->ucb$b_dipl=IPL$_IOLOCK8;

    ucb->ucb$l_devchar = DEV$M_REC | DEV$M_AVL | DEV$M_CCL | DEV$M_FOD/*| DEV$M_OOV*/;

    ucb->ucb$l_devchar2 = DEV$M_NNM;
    ucb->ucb$b_devclass = DC$_MISC;
    ucb->ucb$b_devtype = DT$_TTYUNKN;
    ucb->ucb$w_devbufsiz = 132;

    ucb->ucb$l_devdepend = 99; // just something to fill

    // dropped the mutex stuff

    return;
}

void ubd$struc_reinit (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb)
{
    ddb->ddb$ps_ddt=&ubd$ddt;
    //dpt_store_isr(crb,nl_isr);
    return;
}

int ubd$unit_init (struct _idb * idb, struct _ucb * ucb)
{
    ucb->ucb$v_online = 0;
    //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

    // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
    // no adp or cram stuff

    // or ints etc

    ucb->ucb$v_online = 1;

    return SS$_NORMAL;
}

struct _dpt ubd$dpt;
struct _ddb ubd$ddb;
struct _ucb ubd$ucb;
struct _crb ubd$crb;

int ubd$init_tables()
{
    ini_dpt_name(&ubd$dpt, "DADRIVER");
    ini_dpt_adapt(&ubd$dpt, 0);
    ini_dpt_defunits(&ubd$dpt, 1);
    ini_dpt_ucbsize(&ubd$dpt,sizeof(struct _ucb));
    ini_dpt_struc_init(&ubd$dpt, ubd$struc_init);
    ini_dpt_struc_reinit(&ubd$dpt, ubd$struc_reinit);
    ini_dpt_ucb_crams(&ubd$dpt, 1/*NUMBER_CRAMS*/);
    ini_dpt_end(&ubd$dpt);

    ini_ddt_unitinit(&ubd$ddt, ubd$unit_init);
    ini_ddt_start(&ubd$ddt, startio);
    ini_ddt_cancel(&ubd$ddt, ioc_std$cancelio);
    ini_ddt_end(&ubd$ddt);

    /* for the fdt init part */
    /* a lot of these? */
    ini_fdt_act(&ubd$fdt,IO$_ACCESS,acp_std$access,1);
    ini_fdt_act(&ubd$fdt,IO$_READLBLK,acp_std$readblk,1);
    ini_fdt_act(&ubd$fdt,IO$_READPBLK,acp_std$readblk,1);
    ini_fdt_act(&ubd$fdt,IO$_READVBLK,acp_std$readblk,1);
    ini_fdt_act(&ubd$fdt,IO$_WRITELBLK,acp_std$writeblk,1);
    ini_fdt_act(&ubd$fdt,IO$_WRITEPBLK,acp_std$writeblk,1);
    ini_fdt_act(&ubd$fdt,IO$_WRITEVBLK,acp_std$writeblk,1);
    ini_fdt_act(&ubd$fdt,IO$_CREATE,acp_std$access,1);
    ini_fdt_act(&ubd$fdt,IO$_DEACCESS,acp_std$deaccess,1);
    ini_fdt_act(&ubd$fdt,IO$_DELETE,acp_std$modify,1);
    ini_fdt_act(&ubd$fdt,IO$_MODIFY,acp_std$modify,1);
    ini_fdt_act(&ubd$fdt,IO$_ACPCONTROL,acp_std$modify,1);
    ini_fdt_act(&ubd$fdt,IO$_MOUNT,acp_std$mount,1);
    ini_fdt_end(&ubd$fdt);

    return SS$_NORMAL;
}

/*
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#define MAJOR_NR UBD_MAJOR
#include "linux/config.h"
#include "linux/blk.h"
#include "linux/blkdev.h"
#include "linux/hdreg.h"
#include "linux/init.h"
#include "linux/devfs_fs_kernel.h"
#include "linux/cdrom.h"
#include "linux/proc_fs.h"
#include "linux/ctype.h"
#include "linux/capability.h"
#include "linux/mm.h"
#include "linux/vmalloc.h"
#include "linux/blkpg.h"
#include "asm/segment.h"
#include "asm/uaccess.h"
#include "asm/irq.h"
#include "asm/types.h"
#include "user_util.h"
#include "mem_user.h"
#include "kern_util.h"
#include "kern.h"
#include "mconsole_kern.h"
#include "init.h"
#include "irq_user.h"
#include "ubd_user.h"
#include "2_5compat.h"

extern __u64 file_size(char *file);

static int ubd_open(struct inode * inode, struct file * filp);
static int ubd_release(struct inode * inode, struct file * file);
static int ubd_ioctl(struct inode * inode, struct file * file,
                     unsigned int cmd, unsigned long arg);

#define MAX_DEV (8)

static int blk_sizes[MAX_DEV] = { [ 0 ... MAX_DEV - 1 ] = BLOCK_SIZE };

static int hardsect_sizes[MAX_DEV] = { [ 0 ... MAX_DEV - 1 ] = 512 };

static int sizes[MAX_DEV] = { [ 0 ... MAX_DEV - 1 ] = 0 };

static struct block_device_operations ubd_blops =
{
open:
    ubd_open,
release:
    ubd_release,
ioctl:
    ubd_ioctl,
};

static struct hd_struct	ubd_part[MAX_DEV] =
    { [ 0 ... MAX_DEV - 1 ] = { 0, 0, 0 } };

static request_queue_t *ubd_queue;

static int fake_major = 0;

static struct gendisk ubd_gendisk = INIT_GENDISK(MAJOR_NR, "ubd", ubd_part,
                                    sizes, MAX_DEV, &ubd_blops);

static struct gendisk fake_gendisk = INIT_GENDISK(0, "ubd", ubd_part,
                                     sizes, MAX_DEV, &ubd_blops);

#ifdef CONFIG_BLK_DEV_UBD_SYNC
#define OPEN_FLAGS O_RDWR | O_SYNC
#else
#define OPEN_FLAGS O_RDWR
#endif

static int global_openflags = OPEN_FLAGS;

struct cow
{
    char *file;
    int fd;
    unsigned long *bitmap;
    unsigned long bitmap_len;
    int bitmap_offset;
    int data_offset;
};

struct ubd
{
    char *file;
    int is_dir;
    int count;
    int fd;
    __u64 size;
    int boot_openflags;
    int openflags;
    devfs_handle_t real;
    devfs_handle_t fake;
    struct cow cow;
};

#define DEFAULT_COW { \
	file:			NULL, \
        fd:			-1, \
        bitmap:			NULL, \
	bitmap_offset:		0, \
        data_offset:		0, \
}

#define DEFAULT_UBD { \
	file: 			NULL, \
	is_dir:			0, \
	count:			0, \
	fd:			-1, \
	size:			-1, \
	boot_openflags:		OPEN_FLAGS, \
	openflags:		OPEN_FLAGS, \
	real:			NULL, \
	fake:			NULL, \
        cow:			DEFAULT_COW, \
}

struct ubd ubd_dev[MAX_DEV] =
{
    {
file: 			"root_fs"
        ,
        is_dir:			0,
        count:			0,
fd:
        -1,
        size:			0,
boot_openflags:
        OPEN_FLAGS,
openflags:
        OPEN_FLAGS,
real:
        NULL,
fake:
        NULL,
cow:
        DEFAULT_COW,
    },
    [ 1 ... MAX_DEV - 1 ] = DEFAULT_UBD
};

static struct hd_driveid ubd_id =
{
    cyls:		0,
    heads:		128,
    sectors:	32,
};

static int fake_ide = 0;
static struct proc_dir_entry *proc_ide_root = NULL;
static struct proc_dir_entry *proc_ide = NULL;

static void make_proc_ide(void)
{
    proc_ide_root = proc_mkdir("ide", 0);
    proc_ide = proc_mkdir("ide0", proc_ide_root);
}

static int proc_ide_read_media(char *page, char **start, off_t off, int count,
                               int *eof, void *data)
{
    int len;

    strcpy(page, "disk\n");
    len = strlen("disk\n");
    len -= off;
    if (len < count)
    {
        *eof = 1;
        if (len <= 0) return 0;
    }
    else len = count;
    *start = page + off;
    return len;

}

static void make_ide_entries(char *dev_name)
{
    struct proc_dir_entry *dir, *ent;
    char name[64];

    if(!fake_ide) return;
    if(proc_ide_root == NULL) make_proc_ide();
    dir = proc_mkdir(dev_name, proc_ide);
    ent = create_proc_entry("media", S_IFREG|S_IRUGO, dir);
    if(!ent) return;
    ent->nlink = 1;
    ent->data = NULL;
    ent->read_proc = proc_ide_read_media;
    ent->write_proc = NULL;
    sprintf(name,"ide0/%s", dev_name);
    proc_symlink(dev_name, proc_ide_root, name);
}

static int fake_ide_setup(char *str)
{
    fake_ide = 1;
    return(1);
}

__setup("fake_ide", fake_ide_setup);

__uml_help(fake_ide_setup,
           "fake_ide\n"
           "    Create ide0 entries that map onto ubd devices.\n\n"
          );

static int ubd_setup_common(char *str, int *index_out)
{
    char *backing_file;
    int n, sync, perm = O_RDWR;

    if(index_out) *index_out = -1;
    n = *str++;
    if(n == '=')
    {
        char *end;
        int major;

        if(!strcmp(str, "sync"))
        {
            global_openflags |= O_SYNC;
            return(0);
        }
        major = simple_strtoul(str, &end, 0);
        if(*end != '\0')
        {
            printk(KERN_ERR
                   "ubd_setup : didn't parse major number\n");
            return(1);
        }
        fake_gendisk.major = major;
        fake_major = major;
        printk(KERN_INFO "Setting extra ubd major number to %d\n",
               major);
        return(0);
    }
    if(n < '0')
    {
        printk(KERN_ERR "ubd_setup : index out of range\n");
        return(1);
    }
    n -= '0';
    if(n >= MAX_DEV)
    {
        printk(KERN_ERR "ubd_setup : index out of range\n");
        return(1);
    }
    if(index_out) *index_out = n;
    sync = ubd_dev[n].boot_openflags & O_SYNC;
    if (*str == 'r')
    {
        perm = O_RDONLY;
        str++;
    }
    if (*str == 's')
    {
        sync = O_SYNC;
        str++;
    }
    if(*str++ != '=')
    {
        printk(KERN_ERR "ubd_setup : Expected '='\n");
        return(1);
    }
    backing_file = strchr(str, ',');
    if(backing_file)
    {
        *backing_file = '\0';
        backing_file++;
    }
    ubd_dev[n].file = str;
    ubd_dev[n].cow.file = backing_file;
    ubd_dev[n].boot_openflags = global_openflags | perm | sync;
    return(0);
}

static int ubd_setup(char *str)
{
    ubd_setup_common(str, NULL);
    return(1);
}

__setup("ubd", ubd_setup);
__uml_help(ubd_setup,
           "ubd<n>=<filename>\n"
           "    This is used to associate a device with a file in the underlying\n"
           "    filesystem. Usually, there is a filesystem in the file, but \n"
           "    that's not required. Swap devices containing swap files can be\n"
           "    specified like this. Also, a file which doesn't contain a\n"
           "    filesystem can have its contents read in the virtual \n"
           "    machine by running dd on the device. n must be in the range\n"
           "    0 to 7. Appending an 'r' to the number will cause that device\n"
           "    to be mounted read-only. For example ubd1r=./ext_fs. Appending\n"
           "    an 's' (has to be _after_ 'r', if there is one) will cause data\n"
           "    to be written to disk on the host immediately.\n\n"
          );

static int fakehd(char *str)
{
    printk(KERN_INFO
           "fakehd : Changing ubd_gendisk.major_name to \"hd\".\n");
    ubd_gendisk.major_name = "hd";
    return(1);
}

__setup("fakehd", fakehd);
__uml_help(fakehd,
           "fakehd\n"
           "    Change the ubd device name to \"hd\".\n\n"
          );

static void do_ubd_request(request_queue_t * q);

int thread_fd = -1;

int intr_count = 0;

static spinlock_t ubd_lock = SPIN_LOCK_UNLOCKED;

static void ubd_finish(int error)
{
    int nsect;

    if(error)
    {
        return;
    }
    nsect = CURRENT->current_nr_sectors;
    CURRENT->sector += nsect;
    CURRENT->buffer += nsect << 9;
    CURRENT->errors = 0;
    CURRENT->nr_sectors -= nsect;
    CURRENT->current_nr_sectors = 0;
}

static void ubd_handler(void)
{
    struct io_thread_req req;
    int n;

    DEVICE_INTR = NULL;
    intr_count++;
    n = read_ubd_fs(thread_fd, &req, sizeof(req));
    if(n != sizeof(req))
    {
        printk(KERN_ERR "Pid %d - spurious interrupt in ubd_handler, "
               "errno = %d\n", getpid(), -n);
        spin_lock(&REQUEST_LOCK);
        spin_unlock(&REQUEST_LOCK);
        return;
    }

    if((req.offset != ((__u64) (CURRENT->sector)) << 9) ||
            (req.length != (CURRENT->current_nr_sectors) << 9))
        panic("I/O op mismatch");

    spin_lock(&REQUEST_LOCK);
    ubd_finish(req.error);
    reactivate_fd(thread_fd);
    do_ubd_request(ubd_queue);
    spin_unlock(&REQUEST_LOCK);
}

static void ubd_intr(int irq, void *dev, struct pt_regs *unused)
{
    ubd_handler();
}

static int io_pid = -1;

void kill_io_thread(void)
{
    if(io_pid != -1) kill(io_pid, SIGKILL);
}

__uml_exitcall(kill_io_thread);

int sync = 0;

devfs_handle_t ubd_dir_handle;
devfs_handle_t ubd_fake_dir_handle;

static int ubd_add(int n)
{
    char name[sizeof("nnnnnn\0")], dev_name[sizeof("ubd0x")];

    if(ubd_dev[n].file == NULL) return(-1);
    sprintf(name, "%d", n);
    ubd_dev[n].real = devfs_register(ubd_dir_handle, name,
                                     DEVFS_FL_DEFAULT, MAJOR_NR, n,
                                     S_IFBLK | S_IRUSR | S_IWUSR |
                                     S_IRGRP |S_IWGRP,
                                     &ubd_blops, NULL);
    if(fake_major != 0)
    {
        ubd_dev[n].fake = devfs_register(ubd_fake_dir_handle, name,
                                         DEVFS_FL_DEFAULT, fake_major,
                                         n, S_IFBLK | S_IRUSR |
                                         S_IWUSR | S_IRGRP | S_IWGRP,
                                         &ubd_blops, NULL);
    }
    if(!strcmp(ubd_gendisk.major_name, "ubd"))
    {
        sprintf(dev_name, "%s%d", ubd_gendisk.major_name, n);
    }
    else sprintf(dev_name, "%s%c", ubd_gendisk.major_name,
                     n + 'a');
    make_ide_entries(dev_name);
    return(0);
}

static int ubd_config(char *str)
{
    int n, err;

    str = uml_strdup(str);
    if(str == NULL)
    {
        printk(KERN_ERR "ubd_config failed to strdup string\n");
        return(1);
    }
    err = ubd_setup_common(str, &n);
    if(err)
    {
        kfree(str);
        return(-1);
    }
    if(n != -1) ubd_add(n);
    return(0);
}

static int ubd_remove(char *str)
{
    int n;

    if(!isdigit(*str)) return(-1);
    n = *str - '0';
    if(ubd_dev[n].file == NULL) return(0);
    if(ubd_dev[n].count > 0) return(-1);
    if(ubd_dev[n].real != NULL) devfs_unregister(ubd_dev[n].real);
    if(ubd_dev[n].fake != NULL) devfs_unregister(ubd_dev[n].fake);
    ubd_dev[n] = ((struct ubd) DEFAULT_UBD);
    return(0);
}

static struct mc_device ubd_mc =
{
name:		"ubd"
    ,
config:
    ubd_config,
remove:
    ubd_remove,
};

int ubd_mc_init(void)
{
    mconsole_register_dev(&ubd_mc);
    return(0);
}

__initcall(ubd_mc_init);

static request_queue_t *ubd_get_queue(kdev_t device)
{
    return(ubd_queue);
}

long ubd_iodb_vmsinit(void)
{
#if 0
    struct _ucb * ucb=&ubd$ucb;
    struct _ddb * ddb=&ubd$ddb;
    struct _crb * crb=&ubd$crb;
#endif
    struct _ucb * ucb=kmalloc(sizeof(struct _ucb),GFP_KERNEL);
    struct _ddb * ddb=kmalloc(sizeof(struct _ddb),GFP_KERNEL);
    struct _crb * crb=kmalloc(sizeof(struct _crb),GFP_KERNEL);
    unsigned long idb=0,orb=0;
    struct _ddb * newddb;

//  ioc_std$clone_ucb(&ubd$ucb,&ucb);
    bzero(ucb,sizeof(struct _ucb));
    bzero(ddb,sizeof(struct _ddb));
    bzero(crb,sizeof(struct _crb));

#if 0
    init_ddb(&ubd$ddb,&ubd$ddt,&ubd$ucb,"daa");
    init_ucb(&ubd$ucb, &ubd$ddb, &ubd$ddt, &ubd$crb);
    init_crb(&ubd$crb);
#endif
    init_ddb(ddb,&ubd$ddt,ucb,"daa");
    init_ucb(ucb, ddb, &ubd$ddt, crb);
    init_crb(crb);

    ubd$init_tables();
    ubd$struc_init (crb, ddb, idb, orb, ucb);
    ubd$struc_reinit (crb, ddb, idb, orb, ucb);
    ubd$unit_init (idb, ucb);

    insertdevlist(ddb);

    return ddb;
}

long ubd_iodbunit_vmsinit(struct _ddb * ddb,int unitno,void * dsc)
{
    unsigned short chan;
    struct _ccb * ccb;
    struct _ucb * newucb;

    ubd_open_dev(&ubd_dev[unitno]);
    ioc_std$clone_ucb(ddb->ddb$ps_ucb/*&ubd$ucb*/,&newucb);
    exe$assign(dsc,&chan,0,0,0);
    registerdevchan(MKDEV(UBD_MAJOR,unitno),chan);
    ccb = &ctl$ga_ccb_table[chan];
    ccb->ccb$l_ucb->ucb$l_orb=ubd_dev[unitno].fd;

    return newucb;
}

int ubd_vmsinit(void)
{
    $DESCRIPTOR(u0,"daa0");
    $DESCRIPTOR(u1,"daa1");
    //struct _ucb * u=makeucbetc(&ddb,&ddt,&dpt,&fdt,"ubd0","ubdriver");

    struct _ddb * ddb = ubd_iodb_vmsinit();

    ubd_iodbunit_vmsinit(ddb, 0,&u0);
    ubd_iodbunit_vmsinit(ddb, 1,&u1);

//  return chan;
}

int ubd_init(void)
{
    unsigned long stack;
    int i, err;

    ubd_dir_handle = devfs_mk_dir (NULL, "ubd", NULL);
    if (devfs_register_blkdev(MAJOR_NR, "ubd", &ubd_blops))
    {
        printk(KERN_ERR "ubd: unable to get major %d\n", MAJOR_NR);
        return -1;
    }
    ubd_queue = BLK_DEFAULT_QUEUE(MAJOR_NR);
#if 0
    INIT_QUEUE(ubd_queue, DEVICE_REQUEST, &ubd_lock);
    INIT_ELV(ubd_queue, &ubd_queue->elevator);
#endif
    read_ahead[MAJOR_NR] = 8;		/* 8 sector (4kB) read-ahead */
    blksize_size[MAJOR_NR] = blk_sizes;
    blk_size[MAJOR_NR] = sizes;
    INIT_HARDSECT(hardsect_size, MAJOR_NR, hardsect_sizes);
    add_gendisk(&ubd_gendisk);
    if (fake_major != 0)
    {
        char name[sizeof("ubd_nnn\0")];

        snprintf(name, sizeof(name), "ubd_%d", fake_major);
        ubd_fake_dir_handle = devfs_mk_dir(NULL, name, NULL);
        if(devfs_register_blkdev(fake_major, "ubd", &ubd_blops))
        {
            printk(KERN_ERR "ubd: unable to get major %d\n",
                   fake_major);
            return -1;
        }
        blk_dev[fake_major].queue = ubd_get_queue;
        read_ahead[fake_major] = 8;	/* 8 sector (4kB) read-ahead */
        blksize_size[fake_major] = blk_sizes;
        INIT_HARDSECT(hardsect_size, fake_major, hardsect_sizes);
        add_gendisk(&fake_gendisk);
    }
    for(i=0; i<MAX_DEV; i++) ubd_add(i);
    if(sync)
    {
        printk(KERN_INFO "ubd : Synchronous mode\n");
        return(0);
    }
    stack = alloc_stack();
    io_pid = start_io_thread(stack + PAGE_SIZE - sizeof(void *),
                             &thread_fd);
    if(io_pid < 0)
    {
        printk(KERN_ERR
               "ubd : Failed to start I/O thread (errno = %d) - "
               "falling back to synchronous I/O\n", -io_pid);
        return(0);
    }
    err = um_request_irq(UBD_IRQ, thread_fd, ubd_intr, SA_INTERRUPT,
                         "ubd", ubd_dev);
    if(err != 0) printk(KERN_ERR
                            "um_request_irq failed - errno = %d\n", -err);
    ubd_vmsinit();

    return(err);
}

__initcall(ubd_init);

static void ubd_close(struct ubd *dev)
{
    close_fd(dev->fd);
    if(dev->cow.file != NULL)
    {
        close_fd(dev->cow.fd);
        vfree(dev->cow.bitmap);
        dev->cow.bitmap = NULL;
    }
}

static int ubd_open_dev(struct ubd *dev)
{
    int err, flags, n, create_cow, *create_ptr;

    create_cow = 0;
    create_ptr = (dev->cow.file != NULL) ? &create_cow : NULL;
    dev->fd = open_ubd_file(dev->file, &dev->openflags, &dev->cow.file,
                            &dev->cow.bitmap_offset, &dev->cow.bitmap_len,
                            &dev->cow.data_offset, create_ptr);

    if((dev->fd == -ENOENT) && create_cow)
    {
        n = dev - ubd_dev;
        dev->fd = create_cow_file(dev->file, dev->cow.file, 1 << 9,
                                  &dev->cow.bitmap_offset,
                                  &dev->cow.bitmap_len,
                                  &dev->cow.data_offset);
        if(dev->fd >= 0)
        {
            printk(KERN_INFO "Creating \"%s\" as COW file for "
                   "\"%s\"\n", dev->file, dev->cow.file);
        }
    }

    if(dev->fd < 0) return(dev->fd);

    if(dev->cow.file != NULL)
    {
        err = -ENOMEM;
        dev->cow.bitmap = (void *) vmalloc(dev->cow.bitmap_len);
        if(dev->cow.bitmap == NULL) goto error;
        flush_tlb_kernel_vm();

        err = read_cow_bitmap(dev->fd, dev->cow.bitmap,
                              dev->cow.bitmap_offset,
                              dev->cow.bitmap_len);
        if(err) goto error;

        flags = O_RDONLY;
        err = open_ubd_file(dev->cow.file, &flags, NULL, NULL, NULL,
                            NULL, NULL);
        if(err < 0) goto error;
        dev->cow.fd = err;
    }
    return(0);
error:
    close_fd(dev->fd);
    return(err);
}

static int ubd_open(struct inode * inode, struct file * filp)
{
    char *file;
    int n;

    n = minor(inode->i_rdev);
    if(n > MAX_DEV)
        return -ENODEV;
    if(ubd_is_dir(ubd_dev[n].file))
    {
        ubd_dev[n].is_dir = 1;
        return(0);
    }
    if(ubd_dev[n].count == 0)
    {
        ubd_dev[n].openflags = ubd_dev[n].boot_openflags;
        /* XXX This error is wrong when errno isn't stored in
         * ubd_dev[n].fd
         */
        if(ubd_open_dev(&ubd_dev[n]) < 0)
        {
            printk(KERN_ERR "ubd%d: Can't open \"%s\": "
                   "errno = %d\n", n, ubd_dev[n].file,
                   -ubd_dev[n].fd);
        }
        if(ubd_dev[n].fd < 0)
            return -ENODEV;
        file = ubd_dev[n].cow.file ? ubd_dev[n].cow.file :
               ubd_dev[n].file;
        ubd_dev[n].size = file_size(file);
        if(ubd_dev[n].size < 0) return(ubd_dev[n].size);
        ubd_part[n].start_sect = 0;
        ubd_part[n].nr_sects = ubd_dev[n].size / blk_sizes[n];
        sizes[n] = ubd_dev[n].size / BLOCK_SIZE;
    }
    ubd_dev[n].count++;
    if ((filp->f_mode & FMODE_WRITE) &&
            ((ubd_dev[n].openflags & ~O_SYNC) == O_RDONLY))
    {
        if(--ubd_dev[n].count == 0) ubd_close(&ubd_dev[n]);
        return -EROFS;
    }
    return(0);
}

static int ubd_release(struct inode * inode, struct file * file)
{
    int n;

    n =  minor(inode->i_rdev);
    if(n > MAX_DEV)
        return -ENODEV;
    if(--ubd_dev[n].count == 0) ubd_close(&ubd_dev[n]);
    return(0);
}

int cow_read = 0;
int cow_write = 0;

void cowify_req(struct io_thread_req *req, struct ubd *dev)
{
    int i, update_bitmap, sector = req->offset >> 9;

    if(req->length > (sizeof(req->sector_mask) * 8) << 9)
        panic("Operation too long");
    if(req->op == UBD_READ)
    {
        for(i = 0; i < req->length >> 9; i++)
        {
            if(ubd_test_bit(sector + i, (unsigned char *)
                            dev->cow.bitmap))
            {
                ubd_set_bit(i, (unsigned char *)
                            &req->sector_mask);
                cow_read++;
            }
        }
    }
    else
    {
        update_bitmap = 0;
        for(i = 0; i < req->length >> 9; i++)
        {
            cow_write++;
            ubd_set_bit(i, (unsigned char *)
                        &req->sector_mask);
            if(!ubd_test_bit(sector + i, (unsigned char *)
                             dev->cow.bitmap))
                update_bitmap = 1;
            ubd_set_bit(sector + i, (unsigned char *)
                        dev->cow.bitmap);
        }
        if(update_bitmap)
        {
            req->cow_offset = sector / (sizeof(unsigned long) * 8);
            req->bitmap_words[0] =
                dev->cow.bitmap[req->cow_offset];
            req->bitmap_words[1] =
                dev->cow.bitmap[req->cow_offset + 1];
            req->cow_offset *= sizeof(unsigned long);
            req->cow_offset += dev->cow.bitmap_offset;
        }
    }
}

static int prepare_request(struct request *req, struct io_thread_req *io_req)
{
    struct ubd *dev;
    __u64 block;
    int nsect;

    if(req->rq_status == RQ_INACTIVE) return(1);

    dev = &ubd_dev[minor(req->rq_dev)];
    if(dev->is_dir)
    {
        strcpy(req->buffer, "HOSTFS:");
        strcat(req->buffer, dev->file);
        return(1);
    }
    if(IS_WRITE(req) && ((dev->openflags & O_ACCMODE) == O_RDONLY))
    {
        printk("Write attempted on readonly ubd device %d\n",
               minor(req->rq_dev));
        return(1);
    }

    block = req->sector;
    nsect = req->current_nr_sectors;

    io_req->op = (req->cmd == READ) ? UBD_READ : UBD_WRITE;
    io_req->fds[0] = (dev->cow.file != NULL) ? dev->cow.fd : dev->fd;
    io_req->fds[1] = dev->fd;
    io_req->offsets[0] = 0;
    io_req->offsets[1] = dev->cow.data_offset;
    io_req->offset = ((__u64) block) << 9;
    io_req->length = nsect << 9;
    io_req->buffer = req->buffer;
    io_req->sectorsize = 1 << 9;
    io_req->sector_mask = 0;
    io_req->cow_offset = -1;
    io_req->error = 0;

    if(dev->cow.file != NULL) cowify_req(io_req, dev);
    return(0);
}

static void do_ubd_request(request_queue_t *q)
{
    struct io_thread_req io_req;
    struct request *req;
    int err, n;

    printk("do_ubd_request is used!\n");
    return;

    if(thread_fd == -1)
    {
        while(!list_empty(&q->queue_head))
        {
            req = blkdev_entry_next_request(&q->queue_head);
            err = prepare_request(req, &io_req);
            if(!err)
            {
                do_io(&io_req);
                ubd_finish(io_req.error);
            }
        }
    }
    else
    {
        if(DEVICE_INTR || list_empty(&q->queue_head)) return;
        req = blkdev_entry_next_request(&q->queue_head);
        err = prepare_request(req, &io_req);
        if(!err)
        {
            SET_INTR(ubd_handler);
            n = write_ubd_fs(thread_fd, (char *) &io_req,
                             sizeof(io_req));
            if(n != sizeof(io_req))
                printk("write to io thread failed, "
                       "errno = %d\n", -n);
        }
    }
}

static int ubd_ioctl(struct inode * inode, struct file * file,
                     unsigned int cmd, unsigned long arg)
{
    struct hd_geometry *loc = (struct hd_geometry *) arg;
    int dev, err;

    if(!inode) return -EINVAL;
    dev = minor(inode->i_rdev);
    if (dev > MAX_DEV)
        return -EINVAL;
    switch (cmd)
    {
        struct hd_geometry g;
        struct cdrom_volctrl volume;
    case HDIO_GETGEO:
        if (!loc)  return -EINVAL;
        g.heads = 128;
        g.sectors = 32;
        g.cylinders = ubd_dev[dev].size / (128 * 32);
        g.start = 2;
        return copy_to_user(loc, &g, sizeof g) ? -EFAULT : 0;
    case BLKRASET:
        if(!capable(CAP_SYS_ADMIN))  return -EACCES;
        if(arg > 0xff) return -EINVAL;
        read_ahead[major(inode->i_rdev)] = arg;
        return 0;
    case BLKRAGET:
        if (!arg)  return -EINVAL;
        err = verify_area(VERIFY_WRITE, (long *) arg, sizeof(long));
        if (err)
            return err;
        return 0;
    case BLKGETSIZE:   /* Return device size */
        if (!arg)  return -EINVAL;
        err = verify_area(VERIFY_WRITE, (long *) arg, sizeof(long));
        if (err)
            return err;
        put_user(ubd_dev[dev].size >> 9, (long *) arg);
        return 0;
    case BLKFLSBUF:
        if(!capable(CAP_SYS_ADMIN))  return -EACCES;
        return 0;

    case BLKRRPART: /* Re-read partition tables */
        return 0; /* revalidate_hddisk(inode->i_rdev, 1); */

    case HDIO_SET_UNMASKINTR:
        if (!capable(CAP_SYS_ADMIN)) return -EACCES;
        if ((arg > 1) || (minor(inode->i_rdev) & 0x3F))
            return -EINVAL;
        return 0;

    case HDIO_GET_UNMASKINTR:
        if (!arg)  return -EINVAL;
        err = verify_area(VERIFY_WRITE, (long *) arg, sizeof(long));
        if (err)
            return err;
        return 0;

    case HDIO_GET_MULTCOUNT:
        if (!arg)  return -EINVAL;
        err = verify_area(VERIFY_WRITE, (long *) arg, sizeof(long));
        if (err)
            return err;
        return 0;

    case HDIO_SET_MULTCOUNT:
        if (!capable(CAP_SYS_ADMIN)) return -EACCES;
        if (minor(inode->i_rdev) & 0x3F) return -EINVAL;
        return 0;

    case HDIO_GET_IDENTITY:
        ubd_id.cyls = ubd_dev[dev].size / (128 * 32);
        if (copy_to_user((char *) arg, (char *) &ubd_id,
                         sizeof(ubd_id)))
            return -EFAULT;
        return 0;

    case CDROMVOLREAD:
        if(copy_from_user(&volume, (char *) arg, sizeof(volume)))
            return -EFAULT;
        volume.channel0 = 255;
        volume.channel1 = 255;
        volume.channel2 = 255;
        volume.channel3 = 255;
        if(copy_to_user((char *) arg, &volume, sizeof(volume)))
            return -EFAULT;
        return 0;

    default:
        return blk_ioctl(inode->i_rdev, cmd, arg);
    }
}

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
