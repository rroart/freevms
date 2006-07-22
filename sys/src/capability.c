// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <far_pointers.h>
#include <linux/sched.h>
#include <ssdef.h>
#include <cpbdef.h>
#include <ipldef.h>
#include <statedef.h>
#include <internals.h>
#include <smp_routines.h>
#include <asm/hw_irq.h>

int sch$clear_affinity(unsigned int cpu_mask, struct _pcb *pcb, unsigned int flags, UINT64_PQ prev_mask_p);

int sch$add_cpu_cap(unsigned int cpu_id, unsigned int mask, UINT64_PQ prev_mask_p) {
  if (prev_mask_p)
    *prev_mask_p = smp$gl_cpu_data[cpu_id]->cpu$l_capability;
  smp$gl_cpu_data[cpu_id]->cpu$l_capability |= mask;
  sch$al_cpu_cap[cpu_id]=smp$gl_cpu_data[cpu_id]->cpu$l_capability;
  sch$gl_capability_sequence++;
  return SS$_NORMAL;
}

int sch$clear_capability(struct _pcb *pcb, unsigned int mask, unsigned int cpu_id, unsigned int flags, UINT64_PQ prev_mask_p);

int sch$set_affinity(unsigned int cpu_mask, struct _pcb *pcb, unsigned int flags, UINT64_PQ prev_mask_p);

sch$remove_cpu_cap(unsigned int cpu_id, unsigned int mask, UINT64_PQ prev_mask_p) {
  if (prev_mask_p)
    *prev_mask_p = smp$gl_cpu_data[cpu_id]->cpu$l_capability;
  smp$gl_cpu_data[cpu_id]->cpu$l_capability &= ~mask;
  sch$al_cpu_cap[cpu_id]=smp$gl_cpu_data[cpu_id]->cpu$l_capability;
  sch$gl_capability_sequence++;
  struct _pcb * pcb = smp$gl_cpu_data[cpu_id]->cpu$l_curpcb;
  if ((pcb->pcb$l_capability & smp$gl_cpu_data[cpu_id]->cpu$l_capability) != pcb->pcb$l_capability) {
  if (ctl$gl_pcb->pcb$l_cpu_id!=cpu_id)
    smp_send_work(CPU$M_RESCHED, cpu_id);
  else
    SOFTINT_RESCHED_VECTOR;
  }
  return SS$_NORMAL;
}

int sch$set_capability(struct _pcb *pcb, unsigned int mask, unsigned int cpu_id, unsigned int flags, UINT64_PQ prev_mask_p);

int sch$acquire_affinity(struct _pcb *pcb, int obsolete, int cpu_id) { // check implicit name
  pcb->pcb$b_affinity_skip = sch$gl_affinity_skip;
  pcb->pcb$l_capability |= CPB$M_IMPLICIT_AFFINITY;
  pcb->pcb$l_affinity = cpu_id;
  if (pcb->pcb$w_state == SCH$C_CUR) {
    if (pcb->pcb$l_affinity != pcb->pcb$l_cpu_id) {
      if (ctl$gl_pcb->pcb$l_cpu_id!=pcb->pcb$l_cpu_id)
	smp_send_work(CPU$M_RESCHED, pcb->pcb$l_cpu_id);
      else
	SOFTINT_RESCHED_VECTOR;
    }
  }
  return SS$_NORMAL;
}

int sch$release_affinity(struct _pcb *pcb) { // check _implicit name
  pcb->pcb$l_capability &= ~CPB$M_IMPLICIT_AFFINITY; // check. enough?
  return SS$_NORMAL;
}

int sch$calculate_affinity(int mask) {
  int i;
  int new_mask = 0;
  for(i=0; i<32; i++) {
    if ((mask & smp$gl_cpu_data[i]->cpu$l_capability) == mask)
      new_mask |= 1 << i;
  }
  return new_mask;
}

int scs_std$change_affinity( struct _ucb *ucb_p );

int sch$release_capability(struct _pcb *pcb, unsigned int mask, unsigned int cpu_id, unsigned int flags, UINT64_PQ prev_mask_p) {
  pcb->pcb$l_capability &= ~mask;
  pcb->pcb$l_current_affinity = sch$calculate_affinity(pcb->pcb$l_capability);
  return SS$_NORMAL;
}

int sch$require_capability(struct _pcb *pcb, unsigned int mask, unsigned int cpu_id, unsigned int flags, UINT64_PQ prev_mask_p) {
  int oldipl = vmslock(&SPIN_SCHED, IPL$_SCHED);
  if (prev_mask_p)
    *prev_mask_p = pcb->pcb$l_capability;
  pcb->pcb$l_capability |= mask; // check order
  // do pcb$l_permanant_capability?
#if 0
  if (*prev_mask_p != pcb->pcb$l_capability) {
    smp$gl_cpu_data[pcb->pcb$l_affinity]->cpu$l_hardaff++;
    smp$gl_cpu_data[cpu_id]->cpu$l_hardaff++;
  }
#endif
  pcb->pcb$l_affinity = cpu_id;
  if (flags & CPB$M_FLAG_PERMANENT)
    pcb->pcb$l_permanent_cpu_affinity = pcb->pcb$l_affinity;
  pcb->pcb$l_current_affinity = sch$calculate_affinity(pcb->pcb$l_capability);
  if (ctl$gl_pcb == pcb) {
    if ((pcb->pcb$l_current_affinity & (1 << pcb->pcb$l_cpu_id)) == 0) {
      if (pcb->pcb$l_affinity != pcb->pcb$l_cpu_id) {
	if (ctl$gl_pcb->pcb$l_cpu_id!=pcb->pcb$l_cpu_id)
	  smp_send_work(CPU$M_RESCHED, pcb->pcb$l_cpu_id);
	else
	  SOFTINT_RESCHED_VECTOR;
      }
    }
  }
  vmsunlock(&SPIN_SCHED, oldipl);
  return SS$_NORMAL;
}

