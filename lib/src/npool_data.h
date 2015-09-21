#ifndef NPOOL_DATA_H
#define NPOOL_DATA_H

#include <vms_types.h>

#define IOC_C_INVALID_ADDRESS -8388608
#define IOC_C_MAXLISTPKT 8192
#define IOC_C_NPAGGRNMSK 63
#define IOC_C_NPAGGRNBITS 6
#define IOC_C_NUMLISTS 128

#define LSTHDS$K_LENGTH 2136
#define LSTHDS$C_LENGTH 2136

#define POOL_MAP$K_LENGTH 32
#define NPOOL$M_NOT_NPP 0x1
#define NPOOL$M_POOL_SEPARATE 0x2
#define NPOOL$M_POOL_WITHIN_NPP 0x4
#define NPOOL$M_MINIMUM_MODE 0x8

#define NPOOL$K_LENGTH 180
#define NPOOL$C_LENGTH 180

struct _lsthds
{
    UINT32 lsthds$l_filler0;
    UINT32 lsthds$l_varallocbytes;
    UINT32 lsthds$l_variablelist_unused;
    void *lsthds$ps_variablelist;
    UINT32 lsthds$l_variablelist_high;
    UINT32 lsthds$l_expansions;
    UINT32 lsthds$l_filler2;
    unsigned int *lsthds$ar_listattempts;
    unsigned int *lsthds$ar_listfails;
    unsigned int *lsthds$ar_listdeallocs;
    UINT32 lsthds$l_rad;
    UINT32 lsthds$l_pooltype;
    struct _npool_data *lsthds$ps_npool_data;
    INT8 lsthds$b_filler3 [4];
#ifdef __i386__
    UINT64 lsthds$q_listheads [129];
#else
    UINT64 lsthds$q_listheads [129][2];
#endif
    UINT64 lsthds$q_filler4;
    UINT64 lsthds$q_filler5;
    UINT64 lsthds$q_listcounters [129];
};

struct _pool_map
{
    void *pool_map$pq_segment_address;
    UINT64 pool_map$q_segment_length;
    void *pool_map$pq_segment_end_address;
    UINT32 pool_map$l_rad;
    INT32 pool_map$l_filler1;
};

struct _npool_data
{
    UINT32 npool$l_on_rad_dealloc;
    UINT32 npool$l_total_dealloc;
    UINT64 npool$q_per_pool_diag;
    void *npool$ps_ringbuf;
    void *npool$ps_nextnph;
    UINT32 npool$l_ringbufcnt;
    char filler6 [100];
    struct _lsthds (*(*npool$ar_lsthds));
    UINT32 npool$l_max_lsthds;
    UINT32 npool$l_gran_mask;
    UINT32 npool$l_num_lookaside;
    void *npool$ps_variable_list;
    struct _pool_map *npool$ps_pool_map;
    UINT32 npool$l_pool_map_size;
    UINT32 npool$l_pool_map_segments;
    union
    {
        struct
        {
            union
            {
                struct _npool_data *npool$ar_pool_data [2];
                struct
                {
                    void *npool$ar_npp_pool_data;
                    void *npool$ar_bap_pool_data;
                };
            };
        };
        struct
        {
            union
            {
                UINT32 npool$l_pool_flags;
                struct
                {
                    unsigned npool$v_not_npp        : 1;
                    unsigned npool$v_pool_separate  : 1;
                    unsigned npool$v_pool_within_npp    : 1;
                    unsigned npool$v_minimum_mode       : 1;
                    unsigned npool$v_fill_0_        : 4;
                };
            };
            UINT32 npool$l_filler3;
            UINT64 npool$q_filler4;
        };
    };
};

#endif

