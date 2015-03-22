#ifndef EISDDEF_H
#define EISDDEF_H

#define EISD$K_MAJORID 1
#define EISD$K_MINORID 1
#define EISD$M_GBL 0x1
#define EISD$M_CRF 0x2
#define EISD$M_DZRO 0x4
#define EISD$M_WRT 0x8
#define EISD$M_INITALCODE 0x10
#define EISD$M_BASED 0x20
#define EISD$M_FIXUPVEC 0x40
#define EISD$M_RESIDENT 0x80
#define EISD$M_VECTOR 0x100
#define EISD$M_PROTECT 0x200
#define EISD$M_LASTCLU 0x400
#define EISD$M_EXE 0x800
#define EISD$M_NONSHRADR 0x1000
#define EISD$M_QUAD_LENGTH 0x2000
#define EISD$M_ALLOC_64BIT 0x4000
#define EISD$K_LENDZRO 36
#define EISD$C_LENDZRO 36
#define EISD$K_LENPRIV 36
#define EISD$C_LENPRIV 36
#define EISD$K_LENGLBL 56
#define EISD$C_LENGLBL 56
#define EISD$K_MAXLENGLBL 84
#define EISD$C_MAXLENGLBL 84

#define EISD$K_MATALL 0
#define EISD$K_MATEQU 1
#define EISD$K_MATLEQ 2
#define EISD$K_MATNEV 3

#define EISD$K_NORMAL 0

#define EISD$K_SHRFXD 1
#define EISD$K_PRVFXD 2
#define EISD$K_SHRPIC 3
#define EISD$K_PRVPIC 4
#define EISD$K_USRSTACK 253
#define EISD$S_EISDDEF 84

struct _eisd
{
    struct
    {
        unsigned int eisd$l_majorid;
        unsigned int eisd$l_minorid;
    };
    unsigned int eisd$l_eisdsize;
    unsigned int eisd$l_secsize;
    union
    {
        unsigned long long eisd$q_virt_addr;
        void *eisd$l_virt_addr;
        struct
        {
            unsigned eisd$v_vaddr : 30;
            unsigned eisd$v_p1 : 1;
            unsigned eisd$v_system : 1;
        };
    };
    union
    {
        unsigned int eisd$l_flags;
        struct
        {
            unsigned eisd$v_gbl : 1;
            unsigned eisd$v_crf : 1;
            unsigned eisd$v_dzro : 1;
            unsigned eisd$v_wrt : 1;
            unsigned eisd$v_initalcode : 1;
            unsigned eisd$v_based : 1;
            unsigned eisd$v_fixupvec : 1;
            unsigned eisd$v_resident : 1;
            unsigned eisd$v_vector : 1;
            unsigned eisd$v_protect : 1;
            unsigned eisd$v_lastclu : 1;
            unsigned eisd$v_exe : 1;
            unsigned eisd$v_nonshradr : 1;
            unsigned eisd$v_quad_length : 1;
            unsigned eisd$v_alloc_64bit : 1;
            unsigned eisd$v_fill_0_ : 1;
        };
    };
    unsigned int eisd$l_vbn;
    struct
    {
        unsigned char eisd$b_pfc;
        unsigned char eisd$b_matchctl;
        unsigned char eisd$b_type;
        unsigned char eisd$b_fill_1;
    };
    unsigned int eisd$l_ident;
    union
    {
        char eisd$t_gblnam [44];
        unsigned long long eisd$q_secsize;
    };
};

#endif

