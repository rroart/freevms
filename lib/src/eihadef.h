#ifndef EIHADEF_H
#define EIHADEF_H

#include <vms_types.h>

#define EIHA$K_LENGTH 48
#define EIHA$C_LENGTH 48
#define EIHA$S_EIHADEF 48

struct _eiha
{
    UINT32 eiha$l_size;
    UINT32 eiha$l_spare;
    union
    {
        UINT64 eiha$q_tfradr1;
        struct
        {
            UINT32 eiha$l_tfradr1;
            UINT32 eiha$l_tfradr1_h;
        };
    };
    union
    {
        UINT64 eiha$q_tfradr2;
        struct
        {
            UINT32 eiha$l_tfradr2;
            UINT32 eiha$l_tfradr2_h;
        };
    };
    union
    {
        UINT64 eiha$q_tfradr3;
        struct
        {
            UINT32 eiha$l_tfradr3;
            UINT32 eiha$l_tfradr3_h;
        };
    };
    union
    {
        UINT64 eiha$q_tfradr4;
        struct
        {
            UINT32 eiha$l_tfradr4;
            UINT32 eiha$l_tfradr4_h;
        };
    };
    union
    {
        UINT32 eiha$l_inishr;
        UINT64 eiha$q_inishr;
    };
};

#endif

