// $Id$
// $Locker$

// Author. Roar Thron�s.
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

#include <system_service_setup.h>
#include <system_data_cells.h>
#include <lnmstrdef.h>
#include <dyndef.h>
#include <lnmdef.h>
#include <descrip.h>

#ifdef CONFIG_VMS

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
#endif

struct _lnmth lnm_sys_dir_table_header;

struct _lnmx lnm_sys_dir_xlat = {
  lnmx$l_flags : LNMX$M_TERMINAL,
  lnmx$l_index : LNMX$C_TABLE,
  lnmx$l_next : 0
};

struct _lnmb lnm_sys_dir = {
  lnmb$l_flink: &lnm_sys_dir,
  lnmb$l_blink: &lnm_sys_dir,
  lnmb$w_size: sizeof(struct _lnmb),
  lnmb$b_type: DYN$C_LNM,
  lnmb$b_acmode: MODE_K_KERNEL,
  lnmb$l_lnmx : &lnm_sys_dir_xlat,
  lnmb$l_table : &lnm_sys_dir_table_header,
  lnmb$b_flags : LNM$M_NO_ALIAS|LNM$M_TABLE|LNM$M_NO_DELETE,
  lnmb$b_count : 21,
  lnmb$t_name : "LNM$SYSTEM_DIRECTORY",
};

struct _lnmth lnm_sys_dir_table_header={
  lnmth$l_flags:LNMTH$M_SHAREABLE|LNMTH$M_DIRECTORY,
  lnmth$l_hash : 0,
  lnmth$l_orb  : 0,
  lnmth$l_name : &lnm_sys_dir,
  lnmth$l_parent : 0,
  lnmth$l_child : 0,
  lnmth$l_sibling : 0,
  lnmth$l_qtable : 0,
  lnmth$l_byteslm : 0x7fffffff,
  lnmth$l_bytes : 0x7fffffff
};

struct _lnmth lnm_sys_table_header;

struct _lnmx lnm_sys_xlat = {
  lnmx$l_flags : LNMX$M_TERMINAL,
  lnmx$l_index : LNMX$C_TABLE,
  lnmx$l_next : 0
};

struct _lnmb lnm_sys = {
  lnmb$l_flink: &lnm_sys,
  lnmb$l_blink: &lnm_sys,
  lnmb$w_size: sizeof(struct _lnmb),
  lnmb$b_type: DYN$C_LNM,
  lnmb$b_acmode: MODE_K_KERNEL,
  lnmb$l_lnmx : &lnm_sys_xlat,
  lnmb$l_table : &lnm_sys_table_header,
  lnmb$b_flags : LNM$M_NO_ALIAS|LNM$M_TABLE,
  lnmb$b_count : 17,
  lnmb$t_name : "LNM$SYSTEM_TABLE",
};

struct _lnmth lnm_sys_table_header={
  lnmth$l_flags: LNMTH$M_SYSTEM|LNMTH$M_SHAREABLE,
  lnmth$l_hash : 0,
  lnmth$l_orb  : 0,
  lnmth$l_name : &lnm_sys,
  lnmth$l_parent : 0,
  lnmth$l_child : 0,
  lnmth$l_sibling : 0,
  lnmth$l_qtable : 0,
  lnmth$l_byteslm : 0x7fffffff,
  lnmth$l_bytes : 0x7fffffff
};

void lnm_init_sys(void) {

  /* this has to be done after malloc has been initialized */
  /* can possibly done with mallocs */

  unsigned long ahash;
  unsigned long * myhash;
  int status;
  struct _lnmb * lnm$system_directory=&lnm_sys_dir;
  struct _lnmth * lnm$system_directory_table_header=&lnm_sys_dir_table_header;

  $DESCRIPTOR(mypartab,"LNM$SYSTEM_DIRECTORY");

  $DESCRIPTOR(mytabnam,"LNM$SYSTEM_TABLE");

  // not yet ready for this?
  //lnm$inslogtab(ret_lnm, &sys_table);
  // then do it manually?

  lnm_sys_dir_table_header.lnmth$l_child = &lnm_sys_table_header;
  lnm_sys_table_header.lnmth$l_parent = &lnm_sys_dir_table_header;
  lnm_sys_dir_table_header.lnmth$l_hash = &lnmhshs;
  lnm_sys_table_header.lnmth$l_hash = &lnmhshs;
  lnm_sys.lnmb$l_table=&lnm_sys_dir_table_header; // beware this and over

  lnm$al_dirtbl[0]=&lnm_sys_dir;

  /*ctl$gl_lnmdirect=LNM$PROCESS_DIRECTORY;
    lnm$al_dirtbl[0]=LNM$SYSTEM_DIRECTORY;
    lnm$al_dirtbl[1]=ctl$gl_lnmdirect;*/
  myhash=&ahash; //lnmmalloc(sizeof(unsigned long));
  status=lnm$hash(mypartab.dsc$w_length,mypartab.dsc$a_pointer,0xffff,myhash);
#ifdef LNM_DEBUG 
  lnmprintf("here %x %x\n",myhash,*myhash);
#endif
  lnmhshs.entry[2*(*myhash)]=lnm$system_directory;
  lnmhshs.entry[2*(*myhash)+1]=lnm$system_directory;
  status=lnm$hash(mytabnam.dsc$w_length,mytabnam.dsc$a_pointer,0xffff,myhash);
#ifdef LNM_DEBUG 
  lnmprintf("here %x %x\n",myhash,*myhash);
#endif
  lnmhshs.entry[2*(*myhash)]=&lnm_sys;
  lnmhshs.entry[2*(*myhash)+1]=&lnm_sys;
}

void exe$swapinit(void) {

  /* allocate and initialize lnm stuff */

  /* do sysinit, but init-like stuff is done i main.c */



 mainloop:
  goto mainloop;

}
