// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/unistd.h>
#include<linux/linkage.h>
#include<linux/sched.h>
#include<starlet.h>

/* Author: Roar Thronæs */

//asmlinkage int sys_$setpri(unsigned int *pidadr, void *prcnam, unsigned int pri, unsigned int *prvpri, unsigned int*pol, unsigned int *prvpol) {
asmlinkage int exe$setpri(struct struct_setpri *s) {
  struct _pcb *p;
  if (s->pidadr) {
    p=current;
  } else {
    int i;
    for (i=0;i<PIDHASH_SZ; i++)
      if (pidhash[i]->pid==*(s->pidadr)) {
	p=pidhash[i];
	break;
      }
  }

  if (!p) p=current;

  p->pcb$b_prib=s->pri;
  printk("here I am\n");
}
