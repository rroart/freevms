// $Id$
// $Locker$

// Author. Roar Thron�s.
/**
 \file exception_init.c
 \brief misc important hairy system routines
 \author Roar Thron�s
 */

#include <ssdef.h>
#include <starlet.h>
#include <sysdep.h>
#include <exe_routines.h>
#include <misc_routines.h>

#include <linux/linkage.h>
#include <asm/unistd.h>

/**
 \brief wait until event flag set and eventual iosb flag too - see 5.2 6.3.6.3 (recheck the implementation of this routine)
 \param efn event flag number
 \param iosb io status block
 */

int exe$synch(unsigned int efn, struct _iosb *iosb)
{
    if (!iosb)
    {
        sys$waitfr(efn);
        return SS$_NORMAL;
    }
    if (iosb->iosb$w_status)
    {
        return SS$_NORMAL;
    }

    while (1)
    {
        sys$waitfr(efn);
        if (iosb->iosb$w_status & 0xff)
        {
            return iosb->iosb$w_status;
        }
        exe$clref(efn);
    }
}

/**
 \brief to be executed after lower mode asts
 */

int exe$clrast(void)
{
    return cmod$astexit(); // check. should be system call?
#if 0
    printk("this does not work yet (how to implement?), and is strongly discouraged in real VMS too\n");
#endif
}

extern asmlinkage int exe$qio(unsigned int efn, unsigned short int chan, unsigned int func, struct _iosb *iosb,
        void (*astadr)(__unknown_params), long astprm, void*p1, long p2, long p3, long p4, long p5, long p6);
extern asmlinkage int exe$qiow(unsigned int efn, unsigned short int chan, unsigned int func, struct _iosb *iosb,
        void (*astadr)(__unknown_params), long astprm, void*p1, long p2, long p3, long p4, long p5, long p6);

#if (defined __i386__) || (defined __x86_64__)
int sys$qio(unsigned int efn, unsigned short int chan, unsigned int func, struct _iosb *iosb, void (*astadr)(__unknown_params),
        long astprm, void*p1, long p2, long p3, long p4, long p5, long p6)
{
    struct struct_qio s;
    s.efn = efn;
    s.chan = chan;
    s.func = func;
    s.iosb = iosb;
    s.astadr = astadr;
    s.astprm = astprm;
    s.p1 = p1;
    s.p2 = p2;
    s.p3 = p3;
    s.p4 = p4;
    s.p5 = p5;
    s.p6 = p6;
    //  return ({ unsigned int resultvar; asm volatile ( "bpushl .L__X'%k2, %k2\n\t" "bmovl .L__X'%k2, %k2\n\t" "movl %1, %%eax\n\t" "int $0x81\n\t" "bpopl .L__X'%k2, %k2\n\t" : "=a" (resultvar) : "i" (__NR_$qio) , "acdSD" (&s) : "memory", "cc"); if (resultvar >= 0xfffff001) { errno= (-resultvar); resultvar = 0xffffffff; } (int) resultvar; });
    return INLINE_SYSCALL($qio, 1, &s);
}

int sys$qiow(unsigned int efn, unsigned short int chan, unsigned int func, struct _iosb *iosb, void (*astadr)(__unknown_params),
        long astprm, void*p1, long p2, long p3, long p4, long p5, long p6)
{
    struct struct_qio s;
    s.efn = efn;
    s.chan = chan;
    s.func = func;
    s.iosb = iosb;
    s.astadr = astadr;
    s.astprm = astprm;
    s.p1 = p1;
    s.p2 = p2;
    s.p3 = p3;
    s.p4 = p4;
    s.p5 = p5;
    s.p6 = p6;
    //  return ({ unsigned int resultvar; asm volatile ( "bpushl .L__X'%k2, %k2\n\t" "bmovl .L__X'%k2, %k2\n\t" "movl %1, %%eax\n\t" "int $0x81\n\t" "bpopl .L__X'%k2, %k2\n\t" : "=a" (resultvar) : "i" (__NR_$qio) , "acdSD" (&s) : "memory", "cc"); if (resultvar >= 0xfffff001) { errno= (-resultvar); resultvar = 0xffffffff; } (int) resultvar; });
    return INLINE_SYSCALL($qiow, 1, &s);
}

int sys$waitfr(unsigned int efn)
{
    return INLINE_SYSCALL($waitfr, 1, efn);
}

int sys$hiber(void)
{
    return INLINE_SYSCALL($hiber, 0);
}

int sys$dclast(void (*astadr)(__unknown_params), unsigned long astprm, unsigned int acmode)
{
    return INLINE_SYSCALL($dclast, 3, astadr, astprm, acmode);
}

int sys$setast(char enbflg)
{
    return INLINE_SYSCALL($setast, 1, ((unsigned long) enbflg));
}

int sys$dassgn(unsigned short int chan)
{
    return INLINE_SYSCALL($dassgn, 1, chan);
}

int sys$clrast()
{
    return INLINE_SYSCALL($clrast, 0);
}

int sys$assign(void *devnam, unsigned short *chan, unsigned int acmode, void *mbxnam, unsigned int flags)
{
    return INLINE_SYSCALL($assign, 5, devnam, chan, acmode, mbxnam, flags);
}

int sys$trnlnm(unsigned int *attr, void *tabnam, void *lognam, unsigned char *acmode, void *itmlst)
{
    return INLINE_SYSCALL($trnlnm, 5, attr, tabnam, lognam, acmode, itmlst);
}

int sys$cmkrnl(int (*routin)(), unsigned int *arglst)
{
    return INLINE_SYSCALL($cmkrnl, 2, routin, arglst);
}

int sys$enq(unsigned int efn, unsigned int lkmode, struct _lksb *lksb, unsigned int flags, void *resnam, unsigned int parid,
        void (*astadr)(), unsigned long astprm, void (*blkastadr)(), unsigned int acmode, unsigned int rsdm_id)
{
    struct struct_args s;
    s.s1 = efn;
    s.s2 = lkmode;
    s.s3 = lksb;
    s.s4 = flags;
    s.s5 = resnam;
    s.s6 = parid;
    s.s7 = astadr;
    s.s8 = astprm;
    s.s9 = blkastadr;
    s.s10 = acmode;
    s.s11 = rsdm_id;
    //  s.null_arg=null_arg;
    return INLINE_SYSCALL($enq, 1, &s);
}

int sys$enqw(unsigned int efn, unsigned int lkmode, struct _lksb *lksb, unsigned int flags, void *resnam, unsigned int parid,
        void (*astadr)(), unsigned long astprm, void (*blkastadr)(), unsigned int acmode, unsigned int rsdm_id)
{
    struct struct_args s;
    s.s1 = efn;
    s.s2 = lkmode;
    s.s3 = lksb;
    s.s4 = flags;
    s.s5 = resnam;
    s.s6 = parid;
    s.s7 = astadr;
    s.s8 = astprm;
    s.s9 = blkastadr;
    s.s10 = acmode;
    s.s11 = rsdm_id;
    //  s.null_arg=null_arg;
    return INLINE_SYSCALL($enqw, 1, &s);
}

int sys$deq(unsigned int lkid, void *valblk, unsigned int acmode, unsigned int flags)
{
    return INLINE_SYSCALL($deq, 4, lkid, valblk, acmode, flags);
}
#endif

// not the right place, but the closest I could find

asmlinkage void exe$cmkrnl_not(int (*routine)(), int * args)
{
    //basically check privs
    //if ok, do an int 0x85 (new one) with routine and args as params
    //at int 0x85 I suppose privs must be checked again
    //then routine with args will be executed
    //I suppose a stupid cpu can do this
}

asmlinkage void exe$cmexec_not(int (*routine)(), int * args)
{
    //basically check privs
    //if ok, do an int 0x85 (new one) with routine and args as params
    //at int 0x85 I suppose privs must be checked again
    //then routine with args will be executed
    //I suppose a stupid cpu can do this
}
