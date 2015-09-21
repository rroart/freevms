#ifndef IHDDEF_H
#define IHDDEF_H

#include <vms_types.h>

#define IHD$K_MAJORID 12848
#define IHD$K_MINORID 13616
#define IHD$K_EXE 1
#define IHD$K_LIM 2
#define IHD$M_LNKDEBUG 0x1
#define IHD$M_LNKNOTFR 0x2
#define IHD$M_NOP0BUFS 0x4
#define IHD$M_PICIMG 0x8
#define IHD$M_P0IMAGE 0x10
#define IHD$M_DBGDMT 0x20
#define IHD$M_INISHR 0x40
#define IHD$M_IHSLONG 0x80
#define IHD$M_UPCALLS 0x100
#define IHD$M_MATCHCTL 0x7000000
#define IHD$K_LENGTH 48
#define IHD$C_LENGTH 48
#define IHD$C_MINCODE -1
#define IHD$C_NATIVE -1
#define IHD$C_RSX 0
#define IHD$C_BPA 1
#define IHD$C_ALIAS 2
#define IHD$C_CLI 3
#define IHD$C_PMAX 4
#define IHD$C_ALPHA 5
#define IHD$C_MAXCODE 5

#define IHD$C_GEN_XLNKR 1
#define IHD$C_GEN_NATIVE 2
#define IHD$C_GEN_LNKFLG 3
#define IHD$C_GEN_SYSVER 4
#define IHD$C_GEN_FIXUP 5

#define IHD$C_GEN_NEWISD 6
#define IHD$S_IHDDEF 512

struct _ihd
{
    UINT16 ihd$w_size;
    UINT16 ihd$w_activoff;
    UINT16 ihd$w_symdbgoff;
    UINT16 ihd$w_imgidoff;
    UINT16 ihd$w_patchoff;
    UINT16 ihd$w_version_array_off;
    UINT16 ihd$w_majorid;
    UINT16 ihd$w_minorid;
    UINT8 ihd$b_hdrblkcnt;
    UINT8 ihd$b_imgtype;
    INT16 ihddef$$_fill_2;
    UINT64 ihd$q_privreqs;
    UINT16 ihd$w_iochancnt;
    UINT16 ihd$w_imgiocnt;
    union
    {
        UINT32 ihd$l_lnkflags;
        struct
        {
            unsigned ihd$v_lnkdebug : 1;
            unsigned ihd$v_lnknotfr : 1;
            unsigned ihd$v_nop0bufs : 1;
            unsigned ihd$v_picimg   : 1;
            unsigned ihd$v_p0image  : 1;
            unsigned ihd$v_dbgdmt   : 1;
            unsigned ihd$v_inishr   : 1;
            unsigned ihd$v_ihslong  : 1;
            unsigned ihd$v_upcalls  : 1;
            unsigned ihddef$$_fill_3    : 15;
            unsigned ihd$v_matchctl : 3;
            unsigned ihd$v_fill_0_  : 5;
        };
    };
    UINT32 ihd$l_ident;
    UINT32 ihd$l_sysver;
    void *ihd$l_iafva;
    char ihd$t_skip [462];
    UINT16 ihd$w_alias;
};

#endif

