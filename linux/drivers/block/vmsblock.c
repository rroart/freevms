#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/kernel_stat.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/config.h>
#include <linux/locks.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/smp_lock.h>
#include <linux/completion.h>

#include <asm/system.h>
#include <asm/io.h>
#include <linux/blk.h>
#include <linux/highmem.h>
#include <linux/slab.h>
#include <linux/module.h>

spinlock_t io_request_lock = SPIN_LOCK_UNLOCKED;

/*
 * For the allocated request tables
 */
static kmem_cache_t *request_cachep;

/* This specifies how many sectors to read ahead on the disk. */

int read_ahead[MAX_BLKDEV];

/* blk_dev_struct is:
 *  *request_fn
 *  *current_request
 */
struct blk_dev_struct blk_dev[MAX_BLKDEV]; /* initialized by blk_dev_init() */

/*
 * blk_size contains the size of all block-devices in units of 1024 byte
 * sectors:
 *
 * blk_size[MAJOR][MINOR]
 *
 * if (!blk_size[MAJOR]) then no minor size checking is done.
 */
int * blk_size[MAX_BLKDEV];

/*
 * blksize_size contains the size of all block-devices:
 *
 * blksize_size[MAJOR][MINOR]
 *
 * if (!blksize_size[MAJOR]) then 1024 bytes is assumed.
 */
int * blksize_size[MAX_BLKDEV];

/*
 * hardsect_size contains the size of the hardware sector of a device.
 *
 * hardsect_size[MAJOR][MINOR]
 *
 * if (!hardsect_size[MAJOR])
 *      then 512 bytes is assumed.
 * else
 *      sector_size is hardsect_size[MAJOR][MINOR]
 * This is currently set by some scsi devices and read by the msdos fs driver.
 * Other uses may appear later.
 */
int * hardsect_size[MAX_BLKDEV];

/*
 * The following tunes the read-ahead algorithm in mm/filemap.c
 */
int * max_readahead[MAX_BLKDEV];

/*
 * Max number of sectors per request
 */
int * max_sectors[MAX_BLKDEV];

/*
 * How many reqeusts do we allocate per queue,
 * and how many do we "batch" on freeing them?
 */
static int queue_nr_requests, batch_requests;

static long ro_bits[MAX_BLKDEV][8];

int is_read_only(kdev_t dev)
{
    int minor,major;

    major = MAJOR(dev);
    minor = MINOR(dev);
    if (major < 0 || major >= MAX_BLKDEV) return 0;
    return ro_bits[major][minor >> 5] & (1 << (minor & 31));
}

void set_device_ro(kdev_t dev,int flag)
{
    int minor,major;

    major = MAJOR(dev);
    minor = MINOR(dev);
    if (major < 0 || major >= MAX_BLKDEV) return;
    if (flag) ro_bits[major][minor >> 5] |= 1 << (minor & 31);
    else ro_bits[major][minor >> 5] &= ~(1 << (minor & 31));
}

#define MB(kb)  ((kb) << 10)

int __init blk_dev_init(void)
{
    struct blk_dev_struct *dev;
    int total_ram;

    request_cachep = kmem_cache_create("blkdev_requests",
                                       sizeof(struct request),
                                       0, SLAB_HWCACHE_ALIGN, NULL, NULL);

    if (!request_cachep)
        panic("Can't create request pool slab cache\n");

    for (dev = blk_dev + MAX_BLKDEV; dev-- != blk_dev;)
        dev->queue = NULL;

    memset(ro_bits,0,sizeof(ro_bits));
    memset(max_readahead, 0, sizeof(max_readahead));
    memset(max_sectors, 0, sizeof(max_sectors));

    extern unsigned int nr_free_pages(void);
    total_ram = nr_free_pages() << (PAGE_SHIFT - 10);

    /*
     * Free request slots per queue.
     * (Half for reads, half for writes)
     */
    queue_nr_requests = 64;
    if (total_ram > MB(32))
        queue_nr_requests = 128;

    /*
     * Batch frees according to queue length
     */
    batch_requests = queue_nr_requests/4;
    printk("block: %d slots per queue, batch=%d\n", queue_nr_requests, batch_requests);

#if defined(CONFIG_IDE) && defined(CONFIG_BLK_DEV_IDE)
    ide_init();     /* this MUST precede hd_init */
#endif
#if defined(CONFIG_IDE) && defined(CONFIG_BLK_DEV_HD)
    hd_init();
#endif
#ifdef CONFIG_BLK_DEV_FD_VMS
    vms_floppy_init();
#endif
#ifdef CONFIG_BLK_DEV_FD
    floppy_init();
#else
#if defined(__i386__)   /* Do we even need this? */
    outb_p(0xc, 0x3f2);
#endif
#endif

    return 0;
};
