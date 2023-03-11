// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <ssdef.h>
#include <descrip.h>
#include <starlet.h>
#include <misc.h>
#include <cli$routines.h>

void spin_lock(int * i) { }

void spin_unlock(int * i) { }

void write_lock(int * i) { }

void write_unlock(int * i) { }

#define __KERNEL__
#define __ASM_SPINLOCK_H
typedef struct { } spinlock_t;
typedef struct { } rwlock_t;
spinlock_t mylock;
#define SPIN_LOCK_UNLOCKED mylock
#include <linux/types.h>
#include <linux/posix_types.h>
#include <linux/sched.h>
#include <system_data_cells.h>
#undef __KERNEL__

#if 0
// makes overlapping type trouble
#include <stdio.h>
#include <stdlib.h>
#else
int printf(const char *format, ...);
long int strtol(const char *nptr, char **endptr, int base);
#endif

int sda$getmem(const void *src, void *dest, int n);
int sda$getmemlong(const void *src, void *dest);

#if 0
// not yet
int sda$find_name(char * name)
{
    sts=exe$nampid(current,pidadr,prcnam,&retpcb,&ipid,&epid);
    vmsunlock(&SPIN_SCHED,0);
}
#endif

int sda$show_process(int mask)
{
    int retlen;
    $DESCRIPTOR(p, "p1");
    $DESCRIPTOR(i, "index");
    char c[80];
    struct dsc$descriptor o;
    o.dsc$a_pointer=c;
    o.dsc$w_length=80;
    memset (c, 0, 80);
    int sts = cli$present(&p);
    if (sts&1)
    {
        sts = cli$get_value(&p, &o, &retlen);
        o.dsc$w_length=retlen;
    }
    int index = 2;
    sts = cli$present(&i);
    if (sts&1)
    {
        sts = cli$get_value(&i, &o, &retlen);
        index = strtol(c, 0, 16);
    }
    else
    {

    }

#ifdef __x86_64__
    // do it like this until address perms set
    unsigned long *vecp=&sch$gl_pcbvec;
    unsigned long *vec;
    sda$getmemlong(vecp, &vec);
#else
    unsigned long *vec=sch$gl_pcbvec;
#endif
    struct _pcb * pcb_p;
    struct _pcb pcb;
    sda$getmemlong(&vec[index], &pcb_p);
    sda$getmem(pcb_p, &pcb, sizeof (pcb));

    printf("  Process index: %4x   Name: %15s  Extended PID: %8x\n", index, pcb.pcb$t_lname, pcb.pcb$l_epid);
    printf("--------------------------------------------------------------------\n");
#if 0
    printf("Process status:          %8x\n", pcb.pcb$l_sts);
    printf("        status2:         %8x\n\n", pcb.pcb$l_sts2);
#endif
#ifdef __x86_64__
    printf("PCB address      %16lx    JIB address              %16lx\n",pcb_p,pcb.pcb$l_jib);
    printf("PHD address      %16lx    Swapfile disk address    %16lx\n",pcb.pcb$l_phd,0);
#else
    printf("PCB address              %8x    JIB address              %8x\n",pcb_p,pcb.pcb$l_jib);
    printf("PHD address              %8x    Swapfile disk address    %8x\n",pcb.pcb$l_phd,0);
#endif
#if 0
    printf("KTB vector address       %8x    HWPCB address   %8x.%8x\n");
    printf("Callback vector address  %8x    Termination mailbox          %8x\n");
    printf("Master internal PID      %8x    Subprocess count                1\n");
    printf("Creator extended PID     %8x    Creator internal PID     %8x\n");
#endif
    printf("Previous CPU Id          %8x    Current CPU Id           %8x\n", -1, pcb.pcb$l_cpu_id);
#if 0
    printf("Previous ASNSEQ  0000000000000013    Previous ASN     000000000000003C\n");
#endif
    printf("Initial process priority       %2x    # open files remaining         93/100\n", pcb.pcb$b_prib);
#if 0
    printf("Delete pending count            0    Direct I/O count/limit        150/150\n");
    printf("    UIC                [00001,000004]    Buffered I/O count/limit      150/150\n");
    printf("Abs time of last event   %8x    BUFIO byte count/limit      96992/96992\n");
    printf("# of threads                    1    ASTs remaining                246/250\n");
    printf("Swapped copy of LEFC0    %8x    Timer entries remaining        20/20\n");
    printf("Swapped copy of LEFC1    %8x    Active page table count         0\n");
    printf("Global cluster 2 pointer %8x    Process WS page count         373\n");
    printf("Global cluster 3 pointer %8x    Global WS page count           53\n");
    printf("PCB Specific Spinlock    %8x    Subprocesses in job             1\n");
#endif
    printf("       Current capabilities:    System: %8x\n", pcb.pcb$l_capability, 0);
#if 0
    printf("       User:   %8x\n");
#endif
    printf("       Permanent capabilities:  System: %8x\n", pcb.pcb$l_permanent_capability, 0);
#if 0
    printf("       User:   %8x\n");
#endif
    printf("       Current affinities:      %8x\n", pcb.pcb$l_current_affinity);
    printf("       Permanent affinities:    %8x\n", pcb.pcb$l_affinity);
#if 0
    printf("       Thread status:           %8x  \n");
    printf("              status2:          %8x  \n");
    printf("\n");
    printf("KTB address              %8x    HWPCB address   %8x.%8x\n");
    printf("PKTA address             %8x    Callback vector address  %8x\n");
    printf("Internal PID             %8x    Callback error           %8x\n");
    printf("Extended PID             %8x    Current CPU id           %8x\n");
#endif
    printf("State                          %2x    Flags                    %8x\n", pcb.pcb$w_state, pcb.flags);
    printf("Base priority                  %2x    Current priority               %2x\n", pcb.pcb$b_prib, pcb.pcb$b_pri);
#if 0
    printf("Waiting EF cluster              1    Event flag wait mask     %8x\n");
    printf("CPU since last quantum       %8x    Mutex count                     0\n");
#endif
#if 0
    printf("ASTs active                  NONE\n");
#endif
    printf("\n");
}

void __this_fixmap_does_not_exist(void) { }
