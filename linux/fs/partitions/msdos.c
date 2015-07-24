// $Id$
// $Locker$

// Author. Roar Thron�s.
// Modified Linux source file, 2001-2004.

/*
 *  fs/partitions/msdos.c
 *
 *  Code extracted from drivers/block/genhd.c
 *  Copyright (C) 1991-1998  Linus Torvalds
 *
 *  Thanks to Branko Lankester, lankeste@fwi.uva.nl, who found a bug
 *  in the early extended-partition checks and added DM partitions
 *
 *  Support for DiskManager v6.0x added by Mark Lord,
 *  with information provided by OnTrack.  This now works for linux fdisk
 *  and LILO, as well as loadlin and bootln.  Note that disks other than
 *  /dev/hda *must* have a "DOS" type 0x51 partition in the first slot (hda1).
 *
 *  More flexible handling of extended partitions - aeb, 950831
 *
 *  Check partition table on IDE disks for common CHS translations
 *
 *  Re-organised Feb 1998 Russell King
 */

#include <linux/config.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/string.h>
#include <linux/blk.h>

#ifdef CONFIG_BLK_DEV_IDE
#include <linux/ide.h>  /* IDE xlate */
#endif /* CONFIG_BLK_DEV_IDE */

#include <asm/system.h>

#include "check.h"
#include "msdos.h"

/*
 * Many architectures don't like unaligned accesses, which is
 * frequently the case with the nr_sects and start_sect partition
 * table entries.
 */
#include <asm/unaligned.h>

#define SYS_IND(p)  (get_unaligned(&p->sys_ind))
#define NR_SECTS(p) ({ __typeof__(p->nr_sects) __a =    \
                get_unaligned(&p->nr_sects);    \
                le32_to_cpu(__a); \
            })

#define START_SECT(p)   ({ __typeof__(p->start_sect) __a =  \
                get_unaligned(&p->start_sect);  \
                le32_to_cpu(__a); \
            })

static inline int is_extended_partition(struct partition *p)
{
    return (SYS_IND(p) == DOS_EXTENDED_PARTITION ||
            SYS_IND(p) == WIN98_EXTENDED_PARTITION ||
            SYS_IND(p) == LINUX_EXTENDED_PARTITION);
}

/*
 * partition_name() formats the short partition name into the supplied
 * buffer, and returns a pointer to that buffer.
 * Used by several partition types which makes conditional inclusion messy,
 * use __attribute__ ((unused)) instead.
 */
static char __attribute__ ((unused))
*partition_name (struct gendisk *hd, int minor, char *buf)
{
#ifdef CONFIG_DEVFS_FS
    sprintf(buf, "p%d", (minor & ((1 << hd->minor_shift) - 1)));
    return buf;
#else
    return disk_name(hd, minor, buf);
#endif
}

#define MSDOS_LABEL_MAGIC1  0x55
#define MSDOS_LABEL_MAGIC2  0xAA

static inline int
msdos_magic_present(unsigned char *p)
{
    return (p[0] == MSDOS_LABEL_MAGIC1 && p[1] == MSDOS_LABEL_MAGIC2);
}

/*
 * Create devices for each logical partition in an extended partition.
 * The logical partitions form a linked list, with each entry being
 * a partition table with two entries.  The first entry
 * is the real data partition (with a start relative to the partition
 * table start).  The second is a pointer to the next logical partition
 * (with a start relative to the entire extended partition).
 * We do not create a Linux partition for the partition tables, but
 * only for the actual data partitions.
 */

static void extended_partition(struct gendisk *hd, struct block_device *bdev,
                               int minor, unsigned long first_size, int *current_minor)
{
    struct partition *p;
    Sector sect;
    unsigned char *data;
    unsigned long first_sector, this_sector, this_size;
    int mask = (1 << hd->minor_shift) - 1;
    int sector_size = get_hardsect_size(to_kdev_t(bdev->bd_dev)) / 512;
    int loopct = 0;     /* number of links followed
                   without finding a data partition */
    int i;

    this_sector = first_sector = hd->part[minor].start_sect;
    this_size = first_size;

    while (1)
    {
        if (++loopct > 100)
            return;
        if ((*current_minor & mask) == 0)
            return;
        data = read_dev_sector(bdev, this_sector, &sect);
        if (!data)
            return;

        if (!msdos_magic_present(data + 510))
            goto done;

        p = (struct partition *) (data + 0x1be);

        /*
         * Usually, the first entry is the real data partition,
         * the 2nd entry is the next extended partition, or empty,
         * and the 3rd and 4th entries are unused.
         * However, DRDOS sometimes has the extended partition as
         * the first entry (when the data partition is empty),
         * and OS/2 seems to use all four entries.
         */

        /*
         * First process the data partition(s)
         */
        for (i=0; i<4; i++, p++)
        {
            unsigned long offs, size, next;
            if (!NR_SECTS(p) || is_extended_partition(p))
                continue;

            /* Check the 3rd and 4th entries -
               these sometimes contain random garbage */
            offs = START_SECT(p)*sector_size;
            size = NR_SECTS(p)*sector_size;
            next = this_sector + offs;
            if (i >= 2)
            {
                if (offs + size > this_size)
                    continue;
                if (next < first_sector)
                    continue;
                if (next + size > first_sector + first_size)
                    continue;
            }

            add_gd_partition(hd, *current_minor, next, size);

            (*current_minor)++;
            loopct = 0;
            if ((*current_minor & mask) == 0)
                goto done;
        }
        /*
         * Next, process the (first) extended partition, if present.
         * (So far, there seems to be no reason to make
         *  extended_partition()  recursive and allow a tree
         *  of extended partitions.)
         * It should be a link to the next logical partition.
         * Create a minor for this just long enough to get the next
         * partition table.  The minor will be reused for the next
         * data partition.
         */
        p -= 4;
        for (i=0; i<4; i++, p++)
            if (NR_SECTS(p) && is_extended_partition(p))
                break;
        if (i == 4)
            goto done;   /* nothing left to do */

        this_sector = first_sector + START_SECT(p) * sector_size;
        this_size = NR_SECTS(p) * sector_size;
        minor = *current_minor;
        put_dev_sector(sect);
    }
done:
    put_dev_sector(sect);
}

/* james@bpgc.com: Solaris has a nasty indicator: 0x82 which also
   indicates linux swap.  Be careful before believing this is Solaris. */

static void
solaris_x86_partition(struct gendisk *hd, struct block_device *bdev,
                      int minor, int *current_minor)
{
}

static void bsd_partition(struct gendisk *hd, struct block_device *bdev,
                          int minor, int *current_minor)
{
}

static void netbsd_partition(struct gendisk *hd, struct block_device *bdev,
                             int minor, int *current_minor)
{
}

static void openbsd_partition(struct gendisk *hd, struct block_device *bdev,
                              int minor, int *current_minor)
{
}

/*
 * Create devices for Unixware partitions listed in a disklabel, under a
 * dos-like partition. See extended_partition() for more information.
 */
static void unixware_partition(struct gendisk *hd, struct block_device *bdev,
                               int minor, int *current_minor)
{
}

/*
 * Minix 2.0.0/2.0.2 subpartition support.
 * Anand Krishnamurthy <anandk@wiproge.med.ge.com>
 * Rajeev V. Pillai    <rajeevvp@yahoo.com>
 */
static void minix_partition(struct gendisk *hd, struct block_device *bdev,
                            int minor, int *current_minor)
{
}

static struct
{
    unsigned char id;
    void (*parse)(struct gendisk *, struct block_device *, int, int *);
} subtypes[] =
{
    {BSD_PARTITION, bsd_partition},
    {NETBSD_PARTITION, netbsd_partition},
    {OPENBSD_PARTITION, openbsd_partition},
    {MINIX_PARTITION, minix_partition},
    {UNIXWARE_PARTITION, unixware_partition},
    {SOLARIS_X86_PARTITION, solaris_x86_partition},
    {0, NULL},
};
/*
 * Look for various forms of IDE disk geometry translation
 */
static int handle_ide_mess(struct block_device *bdev)
{
#ifdef CONFIG_BLK_DEV_IDE
    Sector sect;
    unsigned char *data;
    kdev_t dev = to_kdev_t(bdev->bd_dev);
    unsigned int sig;
    int heads = 0;
    struct partition *p;
    int i;
    /*
     * The i386 partition handling programs very often
     * make partitions end on cylinder boundaries.
     * There is no need to do so, and Linux fdisk doesnt always
     * do this, and Windows NT on Alpha doesnt do this either,
     * but still, this helps to guess #heads.
     */
    data = read_dev_sector(bdev, 0, &sect);
    if (!data)
        return -1;
    if (!msdos_magic_present(data + 510))
    {
        put_dev_sector(sect);
        return 0;
    }
    sig = le16_to_cpu(*(unsigned short *)(data + 2));
    p = (struct partition *) (data + 0x1be);
    for (i = 0; i < 4; i++)
    {
        struct partition *q = &p[i];
        if (NR_SECTS(q))
        {
            if ((q->sector & 63) == 1 &&
                    (q->end_sector & 63) == 63)
                heads = q->end_head + 1;
            break;
        }
    }
    if (SYS_IND(p) == EZD_PARTITION)
    {
        /*
         * Accesses to sector 0 must go to sector 1 instead.
         */
        if (ide_xlate_1024(dev, -1, heads, " [EZD]"))
            goto reread;
    }
    else if (SYS_IND(p) == DM6_PARTITION)
    {

        /*
         * Everything on the disk is offset by 63 sectors,
         * including a "new" MBR with its own partition table.
         */
        if (ide_xlate_1024(dev, 1, heads, " [DM6:DDO]"))
            goto reread;
    }
    else if (sig <= 0x1ae &&
             data[sig] == 0xAA && data[sig+1] == 0x55 &&
             (data[sig+2] & 1))
    {
        /* DM6 signature in MBR, courtesy of OnTrack */
        (void) ide_xlate_1024 (dev, 0, heads, " [DM6:MBR]");
    }
    else if (SYS_IND(p) == DM6_AUX1PARTITION ||
             SYS_IND(p) == DM6_AUX3PARTITION)
    {
        /*
         * DM6 on other than the first (boot) drive
         */
        (void) ide_xlate_1024(dev, 0, heads, " [DM6:AUX]");
    }
    else
    {
        (void) ide_xlate_1024(dev, 2, heads, " [PTBL]");
    }
    put_dev_sector(sect);
    return 1;

reread:
    put_dev_sector(sect);
    /* Flush the cache */
#endif /* CONFIG_BLK_DEV_IDE */
    return 1;
}

int msdos_partition(struct gendisk *hd, struct block_device *bdev,
                    unsigned long first_sector, int first_part_minor)
{
    int i, minor = first_part_minor;
    Sector sect;
    struct partition *p;
    unsigned char *data;
    int mask = (1 << hd->minor_shift) - 1;
    int sector_size = get_hardsect_size(to_kdev_t(bdev->bd_dev)) / 512;
    int current_minor = first_part_minor;
    int err;

    err = handle_ide_mess(bdev);
    if (err <= 0)
        return err;
    data = read_dev_sector(bdev, 0, &sect);
    if (!data)
        return -1;
    if (!msdos_magic_present(data + 510))
    {
        put_dev_sector(sect);
        return 0;
    }
    p = (struct partition *) (data + 0x1be);

    /*
     * Look for partitions in two passes:
     * First find the primary and DOS-type extended partitions.
     * On the second pass look inside *BSD, Unixware and Solaris partitions.
     */

    current_minor += 4;
    for (i=1 ; i<=4 ; minor++,i++,p++)
    {
        if (!NR_SECTS(p))
            continue;
        add_gd_partition(hd, minor,
                         first_sector+START_SECT(p)*sector_size,
                         NR_SECTS(p)*sector_size);
        if (is_extended_partition(p))
        {
            unsigned long size = hd->part[minor].nr_sects;
            printk(" <");
            /* prevent someone doing mkfs or mkswap on an
               extended partition, but leave room for LILO */
            if (size > 2)
                hd->part[minor].nr_sects = 2;
            extended_partition(hd, bdev, minor, size, &current_minor);
            printk(" >");
        }
    }

    /*
     *  Check for old-style Disk Manager partition table
     */
    if (msdos_magic_present(data + 0xfc))
    {
        p = (struct partition *) (0x1be + data);
        for (i = 4 ; i < 16 ; i++, current_minor++)
        {
            p--;
            if ((current_minor & mask) == 0)
                break;
            if (!(START_SECT(p) && NR_SECTS(p)))
                continue;
            add_gd_partition(hd, current_minor, START_SECT(p), NR_SECTS(p));
        }
    }
    printk("\n");

    /* second pass - output for each on a separate line */
    minor -= 4;
    p = (struct partition *) (0x1be + data);
    for (i=1 ; i<=4 ; minor++,i++,p++)
    {
        unsigned char id = SYS_IND(p);
        int n;

        if (!NR_SECTS(p))
            continue;

        for (n = 0; subtypes[n].parse && id != subtypes[n].id; n++)
            ;

        if (subtypes[n].parse)
            subtypes[n].parse(hd, bdev, minor, &current_minor);
    }
    put_dev_sector(sect);
    return 1;
}
