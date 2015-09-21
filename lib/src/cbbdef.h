#ifndef CBBDEF_H
#define CBBDEF_H

#include <vms_types.h>

#define CBB$M_LOCK_BIT 0x1
#define CBB$M_AUTO_LOCK 0x2
#define CBB$M_TIMEOUT_CRASH 0x4
#define CBB$M_SUMMARY_BITS 0x8
#define CBB$M_SET_COUNT 0x10

#define CBB$K_LENGTH 48
#define CBB$C_LENGTH 48

#define CBB$C_OR 0
#define CBB$C_AND 1
#define CBB$C_XOR 2
#define CBB$C_BIC 3
#define CBB$C_ORNOT 4
#define CBB$C_EQV 5
#define CBB$C_MAX_FUNCTION 5

#define CBB$M_OVERRIDE_LOCK 1

struct _cbb
{
    UINT32 cbb$l_data_offset;
    UINT16 cbb$w_unit_count;
    UINT8 cbb$b_unit_size;
    UINT8 cbb$b_lock_ipl;
    UINT16 cbb$w_size;
    UINT8 cbb$b_type;
    UINT8 cbb$b_subtype;
    UINT32 cbb$l_bit_count;
    union
    {
        UINT64 cbb$q_interlock;
        union
        {
            UINT64 cbb$q_state;
            struct
            {
                unsigned int cbb$v_lock_bit : 1;
                unsigned int cbb$v_auto_lock : 1;
                unsigned int cbb$v_timeout_crash : 1;
                unsigned int cbb$v_summary_bits : 1;
                unsigned int cbb$v_set_count : 1;
                UINT32 cbb$v_fill_2_ : 3;
            };
        };
    };
    UINT32 cbb$l_timeout_count;
    UINT32 cbb$l_saved_ipl;
    UINT32 cbb$l_valid_bits;
    UINT8 cbb$b_cbb_padding_0 [4];
    UINT64 cbb$q_summary_bitmask;
};

typedef struct _cbb    CBB;
typedef struct _cbb *  CBB_PQ;
typedef struct _cbb ** CBB_PPQ;

#endif
