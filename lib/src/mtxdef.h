#ifndef MTXDEF_H
#define MTXDEF_H

#include <vms_types.h>

#define MTX$S_MTXDEF 4

struct _mtx
{
    union
    {
        INT32 mtxdef$$_fill_1;
        struct
        {
            unsigned mtxdef$$_fill_2 : 16;
            unsigned mtx$v_wrt : 1;
            unsigned mtx$v_interlock : 1;
            unsigned mtx$v_fill_2_ : 6;
        };
        struct
        {
            UINT16 mtx$w_owncnt;
            UINT16 mtx$w_sts;
        };
    };
};

#endif

