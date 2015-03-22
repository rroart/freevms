#ifndef NPOOL_DATA_H
#define NPOOL_DATA_H

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
    unsigned int lsthds$l_filler0;
    unsigned int lsthds$l_varallocbytes;
    unsigned int lsthds$l_variablelist_unused;
    void *lsthds$ps_variablelist;
    unsigned int lsthds$l_variablelist_high;
    unsigned int lsthds$l_expansions;
    unsigned int lsthds$l_filler2;
    unsigned int *lsthds$ar_listattempts;
    unsigned int *lsthds$ar_listfails;
    unsigned int *lsthds$ar_listdeallocs;
    unsigned int lsthds$l_rad;
    unsigned int lsthds$l_pooltype;
    struct _npool_data *lsthds$ps_npool_data;
    char lsthds$b_filler3 [4];
#ifdef __i386__
    unsigned long long lsthds$q_listheads [129];
#else
    unsigned long long lsthds$q_listheads [129][2];
#endif
    unsigned long long lsthds$q_filler4;
    unsigned long long lsthds$q_filler5;
    unsigned long long lsthds$q_listcounters [129];
};

struct _pool_map
{
    void *pool_map$pq_segment_address;
    unsigned long long pool_map$q_segment_length;
    void *pool_map$pq_segment_end_address;
    unsigned int pool_map$l_rad;
    int pool_map$l_filler1;
};

struct _npool_data
{
    unsigned int npool$l_on_rad_dealloc;
    unsigned int npool$l_total_dealloc;
    unsigned long long npool$q_per_pool_diag;
    void *npool$ps_ringbuf;
    void *npool$ps_nextnph;
    unsigned int npool$l_ringbufcnt;
    char filler6 [100];
    struct _lsthds (*(*npool$ar_lsthds));
    unsigned int npool$l_max_lsthds;
    unsigned int npool$l_gran_mask;
    unsigned int npool$l_num_lookaside;
    void *npool$ps_variable_list;
    struct _pool_map *npool$ps_pool_map;
    unsigned int npool$l_pool_map_size;
    unsigned int npool$l_pool_map_segments;
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
                unsigned int npool$l_pool_flags;
                struct
                {
                    unsigned npool$v_not_npp        : 1;
                    unsigned npool$v_pool_separate  : 1;
                    unsigned npool$v_pool_within_npp    : 1;
                    unsigned npool$v_minimum_mode       : 1;
                    unsigned npool$v_fill_0_        : 4;
                };
            };
            unsigned int npool$l_filler3;
            unsigned long long npool$q_filler4;
        };
    };
};

#endif

