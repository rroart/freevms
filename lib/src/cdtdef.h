#ifndef cdtdef_h
#define cdtdef_h

#define CDT$C_CLOSED 0
#define CDT$C_LISTEN 1
#define CDT$C_OPEN 2
#define CDT$C_DISC_ACK 3
#define CDT$C_DISC_REC 4
#define CDT$C_DISC_SENT 5
#define CDT$C_DISC_MTCH 6
#define CDT$C_CON_SENT 7
#define CDT$C_CON_ACK 8
#define CDT$C_CON_REC 9
#define CDT$C_ACCP_SENT 10
#define CDT$C_REJ_SENT 11
#define CDT$C_DISC_MTCH_RSPQ 12
#define CDT$C_DISC_RSPQ 13
#define CDT$C_VC_FAIL 14

#define CDT$C_CON_PEND 1
#define CDT$C_ACCP_PEND 2
#define CDT$C_REJ_PEND 3
#define CDT$C_DISC_PEND 4
#define CDT$C_CR_PEND 5
#define CDT$C_DCR_PEND 6

#define CDT$C_RATING0 0
#define CDT$C_RATING1 1
#define CDT$C_RATING2 2
#define CDT$C_RATING3 3

#define CDT$C_YELLOW 4
#define CDT$C_RATING5 5
#define CDT$C_RED 6
#define CDT$C_UNEQUAL_PATH 7
#define CDT$C_LOAD_SHARE_DISABLE 8

#define CDT$C_BAD_RATING -2147483648
#define CDT$K_BAD_RATING -2147483648
#define CDT$C_LOADSHARE 0
#define CDT$C_PRE_LOADSHARE 1

#define CDT$K_LENGTH 400
#define CDT$C_LENGTH 400
#define CDT$S_CDTDEF 400

struct _cdt
{
    union
    {
        void *cdt$l_msginput;
        struct _cdt *cdt$l_link;
    };
    int (*cdt$l_dginput)(void);
    unsigned short int cdt$w_size;
    unsigned char cdt$b_type;
    unsigned char cdt$b_subtyp;
    int (*cdt$l_erraddr)(void);
    int (*cdt$l_fast_recvmsg_request)(void);
    int (*cdt$l_fast_recvmsg_pm)(void);
    int (*cdt$l_change_affinity)(void);
    struct _pdt *cdt$l_pdt;
    unsigned int cdt$l_rconid;
    unsigned int cdt$l_lconid;
    struct _pb *cdt$l_pb;
    unsigned char cdt$b_rstation [6];
    unsigned short int cdt$w_reason;
    unsigned short int cdt$w_state;
    unsigned short int cdt$w_blkstate;
    void *cdt$l_scsmsg;
    void *cdt$l_waitqfl;
    void *cdt$l_waitqbl;
    void *cdt$l_crwaitqfl;
    void *cdt$l_crwaitqbl;
    unsigned short int cdt$w_send;
    unsigned short int cdt$w_rec;
    unsigned short int cdt$w_minrec;
    unsigned short int cdt$w_pendrec;
    unsigned short int cdt$w_initlrec;
    unsigned short int cdt$w_minsend;
    unsigned short int cdt$w_dgrec;
    unsigned char cdt$b_priority;
    char cdtdef$$_fill_1;
    void *cdt$l_rprocnam;
    void *cdt$l_lprocnam;
    void *cdt$l_condat;
    void *cdt$l_auxstruc;
    int (*cdt$l_badrsp)(void);
    unsigned int cdt$l_fpc;
    unsigned int cdt$l_fr5;
    void *cdt$l_cdtlst;
    unsigned int cdt$l_dgsent;
    unsigned int cdt$l_dgrcvd;
    unsigned int cdt$l_dgdiscard;
    unsigned int cdt$l_msgsent;
    unsigned int cdt$l_msgrcvd;
    unsigned int cdt$l_non_fp_sendmsgs;
    unsigned int cdt$l_non_fp_rcvdmsgs;
    unsigned int cdt$l_snddats;
    unsigned int cdt$l_bytsent;
    unsigned int cdt$l_reqdats;
    unsigned int cdt$l_bytreqd;
    unsigned int cdt$l_bytmapd;
    unsigned short int cdt$w_qcr_cnt;
    unsigned short int cdt$w_qbdlt_cnt;
    void *cdt$l_move_path_addr;
    void *cdt$l_share_flink;
    void *cdt$l_share_blink;
    struct _sb *cdt$l_sb;
    unsigned int cdt$l_con_req_ctr;
    unsigned int cdt$l_load_rating;
    unsigned int cdt$l_time_stamp;
    unsigned int cdt$l_queue_time_stamp;
    unsigned int cdt$l_discon_counter;
    void *cdt$l_optimal_path;
    unsigned int cdt$l_bytes_xfer;
    unsigned int cdt$l_bytes_dg_xmt;
    unsigned int cdt$l_bytes_dg_rcv;
    unsigned int cdt$l_bytes_msg_xmt;
    unsigned int cdt$l_bytes_msg_rcv;
    unsigned int cdt$l_bytes_xfer_last;
    unsigned int cdt$l_bytes_dg_xmt_last;
    unsigned int cdt$l_bytes_dg_rcv_last;
    unsigned int cdt$l_bytes_msg_xmt_last;
    unsigned int cdt$l_bytes_msg_rcv_last;
    unsigned int cdt$l_bytmapd_last;
    unsigned int cdt$l_dgsent_last;
    unsigned int cdt$l_dgrcvd_last;
    unsigned int cdt$l_msgsent_last;
    unsigned int cdt$l_msgrcvd_last;
    unsigned int cdt$l_bytes_xfer_peak;
    unsigned int cdt$l_bytes_dg_xmt_peak;
    unsigned int cdt$l_bytes_dg_rcv_peak;
    unsigned int cdt$l_bytes_msg_xmt_peak;
    unsigned int cdt$l_bytes_msg_rcv_peak;
    unsigned int cdt$l_bytmapd_peak;
    unsigned int cdt$l_dgsent_peak;
    unsigned int cdt$l_dgrcvd_peak;
    unsigned int cdt$l_msgsent_peak;
    unsigned int cdt$l_msgrcvd_peak;
    unsigned int cdt$l_bytes_xfer_avg;
    unsigned int cdt$l_bytes_dg_xmt_avg;
    unsigned int cdt$l_bytes_dg_rcv_avg;
    unsigned int cdt$l_bytes_msg_xmt_avg;
    unsigned int cdt$l_bytes_msg_rcv_avg;
    unsigned int cdt$l_bytmapd_avg;
    unsigned int cdt$l_dgsent_avg;
    unsigned int cdt$l_dgrcvd_avg;
    unsigned int cdt$l_msgsent_avg;
    unsigned int cdt$l_msgrcvd_avg;
    unsigned int cdt$l_bytes_xfer_int;
    unsigned short int cdt$w_local_index;
    unsigned char cdt$b_ls_flag;
    unsigned char cdt$b_sysap_version;
    char cdt$b_quad_fill1 [4];
    unsigned char cdt$b_scs_maint_block [16];
    unsigned int cdt$l_fp_scs_nosend;
    unsigned int cdt$l_fp_scs_norecv;
    unsigned int cdt$l_reserved3;
    unsigned int cdt$l_reserved4;
};

#endif

