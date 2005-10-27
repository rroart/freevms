// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<misc.h>
#include<ssdef.h>
#include<system_data_cells.h>

asmlinkage int exe$exit(unsigned int code) {
  struct _exh * exh = ctl$gl_thexec;
  if (exh) {
    ctl$gl_thexec = exh->exh$l_flink;
    char ** addr = ((long)&code) + 0x28;
    (*addr)=exh->exh$l_handler;
    if (exh->exh$l_first_arg) {
      addr = ((long)addr) + 0xc;
      *addr = exh->exh$l_first_arg + 4;
    }
    return SS$_NORMAL;
  } else {
#if 0
    return sys_exit(code);
#endif
    do_exit((code&0xff)<<8);
  }
}
