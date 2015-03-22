#ifndef ISDDEF_H
#define ISDDEF_H

#define ISD$K_LENDZRO 12
#define ISD$C_LENDZRO 12
#define ISD$M_GBL 0x1
#define ISD$M_CRF 0x2
#define ISD$M_DZRO 0x4
#define ISD$M_WRT 0x8
#define ISD$M_MATCHCTL 0x70
#define ISD$M_LASTCLU 0x80
#define ISD$M_INITALCODE 0x100
#define ISD$M_BASED 0x200
#define ISD$M_FIXUPVEC 0x400
#define ISD$M_RESIDENT 0x800
#define ISD$M_VECTOR 0x20000
#define ISD$M_PROTECT 0x40000
#define ISD$S_FLAGSIZ 24
#define ISD$K_LENPRIV 16
#define ISD$C_LENPRIV 16
#define ISD$K_LENGLBL 36
#define ISD$C_LENGLBL 36
#define ISD$K_MAXLENGLBL 64
#define ISD$C_MAXLENGLBL 64

#define ISD$K_MATALL 0
#define ISD$K_MATEQU 1
#define ISD$K_MATLEQ 2
#define ISD$K_MATNEV 3

#define ISD$K_NORMAL 0
#define ISD$K_SHRFXD 1
#define ISD$K_PRVFXD 2
#define ISD$K_SHRPIC 3
#define ISD$K_PRVPIC 4
#define ISD$K_USRSTACK 253
#define ISD$S_ISDDEF 64

struct _isd
{
    unsigned short int isd$w_size;
    unsigned short int isd$w_pagcnt;
    union
    {
        unsigned int isd$l_vpnpfc;
        struct
        {
            unsigned isd$v_vpn : 21;
            unsigned isd$v_p1 : 1;
            unsigned isd$v_system : 1;
            unsigned isddef$$_fill_1 : 1;
            unsigned isd$v_pfc : 8;
        };
        struct
        {
            unsigned isd$v_vpg : 23;
            unsigned isd$v_fill_0_ : 1;
        };
        struct
        {
            char isddef$$_fill_4 [3];
            unsigned char isd$b_pfc;
        };
    };
    union
    {
        unsigned int isd$l_flags;
        struct
        {
            unsigned isd$v_gbl : 1;
            unsigned isd$v_crf : 1;
            unsigned isd$v_dzro : 1;
            unsigned isd$v_wrt : 1;
            unsigned isd$v_matchctl : 3;
            unsigned isd$v_lastclu : 1;
            unsigned isd$v_initalcode : 1;
            unsigned isd$v_based : 1;
            unsigned isd$v_fixupvec : 1;
            unsigned isd$v_resident : 1;
            unsigned isddef$$_fill_2 : 5;
            unsigned isd$v_vector : 1;
            unsigned isd$v_protect : 1;
            unsigned isddef$$_fill_3 : 5;
        };
        struct
        {
            char isddef$$_fill_5 [3];
            unsigned char isd$b_type;
        };
    };
    unsigned int isd$l_vbn;
    unsigned int isd$l_ident;
    char isd$t_gblnam [44];
};

#endif

