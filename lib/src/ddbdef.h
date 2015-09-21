#ifndef DDBDEF_H
#define DDBDEF_H

#include <vms_types.h>

#define DDB$M_NO_TIMEOUT 0x1
#define DDB$M_PAC 0x2
#define DDB$K_PACK 1
#define DDB$K_CART 2
#define DDB$K_SLOW 3
#define DDB$K_TAPE 4


#define DDB$K_LENGTH 72
#define DDB$C_LENGTH 72
#define DDB$S_DDBDEF 72

struct _ddb
{
    union
    {
        INT32 ddb$l_link;
        INT32 ddb$l_blink;
        struct
        {
            struct _ddb *ddb$ps_link;
            struct _ddb *ddb$ps_blink;
        };
    };
    union
    {
        INT32 ddb$l_ucb;
        struct
        {
            struct _ucb *ddb$ps_ucb;
        };
    };
    union
    {
        UINT16 ddb$w_size;
        struct
        {
            UINT16 ddb$iw_size;
        };
    };
    union
    {
        UINT8 ddb$b_type;
        struct
        {
            unsigned char ddb$ib_type;
        };
    };
    union
    {
        UINT8 ddb$b_flags;
        struct
        {
            unsigned ddb$v_no_timeout : 1;
            unsigned ddb$v_pac : 1;
            unsigned ddb$v_fill_22_ : 6;
        };
    };
    union
    {
        INT32 ddb$l_ddt;
        struct
        {
            struct _ddt *ddb$ps_ddt;
        };
    };
    union
    {
        UINT32 ddb$l_acpd;
        UINT32 ddb$il_acpd;
        struct
        {
            INT8 ddbdef$$_fill_4 [3];
            union
            {
                UINT8 ddb$b_acpclass;
                struct
                {
                    UINT8 ddb$ib_acpclass;
                };
            };
        };
    };
    union
    {
        char ddb$t_name [16];
        struct
        {
            union
            {
                UINT8 ddb$b_name_len;
                struct
                {
                    UINT8 ddb$ib_name_len;
                };
            };
            char ddb$t_name_str [15];
        };
    };
    struct _dpt *ddb$ps_dpt;
    struct _ddb *ddb$ps_drvlink;
    union
    {
        unsigned long ddb$l_sb;
        struct
        {
            struct _sb *ddb$ps_sb;
        };
    };
    union
    {
        unsigned long ddb$l_conlink;
        struct
        {
            struct _ddb *ddb$ps_conlink;
        };
    };
    union
    {
        UINT32 ddb$l_allocls;
        struct
        {
            UINT32 ddb$il_allocls;
        };
    };
    union
    {
        struct _ucb *ddb$l_2p_ucb;
        struct _ucb *ddb$ps_2p_ucb;
        struct _ucb *ddb$l_dp_ucb;
    };
    union
    {
        UINT32 ddb$l_port_id;
        struct
        {
            char ddb$t_port_id [1];
        };
    };
    UINT32 ddb$l_class_lkid;
    void *ddb$ps_2p_ddb;
};

#endif
