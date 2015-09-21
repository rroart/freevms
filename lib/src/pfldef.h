#ifndef PFLDEF_H
#define PFLDEF_H

#include <vms_types.h>

#define PFL$M_INITED 0x1
#define PFL$M_PAGFILFUL 0x2
#define PFL$M_SWPFILFUL 0x4
#define PFL$M_SWAP_FILE 0x8
#define PFL$M_DINSPEN 0x10
#define PFL$M_STOPPER 0x80000000
#define PFL$K_MAX_EXPO_INDEX 6
#define PFL$C_MAX_EXPO_INDEX 6
#define PFL$K_ALLOC2DIR_SHIFT 4
#define PFL$C_ALLOC2DIR_SHIFT 4
#define PFL$K_ALLOC2DIR_SIZE 16
#define PFL$C_ALLOC2DIR_SIZE 16
#define PFL$M_ALLOC2DIR_MASK 65535

#define PFL$K_LENGTH 148
#define PFL$C_LENGTH 148
#define PFL$S_PFLDEF 156

struct _pfl
{
    struct _pfl *pfl$l_flink;
    struct _pfl *pfl$l_blink;
    UINT16 pfl$w_size;
    UINT8 pfl$b_type;
    UINT8 pfl$b_fill_3;
    UINT32 pfl$l_pfc;
    struct _wcb *pfl$l_window;
    UINT32 pfl$l_vbn;
    UINT32 pfl$l_bitmapsiz;
    UINT32 pfl$l_frepagcnt;
    union
    {
        void *pfl$pq_bitmap;
        struct
        {
            void *pfl$l_bitmap;
            UINT32 pfl$l_bitmap_hi;
        };
    };
    void *pfl$pq_bitmap_dir;
    UINT64 pfl$q_last_dir_quad;
    UINT32 pfl$l_allocsiz;
    UINT32 pfl$l_rsrvpagcnt;
    UINT32 pfl$l_swprefcnt;
    INT32 pfl$l_poolbytes;
    INT32 pfl$l_s2pages;
    UINT32 pfl$l_minfrepagcnt;
    UINT32 pfl$l_pgflx;
    union
    {
        UINT32 pfl$l_flags;
        struct
        {
            unsigned pfl$v_inited : 1;
            unsigned pfl$v_pagfilful : 1;
            unsigned pfl$v_swpfilful : 1;
            unsigned pfl$v_swap_file : 1;
            unsigned pfl$v_dinspen : 1;
            unsigned pfl$$_fill_1 : 26;
            unsigned pfl$v_stopper : 1;
        };
    };
    UINT32 pfl$l_refcnt;
    UINT32 pfl$l_maxvbn;
    INT32 pfl$l_startbyte;
    INT32 pfl$l_max_alloc_expo;
    INT32 pfl$l_cur_alloc_expo;
    INT32 pfl$l_bitmap_quads;
    INT32 pfl$l_dir_quads;
    INT32 pfl$l_dir_cluster [8];
    INT8 pfl$b_fill_0_ [4];
    UINT32 pfl$l_bitmaploc;
    INT8 pfl$b_fill_1_ [4];
};

typedef struct PFL * PFL_PQ;

#endif

