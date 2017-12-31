#ifndef cludcbdef_h
#define cludcbdef_h

/* as if this will be used in ages... */

#define CLUDCB$M_QS_REM_INA 0x1
#define CLUDCB$M_QS_REM_ACT 0x2
#define CLUDCB$M_QS_NOT_READY 0x4
#define CLUDCB$M_QS_READY 0x8
#define CLUDCB$M_QS_ACTIVE 0x10
#define CLUDCB$M_QS_CLUSTER 0x20
#define CLUDCB$M_QS_VOTE 0x40
#define CLUDCB$M_QF_INQTMO 0x1
#define CLUDCB$M_QF_INQIP 0x2
#define CLUDCB$M_QF_TIM 0x4
#define CLUDCB$M_QF_RIP 0x8
#define CLUDCB$M_QF_WIP 0x10
#define CLUDCB$M_QF_ERROR 0x20
#define CLUDCB$M_QF_FIRST_ERR 0x40
#define CLUDCB$M_QF_WRL_ERR 0x80
#define CLUDCB$M_QF_NOACCESS 0x100
#define CLUDCB$M_CSP_ACK 0x1
#define CLUDCB$M_CSP_LBN_VALID 0x2
#define CLUDCB$M_CSP_MVHELP 0x4
#define CLUDCB$K_F_LENGTH 64
#define CLUDCB$C_F_LENGTH 64
#define CLUDCB$K_LENGTH 580
#define CLUDCB$C_LENGTH 580

#define CLUDCB$S_DISK_QUORUM 16

struct _cludcb
{
    struct _cludcb *cludcb$l_cludcbfl;
    struct _cludcb *cludcb$l_cludcbbl;
    unsigned short int cludcb$w_size;
    unsigned char cludcb$b_type;
    unsigned char cludcb$b_subtype;
    struct _ucb *cludcb$l_ucb;
    struct _irp *cludcb$l_irp;
    struct _tqe *cludcb$l_tqe;
    unsigned int cludcb$l_watcher_csid;
    unsigned int cludcb$l_act_count;

    unsigned int cludcb$l_qflbn;
    char cludcb$t_align2 [4];
    union
    {
        unsigned short int cludcb$w_state;
        struct
        {
            unsigned cludcb$v_qs_rem_ina : 1;
            unsigned cludcb$v_qs_rem_act : 1;
            unsigned cludcb$v_qs_not_ready : 1;
            unsigned cludcb$v_qs_ready : 1;
            unsigned cludcb$v_qs_active : 1;
            unsigned cludcb$v_qs_cluster : 1;
            unsigned cludcb$v_qs_vote : 1;
            unsigned cludcb$v_fill_6_ : 1;
        };
    };
    short int cludcb$w_fill_1;
    union
    {
        unsigned short int cludcb$w_flags;
        struct
        {
            unsigned cludcb$v_qf_inqtmo : 1;
            unsigned cludcb$v_qf_inqip : 1;
            unsigned cludcb$v_qf_tim : 1;
            unsigned cludcb$v_qf_rip : 1;
            unsigned cludcb$v_qf_wip : 1;
            unsigned cludcb$v_qf_error : 1;
            unsigned cludcb$v_qf_first_err : 1;
            unsigned cludcb$v_qf_wrl_err : 1;
            unsigned cludcb$v_qf_noaccess : 1;
            unsigned cludcb$v_fill_7_ : 7;
        };
    };
    short int cludcb$w_fill_2;


    union
    {
        unsigned short int cludcb$w_csp_flags;
        struct
        {
            unsigned cludcb$v_csp_ack : 1;
            unsigned cludcb$v_csp_lbn_valid : 1;
            unsigned cludcb$v_csp_mvhelp : 1;
            unsigned cludcb$v_fill_8_ : 5;
        };
    };
    char cludcb$t_align4 [6];
    unsigned char cludcb$b_counter;
    char cludcb$b_fill_3 [3];
    char cludcb$t_align5 [4];

    char cludcb$t_buffer [516];

};

#endif

