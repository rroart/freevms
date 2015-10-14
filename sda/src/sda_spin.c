// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <stdio.h>
#include <string.h>

#include <ssdef.h>
#include <descrip.h>
#include <spldef.h>
#include <starlet.h>
#include <misc.h>
#include <cli$routines.h>

#include <system_data_cells.h>

int sda$getmem(const void *src, void *dest, int n);

static int print_spin(char * name, void * addr)
{
    struct _spl spl;
    sda$getmem(addr, &spl, sizeof(spl));
    printf("%15s                        Address        %8lx\n", name, (unsigned long) addr);
    printf("Owner CPU ID       %8x            IPL            %8x\n", spl.spl$l_own_cpu, spl.spl$l_ipl);
    printf("Ownership Depth    %8x            Rank           %8x\n", spl.spl$l_own_cnt, spl.spl$l_rank);
#if 0
    printf("Timeout Interval   %8x            Share Array    %8x\n\n", spl.spl$l_timo_int, spl.spl$ps_share_array);
#endif
}

int sda$show_spinlocks(int mask)
{
    int retlen;
    $DESCRIPTOR(p, "p1");
    char c[80];
    struct dsc$descriptor o;
    o.dsc$a_pointer = c;
    o.dsc$w_length = 80;
    memset(c, 0, 80);
    int sts = cli$present(&p);
    if (sts & 1)
    {
        sts = cli$get_value(&p, &o, &retlen);
        o.dsc$w_length = retlen;
    }

    printf("System static spinlock structures\n");
    printf("---------------------------------\n");
#if 0
    print_spin("EMB", &SPIN_EMB);
#endif
    print_spin("ATOMIC", &SPIN_ATOMIC);
#if 0
    print_spin("MCHECK", &SPIN_MCHECK);
    print_spin("MEGA", &SPIN_MEGA);
#endif
    print_spin("HWCLK", &SPIN_HWCLK);
#if 0
    print_spin("VIRTCONS", &SPIN_VIRTCONS);
    print_spin("INVALIDATE", &SPIN_INVALIDATE);
    print_spin("PERFMON", &SPIN_PERFMON);
#endif
    print_spin("POOL", &SPIN_POOL);
    print_spin("MAILBOX", &SPIN_MAILBOX);
#if 0
    print_spin("PR_LK11", &SPIN_PR_LK11);
#endif

    struct _generic_64 now;
    struct _generic_64 step1;
    $DESCRIPTOR(secs, "0 00:00:10.00");
    sys$gettim(&now);
    sys$bintim(&secs, &step1);
    now.gen64$r_quad_overlay.gen64$q_quadword -= step1.gen64$r_quad_overlay.gen64$q_quadword;
    sys$schdwk(0, 0, &now, &step1);
    sys$hiber();

    print_spin("IOLOCK11", &SPIN_IOLOCK11);
#if 0
    print_spin("PR_LK10", &SPIN_PR_LK10);
#endif
    print_spin("IOLOCK10", &SPIN_IOLOCK10);
#if 0
    print_spin("PR_LK9", &SPIN_PR_LK9);
#endif
    print_spin("SCHED", &SPIN_SCHED);
    print_spin("MMG", &SPIN_MMG);
#if 0
    print_spin("JIB", &SPIN_JIB);
#endif
    print_spin("TIMER", &SPIN_TIMER);
#if 0
    print_spin("PR_LK8", &SPIN_PR_LK8);
#endif
    print_spin("IOLOCK8", &SPIN_IOLOCK8);
    print_spin("SCS", &SPIN_SCS);
}
