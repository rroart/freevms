// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>

#include<syidef.h>
#include<ssdef.h>
#include<misc.h>
#include<sbdef.h>
#include<starlet.h>

extern struct _sb mysb;

asmlinkage int exe$getsyi(unsigned int efn, unsigned int *csidadr, void *nodename, void *itmlst, struct _iosb *iosb, void (*astadr)(), unsigned long astprm) {
  int sts;
  struct item_list_3 * it=itmlst;
  // no use of namcsid yet
  if (iosb) {
    // check iosb write access
    memset(iosb,0,sizeof(struct _iosb));
  }
  exe$clref(efn);
  // check ast quota
  // check all access in itemlist

  while (it->item_code) {
    switch (it->item_code) {
    case SYI$_SCSNODE:
      bcopy(&mysb.sb$t_nodename,it->bufaddr,15);
      break;

    default:
      printk("unrecognized syi\n");
      break;
    }
    it++;
  }
  return SS$_NORMAL;
}

asmlinkage int exe$getsyiw(unsigned int efn, unsigned int *csidadr, void *nodename, void *itmlst, struct _iosb *iosb, void (*astadr)(), unsigned long astprm) {
  int status=exe$getsyi(efn,csidadr,nodename,itmlst,iosb,astadr,astprm);
  if ((status&1)==0) return status;
  return exe$synch(efn,iosb);
}

asmlinkage int exe$getsyiw_wrap(struct struct_args * s) {
  return exe$getsyiw(s->s1,s->s2,s->s3,s->s4,s->s5,s->s6,s->s7);
}


asmlinkage int exe$getsyi_wrap(struct struct_args * s) {
  return exe$getsyi(s->s1,s->s2,s->s3,s->s4,s->s5,s->s6,s->s7);
}
