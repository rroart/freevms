#ifndef RVTDEF_H
#define RVTDEF_H

#include <vms_types.h>

#define     RVT$K_LENGTH        88
#define     RVT$C_LENGTH        88
#define     RVT$C_UCB_POINTER   0
#define     RVT$C_PHYSICAL_VOLUME   1
#define     RVT$C_VOLUME_LOCK_ID    2
#define     RVT$C_VOLUME_IDENTIFIER 3

#define     RVT$C_RVTVCB        4
#define     RVT$C_MINSIZE       18
#define     RVT$S_RVTDEF        88

struct _rvt
{
    UINT32 rvt$l_struclkid;
    UINT32 rvt$l_refc;
    UINT16 rvt$w_size;
    UINT8 rvt$b_type;
    UINT8 rvt$b_nvols;
    char rvt$t_strucname [12];
    char rvt$t_vlslcknam [12];
    UINT32 rvt$l_blockid;
    UINT8 rvt$b_acb [36];
    UINT32 rvt$l_trans;
    UINT32 rvt$l_activity;
    union
    {
        struct _ucb *rvt$l_ucblst;
        void *rvt$a_rvtvcb;
    };
};

#endif

