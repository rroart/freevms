#include<linux/unistd.h>
#include<linux/linkage.h>
#include<linux/sched.h>
#include"../../freevms/starlet/src/starlet.h"
#include"../../freevms/librtl/src/descrip.h"

/* Author: Roar Thronæs */

//asmlinkage int sys_$setpri(unsigned int *pidadr, void *prcnam, unsigned int pri, unsigned int *prvpri, unsigned int*pol, unsigned int *prvpol) {
asmlinkage int sys_$setprn(struct dsc$descriptor *s) {
  struct _pcb *p;

  if (!p) p=current;
  p=current;
  strncpy(p->pcb$t_lname,s->dsc$a_pointer,s->dsc$w_length);

  printk("here I am2 %x\n",s->dsc$a_pointer);
}




