#ifndef pfldef_h
#define pfldef_h

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
    unsigned short int pfl$w_size;
    unsigned char pfl$b_type;
    unsigned char pfl$b_fill_3;
    unsigned int pfl$l_pfc;
    struct _wcb *pfl$l_window;
    unsigned int pfl$l_vbn;
    unsigned int pfl$l_bitmapsiz;
    unsigned int pfl$l_frepagcnt;
    union
    {
        void *pfl$pq_bitmap;
        struct
        {
            void *pfl$l_bitmap;
            unsigned int pfl$l_bitmap_hi;
        };
    };
    void *pfl$pq_bitmap_dir;
    unsigned long long pfl$q_last_dir_quad;
    unsigned int pfl$l_allocsiz;
    unsigned int pfl$l_rsrvpagcnt;
    unsigned int pfl$l_swprefcnt;
    int pfl$l_poolbytes;
    int pfl$l_s2pages;
    unsigned int pfl$l_minfrepagcnt;
    unsigned int pfl$l_pgflx;
    union
    {
        unsigned int pfl$l_flags;
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
    unsigned int pfl$l_refcnt;
    unsigned int pfl$l_maxvbn;
    int pfl$l_startbyte;
    int pfl$l_max_alloc_expo;
    int pfl$l_cur_alloc_expo;
    int pfl$l_bitmap_quads;
    int pfl$l_dir_quads;
    int pfl$l_dir_cluster [8];
    char pfl$b_fill_0_ [4];
    unsigned int pfl$l_bitmaploc;
    char pfl$b_fill_1_ [4];
};

typedef struct PFL * PFL_PQ;

#endif

