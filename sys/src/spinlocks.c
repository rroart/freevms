#include<linux/kernel.h>
#include<linux/sched.h>
#include<spldef.h>
#include<asm/bitops.h>
#include<system_data_cells.h>

// define SPINDEF in spinlocks_mon?
#undef SPINDEB
#define SPINDEB

#ifdef SPINDEB
static int spinc = 0;
long spin[1024];
#endif

inline int smp$acquire(struct _spl * spl) {
  // remember to do a smp enabled check
#ifdef SPINDEB
  long * l = &spl;
  spin[spinc++]=l[-1];
  spin[spinc++]=spl;
  spin[spinc++]=ctl$gl_pcb;
  spin[spinc++]=l;
  spin[spinc++]=smp_processor_id();
  spin[spinc++]=spl->spl$l_own_cnt;
  spin[spinc++]=ctl$gl_pcb->psl_ipl;
  spin[spinc++]=0;
  if(spinc>1000) spinc=0;
#endif
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
#ifdef SPINDEB
    if (spl->spl$l_own_cpu==0)
      panic("cpu 0\n");
#endif
    spl->spl$l_own_cnt++;
#ifdef SPINDEB
    if (spl->spl$l_own_cnt==1) {
      panic("cnt 1\n");
#endif
    }
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
 
