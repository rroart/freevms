#ifndef starlet_h
#define starlet_h

int sys$testcode(void); 

int sys$setprn  ( void *prcnam);

int sys$setpri(unsigned int *pidadr, void *prcnam, unsigned int pri, unsigned int *prvpri, unsigned int*pol, unsigned int *prvpol);

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
