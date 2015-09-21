#ifndef KFPBDEF_H
#define KFPBDEF_H

#include <vms_types.h>

#define KFPB$K_LENGTH 16
#define KFPB$C_LENGTH 16
#define KFPB$S_KFPBDEF 16

struct _kfpb
{
    struct _kfd *kfpb$l_kfdlst;
    void *kfpb$l_kfehshtab;
    UINT16 kfpb$w_size;
    UINT8 kfpb$b_type;
    UINT8 kfpb$b_spare;
    UINT16 kfpb$w_kfdlstcnt;
    UINT16 kfpb$w_hshtablen;
};

#endif

