#ifndef NPHDEF_H
#define NPHDEF_H

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
        unsigned unsigned long long nph$q_addr;
        struct
        {
            unsigned int nph$l_addr_low;
            unsigned int nph$l_addr_high;
        };
    };
    int (*nph$l_pc)(void);
    unsigned short int nph$w_function;
    unsigned char nph$b_type;
    unsigned char nph$b_rmod;
    unsigned int nph$l_size;
    unsigned char nph$b_ipl;
    unsigned char nph$b_cpu;
    unsigned short int nph$w_unused;
    union
    {
        unsigned long long nph$q_time;
        struct
        {
            unsigned int nph$l_time_low;
            unsigned int nph$l_time_high;
        };
    };
};

#endif

