// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<misc.h>
#include<ssdef.h>
#include<system_data_cells.h>

asmlinkage int exe$exit(unsigned int code) {
#ifdef __x86_64__
  long dummy;
  __asm__ __volatile__ ("movq %%rsp,%0; ":"=r" (dummy) );
#endif
  struct _exh * exh = ctl$gl_thexec;
  if (exh) {
    ctl$gl_thexec = exh->exh$l_flink;
#ifdef __i386__
    char ** addr = ((long)&code) + 0x28;
    (*addr)=exh->exh$l_handler;
    if (exh->exh$l_first_arg) {
      addr = ((long)addr) + 0xc;
      *addr = exh->exh$l_first_arg + 4;
    }
#else
    char ** addr = ((long)dummy+0x60);
    (*addr)=exh->exh$l_handler;
    if (exh->exh$l_first_arg) {
      addr = ((long)addr) + 0x18;
      *addr = exh->exh$l_first_arg + 0x8; //0x28;
    }
#endif
    return SS$_NORMAL;
  } else {
#if 0
    return sys_exit(code);
#endif
    do_exit((code&0xff)<<8);
  }
}
