#include "starlet.h"
#include "sysdep.h"
#include <asm/unistd.h>

int sys$testcode(void) {
  return INLINE_SYSCALLTEST($setprn,0);
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
  return INLINE_SYSCALL($crelnt,1,&s);
}

int sys$crelnm  (unsigned int *attr, void *tabnam, void *lognam, unsigned char *acmode, void *itmlst) {
  return INLINE_SYSCALL($crelnm,5,attr,tabnam,lognam,acmode,itmlst);
}

int sys$dellnm  (void *tabnam, void *lognam, unsigned char *acmode) {
  return INLINE_SYSCALL($dellnm,3,tabnam,lognam,acmode);
}

int sys$trnlnm  (unsigned int *attr, void *tabnam, void
		 *lognam, unsigned char *acmode, void *itmlst) {
  return INLINE_SYSCALL($trnlnm,5,attr,tabnam,lognam,acmode,itmlst);
}

int sys$dclast  ( void (*astadr)(unsigned long), unsigned long astprm, unsigned int acmode) { 
  return INLINE_SYSCALL($dclast,3,astadr,astprm,acmode);
}

int sys$waitfr  (unsigned int efn) {
  return INLINE_SYSCALL($waitfr,1,efn);
}

int sys$wfland  (unsigned int efn, unsigned int mask) {
  return INLINE_SYSCALL($wfland,2,efn,mask);
}

int sys$wflor  (unsigned int efn, unsigned int mask) {
  return INLINE_SYSCALL($wflor,2,efn,mask);
}

int sys$clref  (unsigned int efn) {
  return INLINE_SYSCALL($clref,1,efn);
}

int sys$setime  (unsigned long long  *timadr) {
  return INLINE_SYSCALL($setime,1,timadr);
}

int sys$setimr  (unsigned int efn, signed long long *daytim,
		 void (*astadr)(long), unsigned
		 long reqidt, unsigned int flags) {
  return INLINE_SYSCALL($setimr,5,efn,daytim,astadr,reqidt,flags);
}

int sys$cantim  (unsigned long long reqidt, unsigned int acmode) {
  return INLINE_SYSCALL($cantim,2,reqidt,acmode);
}

int sys$numtim  (unsigned short int timbuf [7], unsigned long long * timadr) {
  return INLINE_SYSCALL($numtim,2,timbuf,timadr);
}

int sys$schdwk(unsigned int *pidadr, void *prcnam, signed long long * daytim, signed long long * reptim) {
  return INLINE_SYSCALL($schdwk,4,pidadr,prcnam,daytim,reptim);
}

int sys$gettim (unsigned long long * timadr) {
  return INLINE_SYSCALL($gettim,1,timadr);
}

int sys$hiber(void) {
  return INLINE_SYSCALL($hiber,0);
}

int sys$wake(unsigned long *pidadr, void *prcnam) {
  return INLINE_SYSCALL($wake,2,pidadr,prcnam);
}

int sys$resume (unsigned int *pidadr, void *prcnam) {
  return INLINE_SYSCALL($resume,2,pidadr,prcnam);
}

int sys$exit(unsigned int code) {
  return INLINE_SYSCALL($exit,1,code);
}

int sys$forcex(unsigned int *pidadr, void *prcnam, unsigned int code) {
  return INLINE_SYSCALL($forcex,3,pidadr,prcnam,code);
}

int sys$setef(unsigned int efn) {
  return INLINE_SYSCALL($setef,1,efn);
}

int sys$readef(unsigned int efn, unsigned int *state) {
  return INLINE_SYSCALL($readef,2,efn,state);
}

int sys$synch(unsigned int efn, struct _iosb *iosb) {
  return INLINE_SYSCALL($synch,2,efn,iosb);
}

int sys$enq  (unsigned int efn, unsigned int lkmode, struct _lksb *lksb, unsigned int flags, void *resnam, unsigned int parid, void (*astadr)(), unsigned long astprm, void (*blkastadr)(), unsigned int acmode, unsigned int rsdm_id) {
  struct struct_enq s;
  s.efn=efn;
  s.lkmode=lkmode;
  s.lksb=lksb;
  s.flags=flags;
  s.resnam=resnam;
  s.parid=parid;
  s.astadr=astadr;
  s.astprm=astprm;
  s.blkastadr=blkastadr;
  s.acmode=acmode;
  s.rsdm_id=rsdm_id;
  //  s.null_arg=null_arg;
  return INLINE_SYSCALL($enq,1,&s);
}

int sys$enqw  (unsigned int efn, unsigned int lkmode, struct _lksb *lksb, unsigned int flags, void *resnam, unsigned int parid, void (*astadr)(), unsigned long astprm, void (*blkastadr)(), unsigned int acmode, unsigned int rsdm_id) {
  struct struct_enq s;
  s.efn=efn;
  s.lkmode=lkmode;
  s.lksb=lksb;
  s.flags=flags;
  s.resnam=resnam;
  s.parid=parid;
  s.astadr=astadr;
  s.astprm=astprm;
  s.blkastadr=blkastadr;
  s.acmode=acmode;
  s.rsdm_id=rsdm_id;
  //  s.null_arg=null_arg;
  return INLINE_SYSCALL($enqw,1,&s);
}

int sys$deq(unsigned int lkid, void *valblk, unsigned int acmode, unsigned int flags) {
  return INLINE_SYSCALL($deq,4,lkid,valblk,acmode,flags);
}

int sys$getlki(unsigned int efn, unsigned int *lkidadr,void *itmlst, struct _iosb *iosb, void (*astadr)(int), int astprm, unsigned int reserved) {
  struct struct_getlki s;
  s.efn=efn;
  s.lkidadr=lkidadr;
  s.itmlst=itmlst;
  s.iosb=iosb;
  s.astadr=astadr;
  s.astprm=astprm;
  s.reserved=reserved;
  return INLINE_SYSCALL($getlki,1,&s);
}

int sys$getlkiw(unsigned int efn, unsigned int *lkidadr,void *itmlst, struct _iosb *iosb, void (*astadr)(int), int astprm, unsigned int reserved) {
  struct struct_getlki s;
  s.efn=efn;
  s.lkidadr=lkidadr;
  s.itmlst=itmlst;
  s.iosb=iosb;
  s.astadr=astadr;
  s.astprm=astprm;
  s.reserved=reserved;
  return INLINE_SYSCALL($getlkiw,1,&s);
}

int sys$dassgn(unsigned short int chan) {
  return INLINE_SYSCALL($dassgn,1,chan); 
}

int sys$assign(void *devnam, unsigned short int *chan,unsigned int acmode, void *mbxnam,int flags) {
  return INLINE_SYSCALL($assign,5,devnam,chan,acmode,mbxnam,flags);
}

int sys$qiow(unsigned int efn, unsigned short int chan,unsigned int func, struct _iosb *iosb, void(*astadr)(__unknown_params), long  astprm, void*p1, long p2, long  p3, long p4, long p5, long p6) {
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
  return INLINE_SYSCALL($qiow,1,&s);
}

int sys$qio(unsigned int efn, unsigned short int chan,unsigned int func, struct _iosb *iosb, void(*astadr)(__unknown_params), long  astprm, void*p1, long p2, long  p3, long p4, long p5, long p6) {
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
  return INLINE_SYSCALL($qio,1,&s);
}

int sys$clrast(void) {
  return INLINE_SYSCALL($clrast,0);
}

int sys$setast(char enbflg) {
  return INLINE_SYSCALL($setast,1,((unsigned long)enbflg));
}

int sys$ascefc(unsigned int efn, void *name, char prot, char perm) {
  return INLINE_SYSCALL($ascefc,4,efn,name,(unsigned long) prot, (unsigned long) perm);
}

int sys$dacefc(unsigned int efn) {
  return INLINE_SYSCALL($dacefc,1,efn);
}

int sys$dlcefc(void *name) {
  return INLINE_SYSCALL($dlcefc,1,name);
}

int sys$asctim  (unsigned short int *timlen, void *timbuf,
		 unsigned long long *timadr, char cvtflg) {
  return INLINE_SYSCALL3($asctim,4,timlen,timbuf,timadr,(unsigned long)cvtflg);
}

int sys$bintim  (void *timbuf, unsigned long long *timadr) {
  return INLINE_SYSCALL3($bintim,2,timbuf,timadr);
}

