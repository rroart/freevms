#include "starlet.h"
#include "sysdep.h"
#include <asm/unistd.h>

int sys$testcode(void) {
  return INLINE_SYSCALL2($setprn,0);
}

int sys$setprn  ( void *prcnam) { 
  return INLINE_SYSCALL($setprn,1,prcnam);
}

int sys$setpri( unsigned int * pidadr, void * prcnam, unsigned int pri, unsigned int * prvpri, unsigned int * pol, unsigned int * prvpol) {
  struct struct_setpri s;
  s.pidadr=pidadr;
  s.prcnam=prcnam;
  s.pri=pri;
  s.prvpri=prvpri;
  s.pol=pol;
  s.prvpol=prvpol;

  return INLINE_SYSCALL($setpri,1,&s);
  //  return INLINE_SYSCALL($setpri,6,pidadr,prcnam,pri,prvpri,pol,prvpol);
  /*return r;*/
  return 1;
};


 int sys$crelnt  (unsigned int *attr, void *resnam, unsigned
                         int *reslen, unsigned int *quota,
                unsigned short *promsk, void *tabnam, void
		  *partab, unsigned char *acmode) {

   struct struct_crelnt s;
   s.attr=attr;
   s.resnam=resnam;
   s.reslen=reslen;
   s.quota=quota;
   s.promsk=promsk;
   s.tabnam=tabnam;
   s.partab=partab;
   s.acmode=acmode;
   
}

