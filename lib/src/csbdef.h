#ifndef CSBDEF_H
#define CSBDEF_H

#include <fkbdef.h>

#define CSB$M_CAP_VCC 0x2
#define CSB$M_CAP_EXT_STATUS 0x8
#define CSB$M_CAP_CWCREPRC 0x10
#define CSB$M_CAP_THREADS 0x20
#define CSB$M_CAP_CWLOGICALS 0x40
#define CSB$M_CAP_IPC_DEMULT_CONN 0x80
#define CSB$M_CAP_RMBXFR 0x100
#define CSB$M_CAP_WBM_SHADOW 0x200
#define CSB$M_CAP_WBM_ALL 0x400
#define CSB$M_CAP_SCHED_CLASS 0x800
#define CSB$M_LONG_BREAK 0x1
#define CSB$M_MEMBER 0x2
#define CSB$M_REMOVED 0x4
#define CSB$M_QF_SAME 0x8
#define CSB$M_QF_WATCHER 0x10
#define CSB$M_QF_NOACCESS 0x20
#define CSB$M_CLUSTER 0x100
#define CSB$M_QF_ACTIVE 0x200
#define CSB$M_SHUTDOWN 0x400
#define CSB$M_GONE 0x800
#define CSB$M_RESYNCH_FKB_BSY 0x1000
#define CSB$M_RESEND_FKB_BSY 0x2000
#define CSB$M_RCVMSG_FRK_IP 0x4000
#define CSB$M_LOCKED 0x10000
#define CSB$M_SELECTED 0x20000
#define CSB$M_RESERVED1 0x40000
#define CSB$M_SEND_EXT_STATUS 0x80000
#define CSB$M_LOCAL 0x1000000
#define CSB$M_STATUS_RCVD 0x2000000
#define CSB$M_SEND_STATUS 0x4000000
#define CSB$M_QF_RFRSH_RQD 0x8000000
#define CSB$M_QF_RFRSH_IP 0x10000000
#define CSB$M_QF_IOSYNCH 0x20000000
#define CSB$M_ACT_LSHARE 0x40000000
#define CSB$M_PASS_LSHARE 0x80000000
#define CSB$K_OPEN 1
#define CSB$K_STATUS 2
#define CSB$K_RECONNECT 3
#define CSB$K_NEW 4
#define CSB$K_CONNECT 5
#define CSB$K_ACCEPT 6
#define CSB$K_DISCONNECT 7
#define CSB$K_REACCEPT 8
#define CSB$K_WAIT 9
#define CSB$K_DEAD 10
#define CSB$K_LOCAL 11
#define CSB$C_LENGTH 456
#define CSB$K_LENGTH 456
#define CSB$S_CSBDEF 456

struct _csb
{
    void *csb$l_sysqfl;
    void *csb$l_sysqbl;
    UINT16 csb$w_size;
    UINT8 csb$b_type;
    UINT8 csb$b_subtype;
    UINT8 csb$b_cluver;
    UINT8 csb$b_ecolvl;
    UINT8 csb$b_vernum;
    UINT32 csb$l_cache_protocol_ver;
    UINT16 csb$w_lckdirwt;
    UINT16 csb$w_votes;
    UINT16 csb$w_evotes;
    UINT16 csb$w_qdvotes;
    UINT16 csb$w_quorum;
    UINT16 csb$w_nodes;
    INT8 csb$b_cnct [16];
    INT8 csb$b_nodemap [32];
    INT8 csb$b_hwname [61];
    INT8 csb$b_hwname_pad [3];
    char csb$t_sw_version [8];
    UINT64 csb$q_swincarn;
    UINT64 csb$q_reftime;
    UINT64 csb$q_lnm_seqnum;
    unsigned int csb$wil_cnx_sts_r0;
    UINT16 csb$w_cnx_sts_r0;
    unsigned int csb$wil_cnx_sts_r1;
    UINT16 csb$w_cnx_sts_r1;
    struct _fkb csb$r_resend_msgs_fkb;
    struct _fkb csb$r_scs2lckmgr_fkb;
    struct _cdt *csb$l_cdt;
    struct _pdt *csb$l_pdt;
    struct _club *csb$l_club;
    struct _sb *csb$l_sb;
    struct _tqe *csb$l_tqe;
    UINT32 csb$l_timeout;
    UINT32 csb$l_rmax_vctmo;
    INT32 csb$l_abstim_offset;
    UINT32 csb$l_csid;
    UINT16 csb$w_csid_idx;
    UINT16 csb$w_csid_seq;
    unsigned int csb$wil_cnid;
    UINT16 csb$w_cnid;
    INT64 csb$q_partnerq;
    void *csb$l_partnerqfl;
    void *csb$l_partnerqbl;
    INT64 csb$q_warmcdrpq;
    struct _cdrp *csb$l_warmcdrpqfl;
    struct _cdrp *csb$l_warmcdrpqbl;
    UINT64 csb$q_rm_ctxq;
    UINT32 csb$l_rm_ctxqfl;
    UINT32 csb$l_rm_ctxqbl;
    void *csb$l_sentqbl;
    void *csb$l_resendqfl;
    void *csb$l_resendqbl;
    UINT32 csb$l_capability;
    UINT32 csb$l_status;
    UINT32 csb$l_state;
    struct _cdrp *csb$l_currcdrp;
    UINT32 csb$l_refcnt;
    UINT32 csb$l_unackedmsgs;
    UINT32 csb$l_remacklim;
    unsigned int csb$wil_sendseqnm;
    UINT16 csb$w_sendseqnm;
    unsigned int csb$wil_rcvdseqnm;
    UINT16 csb$w_rcvdseqnm;
    unsigned int csb$wil_ackrseqnm;
    UINT16 csb$w_ackrseqnm;
    unsigned int csb$wil_lastsent;
    UINT16 csb$w_lastsent;
    unsigned int csb$wil_pass_cntr;
    UINT16 csb$w_pass_cntr;
    unsigned int csb$wil_act_cntr;
    UINT16 csb$w_act_cntr;
    unsigned int csb$wil_err_cntr;
    UINT16 csb$w_err_cntr;
    UINT32 csb$l_scs2lckmgr_msgcnt;
    INT32 csb$l_scs2lckmgr_msgarray;
    INT64 csb$q_scs2lckmgr_pad;
};

#endif

