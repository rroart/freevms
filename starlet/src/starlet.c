#define sys$filescan sys$filescan_not
#include "starlet.h"
#include "sysdep.h"
#include "../../linux/include/asm-i386/unistd.h"
#ifdef compile_um
#include "unistdum.h"
#endif
#include<stdarg.h>

#include <iosbdef.h>
#include <ssdef.h>

#ifndef __x86_64__
int sys$testcode(void) {
  return INLINE_SYSCALLTEST($setprn,0);
}
#endif

#ifdef __x86_64__
static int syscall_struct() {}
#endif

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

#ifdef __x86_64__
  syscall_struct();
#endif
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
#ifdef __x86_64__
  syscall_struct();
#endif
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
  return INLINE_SYSCALL($cantim,2,(long)reqidt,acmode); /* fix this */
}

int sys$numtim  (unsigned short int timbuf [7], unsigned long long * timadr) {
  return INLINE_SYSCALL($numtim,2,timbuf,timadr);
}

int sys$schdwk(unsigned int *pidadr, void *prcnam, signed long long * daytim, signed long long * reptim) {
  return INLINE_SYSCALL($schdwk,4,pidadr,prcnam,daytim,reptim);
}

int sys$canwak(unsigned int *pidadr, void *prcnam) {
  return INLINE_SYSCALL($canwak,2,pidadr,prcnam);
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

int sys$setrwm(int flags) {
  return INLINE_SYSCALL($setrwm,1,flags);
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
  struct struct_args s;
  s.s1=efn;
  s.s2=lkmode;
  s.s3=lksb;
  s.s4=flags;
  s.s5=resnam;
  s.s6=parid;
  s.s7=astadr;
  s.s8=astprm;
  s.s9=blkastadr;
  s.s10=acmode;
  s.s11=rsdm_id;
  //  s.null_arg=null_arg;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($enq,1,&s);
}

int sys$enqw  (unsigned int efn, unsigned int lkmode, struct _lksb *lksb, unsigned int flags, void *resnam, unsigned int parid, void (*astadr)(), unsigned long astprm, void (*blkastadr)(), unsigned int acmode, unsigned int rsdm_id) {
  struct struct_args s;
  s.s1=efn;
  s.s2=lkmode;
  s.s3=lksb;
  s.s4=flags;
  s.s5=resnam;
  s.s6=parid;
  s.s7=astadr;
  s.s8=astprm;
  s.s9=blkastadr;
  s.s10=acmode;
  s.s11=rsdm_id;
  //  s.null_arg=null_arg;
#ifdef __x86_64__
  syscall_struct();
#endif
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
#ifdef __x86_64__
  syscall_struct();
#endif
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
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($getlkiw,1,&s);
}

int sys$dassgn(unsigned short int chan) {
  return INLINE_SYSCALL($dassgn,1,chan); 
}

int sys$assign(void *devnam, unsigned short int *chan,unsigned int acmode, void *mbxnam,int flags) {
  return INLINE_SYSCALL($assign,5,devnam,chan,acmode,mbxnam,flags);
}

static int exe$synch(unsigned int efn, struct _iosb *iosb) {
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
  sys$clref(efn);
  goto again;
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
#ifdef __x86_64__
  syscall_struct();
#endif
  int status = INLINE_SYSCALL($qio,1,&s);
  if ((status&1)==0) return status;
  return exe$synch(efn,iosb);
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
#ifdef __x86_64__
  syscall_struct();
#endif
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

int sys$crembx  (char prmflg, unsigned short int *chan, unsigned int maxmsg, unsigned int bufquo, unsigned int promsk, unsigned int acmode, void *lognam,long flags,...) {
  struct struct_crembx s;
  s.prmflg=prmflg;
  s.chan=chan;
  s.maxmsg=maxmsg;
  s.bufquo=bufquo;
  s.promsk=promsk;
  s.acmode=acmode;
  s.lognam=lognam;
  s.flags=flags;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($crembx,1,&s);
}

int sys$delmbx  (unsigned short int chan) {
  return INLINE_SYSCALL($delmbx,1,(unsigned long) chan);
}

int sys$mount(void *itmlst) {
  return INLINE_SYSCALL($mount,1,itmlst);
}

int sys$cretva (struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode) {
  return INLINE_SYSCALL($cretva,3,inadr,retadr,acmode);
}

int sys$expreg(unsigned int pagcnt, struct _va_range *retadr,unsigned int acmode, char region) {
  return INLINE_SYSCALL($expreg,4,pagcnt,retadr,acmode,(int)region);
}
int sys$deltva(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode) {
  return INLINE_SYSCALL($deltva,3,inadr,retadr,acmode);
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
#ifdef __x86_64__
  syscall_struct();
#endif
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
#ifdef __x86_64__
  syscall_struct();
#endif
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
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($create_region_32,1,&s);
}

int sys$getjpi(unsigned int efn, unsigned int *pidadr, void * prcnam, void *itmlst, struct _iosb *iosb, void (*astadr)(), unsigned long long astprm) {
  struct struct_getjpi s;
  s.efn=efn;
  s.pidadr=pidadr;
  s.prcnam=prcnam;
  s.itmlst=itmlst;
  s.iosb=iosb;
  s.astadr=astadr;
  s.astprm=astprm;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($getjpi,1,&s);
}

int sys$getjpiw(unsigned int efn, unsigned int *pidadr, void * prcnam, void *itmlst, struct _iosb *iosb, void (*astadr)(), unsigned long long astprm) {
  struct struct_getjpi s;
  s.efn=efn;
  s.pidadr=pidadr;
  s.prcnam=prcnam;
  s.itmlst=itmlst;
  s.iosb=iosb;
  s.astadr=astadr;
  s.astprm=astprm;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($getjpiw,1,&s);
}

int sys$sndopr(void *msgbuf, unsigned short int chan) {
  return INLINE_SYSCALL($sndopr,2,msgbuf,(int)chan);
}

int sys$process_affinity (unsigned int *pidadr, void *prcnam, struct _generic_64 *select_mask, struct _generic_64 *modify_mask, struct _generic_64 *prev_mask, struct _generic_64 *flags,...) {
  struct struct_args s;
  s.s1 = pidadr;
  s.s2 = prcnam;
  s.s3 = select_mask;
  s.s4 = modify_mask;
  s.s5 = prev_mask;
  s.s6 = flags;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($process_affinity, 1, &s);
}

int sys$set_implicit_affinity (unsigned int *pidadr, void *prcnam, struct _generic_64 *state, int cpu_id, struct _generic_64 *prev_mask) {
  return INLINE_SYSCALL($set_implicit_affinity, 5, pidadr, prcnam, state, cpu_id, prev_mask);
}

int sys$cpu_capabilities (int cpu_id, struct _generic_64 *select_mask, struct _generic_64 *modify_mask, struct _generic_64 *prev_mask, struct _generic_64 *flags) {
  return INLINE_SYSCALL($cpu_capabilities, 5, cpu_id, select_mask, modify_mask, prev_mask, flags);
}

int sys$process_capabilities (unsigned int *pidadr, void *prcnam, struct _generic_64 *select_mask, struct _generic_64 *modify_mask, struct _generic_64 *prev_mask, struct _generic_64 *flags) {
  struct struct_args s;
  s.s1 = pidadr;
  s.s2 = prcnam;
  s.s3 = select_mask;
  s.s4 = modify_mask;
  s.s5 = prev_mask;
  s.s6 = flags;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($process_capabilities, 1, &s);
}

int sys$getdvi(unsigned int efn, unsigned short int chan, void *devnam, void *itmlst, struct _iosb *iosb, void (*astadr)(), int astprm, unsigned long long *nullarg) {
  struct struct_getdvi s;
  s.efn=efn;
  s.chan=chan;
  s.devnam=devnam;
  s.itmlst=itmlst;
  s.iosb=iosb;
  s.astadr=astadr;
  s.astprm=astprm;
  s.nullarg=nullarg;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($getdvi,1,&s);
}

int sys$getdviw(unsigned int efn, unsigned short int chan, void *devnam, void *itmlst, struct _iosb *iosb, void (*astadr)(), int astprm, unsigned long long *nullarg) {
  struct struct_getdvi s;
  s.efn=efn;
  s.chan=chan;
  s.devnam=devnam;
  s.itmlst=itmlst;
  s.iosb=iosb;
  s.astadr=astadr;
  s.astprm=astprm;
  s.nullarg=nullarg;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($getdviw,1,&s);
}

int sys$fao(void * ctrstr , short int * outlen , void * outbuf , ...) {
  struct struct_args s;
  va_list args;
  long * argv = (long *)&s;
  int argc=0;
  va_start(args,outbuf);
  while(argc<15) { // check. should be 17.
    *argv=va_arg(args,long);
    argv++;
    argc++;
  }
  va_end(args);
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($faol,4,ctrstr,outlen,outbuf,&s); // need not call fao?
}

int sys$faol(void * ctrstr , short int * outlen , void * outbuf , int * prmlst) {
  return INLINE_SYSCALL($faol,4,ctrstr,outlen,outbuf,prmlst);
}

int sys$device_scan(void *return_devnam, unsigned short int *retlen, void *search_devnam, void *itmlst, unsigned long long *contxt) {
  return INLINE_SYSCALL($device_scan,5,return_devnam,retlen,search_devnam,itmlst,contxt);
}

int sys$imgact(void * name, void * dflnam, void * hdrbuf, unsigned long imgctl, unsigned long long * inadr, unsigned long long * retadr, unsigned long long * ident, unsigned long acmode) {
  struct struct_args s;
  s.s1 = (unsigned long) name;
  s.s2 = (unsigned long) dflnam;
  s.s3 = (unsigned long) hdrbuf;
  s.s4 = (unsigned long) imgctl;
  s.s5 = (unsigned long) inadr;
  s.s6 = (unsigned long) retadr;
  s.s7 = (unsigned long) ident;
  s.s8 = (unsigned long) acmode;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($imgact,1,&s);
}

int sys$imgfix() {
  return INLINE_SYSCALL($imgfix,0);
}

int sys$imgsta(void * transfer, void * parseinfo, void * header, void * file, unsigned long linkstatus, unsigned long clistatus) {
  struct struct_args s;
  s.s1 = (unsigned long) transfer;
  s.s2 = (unsigned long) parseinfo;
  s.s3 = (unsigned long) header;
  s.s4 = (unsigned long) file;
  s.s5 = (unsigned long) linkstatus;
  s.s6 = (unsigned long) clistatus;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($imgsta,1,&s);
}

int sys$rundwn(int mode) {
  return INLINE_SYSCALL($rundwn,1,mode);
}

int sys$purgws  (struct _va_range *inadr) { 
  return INLINE_SYSCALL($purgws,1,inadr);
}

int sys$adjwsl  (signed int pagcnt, unsigned int *wsetlm) { 
  return INLINE_SYSCALL($adjwsl,2,pagcnt, wsetlm);
}

int sys$lkwset(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode) {
  return INLINE_SYSCALL($lkwset,3,inadr,retadr,acmode);
}

int sys$lckpag(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode) {
  return INLINE_SYSCALL($lckpag,3,inadr,retadr,acmode);
}

int sys$ulwset(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode) {
  return INLINE_SYSCALL($ulwset,3,inadr,retadr,acmode);
}

int sys$ulkpag(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode) {
  return INLINE_SYSCALL($ulkpag,3,inadr,retadr,acmode);
}

int sys$creprc(unsigned int *pidadr, void *image, void *input, void *output, void *error, struct _generic_64 *prvadr, unsigned int *quota, void*prcnam, unsigned int baspri, unsigned int uic, unsigned short int mbxunt, unsigned int stsflg) {
  struct struct_args s;
  s.s1 = (unsigned long) pidadr;
  s.s2 = (unsigned long) image;
  s.s3 = (unsigned long) input;
  s.s4 = (unsigned long) output;
  s.s5 = (unsigned long) error;
  s.s6 = (unsigned long) prvadr;
  s.s7 = (unsigned long) quota;
  s.s8 = (unsigned long) prcnam;
  s.s9 = (unsigned long) baspri;
  s.s10 = (unsigned long) uic;
  s.s11 = (unsigned long) mbxunt;
  s.s12 = (unsigned long) stsflg;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($creprc,1,&s);
}

int sys$getsyi(unsigned int efn, unsigned int *csidadr, void *nodename, void *itmlst, struct _iosb *iosb, void (*astadr)(), unsigned long astprm) {
  struct struct_args s;
  s.s1 = (unsigned long) efn;
  s.s2 = (unsigned long) csidadr;
  s.s3 = (unsigned long) nodename;
  s.s4 = (unsigned long) itmlst;
  s.s5 = (unsigned long) iosb;
  s.s6 = (unsigned long) astadr;
  s.s7 = (unsigned long) astprm;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($getsyi,1,&s);
}

int sys$getsyiw(unsigned int efn, unsigned int *csidadr, void *nodename, void *itmlst, struct _iosb *iosb, void (*astadr)(), unsigned long astprm) {
  struct struct_args s;
  s.s1 = (unsigned long) efn;
  s.s2 = (unsigned long) csidadr;
  s.s3 = (unsigned long) nodename;
  s.s4 = (unsigned long) itmlst;
  s.s5 = (unsigned long) iosb;
  s.s6 = (unsigned long) astadr;
  s.s7 = (unsigned long) astprm;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($getsyiw,1,&s);
}

int sys$set_security(void *clsnam, void *objnam, unsigned int *objhan, unsigned int flags, void *itmlst, unsigned int *contxt, unsigned int *acmode) {
  struct struct_args s;
  s.s1 = (unsigned long) clsnam;
  s.s2 = (unsigned long) objnam;
  s.s3 = (unsigned long) objhan;
  s.s4 = (unsigned long) flags;
  s.s5 = (unsigned long) itmlst;
  s.s6 = (unsigned long) contxt;
  s.s7 = (unsigned long) acmode;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($set_security,1,&s);
}

int sys$get_security(void *clsnam, void *objnam, unsigned int *objhan, unsigned int flags, void *itmlst, unsigned int *contxt, unsigned int *acmode) {
  struct struct_args s;
  s.s1 = (unsigned long) clsnam;
  s.s2 = (unsigned long) objnam;
  s.s3 = (unsigned long) objhan;
  s.s4 = (unsigned long) flags;
  s.s5 = (unsigned long) itmlst;
  s.s6 = (unsigned long) contxt;
  s.s7 = (unsigned long) acmode;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($get_security,1,&s);
}

int sys$cli(void * cliv, int par1, int par2) {
  return INLINE_SYSCALL($cli,3,cliv, par1, par2);
}

int sys$setprv (char enbflg, struct _generic_64 *prvadr, char prmflg, struct _generic_64 *prvprv) {
  return INLINE_SYSCALL($setprv,4,enbflg, prvadr, prmflg, prvprv);
}

int sys$cancel (unsigned short int chan) {
  return INLINE_SYSCALL($cancel,1,(int)chan);
}

int sys$cmkrnl(int (*routin)(), unsigned int *arglst) {
  return INLINE_SYSCALL($cmkrnl,2,routin,arglst);
}

int sys$asctoid(void *name, unsigned int *id, unsigned int *attrib) {
  return INLINE_SYSCALL($asctoid,3,name,id,attrib);
} 
     
int sys$find_held  (struct _generic_64 *holder, unsigned int *id, unsigned int *attrib, unsigned int *contxt) {
  return INLINE_SYSCALL($find_held,4,holder,id,attrib,contxt);
}     

int sys$setddir (void * newdiraddr, unsigned short int * lengthaddr, void * curdiraddr) { return INLINE_SYSCALL1($setddir,3,newdiraddr,lengthaddr,curdiraddr); }
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
#undef sys$filescan
int sys$filescan (void *srcstr, void *valuelst, unsigned int *fldflags, void *auxout, unsigned short int *retlen) { return INLINE_SYSCALL1($filescan,5,srcstr,valuelst,fldflags,auxout,retlen); }
int sys$setddir2 (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($setddir,3,fab,err,suc); }
int sys$setdfprot (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($setdfprot,3,fab,err,suc); }
int sys$ssvexc (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($ssvexc,3,fab,err,suc); }
int sys$rmsrundwn (struct _fab * fab, void * err, void * suc) { return INLINE_SYSCALL1($rmsrundwn,3,fab,err,suc); }

int sys$getuai(unsigned int efn, unsigned int *contxt, void *usrnam, void *itmlst, struct _iosb *iosb, void (*astadr)(__unknown_params), int astprm) {
  struct struct_args s;
  s.s1 = (unsigned long) efn;
  s.s2 = (unsigned long) contxt;
  s.s3 = (unsigned long) usrnam;
  s.s4 = (unsigned long) itmlst;
  s.s5 = (unsigned long) iosb;
  s.s6 = (unsigned long) astadr;
  s.s7 = (unsigned long) astprm;
#ifdef __x86_64__
  syscall_struct();
#endif
  return INLINE_SYSCALL($getuai,1,&s);
}

int sys$asctim  (unsigned short int *timlen, void *timbuf,
		 unsigned long long *timadr, char cvtflg) {
  return INLINE_SYSCALL3($asctim,4,timlen,timbuf,timadr,(unsigned long)cvtflg);
}

int sys$bintim  (void *timbuf, unsigned long long *timadr) {
  return INLINE_SYSCALL3($bintim,2,timbuf,timadr);
}

int sys$dclexh(void *desblk) {
  return INLINE_SYSCALL($dclexh,1,desblk);
}

#if 0
struct _fabdef cc$rms_fab = {NULL,0,NULL,NULL,0,0,0,0,0,0,0,0,0,0,0,0,0,NULL};
struct _namdef cc$rms_nam = {0,0,0,0,0,0,0,0,0,0,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,0,0};
struct _xabdatdef cc$rms_xabdat = {XAB$C_DAT,0,
				   0, 0, 0, 0,
			       VMSTIME_ZERO, VMSTIME_ZERO,
			       VMSTIME_ZERO, VMSTIME_ZERO,
			       VMSTIME_ZERO, VMSTIME_ZERO};
struct _xabfhcdef cc$rms_xabfhc = {XAB$C_FHC,0,0,0,0,0,0,0,0,0,0,0};
struct _xabprodef1 cc$rms_xabpro = {XAB$C_PRO,0,0,0};
struct _rabdef cc$rms_rab = {NULL,NULL,NULL,NULL,0,0,0,{0,0,0}};
struct _xabkeydef cc$rms_xabkey = {XAB$C_KEY,XAB$C_KEYLEN};
#endif

void signal(int s) {
  extern void exit(int);
  exit(s);
}
