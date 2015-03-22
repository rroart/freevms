#ifndef JIBDEF_H
#define JIBDEF_H

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
    unsigned short int jib$w_size;
    unsigned char jib$b_type;
    unsigned char jib$b_daytypes;
    char jib$t_username [12];
    char jib$t_account [8];
    unsigned int jib$l_bytcnt;
    unsigned int jib$l_bytlm;
    unsigned int jib$l_pbytcnt;
    unsigned int jib$l_pbytlim;
    unsigned int jib$l_filcnt;
    unsigned int jib$l_fillm;
    unsigned int jib$l_tqcnt;
    unsigned int jib$l_tqlm;
    unsigned int jib$l_pgflquota;
    int jib$l_pgflcnt;
    unsigned int jib$l_cpulim;
    unsigned int jib$l_prccnt;
    unsigned int jib$l_prclim;
    unsigned short int jib$w_shrfcnt;
    unsigned short int jib$w_shrflim;
    unsigned int jib$l_enqcnt;
    unsigned int jib$l_enqlm;
    unsigned short int jib$w_maxjobs;
    unsigned short int jib$w_maxdetach;
    unsigned int jib$l_mpid;
    void *jib$l_jlnamfl;
    void *jib$l_jlnambl;
    unsigned int jib$l_pdayhours;
    unsigned int jib$l_odayhours;
    unsigned int jib$l_jobtype;
    union
    {
        unsigned int jib$l_flags;
        struct
        {
            unsigned jib$v_bytcnt_waiters : 1;
            unsigned jib$v_tqcnt_waiters : 1;
            unsigned jib$v_fill_0_ : 6;
        };
    };
    unsigned int jib$l_org_bytlm;
    unsigned int jib$l_org_pbytlm;
    unsigned int jib$l_jtquota;
};

#endif

