// $Id$
// $Locker$

// Author. Roar Thronæs.
// Author. Linux people.

#include <linux/slab.h>
#include <linux/kernel_stat.h>
#include <linux/swap.h>
#include <linux/swapctl.h>
#include <linux/smp_lock.h>
#include <linux/pagemap.h>
#include <linux/init.h>
#include <linux/highmem.h>
#include <linux/file.h>
#include <linux/compiler.h>

#include <asm/pgalloc.h>

#include <system_data_cells.h>

struct mm_struct *swap_mm = &init_mm;

DECLARE_WAIT_QUEUE_HEAD(kswapd_wait);

int try_to_free_pages(zone_t *classzone, unsigned int gfp_mask, unsigned int order)
{
#if 0
	int priority = DEF_PRIORITY;
	int nr_pages = SWAP_CLUSTER_MAX;

	gfp_mask = pf_gfp_mask(gfp_mask);
	do {
		nr_pages = shrink_caches(classzone, priority, gfp_mask, nr_pages);
		if (nr_pages <= 0)
			return 1;
	} while (--priority);

	/*
	 * Hmm.. Cache shrink failed - time to kill something?
	 * Mhwahahhaha! This is the part I really like. Giggle.
	 */
	out_of_memory();
	return 0;
#endif
	return 1;
}

void balance(void) {
  if (sch$gl_freelim<=sch$gl_freecnt || sch$gl_freelim>= sgn$gl_freegoal) {
    /* no deleted phd check yet */
    // if sch$gb_sip & sch$v_mpw
    // other checks
    mmg$purgempl();
  }
}

void swapsched(void) {

}

int kswapd(void *unused)
{
	struct task_struct *tsk = current;
	DECLARE_WAITQUEUE(wait, tsk);

	daemonize();
	strcpy(tsk->pcb$t_lname, "SWAPPER");
	sigfillset(&tsk->blocked);
	
	/*
	 * Tell the memory management that we're a "memory allocator",
	 * and that if we need more memory we should get access to it
	 * regardless (see "__alloc_pages()"). "kswapd" should
	 * never get caught in the normal page freeing logic.
	 *
	 * (Kswapd normally doesn't need memory anyway, but sometimes
	 * you need a small amount of memory in order to be able to
	 * page out something else, and this flag essentially protects
	 * us from recursively trying to free more memory as we're
	 * trying to free the first piece of memory in the first place).
	 */
	tsk->flags |= PF_MEMALLOC;

	/*
	 * Kswapd main loop.
	 */
	for (;;) {
		__set_current_state(TASK_INTERRUPTIBLE);
		add_wait_queue(&kswapd_wait, &wait);

		mb();
		//		if (kswapd_can_sleep())
		//			schedule();
		if ((current->pcb$l_sts&PCB$M_WAKEPEN)==0)
		  schedule();

		__set_current_state(TASK_RUNNING);
		remove_wait_queue(&kswapd_wait, &wait);

		printk("doing the swap\n");

		balance();
		mmg$wrtmfypag();
		swapsched();

		/* powerfail stuff */

		//		run_task_queue(&tq_disk);
	}
}

static int __init kswapd_init(void)
{
	printk("Starting kswapd\n");
	swap_setup();
	kernel_thread(kswapd, NULL, CLONE_FS | CLONE_FILES | CLONE_SIGNAL);
	return 0;
}

module_init(kswapd_init)

void exe$swapinit(void) {

  /* allocate and initialize lnm stuff */

  /* do sysinit, but init-like stuff is done i main.c */

 mainloop:
  goto mainloop;

}
