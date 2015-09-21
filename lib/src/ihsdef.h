#ifndef IHSDEF_H
#define IHSDEF_H

#include <vms_types.h>

#define IHS$K_LENGTH 28
#define IHS$C_LENGTH 28
#define IHS$S_IHSDEF 28

struct _ihs
{
    UINT32 ihs$l_dstvbn;
    UINT32 ihs$l_gstvbn;
    UINT16 ihs$w_dstblks;
    UINT16 ihs$w_gstrecs;
    UINT32 ihs$l_dmtvbn;
    UINT32 ihs$l_dmtbytes;
    INT32 ihs$l_dstblks;
    INT32 ihs$l_gstrecs;
};

#endif

