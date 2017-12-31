#ifndef starlet_h
#define starlet_h

#include <zarg.h>

/*
** System Service Entry Points.
*/

struct _iosb;
struct _generic_64;
struct _lksb;

#define __unknown_params

int sys$testcode(void);

/*
** sys$setprn() - set process name.
**	prcnam - address of string descriptor for new process name
*/
int sys$setprn(/* const */ void *prcnam);

/*
** sys$setpri() - set process priority
**	pidadr - address of process id, or zero
**	prcnam - address of string descriptor of process name, or zero
**	pri - new base priority
**	prvpri - address to receive previous priority, or zero
**	pol - address of new scheduling policy, or zero
**	prvpol - address to receive previous sched policy or zero.
*/

int sys$setpri(unsigned int *pidadr, /* const */ void *prcnam,
               unsigned int pri, unsigned int *prvpri,
               unsigned int *pol, unsigned int *prvpol);

/*
** sys$dclast() - declare an AST routine to the current process
**	astadr - address of AST routine
**	astprm - ast parameter passed to AST routine
**	acmode - mode for AST routine.
*/

int sys$dclast(void (*astadr)(unsigned long),
               unsigned long astprm, unsigned int acmode);

int sys$hiber(void);

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

int sys$enqw  (unsigned int efn, unsigned int lkmode, struct _lksb *lksb, unsigned int flags, void *resnam, unsigned int parid, void (*astadr)(void), unsigned long astprm, void (*blkastadr)(void), unsigned int acmode, unsigned int rsdm_id);

int sys$enq  (unsigned int efn, unsigned int lkmode, struct _lksb *lksb, unsigned int flags, void *resnam, unsigned int parid, void (*astadr)(void), unsigned long astprm, void (*blkastadr)(void), unsigned int acmode, unsigned int rsdm_id);

int sys$deq(unsigned int lkid, void *valblk, unsigned int acmode, unsigned int flags);

int sys$getlki(unsigned int efn, unsigned int *lkidadr,void *itmlst, struct _iosb *iosb, void (*astadr)(int), int astprm, unsigned int reserved);

int sys$getlkiw(unsigned int efn, unsigned int *lkidadr,void *itmlst, struct _iosb *iosb, void (*astadr)(int), int astprm, unsigned int reserved);

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

int sys$dassgn(unsigned short int chan);

int sys$assign(void *devnam, unsigned short int *chan,unsigned int acmode, void *mbxnam,int flags);
#define sys$assign(...) sys$assign(_buildargz5(__VA_ARGS__))

int sys$qiow(unsigned int efn, unsigned short int chan, unsigned int func,
             struct _iosb *iosb, void(*astadr)(__unknown_params), long  astprm,
             void *p1, long p2, long  p3, long p4, long p5, long p6);

int sys$qio(unsigned int efn, unsigned short int chan, unsigned int func,
            struct _iosb *iosb, void(*astadr)(__unknown_params), long astprm,
            void *p1, long p2, long  p3, long p4, long p5, long p6);

int sys$clrast(void);

int sys$setast(char enbflg);

int sys$ascefc(unsigned int efn, void *name, char prot, char perm);

int sys$dacefc(unsigned int efn);

int sys$dlcefc(void *name);

int sys$crembx  (char prmflg, unsigned short int *chan, unsigned int maxmsg, unsigned int bufquo, unsigned int promsk, unsigned int acmode, void *lognam,long flags,...);

int sys$delmbx  (unsigned short int chan);

int sys$getdvi(unsigned int efn, unsigned short int chan, void *devnam, void *itmlst, struct _iosb *iosb, void (*astadr)(void), int astprm, unsigned long long *nullarg);

int sys$getdviw(unsigned int efn, unsigned short int chan, void *devnam, void *itmlst, struct _iosb *iosb, void (*astadr)(void), int astprm, unsigned long long *nullarg);

int sys$device_scan(void *return_devnam, unsigned short int *retlen, void *search_devnam, void *itmlst, unsigned long long *contxt);

int sys$mount(void *itmlst);

struct _fab;

int sys$close (struct _fab * fab, void * err, void * suc);
int sys$connect (struct _fab * fab, void * err, void * suc);
int sys$create (struct _fab * fab, void * err, void * suc);
int sys$delete (struct _fab * fab, void * err, void * suc);
int sys$disconnect (struct _fab * fab, void * err, void * suc);
int sys$display (struct _fab * fab, void * err, void * suc);
int sys$enter (struct _fab * fab, void * err, void * suc);
int sys$erase (struct _fab * fab, void * err, void * suc);
int sys$extend (struct _fab * fab, void * err, void * suc);
int sys$find (struct _fab * fab, void * err, void * suc);
int sys$flush (struct _fab * fab, void * err, void * suc);
int sys$free (struct _fab * fab, void * err, void * suc);
int sys$get (struct _fab * fab, void * err, void * suc);
int sys$modify (struct _fab * fab, void * err, void * suc);
int sys$nxtvol (struct _fab * fab, void * err, void * suc);
int sys$open (struct _fab * fab, void * err, void * suc);
int sys$parse (struct _fab * fab, void * err, void * suc);
int sys$put (struct _fab * fab, void * err, void * suc);
int sys$read (struct _fab * fab, void * err, void * suc);
int sys$release (struct _fab * fab, void * err, void * suc);
int sys$remove (struct _fab * fab, void * err, void * suc);
int sys$rename (struct _fab * fab, void * err, void * suc);
int sys$rewind (struct _fab * fab, void * err, void * suc);
int sys$search (struct _fab * fab, void * err, void * suc);
int sys$space (struct _fab * fab, void * err, void * suc);
int sys$truncate (struct _fab * fab, void * err, void * suc);
int sys$update (struct _fab * fab, void * err, void * suc);
int sys$wait (struct _fab * fab, void * err, void * suc);
int sys$write (struct _fab * fab, void * err, void * suc);
int sys$filescan (void *srcstr, void *valuelst, ...);
#define sys$filescan(...) sys$filescan(_buildargz5(__VA_ARGS__))
int sys$setddir2 (struct _fab * fab, void * err, void * suc);
int sys$setdfprot (struct _fab * fab, void * err, void * suc);
int sys$ssvexc (struct _fab * fab, void * err, void * suc);
int sys$rmsrundwn (struct _fab * fab, void * err, void * suc);
int sys$imgact(void * name, void * dflnam, void * hdrbuf, unsigned long imgctl, unsigned long long * inadr, unsigned long long * retadr, unsigned long long * ident, unsigned long acmode);
int sys$imgfix(void);
int sys$rundwn(int mode);
int sys$dclexh(void *desblk);
int sys$getsyi(unsigned int efn, unsigned int *csidadr, void *nodename, void *itmlst, struct _iosb *iosb, void (*astadr)(void), unsigned long astprm);
int sys$get_security(void *clsnam, void *objnam, unsigned int *objhan, unsigned int flags, void *itmlst, unsigned int *contxt, unsigned int *acmode);
int sys$creprc(unsigned int *pidadr, void *image, void *input, void *output, void *error, struct _generic_64 *prvadr, unsigned int *quota, void*prcnam, unsigned int baspri, unsigned int uic, unsigned short int mbxunt, unsigned int stsflg);
int sys$canwak(unsigned int *pidadr, void *prcnam);
int sys$setrwm(int flags);
int sys$getjpi(unsigned int efn, unsigned int *pidadr, void * prcnam, void *itmlst, struct _iosb *iosb, void (*astadr)(void), unsigned long long astprm);
int sys$getjpiw(unsigned int efn, unsigned int *pidadr, void * prcnam, void *itmlst, struct _iosb *iosb, void (*astadr)(void), unsigned long long astprm);
struct _va_range;
int sys$expreg(unsigned int pagcnt, struct _va_range *retadr,unsigned int acmode, char region);
int sys$deltva(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode);
int sys$setddir (void * newdiraddr, unsigned short int * lengthaddr, void * curdiraddr);
int sys$cmkrnl(int (*routin)(void), unsigned int *arglst);
int sys$cli(void * cliv, int par1, int par2);
int sys$getsyiw(unsigned int efn, unsigned int *csidadr, void *nodename, void *itmlst, struct _iosb *iosb, void (*astadr)(void), unsigned long astprm);
int sys$cancel (unsigned short int chan);
int sys$adjwsl  (signed int pagcnt, unsigned int *wsetlm);
int sys$set_security(void *clsnam, void *objnam, unsigned int *objhan, unsigned int flags, void *itmlst, unsigned int *contxt, unsigned int *acmode);
int sys$fao(void * ctrstr , short int * outlen , void * outbuf , ...);
int sys$faol(void * ctrstr , short int * outlen , void * outbuf , int * prmlst);
int sys$getuai(unsigned int efn, unsigned int *contxt, void *usrnam, void *itmlst, struct _iosb *iosb, void (*astadr)(__unknown_params), int astprm);
int sys$wake(unsigned long *pidadr, void *prcnam);
int sys$find_held  (struct _generic_64 *holder, unsigned int *id, unsigned int *attrib, unsigned int *contxt);
int sys$asctoid(void *name, unsigned int *id, unsigned int *attrib);
int sys$sndopr(void *msgbuf, unsigned short int chan);
ROT(x,y);

struct struct_crelnt
{
    unsigned int *attr;
    void *resnam;
    unsigned int *reslen;
    unsigned int *quota;
    unsigned short *promsk;
    void *tabnam;
    void *partab;
    unsigned char *acmode;
};

struct struct_setpri
{
    unsigned int *pidadr;
    void *prcnam;
    unsigned int pri;
    unsigned int *prvpri;
    unsigned int*pol;
    unsigned int *prvpol;
};

struct struct_qio
{
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

struct struct_enq
{
    unsigned int efn;
    unsigned int lkmode;
    struct _lksb *lksb;
    unsigned int flags;
    void *resnam;
    unsigned int parid;
    void (*astadr)(void);
    unsigned long astprm;
    void (*blkastadr)(void);
    unsigned int acmode;
    unsigned int rsdm_id;
    unsigned long null_arg;
};

struct struct_getlki
{
    unsigned int efn;
    unsigned int *lkidadr;
    void *itmlst;
    struct _iosb *iosb;
    void (*astadr)(int);
    int astprm;
    unsigned int reserved;
};

struct struct_crembx
{
    char prmflg;
    unsigned short int *chan;
    unsigned int maxmsg;
    unsigned int bufquo;
    unsigned int promsk;
    unsigned int acmode;
    void *lognam;
    long flags;
};

struct struct_mgblsc
{
    struct _va_range *inadr;
    struct _va_range *retadr;
    unsigned int acmode;
    unsigned int flags;
    void *gsdnam;
    struct _secid *ident;
    unsigned int relpag;
};

struct struct_crmpsc
{
    struct _va_range *inadr;
    struct _va_range *retadr;
    unsigned int acmode;
    unsigned int flags;
    void *gsdnam;
    unsigned long long * ident;
    unsigned int relpag;
    unsigned /*short*/ int chan;
    unsigned int pagcnt;
    unsigned int vbn;
    unsigned int prot;
    unsigned int pfc;
};

struct struct_create_region_32
{
    unsigned long length;
    unsigned int region_prot;
    unsigned int flags;
    unsigned long long *return_region_id;
    void **return_va;
    unsigned long *return_length;
    unsigned long start_va;
};

struct struct_args
{
    unsigned long s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16;
};

struct struct_getjpi
{
    unsigned int efn;
    unsigned int *pidadr;
    void * prcnam;
    void *itmlst;
    struct _iosb *iosb;
    void (*astadr)(void);
    unsigned long long astprm;
};

struct struct_getdvi
{
    unsigned int efn;
    unsigned short int chan;
    void *devnam;
    void *itmlst;
    struct _iosb *iosb;
    void (*astadr)(void);
    int astprm;
    unsigned long long *nullarg;
};

#endif
