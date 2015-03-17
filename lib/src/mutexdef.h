#ifndef mutexdef_h
#define mutexdef_h

#include <cpudef.h>

#define MUTEX$M_WRT 0x1
#define MUTEX$M_INTERLOCK 0x1
#define MUTEX$S_MUTEXDEF 8

struct _mutex
{
    union
    {
        unsigned short int mutex$w_sts;
        unsigned mutex$v_wrt : 1;
        unsigned mutex$v_interlock : 1;
    };
    unsigned short int mutex$w_fill_2;
    unsigned int mutex$l_owncnt;
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
    unsigned short int mtxdbg$w_mbo;
    unsigned char mtxdbg$b_type;
    unsigned char mtxdbg$b_subtype;
    unsigned int mtxdbg$l_revision;
    long long mtxdbg$q_size;
    int (*mtxdbg$l_start_trace)();
    int (*mtxdbg$l_stop_trace)();
    void (*mtxdbg$l_trace_mutex)();
    void (*mtxdbg$l_trace_mutex_wait)();
    void (*mtxdbg$l_debug_print)();
    union
    {
        unsigned int mtxdbg$l_trace_flags;
        struct
        {
            unsigned mtxdbg$v_mutex : 1;
            unsigned mtxdbg$v_mutex_wait : 1;
            unsigned mtxdbg$v_fill_0_ : 6;
        };
    };
    unsigned int mtxdbg$l_mtx_flags;
    int mtxdbg$l_cpu_flags;
    unsigned int mtxdbg$l_trace_run;
    char mtxdbg$b_fill_1_ [4];
    unsigned long long mtxdbg$q_reserved1;
    unsigned long long mtxdbg$q_reserved2;
    unsigned long long mtxdbg$q_reserved3;
    unsigned long long mtxdbg$q_reserved4;
    unsigned long long mtxdbg$q_scc [32];
    unsigned long long mtxdbg$q_systime [32];
};

#define MTXTRE$K_LENGTH 32

struct _mtxtre
{
    unsigned long long mtxtre$q_timestamp;
    unsigned int mtxtre$l_pc;
    unsigned int mtxtre$l_cpuid;
    unsigned int mtxtre$l_mode;
    unsigned int mtxtre$l_flag;
    unsigned int mtxtre$l_mutex;
    struct _pcb *mtxtre$l_pcb;
};
#define MTXTRH$K_LENGTH 64

struct _mtxtrh
{
    int mtxtrh$l_idx;
    unsigned int mtxtrh$l_max_idx;
    unsigned short int mtxtrh$w_mbo;
    unsigned char mtxtrh$b_type;
    unsigned char mtxtrh$b_subtype;
    unsigned int mtxtrh$l_fill1;
    long long mtxtrh$q_size;
    struct _mtxtre *mtxtrh$q_entry_ptr;
    struct _mtxtre mtxtrh$r_entry [1];
};

#endif

