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
    char ucbdef$$_ni_ucbfill_1 [248];
    unsigned int ucb$l_ni_pid;
    unsigned int ucb$l_ni_quota;
    unsigned int ucb$l_ni_ast;
    unsigned int ucb$l_ni_stopirp;
    unsigned int ucb$l_ni_resbuf;
    unsigned int ucb$l_ni_vcib;
    unsigned int ucb$l_ni_lsb;
    unsigned int ucb$l_ni_p2param;
    unsigned int ucb$l_ni_mst;
    unsigned int ucb$l_ni_lstrcvtim;
    unsigned int ucb$l_ni_lstxmttim;
    unsigned int ucb$l_ni_lststratt;
    unsigned int ucb$l_ni_lststrcmp;
    unsigned int ucb$l_ni_lststrfld;
    unsigned int ucb$l_ni_lstuubtim;
    unsigned int ucb$l_ni_acc;
    unsigned int ucb$l_ni_bsz;
    unsigned int ucb$l_ni_dch;
    unsigned short int ucb$g_ni_des [4];
    unsigned int ucb$l_ni_fmt;
    unsigned int ucb$l_ni_gsp;
    unsigned short int ucb$g_ni_hwa [4];
    unsigned int ucb$l_ni_mbs;
    unsigned int ucb$l_ni_med;
    unsigned int ucb$l_ni_mrb;
    long ucb$l_extra_l_1;
    unsigned char ucb$g_ni_pid [8];
    unsigned int ucb$l_ni_prm;
    unsigned int ucb$l_ni_sap;
    unsigned int ucb$l_ni_srv;
    unsigned int ucb$l_ni_apc;
    unsigned int ucb$l_ni_bfn;
    unsigned int ucb$l_ni_cca;
    unsigned int ucb$l_ni_mlt;
    unsigned int ucb$l_ni_pad;
    unsigned int ucb$l_ni_pty;
    unsigned int ucb$l_ni_res;
    union
    {
        unsigned int ucb$l_ni_xfc;
        unsigned int ucb$l_ni_xac;
    };
    union
    {
        unsigned int ucb$l_ni_rfc;
        unsigned int ucb$l_ni_rac;
    };
    unsigned int ucb$l_ni_con;
    unsigned int ucb$l_ni_ilp;
    unsigned int ucb$l_ni_crc;
    int ucb$l_extra_l_2;
    unsigned short int ucb$g_ni_pha [4];
    unsigned int ucb$l_ni_famode;
    unsigned int ucb$l_ni_srmode;
    unsigned int ucb$l_ni_hdr;
    unsigned int ucb$l_ni_fc;
    unsigned int ucb$l_ni_ac;
    unsigned int ucb$l_ni_mcena;
    unsigned long long ucb$q_ni_mclast;
    unsigned char ucb$t_ni_embed_multi [88];
    int ucb$l_extra_l_3;
    unsigned int ucb$l_ni_source;
    unsigned int ucb$l_ni_sancheck;
    unsigned int ucb$l_ni_saena;
    unsigned long long ucb$q_ni_salast;
    unsigned long long ucb$q_lan_ocrctr;
    unsigned long long ucb$q_lan_morctr;
    unsigned long long ucb$q_lan_octctr;
    unsigned long long ucb$q_lan_motctr;
    unsigned long long ucb$q_lan_pdrctr;
    unsigned long long ucb$q_lan_mprctr;
    unsigned long long ucb$q_lan_pdtctr;
    unsigned long long ucb$q_lan_mptctr;
    unsigned long long ucb$q_lan_uubctr;
    unsigned long long ucb$q_lan_mnectr;
    unsigned long long ucb$q_lan_uubclr;
    unsigned long long ucb$q_lan_pdtclr;
    unsigned long long ucb$q_lan_octclr;
    unsigned long long ucb$q_lan_pdrclr;
    unsigned long long ucb$q_lan_ocrclr;
    unsigned long long ucb$q_lan_mneclr;
    unsigned short int ucb$w_ni_mnectr;
    unsigned short int ucb$w_ni_ubuctr;
    unsigned int ucb$l_ni_sblctr;
    unsigned int ucb$l_ni_sbyctr;
    unsigned int ucb$l_ni_rblctr;
    unsigned int ucb$l_ni_rbyctr;
    unsigned int ucb$l_lan_btsctr;
    unsigned long long ucb$q_ni_rcvmsg;
    unsigned long long ucb$q_ni_rcvreq;
    int ucb$l_lnm_flink;
    int ucb$l_lnm_blink;
    void *ucb$a_lnm_lpb;
    int ucb$l_lnm_cnm_len;
    void *ucb$a_lnm_cnm;
    unsigned int ucb$l_ni_tr_xcuse;
    unsigned long long ucb$t_ni_tr_lstmc [4];
    unsigned long long ucb$q_ni_tr_xcchk;
    unsigned long long ucb$q_ni_tr_xchit;
    unsigned int ucb$l_ni_lsb_size;
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
        unsigned int ucb$l_ni_sts;
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
        unsigned int ucb$l_ni_prm_str;
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

