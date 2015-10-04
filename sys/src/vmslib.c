// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <ssdef.h>

#include <descrip.h>

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <misc_routines.h>

#include <stdarg.h>

#ifdef NOKERNEL
#define printk printf
#define kfree free
#define kmalloc malloc
#endif

long CH$PTR(long X)
{
    return X;
}

long CH$MOVE(long size, long src, long addr)
{
    memcpy(addr,src, size);
    return addr+size;
    printk("CH$MOVE not implemented\n");
}

find_cpu_data(long * l)
{
    int cpuid = smp_processor_id();
    * l=smp$gl_cpu_data[cpuid];
}

//ENTRY  SWAPBYTES,^M<>
int swapbytes(long WrdCnt, long Start)
{
    int *R0;
    unsigned char * R1 = Start;         // starting word address.
Swp_Loop:
    R0 = *R1;               // low ==> temp
    *R1 = R1[1];            // high ==> low
    R1++;
    *R1++ = R0;         // temp ==> High
    if  (--WrdCnt) goto Swp_Loop;       // decr word's left to do
    return R0;
}

long get_eip(long l)
{
    long * m = &l;
#ifdef __i386__
    return m[-1];
#else
#if 0
    asm volatile("movq %%rip, %0\n\t" :"=m" (m));
    return m;
#endif
    return m[1];
#endif
}
