#ifndef PSCANCTXDEF_H
#define PSCANCTXDEF_H

#include <vms_types.h>

#define     PSCANCTX$M_SUPER        0x1
#define     PSCANCTX$M_CSID_VEC     0x2
#define     PSCANCTX$M_LOCKED       0x4
#define     PSCANCTX$M_MULTI_NODE       0x8
#define     PSCANCTX$M_BUSY             0x10
#define     PSCANCTX$M_RELOCK       0x20
#define     PSCANCTX$M_THREAD       0x40
#define     PSCANCTX$M_NEED_THREAD_CAP  0x80
#define     PSCANCTX$M_SCHED_CLASS_CAP  0x100
#define     PSCANCTX$K_LENGTH       64
#define     PSCANCTX$M_THREAD_ITEM      1073741824
#define     PSCANCTX$V_THREAD_ITEM      30
#define     PSCANCTX$M_NODE_ITEM        -2147483648
#define     PSCANCTX$V_NODE_ITEM        31
#define     PSCANCTX$S_$PSCANCTXDEF     64

#define     PSCANBUF$M_SPARE0       0x1
#define     PSCANBUF$K_LENGTH       32
#define     PSCANBUF$S_$PSCANBUFDEF     32

#define     PSCANITM$K_LENGTH       12
#define     PSCANITM$S_$PSCANITMDEF     13

struct _pscanctx
{
    void *pscanctx$l_flink;
    void *pscanctx$l_blink;
    UINT16 pscanctx$w_size;
    UINT8 pscanctx$b_type;
    UINT8 pscanctx$b_subtype;
    UINT16 pscanctx$w_maj_vers;
    UINT16 pscanctx$w_min_vers;
    union
    {
        UINT32 pscanctx$l_flags;
        struct
        {
            unsigned pscanctx$v_super : 1;
            unsigned pscanctx$v_csid_vec : 1;
            unsigned pscanctx$v_locked : 1;
            unsigned pscanctx$v_multi_node : 1;
            unsigned pscanctx$v_busy : 1;
            unsigned pscanctx$v_relock : 1;
            unsigned pscanctx$v_thread : 1;
            unsigned pscanctx$v_need_thread_cap : 1;
            unsigned pscanctx$v_sched_class_cap : 1;
            unsigned pscanctx$v_fill_2_ : 7;
        };
    };
    UINT32 pscanctx$l_cur_csid;
    UINT32 pscanctx$l_cur_ipid;
    UINT32 pscanctx$l_next_ipid;
    UINT32 pscanctx$l_cur_epid;
    UINT16 pscanctx$w_itmlstoff;
    UINT16 pscanctx$w_bufferoff;
    UINT16 pscanctx$w_csidoff;
    UINT16 pscanctx$w_csididx;
    UINT32 pscanctx$l_svapte;
    struct _cwpssrv *pscanctx$l_cwpssrv;
    void *pscanctx$l_jpibufadr;
    UINT16 pscanctx$w_seqnum;
    UINT8 pscanctx$b_acmode;
    UINT8 pscanctx$b_spare0;
    UINT32 pscanctx$l_spare1;
};

struct _pscanbuf
{
    UINT32 pscanbuf$l_buflen;
    UINT32 pscanbuf$l_spare0;
    UINT16 pscanbuf$w_size;
    UINT8 pscanbuf$b_type;
    UINT8 pscanbuf$b_subtype;
    UINT16 pscanbuf$w_maj_vers;
    UINT16 pscanbuf$w_min_vers;
    union
    {
        UINT32 pscanbuf$l_flags;
        struct
        {
            unsigned pscanbuf$v_spare0  : 1;
            unsigned pscanbuf$v_fill_5_ : 7;
        };
    };
    void *pscanbuf$l_itmlstadr;
    UINT32 pscanbuf$l_buffer_offset;
    UINT32 pscanbuf$l_free_offset;
};

struct _pscanitm
{
    UINT32 pscanitm$l_alloc_length;
    UINT32 pscanitm$l_itmlen;
    UINT16 pscanitm$w_size;
    UINT8 pscanitm$b_type;
    UINT8 pscanitm$b_subtype;
    UINT8 pscanitm$b_itmlst;
};

#endif

