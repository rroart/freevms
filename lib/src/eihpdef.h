#ifndef EIHPDEF_H
#define EIHPDEF_H

#include <vms_types.h>

#define EIHP$K_LENGTH 60
#define EIHP$C_LENGTH 60
#define EIHP$S_EIHPDEF 60

struct _eihp
{
    struct
    {
        UINT32 eihp$l_majorid;
        UINT32 eihp$l_minorid;
    };
    UINT32 eihp$l_eco1;
    UINT32 eihp$l_eco2;
    UINT32 eihp$l_eco3;
    UINT32 eihp$l_eco4;
    UINT32 eihp$l_patcomtxt;
    UINT32 eihp$l_rw_patsiz;
    union
    {
        UINT32 eihp$l_rw_patadr;
        UINT64 eihp$q_rw_patadr;
    };
    UINT32 eihp$l_ro_patsiz;
    union
    {
        UINT32 eihp$l_ro_patadr;
        UINT64 eihp$q_ro_patadr;
    };
    UINT64 eihp$q_patdate;
};

#endif

