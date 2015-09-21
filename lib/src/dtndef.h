#ifndef DTNDEF_H
#define DTNDEF_H

#include <vms_types.h>

#define DTN$K_BASE_LENGTH 24
#define DTN$K_LENGTH 52
#define DTN$K_NAMELEN_MAX 27

struct _dtn
{
    struct _dtn *dtn$ps_flink;
    struct _dtn *dtn$ps_blink;
    UINT16 dtn$w_size;
    UINT8 dtn$b_type;
    UINT8 dtn$b_subtype;
    UINT32 dtn$l_flags;
    UINT8 dtn$b_devtype;
    UINT8 dtn$b_devclass;
    INT16 dtn$w_spare1;
    struct _ucb *dtn$ps_ucblist;
    union
    {
        char dtn$t_dtname [28];
        struct
        {
            unsigned char dtn$ib_dtname_len;
            char dtn$t_dtname_str [27];
        };
    };
};

#endif

