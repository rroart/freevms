#ifndef QSSYSDEF_H
#define QSSYSDEF_H

#include <dtndef.h>

#define QSRV_HLPR_ACT$C_C_PREP 1
#define QSRV_HLPR_ACT$C_C_POST 2
#define QSRV_HLPR_ACT$C_C_CLNUP 3
#define QSRV_HLPR_ACT$C_S_PREP 4
#define QSRV_HLPR_ACT$C_S_POST 5
#define QSRV_HLPR_ACT$C_S_CLNUP 6

#define QSRV_NTFY_ACT$C_PATH_REST 1
#define QSRV_NTFY_ACT$C_PATH_LOST 2

#define QSRV_PACKACK$M_ORIG_BUFIO 0x1
#define QSRV_PACKACK$M_DATA_VALID 0x2

#define QSRV_PACKACK$C_LENGTH 128
#define QSRV_PACKACK$K_LENGTH 128
#define QSRV_PACKACK$C_LENGTH_WWID 448
#define QSRV_PACKACK$K_LENGTH_WWID 448

#define QSRV_PACKACK$S_QSRV_PACKACK 448

#define QSRV_ACTION$C_MOUNTVER 1

#define QSRV_ACTION$C_MPDEV_PATHSWITCH 2

struct _qsrv_packack
{
    union
    {
        UINT32 qsrv_packack$l_controls;
        union
        {
            UINT32 qsrv_packack$l_ctrl_bits;
            struct
            {
                unsigned qsrv_packack$v_orig_bufio : 1;
                unsigned qsrv_packack$v_data_valid : 1;
                unsigned qsrv_packack$v_fill_2_ : 6;
            };
        };
    };
    union
    {
        UINT32 qsrv_packack$l_driver_version;
        struct
        {
            UINT16 qsrv_packack$w_major_vers;
            UINT16 qsrv_packack$w_minor_vers;
        };
    };
    struct
    {
        union
        {
            UINT64 qsrv_packack$q_devchar;
            struct
            {
                UINT32 qsrv_packack$l_devchar;
                UINT32 qsrv_packack$l_devchar2;
            };
        };
        UINT64 qsrv_packack$q_devdepend;
        UINT64 qsrv_packack$q_devdepend2;
        UINT8 qsrv_packack$b_devclass;
        UINT8 qsrv_packack$b_devtype;
        UINT16 qsrv_packack$w_devbufsize;
        UINT32 qsrv_packack$l_sts;
        UINT32 qsrv_packack$l_devsts;
        UINT32 qsrv_packack$l_media_id;
    };
    struct
    {
        UINT16 qsrv_packack$w_mt3_density;
        UINT16 qsrv_packack$w_spare_1;
    };
    struct
    {
        union
        {
            struct
            {
                UINT32 qsrv_packack$l_maxblock;
                UINT32 qsrv_packack$l_maxbcnt;
            };
            struct
            {
                UINT32 qsrv_packack$l_record;
            };
        };
        UINT32 qsrv_packack$l_alloclass;
    };
    UINT32 qsrv_packack$l_devchar3;
    UINT32 qsrv_packack$l_wwid_length;
    struct _dtn qsrv_packack$r_dtn;
    char qsrv_packack$t_wwid [280];
};

#endif

