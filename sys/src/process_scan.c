// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/sched.h>
#include <system_data_cells.h>

// we will need struct _pscanctx soon

unsigned long pscan_id = 2;

int exe$pscan_next_id(struct _pcb ** p) {
  int i;
  unsigned long *vec=sch$gl_pcbvec;
  for (i=pscan_id;i<MAXPROCESSCNT;i++) {
    if (vec[i]==0) continue;
    *p=vec[i];
    pscan_id++;
    return 1;
  }
  pscan_id=2;
  return 0;
}
