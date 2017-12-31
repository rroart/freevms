#include <exe_routines.h>

int sys$cmkrnl(int (*routin)(), unsigned int *arglst);

int   rms_std$alononpaged (int reqsize, int32 *alosize_p, void **pool_p)
{
    long arglst[4];
    arglst[0] = 3;
    arglst[1] = reqsize;
    arglst[2] = alosize_p;
    arglst[3] = pool_p;
    return sys$cmkrnl(exe_std$alononpaged, &arglst[0]);
}

int   rms_std$deanonpgdsiz (void *pool, int size)
{
    long arglst[3];
    arglst[0] = 2;
    arglst[1] = pool;
    arglst[2] = size;
    return sys$cmkrnl(exe_std$deanonpgdsiz, &arglst[0]);
}
