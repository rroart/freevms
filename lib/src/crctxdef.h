#ifndef CRCTXDEF_H
#define CRCTXDEF_H

#include <vms_types.h>

#define CRCTX$M_HIGH_PRIO 0x1
#define CRCTX$M_ITEM_VALID 0x2

#define CRCTX$K_LENGTH 96
#define CRCTX$S_CRCTXDEF 96

#define CRCTX_BUF$K_LENGTH 32

struct _crctx
{
    struct _crctx *crctx$l_flink;
    struct _crctx *crctx$l_blink;
    UINT16 crctx$w_size;
    UINT8 crctx$b_type;
    UINT8 crctx$b_subtype;
    struct _crab *crctx$l_crab;
    union
    {
        UINT8 crctx$b_flck;
        UINT32 crctx$l_flck;
    };
    union
    {
        INT32 crctx$l_flags;
        struct
        {
            unsigned crctx$v_high_prio : 1;
            unsigned crctx$v_item_valid : 1;
            unsigned crctx$v_fill_0_ : 6;
        };
    };
    void *crctx$l_wqfl;
    void *crctx$l_wqbl;
    union
    {
        INT32 crctx$l_context1;
        INT64 crctx$q_context1;
    };
    union
    {
        INT32 crctx$l_context2;
        INT64 crctx$q_context2;
    };
    union
    {
        INT32 crctx$l_context3;
        INT64 crctx$q_context3;
    };
    INT32 crctx$l_item_cnt;
    INT32 crctx$l_item_num;
    INT32 crctx$l_up_bound;
    INT32 crctx$l_low_bound;
    INT32 (*crctx$l_callback)(void);
    INT32 (*crctx$l_saved_callback)(void);
    void *crctx$l_aux_context;
    INT32 crctx$l_reserved1;
    INT32 crctx$l_dma_adr;
    void *crctx$ps_caller_pc;
};

struct _crctx_buf
{
    UINT32 crctx_buf$l_xaction;
    UINT32 crctx_buf$l_item_num;
    UINT32 crctx_buf$l_item_cnt;
    struct _crctx *crctx_buf$ps_crctx;
    void *crctx_buf$ps_caller_pc;
    UINT32 crctx_buf$l_status;
    UINT32 crctx_buf$l_dma_addr;
    UINT32 crctx_buf$l_count;
};

struct _crctx_buf_h
{
    INT64 crctx_buf_h$q_free;
    UINT32 crctx_buf_h$l_size;
    INT32 crctx_buf_h$l_reserved;
    INT32 crctx_buf_h$l_count;
    INT32 crctx_buf_h$l_reserved2;
};

#endif

