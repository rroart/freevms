#ifndef MUTEXDEF_H
#define MUTEXDEF_H

#include <cpudef.h>

#define MUTEX$M_WRT 0x1
#define MUTEX$M_INTERLOCK 0x1
#define MUTEX$S_MUTEXDEF 8

struct _mutex
{
    union
    {
        UINT16 mutex$w_sts;
        unsigned mutex$v_wrt : 1;
        unsigned mutex$v_interlock : 1;
    };
    UINT16 mutex$w_fill_2;
    UINT32 mutex$l_owncnt;
};

#define MTXDBG$K_REV1 1
#define MTXDBG$K_REV2 2
#define MTXDBG$K_REVISION 2
#define MTXDBG$K_MUTEX 1
#define MTXDBG$K_INFO 2
#define MTXDBG$K_MAX_FLAG 2
#define MTXDBG$K_LOCKR 1
#define MTXDBG$K_LOCKW 2
#define MTXDBG$K_UNLOCK 3
#define MTXDBG$K_LOCKREXEC 4
#define MTXDBG$K_LOCKWEXEC 5
#define MTXDBG$K_UNLOCKEXEC 6
#define MTXDBG$K_LOCKR_QUAD 7
#define MTXDBG$K_LOCKW_QUAD 8
#define MTXDBG$K_UNLOCK_QUAD 9
#define MTXDBG$K_LOCKREXEC_QUAD 10
#define MTXDBG$K_LOCKWEXEC_QUAD 11
#define MTXDBG$K_UNLOCKEXEC_QUAD 12
#define MTXDBG$K_LOCKWNOWAIT 13
#define MTXDBG$K_LOCKWNOWAIT_QUAD 14
#define MTXDBG$K_LOCKR_WAIT 15
#define MTXDBG$K_LOCKW_WAIT 16
#define MTXDBG$K_UNLOCK_REL 17
#define MTXDBG$K_LOCKREXEC_INUSE 18
#define MTXDBG$K_LOCKWEXEC_INUSE 19
#define MTXDBG$K_UNLOCKEXEC_REL 20
#define MTXDBG$K_LOCKR_QUAD_WAIT 21
#define MTXDBG$K_LOCKW_QUAD_WAIT 22
#define MTXDBG$K_UNLOCK_QUAD_REL 23
#define MTXDBG$K_LOCKREXEC_QUAD_INUSE 24
#define MTXDBG$K_LOCKWEXEC_QUAD_INUSE 25
#define MTXDBG$K_UNLOCKEXEC_QUAD_REL 26
#define MTXDBG$K_MAX_MODE 26
#define MTXDBG$M_MUTEX 0x1
#define MTXDBG$M_MUTEX_WAIT 0x2
#define MTXDBG$K_LENGTH 608
#define MTXDBG$C_LENGTH 608

struct _mtxdbg
{
    struct _mtxtrh *mtxdbg$q_trace_buffer;
    UINT16 mtxdbg$w_mbo;
    UINT8 mtxdbg$b_type;
    UINT8 mtxdbg$b_subtype;
    UINT32 mtxdbg$l_revision;
    INT64 mtxdbg$q_size;
    INT32 (*mtxdbg$l_start_trace)(void);
    INT32 (*mtxdbg$l_stop_trace)(void);
    void (*mtxdbg$l_trace_mutex)(void);
    void (*mtxdbg$l_trace_mutex_wait)(void);
    void (*mtxdbg$l_debug_print)(void);
    union
    {
        UINT32 mtxdbg$l_trace_flags;
        struct
        {
            unsigned mtxdbg$v_mutex : 1;
            unsigned mtxdbg$v_mutex_wait : 1;
            unsigned mtxdbg$v_fill_0_ : 6;
        };
    };
    UINT32 mtxdbg$l_mtx_flags;
    INT32 mtxdbg$l_cpu_flags;
    UINT32 mtxdbg$l_trace_run;
    INT8 mtxdbg$b_fill_1_ [4];
    UINT64 mtxdbg$q_reserved1;
    UINT64 mtxdbg$q_reserved2;
    UINT64 mtxdbg$q_reserved3;
    UINT64 mtxdbg$q_reserved4;
    UINT64 mtxdbg$q_scc [32];
    UINT64 mtxdbg$q_systime [32];
};

#define MTXTRE$K_LENGTH 32

struct _mtxtre
{
    UINT64 mtxtre$q_timestamp;
    UINT32 mtxtre$l_pc;
    UINT32 mtxtre$l_cpuid;
    UINT32 mtxtre$l_mode;
    UINT32 mtxtre$l_flag;
    UINT32 mtxtre$l_mutex;
    struct _pcb *mtxtre$l_pcb;
};
#define MTXTRH$K_LENGTH 64

struct _mtxtrh
{
    INT32 mtxtrh$l_idx;
    UINT32 mtxtrh$l_max_idx;
    UINT16 mtxtrh$w_mbo;
    UINT8 mtxtrh$b_type;
    UINT8 mtxtrh$b_subtype;
    UINT32 mtxtrh$l_fill1;
    INT64 mtxtrh$q_size;
    struct _mtxtre *mtxtrh$q_entry_ptr;
    struct _mtxtre mtxtrh$r_entry [1];
};

#endif

