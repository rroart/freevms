// $Id$
// $Locker$

// Author. Roar Thronæs.

// like memcpy

#include <starlet.h>

int memcpy(void *dest, const void *src, int n);

int inst$getmem(const void *src, void *dest, int n)
{
    if (n==0)
        n = sizeof(void *);
    long arglst[4];
    arglst[0]=3;
    arglst[1]=dest;
    arglst[2]=src;
    arglst[3]=n;
    sys$cmkrnl(memcpy, &arglst[0]);
}
int exe_std$alononpaged (int reqsize, int *alosize_p, void **pool_p);

int inst$alononpaged(int reqsize, int *alosize_p, void **pool_p)
{
    long arglst[4];
    arglst[0]=3;
    arglst[1]=reqsize;
    arglst[2]=alosize_p;
    arglst[3]=pool_p;
    sys$cmkrnl(exe_std$alononpaged, &arglst[0]);
}

int inst$exit()
{
    // not yet exit(0);
    extern int do_ret;
    do_ret = 1;
}
