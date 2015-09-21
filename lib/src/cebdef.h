#ifndef CEBDEF_H
#define CEBDEF_H

#include <vms_types.h>

#define CEB$K_LENGTH 72
#define CEB$C_LENGTH 72
#define CEB$S_CEBDEF 72

struct _ceb
{
    struct _ceb *ceb$l_cebfl;
    struct _ceb *ceb$l_cebbl;
    UINT16 ceb$w_size;
    UINT8 ceb$b_type;
    INT8 ceb$b_subtype;
    union
    {
        UINT32 ceb$l_sts;
        struct
        {
            unsigned ceb$v_noquota : 1;
            unsigned ceb$v_perm : 1;
            unsigned ceb$v_fill_0_ : 6;
        };
    };
    UINT32 ceb$l_pid;
    UINT32 ceb$l_efc;
    void *ceb$l_wqfl;
    void *ceb$l_wqbl;
    UINT32 ceb$l_wqcnt;
    UINT32 ceb$l_state;
    struct _orb *ceb$l_orb;
    union
    {
        UINT32 ceb$l_uic;
        struct
        {
            INT8 cebdef$$_fill_2 [2];
            UINT16 ceb$w_grp;
        };
    };
    UINT32 ceb$l_prot;
    UINT32 ceb$l_refc;
    char ceb$t_efcnam [16];
};

#endif

