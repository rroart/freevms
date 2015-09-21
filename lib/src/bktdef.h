#ifndef BKTDEF_H
#define BKTDEF_H

#include <vms_types.h>

#define BKT$K_OVERHDSZ 14
#define BKT$C_OVERHDSZ 14
#define BKT$M_LASTBKT 0x1
#define BKT$M_ROOTBKT 0x2
#define BKT$M_PTR_SZ 0x18
#define BKT$C_ENDOVHD 4
#define BKT$C_DATBKTOVH 2
#define BKT$C_DUPBKTOVH 4

#define BKT$C_MAXBKTSIZ 63
#define BKT$S_BKTDEF 14

// looks like General Bucket Header in rmsint2.doc?

struct _bkt
{
    UINT8 bkt$b_checkchar;
    union
    {
        UINT8 bkt$b_areano;
        UINT8 bkt$b_indexno;
    };
    UINT16 bkt$w_adrsample;
    union
    {
        UINT16 bkt$w_freespace;
        UINT16 bkt$w_keyfrespc;
    };
    union
    {
        UINT16 bkt$w_nxtrecid;
        struct
        {
            UINT8 bkt$b_nxtrecid;
            UINT8 bkt$b_lstrecid;
        };
    };
    UINT32 bkt$l_nxtbkt;
    UINT8 bkt$b_level;
    union
    {
        UINT8 bkt$b_bktcb;
#if 0
        // offset probs
        struct
        {
            unsigned bkt$v_lastbkt : 1;
            unsigned bkt$v_rootbkt : 1;
            unsigned bkt$$_fill_1  : 1;
            unsigned bkt$v_ptr_sz  : 2;
            unsigned bkt$v_fill_4_ : 3;
        };
#endif
    };
};

#endif

