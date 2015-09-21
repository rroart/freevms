#ifndef JIBDEF_H
#define JIBDEF_H

#include <vms_types.h>

#define JIB$C_DETACHED 0
#define JIB$C_NETWORK 1
#define JIB$C_BATCH 2
#define JIB$C_LOCAL 3
#define JIB$C_DIALUP 4
#define JIB$C_REMOTE 5
#define JIB$M_BYTCNT_WAITERS 0x1
#define JIB$M_TQCNT_WAITERS 0x2
#define JIB$K_LENGTH 140
#define JIB$C_LENGTH 140
#define JIB$S_JIBDEF 140

struct _jib
{
    struct _mtl *jib$l_mtlfl;
    struct _mtl *jib$l_mtlbl;
    UINT16 jib$w_size;
    UINT8 jib$b_type;
    UINT8 jib$b_daytypes;
    char jib$t_username [12];
    char jib$t_account [8];
    UINT32 jib$l_bytcnt;
    UINT32 jib$l_bytlm;
    UINT32 jib$l_pbytcnt;
    UINT32 jib$l_pbytlim;
    UINT32 jib$l_filcnt;
    UINT32 jib$l_fillm;
    UINT32 jib$l_tqcnt;
    UINT32 jib$l_tqlm;
    UINT32 jib$l_pgflquota;
    INT32 jib$l_pgflcnt;
    UINT32 jib$l_cpulim;
    UINT32 jib$l_prccnt;
    UINT32 jib$l_prclim;
    UINT16 jib$w_shrfcnt;
    UINT16 jib$w_shrflim;
    UINT32 jib$l_enqcnt;
    UINT32 jib$l_enqlm;
    UINT16 jib$w_maxjobs;
    UINT16 jib$w_maxdetach;
    UINT32 jib$l_mpid;
    void *jib$l_jlnamfl;
    void *jib$l_jlnambl;
    UINT32 jib$l_pdayhours;
    UINT32 jib$l_odayhours;
    UINT32 jib$l_jobtype;
    union
    {
        UINT32 jib$l_flags;
        struct
        {
            unsigned jib$v_bytcnt_waiters : 1;
            unsigned jib$v_tqcnt_waiters : 1;
            unsigned jib$v_fill_0_ : 6;
        };
    };
    UINT32 jib$l_org_bytlm;
    UINT32 jib$l_org_pbytlm;
    UINT32 jib$l_jtquota;
};

#endif

