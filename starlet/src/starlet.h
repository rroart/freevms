#ifndef starlet_h
#define starlet_h

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


#endif
