#ifndef CLUBDEF_H
#define CLUBDEF_H

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
    char clubfkb$b_fork_block [32];
    unsigned int clubfkb$l_pc2;
    union
    {
        unsigned int clubfkb$l_status;
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
    char clubpwf$b_fork_block [32];


    union
    {
        unsigned int clubpwf$l_status;
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
    char clufcb$b_fork_block [32];
    unsigned int clufcb$l_step;
    unsigned int clufcb$l_id;
    union
    {
        unsigned int clufcb$l_status;
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
    char clufcb$b_nodemap [32];
    char clufcb$b_respmap [32];
    unsigned int clufcb$l_index;
    struct _fkb *clufcb$l_aux_fkb;
};

struct _club
{
    struct _csb *club$l_csbqfl;
    struct _csb *club$l_csbqbl;
    unsigned short int club$w_size;
    unsigned char club$b_type;
    unsigned char club$b_subtype;
    unsigned int club$l_poll_ctx;
    struct _csb *club$l_local_csb;
    struct _acb *club$l_astqfl;
    struct _acb *club$l_astqbl;
    union
    {
        unsigned int club$l_flags;
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
        unsigned int club$l_lk_flags;
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
    unsigned short int club$w_rseqnum;
    unsigned short int club$w_dirseqnum;
    union
    {
        unsigned char club$b_qstatus;
        struct
        {
            unsigned club$v_no_fquorum : 1;
            unsigned club$v_no_dquorum : 1;
            unsigned club$v_ifw_req : 1;
            unsigned club$v_rns_req : 1;
            unsigned club$v_fill_03 : 4;
        };
    };
    char club$b_fill_2;
    unsigned short int club$w_qdvotes;
    unsigned short int club$w_quorum;
    unsigned short int club$w_votes;
    unsigned short int club$w_cevotes;
    unsigned short int club$w_adj_cevotes;
    unsigned short int club$w_nodes;
    char club$b_fsysid [6];
    long long club$q_ftime;
    unsigned int club$l_lst_xtn;
    unsigned int club$l_lst_coord;
    long long club$q_lst_time;
    unsigned char club$b_lst_code;
    unsigned char club$b_lst_phase;
    unsigned short int club$w_newqdvotes;
    unsigned int club$l_cur_xtn;
    unsigned int club$l_cur_coord;
    long long club$q_cur_time;
    unsigned char club$b_cur_code;
    unsigned char club$b_cur_phase;
    unsigned short int club$w_msgcnt;
    struct _csb *club$l_coord;
    union
    {
        unsigned int club$l_local_csid;
        struct
        {
            unsigned short int club$w_local_csid_idx;
            unsigned short int club$w_local_csid_seq;
        };
    };
    unsigned short int club$w_next_csid;
    unsigned short int club$w_first_index;
    unsigned int club$l_max_xtn;
    unsigned int club$l_retrycnt;
    unsigned int club$l_ctx0;
    unsigned int club$l_ret1;
    unsigned int club$l_ctx1;
    unsigned int club$l_ret2;
    unsigned int club$l_ctx2;
    struct _tqe *club$l_tqe;
    unsigned int club$l_cspipid;
    unsigned long long club$q_newtime;
    unsigned long long club$q_newtime_ref;
    unsigned short int club$w_newquorum;
    unsigned short int club$w_newcevotes;
    unsigned int club$l_fmerit;
    union
    {
        unsigned int club$l_e_memseq;
        struct
        {
            unsigned short int club$w_memseq;
            short int club$w_fill_1;
        };
    };
    unsigned int club$l_random;
    struct _cludcb *club$l_cludcb;
    char club$t_qdname [16];
    struct _cluicb *club$l_cluicb;
    unsigned int club$l_foreign_cluster;
    unsigned int club$l_enbl_verbose;
    unsigned int club$l_qlost_clugen;
    unsigned int club$l_stg_join_clugen;
    unsigned int club$l_join_clugen;
    unsigned short int club$w_stg_join_flags;
    union
    {
        unsigned short int club$w_join_flags;
        struct
        {
            unsigned club$v_clugen_valid : 1;
            unsigned club$v_fill_04 : 15;
        };
    };
    unsigned int club$l_rm_quota;
    unsigned long long club$q_cspq;
    char club$b_fork_block [40];
    char club$b_nodemap [32];
    char club$b_clufcb [120];
    char club$b_hang_fkb [32];
    char club$b_clubpwf [40];
    unsigned int club$l_reslocktmo;
    unsigned int club$l_reslockcsid;
    unsigned int club$l_locktime;
    unsigned short int club$w_merge_cnt;
    unsigned short int club$w_parseqnum;
    unsigned char club$b_newrbld_req;
    unsigned char club$b_rbld_clu;
    unsigned char club$b_rbld_loc;
    char club$b_fill_21;
    struct _clurcb *club$l_clurcbfl;
    struct _clurcb *club$l_clurcbbl;
    unsigned short int club$w_rbld_inhib;
    short int club$w_fill_22;
    unsigned short int club$w_ncnid;
    unsigned short int club$w_newncnid;
    unsigned int club$l_toff;
    unsigned int club$l_ton;
    void *club$l_tbls;
    struct _tqe *club$l_qtqe;
    unsigned int club$l_sync_step;
    unsigned long long club$q_tdf;
    unsigned long long club$q_block_seq;

    char club$b_lckmgr_fork_block [40];
    unsigned int club$l_rmbuf_link;
    unsigned int club$l_max_rmbufs;
    unsigned int club$l_cached_rmbufs;
    unsigned int club$l_tot_rmbufs;
};

#endif



