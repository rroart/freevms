#ifndef CRABDEF_H
#define CRABDEF_H

#define CRAB$K_LENGTH 88

#define CRAB_ARRAY$K_LENGTH 8

struct _crab
{
    struct _crab *crab$l_flink;
    struct _crab *crab$l_blink;
    UINT16 crab$w_size;
    UINT8 crab$b_type;
    UINT8 crab$b_subtype;
    struct _spl *crab$l_spinlock;
    void *crab$l_wqfl;
    void *crab$l_wqbl;
    struct _adp *crab$ps_adp;
    UINT32 crab$l_total_items;
    UINT32 crab$l_alloc_gran_mask;
    struct _spl *crab$l_nosync_spinlock;
    void (*crab$ps_dealloc_cb)(void);
    void *crab$ps_ringbuffer;

    UINT32 crab$l_base_item;
    UINT32 crab$l_valid_desc_cnt;
    struct _fkb *crab$l_fqfl;
    struct _fkb *crab$l_fqbl;
    UINT16 crab$w_fsize;
    UINT8 crab$b_ftype;
    UINT8 crab$b_flck;
    void (*crab$l_fpc)(void);
    INT64 crab$q_fr3;
    INT64 crab$q_fr4;
    UINT32 crab$l_alloc_array;
    INT8 crab$b_fill_0_ [4];
};

struct _crab_array
{
    UINT32 crab_array$l_num;
    UINT32 crab_array$l_cnt;
};

#endif

