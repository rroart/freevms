#ifndef IDBDEF_H
#define IDBDEF_H

#include <vms_types.h>

#define     IDB$M_CRAM_ALLOC    0x1
#define     IDB$M_VLE       0x2
#define     IDB$M_NORESIZE      0x4
#define     IDB$M_MCJ       0x8
#define     IDB$M_SHARED_INT    0x10
#define     IDB$M_DISTRIBUTED_INT   0x20
#define     IDB$M_ISR_CALLABLE  0x40
#define     IDB$K_BASE_LENGTH   56
#define     IDB$C_BASE_LENGTH   56

#define     IDB$K_LENGTH        88
#define     IDB$C_LENGTH        88
#define     IDB$S_IDBDEF        88

struct _idb
{
    UINT64 idb$q_csr;
    UINT16 idb$w_size;
    UINT8 idb$b_type;
    INT8 idb$b_spare;
    UINT16 idb$w_units;
    UINT16 idb$w_tt_enable;
    struct _ucb *idb$ps_owner;
    struct _cram *idb$ps_cram;
    struct _spl *idb$ps_spl;
    struct _adp *idb$ps_adp;
    union
    {
        UINT32 idb$l_flags;
        struct
        {
            unsigned idb$v_cram_alloc      : 1;
            unsigned idb$v_vle             : 1;
            unsigned idb$v_noresize        : 1;
            unsigned idb$v_mcj             : 1;
            unsigned idb$v_shared_int      : 1;
            unsigned idb$v_distributed_int : 1;
            unsigned idb$v_isr_callable    : 1;
            unsigned idb$v_fill_0_         : 1;
        };
    };
    UINT32 idb$l_device_specific;
    INT32 idb$l_vector;
    void *idb$ps_auxstruc;
    UINT32 idb$l_interrupt_cpu;
    UINT32 idb$l_reserved;
    struct _ucb *idb$l_ucblst [8];
};

#endif

