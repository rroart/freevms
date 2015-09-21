#ifndef EIHSDEF_H
#define EIHSDEF_H

#include <vms_types.h>

#define EIHS$K_MAJORID 1
#define EIHS$K_MINORID 1
#define EIHS$K_LENGTH 32
#define EIHS$C_LENGTH 32
#define EIHS$S_EIHSDEF 32

struct _eihs
{
    struct
    {
        UINT32 eihs$l_majorid;
        UINT32 eihs$l_minorid;
    };
    UINT32 eihs$l_dstvbn;
    UINT32 eihs$l_dstsize;
    UINT32 eihs$l_gstvbn;
    UINT32 eihs$l_gstsize;
    UINT32 eihs$l_dmtvbn;
    UINT32 eihs$l_dmtbytes;
};

#endif

