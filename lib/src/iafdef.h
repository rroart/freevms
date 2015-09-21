#ifndef IAFDEF_H
#define IAFDEF_H

#define IAF$K_LENGTH 64
#define IAF$C_LENGTH 64
#define IAF$S_IAFDEF 64

struct _iaf
{
    void *iaf$l_iaflink;
    void *iaf$l_fixuplnk;
    UINT16 iaf$w_size;
    union
    {
        UINT16 iaf$w_flags;
        struct
        {
            unsigned iaf$v_shr  : 1;
            unsigned iaf$v_fill_0_  : 7;
        };
    };
    UINT32 iaf$l_g_fixoff;
    UINT32 iaf$l_dotadroff;
    UINT32 iaf$l_chgprtoff;
    UINT32 iaf$l_shlstoff;
    UINT32 iaf$l_shrimgcnt;
    UINT32 iaf$l_shlextra;
    void *iaf$l_permctx;
    INT32 iafdef$$_fill_1;
    INT32 iafdef$$_fill_2;
    INT32 iafdef$$_fill_3;
    INT32 iafdef$$_fill_4;
    INT32 iafdef$$_fill_5;
    INT32 iafdef$$_fill_6;
};

#endif
