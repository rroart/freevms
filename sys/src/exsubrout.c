// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <system_data_cells.h>
#include <internals.h>
#include <ipldef.h>

void exe$instimq(struct _tqe * t) {
  static signed int times=-500;
  struct _tqe *tmp=exe$gl_tqfl->tqe$l_tqfl;
  int savipl=vmslock(&SPIN_TIMER,IPL$_TIMER);
  times++;
  if (times>=0 && times<5)     printk("%x %x %x\n",t,t->tqe$q_delta,t->tqe$q_time);
  if (times>=0 && times<5)     printk("%x %x %x\n",exe$gl_tqfl,exe$gl_tqfl->tqe$q_delta,exe$gl_tqfl->tqe$q_time);
  //  printk("+");
  while (tmp!=exe$gl_tqfl && t->tqe$q_time>tmp->tqe$q_time)
    tmp=tmp->tqe$l_tqfl;
  //  printk("-");
  insque(t,tmp->tqe$l_tqbl);
  //  printk("0");
  if (times>=0 && times<5)  printk("%x %x %x\n",exe$gl_tqfl,exe$gl_tqfl->tqe$q_delta,exe$gl_tqfl->tqe$q_time);
  vmsunlock(&SPIN_TIMER,savipl);
}

void exe$rmvtimq(unsigned long reqidt, unsigned int acmode) {
  struct _tqe *tmp=exe$gl_tqfl->tqe$l_tqfl;
  //int savipl=vmslock(&SPIN_TIMER,IPL$_TIMER);
  while (tmp!=exe$gl_tqfl) {
    if (reqidt==tmp->tqe$l_astprm) {
      remque(tmp,0);
      vfree(tmp);
    }
    tmp=tmp->tqe$l_tqfl;
  cont:
  }
}
