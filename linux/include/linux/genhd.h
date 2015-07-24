#ifndef _LINUX_GENHD_H
#define _LINUX_GENHD_H

/*
 *  genhd.h Copyright (C) 1992 Drew Eckhardt
 *  Generic hard disk header file by
 *      Drew Eckhardt
 *
 *      <drew@colorado.edu>
 */

#include <linux/config.h>
#include <linux/types.h>
#include <linux/major.h>

enum
{
    /* These three have identical behaviour; use the second one if DOS fdisk gets
       confused about extended/logical partitions starting past cylinder 1023. */
    DOS_EXTENDED_PARTITION = 5,
    LINUX_EXTENDED_PARTITION = 0x85,
    WIN98_EXTENDED_PARTITION = 0x0f,

    LINUX_SWAP_PARTITION = 0x82,
    LINUX_RAID_PARTITION = 0xfd,    /* autodetect RAID partition */

    SOLARIS_X86_PARTITION = LINUX_SWAP_PARTITION,

    DM6_PARTITION = 0x54,   /* has DDO: use xlated geom & offset */
    EZD_PARTITION = 0x55,   /* EZ-DRIVE */
    DM6_AUX1PARTITION = 0x51,   /* no DDO:  use xlated geom */
    DM6_AUX3PARTITION = 0x53,   /* no DDO:  use xlated geom */

    FREEBSD_PARTITION = 0xa5,    /* FreeBSD Partition ID */
    OPENBSD_PARTITION = 0xa6,    /* OpenBSD Partition ID */
    NETBSD_PARTITION = 0xa9,   /* NetBSD Partition ID */
    BSDI_PARTITION = 0xb7,    /* BSDI Partition ID */
    /* Ours is not to wonder why.. */
    BSD_PARTITION = FREEBSD_PARTITION,
    MINIX_PARTITION = 0x81,  /* Minix Partition ID */
    PLAN9_PARTITION = 0x39,  /* Plan 9 Partition ID */
    UNIXWARE_PARTITION = 0x63,      /* Partition ID, same as */
    /* GNU_HURD and SCO Unix */
};

struct partition
{
    unsigned char boot_ind;     /* 0x80 - active */
    unsigned char head;     /* starting head */
    unsigned char sector;       /* starting sector */
    unsigned char cyl;      /* starting cylinder */
    unsigned char sys_ind;      /* What partition type */
    unsigned char end_head;     /* end head */
    unsigned char end_sector;   /* end sector */
    unsigned char end_cyl;      /* end cylinder */
    unsigned int start_sect;    /* starting sector counting from 0 */
    unsigned int nr_sects;      /* nr of sectors in partition */
} __attribute__((packed));

#ifdef __KERNEL__

struct hd_struct
{
    unsigned long start_sect;
    unsigned long nr_sects;
    int number;                     /* stupid old code wastes space  */
};

#define GENHD_FL_REMOVABLE  1

struct gendisk
{
    int major;          /* major number of driver */
    const char *major_name;     /* name of major driver */
    int minor_shift;        /* number of times minor is shifted to
                       get real minor */
    int max_p;          /* maximum partitions per device */

    struct hd_struct *part;     /* [indexed by minor] */
    int *sizes;         /* [idem], device size in blocks */
    int nr_real;            /* number of real devices */

    void *real_devices;     /* internal use */
    struct gendisk *next;
    struct block_device_operations *fops;

    char *flags;                    /* one per physical disc */
};

/* drivers/block/genhd.c */
extern struct gendisk *gendisk_head;

extern void add_gendisk(struct gendisk *gp);
extern void del_gendisk(struct gendisk *gp);
extern struct gendisk *get_gendisk(kdev_t dev);

#endif  /*  __KERNEL__  */

#ifdef __KERNEL__

char *disk_name (struct gendisk *hd, int minor, char *buf);

extern void devfs_register_partitions (struct gendisk *dev, int minor,
                                       int unregister);

/*
 * FIXME: this should use genhd->minor_shift, but that is slow to look up.
 */
static inline unsigned int disk_index (kdev_t dev)
{
    int major = MAJOR(dev);
    int minor = MINOR(dev);
    unsigned int index;

    switch (major)
    {
    case DAC960_MAJOR+0:
        index = (minor & 0x00f8) >> 3;
        break;
    case SCSI_DISK0_MAJOR:
        index = (minor & 0x00f0) >> 4;
        break;
    case IDE0_MAJOR:    /* same as HD_MAJOR */
    case XT_DISK_MAJOR:
        index = (minor & 0x0040) >> 6;
        break;
    case IDE1_MAJOR:
        index = ((minor & 0x0040) >> 6) + 2;
        break;
    default:
        return 0;
    }
    return index;
}

#endif

#endif
