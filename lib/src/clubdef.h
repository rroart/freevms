#ifndef CLUBDEF_H
#define CLUBDEF_H

#include <vms_types.h>

#define     CLUBFKB$M_FKB_BUSY      0x1
#define     CLUBFKB$M_FORKQ             0x2
#define     CLUBFKB$C_LENGTH        40
#define     CLUBFKB$K_LENGTH        40
#define     CLUBFKB$S_CLUBFKBDEF        40

#define     CLUBPWF$M_BUSY                  0x1
#define     CLUBPWF$C_LENGTH        40
#define     CLUBPWF$K_LENGTH        40
#define     CLUBPWF$S_CLUBPWFDEF        40

#define     CLUFCB$M_ACTIVE             0x1
#define     CLUFCB$M_PENDING        0x2
#define     CLUFCB$M_SYNC_NODE      0x4
#define     CLUFCB$M_FKB_BUSY       0x8
#define     CLUFCB$M_WAITING        0x10
#define     CLUFCB$M_AUX                0x20
#define     CLUFCB$M_RB_SUSPEND     0x40
#define     CLUFCB$C_LENGTH             120
#define     CLUFCB$K_LENGTH             120
#define     CLUFCB$S_CLUFCBDEF      120

#define     CLUB$M_CLUSTER          0x1
#define     CLUB$M_QF_ACTIVE    0x2
#define     CLUB$M_QF_DYNVOTE   0x4
#define     CLUB$M_QF_WATCHER   0x8
#define     CLUB$M_SHUTDOWN         0x10
#define     CLUB$M_QF_REFRESH_REQ   0x20
#define     CLUB$M_LNM_RESYNCH  0x40
#define     CLUB$M_STS_PPHASE   0x100
#define     CLUB$M_STS_PH0      0x200
#define     CLUB$M_STS_PH1B     0x400
#define     CLUB$M_STS_PH1      0x800
#define     CLUB$M_STS_PH2      0x1000
#define     CLUB$M_TDF_VALID    0x2000
#define     CLUB$M_FKB_BUSY     0x10000
#define     CLUB$M_UNLOCK       0x20000
#define     CLUB$M_NO_FORM      0x40000
#define     CLUB$M_INIT     0x80000
#define     CLUB$M_BACKOUT      0x100000
#define     CLUB$M_PRIOR_PROTOCOL   0x200000
#define     CLUB$M_VERBOSE      0x400000
#define     CLUB$M_LOST_CNX     0x800000
#define     CLUB$M_QF_FAILED_NODE   0x1000000
#define     CLUB$M_QF_VOTE      0x2000000
#define     CLUB$M_QF_NEWVOTE   0x4000000
#define     CLUB$M_ADJ_QUORUM   0x8000000
#define     CLUB$M_QUORUM       0x10000000
#define     CLUB$M_TRANSITION   0x20000000
#define     CLUB$M_RESLOCKIP    0x40000000
#define     CLUB$M_QTQEBSY      0x80000000
#define     CLUB$M_LK_MERGEIP   0x4
#define     CLUB$M_LK_DO_FULL   0x8
#define     CLUB$M_LK_FULL      0x10
#define     CLUB$M_LK_DO_DIR    0x20
#define     CLUB$M_LK_DIR       0x40
#define     CLUB$M_LK_NO_RMVDIR 0x80
#define     CLUB$M_LK_INIT_RBLD 0x100
#define     CLUB$M_LK_NO_RM     0x200
#define     CLUB$M_LK_TABLE_V51 0x400
#define     CLUB$M_LK_SPECIAL_1 0x800
#define     CLUB$M_LK_RM_DSBL   0x1000
#define     CLUB$M_LK_TABLE_1   0x2000
#define     CLUB$M_LK_SHUTDOWN  0x4000
#define     CLUB$M_LK_SHUT_IP   0x8000
#define     CLUB$M_NO_FQUORUM   0x1
#define     CLUB$M_NO_DQUORUM   0x2
#define     CLUB$M_IFW_REQ      0x4
#define     CLUB$M_RNS_REQ      0x8
#define     CLUB$M_CLUGEN_VALID 0x1
#define     CLUB$C_LENGTH       640
#define     CLUB$K_LENGTH       640
#define     CLUB$S_CLUBDEF      640

struct _clubfkb
{
    INT8 clubfkb$b_fork_block [32];
    UINT32 clubfkb$l_pc2;
    union
    {
        UINT32 clubfkb$l_status;
        struct
        {
            unsigned clubfkb$v_fkb_busy : 1;
            unsigned clubfkb$v_forkq : 1;
            unsigned clubfkb$v_fill_2_ : 6;
        };
    };
};

struct _clubpwf
{
    INT8 clubpwf$b_fork_block [32];


    union
    {
        UINT32 clubpwf$l_status;
        struct
        {
            unsigned clubpwf$v_busy : 1;
            unsigned clubpwf$v_fill_5_ : 7;
        };
    };
    char clubpwf$t_align [4];
};


struct _clufcb
{
    INT8 clufcb$b_fork_block [32];
    UINT32 clufcb$l_step;
    UINT32 clufcb$l_id;
    union
    {
        UINT32 clufcb$l_status;
        struct
        {
            unsigned clufcb$v_active : 1;
            unsigned clufcb$v_pending : 1;
            unsigned clufcb$v_sync_node : 1;
            unsigned clufcb$v_fkb_busy : 1;
            unsigned clufcb$v_waiting : 1;
            unsigned clufcb$v_aux : 1;
            unsigned clufcb$v_rb_suspend : 1;
            unsigned clufcb$v_fill_8_ : 1;
        };
    };
    struct _csb *clufcb$l_sync_csb;
    INT8 clufcb$b_nodemap [32];
    INT8 clufcb$b_respmap [32];
    UINT32 clufcb$l_index;
    struct _fkb *clufcb$l_aux_fkb;
};

struct _club
{
    struct _csb *club$l_csbqfl;
    struct _csb *club$l_csbqbl;
    UINT16 club$w_size;
    UINT8 club$b_type;
    UINT8 club$b_subtype;
    UINT32 club$l_poll_ctx;
    struct _csb *club$l_local_csb;
    struct _acb *club$l_astqfl;
    struct _acb *club$l_astqbl;
    union
    {
        UINT32 club$l_flags;
        struct
        {
            unsigned club$v_cluster : 1;
            unsigned club$v_qf_active : 1;
            unsigned club$v_qf_dynvote : 1;
            unsigned club$v_qf_watcher : 1;
            unsigned club$v_shutdown : 1;
            unsigned club$v_qf_refresh_req : 1;
            unsigned club$v_lnm_resynch : 1;
            unsigned club$v_fill_0 : 1;
            unsigned club$v_sts_pphase : 1;
            unsigned club$v_sts_ph0 : 1;
            unsigned club$v_sts_ph1b : 1;
            unsigned club$v_sts_ph1 : 1;
            unsigned club$v_sts_ph2 : 1;
            unsigned club$v_tdf_valid : 1;
            unsigned club$v_fill_01 : 2;
            unsigned club$v_fkb_busy : 1;
            unsigned club$v_unlock : 1;
            unsigned club$v_no_form : 1;
            unsigned club$v_init : 1;
            unsigned club$v_backout : 1;
            unsigned club$v_prior_protocol : 1;
            unsigned club$v_verbose : 1;
            unsigned club$v_lost_cnx : 1;
            unsigned club$v_qf_failed_node : 1;
            unsigned club$v_qf_vote : 1;
            unsigned club$v_qf_newvote : 1;
            unsigned club$v_adj_quorum : 1;
            unsigned club$v_quorum : 1;
            unsigned club$v_transition : 1;
            unsigned club$v_reslockip : 1;
            unsigned club$v_qtqebsy : 1;
        };
    };
    union
    {
        UINT32 club$l_lk_flags;
        struct
        {
            unsigned club$v_fill_10 : 1;
            unsigned club$v_fill_11 : 1;
            unsigned club$v_lk_mergeip : 1;
            unsigned club$v_lk_do_full : 1;
            unsigned club$v_lk_full : 1;
            unsigned club$v_lk_do_dir : 1;
            unsigned club$v_lk_dir : 1;
            unsigned club$v_lk_no_rmvdir : 1;
            unsigned club$v_lk_init_rbld : 1;
            unsigned club$v_lk_no_rm : 1;
            unsigned club$v_lk_table_v51 : 1;
            unsigned club$v_lk_special_1 : 1;
            unsigned club$v_lk_rm_dsbl : 1;
            unsigned club$v_lk_table_1 : 1;
            unsigned club$v_lk_shutdown : 1;
            unsigned club$v_lk_shut_ip : 1;
        };
    };
    UINT16 club$w_rseqnum;
    UINT16 club$w_dirseqnum;
    union
    {
        UINT8 club$b_qstatus;
        struct
        {
            unsigned club$v_no_fquorum : 1;
            unsigned club$v_no_dquorum : 1;
            unsigned club$v_ifw_req : 1;
            unsigned club$v_rns_req : 1;
            unsigned club$v_fill_03 : 4;
        };
    };
    INT8 club$b_fill_2;
    UINT16 club$w_qdvotes;
    UINT16 club$w_quorum;
    UINT16 club$w_votes;
    UINT16 club$w_cevotes;
    UINT16 club$w_adj_cevotes;
    UINT16 club$w_nodes;
    INT8 club$b_fsysid [6];
    INT64 club$q_ftime;
    UINT32 club$l_lst_xtn;
    UINT32 club$l_lst_coord;
    INT64 club$q_lst_time;
    UINT8 club$b_lst_code;
    UINT8 club$b_lst_phase;
    UINT16 club$w_newqdvotes;
    UINT32 club$l_cur_xtn;
    UINT32 club$l_cur_coord;
    INT64 club$q_cur_time;
    UINT8 club$b_cur_code;
    UINT8 club$b_cur_phase;
    UINT16 club$w_msgcnt;
    struct _csb *club$l_coord;
    union
    {
        UINT32 club$l_local_csid;
        struct
        {
            UINT16 club$w_local_csid_idx;
            UINT16 club$w_local_csid_seq;
        };
    };
    UINT16 club$w_next_csid;
    UINT16 club$w_first_index;
    UINT32 club$l_max_xtn;
    UINT32 club$l_retrycnt;
    UINT32 club$l_ctx0;
    UINT32 club$l_ret1;
    UINT32 club$l_ctx1;
    UINT32 club$l_ret2;
    UINT32 club$l_ctx2;
    struct _tqe *club$l_tqe;
    UINT32 club$l_cspipid;
    UINT64 club$q_newtime;
    UINT64 club$q_newtime_ref;
    UINT16 club$w_newquorum;
    UINT16 club$w_newcevotes;
    UINT32 club$l_fmerit;
    union
    {
        UINT32 club$l_e_memseq;
        struct
        {
            UINT16 club$w_memseq;
            INT16 club$w_fill_1;
        };
    };
    UINT32 club$l_random;
    struct _cludcb *club$l_cludcb;
    char club$t_qdname [16];
    struct _cluicb *club$l_cluicb;
    UINT32 club$l_foreign_cluster;
    UINT32 club$l_enbl_verbose;
    UINT32 club$l_qlost_clugen;
    UINT32 club$l_stg_join_clugen;
    UINT32 club$l_join_clugen;
    UINT16 club$w_stg_join_flags;
    union
    {
        UINT16 club$w_join_flags;
        struct
        {
            unsigned club$v_clugen_valid : 1;
            unsigned club$v_fill_04 : 15;
        };
    };
    UINT32 club$l_rm_quota;
    UINT64 club$q_cspq;
    INT8 club$b_fork_block [40];
    INT8 club$b_nodemap [32];
    INT8 club$b_clufcb [120];
    INT8 club$b_hang_fkb [32];
    INT8 club$b_clubpwf [40];
    UINT32 club$l_reslocktmo;
    UINT32 club$l_reslockcsid;
    UINT32 club$l_locktime;
    UINT16 club$w_merge_cnt;
    UINT16 club$w_parseqnum;
    UINT8 club$b_newrbld_req;
    UINT8 club$b_rbld_clu;
    UINT8 club$b_rbld_loc;
    INT8 club$b_fill_21;
    struct _clurcb *club$l_clurcbfl;
    struct _clurcb *club$l_clurcbbl;
    UINT16 club$w_rbld_inhib;
    INT16 club$w_fill_22;
    UINT16 club$w_ncnid;
    UINT16 club$w_newncnid;
    UINT32 club$l_toff;
    UINT32 club$l_ton;
    void *club$l_tbls;
    struct _tqe *club$l_qtqe;
    UINT32 club$l_sync_step;
    UINT64 club$q_tdf;
    UINT64 club$q_block_seq;

    INT8 club$b_lckmgr_fork_block [40];
    UINT32 club$l_rmbuf_link;
    UINT32 club$l_max_rmbufs;
    UINT32 club$l_cached_rmbufs;
    UINT32 club$l_tot_rmbufs;
};

#endif



