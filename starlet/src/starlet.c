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
  return INLINE_SYSCALL($cantim,2,reqidt,acmode); /* fix this */
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

int sys$crembx  (char prmflg, unsigned short int *chan, unsigned int maxmsg, unsigned int bufquo, unsigned int promsk, unsigned int acmode, void *lognam,...) {
  struct struct_crembx s;
  s.prmflg=prmflg;
  s.chan=chan;
  s.maxmsg=maxmsg;
  s.bufquo=bufquo;
  s.promsk=promsk;
  s.acmode=acmode;
  s.lognam=lognam;
  return INLINE_SYSCALL($crembx,1,&s);
}

int sys$delmbx  (unsigned short int chan) {
  return INLINE_SYSCALL($delmbx,1,(unsigned long) chan);
}

int sys$mount(void *itmlst) {
  return INLINE_SYSCALL($mount,1,itmlst);
}

int sys$mgblsc(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode, unsigned int flags, void *gsdnam, struct _secid *ident, unsigned int relpag) {
  struct struct_mgblsc s;
  s.inadr=inadr;
  s.retadr=retadr;
  s.acmode=acmode;
  s.flags=flags;
  s.gsdnam=gsdnam;
  s.ident=ident;
  s.relpag=relpag;
  return INLINE_SYSCALL($mgblsc,1,&s);
}

int sys$crmpsc(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode, unsigned int flags, void *gsdnam, unsigned long long * ident, unsigned int relpag, unsigned /*short*/ int chan, unsigned int pagcnt, unsigned int vbn, unsigned int prot,unsigned int pfc) {
  struct struct_crmpsc s;
  s.inadr=inadr;
  s.retadr=retadr;
  s.acmode=acmode;
  s.flags=flags;
  s.gsdnam=gsdnam;
  s.ident=ident;
  s.relpag=relpag;
  s.chan=chan;
  s.pagcnt=pagcnt;
  s.vbn=vbn;
  s.prot=prot;
  s.pfc=pfc;
  return INLINE_SYSCALL($crmpsc,1,&s);
}

int sys$create_region_32  ( unsigned long length, unsigned int region_prot, unsigned int flags, unsigned long long *return_region_id, void **return_va, unsigned long *return_length, unsigned long start_va) {
  struct struct_create_region_32 s;
  s.length=length;
  s.region_prot=region_prot;
  s.flags=flags;
  s.return_region_id=return_region_id;
  s.return_va=return_va;
  s.return_length=return_length;
  s.start_va=start_va;
  return INLINE_SYSCALL($create_region_32,1,&s);
}

int sys$close (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($close,3,fab,err,suc); }
int sys$connect (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($connect,3,fab,err,suc); }
int sys$create (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($create,3,fab,err,suc); }
int sys$delete (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($delete,3,fab,err,suc); }
int sys$disconnect (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($disconnect,3,fab,err,suc); }
int sys$display (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($display,3,fab,err,suc); }
int sys$enter (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($enter,3,fab,err,suc); }
int sys$erase (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($erase,3,fab,err,suc); }
int sys$extend (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($extend,3,fab,err,suc); }
int sys$find (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($find,3,fab,err,suc); }
int sys$flush (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($flush,3,fab,err,suc); }
int sys$free (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($free,3,fab,err,suc); }
int sys$get (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($get,3,fab,err,suc); }
int sys$modify (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($modify,3,fab,err,suc); }
int sys$nxtvol (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($nxtvol,3,fab,err,suc); }
int sys$open (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($open,3,fab,err,suc); }
int sys$parse (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($parse,3,fab,err,suc); }
int sys$put (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($put,3,fab,err,suc); }
int sys$read (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($read,3,fab,err,suc); }
int sys$release (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($release,3,fab,err,suc); }
int sys$remove (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($remove,3,fab,err,suc); }
int sys$rename (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($rename,3,fab,err,suc); }
int sys$rewind (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($rewind,3,fab,err,suc); }
int sys$search (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($search,3,fab,err,suc); }
int sys$space (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($space,3,fab,err,suc); }
int sys$truncate (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($truncate,3,fab,err,suc); }
int sys$update (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($update,3,fab,err,suc); }
int sys$wait (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($wait,3,fab,err,suc); }
int sys$write (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($write,3,fab,err,suc); }
int sys$filescan (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($filescan,3,fab,err,suc); }
int sys$setddir (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($setddir,3,fab,err,suc); }
int sys$setdfprot (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($setdfprot,3,fab,err,suc); }
int sys$ssvexc (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($ssvexc,3,fab,err,suc); }
int sys$rmsrundwn (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($rmsrundwn,3,fab,err,suc); }

int sys$asctim  (unsigned short int *timlen, void *timbuf,
		 unsigned long long *timadr, char cvtflg) {
  return INLINE_SYSCALL3($asctim,4,timlen,timbuf,timadr,(unsigned long)cvtflg);
}

int sys$bintim  (void *timbuf, unsigned long long *timadr) {
  return INLINE_SYSCALL3($bintim,2,timbuf,timadr);
}

