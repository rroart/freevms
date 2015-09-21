#ifndef RDTDEF_H
#define RDTDEF_H

#include <vms_types.h>

#define RDT$S_RDTDEF 24

struct _rdt
{
    UINT8 rdt$b_scs_maint_block [16];
    void *rdt$l_waitfl;
    void *rdt$l_waitbl;
    UINT16 rdt$w_size;
    UINT8 rdt$b_type;
    UINT8 rdt$b_subtyp;
    void *rdt$l_freerd;
    UINT32 rdt$l_maxrdidx;
    UINT32 rdt$l_qrdt_cnt;
};

#endif
