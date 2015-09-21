#ifndef SHLDEF_H
#define SHLDEF_H

#define SHL$M_FIXUP 0x1
#define SHL$C_OLD_SHL_SIZE 56
#define SHL$C_MAXNAMLNG 39
#define SHL$K_LENGTH 64
#define SHL$C_LENGTH 64
#define SHL$S_SHLDEF 64

struct _shl
{
    void *shl$l_baseva;
    struct _shl *shl$l_shlptr;

    UINT32 shl$l_ident;
    void *shl$l_permctx;
    INT8 shl$b_shl_size;
    INT16 shldef$$_fill_1;
    union
    {
        UINT8 shl$b_flags;
        struct
        {
            unsigned shl$v_fixup    : 1;
            unsigned shl$v_fill_2_  : 7;
        };
    };
    void *shl$l_icb;
    union
    {
        char shl$t_imgnam [40];
        INT8 shl$b_namlng;
    };
};

#endif
