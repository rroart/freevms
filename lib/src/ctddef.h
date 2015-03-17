#ifndef ctddef_h
#define ctddef_h

struct _ctd
{
    struct _ctd *ctd$pq_qlink;
    unsigned short int ctd$w_size;
    unsigned char ctd$b_type;
    unsigned char ctd$b_rmod;
    unsigned int ctd$l_pid;
    unsigned int ctd$l_acb64x;
    unsigned int ctd$l_flags;
    unsigned int ctd$l_acb_flags;
    unsigned int ctd$l_thread_pid;
    void (*ctd$l_kast)(void);
    unsigned int ctd$l_imgcnt;
    void (*ctd$pq_ast)(void);
    unsigned long long ctd$q_astprm;
    void *ctd$pq_iosb;
    unsigned int ctd$l_current_tran_code;
    unsigned int ctd$l_final_tran_code;
    unsigned int ctd$l_source_node;
    unsigned int ctd$l_target_node;
    unsigned int ctd$l_status;
    unsigned int ctd$l_efn;
    struct _tqe *ctd$l_timeout_tqe;
    char ctd$b_fill_0_ [4];
    unsigned long long ctd$q_start_time;
    unsigned long long ctd$q_interim_time;
};

#define CTD$K_LENGTH 112

typedef struct _ctd *  CTD_PQ;
typedef struct _ctd ** CTD_PPQ;

#endif

