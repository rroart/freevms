#ifndef crabdef_h
#define crabdef_h

#define CRAB$K_LENGTH 88

#define CRAB_ARRAY$K_LENGTH 8

struct _crab
{
    struct _crab *crab$l_flink;
    struct _crab *crab$l_blink;
    unsigned short int crab$w_size;
    unsigned char crab$b_type;
    unsigned char crab$b_subtype;
    struct _spl *crab$l_spinlock;
    void *crab$l_wqfl;
    void *crab$l_wqbl;
    struct _adp *crab$ps_adp;
    unsigned int crab$l_total_items;
    unsigned int crab$l_alloc_gran_mask;
    struct _spl *crab$l_nosync_spinlock;
    void (*crab$ps_dealloc_cb)();
    void *crab$ps_ringbuffer;

    unsigned int crab$l_base_item;
    unsigned int crab$l_valid_desc_cnt;
    struct _fkb *crab$l_fqfl;
    struct _fkb *crab$l_fqbl;
    unsigned short int crab$w_fsize;
    unsigned char crab$b_ftype;
    unsigned char crab$b_flck;
    void (*crab$l_fpc)();
    long long crab$q_fr3;
    long long crab$q_fr4;
    unsigned int crab$l_alloc_array;
    char crab$b_fill_0_ [4];
};

struct _crab_array
{
    unsigned int crab_array$l_num;
    unsigned int crab_array$l_cnt;
};

#endif

