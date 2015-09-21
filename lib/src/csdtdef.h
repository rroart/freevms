#ifndef CSDTDEF_H
#define CSDTDEF_H

#include <vms_types.h>

#define CSDTE$C_LENGTH   64
#define CSDTE$K_LENGTH   64
#define CSDTE$S_CSDTEDEF 64

struct _csdte
{
    UINT64 csdte$q_req_iq;
    UINT64 csdte$q_waitq;
    UINT32 csdte$l_ipid;
    UINT32 csdte$l_queued;
    UINT32 csdte$l_requests;
    UINT32 csdte$l_waitcnt;
    UINT32 csdte$l_maxactive;
    UINT32 csdte$l_active;
    char csdte$t_fillme_4 [24];
};

#define CSDT$C_LENGTH 16
#define CSDT$K_LENGTH 16
#define CSDT$T_CSDTEVEC 16
#define CSDT$S_CSDTDEF 16

typedef struct _csdt
{
    INT32 csdt$l_filler_1;
    INT32 csdt$l_filler_2;
    UINT16 csdt$w_size;
    UINT8 csdt$b_type;
    UINT8 csdt$b_subtype;
    char csdt$t_align [4];
};

#endif

