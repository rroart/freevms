#ifndef CDLDEF_H
#define CDLDEF_H

#include <vms_types.h>

#define CDL$C_LENGTH 16
#define CDL$S_CDLDEF 20

struct _cdl
{
    UINT16 cdl$w_maxconidx;
    INT16 cdldef$$_fill_1;
    unsigned long cdl$l_freecdt;
    UINT16 cdl$w_size;
    UINT8 cdl$b_type;
    UINT8 cdl$b_subtyp;
    unsigned long cdl$l_nocdt_cnt;
    void *cdl$l_base;
};

#endif

