// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <ssdef.h>
#include <starlet.h>
#include <iosbdef.h>

#include<asm/unistd.h>
#include "../../starlet/src/sysdep.h"

int exe$synch(unsigned int efn, struct _iosb *iosb) {
  if (!iosb) {
    sys$waitfr(efn);
    return SS$_NORMAL;
  }
  if (iosb->iosb$w_status) {
    return SS$_NORMAL;
  }
 again:
  sys$waitfr(efn);
  if (iosb->iosb$w_status & 0xff)
    return iosb->iosb$w_status;
  exe$clref(efn);
  goto again;
}

int exe$clrast(void) {
  printk("this does not work yet (how to implement?), and is strong discouraged in real VMS too\n");
}

#ifdef __i386__
int sys$qio(unsigned int efn, unsigned short int chan,unsigned int func, struct _iosb *iosb, void(*astadr)(__unknown_params), long astprm, void*p1, long p2, long p3, long p4, long p5, long p6) {
  struct struct_qio s;
  s.efn=efn;
  s.chan=chan;
  s.func=func;
  s.iosb=iosb;
  s.astadr=astadr;
  s.astprm=astprm;
  s.p1=p1;
  s.p2=p2;
  s.p3=p3;
  s.p4=p4;
  s.p5=p5;
  s.p6=p6;
  //  return ({ unsigned int resultvar; asm volatile ( "bpushl .L__X'%k2, %k2\n\t" "bmovl .L__X'%k2, %k2\n\t" "movl %1, %%eax\n\t" "int $0x81\n\t" "bpopl .L__X'%k2, %k2\n\t" : "=a" (resultvar) : "i" (__NR_$qio) , "acdSD" (&s) : "memory", "cc"); if (resultvar >= 0xfffff001) { errno= (-resultvar); resultvar = 0xffffffff; } (int) resultvar; });
  return INLINE_SYSCALL($qio,1,&s);
}

int sys$qiow(unsigned int efn, unsigned short int chan,unsigned int func, struct _iosb *iosb, void(*astadr)(__unknown_params), long astprm, void*p1, long p2, long p3, long p4, long p5, long p6) {
  struct struct_qio s;
  s.efn=efn;
  s.chan=chan;
  s.func=func;
  s.iosb=iosb;
  s.astadr=astadr;
  s.astprm=astprm;
  s.p1=p1;
  s.p2=p2;
  s.p3=p3;
  s.p4=p4;
  s.p5=p5;
  s.p6=p6;
  //  return ({ unsigned int resultvar; asm volatile ( "bpushl .L__X'%k2, %k2\n\t" "bmovl .L__X'%k2, %k2\n\t" "movl %1, %%eax\n\t" "int $0x81\n\t" "bpopl .L__X'%k2, %k2\n\t" : "=a" (resultvar) : "i" (__NR_$qio) , "acdSD" (&s) : "memory", "cc"); if (resultvar >= 0xfffff001) { errno= (-resultvar); resultvar = 0xffffffff; } (int) resultvar; });
  return INLINE_SYSCALL($qiow,1,&s);
}

int sys$waitfr(unsigned int efn) {
  return INLINE_SYSCALL($waitfr,1,efn);
}
#endif

#ifdef __arch_um__
int sys$qio(unsigned int efn, unsigned short int chan,unsigned int func, struct
	     _iosb *iosb, void(*astadr)(__unknown_params), long  astprm, void*p1, long p2, long  p3, long p4, long p5, long p6) {
  int sts;
  struct struct_qio s;
  s.efn=efn;
  s.chan=chan;
  s.func=func;
  s.iosb=iosb;
  s.astadr=astadr;
  s.astprm=astprm;
  s.p1=p1;
  s.p2=p2;
  s.p3=p3;
  s.p4=p4;
  s.p5=p5;
  s.p6=p6;
  //  return ({     unsigned int resultvar; asm volatile (  "bpushl .L__X'%k2, %k2\n\t"     "bmovl .L__X'%k2, %k2\n\t"      "movl %1, %%eax\n\t"    "int $0x80\n\t" "bpopl .L__X'%k2, %k2\n\t"      : "=a" (resultvar)      : "i" (__NR_$qio  ) , "acdSD" (  &s  )  : "memory", "cc");    if (resultvar >= 0xfffff001) {       errno= (-resultvar);    resultvar = 0xffffffff; }       (int) resultvar; }) ;
  //  return INLINE_SYSCALL($qio,1,&s); // did not work?
  pushpsl();
  sts=exe$qio(&s);
  myrei();
  return sts;
}

int sys$qiow(unsigned int efn, unsigned short int chan,unsigned int func, struct
	     _iosb *iosb, void(*astadr)(__unknown_params), long  astprm, void*p1, long p2, long  p3, long p4, long p5, long p6) {
  int sts;
  struct struct_qio s;
  s.efn=efn;
  s.chan=chan;
  s.func=func;
  s.iosb=iosb;
  s.astadr=astadr;
  s.astprm=astprm;
  s.p1=p1;
  s.p2=p2;
  s.p3=p3;
  s.p4=p4;
  s.p5=p5;
  s.p6=p6;
  //  return ({     unsigned int resultvar; asm volatile (  "bpushl .L__X'%k2, %k2\n\t"     "bmovl .L__X'%k2, %k2\n\t"      "movl %1, %%eax\n\t"    "int $0x80\n\t" "bpopl .L__X'%k2, %k2\n\t"      : "=a" (resultvar)      : "i" (__NR_$qio  ) , "acdSD" (  &s  )  : "memory", "cc");    if (resultvar >= 0xfffff001) {       errno= (-resultvar);    resultvar = 0xffffffff; }       (int) resultvar; }) ;
  //  return INLINE_SYSCALL($qio,1,&s); // did not work?
  pushpsl();
  sts=exe$qiow(&s);
  myrei();
  return sts;
}

int sys$waitfr(unsigned int efn) {
  int sts;
  pushpsl();
  sts=exe$waitfr(efn);
  myrei();
  return sts;
}

#endif

