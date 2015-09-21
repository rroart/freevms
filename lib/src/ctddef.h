#ifndef CTDDEF_H
#define CTDDEF_H

struct _ctd
{
    struct _ctd *ctd$pq_qlink;
    UINT16 ctd$w_size;
    UINT8 ctd$b_type;
    UINT8 ctd$b_rmod;
    UINT32 ctd$l_pid;
    UINT32 ctd$l_acb64x;
    UINT32 ctd$l_flags;
    UINT32 ctd$l_acb_flags;
    UINT32 ctd$l_thread_pid;
    void (*ctd$l_kast)(void);
    UINT32 ctd$l_imgcnt;
    void (*ctd$pq_ast)(void);
    UINT64 ctd$q_astprm;
    void *ctd$pq_iosb;
    UINT32 ctd$l_current_tran_code;
    UINT32 ctd$l_final_tran_code;
    UINT32 ctd$l_source_node;
    UINT32 ctd$l_target_node;
    UINT32 ctd$l_status;
    UINT32 ctd$l_efn;
    struct _tqe *ctd$l_timeout_tqe;
    INT8 ctd$b_fill_0_ [4];
    UINT64 ctd$q_start_time;
    UINT64 ctd$q_interim_time;
};

#define CTD$K_LENGTH 112

typedef struct _ctd *  CTD_PQ;
typedef struct _ctd ** CTD_PPQ;

#endif

