// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <ssdef.h>
#include <starlet.h>
#include <iosbdef.h>

#include<linux/linkage.h>
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
  printk("this does not work yet (how to implement?), and is strongly discouraged in real VMS too\n");
}

extern /*asmlinkage*/ int exe$qio(unsigned int efn, unsigned /*short*/ int chan,unsigned int func, struct _iosb *iosb, void(*astadr)(__unknown_params), long  astprm, void*p1, long p2, long  p3, long p4, long p5, long p6);
extern /*asmlinkage*/ int exe$qiow(unsigned int efn, unsigned /*short*/ int chan,unsigned int func, struct _iosb *iosb, void(*astadr)(__unknown_params), long  astprm, void*p1, long p2, long  p3, long p4, long p5, long p6);

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

int sys$hiber(void) {
  return INLINE_SYSCALL($hiber,0);
}

int sys$dclast(void (*astadr)(__unknown_params), unsigned long astprm, unsigned int acmode) {
  return INLINE_SYSCALL($dclast,3,astadr,astprm,acmode);
}

int sys$setast(char enbflg) {
  return INLINE_SYSCALL($setast,1,((unsigned long)enbflg));
}

int sys$dassgn(unsigned short int chan) {
  return INLINE_SYSCALL($dassgn,1,chan); 
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
  sts=exe$qio(efn,chan,func,iosb,astadr,astprm,p1,p2,p3,p4,p5,p6);
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
  sts=exe$qiow(efn,chan,func,iosb,astadr,astprm,p1,p2,p3,p4,p5,p6);
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

int sys$hiber(void) {
  int sts;
  pushpsl();
  sts=exe$hiber();
  myrei();
  return sts;
}

int sys$dclast(void (*astadr)(__unknown_params), unsigned long astprm, unsigned int acmode) {
  int sts;
  pushpsl();
  sts=exe$dclast(astadr,astprm,acmode);
  myrei();
  return sts;
}
#endif

// not the right place, but the closest I could find

asmlinkage void exe$cmkrnl_not(int (*routine)(),int * args) {
  //basically check privs
  //if ok, do an int 0x85 (new one) with routine and args as params
  //at int 0x85 I suppose privs must be checked again
  //then routine with args will be executed
  //I suppose a stupid cpu can do this
}

asmlinkage void exe$cmexec_not(int (*routine)(),int * args) {
  //basically check privs
  //if ok, do an int 0x85 (new one) with routine and args as params
  //at int 0x85 I suppose privs must be checked again
  //then routine with args will be executed
  //I suppose a stupid cpu can do this
}

