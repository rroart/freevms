#ifndef CDTDEF_H
#define CDTDEF_H

#include <vms_types.h>

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
    INT32 (*cdt$l_dginput)(void);
    UINT8 cdt$w_size;
    UINT8 cdt$b_type;
    UINT8 cdt$b_subtyp;
    INT32 (*cdt$l_erraddr)(void);
    INT32 (*cdt$l_fast_recvmsg_request)(void);
    INT32 (*cdt$l_fast_recvmsg_pm)(void);
    INT32 (*cdt$l_change_affinity)(void);
    struct _pdt *cdt$l_pdt;
    UINT32 cdt$l_rconid;
    UINT32 cdt$l_lconid;
    struct _pb *cdt$l_pb;
    UINT8 cdt$b_rstation [6];
    UINT8 cdt$w_reason;
    UINT8 cdt$w_state;
    UINT8 cdt$w_blkstate;
    void *cdt$l_scsmsg;
    void *cdt$l_waitqfl;
    void *cdt$l_waitqbl;
    void *cdt$l_crwaitqfl;
    void *cdt$l_crwaitqbl;
    UINT8 cdt$w_send;
    UINT8 cdt$w_rec;
    UINT8 cdt$w_minrec;
    UINT8 cdt$w_pendrec;
    UINT8 cdt$w_initlrec;
    UINT8 cdt$w_minsend;
    UINT8 cdt$w_dgrec;
    UINT8 cdt$b_priority;
    INT8 cdtdef$$_fill_1;
    void *cdt$l_rprocnam;
    void *cdt$l_lprocnam;
    void *cdt$l_condat;
    void *cdt$l_auxstruc;
    INT32 (*cdt$l_badrsp)(void);
    UINT32 cdt$l_fpc;
    UINT32 cdt$l_fr5;
    void *cdt$l_cdtlst;
    UINT32 cdt$l_dgsent;
    UINT32 cdt$l_dgrcvd;
    UINT32 cdt$l_dgdiscard;
    UINT32 cdt$l_msgsent;
    UINT32 cdt$l_msgrcvd;
    UINT32 cdt$l_non_fp_sendmsgs;
    UINT32 cdt$l_non_fp_rcvdmsgs;
    UINT32 cdt$l_snddats;
    UINT32 cdt$l_bytsent;
    UINT32 cdt$l_reqdats;
    UINT32 cdt$l_bytreqd;
    UINT32 cdt$l_bytmapd;
    UINT8 cdt$w_qcr_cnt;
    UINT8 cdt$w_qbdlt_cnt;
    void *cdt$l_move_path_addr;
    void *cdt$l_share_flink;
    void *cdt$l_share_blink;
    struct _sb *cdt$l_sb;
    UINT32 cdt$l_con_req_ctr;
    UINT32 cdt$l_load_rating;
    UINT32 cdt$l_time_stamp;
    UINT32 cdt$l_queue_time_stamp;
    UINT32 cdt$l_discon_counter;
    void *cdt$l_optimal_path;
    UINT32 cdt$l_bytes_xfer;
    UINT32 cdt$l_bytes_dg_xmt;
    UINT32 cdt$l_bytes_dg_rcv;
    UINT32 cdt$l_bytes_msg_xmt;
    UINT32 cdt$l_bytes_msg_rcv;
    UINT32 cdt$l_bytes_xfer_last;
    UINT32 cdt$l_bytes_dg_xmt_last;
    UINT32 cdt$l_bytes_dg_rcv_last;
    UINT32 cdt$l_bytes_msg_xmt_last;
    UINT32 cdt$l_bytes_msg_rcv_last;
    UINT32 cdt$l_bytmapd_last;
    UINT32 cdt$l_dgsent_last;
    UINT32 cdt$l_dgrcvd_last;
    UINT32 cdt$l_msgsent_last;
    UINT32 cdt$l_msgrcvd_last;
    UINT32 cdt$l_bytes_xfer_peak;
    UINT32 cdt$l_bytes_dg_xmt_peak;
    UINT32 cdt$l_bytes_dg_rcv_peak;
    UINT32 cdt$l_bytes_msg_xmt_peak;
    UINT32 cdt$l_bytes_msg_rcv_peak;
    UINT32 cdt$l_bytmapd_peak;
    UINT32 cdt$l_dgsent_peak;
    UINT32 cdt$l_dgrcvd_peak;
    UINT32 cdt$l_msgsent_peak;
    UINT32 cdt$l_msgrcvd_peak;
    UINT32 cdt$l_bytes_xfer_avg;
    UINT32 cdt$l_bytes_dg_xmt_avg;
    UINT32 cdt$l_bytes_dg_rcv_avg;
    UINT32 cdt$l_bytes_msg_xmt_avg;
    UINT32 cdt$l_bytes_msg_rcv_avg;
    UINT32 cdt$l_bytmapd_avg;
    UINT32 cdt$l_dgsent_avg;
    UINT32 cdt$l_dgrcvd_avg;
    UINT32 cdt$l_msgsent_avg;
    UINT32 cdt$l_msgrcvd_avg;
    UINT32 cdt$l_bytes_xfer_int;
    UINT8 cdt$w_local_index;
    UINT8 cdt$b_ls_flag;
    UINT8 cdt$b_sysap_version;
    UINT8 cdt$b_quad_fill1 [4];
    UINT8 cdt$b_scs_maint_block [16];
    UINT32 cdt$l_fp_scs_nosend;
    UINT32 cdt$l_fp_scs_norecv;
    UINT32 cdt$l_reserved3;
    UINT32 cdt$l_reserved4;
};

#endif

