#ifndef MTLDEF_H
#define MTLDEF_H

#include <vms_types.h>

#define MTL$K_LENGTH 32
#define MTL$C_LENGTH 32
#define MTL$S_MTLDEF 32

struct _mtl
{
    struct _mtl *mtl$l_mtlfl;
    struct _mtl *mtl$l_mtlbl;
    UINT16 mtl$w_size;
    UINT8 mtl$b_type;
    union
    {
        UINT8 mtl$b_status;
        struct
        {
            unsigned mtl$v_volset : 1;
            unsigned mtl$v_rvtvcb : 1;
            unsigned mtl$v_fill_0_ : 6;
        };
    };
    struct _ucb *mtl$l_ucb;
    void *mtl$l_logname;
    void *mtl$l_lognam2;
    INT32 mtldef$$_fill_1;
    UINT32 mtl$l_uic;
};

#endif

