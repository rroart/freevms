#ifndef qssysdef_h
#define qssysdef_h

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
        unsigned int qsrv_packack$l_controls;
        union
        {
            unsigned int qsrv_packack$l_ctrl_bits;
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
        unsigned int qsrv_packack$l_driver_version;
        struct
        {
            unsigned short int qsrv_packack$w_major_vers;
            unsigned short int qsrv_packack$w_minor_vers;
        };
    };
    struct
    {
        union
        {
            unsigned long long qsrv_packack$q_devchar;
            struct
            {
                unsigned int qsrv_packack$l_devchar;
                unsigned int qsrv_packack$l_devchar2;
            };
        };
        unsigned long long qsrv_packack$q_devdepend;
        unsigned long long qsrv_packack$q_devdepend2;
        unsigned char qsrv_packack$b_devclass;
        unsigned char qsrv_packack$b_devtype;
        unsigned short int qsrv_packack$w_devbufsize;
        unsigned int qsrv_packack$l_sts;
        unsigned int qsrv_packack$l_devsts;
        unsigned int qsrv_packack$l_media_id;
    };
    struct
    {
        unsigned short int qsrv_packack$w_mt3_density;
        unsigned short int qsrv_packack$w_spare_1;
    };
    struct
    {
        union
        {
            struct
            {
                unsigned int qsrv_packack$l_maxblock;
                unsigned int qsrv_packack$l_maxbcnt;
            };
            struct
            {
                unsigned int qsrv_packack$l_record;
            };
        };
        unsigned int qsrv_packack$l_alloclass;
    };
    unsigned int qsrv_packack$l_devchar3;
    unsigned int qsrv_packack$l_wwid_length;
    struct _dtn qsrv_packack$r_dtn;
    char qsrv_packack$t_wwid [280];
};

#endif

