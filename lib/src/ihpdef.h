#ifndef IHPDEF_H
#define IHPDEF_H

#include <vms_types.h>

#define IHP$K_LENGTH 44
#define IHP$C_LENGTH 44
#define IHP$S_IHPDEF 44

struct _ihp
{
    UINT32 ihp$l_eco1;
    UINT32 ihp$l_eco2;
    UINT32 ihp$l_eco3;
    UINT32 ihp$l_eco4;
    UINT32 ihp$l_rw_patsiz;
    void *ihp$l_rw_patadr;
    UINT32 ihp$l_ro_patsiz;
    void *ihp$l_ro_patadr;
    UINT32 ihp$l_patcomtxt;
    UINT64 ihp$q_patdate;
};

#endif

