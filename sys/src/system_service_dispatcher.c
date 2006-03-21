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
#ifdef __i386__
  __asm__ __volatile__(
		       "addl $0x40, %esp\n\t" // check. rewind stack
		       );
#endif
#ifdef __x86_64__
  __asm__ __volatile__(
		       "addq $0x88, %rsp\n\t" // check. rewind stack
		       );
#endif
}
