#include<linux/kernel.h>
#include<linux/sched.h>
#include<spldef.h>
#include<asm/bitops.h>
#include<system_data_cells.h>

inline int smp$acquire(struct _spl * spl) {
  // remember to do a smp enabled check
 again:
  {}
  int bit = test_and_set_bit(0,&spl->spl$l_spinlock);
  if (bit) {
    if (spl->spl$l_own_cpu==smp$gl_cpu_data[ctl$gl_pcb->pcb$l_cpu_id]) {
      spl->spl$l_own_cnt++;
    } else {
      while (test_bit(0,&spl->spl$l_spinlock)) {
      }
      goto again;
    }
  } else {
    spl->spl$l_own_cpu=smp$gl_cpu_data[ctl$gl_pcb->pcb$l_cpu_id];
    spl->spl$l_own_cnt++;
#if 0
    // check where the book wanted these
    int intr;
    local_irq_save(intr);
    local_irq_disable();
    local_irq_restore(intr);
#endif
  }
}

inline int smp$restore(struct _spl * spl) {
  // remember to do a smp enabled check
  spl->spl$l_own_cnt--;
  if (spl->spl$l_own_cnt==-1) {
    spl->spl$l_own_cpu = 0;
    clear_bit(0,&spl->spl$l_spinlock);
  }
}
 
