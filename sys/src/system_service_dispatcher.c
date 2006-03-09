// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/linkage.h>
#include <linux/sched.h>

#include <ipldef.h>
#include <system_data_cells.h>
#include <ipl.h>
#include <sch_routines.h>

asmlinkage int cmod$astexit() {
  struct _pcb * p = ctl$gl_pcb;
  setipl(IPL$_ASTDEL);
  // clear a pcb$l_astact bit
  test_and_clear_bit(p->psl_prv_mod, &p->pcb$b_astact); // check
  sch$newlvl(p);

}
