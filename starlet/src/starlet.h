#ifndef starlet_h
#define starlet_h

#include"misc.h"
#include"lksbdef.h"

/* rename eventually to sys$routines.h */

int sys$testcode(void); 

int sys$setprn  ( void *prcnam);

int sys$setpri(unsigned int *pidadr, void *prcnam, unsigned int pri, unsigned int *prvpri, unsigned int*pol, unsigned int *prvpol);

int sys$dclast( void (*astadr)(unsigned long), unsigned long astprm, unsigned int acmode);

int sys$waitfr(unsigned int efn);

int sys$wfland(unsigned int efn, unsigned int mask);

int sys$wflor  (unsigned int efn, unsigned int mask);

int sys$clref  (unsigned int efn);

int sys$setime  (unsigned long long  *timadr);

int sys$setimr  (unsigned int efn, signed long long *daytim,
		 void (*astadr)(long), unsigned
		 long reqidt, unsigned int flags);

int sys$cantim  (unsigned long long reqidt, unsigned int acmode);

int sys$numtim  (unsigned short int timbuf [7], unsigned long long * timadr);

int sys$gettim (unsigned long long * timadr);

int sys$schdwk(unsigned int *pidadr, void *prcnam, signed long long * daytim, signed long long * reptim);

int sys$resume (unsigned int *pidadr, void *prcnam);

int sys$exit(unsigned int code);

int sys$forcex(unsigned int *pidadr, void *prcnam, unsigned int code);

int sys$setef(unsigned int efn);

int sys$synch(unsigned int efn, struct _iosb *iosb);

int sys$readef(unsigned int efn, unsigned int *state);

int sys$enq  (unsigned int efn, unsigned int lkmode, struct _lksb *lksb, unsigned int flags, void *resnam, unsigned int parid, void (*astadr)(), unsigned long astprm, void (*blkastadr)(), unsigned int acmode, unsigned int rsdm_id);

int exe$deq(unsigned int lkid, void *valblk, unsigned int acmode, unsigned int flags);

int sys$asctim  (unsigned short int *timlen, void *timbuf,
		 unsigned long long *timadr, char cvtflg);

int sys$bintim  (void *timbuf, unsigned long long *timadr);
 int sys$crelnm  (unsigned int *attr, void *tabnam, void *lognam, unsigned char *acmode, void *itmlst);

 int sys$crelnt  (unsigned int *attr, void *resnam, unsigned
                         int *reslen, unsigned int *quota,
                unsigned short *promsk, void *tabnam, void
                         *partab, unsigned char *acmode);

 int sys$dellnm  (void *tabnam, void *lognam, unsigned char *acmode);

 int sys$trnlnm  (unsigned int *attr, void *tabnam, void
		  *lognam, unsigned char *acmode, void *itmlst);

struct struct_crelnt {
 unsigned int *attr;
 void *resnam;
 unsigned int *reslen;
 unsigned int *quota;
 unsigned short *promsk;
 void *tabnam;
 void *partab;
 unsigned char *acmode;
};

struct struct_setpri {
unsigned int *pidadr;
void *prcnam;
 unsigned int pri;
 unsigned int *prvpri;
 unsigned int*pol;
 unsigned int *prvpol;
};

struct struct_qio {
  unsigned int efn;
  unsigned short int chan;
  unsigned int func;
  struct _iosb *iosb;
  void (*astadr)(long);
  long astprm;
  void *p1; 
  long p2;
  long p3;
  long p4;
  long p5;
  long p6;
};

#endif

struct struct_enq {
  unsigned int efn;
  unsigned int lkmode;
  struct _lksb *lksb;
  unsigned int flags;
  void *resnam;
  unsigned int parid;
  void (*astadr)();
  unsigned long astprm;
  void (*blkastadr)();
  unsigned int acmode;
  unsigned int rsdm_id;
  unsigned long null_arg;
};
