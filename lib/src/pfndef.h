#ifndef PFNDEF_H
#define PFNDEF_H

#include <shm_iddef.h>

#define PMAP$C_LENGTH 8

#define PLNK$C_LENGTH 8

#define     PFN$M_PAGTYP        0x7
#define     PFN$M_LOC       0xF0
#define     PFN$M_BUFOBJ        0x100
#define     PFN$M_COLLISION     0x200
#define     PFN$M_BADPAG        0x400
#define     PFN$M_RPTEVT        0x800
#define     PFN$M_DELCON        0x1000
#define     PFN$M_MODIFY        0x2000
#define     PFN$M_UNAVAILABLE   0x4000
#define     PFN$M_SWPPAG_VALID  0x8000
#define     PFN$M_TOP_LEVEL_PT  0x10000
#define     PFN$M_SLOT      0x20000
#define     PFN$M_SHARED        0x40000
#define     PFN$M_ZEROED        0x80000
#define     PFN$S_INDEX_WIDTH   48
#define     PFN$M_TYP0      0x10000
#define     PFN$M_PARTIAL_SECTION   0x80000
#define     PFN$M_GBLBAK        0x80000000
#define     PFN$M_STX       0xFFFF00000000
#define     PFN$M_CRF       0x1000000000000
#define     PFN$M_DZRO      0x2000000000000
#define     PFN$M_WRT       0x4000000000000
#define     PFN$M_STX_HIBIT     0x8000000000000
#define     PFN$M_PGFLPAG       0xFFFFFF00000000
#define     PFN$M_PGFLX     0xFF00000000000000
#define     PFN$M_BAKX      0xFFFFFFFF00000000
#define     PFN$M_GPTX      0xFFFFFFFF00000000
#define     PFN$C_ENTRY_SIZE    40
#define     PFN$C_FREPAGLST     0
#define     PFN$C_MFYPAGLST     1
#define     PFN$C_BADPAGLST     2
#define     PFN$C_RELPEND       3
#define     PFN$C_UNTESTED      3
#define     PFN$C_RDERR     4
#define     PFN$C_WRTINPROG     5
#define     PFN$C_RDINPROG      6
#define     PFN$C_ZERO_LIST     7
#define     PFN$C_PRVPFN        8
#define     PFN$C_ACTIVE        15
#define     PFN$C_PROCESS       0
#define     PFN$C_SYSTEM        1
#define     PFN$C_GLOBAL        2
#define     PFN$C_GBLWRT        3
#define     PFN$C_PPGTBL        4
#define     PFN$C_GPGTBL        5
#define     PFN$C_RESERVED      6
#define     PFN$C_UNKNOWN       7
#define     PFN$C_PFNLST        1
#define     PFN$C_SHM_REG       3
#define     PFN$S_PFNDEF        40

#define PRVPFN$K_LENGTH 40
#define PRVPFN$C_LENGTH 40

struct _pmap
{
    UINT32 pmap$l_start_pfn;
    UINT32 pmap$l_pfn_count;
};

struct _plnk
{
    union
    {
        UINT64 plnk$q_pfn_list_link;
        struct
        {
            UINT32 plnk$l_next_pfn;
            UINT32 plnk$l_pfn_count;
        };
    };
};

#define _pfn page
typedef struct page
{
    union
    {
        struct _plnk pfn$r_shm_list_link;
        struct _shm_id pfn$r_shm_reg_id;
        struct
        {
            union
            {
                UINT32 pfn$l_flink;
                UINT32 pfn$l_shrcnt;
            };
            union
            {
                UINT32 pfn$l_blink;
                UINT32 pfn$l_wslx_qw;
                UINT32 pfn$l_gbl_lck_cnt;
            };
        };
    };
    union
    {
        UINT32 pfn$l_page_state;
        struct
        {
            unsigned pfn$v_pagtyp       : 3;
            unsigned pfndef$$_fill_1        : 1;
            unsigned pfn$v_loc      : 4;
            unsigned pfn$v_bufobj       : 1;
            unsigned pfn$v_collision        : 1;
            unsigned pfn$v_badpag       : 1;
            unsigned pfn$v_rptevt       : 1;
            unsigned pfn$v_delcon       : 1;
            unsigned pfn$v_modify       : 1;
            unsigned pfn$v_unavailable  : 1;
            unsigned pfn$v_swppag_valid : 1;
            unsigned pfn$v_top_level_pt : 1;
            unsigned pfn$v_slot     : 1;
            unsigned pfn$v_shared       : 1;
            unsigned pfn$v_zeroed       : 1;
            unsigned pfndef$$_fill_3        : 12;
        };
    };
    union
    {
        UINT32 pfn$l_pt_pfn;
        UINT32 pfn$l_shm_cpp_id;
    };
    union
    {
        UINT64 pfn$q_pte_index;
        struct
        {
            INT32 pfn$l_refcnt_fill1;
            //      INT16 pfn$w_refcnt_fill2;
            union
            {
                unsigned long npfn$l_refcnt; // was w_, changed because of atomic stuff
                atomic_t ncount;            /* Usage count, see below. */
            };
        };
    };
    union   // temp workaround
    {
        unsigned long pfn$l_refcnt; // was w_, changed because of atomic stuff
        atomic_t count;         /* Usage count, see below. */
    };
    union
    {
        UINT64 pfn$q_bak;
        struct _phd *pfn$l_phd;
        struct
        {
            UINT32 pfn$l_color_flink;
            UINT32 pfn$l_color_blink;
        };
        struct
        {
            unsigned pfn$v_fill_5       : 16;
            unsigned pfn$v_typ0     : 1;
            unsigned pfn$v_fill_6       : 2;
            unsigned pfn$v_partial_section  : 1;
            unsigned pfn$v_fill_7       : 11;
            unsigned pfn$v_gblbak       : 1;
        };
        struct
        {
            unsigned pfn$v_fill_13      : 32;
            unsigned pfn$v_stx      : 16;
            unsigned pfn$v_crf      : 1;
            unsigned pfn$v_dzro     : 1;
            unsigned pfn$v_wrt      : 1;
            unsigned pfn$v_stx_hibit        : 1;
            unsigned pfn$v_fill_0_      : 4;
        };
        struct
        {
            unsigned pfn$v_fill_9       : 32;
            unsigned pfn$v_pgflpag      : 24;
            unsigned pfn$v_pgflx        : 8;
        };
        struct
        {
            unsigned pfn$v_fill_11      : 32;
            unsigned pfn$v_bakx     : 32;
        };
        struct
        {
            unsigned pfn$v_fill_12      : 32;
            unsigned pfn$v_gptx     : 32;
        };
        UINT64 pfn$q_bak_prvpfn;
    };
    union
    {
        UINT16 pfn$w_swppag;
        UINT16 pfn$w_bo_refc;
        UINT16 pfn$w_io_sts;
    };
    UINT16 pfn$w_pt_val_cnt;
    UINT16 pfn$w_pt_lck_cnt;
    UINT16 pfn$w_pt_win_cnt;

    struct list_head list;  /* for slab */  /* ->mapping has some page lists. */
#if 0
    struct address_space *mapping;  /* The inode (or ...) we belong to. */
    unsigned long index;        /* Our offset within mapping. */
#endif
    /*struct page *next_hash;*/     /* Next page sharing our hash bucket in
                       the pagecache hash table. */
    // count is moved up
    //  atomic_t count; ->pfn$l_refcnt      /* Usage count, see below. */
    /*  unsigned long flags;    */  /* atomic flags, some possibly
                       updated asynchronously */
    /*struct list_head lru*/;       /* Pageout list, eg. active_list;
                       protected by pagemap_lru_lock !! */
    /*  wait_queue_head_t wait; */  /* Page locked?  Stand in line... */
    /*struct page **pprev_hash;*/   /* Complement to *next_hash. */
    /* struct buffer_head * buffers; */ /* Buffer maps us to a disk block. */
    // keep virtual for now; need it for some time for mmg$allocontig_align
    /*  void *virtual;*/            /* Kernel virtual address (NULL if
                       not kmapped, ie. highmem) */
    //struct zone_struct *zone; /* Memory zone we are in. */
} mem_map_t;

struct _prvpfn
{
    struct _prvpfn *prvpfn$l_sqfl;
    struct _prvpfn *prvpfn$l_sqbl;
    UINT16 prvpfn$w_size;
    UINT8 prvpfn$b_type;
    UINT8 prvpfn$b_subtype;
    UINT32 prvpfn$l_count;
    UINT32 prvpfn$l_head;
    UINT32 prvpfn$l_tail;
    UINT32 prvpfn$l_reclaimable;
    UINT32 prvpfn$l_reclaimed;
    INT32 prvpfn$l_priority;
    void (*prvpfn$a_callback)(void);
};

typedef struct _pfn * PFN_PQ;
typedef struct _pfn ** PFN_PPQ;
typedef struct _prvpfn * PRVPFN_PQ;
typedef struct _prvpfn ** PRVPFN_PPQ;
typedef struct _pmap * PMAP_PQ;
typedef struct _pmap ** PMAP_PPQ;
typedef struct _plnk * PLNK_PQ;

#endif

