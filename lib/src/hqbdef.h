#ifndef HQBDEF_H
#define HQBDEF_H

#include <vms_types.h>

#define     HQB$M_VC_FAILED     0x1
#define     HQB$M_DISCON_INIT   0x2
#define     HQB$M_PATHMOVE      0x4
#define     HQB$M_UNIT_ONLINE   0x1
#define     HQB$M_V5CL      0x2
#define     HQB$M_HUNN      0x4
#define     HQB$K_LENGTH        72
#define     HQB$S_HQBDEF        72

struct _hqb
{
    struct _hqb *hqb$l_flink;
    struct _hqb *hqb$l_blink;
    UINT16 hqb$w_size;
    UINT8 hqb$b_type;
    UINT8 hqb$b_subtype;
    UINT8 hqb$b_hostno;
    union
    {
        UINT8 hqb$b_state;
        struct
        {
            unsigned hqb$v_vc_failed : 1;
            unsigned hqb$v_discon_init : 1;
            unsigned hqb$v_pathmove : 1;
            unsigned hqb$v_fill_4_ : 5;
        };
    };
    UINT16 hqb$w_cnt_flgs;
    UINT16 hqb$w_htimo;
    union
    {
        UINT16 hqb$w_flags;
        struct
        {
            unsigned hqb$v_unit_online : 1;
            unsigned hqb$v_v5cl : 1;
            unsigned hqb$v_hunn : 1;
            unsigned hqb$v_fill_5_ : 5;
        };
    };
    UINT64 hqb$q_time;
    UINT16 hqb$w_num_que;
    UINT16 hqb$w_max_que;
    struct _hrb *hqb$l_hrb_fl;
    struct _hrb *hqb$l_hrb_bl;
    struct _cdt *hqb$l_cdt;
    struct _dsrv *hqb$l_dsrv;
    UINT8 hqb$b_systemid [6];
    UINT16 hqb$w_max_hulb;
    void *hqb$l_hulb_vector;
    struct _tsrv *hqb$l_tsrv;
    UINT32 hqb$l_ack_time;
    UINT32 hqb$l_arr_time;
};

#endif

