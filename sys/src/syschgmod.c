// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>
#include<prvdef.h>
#include<ssdef.h>

asmlinkage int exe$cmkrnl(int (*routin)(__unknown_params), unsigned int *arglst) {
  struct _pcb * p = ctl$gl_pcb;
  // also check prev mode
  if ((p->pcb$l_priv&PRV$M_CMKRNL)==0)
    return SS$_NOPRIV;
  return routin(arglst[1],arglst[2],arglst[3],arglst[4],arglst[5],arglst[6],arglst[7],arglst[8],arglst[9],arglst[10],arglst[11],arglst[12],arglst[13],arglst[14],arglst[15]);
}
 
asmlinkage int exe$cmexec(int (*routin)(__unknown_params), unsigned int *arglst) {
  struct _pcb * p = ctl$gl_pcb;
  // also check prev mode
  if ((p->pcb$l_priv&PRV$M_CMEXEC)==0)
    return SS$_NOPRIV;
  return routin(arglst[1],arglst[2],arglst[3],arglst[4],arglst[5],arglst[6],arglst[7],arglst[8],arglst[9],arglst[10],arglst[11],arglst[12],arglst[13],arglst[14],arglst[15]);
}
 
