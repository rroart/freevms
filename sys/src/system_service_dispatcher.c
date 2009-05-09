// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file system_service_dispatcher.c
   \brief dispatch system services
   \author Roar Thronæs
 */

#include <linux/linkage.h>
#include <linux/sched.h>

#include <ipldef.h>
#include <system_data_cells.h>
#include <ipl.h>
#include <sch_routines.h>

/**
   \brief run after ast called in lower level modes - see 5.2 7.5.3
   \details clears astact bit and rewinds stack
*/

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
