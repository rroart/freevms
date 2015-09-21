#ifndef CPBDEF_H
#define CPBDEF_H

#include <vms_types.h>

#define     CPB$C_PRIMARY       0
#define     CPB$C_NS        1
#define     CPB$C_QUORUM        2
#define     CPB$C_RUN       3
#define     CPB$C_IMPLICIT_AFFINITY 4
#define     CPB$C_SOFT_RAD_AFFINITY 5
#define     CPB$C_RAD_0     6
#define     CPB$C_RAD_1     7
#define     CPB$C_RAD_2     8
#define     CPB$C_RAD_3     9
#define     CPB$C_RAD_4     10
#define     CPB$C_RAD_5     11
#define     CPB$C_RAD_6     12
#define     CPB$C_RAD_7     13
#define     CPB$C_MAX       32
#define     CPB$C_MAX_SYSTEM_BITS   16
#define     CPB$C_MAX_USER_BITS 16
#define     CPB$C_VECTOR        1
#define     CPB$M_PRIMARY       0x1
#define     CPB$M_VECTOR        0x2
#define     CPB$M_QUORUM        0x4
#define     CPB$M_RUN       0x8
#define     CPB$M_IMPLICIT_AFFINITY 0x10
#define     CPB$M_SOFT_RAD_AFFINITY 0x20
#define     CPB$M_RAD_0     0x40
#define     CPB$M_RAD_1     0x80
#define     CPB$M_RAD_2     0x100
#define     CPB$M_RAD_3     0x200
#define     CPB$M_RAD_4     0x400
#define     CPB$M_RAD_5     0x800
#define     CPB$M_RAD_6     0x1000
#define     CPB$M_RAD_7     0x2000
#define     CPB$S_CPBDEF        4

struct _cpb
{
    union
    {
        UINT32 cpb$l_cpb;
        struct
        {
            unsigned cpb$v_primary      : 1;
            unsigned cpb$v_vector       : 1;
            unsigned cpb$v_quorum       : 1;
            unsigned cpb$v_run      : 1;
            unsigned cpb$v_implicit_affinity    : 1;
            unsigned cpb$v_soft_rad_affinity    : 1;
            unsigned cpb$v_rad_0        : 1;
            unsigned cpb$v_rad_1        : 1;
            unsigned cpb$v_rad_2        : 1;
            unsigned cpb$v_rad_3        : 1;
            unsigned cpb$v_rad_4        : 1;
            unsigned cpb$v_rad_5        : 1;
            unsigned cpb$v_rad_6        : 1;
            unsigned cpb$v_rad_7        : 1;
            unsigned cpb$v_fill_1       : 18;
        };
    };
};

#define     CPB$M_FLAG_CHECK_CPU        0x1
#define     CPB$M_FLAG_PERMANENT        0x2
#define     CPB$M_FLAG_PRIMARY      0x4
#define     CPB$M_FLAG_CHECK_CPU_ACTIVE 0x8
#define     CPB$S_CPB_FLAGSDEF      1

struct _cpb_flags
{
    struct
    {
        unsigned cpb$v_flag_check_cpu   : 1;
        unsigned cpb$v_flag_permanent   : 1;
        unsigned cpb$v_flag_primary     : 1;
        unsigned cpb$v_flag_check_cpu_active: 1;
        unsigned cpb$v_flag_filler      : 4;
    };
};

#endif

