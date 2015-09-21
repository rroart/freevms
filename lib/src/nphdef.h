#ifndef NPHDEF_H
#define NPHDEF_H

#include <vms_types.h>

#define     NPH$C_ALONONPAGED       0
#define     NPH$C_ALONPAGVAR        1
#define     NPH$C_DEANONPAGED       2
#define     NPH$C_DEANONPGDSIZ      3
#define     NPH$C_ALLOCATE_POOL_NPP     4
#define     NPH$C_ALLOCATE_POOL_NPP_ALIGNED 5
#define     NPH$C_DEALLOCATE_POOL_NPP   6
#define     NPH$C_DEALLOCATE_POOL_NPP_SIZED 7
#define     NPH$C_ALLOCATE_POOL_BAP     8
#define     NPH$C_ALLOCATE_POOL_BAP_ALIGNED 9
#define     NPH$C_DEALLOCATE_POOL_BAP   10
#define     NPH$C_DEALLOCATE_POOL_BAP_SIZED 11
#define     NPH$C_POOLZONE_ALLOCATE     12
#define     NPH$C_POOLZONE_DEALLOCATE   13
#define     NPH$C_BAP_NOT_CONTIGUOUS    14
#define     NPH$C_ALLOCATE_POOL_NPP_VAR 15
#define     NPH$C_ALLOCATE_POOL_BAP_VAR 16
#define     NPH$C_ALONONPAGED_ALN       17
#define     NPH$C_EXPAND_NPP        18
#define     NPH$C_EXPAND_BAP        19
#define     NPH$C_MAX_FUNC_TYPE     20
#define     NPH$S_NPHDEF            32

struct _nph
{
    union
    {
        unsigned UINT64 nph$q_addr;
        struct
        {
            UINT32 nph$l_addr_low;
            UINT32 nph$l_addr_high;
        };
    };
    INT32 (*nph$l_pc)(void);
    UINT16 nph$w_function;
    UINT8 nph$b_type;
    UINT8 nph$b_rmod;
    UINT32 nph$l_size;
    UINT8 nph$b_ipl;
    UINT8 nph$b_cpu;
    UINT16 nph$w_unused;
    union
    {
        UINT64 nph$q_time;
        struct
        {
            UINT32 nph$l_time_low;
            UINT32 nph$l_time_high;
        };
    };
};

#endif

