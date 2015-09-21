#ifndef MBXDEF_H
#define MBXDEF_H

#include <vms_types.h>

#define MBX$M_ALLOC 0x1
#define MBX$M_VALID 0x2
#define MBX$M_DELPEND 0x4
#define MBX$M_QUOTALCK 0x8
#define MBX$K_LENGTH 48
#define MBX$C_LENGTH 48
#define MBX$S_MBXDEF 48

struct _mbx
{
    UINT64 mbx$q_msg;
    //  UINT16 mbx$w_size;
    //  UINT8 mbx$b_type;
    union
    {
        UINT8 mbx$b_flags;
        struct
        {
            unsigned mbx$v_alloc : 1;
            unsigned mbx$v_valid : 1;
            unsigned mbx$v_delpend : 1;
            unsigned mbx$v_quotalck : 1;
            unsigned mbx$v_fill_0_ : 4;
        };
    };
    UINT8 mbx$b_creatport;
    UINT16 mbx$w_unit;
    UINT16 mbx$w_ref;
    UINT16 mbx$w_reader;
    UINT16 mbx$w_readast;
    UINT16 mbx$w_writast;
    UINT16 mbx$w_maxmsg;
    UINT16 mbx$w_msgcnt;
    UINT16 mbx$w_buffquo;
    UINT16 mbx$w_prot;
    UINT32 mbx$l_ownuic;
    char mbx$t_name [16];
};

#endif

