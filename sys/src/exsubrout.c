// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <system_data_cells.h>
#include <ipldef.h>
#include <ipl.h>
#include <internals.h>
#include <ssdef.h>
#include <queue.h>

#include <linux/mm.h>
#include <linux/slab.h>

void exe$instimq(struct _tqe * t) {
  static signed int times=-500;
  struct _tqe *tmp=exe$gl_tqfl->tqe$l_tqfl;
  int savipl=vmslock(&SPIN_TIMER,IPL$_TIMER);
#if 0
  times++;
  if (times>=0 && times<5)     printk("%x %x %x\n",t,t->tqe$q_delta,t->tqe$q_time);
  if (times>=0 && times<5)     printk("%x %x %x\n",exe$gl_tqfl,exe$gl_tqfl->tqe$q_delta,exe$gl_tqfl->tqe$q_time);
  //  printk("+");
#endif
  while (tmp!=exe$gl_tqfl && t->tqe$q_time>tmp->tqe$q_time)
    tmp=tmp->tqe$l_tqfl;
  //  printk("-");
  insque(t,tmp->tqe$l_tqbl);
  //  printk("0");
#if 0
  if (times>=0 && times<5)  printk("%x %x %x\n",exe$gl_tqfl,exe$gl_tqfl->tqe$q_delta,exe$gl_tqfl->tqe$q_time);
#endif
  vmsunlock(&SPIN_TIMER,savipl);
}

void exe_std$rmvtimq(int acmode, int reqid, int remval, int ipid) {
  struct _tqe *tmp=exe$gl_tqfl->tqe$l_tqfl,*next;
  //int savipl=vmslock(&SPIN_TIMER,IPL$_TIMER);
  while (tmp && tmp!=exe$gl_tqfl) {
    next=tmp->tqe$l_tqfl;
    if (reqid && reqid==tmp->tqe$l_astprm) {
      remque(tmp,0);
      kfree(tmp);
    }
    if (ipid && ipid==tmp->tqe$l_pid) {
      if (tmp->tqe$b_rqtype==remval) {
	remque(tmp,0);
	kfree(tmp);
      }
    }
    tmp=next;
  cont:
    {
    }
  }
}

int exe_std$chkrdacces (struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb) {
  // doing nothing right now
  return SS$_NORMAL;
}

int exe_std$chkwrtacces (struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb) {
  // doing nothing right now
  return SS$_NORMAL;
}

int exe_std$debit_bytcnt_alo(int debit, struct _pcb *pcb, int *alosize_p,void **pool_p) {
  // not doing everything it should do
  *pool_p=kmalloc(debit,GFP_KERNEL);
  return SS$_NORMAL;
}

int exe$prober_simple(void * addr) {
  char * p = addr;
  char c = * p;
  return c;
}
