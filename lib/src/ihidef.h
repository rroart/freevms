#ifndef IHIDEF_H
#define IHIDEF_H

#include <vms_types.h>

#define IHI$K_LENGTH 80
#define IHI$C_LENGTH 80
#define IHI$S_IHIDEF 80

struct _ihi
{
    char ihi$t_imgnam [40];
    char ihi$t_imgid [16];
    UINT64 ihi$q_linktime;
    char ihi$t_linkid [16];
};

#endif

