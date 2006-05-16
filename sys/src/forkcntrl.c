// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<system_data_cells.h>
#include<ucbdef.h>
#include<irpdef.h>
#include<fkbdef.h>
#include<ipldef.h>
#include<ipl.h>
#include<internals.h>
#include <linux/smp.h>
#include <asm/current.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/hw_irq.h>
#include <queue.h>

asmlinkage void exe$forkdspth(int i);
void exe$fork(struct _irp * i, struct _ucb * u);
void exe$queue_fork(struct _irp * i, struct _ucb * u);

struct fork_lock_struct {
  struct _spl * spin;
  int ipl;
};

// call this smp$al_iplvec?
static struct fork_lock_struct forklock_table[7]={
  { &SPIN_IOLOCK8, IPL$_IOLOCK8 },
  { 0, 0 },
  { &SPIN_IOLOCK8, IPL$_IOLOCK8 },
  { &SPIN_IOLOCK9, IPL$_IOLOCK9 },
  { &SPIN_IOLOCK10, IPL$_IOLOCK10 },
  { &SPIN_IOLOCK11, IPL$_IOLOCK11 },
  { &SPIN_MAILBOX, IPL$_MAILBOX }
};

int inline forklock(int i, signed int j) {
  if (j==-2) j=forklock_table[i-6].ipl;
  return vmslock(forklock_table[i-6].spin, j);
}

void inline forkunlock(int i, signed int j) {
  if (j==-2) j=forklock_table[i-6].ipl;
  vmsunlock(forklock_table[i-6].spin, j);
}

asmlinkage void exe$frkipl6dsp(void) {
  exe$forkdspth(6);
}

asmlinkage void exe$frkipl8dsp(void) {
  exe$forkdspth(8);
}

asmlinkage void exe$frkipl9dsp(void) {
  exe$forkdspth(9);
}

asmlinkage void exe$frkipl10dsp(void) {
  exe$forkdspth(10);
}

asmlinkage void exe$frkipl11dsp(void) {
  exe$forkdspth(11);
}

asmlinkage void exe$forkdspth(int i) {
  void (*func)(void *,void *, void *);
  struct _fkb * f, * dummy = 0, * fq;
#ifdef __x86_64__
  if (intr_blocked(i))
    return;
  regtrap(REG_INTR, i);
#endif
  setipl(i);
  //printk("forkdspth\n");
  fq=smp$gl_cpu_data[smp_processor_id()]->cpu$q_swiqfl[i-6]; /* so far */
  while (!aqempty(fq)) {
    f=remque(fq,dummy);
    //printk("forking entry %x\n",f);
    func=f->fkb$l_fpc;
    vmslock(forklock_table[i-6].spin, -1);
    func(f->fkb$l_fr3,f->fkb$l_fr4,f);
    vmsunlock(forklock_table[i-6].spin, -1);
    fq=smp$gl_cpu_data[smp_processor_id()]->cpu$q_swiqfl[0]; /* so far */
  }
}

void exe$iofork(struct _irp * i, struct _ucb * u) {
  u->ucb$l_sts&=~UCB$M_TIM;
  exe$fork(i,u);
}

void exe$fork(struct _irp * i, struct _ucb * u) {
  u->ucb$l_fr3=i;
  /* should also popl fkb$l_fpc(r5) according to the book. */
  exe$queue_fork(i,u);
}

void exe$queue_fork(struct _irp * i, struct _ucb * u) {
  int curipl;
  int newipl;
  int isempty;
  struct _fkb * f=u;
  /* I think that the below is really an fkb */
  /* need caller and caller's caller address of return again */
  // u->ucb$l_fr4=current; // wrong?
  newipl=f->fkb$b_flck;
  f=smp$gl_cpu_data[smp_processor_id()]->cpu$q_swiqfl[newipl-6];
  isempty=aqempty(f);
  insque(u,f->fkb$l_fqbl);
  if (isempty) {
    /* do SOFTINTS */
    switch (newipl) {
    case IPL$_IOLOCK11:
      SOFTINT_IOLOCK11_VECTOR;
      break;
    case IPL$_IOLOCK10:
      SOFTINT_IOLOCK10_VECTOR;
      break;
    case IPL$_IOLOCK9:
      SOFTINT_IOLOCK9_VECTOR;
      break;
    case IPL$_IOLOCK8:
      SOFTINT_IOLOCK8_VECTOR;
      break;
    case IPL$_QUEUEAST:
      SOFTINT_QUEUEAST_VECTOR;
      break;
    default:
      panic("in forkdspth\n");
    }
  }
}

void exe_std$queue_fork(struct _fkb * fkb) {
  exe$queue_fork(0,fkb);
}

// replaces exe$fork
void exe_std$primitive_fork(long fr3, long fr4, struct _fkb * fkb) {
  fkb->fkb$l_fr3=fr3;
  fkb->fkb$l_fr4=fr4;
  exe_std$queue_fork(fkb);
}
