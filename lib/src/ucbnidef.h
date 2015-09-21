#ifndef UCBNIDEF_H
#define UCBNIDEF_H

#define UCB$C_NI_NUMGSP 4
#define UCB$C_NI_ALLPRM 416

#define UCB$C_NI_EMBED_TABLE_SIZE 88

#define UCB$C_NI_QUE_START 736
#define UCB$C_NI_QUE_NO 2

#define UCB$C_NI_TR_ENTSIZE 16
#define UCB$C_NI_ACTUAL_LENGTH 832

struct _ucbnidef
{
    UINT8 ucbdef$$_ni_ucbfill_1 [248];
    UINT32 ucb$l_ni_pid;
    UINT32 ucb$l_ni_quota;
    UINT32 ucb$l_ni_ast;
    UINT32 ucb$l_ni_stopirp;
    UINT32 ucb$l_ni_resbuf;
    UINT32 ucb$l_ni_vcib;
    UINT32 ucb$l_ni_lsb;
    UINT32 ucb$l_ni_p2param;
    UINT32 ucb$l_ni_mst;
    UINT32 ucb$l_ni_lstrcvtim;
    UINT32 ucb$l_ni_lstxmttim;
    UINT32 ucb$l_ni_lststratt;
    UINT32 ucb$l_ni_lststrcmp;
    UINT32 ucb$l_ni_lststrfld;
    UINT32 ucb$l_ni_lstuubtim;
    UINT32 ucb$l_ni_acc;
    UINT32 ucb$l_ni_bsz;
    UINT32 ucb$l_ni_dch;
    UINT16 ucb$g_ni_des [4];
    UINT32 ucb$l_ni_fmt;
    UINT32 ucb$l_ni_gsp;
    UINT16 ucb$g_ni_hwa [4];
    UINT32 ucb$l_ni_mbs;
    UINT32 ucb$l_ni_med;
    UINT32 ucb$l_ni_mrb;
    long ucb$l_extra_l_1;
    UINT8 ucb$g_ni_pid [8];
    UINT32 ucb$l_ni_prm;
    UINT32 ucb$l_ni_sap;
    UINT32 ucb$l_ni_srv;
    UINT32 ucb$l_ni_apc;
    UINT32 ucb$l_ni_bfn;
    UINT32 ucb$l_ni_cca;
    UINT32 ucb$l_ni_mlt;
    UINT32 ucb$l_ni_pad;
    UINT32 ucb$l_ni_pty;
    UINT32 ucb$l_ni_res;
    union
    {
        UINT32 ucb$l_ni_xfc;
        UINT32 ucb$l_ni_xac;
    };
    union
    {
        UINT32 ucb$l_ni_rfc;
        UINT32 ucb$l_ni_rac;
    };
    UINT32 ucb$l_ni_con;
    UINT32 ucb$l_ni_ilp;
    UINT32 ucb$l_ni_crc;
    INT32 ucb$l_extra_l_2;
    UINT16 ucb$g_ni_pha [4];
    UINT32 ucb$l_ni_famode;
    UINT32 ucb$l_ni_srmode;
    UINT32 ucb$l_ni_hdr;
    UINT32 ucb$l_ni_fc;
    UINT32 ucb$l_ni_ac;
    UINT32 ucb$l_ni_mcena;
    UINT64 ucb$q_ni_mclast;
    UINT8 ucb$t_ni_embed_multi [88];
    INT32 ucb$l_extra_l_3;
    UINT32 ucb$l_ni_source;
    UINT32 ucb$l_ni_sancheck;
    UINT32 ucb$l_ni_saena;
    UINT64 ucb$q_ni_salast;
    UINT64 ucb$q_lan_ocrctr;
    UINT64 ucb$q_lan_morctr;
    UINT64 ucb$q_lan_octctr;
    UINT64 ucb$q_lan_motctr;
    UINT64 ucb$q_lan_pdrctr;
    UINT64 ucb$q_lan_mprctr;
    UINT64 ucb$q_lan_pdtctr;
    UINT64 ucb$q_lan_mptctr;
    UINT64 ucb$q_lan_uubctr;
    UINT64 ucb$q_lan_mnectr;
    UINT64 ucb$q_lan_uubclr;
    UINT64 ucb$q_lan_pdtclr;
    UINT64 ucb$q_lan_octclr;
    UINT64 ucb$q_lan_pdrclr;
    UINT64 ucb$q_lan_ocrclr;
    UINT64 ucb$q_lan_mneclr;
    UINT16 ucb$w_ni_mnectr;
    UINT16 ucb$w_ni_ubuctr;
    UINT32 ucb$l_ni_sblctr;
    UINT32 ucb$l_ni_sbyctr;
    UINT32 ucb$l_ni_rblctr;
    UINT32 ucb$l_ni_rbyctr;
    UINT32 ucb$l_lan_btsctr;
    UINT64 ucb$q_ni_rcvmsg;
    UINT64 ucb$q_ni_rcvreq;
    INT32 ucb$l_lnm_flink;
    INT32 ucb$l_lnm_blink;
    void *ucb$a_lnm_lpb;
    INT32 ucb$l_lnm_cnm_len;
    void *ucb$a_lnm_cnm;
    UINT32 ucb$l_ni_tr_xcuse;
    UINT64 ucb$t_ni_tr_lstmc [4];
    UINT64 ucb$q_ni_tr_xcchk;
    UINT64 ucb$q_ni_tr_xchit;
    UINT32 ucb$l_ni_lsb_size;
    void *ucb$a_ni_unit_init;
};

#define UCB$M_NI_START_BEGUN 0x1
#define UCB$M_LAN_LENGTH 0x2
#define UCB$M_LAN_UNIQID 0x4
#define UCB$M_NI_SHARE 0x8
#define UCB$M_NI_STARTED 0x10
#define UCB$M_NI_RESTART_INUSE 0x20
#define UCB$M_NI_PASS 0x40
#define UCB$M_NI_SHARE_DEF 0x80

struct _ucbnists
{
    union
    {
        UINT32 ucb$l_ni_sts;
        struct
        {
            unsigned ucb$v_ni_start_begun   : 1;
            unsigned ucb$v_lan_length       : 1;
            unsigned ucb$v_lan_uniqid       : 1;
            unsigned ucb$v_ni_share     : 1;
            unsigned ucb$v_ni_started       : 1;
            unsigned ucb$v_ni_restart_inuse : 1;
            unsigned ucb$v_ni_pass      : 1;
            unsigned ucb$v_ni_share_def : 1;
            unsigned ucb$v_ni_filler        : 24;
        };
    };
};

#define UCB$M_NI_PRM_ACC 0x1
#define UCB$M_NI_PRM_APC 0x2
#define UCB$M_NI_PRM_BFN 0x4
#define UCB$M_NI_PRM_BSZ 0x8
#define UCB$M_NI_PRM_BUS 0x10
#define UCB$M_NI_PRM_CCA 0x20
#define UCB$M_NI_PRM_CNM 0x40
#define UCB$M_NI_PRM_CON 0x80
#define UCB$M_NI_PRM_ILP 0x100
#define UCB$M_NI_PRM_CRC 0x200
#define UCB$M_NI_PRM_DCH 0x400
#define UCB$M_NI_PRM_DES 0x800
#define UCB$M_NI_PRM_FMT 0x1000
#define UCB$M_NI_PRM_GSP 0x2000
#define UCB$M_NI_PRM_HWA 0x4000
#define UCB$M_NI_PRM_MCA 0x8000
#define UCB$M_NI_PRM_MED 0x10000
#define UCB$M_NI_PRM_MLT 0x20000
#define UCB$M_NI_PRM_MRB 0x40000
#define UCB$M_NI_PRM_PAD 0x80000
#define UCB$M_NI_PRM_PHA 0x100000
#define UCB$M_NI_PRM_PID 0x200000
#define UCB$M_NI_PRM_PRM 0x400000
#define UCB$M_NI_PRM_PTY 0x800000
#define UCB$M_NI_PRM_RES 0x1000000
#define UCB$M_NI_PRM_SAP 0x2000000
#define UCB$M_NI_PRM_SRV 0x4000000
#define UCB$M_NI_PRM_MBS 0x8000000
#define UCB$M_NI_PRM_RFC 0x10000000
#define UCB$M_NI_PRM_XFC 0x20000000
#define UCB$M_NI_PRM_SRC 0x40000000

struct _ucbniprm
{
    union
    {
        UINT32 ucb$l_ni_prm_str;
        struct
        {
            unsigned ucb$v_ni_prm_acc       : 1;
            unsigned ucb$v_ni_prm_apc       : 1;
            unsigned ucb$v_ni_prm_bfn       : 1;
            unsigned ucb$v_ni_prm_bsz       : 1;
            unsigned ucb$v_ni_prm_bus       : 1;
            unsigned ucb$v_ni_prm_cca       : 1;
            unsigned ucb$v_ni_prm_cnm       : 1;
            unsigned ucb$v_ni_prm_con       : 1;
            unsigned ucb$v_ni_prm_ilp       : 1;
            unsigned ucb$v_ni_prm_crc       : 1;
            unsigned ucb$v_ni_prm_dch       : 1;
            unsigned ucb$v_ni_prm_des       : 1;
            unsigned ucb$v_ni_prm_fmt       : 1;
            unsigned ucb$v_ni_prm_gsp       : 1;
            unsigned ucb$v_ni_prm_hwa       : 1;
            unsigned ucb$v_ni_prm_mca       : 1;
            unsigned ucb$v_ni_prm_med       : 1;
            unsigned ucb$v_ni_prm_mlt       : 1;
            unsigned ucb$v_ni_prm_mrb       : 1;
            unsigned ucb$v_ni_prm_pad       : 1;
            unsigned ucb$v_ni_prm_pha       : 1;
            unsigned ucb$v_ni_prm_pid       : 1;
            unsigned ucb$v_ni_prm_prm       : 1;
            unsigned ucb$v_ni_prm_pty       : 1;
            unsigned ucb$v_ni_prm_res       : 1;
            unsigned ucb$v_ni_prm_sap       : 1;
            unsigned ucb$v_ni_prm_srv       : 1;
            unsigned ucb$v_ni_prm_mbs       : 1;
            unsigned ucb$v_ni_prm_rfc       : 1;
            unsigned ucb$v_ni_prm_xfc       : 1;
            unsigned ucb$v_ni_prm_src       : 1;
            unsigned ucb$v_ni_prm_filler    : 1;
        };
    };
};

#endif

