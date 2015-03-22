#ifndef AREADEF_H
#define AREADEF_H

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
    char area$$_fill_1;
    unsigned char area$b_flags;
    unsigned char area$b_areaid;
    unsigned char area$b_arbktsz;
    unsigned short int area$w_volume;
    unsigned char area$b_aln;
    union
    {
        unsigned char area$b_aop;
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
    unsigned int area$l_avail;
    unsigned int area$l_cvbn;
    unsigned int area$l_cnblk;
    unsigned int area$l_used;
    unsigned int area$l_nxtvbn;
    unsigned int area$l_nxt;
    unsigned int area$l_nxblk;
    unsigned short int area$w_deq;
    char area$$_fill_4 [2];
    unsigned int area$l_loc;
    unsigned short int area$w_rfi [3];
    unsigned int area$l_total_alloc;
    char area$$_fill_5 [8];
    unsigned short int area$w_check;
};

#endif

