// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<ssdef.h>

#include<linux/linkage.h>
#include<linux/sched.h>

#include<system_data_cells.h>

#include <tqedef.h>
#include <exe_routines.h>

asmlinkage int exe$cantim(unsigned long reqidt, unsigned int acmode){
  exe_std$rmvtimq(acmode,reqidt,0,0);
  return SS$_NORMAL;
}

asmlinkage int exe$canwak(unsigned int *pidadr, void *prcnam){
  int ipid=ctl$gl_pcb->pcb$l_pid;
  exe_std$rmvtimq(0,0,TQE$C_WKSNGL,ipid);
  return SS$_NORMAL;
}
