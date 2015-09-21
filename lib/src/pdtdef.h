#ifndef PDTDEF_H
#define PDTDEF_H

#include <vms_types.h>

#define PDT$M_SNGLHOST 0x1
#define PDT$C_PA 1
#define PDT$C_PU 2
#define PDT$C_PE 3
#define PDT$C_PS 4

#define PDT$C_PI 6
#define PDT$C_PL 7
#define PDT$C_PW 8
#define PDT$C_PN 9
#define PDT$C_PC 10
#define PDT$C_PB 11
#define PDT$C_PM 12
#define PDT$M_CNTBSY 0x1
#define PDT$M_CNTRLS 0x2
#define PDT$M_XCNTRS 0x4
#define PDT$M_NON_CI_BHANDLE 0x8
#define PDT$M_AFFINITY 0x10
#define PDT$C_BASEVER 0
#define PDT$C_LISTENVER 1
#define PDT$C_BALANCEVER 2
#define PDT$C_REORGVER 1
#define PDT$C_CREDITVER 2
#define PDT$C_NI_CLASS 10
#define PDT$C_DSSI_MEDIUM_CLASS 48
#define PDT$C_CI_CLASS 140
#define PDT$C_MC_CLASS 800
#define PDT$C_SM_CLASS 32767

#define PDT$C_YELLOW 4
#define PDT$C_RED 6
#define PDT$C_UNEQUAL_PATH 7
#define PDT$C_CTRSTART 636
#define PDT$C_STD_CTREND 668
#define PDT$C_STDNO_CTR 7
#define PDT$C_EXT_CTRSTART 668
#define PDT$C_EXT_CTREND 776
#define PDT$C_EXTNO_CTR 26
#define SCS$C_EB_MAX_INDEX 9
#define PDT$K_LENGTH 896
#define PDT$C_LENGTH 896

#define PDT$S_PDTDEF 896

struct _pdt
{
    struct _pdt *pdt$l_flink;
    UINT16 pdt$w_portchar;
    INT8 pdtdef$$_fill_2;
    UINT8 pdt$b_pdt_type;
    UINT16 pdt$w_size;
    UINT8 pdt$b_type;
    UINT8 pdt$b_subtyp;
    INT32 (*pdtvec$l_allocdg)(void);
    INT32 (*pdtvec$l_allocmsg)(void);
    INT32 (*pdtvec$l_deallocdg)(void);
    INT32 (*pdtvec$l_deallomsg)(void);
    INT32 (*pdtvec$l_ins_pes_mfreeq)(void);
    INT32 (*pdtvec$l_ins_opt_mfreeq)(void);
    INT32 (*pdtvec$l_rem_opt_mfreeq)(void);
    INT32 (*pdtvec$l_rem_pes_mfreeq)(void);
    INT32 (*pdtvec$l_add_free_dg)(void);
    INT32 (*pdtvec$l_rem_free_dg)(void);
    INT32 (*pdtvec$l_queue_dg)(void);
    INT32 (*pdtvec$l_reqdata)(void);
    INT32 (*pdtvec$l_senddata)(void);
    INT32 (*pdtvec$l_senddatawmsg)(void);
    INT32 (*pdtvec$l_senddg)(int disposition_flag, int dg_msg_length, struct _cdrp *cdrp);
    INT32 (*pdtvec$l_sendmsg)(void);
    INT32 (*pdtvec$l_sendmsgl)(void);
    INT32 (*pdtvec$l_trnmsgh)(void);
    INT32 (*pdtvec$l_trnmsgl)(void);
    INT32 (*pdtvec$l_readcount)(void);
    INT32 (*pdtvec$l_rlscount)(void);
    INT32 (*pdtvec$l_mreset)(void);
    INT32 (*pdtvec$l_mstart)(void);
    INT32 (*pdtvec$l_stop_vcs)(void);
    INT32 (*pdtvec$l_shut_all_vc)(void);
    INT32 (*pdtvec$l_crash_vc)(void);
    INT32 (*pdtvec$l_crash_port)(void);
    INT32 (*pdtvec$l_reinit_port)(void);
    INT32 (*pdtvec$l_flush_vc)(void);
    INT32 (*pdtvec$l_log_error_scs)(void);
    INT32 (*pdtvec$l_cleanup_pkt)(void);
    INT32 (*pdtvec$l_pb_from_msg)(void);
    INT32 (*pdtvec$l_chk_lost_ack)(void);
    INT32 (*pdtvec$l_remove_pb)(void);
    INT32 (*pdt$l_connect)(void);
    INT32 (*pdt$l_dconnect)(void);
    INT32 (*pdt$l_add_scs_hdr)(void);
    INT32 (*pdt$l_cancel_wait)(void);
    INT32 (*pdt$l_map)(void);
    INT32 (*pdt$l_unmap)(void);
    INT32 (*pdt$l_fast_sendmsg_request)(void);
    INT32 (*pdt$l_fast_sendmsg_ass_res_pm)(void);
    INT32 (*pdt$l_fast_sendmsg_pm)(void);
    INT32 (*pdt$l_alloc_rbun)(void);
    INT32 (*pdt$l_dealloc_rbun)(void);
    INT32 (*pdt$l_fast_recvmsg_chk_res)(void);
    INT32 (*pdt$l_test_crash_port)(void);
    INT32 (*pdt$l_test_ins_comqh)(void);
    INT32 (*pdt$l_test_1_port)(void);
    INT32 (*pdt$l_test_2_port)(void);
    INT32 (*pdtdef$$_fill_4 [7])(void);
    struct _fkb *pdt$l_waitqfl;
    struct _fkb *pdt$l_waitqbl;
    void *pdt$l_pm_portlock;
    struct _rbun *pdt$l_rbun_list;
    UINT32 pdt$l_rbun_length;
    UINT32 pdt$l_rbun_pooltype;
    UINT32 pdt$l_non_fp_sendmsgs;
    union
    {
        UINT32 pdt$l_dghdrsz;
        UINT32 pdt$l_msghdrsz;
    } pdthdrsz;
    UINT32 pdt$l_dgovrhd;
    UINT32 pdt$l_maxbcnt;
    UINT16 pdt$w_flags;
    INT16 pdtdef$$_fill_5;
    char pdt$t_cntowner [16];
    struct _cdrp *pdt$l_cntcdrp;
    UINT32 pdt$l_pollsweep;
    struct _ucb *pdt$l_ucb0;
    struct _adp *pdt$l_adp;
    UINT32 pdt$l_max_vctmo;
    UINT16 pdt$w_scsversion;
    UINT16 pdt$w_ppdversion;
    INT32 (*pdt$l_load_vector)(void);
    UINT16 pdt$w_load_class;
    INT16 pdtdef$$_fill_6;
    UINT64 pdt$q_pb;
    UINT64 pdt$q_conn_wait;
    UINT64 pdt$q_yellow;
    UINT64 pdt$q_red;
    UINT64 pdt$q_disabled;
    UINT32 pdt$l_port_map;
    INT32 pdt$l_avail_thruput;
    UINT32 pdt$l_load_rating;
    UINT32 pdt$l_time_stamp;
    UINT32 pdt$l_saturation_pt;
    UINT32 pdt$l_max_thruput_threshold;
    UINT32 pdt$l_min_thruput_threshold;
    UINT32 pdt$l_tolerance_threshold;
    UINT32 pdt$l_bytes_dg_xmt;
    UINT32 pdt$l_bytes_dg_rcv;
    UINT32 pdt$l_bytes_msg_xmt;
    UINT32 pdt$l_bytes_msg_rcv;
    UINT32 pdt$l_bytes_mapped;
    UINT32 pdt$l_dg_xmt;
    UINT32 pdt$l_dg_rcv;
    UINT32 pdt$l_msg_xmt;
    UINT32 pdt$l_msg_rcv;
    UINT64 pdt$q_bytes_xfer;
    UINT32 pdt$l_num_map;
    UINT32 pdt$l_port_cmd;
    INT8 pdtdef$$_fill_55 [4];
    UINT32 pdt$l_bytes_dg_xmt_last;
    UINT32 pdt$l_bytes_dg_rcv_last;
    UINT32 pdt$l_bytes_msg_xmt_last;
    UINT32 pdt$l_bytes_msg_rcv_last;
    UINT32 pdt$l_bytes_mapped_last;
    UINT32 pdt$l_dg_xmt_last;
    UINT32 pdt$l_dg_rcv_last;
    UINT32 pdt$l_msg_xmt_last;
    UINT32 pdt$l_msg_rcv_last;
    UINT64 pdt$q_bytes_xfer_last;
    UINT32 pdt$l_num_map_last;
    UINT32 pdt$l_port_cmd_last;
    INT32 pdtdef$$_fill_66;
    UINT32 pdt$l_bytes_xfer_int;
    UINT32 pdt$l_equal_path_call_count;
    UINT32 pdt$l_unequal_path_call_count;
    UINT32 pdt$l_connection_move_count;
    UINT32 pdt$l_bytes_dg_xmt_peak;
    UINT32 pdt$l_bytes_dg_rcv_peak;
    UINT32 pdt$l_bytes_msg_xmt_peak;
    UINT32 pdt$l_bytes_msg_rcv_peak;
    UINT32 pdt$l_bytes_mapped_peak;
    UINT32 pdt$l_dg_xmt_peak;
    UINT32 pdt$l_dg_rcv_peak;
    UINT32 pdt$l_msg_xmt_peak;
    UINT32 pdt$l_msg_rcv_peak;
    UINT64 pdt$q_bytes_xfer_peak;
    UINT32 pdt$l_port_cmd_peak;
    UINT32 pdt$l_bytes_dg_xmt_avg;
    UINT32 pdt$l_bytes_dg_rcv_avg;
    UINT32 pdt$l_bytes_msg_xmt_avg;
    UINT32 pdt$l_bytes_msg_rcv_avg;
    UINT32 pdt$l_bytes_mapped_avg;
    UINT32 pdt$l_dg_xmt_avg;
    UINT32 pdt$l_dg_rcv_avg;
    UINT32 pdt$l_msg_xmt_avg;
    UINT32 pdt$l_msg_rcv_avg;
    UINT64 pdt$q_bytes_xfer_avg;
    UINT32 pdt$l_port_cmd_avg;
    UINT8 pdt$b_ls_flag;
    INT8 pdt$t_fill_0 [3];
    UINT32 pdt$l_stdno_ctr;
    UINT32 pdt$l_path0_ack;
    UINT32 pdt$l_path0_nak;
    UINT32 pdt$l_path0_nrsp;
    UINT32 pdt$l_path1_ack;
    UINT32 pdt$l_path1_nak;
    UINT32 pdt$l_path1_nrsp;
    UINT32 pdt$l_dg_disc;
    UINT32 pdt$l_extno_ctr;
    UINT32 pdt$l_spare1_cnt;
    UINT32 pdt$l_spare2_cnt;
    UINT32 pdt$l_spare3_cnt;
    UINT32 pdt$l_spare4_cnt;
    union
    {
        struct
        {
            UINT32 pdt$l_snddat_oper_snt;
            UINT32 pdt$l_snddat_data_snt;
            UINT32 pdt$l_snddat_bodies_snt;
            UINT32 pdt$l_reqdat_oper_snt;
            UINT32 pdt$l_retdat_data_rcv;
            UINT32 pdt$l_retdat_bodies_rcv;
            UINT32 pdt$l_sntdat_bodies_rec;
            UINT32 pdt$l_sntdat_data_rec;
            UINT32 pdt$l_cnf_snt;
            UINT32 pdt$l_datreq_bodies_rcv;
            UINT32 pdt$l_retdat_bodies_snt;
            UINT32 pdt$l_retdat_data_snt;
        } pdt1;
        struct
        {
            UINT32 pdt$l_np_sntdat_bodies_snt;
            UINT32 pdt$l_np_sntdat_data_snt;
            UINT32 pdt$l_np_cnf_bodies_rcv;
            UINT32 pdt$l_np_reqdat_oper_cmp;
            UINT32 pdt$l_np_retdat_bodies_rcv;
            UINT32 pdt$l_np_retdat_data_rcv;
            UINT32 pdt$l_np_sntdat_bodies_rcv;
            UINT32 pdt$l_np_sntdat_data_rcv;
            UINT32 pdt$l_np_cnf_bodies_snt;
            UINT32 pdt$l_np_reqdat_bodies_rcv;
            UINT32 pdt$l_np_retdat_bodies_snt;
            UINT32 pdt$l_np_retdat_data_snt;
        } pdt2;
    } pdtx;
    UINT32 pdt$l_dgsnt;
    UINT32 pdt$l_dg_txt_snt;
    UINT32 pdt$l_msg_snt;
    UINT32 pdt$l_msg_txt_snt;
    UINT32 pdt$l_misc_snt;
    UINT32 pdt$l_dg_rec;
    UINT32 pdt$l_dg_txtrec;
    UINT32 pdt$l_msg_rec;
    UINT32 pdt$l_msg_txt_rec;
    UINT32 pdt$l_misc_rec;
    UINT32 pdt$l_snddat_data_snt_last;
    UINT32 pdt$l_snddat_oper_snt_last;
    UINT32 pdt$l_retdat_data_rcv_last;
    UINT32 pdt$l_reqdat_oper_snt_last;
    UINT32 pdt$l_cnf_snt_last;
    UINT32 pdt$l_sntdat_data_rec_last;
    UINT32 pdt$l_datreq_bodies_rcv_last;
    UINT32 pdt$l_retdat_data_snt_last;
    UINT32 pdt$l_avg_xfer_size;
    UINT32 pdt$l_eb_table;
    INT32 pdtdef$$_fill_77;
    void *pdt$q_formpb [2];
    UINT16 pdt$w_pbcount;
    INT16 pdtdef$$_fill_7;
    union
    {
        UINT8 pdt$b_port_num;
        unsigned char pdt$t_port_num [6];
    } pdtport;
    union
    {
        UINT8 pdt$b_max_port;
        unsigned char pdt$t_max_port [6];
    } pdtmaxport;
    UINT32 pdt$l_curcnt;
    UINT32 pdt$l_pooldue;
    void *pdt$l_bdlt;
    UINT8 pdt$b_scs_maint_block [16];
    UINT32 pdt$l_tqeaddr;
    UINT32 pdt$l_timvcfail;
    union
    {
        UINT64 pdt$q_mgt_handles;
        struct
        {
            UINT32 pdt$l_mgt_handle;
            UINT32 pdt$l_mgt_mgr_handle;
        };
    };
    INT32 pdt$l_mgt_priority;
    INT8 pdt$b_szalign1 [4];
    UINT64 pdt$q_comql;
    UINT64 pdt$q_comqh;
    UINT64 pdt$q_comq2;
    UINT64 pdt$q_comq3;
    UINT64 pdt$q_rspq;
    void *pdt$l_dfqhdr;
    void *pdt$l_mfqhdr;
};

#define PDT$M_CUR_LBS 0x1
#define PDT$M_PRV_LBS 0x2
#define PDT$M_X_LBS 0x4

struct _lbsts
{
    union
    {
        struct
        {
            unsigned pdt$v_cur_lbs : 1;
            unsigned pdt$v_prv_lbs : 1;
            unsigned pdt$v_x_lbs : 1;
            unsigned pdt$v_fill_19_ : 5;
        };
        UINT8 pdt$b_lbsts;
    };
};

#endif

