#include<linux/linkage.h>
#include"../../freevms/sys/src/system_data_cells.h"
#include"../../freevms/lib/src/ucbdef.h"
#include"../../freevms/lib/src/irpdef.h"
#include"../../freevms/lib/src/fkbdef.h"
#include"../../freevms/lib/src/ipldef.h"
#include"../../freevms/pal/src/ipl.h"
#include <linux/smp.h>
#include <asm/current.h>
#include <linux/kernel.h>
#include <asm/hw_irq.h>

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
  void (*func)(void *,void *);
  struct _fkb * f, * dummy, * fq;
  if (intr_blocked(i))
    return;
  regtrap(REG_INTR, i);
  setipl(i);
  printk("forkdspth\n");
  fq=smp$gl_cpu_data[smp_processor_id()]->cpu$q_swiqfl[i-6]; /* so far */
  while (!aqempty(fq)) {
    f=remque(fq,dummy);
    printk("forking entry %x\n",f);
    func=f->fkb$l_fpc;
    func(f->fkb$l_fr3,f);
    fq=smp$gl_cpu_data[smp_processor_id()]->cpu$q_swiqfl[0]; /* so far */
  }
}

void exe$iofork(struct _irp * i, struct _ucb * u) {
  int curipl;
  int newipl;
  int isempty;
  struct _fkb * f=u;
  /* I think that the below is really an fkb */
  /* need caller and caller's caller address of return again */
  u->ucb$l_sts&=~UCB$M_TIM;
  u->ucb$l_fr3=i;
  u->ucb$l_fr4=current;
  newipl=u->ucb$b_dipl;
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
