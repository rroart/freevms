// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/unistd.h>
#include<linux/linkage.h>
#include<linux/sched.h>
#include<asm/bitops.h>
#include<starlet.h>
#include<ssdef.h>
#include<pridef.h>
#include<statedef.h>
#include<evtdef.h>
#include<ipldef.h>
#include<descrip.h>
#include <system_data_cells.h>
#include <internals.h>

/* Author: Roar Thronæs */

asmlinkage int exe$hiber(void) {
  /* spinlock sched */
  struct _pcb * p=current;
  vmslock(&SPIN_SCHED,IPL$_SCHED);
  
#if 0
  if (p->pcb$l_sts & PCB$M_WAKEPEN) {
    p->pcb$l_sts&=~PCB$M_WAKEPEN;
  }
#endif
  /* release spin */
  /* set ipl 0 ? */

  if (test_and_clear_bit(12,&p->pcb$l_sts)) { // PCB$M_WAKEPEN 0x1000 12 bits
    vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
    setipl(0);
    return SS$_NORMAL;
  }
  /* cwps stuff not yet */
  return sch$wait(p,sch$gq_hibwq);
}

/* return params not as specified */
void * exe$nampid2(struct _pcb *p, unsigned long *pidadr, void *prcnam) {
  /* sched spinlock */
  vmslock(&SPIN_SCHED,IPL$_SCHED);
  if (pidadr==0 && prcnam==0) {
    return p;
  }
  if (pidadr) {
    struct _pcb * tmp, **htable = &pidhash[pid_hashfn(*pidadr)];
    //printk("bef for\n");
    for(tmp = *htable; tmp && tmp->pid != *pidadr; tmp = tmp->pidhash_next) ;
    //printk("aft for\n");
    return tmp;
  }
  if (prcnam) {
    /* not yet */
    return;
  }
  /* should not get here */
  vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
  return 0;
}

asmlinkage int exe$wake(unsigned long *pidadr, void *prcnam) {
  struct _pcb *p;
  p=exe$nampid2(current,pidadr,prcnam);
  if (p) {
    sch$wake(p->pid);
    vmsunlock(&SPIN_SCHED,IPL$_ASTDEL);
    return;
  }
  /* no cwps stuff yet */
  setipl(0);
}

asmlinkage int exe$suspnd(unsigned int *pidadr, void *prcnam, unsigned int flags ) {

}

asmlinkage int exe$resume (unsigned int *pidadr, void *prcnam) {
  struct _pcb *p;
  p=exe$nampid2(current,pidadr,prcnam);
  vmsunlock(&SPIN_SCHED,0);
  if (p) return sch$rse(p,PRI$_RESAVL,EVT$_RESUME);
  /* no cwps here either */
}

//asmlinkage int sys_$setpri(unsigned int *pidadr, void *prcnam, unsigned int pri, unsigned int *prvpri, unsigned int*pol, unsigned int *prvpol) {
asmlinkage int exe$setprn(struct dsc$descriptor *s) {
  struct _pcb *p;

  if (!p) p=current;
  p=current;
  strncpy(p->pcb$t_lname,s->dsc$a_pointer,s->dsc$w_length);

  printk("here I am2 %x\n",s->dsc$a_pointer);
}




