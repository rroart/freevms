#ifndef EIHVNDEF_H
#define EIHVNDEF_H

#include <vms_types.h>

#define EIHVN$M_SUBVERSION_MINOR_ID 0xFFFF
#define EIHVN$M_SUBVERSION_MAJOR_ID 0xFFFF0000

struct _eimg_version_array
{
    UINT32 eihvn$l_subsystem_mask;
    union
    {
        UINT32 eihvn$l_subversion_array;
        struct
        {
            unsigned eihvn$v_subversion_minor_id : 16;
            unsigned eihvn$v_subversion_major_id : 16;
        };
    };
};

#define EIHVN$M_VERSION_MINOR_ID 0xFFFFFF
#define EIHVN$M_VERSION_MAJOR_ID 0xFF000000

struct _eimg_overall_version
{
    union
    {
        INT32 eihvn$l_version_bits;
        struct
        {
            unsigned eihvn$v_version_minor_id : 24;
            unsigned eihvn$v_version_major_id : 8;
        };
    };
};

#define EIHVN$K_LENGTH 132
#define EIHVN$C_LENGTH 132

#endif

