// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/vmalloc.h>
#include<asm/current.h>
#include<system_data_cells.h>
#include<lckdef.h>
#include<lckctxdef.h>
#include<misc.h>
#include<ssdef.h>
#include<starlet.h>
#include<ipldef.h>
#include<rsbdef.h>
#include<lkbdef.h>
#include<pridef.h>
#include<descrip.h>
#include<cdrpdef.h>

asmlinkage int exe$getlki(unsigned long efn, unsigned long *lkidadr,void *itmlst, struct _iosb *iosb, void (*astadr)(), long astprm,unsigned long reserved) {
}

asmlinkage int exe$getlkiw(unsigned long efn, unsigned long *lkidadr,void *itmlst, struct _iosb *iosb, void (*astadr)(), long astprm,unsigned long reserved) {
  int status=exe$getlki(efn,lkidadr,itmlst,iosb,astadr,astprm,reserved);
  printk("\n\nremember to do like qiow\n\n");
  if ((status&1)==0) return status;
  return exe$synch(efn,iosb);
}

asmlinkage int exe$getlkiw_wrap(struct struct_args * s) {
  return exe$getlkiw(s->s1,s->s2,s->s3,s->s4,s->s5,s->s6,s->s7);
}

asmlinkage int exe$getlki_wrap(struct struct_args * s) {
  return exe$getlki(s->s1,s->s2,s->s3,s->s4,s->s5,s->s6,s->s7);
}

