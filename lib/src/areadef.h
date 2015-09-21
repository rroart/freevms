#ifndef AREADEF_H
#define AREADEF_H

#include <vms_types.h>

#define AREA$C_CYL 1
#define AREA$C_LBN 2
#define AREA$C_VBN 3
#define AREA$C_RFI 4
#define AREA$M_HARD 0x1
#define AREA$M_ONC 0x2
#define AREA$M_CBT 0x20
#define AREA$M_CTG 0x80
#define AREA$K_BLN 64
#define AREA$C_BLN 64
#define AREA$S_AREADEF 64

struct _prologue_area
{
    INT8 area$$_fill_1;
    UINT8 area$b_flags;
    UINT8 area$b_areaid;
    UINT8 area$b_arbktsz;
    UINT16 area$w_volume;
    UINT8 area$b_aln;
    union
    {
        UINT8 area$b_aop;
#if 0
        // offset probs
        struct
        {
            unsigned area$v_hard : 1;
            unsigned area$v_onc : 1;
            unsigned area$$_fill_2 : 3;
            unsigned area$v_cbt : 1;
            unsigned area$$_fill_3 : 1;
            unsigned area$v_ctg : 1;
        };
#endif
    };
    UINT32 area$l_avail;
    UINT32 area$l_cvbn;
    UINT32 area$l_cnblk;
    UINT32 area$l_used;
    UINT32 area$l_nxtvbn;
    UINT32 area$l_nxt;
    UINT32 area$l_nxblk;
    UINT16 area$w_deq;
    INT8 area$$_fill_4 [2];
    UINT32 area$l_loc;
    UINT16 area$w_rfi [3];
    UINT32 area$l_total_alloc;
    INT8 area$$_fill_5 [8];
    UINT16 area$w_check;
};

#endif

