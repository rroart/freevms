// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/completion.h>
#include <linux/personality.h>
#include <linux/mm.h>
#include <linux/module.h>

#include <system_data_cells.h>
#include <descrip.h>
#include <dyndef.h>
#include <ipldef.h>
#include <phddef.h>
#include <pqbdef.h>
#include <prcdef.h>
#include <pridef.h>
#include <rdedef.h>
#include <secdef.h>
#include <ssdef.h>
#include <starlet.h>
#include <queue.h>
#include <exe_routines.h>
#include <misc_routines.h>
#include <sch_routines.h>
#include <internals.h>

#include <linux/sched.h>
#include <linux/slab.h>

#include <asm/uaccess.h>

struct _generic_64
{
    long long l;
};

asmlinkage int exe$creprc(unsigned int *pidadr, void *image, void *input, void *output, void *error, struct _generic_64 *prvadr, unsigned int *quota, void*prcnam, unsigned int baspri, unsigned int uic, unsigned short int mbxunt, unsigned int stsflg,...);

asmlinkage int exe$creprc_wrap(struct struct_args *s)
{
    return exe$creprc(s->s1,s->s2,s->s3,s->s4,s->s5,s->s6,s->s7,s->s8,s->s9,s->s10,s->s11,s->s12);
}

asmlinkage int exe$creprc(unsigned int *pidadr, void *image, void *input, void *output, void *error, struct _generic_64 *prvadr, unsigned int *quota, void*prcnam, unsigned int baspri, unsigned int uic, unsigned short int mbxunt, unsigned int stsflg,...)
{
    unsigned long stack_here;
    struct _pcb * p, * cur;
    int retval;

    struct dsc$descriptor * imd = image, * ind = input, * oud = output, * erd = error;

    unsigned long clone_flags=CLONE_VFORK;
    //check pidadr

    ctl$gl_creprc_flags = stsflg;
    // check for PRC$M_NOUAF sometime

    if (stsflg&PRC$M_DETACH)
    {

    }
    if (uic)
    {

    }
    //setipl(IPL$_ASTDEL);//postpone this?
    cur=ctl$gl_pcb;
    vmslock(&SPIN_SCHED, IPL$_SCHED);
    vmslock(&SPIN_MMG, IPL$_MMG);
    p = alloc_task_struct();
    //bzero(p,sizeof(struct _pcb));//not wise?
    memset(p,0,sizeof(struct _pcb));

    // check more
    // compensate for no struct clone/copy
    p->sigmask_lock = SPIN_LOCK_UNLOCKED;
    p->alloc_lock = SPIN_LOCK_UNLOCKED;

    qhead_init(&p->pcb$l_astqfl);
    // and enable ast del to all modes

    p->pcb$b_type = DYN$C_PCB;

    p->pcb$b_asten=15;
    p->phd$b_astlvl=4;
    p->pr_astlvl=4;
    p->psl=0;
    p->pslindex=0;

    qhead_init(&p->pcb$l_lockqfl);
    // set capabilities
    p->pcb$l_permanent_capability = sch$gl_default_process_cap;
    p->pcb$l_capability = p->pcb$l_permanent_capability;
    // set affinity
    // set default fileprot
    // set arb
    // set mbx stuff
    // from setprn:
    if (prcnam)
    {
        struct dsc$descriptor *s=prcnam;
        strncpy(p->pcb$t_lname,s->dsc$a_pointer,s->dsc$w_length);
    }
    // set priv
    p->pcb$l_priv=ctl$gl_pcb->pcb$l_priv;
    // set pris
    p->pcb$b_prib=31-baspri;
    p->pcb$b_pri=31-baspri-6;
    //  if (p->pcb$b_pri<16) p->pcb$b_pri=16;
    p->pcb$w_quant=-QUANTUM;

    // set uic
    p->pcb$l_uic=ctl$gl_pcb->pcb$l_uic;
    // set vms pid
    // check process name
    // do something with pqb

    p->pcb$l_pqb=kmalloc(sizeof(struct _pqb),GFP_KERNEL);
    memset(p->pcb$l_pqb,0,sizeof(struct _pqb));

    struct _pqb * pqb = p->pcb$l_pqb;

    pqb->pqb$q_prvmsk = ctl$gq_procpriv;

    if (imd)
        memcpy(pqb->pqb$t_image,imd->dsc$a_pointer,imd->dsc$w_length);
    if (ind)
        memcpy(pqb->pqb$t_input,ind->dsc$a_pointer,ind->dsc$w_length);
    if (oud)
        memcpy(pqb->pqb$t_output,oud->dsc$a_pointer,oud->dsc$w_length);
    if (erd)
        memcpy(pqb->pqb$t_error,erd->dsc$a_pointer,erd->dsc$w_length);

    if (oud) // temp measure
        memcpy(p->pcb$t_terminal,oud->dsc$a_pointer,oud->dsc$w_length);

    // translate some logicals
    // copy security clearance
    // copy msg
    // copy flags
    // set jib
    // do quotas
    // process itmlst
    // set pcb$l_pqb
#if 0
    setipl(IPL$_MMG);
    vmslock(&SPIN_SCHED,-1);
    // find vacant slot in pcb vector
    // and store it
#endif
    // make ipid and epid
    p->pcb$l_pid=alloc_ipid();
    {
        unsigned long *vec=sch$gl_pcbvec;
        vec[p->pcb$l_pid&0xffff]=p;
    }
    p->pcb$l_epid=exe$ipid_to_epid(p->pcb$l_pid);
    // should invoke sch$chse, put this at bottom?
    // setipl(0) and return

    // now lots of things from fork

    retval = -EAGAIN;
    /*
     * Check if we are over our maximum process limit, but be sure to
     * exclude root. This is needed to make it possible for login and
     * friends to set the per-user process limit to something lower
     * than the amount of processes root is running. -- Rik
     */
#if 0
    if (atomic_read(&p->user->processes) >= p->rlim[RLIMIT_NPROC].rlim_cur
            && !capable(CAP_SYS_ADMIN) && !capable(CAP_SYS_RESOURCE))
        goto bad_fork_free;

    atomic_inc(&p->user->__count);
    atomic_inc(&p->user->processes);
#endif

    /*
     * Counter increases are protected by
     * the kernel lock so nr_threads can't
     * increase under us (but it may decrease).
     */

    get_exec_domain(p->exec_domain);

    if (p->binfmt && p->binfmt->module)
        __MOD_INC_USE_COUNT(p->binfmt->module);

    p->did_exec = 0;
    p->swappable = 0;
    p->state = TASK_UNINTERRUPTIBLE;

    //copy_flags(clone_flags, p);
    // not here?    p->pcb$l_pid = alloc_ipid();

    p->run_list.next = NULL;
    p->run_list.prev = NULL;

    p->p_cptr = NULL;
    init_waitqueue_head(&p->wait_chldexit);
    p->vfork_done = NULL;
    spin_lock_init(&p->alloc_lock);

    p->sigpending = 0;
    init_sigpending(&p->pending);

    p->it_real_value = p->it_virt_value = p->it_prof_value = 0;
    p->it_real_incr = p->it_virt_incr = p->it_prof_incr = 0;
    init_timer(&p->real_timer);
    p->real_timer.data = (unsigned long) p;

    p->leader = 0;      /* session leadership doesn't inherit */
    p->tty_old_pgrp = 0;
    p->times.tms_utime = p->times.tms_stime = 0;
    p->times.tms_cutime = p->times.tms_cstime = 0;
    p->lock_depth = -1;     /* -1 = no lock */
    p->start_time = jiffies;

    INIT_LIST_HEAD(&p->local_pages);

    p->files = current->files;
    p->fs = current->fs;
    p->sig = current->sig;

    /* copy all the process information */
    if (copy_files(clone_flags, p))
        goto bad_fork_cleanup;
    if (copy_fs(clone_flags, p))
        goto bad_fork_cleanup_files;
    if (copy_sighand(clone_flags, p))
        goto bad_fork_cleanup_fs;

bad_fork_cleanup:
bad_fork_cleanup_files:
bad_fork_cleanup_fs:

    // now a hole

    // now more from fork

    /* ok, now we should be set up.. */
    p->swappable = 1;
    p->exit_signal = 0;
    p->pdeath_signal = 0;

    /*
     * "share" dynamic priority between parent and child, thus the
     * total amount of dynamic priorities in the system doesnt change,
     * more scheduling fairness. This is only important in the first
     * timeslice, on the long run the scheduling behaviour is unchanged.
     */

    /*
     * Ok, add it to the run-queues and make it
     * visible to the rest of the system.
     *
     * Let it rip!
     */
    retval = p->pcb$l_epid;
    INIT_LIST_HEAD(&p->thread_group);

    /* Need tasklist lock for parent etc handling! */
    write_lock_irq(&tasklist_lock);

    /* CLONE_PARENT and CLONE_THREAD re-use the old parent */
    p->p_opptr = current->p_opptr;
    p->p_pptr = current->p_pptr;

    p->p_opptr = current /*->p_opptr*/;
    p->p_pptr = current /*->p_pptr*/;

    SET_LINKS(p);

    nr_threads++;
    write_unlock_irq(&tasklist_lock);

    //  printk("fork befwak\n");
    //wake_up_process(p);       /* do this last */
    //  wake_up_process2(p,PRI$_TICOM);     /* do this last */
    //goto fork_out;//??


    // now something from exec

    // wait, better do execve itself

    memcpy(p->rlim, current->rlim, sizeof(p->rlim));

    qhead_init(&p->pcb$l_sqfl);

    struct mm_struct * mm = mm_alloc();
    p->mm = mm;
    p->active_mm = mm;

    p->user = INIT_USER;

    spin_lock(&mmlist_lock);
#if 0
    list_add(&mm->mmlist, &p->p_pptr->mm->mmlist);
#endif
    mmlist_nr++;
    spin_unlock(&mmlist_lock);

    // Now we are getting into the area that is really the swappers

    // To be moved to shell.c and swp$shelinit later

    p->pcb$l_phd=kmalloc(sizeof(struct _phd),GFP_KERNEL);
    init_phd(p->pcb$l_phd);

    init_fork_p1pp(p,p->pcb$l_phd,ctl$gl_pcb,ctl$gl_pcb->pcb$l_phd);
#ifdef __x86_64__
    shell_init_other(p,ctl$gl_pcb,0x7ff80000-0x1000,0x7fffe000);
    shell_init_other(p,ctl$gl_pcb,0x7ff80000-0x2000,0x7fffe000);
    shell_init_other(p,ctl$gl_pcb,0x7ff90000-0x1000,0x7fffe000);
    shell_init_other(p,ctl$gl_pcb,0x7ff90000-0x2000,0x7fffe000);
    shell_init_other(p,ctl$gl_pcb,0x7ffa0000-0x1000,0x7fffe000);
    shell_init_other(p,ctl$gl_pcb,0x7ffa0000-0x2000,0x7fffe000);
#else
    shell_init_other(p,ctl$gl_pcb,0x7ff80000-0x1000,0x7fffe000);
    shell_init_other(p,ctl$gl_pcb,0x7ff80000-0x2000,0x7fffe000);
    shell_init_other(p,ctl$gl_pcb,0x7ff90000-0x1000,0x7fffe000);
    shell_init_other(p,ctl$gl_pcb,0x7ff90000-0x2000,0x7fffe000);
#endif
    int exe$procstrt(struct _pcb * p);
    struct pt_regs * regs = &pidadr;
    //printk("newthread %x\n",p),
    retval = new_thread(0, clone_flags, 0, 0, p, 0);

    int eip=0,esp=0;

    //  start_thread(regs,eip,esp);

    sch$chse(p, PRI$_TICOM);

    vmsunlock(&SPIN_MMG,-1);
    vmsunlock(&SPIN_SCHED,0);

    return SS$_NORMAL;

#if 0
    return sys_execve(((struct dsc$descriptor *)image)->dsc$a_pointer,0,0);

    return SS$_NORMAL;
#endif

#if 0
    {
        char * filename=((struct dsc$descriptor *)image)->dsc$a_pointer;
        char ** argv=0;
        char ** envp=0;
        struct pt_regs * regs=0;
        struct linux_binprm bprm;
        struct file *file;
        int retval;
        int i;

        file = open_exec(filename);

        retval = PTR_ERR(file);
        if (IS_ERR(file))
            return retval;

        bprm.p = PAGE_SIZE*MAX_ARG_PAGES-sizeof(void *);
        memset(bprm.page, 0, MAX_ARG_PAGES*sizeof(bprm.page[0]));

        bprm.file = file;
        bprm.filename = filename;
        bprm.sh_bang = 0;
        bprm.loader = 0;
        bprm.exec = 0;
        if ((bprm.argc = count(argv, bprm.p / sizeof(void *))) < 0)
        {
            allow_write_access(file);
            fput(file);
            //printk("here 7 %x\n",bprm.argc);
            return bprm.argc;
        }

        if ((bprm.envc = count(envp, bprm.p / sizeof(void *))) < 0)
        {
            allow_write_access(file);
            fput(file);
            //printk("here 6\n");
            return bprm.envc;
        }

        retval = prepare_binprm(&bprm);
        //printk("here 4\n");
        if (retval < 0)
            goto out;

        retval = copy_strings_kernel(1, &bprm.filename, &bprm);
        //printk("here 3\n");
        if (retval < 0)
            goto out;

        bprm.exec = bprm.p;
        retval = copy_strings(bprm.envc, envp, &bprm);
        //printk("here 2\n");
        if (retval < 0)
            goto out;

        retval = copy_strings(bprm.argc, argv, &bprm);
        //printk("here 1\n");
        if (retval < 0)
            goto out;

        retval = search_binary_handler(&bprm,regs);
        if (retval >= 0)
            /* execve success */
            return retval;

out:
        /* Something went wrong, return the inode and free the argument pages*/
        allow_write_access(bprm.file);
        if (bprm.file)
            fput(bprm.file);

        for (i = 0 ; i < MAX_ARG_PAGES ; i++)
        {
            struct page * page = bprm.page[i];
            if (page)
                __free_page(page);
        }

        return retval;
    }
#endif

fork_out:
    return retval;

bad_fork_free:
    free_task_struct(p);
    goto fork_out;

}

