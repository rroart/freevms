#ifndef LCKCTXDEF_H
#define LCKCTXDEF_H

#include <vms_types.h>

#define LCKCTX$M_BUSY 0x1
#define LCKCTX$M_CANCEL 0x2
#define LCKCTX$M_CMP_RQD 0x4
#define LCKCTX$K_LEN 200
#define LCKCTX$C_LEN 200
#define LCKCTX$S_LCKCTXDEF 200

struct _lckctx
{
    struct
    {
        void *lckctx$l_fqfl;
        void *lckctx$l_fqbl;
        UINT16 lckctx$w_size;
        UINT8 lckctx$b_type;
        UINT8 lckctx$b_flck;
        void (*lckctx$l_fpc)(void);
        unsigned long lckctx$l_fr3;
        unsigned long lckctx$l_fr4;
    }; /* fkb */
    union
    {
        UINT32 lckctx$l_flags;
        struct
        {
            unsigned lckctx$v_busy : 1;
            unsigned lckctx$v_cancel : 1;
            unsigned lckctx$v_cmp_rqd : 1;
            unsigned lckctx$v_fill_0_ : 5;
        } ;
    } ;
    INT8 lckctx$b_fill_1_ [4];
    struct _lkb *lckctx$q_lkb;
    union
    {
        struct
        {
            UINT64 lckctx$q_cr3;
            UINT64 lckctx$q_cr4;
            UINT64 lckctx$q_cr5;
            void *lckctx$pq_ret1;
            UINT64 lckctx$q_tmp1;
        };
        struct
        {
            void *lckctx$pq_cpladr;
            UINT64 lckctx$q_cplprm;
        };
    };
    union
    {
        INT8 lckctx$b_args [112];
        struct
        {
            UINT64 lckctx$q_enq_lock_acmode;
            UINT64 lckctx$q_enq_lkmode;
            UINT64 lckctx$q_enq_lksb;
            UINT64 lckctx$q_enq_flags;
            UINT64 lckctx$q_enq_resnam;
            UINT64 lckctx$q_enq_parid;
            UINT64 lckctx$q_enq_cmp_adr;
            UINT64 lckctx$q_enq_ctx_prm1;
            UINT64 lckctx$q_enq_ctx_prm2;
            UINT64 lckctx$q_enq_ctx_prm3;
            UINT64 lckctx$q_enq_blk_adr;
            UINT64 lckctx$q_enq_name_acmode;
            UINT64 lckctx$q_enq_priority;
            void *lckctx$pq_enq_req_acpted_adr;
        };
        struct
        {
            UINT64 lckctx$q_deq_lockid;
            UINT64 lckctx$q_deq_valblk;
            UINT64 lckctx$q_deq_flags;
            UINT64 lckctx$q_deq_ctx_prm1;
            UINT64 lckctx$q_deq_ctx_prm2;
            UINT64 lckctx$q_deq_ctx_prm3;
            void *lckctx$pq_deq_retadr;
        } ;
    };
};

#endif

