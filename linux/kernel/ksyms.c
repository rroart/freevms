// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004.

/*
 * Herein lies all the functions/variables that are "exported" for linkage
 * with dynamically loaded kernel modules.
 *			Jon.
 *
 * - Stacked module support and unified symbol table added (June 1994)
 * - External symbol table support added (December 1994)
 * - Versions on symbols added (December 1994)
 *   by Bjorn Ekwall <bj0rn@blox.se>
 */

#include <linux/config.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/cdrom.h>
#include <linux/kernel_stat.h>
#include <linux/vmalloc.h>
#include <linux/sys.h>
#include <linux/utsname.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/serial.h>
#include <linux/locks.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <linux/reboot.h>
#include <linux/pagemap.h>
#include <linux/sysctl.h>
#include <linux/hdreg.h>
#include <linux/skbuff.h>
#include <linux/genhd.h>
#include <linux/blkpg.h>
#include <linux/ctype.h>
#include <linux/file.h>
#include <linux/iobuf.h>
#include <linux/console.h>
#include <linux/poll.h>
#include <linux/mmzone.h>
#include <linux/mm.h>
#include <linux/capability.h>
#include <linux/highuid.h>
#include <linux/brlock.h>
#include <linux/fs.h>
#include <linux/tty.h>
#include <linux/in6.h>
#include <linux/completion.h>
#include <linux/seq_file.h>
#include <asm/checksum.h>

#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif

extern void set_device_ro(kdev_t dev,int flag);

extern void *sys_call_table;

extern struct timezone sys_tz;
extern int request_dma(unsigned int dmanr, char * deviceID);
extern void free_dma(unsigned int dmanr);
extern spinlock_t dma_spin_lock;

#ifdef CONFIG_MODVERSIONS
const struct module_symbol __export_Using_Versions
__attribute__((section("__ksymtab"))) =
{
    1 /* Version version */, "Using_Versions"
};
#endif


EXPORT_SYMBOL(inter_module_register);
EXPORT_SYMBOL(inter_module_unregister);
EXPORT_SYMBOL(inter_module_get);
EXPORT_SYMBOL(inter_module_get_request);
EXPORT_SYMBOL(inter_module_put);
EXPORT_SYMBOL(try_inc_mod_count);

/* process memory management */
EXPORT_SYMBOL(do_mmap_pgoff);
EXPORT_SYMBOL(do_munmap);
EXPORT_SYMBOL(do_brk);
EXPORT_SYMBOL(exit_mm);
EXPORT_SYMBOL(exit_files);
EXPORT_SYMBOL(exit_fs);
EXPORT_SYMBOL(exit_sighand);

/* internal kernel memory management */
EXPORT_SYMBOL(_alloc_pages);
EXPORT_SYMBOL(__alloc_pages);
EXPORT_SYMBOL(alloc_pages_node);
EXPORT_SYMBOL(__get_free_pages);
EXPORT_SYMBOL(get_zeroed_page);
EXPORT_SYMBOL(__free_pages);
EXPORT_SYMBOL(free_pages);
EXPORT_SYMBOL(num_physpages);
EXPORT_SYMBOL(kmem_find_general_cachep);
EXPORT_SYMBOL(kmem_cache_create);
EXPORT_SYMBOL(kmem_cache_destroy);
EXPORT_SYMBOL(kmem_cache_shrink);
EXPORT_SYMBOL(kmem_cache_alloc);
EXPORT_SYMBOL(kmem_cache_free);
EXPORT_SYMBOL(kmalloc);
EXPORT_SYMBOL(kfree);
EXPORT_SYMBOL(vfree);
EXPORT_SYMBOL(__vmalloc);
EXPORT_SYMBOL(mem_map);
EXPORT_SYMBOL(remap_page_range);
EXPORT_SYMBOL(max_mapnr);
EXPORT_SYMBOL(high_memory);
EXPORT_SYMBOL(vmtruncate);
EXPORT_SYMBOL(find_vma);
EXPORT_SYMBOL(get_unmapped_area);
EXPORT_SYMBOL(init_mm);
#ifdef CONFIG_HIGHMEM
EXPORT_SYMBOL(kmap_high);
EXPORT_SYMBOL(kunmap_high);
EXPORT_SYMBOL(highmem_start_page);
EXPORT_SYMBOL(create_bounce);
#endif

/* filesystem internal functions */
EXPORT_SYMBOL(getname);
EXPORT_SYMBOL(fput);
EXPORT_SYMBOL(fget);
EXPORT_SYMBOL(sys_close);
EXPORT_SYMBOL(mark_buffer_dirty);
EXPORT_SYMBOL(__mark_buffer_dirty);
EXPORT_SYMBOL(__mark_inode_dirty);
EXPORT_SYMBOL(get_empty_filp);
EXPORT_SYMBOL(fsync_dev);
EXPORT_SYMBOL(fsync_no_super);
EXPORT_SYMBOL(set_blocksize);
EXPORT_SYMBOL(getblk);
EXPORT_SYMBOL(bdget);
EXPORT_SYMBOL(bdput);
EXPORT_SYMBOL(bread);
EXPORT_SYMBOL(__brelse);
EXPORT_SYMBOL(__bforget);
EXPORT_SYMBOL(unlock_buffer);
EXPORT_SYMBOL(generic_direct_IO);
EXPORT_SYMBOL(discard_bh_page);
EXPORT_SYMBOL(block_sync_page);
EXPORT_SYMBOL(generic_cont_expand);
EXPORT_SYMBOL(cont_prepare_write);
EXPORT_SYMBOL(block_truncate_page);
EXPORT_SYMBOL(generic_block_bmap);
EXPORT_SYMBOL(generic_file_mmap);
EXPORT_SYMBOL(generic_buffer_fdatasync);
EXPORT_SYMBOL(get_unused_fd);
EXPORT_SYMBOL(generic_read_dir);
EXPORT_SYMBOL(generic_file_llseek);
EXPORT_SYMBOL(no_llseek);
EXPORT_SYMBOL(__pollwait);
EXPORT_SYMBOL(poll_freewait);
EXPORT_SYMBOL(ROOT_DEV);

/* for stackable file systems (lofs, wrapfs, cryptfs, etc.) */
EXPORT_SYMBOL(default_llseek);
EXPORT_SYMBOL(filemap_sync);
EXPORT_SYMBOL(filemap_fdatasync);
EXPORT_SYMBOL(filemap_fdatawait);

/* block device driver support */
EXPORT_SYMBOL(blksize_size);
EXPORT_SYMBOL(hardsect_size);
EXPORT_SYMBOL(blk_size);
EXPORT_SYMBOL(blk_dev);
EXPORT_SYMBOL(is_read_only);
EXPORT_SYMBOL(set_device_ro);
EXPORT_SYMBOL(sync_dev);
EXPORT_SYMBOL(blkdev_get);
EXPORT_SYMBOL(blkdev_put);
EXPORT_SYMBOL(ioctl_by_bdev);
EXPORT_SYMBOL(grok_partitions);
EXPORT_SYMBOL(register_disk);
EXPORT_SYMBOL(init_buffer);
EXPORT_SYMBOL(max_sectors);
EXPORT_SYMBOL(max_readahead);

/* executable format registration */
EXPORT_SYMBOL(register_binfmt);
EXPORT_SYMBOL(unregister_binfmt);
EXPORT_SYMBOL(search_binary_handler);
EXPORT_SYMBOL(prepare_binprm);
EXPORT_SYMBOL(compute_creds);
EXPORT_SYMBOL(remove_arg_zero);
EXPORT_SYMBOL(set_binfmt);

/* sysctl table registration */
EXPORT_SYMBOL(register_sysctl_table);
EXPORT_SYMBOL(unregister_sysctl_table);
EXPORT_SYMBOL(sysctl_string);
EXPORT_SYMBOL(sysctl_intvec);
EXPORT_SYMBOL(sysctl_jiffies);
EXPORT_SYMBOL(proc_dostring);
EXPORT_SYMBOL(proc_dointvec);
EXPORT_SYMBOL(proc_dointvec_jiffies);
EXPORT_SYMBOL(proc_dointvec_minmax);
EXPORT_SYMBOL(proc_doulongvec_ms_jiffies_minmax);
EXPORT_SYMBOL(proc_doulongvec_minmax);

/* interrupt handling */
EXPORT_SYMBOL(add_timer);
EXPORT_SYMBOL(del_timer);
EXPORT_SYMBOL(request_irq);
EXPORT_SYMBOL(free_irq);
#if !defined(CONFIG_IA64)	/* irq_stat is part of struct cpuinfo_ia64 */
EXPORT_SYMBOL(irq_stat);
#endif

/* waitqueue handling */
EXPORT_SYMBOL(add_wait_queue);
EXPORT_SYMBOL(add_wait_queue_exclusive);
EXPORT_SYMBOL(remove_wait_queue);

/* completion handling */
EXPORT_SYMBOL(wait_for_completion);
EXPORT_SYMBOL(complete);

/* The notion of irq probe/assignment is foreign to S/390 */

#if !defined(CONFIG_ARCH_S390)
EXPORT_SYMBOL(probe_irq_on);
EXPORT_SYMBOL(probe_irq_off);
#endif

#ifdef CONFIG_SMP
EXPORT_SYMBOL(del_timer_sync);
#endif
EXPORT_SYMBOL(mod_timer);
EXPORT_SYMBOL(tq_timer);
EXPORT_SYMBOL(tq_immediate);

#ifdef CONFIG_SMP
/* Various random spinlocks we want to export */
#if 0
EXPORT_SYMBOL(tqueue_lock);
#endif

/* Big-Reader lock implementation */
EXPORT_SYMBOL(__brlock_array);
#ifndef __BRLOCK_USE_ATOMICS
EXPORT_SYMBOL(__br_write_locks);
#endif
EXPORT_SYMBOL(__br_write_lock);
EXPORT_SYMBOL(__br_write_unlock);
#endif

/* Kiobufs */
EXPORT_SYMBOL(alloc_kiovec);
EXPORT_SYMBOL(free_kiovec);
EXPORT_SYMBOL(expand_kiobuf);

EXPORT_SYMBOL(map_user_kiobuf);
EXPORT_SYMBOL(unmap_kiobuf);
EXPORT_SYMBOL(lock_kiovec);
EXPORT_SYMBOL(unlock_kiovec);
EXPORT_SYMBOL(brw_kiovec);
EXPORT_SYMBOL(kiobuf_wait_for_io);

/* dma handling */
EXPORT_SYMBOL(request_dma);
EXPORT_SYMBOL(free_dma);
EXPORT_SYMBOL(dma_spin_lock);
#ifdef HAVE_DISABLE_HLT
EXPORT_SYMBOL(disable_hlt);
EXPORT_SYMBOL(enable_hlt);
#endif

/* resource handling */
EXPORT_SYMBOL(request_resource);
EXPORT_SYMBOL(release_resource);
EXPORT_SYMBOL(allocate_resource);
EXPORT_SYMBOL(check_resource);
EXPORT_SYMBOL(__request_region);
EXPORT_SYMBOL(__check_region);
EXPORT_SYMBOL(__release_region);
EXPORT_SYMBOL(ioport_resource);
EXPORT_SYMBOL(iomem_resource);

/* process management */
EXPORT_SYMBOL(complete_and_exit);
EXPORT_SYMBOL(__wake_up);
EXPORT_SYMBOL(__wake_up_sync);
EXPORT_SYMBOL(wake_up_process);
EXPORT_SYMBOL(sleep_on);
EXPORT_SYMBOL(sleep_on_timeout);
EXPORT_SYMBOL(interruptible_sleep_on);
EXPORT_SYMBOL(interruptible_sleep_on_timeout);
EXPORT_SYMBOL(schedule);
EXPORT_SYMBOL(schedule_timeout);
EXPORT_SYMBOL(jiffies);
EXPORT_SYMBOL(xtime);
EXPORT_SYMBOL(do_gettimeofday);
EXPORT_SYMBOL(do_settimeofday);

#if !defined(__ia64__)
EXPORT_SYMBOL(loops_per_jiffy);
#endif

EXPORT_SYMBOL(kstat);
EXPORT_SYMBOL(nr_running);

/* misc */
EXPORT_SYMBOL(panic);
EXPORT_SYMBOL(sprintf);
EXPORT_SYMBOL(snprintf);
EXPORT_SYMBOL(sscanf);
EXPORT_SYMBOL(vsprintf);
EXPORT_SYMBOL(vsnprintf);
EXPORT_SYMBOL(vsscanf);
EXPORT_SYMBOL(simple_strtol);
EXPORT_SYMBOL(simple_strtoul);
EXPORT_SYMBOL(simple_strtoull);
EXPORT_SYMBOL(system_utsname);	/* UTS data */
EXPORT_SYMBOL(uts_sem);		/* UTS semaphore */
#ifndef __mips__
EXPORT_SYMBOL(sys_call_table);
#endif
EXPORT_SYMBOL(machine_restart);
EXPORT_SYMBOL(machine_halt);
EXPORT_SYMBOL(machine_power_off);
EXPORT_SYMBOL(_ctype);
EXPORT_SYMBOL(securebits);
EXPORT_SYMBOL(cap_bset);
EXPORT_SYMBOL(reparent_to_init);
EXPORT_SYMBOL(daemonize);
EXPORT_SYMBOL(csum_partial); /* for networking and md */
#if 1
EXPORT_SYMBOL(seq_escape);
EXPORT_SYMBOL(seq_printf);
EXPORT_SYMBOL(seq_open);
EXPORT_SYMBOL(seq_release);
EXPORT_SYMBOL(seq_read);
EXPORT_SYMBOL(seq_lseek);
#endif

/* Program loader interfaces */
EXPORT_SYMBOL(setup_arg_pages);
EXPORT_SYMBOL(copy_strings_kernel);
EXPORT_SYMBOL(do_execve);
EXPORT_SYMBOL(flush_old_exec);

/* Miscellaneous access points */
EXPORT_SYMBOL(si_meminfo);

/* Added to make file system as module */
EXPORT_SYMBOL(sys_tz);
EXPORT_SYMBOL(file_fsync);
EXPORT_SYMBOL(___strtok);
EXPORT_SYMBOL(read_ahead);

#ifdef CONFIG_UID16
EXPORT_SYMBOL(overflowuid);
EXPORT_SYMBOL(overflowgid);
#endif
EXPORT_SYMBOL(fs_overflowuid);
EXPORT_SYMBOL(fs_overflowgid);

/* all busmice */
EXPORT_SYMBOL(fasync_helper);
EXPORT_SYMBOL(kill_fasync);

EXPORT_SYMBOL(disk_name);	/* for md.c */

/* binfmt_aout */
EXPORT_SYMBOL(get_write_access);

/* library functions */
EXPORT_SYMBOL(strnicmp);
EXPORT_SYMBOL(strspn);
EXPORT_SYMBOL(strsep);

/* init task, for moving kthread roots - ought to export a function ?? */

EXPORT_SYMBOL(init_task_union);

EXPORT_SYMBOL(tasklist_lock);
