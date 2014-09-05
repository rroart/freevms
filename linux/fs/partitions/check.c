// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004.

/*
 *  Code extracted from drivers/block/genhd.c
 *  Copyright (C) 1991-1998  Linus Torvalds
 *  Re-organised Feb 1998 Russell King
 *
 *  We now have independent partition support from the
 *  block drivers, which allows all the partition code to
 *  be grouped in one location, and it to be mostly self
 *  contained.
 *
 *  Added needed MAJORS for new pairs, {hdi,hdj}, {hdk,hdl}
 */

#include <linux/config.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/blk.h>
#include <linux/init.h>
#include <linux/raid/md.h>

#include "check.h"

#include "msdos.h"

#include <sys$routines.h>
#include <misc_routines.h>
#include <iodef.h>

extern int *blk_size[];

int warn_no_part = 1; /*This is ugly: should make genhd removable media aware*/

static int (*check_part[])(struct gendisk *hd, struct block_device *bdev, unsigned long first_sect, int first_minor) = {
#ifdef CONFIG_ACORN_PARTITION
	acorn_partition,
#endif
#ifdef CONFIG_LDM_PARTITION
	ldm_partition,		/* this must come before msdos */
#endif
#ifdef CONFIG_MSDOS_PARTITION
	msdos_partition,
#endif
#ifdef CONFIG_OSF_PARTITION
	osf_partition,
#endif
#ifdef CONFIG_SUN_PARTITION
	sun_partition,
#endif
#ifdef CONFIG_AMIGA_PARTITION
	amiga_partition,
#endif
#ifdef CONFIG_ATARI_PARTITION
	atari_partition,
#endif
#ifdef CONFIG_MAC_PARTITION
	mac_partition,
#endif
#ifdef CONFIG_SGI_PARTITION
	sgi_partition,
#endif
#ifdef CONFIG_ULTRIX_PARTITION
	ultrix_partition,
#endif
#ifdef CONFIG_IBM_PARTITION
	ibm_partition,
#endif
	NULL
};

/*
 *	This is ucking fugly but its probably the best thing for 2.4.x
 *	Take it as a clear reminder than we should put the device name
 *	generation in the object kdev_t points to in 2.5.
 */
 
#ifdef CONFIG_ARCH_S390
int (*genhd_dasd_name)(char*,int,int,struct gendisk*) = NULL;
EXPORT_SYMBOL(genhd_dasd_name);
#endif

/*
 * disk_name() is used by partition check code and the md driver.
 * It formats the devicename of the indicated disk into
 * the supplied buffer (of size at least 32), and returns
 * a pointer to that same buffer (for convenience).
 */

char *disk_name (struct gendisk *hd, int minor, char *buf)
{
	const char *maj = hd->major_name;
	unsigned int unit = (minor >> hd->minor_shift);
	unsigned int part = (minor & ((1 << hd->minor_shift) -1 ));

#ifdef CONFIG_ARCH_S390
	if (genhd_dasd_name
	    && genhd_dasd_name (buf, unit, part, hd) == 0)
		return buf;
#endif
	/*
	 * IDE devices use multiple major numbers, but the drives
	 * are named as:  {hda,hdb}, {hdc,hdd}, {hde,hdf}, {hdg,hdh}..
	 * This requires special handling here.
	 */
	switch (hd->major) {
		case IDE9_MAJOR:
			unit += 2;
		case IDE8_MAJOR:
			unit += 2;
		case IDE7_MAJOR:
			unit += 2;
		case IDE6_MAJOR:
			unit += 2;
		case IDE5_MAJOR:
			unit += 2;
		case IDE4_MAJOR:
			unit += 2;
		case IDE3_MAJOR:
			unit += 2;
		case IDE2_MAJOR:
			unit += 2;
		case IDE1_MAJOR:
			unit += 2;
		case IDE0_MAJOR:
			maj = "hd";
			break;
		case MD_MAJOR:
			sprintf(buf, "%s%d", maj, unit);
			return buf;
	}
	if (hd->major >= SCSI_DISK1_MAJOR && hd->major <= SCSI_DISK7_MAJOR) {
		unit = unit + (hd->major - SCSI_DISK1_MAJOR + 1) * 16;
		if (unit+'a' > 'z') {
			unit -= 26;
			sprintf(buf, "sd%c%c", 'a' + unit / 26, 'a' + unit % 26);
			if (part)
				sprintf(buf + 4, "%d", part);
			return buf;
		}
	}
	if (hd->major >= COMPAQ_SMART2_MAJOR && hd->major <= COMPAQ_SMART2_MAJOR+7) {
		int ctlr = hd->major - COMPAQ_SMART2_MAJOR;
 		if (part == 0)
 			sprintf(buf, "%s/c%dd%d", maj, ctlr, unit);
 		else
 			sprintf(buf, "%s/c%dd%dp%d", maj, ctlr, unit, part);
 		return buf;
 	}
	if (hd->major >= COMPAQ_CISS_MAJOR && hd->major <= COMPAQ_CISS_MAJOR+7) {
                int ctlr = hd->major - COMPAQ_CISS_MAJOR;
                if (part == 0)
                        sprintf(buf, "%s/c%dd%d", maj, ctlr, unit);
                else
                        sprintf(buf, "%s/c%dd%dp%d", maj, ctlr, unit, part);
                return buf;
	}
	if (hd->major >= DAC960_MAJOR && hd->major <= DAC960_MAJOR+7) {
		int ctlr = hd->major - DAC960_MAJOR;
 		if (part == 0)
 			sprintf(buf, "%s/c%dd%d", maj, ctlr, unit);
 		else
 			sprintf(buf, "%s/c%dd%dp%d", maj, ctlr, unit, part);
 		return buf;
 	}
	if (hd->major == ATARAID_MAJOR) {
		int disk = minor >> hd->minor_shift;
		int part = minor & (( 1 << hd->minor_shift) - 1);
		if (part == 0)
			sprintf(buf, "%s/d%d", maj, disk);
		else
			sprintf(buf, "%s/d%dp%d", maj, disk, part);
		return buf;
	}
	if (part)
		sprintf(buf, "%s%c%d", maj, unit+'a', part);
	else
		sprintf(buf, "%s%c", maj, unit+'a');
	return buf;
}

/*
 * Add a partitions details to the devices partition description.
 */
void add_gd_partition(struct gendisk *hd, int minor, int start, int size)
{
#ifndef CONFIG_DEVFS_FS
	char buf[40];
#endif

	hd->part[minor].start_sect = start;
	hd->part[minor].nr_sects   = size;
#ifdef CONFIG_DEVFS_FS
	printk(" p%d", (minor & ((1 << hd->minor_shift) - 1)));
#else
	if ((hd->major >= COMPAQ_SMART2_MAJOR+0 && hd->major <= COMPAQ_SMART2_MAJOR+7) ||
	    (hd->major >= COMPAQ_CISS_MAJOR+0 && hd->major <= COMPAQ_CISS_MAJOR+7))
		printk(" p%d", (minor & ((1 << hd->minor_shift) - 1)));
	else
		printk(" %s", disk_name(hd, minor, buf));
#endif
}

static void check_partition(struct gendisk *hd, kdev_t dev, int first_part_minor)
{
	static int first_time = 1;
	unsigned long first_sector;
	struct block_device *bdev;
	char buf[64];
	int i;

	if (first_time)
		printk(KERN_INFO "Partition check:\n");
	first_time = 0;
	first_sector = hd->part[MINOR(dev)].start_sect;

	/*
	 * This is a kludge to allow the partition check to be
	 * skipped for specific drives (e.g. IDE CD-ROM drives)
	 */
	if ((int)first_sector == -1) {
		hd->part[MINOR(dev)].start_sect = 0;
		return;
	}

	i = -1;
	if (i >= 0)
		printk(KERN_INFO " /dev/%s:", buf + i);
	else
		printk(KERN_INFO " %s:", disk_name(hd, MINOR(dev), buf));
	bdev = bdget(kdev_t_to_nr(dev));
	bdev->bd_inode->i_size = (loff_t)hd->part[MINOR(dev)].nr_sects << 9;
	bdev->bd_inode->i_blkbits = blksize_bits(block_size(dev));
	for (i = 0; check_part[i]; i++) {
		int res;
		res = check_part[i](hd, bdev, first_sector, first_part_minor);
		if (res) {
			if (res < 0 &&  warn_no_part)
				printk(" unable to read partition table\n");
			goto setup_devfs;
		}
	}

	printk(" unknown partition table\n");
setup_devfs:
	truncate_inode_pages(bdev->bd_inode->i_mapping, 0);
	bdput(bdev);
	i = first_part_minor - 1;
}

/*
 * This function will re-read the partition tables for a given device,
 * and set things back up again.  There are some important caveats,
 * however.  You must ensure that no one is using the device, and no one
 * can start using the device while this function is being executed.
 *
 * Much of the cleanup from the old partition tables should have already been
 * done
 */

void register_disk(struct gendisk *gdev, kdev_t dev, unsigned minors,
	struct block_device_operations *ops, long size)
{
	if (!gdev)
		return;
	grok_partitions(gdev, MINOR(dev)>>gdev->minor_shift, minors, size);
}

void grok_partitions(struct gendisk *dev, int drive, unsigned minors, long size)
{
	int i;
	int first_minor	= drive << dev->minor_shift;
	int end_minor	= first_minor + dev->max_p;

	if(!dev->sizes)
		blk_size[dev->major] = NULL;

	dev->part[first_minor].nr_sects = size;
	/* No such device or no minors to use for partitions */
	if (!size || minors == 1)
		return;

	if (dev->sizes) {
		dev->sizes[first_minor] = size >> (BLOCK_SIZE_BITS - 9);
		for (i = first_minor + 1; i < end_minor; i++)
			dev->sizes[i] = 0;
	}
	blk_size[dev->major] = dev->sizes;
	check_partition(dev, MKDEV(dev->major, first_minor), 1 + first_minor);

 	/*
 	 * We need to set the sizes array before we will be able to access
 	 * any of the partitions on this device.
 	 */
	if (dev->sizes != NULL) {	/* optional safeguard in ll_rw_blk.c */
		for (i = first_minor; i < end_minor; i++)
			dev->sizes[i] = dev->part[i].nr_sects >> (BLOCK_SIZE_BITS - 9);
	}
}

extern struct address_space_operations def_blk_aops;
unsigned char *read_dev_sector(struct block_device *bdev, unsigned long n, Sector *p)
{
	struct address_space *mapping = bdev->bd_inode->i_mapping;
	int sect = PAGE_CACHE_SIZE / 512;
	struct page *page = alloc_pages(GFP_KERNEL, 0);

#if 0
	block_read_full_page2(bdev->bd_inode,page,n/sect);
#else
#if 0
	myqio(0, page_address(page), 4096, n, to_kdev_t(bdev->bd_dev), 2);
#else
	unsigned long long iosb;
	int sts = sys$qiow(0,(unsigned short)dev2chan(to_kdev_t(bdev->bd_dev)),IO$_READPBLK,&iosb,0,0, page_address(page),4096,n*2,0,0,0);
#endif
#endif

	if (!IS_ERR(page)) {
		p->v = page;
		return (unsigned char *)page_address(page) + 512 * (n % sect);
fail:
		page_cache_release(page);
	}
	p->v = NULL;
	return NULL;
}
