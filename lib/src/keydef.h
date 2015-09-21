#ifndef KEYDEF_H
#define KEYDEF_H

#include <vms_types.h>

#define     KEY$M_DUPKEYS       0x1
#define     KEY$M_CHGKEYS       0x2
#define     KEY$M_NULKEYS       0x4
#define     KEY$M_IDX_COMPR     0x8
#define     KEY$M_INITIDX       0x10
#define     KEY$M_KEY_COMPR     0x40
#define     KEY$M_REC_COMPR     0x80
#define     KEY$C_MAX_DAT       10

#define     KEY$C_MAX_PRIMARY   6

#define     KEY$C_MAX_INDEX     6

#define     KEY$C_STRING        0
#define     KEY$C_SGNWORD       1
#define     KEY$C_UNSGNWORD     2
#define     KEY$C_SGNLONG       3
#define     KEY$C_UNSGNLONG     4
#define     KEY$C_PACKED        5
#define     KEY$C_SGNQUAD       6
#define     KEY$C_UNSGNQUAD     7
#define     KEY$C_COLLATED      8
#define     KEY$C_MAX_ASCEND    8
#define     KEY$C_DSTRING       32
#define     KEY$C_DSGNWORD      33
#define     KEY$C_DUNSGNWORD    34
#define     KEY$C_DSGNLONG      35
#define     KEY$C_DUNSGNLONG    36
#define     KEY$C_DPACKED       37
#define     KEY$C_DSGNQUAD      38
#define     KEY$C_DUNSGNQUAD    39
#define     KEY$C_DCOLLATED     40
#define     KEY$C_MAX_DATA      40
#define     KEY$K_BLN       96
#define     KEY$C_BLN       96
#define     KEY$C_SPARE     6
#define     KEY$S_KEYDEF        96

// like Indexed File Prologue Block 1 in rmsint2.doc?

struct _prologue_key
{
    UINT32 key$l_idxfl;
    UINT16 key$w_noff;
    UINT8 key$b_ianum;
    UINT8 key$b_lanum;
    UINT8 key$b_danum;
    UINT8 key$b_rootlev;
    UINT8 key$b_idxbktsz;
    UINT8 key$b_datbktsz;
    UINT32 key$l_rootvbn;
    union
    {
        UINT8 key$b_flags;
#if 0
        // gcc bitfield problems
        struct
        {
            unsigned key$v_dupkeys      : 1;
            unsigned key$v_chgkeys      : 1;
            unsigned key$v_nulkeys      : 1;
            unsigned key$v_idx_compr        : 1;
            unsigned key$v_initidx      : 1;
            unsigned key$$_fill_1       : 1;
            unsigned key$v_key_compr        : 1;
            unsigned key$v_fill_8_      : 1;
        };
        struct
        {
            unsigned key$$_fill_2       : 1;
            unsigned key$$_fill_3       : 2;
            unsigned key$$_fill_4       : 1;
            unsigned key$$_fill_5       : 1;
            unsigned key$$_fill_6       : 1;
            unsigned key$$_fill_7       : 1;
            unsigned key$v_rec_compr        : 1;
        };
#endif
    };
    UINT8 key$b_datatype;
    UINT8 key$b_segments;
    UINT8 key$b_nullchar;
    UINT8 key$b_keysz;
    UINT8 key$b_keyref;
    UINT16 key$w_minrecsz;
    UINT16 key$w_idxfill;
    UINT16 key$w_datfill;
    union
    {
        UINT16 key$w_position;
        UINT16 key$w_position0;
    };
    UINT16 key$w_position1;
    UINT16 key$w_position2;
    UINT16 key$w_position3;
    UINT16 key$w_position4;
    UINT16 key$w_position5;
    UINT16 key$w_position6;
    UINT16 key$w_position7;
    union
    {
        UINT8 key$b_size;
        UINT8 key$b_size0;
    };
    UINT8 key$b_size1;
    UINT8 key$b_size2;
    UINT8 key$b_size3;
    UINT8 key$b_size4;
    UINT8 key$b_size5;
    UINT8 key$b_size6;
    UINT8 key$b_size7;
    char key$t_keynam [32];
    UINT32 key$l_ldvbn;
    union
    {
        UINT8 key$b_type;
        UINT8 key$b_type0;
    };
    UINT8 key$b_type1;
    UINT8 key$b_type2;
    UINT8 key$b_type3;
    UINT8 key$b_type4;
    UINT8 key$b_type5;
    UINT8 key$b_type6;
    UINT8 key$b_type7;
};

#endif

