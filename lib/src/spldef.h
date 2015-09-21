#ifndef SPLDEF_H
#define SPLDEF_H

#include <cpudef.h>

#define SPL$C_SPL_SPINLOCK 1
#define SPL$C_SPL_FORKLOCK 2
#define SPL$C_SPL_DEVICELOCK 3
#define SPL$C_SPL_PORTLOCK_TEMPLATE 4
#define SPL$C_SPL_PORTLOCK 5
#define SPL$M_INTERLOCK 0x1
#define SPL$K_PC_VEC_CNT 16

struct _spl
{
    long spl$l_own_cpu;
    long spl$l_own_cnt;
    UINT16 spl$w_size;
    UINT8 spl$b_type;
    UINT8 spl$b_subtype;
    union
    {
        UINT32 spl$l_spinlock;
        struct
        {
            unsigned spl$v_interlock : 1;
            unsigned spl$v_fill_2_ : 7;
        };
    };
    INT32 spl$l_rank;
    union
    {
        UINT32 spl$l_ipl;
        struct
        {
            UINT8 spl$b_ipl;
            INT8 spl$b_fill1 [3];
        };
    };
    INT32 spl$l_rls_pc;
    UINT32 spl$l_busy_waits;
    INT32 spl$l_wait_cpus;
    INT32 spl$l_wait_pc;
    INT64 spl$q_spins;
    UINT64 spl$q_acq_count;
    UINT32 spl$l_timo_int;
    void *spl$ps_share_array;
    void *spl$ps_share_link;

    char spl$t_name [12];
    char spl$t_align [108];
    INT32 spl$l_vec_inx;
    INT32 spl$l_own_pc_vec [16];
};

#define SPL$K_LENGTH 256
#define SPL$C_LENGTH 256
#define SPL$S_SPLDEF 256

#define SPL_SHR$K_LENGTH 128
#define SPL_SHR$C_LENGTH 128

struct _spl_shr
{
    UINT32 spl_shr$l_link;
    INT32 spl_shr$l_share_count;
    UINT16 spl_shr$w_mbo;
    UINT8 spl_shr$b_type;
    UINT8 spl_shr$b_subtype;
    INT32 spl_shr$l_cpu_id;
    UINT64 spl_shr$q_size;
    INT64 spl_shr$q_timeout_int;
    char spl_shr$t_align [96];
};

#define SPLBLK$M_DYNAMIC_THRESHOLD 0x1
#define SPLBLK$C_LENGTH 256
#define SPLBLK$K_LENGTH 256
#define SPLBLK$C_SHIFT 8

struct _splblk
{
    UINT64 splblk$q_abuse_bitmask;
    UINT32 splblk$l_spares_1 [30];
    UINT64 splblk$q_release_count;
    UINT64 splblk$q_history_bitmask;
    UINT64 splblk$q_abuse_threshold;
    union
    {
        UINT64 splblk$q_flags;
        struct
        {
            unsigned splblk$v_dynamic_threshold : 1;
            unsigned splblk$v_fill_5_ : 7;
        };
    };
    UINT32 splblk$l_spares_2 [24];
};


#define SPLDBG$K_REV1 1
#define SPLDBG$K_REVISION 1
#define SPLDBG$K_ACQ 1
#define SPLDBG$K_REL 2
#define SPLDBG$K_SPIN 3
#define SPLDBG$K_FORKDSPTH 4
#define SPLDBG$K_FORKEND 5
#define SPLDBG$K_MAX_FLAG 5
#define SPLDBG$K_ACQNOIPL 1
#define SPLDBG$K_ACQUIRE 2
#define SPLDBG$K_ACQUIREL 3
#define SPLDBG$K_ACQNOIPL_OWN 4
#define SPLDBG$K_ACQUIRE_OWN 5
#define SPLDBG$K_ACQUIREL_OWN 6
#define SPLDBG$K_ACQUIRE_SHR_OWN 7
#define SPLDBG$K_ACQ_NOSPIN_OWN 8
#define SPLDBG$K_ACQ_SHR_NOSPIN_OWN 9
#define SPLDBG$K_ACQNOIPL_SPIN 10
#define SPLDBG$K_ACQUIRE_SPIN 11
#define SPLDBG$K_ACQUIREL_SPIN 12
#define SPLDBG$K_RESTORE 13
#define SPLDBG$K_RESTOREL 14
#define SPLDBG$K_RELEASE 15
#define SPLDBG$K_RELEASEL 16
#define SPLDBG$K_ACQUIRE_SHR 17
#define SPLDBG$K_ACQUIRE_SHR_SPIN 18
#define SPLDBG$K_RELEASE_SHR 19
#define SPLDBG$K_RESTORE_SHR 20
#define SPLDBG$K_ACQ_NOSPIN 21
#define SPLDBG$K_ACQ_NOSPIN_INUSE 22
#define SPLDBG$K_ACQ_SHR_NOSPIN 23
#define SPLDBG$K_ACQ_SHR_NOSPIN_INUSE 24
#define SPLDBG$K_ACQ_CVT_TO_EX 25
#define SPLDBG$K_ACQ_CVT_TO_EX_INUSE 26
#define SPLDBG$K_ACQ_CVT_TO_EX_SPIN 27
#define SPLDBG$K_ACQ_CVT_TO_SHR 28
#define SPLDBG$K_MAX_MODE 28
#define SPLDBG$M_ACQUIRE 0x1
#define SPLDBG$M_RELEASE 0x2
#define SPLDBG$M_SPINWAIT 0x4
#define SPLDBG$M_LCKMGR 0x8
#define SPLDBG$M_FORKDSPTH 0x10
#define SPLDBG$M_FORKEND 0x20
#define SPLDBG$K_LENGTH 616
#define SPLDBG$C_LENGTH 616

struct _spldbg
{
    struct _spltrh *spldbg$q_trace_buffer;
    UINT16 spldbg$w_mbo;
    UINT8 spldbg$b_type;
    UINT8 spldbg$b_subtype;
    UINT32 spldbg$l_revision;
    UINT64 spldbg$q_size;
    INT32 (*spldbg$l_start_trace)(void);
    INT32 (*spldbg$l_stop_trace)(void);
    void (*spldbg$l_trace_acquire)(void);
    void (*spldbg$l_trace_release)(void);
    void (*spldbg$l_trace_spinwait)(void);
    void (*spldbg$l_trace_forkdspth)(void);
    void (*spldbg$l_trace_forkend)(void);
    union
    {
        UINT32 spldbg$l_trace_flags;
        struct
        {
            unsigned spldbg$v_acquire : 1;
            unsigned spldbg$v_release : 1;
            unsigned spldbg$v_spinwait : 1;
            unsigned spldbg$v_lckmgr : 1;
            unsigned spldbg$v_forkdspth : 1;
            unsigned spldbg$v_forkend : 1;
            unsigned spldbg$v_fill_6_ : 2;
        };
    };
    UINT32 spldbg$l_spl_flags;
    INT32 spldbg$l_cpu_flags;
    UINT32 spldbg$l_frk_flags;
    UINT32 spldbg$l_trace_run;
    UINT64 spldbg$q_reserved1;
    UINT64 spldbg$q_reserved2;
    UINT64 spldbg$q_reserved3;
    UINT64 spldbg$q_reserved4;
    UINT64 spldbg$q_scc [32];
    UINT64 spldbg$q_systime [32];
};

#define SPLTRE$K_LENGTH 32

struct _spltre
{
    UINT64 spltre$q_timestamp;
    UINT32 spltre$l_pc;
    UINT32 spltre$l_cpuid;
    UINT32 spltre$l_mode;
    UINT32 spltre$l_flag;
    union
    {
        struct _spl *spltre$l_spl_addr;
        UINT32 spltre$l_flck;
    };
    struct _pcb *spltre$l_pcb;
};

#define SPLTRH$K_LENGTH 64

struct _spltrh
{
    INT32 spltrh$l_idx;
    UINT32 spltrh$l_max_idx;
    UINT16 spltrh$w_mbo;
    UINT8 spltrh$b_type;
    UINT8 spltrh$b_subtype;
    UINT32 spltrh$l_fill1;
    UINT64 spltrh$q_size;
    struct _spltre *spltrh$q_entry_ptr;
    struct _spltre spltrh$r_entry [1];
};

#endif

