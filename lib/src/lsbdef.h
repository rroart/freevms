#ifndef LSBDEF_H
#define LSBDEF_H

#include <vms_types.h>

#define     LSB$M_CREATED       0x1
#define     LSB$M_ENABLED       0x2
#define     LSB$M_BENABLED      0x4
#define     LSB$M_NMDIS     0x8
#define     LSB$M_ELAN      0x10
#define     LSB$M_CLIP      0x20
#define     LSB$M_NM_MAC_ADDR   0x1
#define     LSB$C_TIMUSB        60

#define     LSB$C_MIN_RCV_MIN   32
#define     LSB$C_DEF_RCV_MIN   64
#define     LSB$C_MAX_RCV_MIN   512
#define     LSB$C_MIN_RCV_MAX   64
#define     LSB$C_DEF_RCV_MAX   128
#define     LSB$C_MAX_RCV_MAX   768
#define     LSB$C_RCV_INC       4
#define     LSB$C_RCV_DEC       1
#define     LSB$C_RCV_ADD       32

#define     LSB$C_QUEUES        936
#define     LSB$C_ABORT_QS      1

#define     LSB$C_QS_CLEAN_ACT  7

#define     LSB$C_QS_CLEAN_INACT    8

#define     LSB$C_INIT_QS       10

#define     LSB$G_V4_CTRSECT    1072
#define     LSB$G_END_V4_CTRSECT    1256

#define     LSB$G_PIVCLR        1616
#define     LSB$C_PIVSIZ        552

#define     LSB$G_CTRSECT       2170
#define     LSB$M_SFL_EXC       0x1
#define     LSB$M_SFL_CCF       0x2
#define     LSB$M_SFL_SHC       0x4
#define     LSB$M_SFL_OPC       0x8
#define     LSB$M_SFL_FTL       0x10
#define     LSB$M_SFL_RFD       0x20
#define     LSB$M_SFL_TUR       0x40
#define     LSB$M_SFL_TFL       0x80
#define     LSB$M_RFL_BCE       0x1
#define     LSB$M_RFL_FME       0x2
#define     LSB$M_RFL_FTL       0x4
#define     LSB$M_RFL_RSE       0x8
#define     LSB$M_RFL_RLE       0x10
#define     LSB$G_END_CTRSECT   2224
#define     LSB$M_TRSFL_TUR     0x1
#define     LSB$M_TRSFL_LE      0x2
#define     LSB$M_TRSFL_ABS     0x4
#define     LSB$M_TRSFL_LF      0x8
#define     LSB$M_TRSFL_TE      0x10
#define     LSB$M_TRSFL_UF      0x20
#define     LSB$M_TRSFL_RC      0x40
#define     LSB$M_TRRFL_RC      0x1
#define     LSB$M_TRRFL_FCE     0x2
#define     LSB$M_TRIFL_LWF     0x1
#define     LSB$M_TRIFL_SLE     0x2
#define     LSB$M_TRIFL_TMO     0x4
#define     LSB$M_TRIFL_RPT     0x8
#define     LSB$M_TRIFL_BCN     0x10
#define     LSB$M_TRIFL_DAD     0x20
#define     LSB$M_TRIFL_PSF     0x40
#define     LSB$M_TRIFL_RR      0x80
#define     LSB$M_TRTFL_LWF     0x1
#define     LSB$M_TRTFL_SSD     0x2
#define     LSB$M_TRTFL_ARF     0x4
#define     LSB$M_TRTFL_RR      0x8
#define     LSB$C_CTRSIZE       262

#define     LSB$M_DEVCTR        0x1
#define     LSB$M_DEVXIDTEST    0x2
#define     LSB$M_MULSEG        0x4
#define     LSB$M_RCVMGT        0x8
#define     LSB$M_RUN       0x1
#define     LSB$M_INITED        0x2
#define     LSB$M_MCA_OVF       0x4
#define     LSB$M_FAT_ERR       0x8
#define     LSB$M_BLKCTL        0x10
#define     LSB$M_PCHACT        0x20
#define     LSB$M_NEED_DAT_LOCK 0x40
#define     LSB$M_GOT_DAT_LOCK  0x80
#define     LSB$M_RING_AVAIL    0x100
#define     LSB$M_FULL_DUPLEX   0x200
#define     LSB$C_ALLPRM        2464
#define     LSB$C_ALLPRMSIZ     12

#define     LSB$C_CPYPRMSIZ     24

#define     LSB$C_MBX_MAX       12
#define     LSB$C_FIXED_LENGTH      2880

#define     LSB$M_CSMACD_ELAN_ACTIVE    0x1
#define     LSB$M_CSMACD_ELAN_START     0x2
#define     LSB$M_CSMACD_ELAN_SHUT      0x4
#define     LSB$M_CSMACD_ELAN_UNAVAILABLE   0x8
#define     LSB$M_CSMACD_LIS_ACTIVE     0x1
#define     LSB$M_CSMACD_LIS_START      0x2
#define     LSB$M_CSMACD_LIS_SHUT       0x4
#define     LSB$M_CSMACD_LIS_UNAVAILABLE    0x8
#define     LSB$C_CSMACD_LENGTH     3328
#define     LSB$M_FDDI_BOO          0x1
#define     LSB$M_FDDI_CTY          0x2
#define     LSB$M_FDDI_UNTO         0x4
#define     LSB$C_FDDI_LENGTH       3040

#define     LSB$C_FDDI_OFF_INIT     0
#define     LSB$C_FDDI_OFF_READY        1
#define     LSB$C_FDDI_OFF_FAULT_REC    2
#define     LSB$C_FDDI_ON_RING_INIT     3
#define     LSB$C_FDDI_ON_RING_RUN      4
#define     LSB$C_FDDI_BROKEN       5

#define     LSB$C_FDDI_OFF          0
#define     LSB$C_FDDI_CAND_PURGER      1
#define     LSB$C_FDDI_NON_PURGER       2
#define     LSB$C_FDDI_PURGER       3

#define     LSB$C_FDDI_UNK      0
#define     LSB$C_FDDI_SUC      1
#define     LSB$C_FDDI_FAIL     2

#define     LSB$C_FDDI_INT_LOOP     0
#define     LSB$C_FDDI_PHY_BROKEN       1
#define     LSB$C_FDDI_PHY_OFF_READY    2
#define     LSB$C_FDDI_WAITING      3
#define     LSB$C_FDDI_STARTING     4
#define     LSB$C_FDDI_FAILED       5
#define     LSB$C_FDDI_WATCH        6
#define     LSB$C_FDDI_IN_USE       7

#define     LSB$C_FDDI_T_REQ_DFLT       100000
#define     LSB$C_FDDI_TVX_DFLT     32768
#define     LSB$C_FDDI_RTTO_DFLT        12500000
#define     LSB$C_MAX_XPLOR_BUF     6
#define     LSB$C_TR_LENGTH         3008
#define     LSB$C_TR_SPEED_4        0
#define     LSB$C_TR_SPEED_16       1
#define     LSB$C_TR_LNK_OFF        1
#define     LSB$C_TR_LNK_INI        2
#define     LSB$C_TR_LNK_RUN        3
#define     LSB$C_TR_LNK_RCY        4
#define     LSB$C_TR_LNK_CNT        5
#define     LSB$C_TR_LNK_BRK        2
#define     LSB$C_ATM_LENGTH        3000

struct _lsbdef
{
    char lsb$g_efbisr [48];
    char lsb$g_efbipl8 [48];
    char lsb$g_efbreset [48];
    char lsb$g_efbnm [48];
    UINT32 lsb$l_error;
    INT32 lsb$l_extra_l_0;
    UINT32 lsb$l_putrcv;
    UINT32 lsb$l_getrcv;
    UINT32 lsb$l_putxmt;
    UINT32 lsb$l_getxmt;
    UINT32 lsb$l_putcmd;
    UINT32 lsb$l_getcmd;
    UINT32 lsb$l_putuns;
    UINT32 lsb$l_getuns;
    UINT32 lsb$l_putsmt;
    UINT32 lsb$l_getsmt;
    UINT32 lsb$l_xmtavl;
    UINT32 lsb$l_rcvcnt;
    UINT32 lsb$l_xmtcnt;
    UINT32 lsb$l_cmdcnt;
    UINT32 lsb$l_unscnt;
    UINT32 lsb$l_smtcnt;
    void *lsb$a_rcvrng;
    UINT32 lsb$l_rcvent;
    UINT32 lsb$l_rcvsiz;
    void *lsb$a_xmtrng;
    UINT32 lsb$l_xmtent;
    UINT32 lsb$l_xmtsiz;
    void *lsb$a_cmdrng;
    UINT32 lsb$l_cmdent;
    UINT32 lsb$l_cmdsiz;
    void *lsb$a_unsrng;
    UINT32 lsb$l_unsent;
    UINT32 lsb$l_unssiz;
    void *lsb$a_smtrng;
    UINT32 lsb$l_smtent;
    UINT32 lsb$l_smtsiz;
    UINT32 lsb$l_scratch1;
    UINT32 lsb$l_scratch2;
    UINT32 lsb$l_scratch3;
    UINT32 lsb$l_scratch4;
    UINT32 lsb$l_scratch5;
    UINT32 lsb$l_lan_code;
    UINT32 lsb$l_drv_code;
    UINT32 lsb$l_dev1_code;
    UINT32 lsb$l_dev2_code;
    UINT64 lsb$q_lan_version;
    UINT64 lsb$q_drv_version;
    UINT64 lsb$q_dev1_version;
    UINT64 lsb$q_dev2_version;
    char lsb$t_hw_serial [12];
    UINT32 lsb$l_hw_version;
    UINT64 lsb$q_hw_bvc;
    char lsb$g_ldc [32];
    UINT32 lsb$l_device;
    UINT32 lsb$l_next_lsb;
    void *lsb$a_baselsb;
    INT32 lsb$l_extra_l_1;
    union
    {
        UINT32 lsb$l_flags;
        struct
        {
            unsigned lsb$v_created      : 1;
            unsigned lsb$v_enabled      : 1;
            unsigned lsb$v_benabled     : 1;
            unsigned lsb$v_nmdis        : 1;
            unsigned lsb$v_elan     : 1;
            unsigned lsb$v_clip     : 1;
            unsigned lsb$v_fill_44_     : 2;
        };
    };
    UINT32 lsb$l_adp;
    UINT32 lsb$l_idb;
    UINT32 lsb$l_ucb0;
    void *lsb$a_eib;
    UINT32 lsb$l_events;
    void *lsb$a_nm_llb;
    void *lsb$a_nm_lhb;
    UINT64 lsb$q_nm_lpb;
    INT32 lsb$l_nm_snm_len;
    void *lsb$a_nm_snm;
    UINT32 lsb$l_nm_last_port;
    union
    {
        UINT32 lsb$l_nm_flags;
        struct
        {
            unsigned lsb$v_nm_mac_addr  : 1;
            unsigned lsb$v_fill_45_     : 7;
        };
    };
    UINT32 lsb$l_nm_min_tid;
    UINT32 lsb$l_nm_max_tid;
    UINT64 lsb$q_nm_desc;
    UINT32 lsb$l_station_buf;
    UINT32 lsb$l_nm_scratch;
    void *lsb$a_init_vcib;
    void *lsb$a_allocate_vcrp;
    UINT32 lsb$l_cretim;
    UINT32 lsb$l_deltim;
    UINT32 lsb$l_enatim;
    UINT32 lsb$l_distim;
    UINT32 lsb$l_lstrcvtim;
    UINT32 lsb$l_lstxmttim;
    UINT32 lsb$l_lstftltim;
    UINT32 lsb$l_prvftltim;
    UINT32 lsb$l_lstexctim;
    UINT32 lsb$l_lstcartim;
    UINT32 lsb$l_lstcoltim;
    UINT32 lsb$l_lstcrctim;
    UINT32 lsb$l_lstlentim;
    UINT32 lsb$l_lstusbtim;
    UINT32 lsb$l_lstuubtim;
    UINT32 lsb$l_lstxertim;
    UINT32 lsb$l_lstrertim;
    UINT32 lsb$l_datxmttim;
    UINT32 lsb$l_datrcvtim;
    UINT32 lsb$l_lstrnatim;
    UINT32 lsb$l_lstravtim;
    UINT32 lsb$l_lstriitim;
    UINT32 lsb$l_lstrirtim;
    UINT32 lsb$l_lstrbitim;
    UINT32 lsb$l_lstdattim;
    UINT32 lsb$l_lstdtdtim;
    UINT32 lsb$l_lstrprtim;
    UINT32 lsb$l_lstfcitim;
    UINT32 lsb$l_lsttritim;
    UINT32 lsb$l_lsttrrtim;
    UINT32 lsb$l_lstdbctim;
    UINT32 lsb$l_lstebetim;
    UINT32 lsb$l_lstlcttim;
    UINT32 lsb$l_lstlemtim;
    UINT32 lsb$l_lstlnktim;
    UINT32 lsb$l_lstcnctim;
    UINT32 lsb$l_lsteentim;
    UINT32 lsb$l_lsteditim;
    UINT32 lsb$l_lsteactim;
    UINT32 lsb$l_lsteuntim;
    UINT32 lsb$l_ctltim;
    UINT32 lsb$l_xmttim;
    UINT32 lsb$l_sidtim;
    UINT32 lsb$l_unatim;
    UINT32 lsb$l_usbtim;
    UINT32 lsb$l_usbsav;
    UINT32 lsb$l_rcvmnr;
    UINT32 lsb$l_rcvmxr;
    UINT32 lsb$l_rcvmin;
    UINT32 lsb$l_rcvmax;
    UINT32 lsb$l_rcvcur;
    UINT32 lsb$l_rcvfil;
    UINT32 lsb$l_rcvadd;
    UINT32 lsb$l_crectr;
    UINT32 lsb$l_enactr;
    UINT32 lsb$l_prtctr;
    UINT32 lsb$l_excctr;
    UINT32 lsb$l_ftlctr;
    UINT32 lsb$l_xmttmo;
    UINT32 lsb$l_ctltmo;
    UINT32 lsb$l_rsfctr;
    UINT32 lsb$l_powerf;
    UINT32 lsb$l_xnwctr;
    UINT32 lsb$l_xbpctr;
    UINT32 lsb$l_xgpctr;
    UINT32 lsb$l_xsbctr;
    UINT32 lsb$l_xbactr;
    UINT32 lsb$l_arpctr;
    UINT32 lsb$l_dat_inictr;
    UINT32 lsb$l_dat_errctr;
    UINT32 lsb$l_dat_stage;
    UINT32 lsb$l_dat_vcrp;
    UINT64 lsb$q_dat_xmtsts;
    UINT32 lsb$l_dat_reqnum;
    void *lsb$a_dat_ucb;
    UINT32 lsb$l_devdepend;
    UINT32 lsb$l_lsterrcsr;
    UINT32 lsb$l_lstftlcod;
    UINT32 lsb$l_prvftlcod;
    UINT64 lsb$q_lstcrcsrc;
    void *lsb$a_trace;
    UINT32 lsb$l_trclen;
    UINT64 lsb$q_trcmask;
    UINT64 lsb$q_trcstop;
    UINT32 lsb$l_trcindex;
    UINT32 lsb$l_trccount;
    UINT32 lsb$l_trcseq;
    UINT32 lsb$l_trcintp;
    UINT32 lsb$l_trcintc;
    void *lsb$a_trcdev;
    UINT32 lsb$l_trcdevlen;
    void *lsb$a_trcmaskdef;
    UINT64 lsb$q_xmtreq;
    UINT64 lsb$q_ctlhld;
    UINT64 lsb$q_ctlreq;
    UINT64 lsb$q_ctlpnd;
    UINT64 lsb$q_post;
    UINT64 lsb$q_delay;
    UINT64 lsb$q_xmtpnd;
    UINT64 lsb$q_rcvpnd;
    UINT64 lsb$q_autors;
    UINT64 lsb$q_nmhold;
    UINT64 lsb$q_rcvbuf;
    UINT32 lsb$l_min_chain;
    UINT32 lsb$l_min_xmt;
    UINT32 lsb$l_align_phy;
    UINT32 lsb$l_align_mask;
    UINT32 lsb$l_bsz;
    UINT32 lsb$l_devhdr_len;
    UINT32 lsb$l_svapte;
    UINT32 lsb$l_sva;
    UINT64 lsb$q_lbuctr;
    UINT64 lsb$q_zerctr;
    UINT64 lsb$q_ocrctr;
    UINT64 lsb$q_octctr;
    UINT64 lsb$q_pdrctr;
    UINT64 lsb$q_pdtctr;
    UINT64 lsb$q_morctr;
    UINT64 lsb$q_mprctr;
    UINT64 lsb$q_defctr;
    UINT64 lsb$q_sinctr;
    UINT64 lsb$q_mulctr;
    UINT64 lsb$q_excctr;
    UINT64 lsb$q_ccfctr;
    UINT64 lsb$q_shcctr;
    UINT64 lsb$q_opcctr;
    UINT64 lsb$q_ttlctr;
    UINT64 lsb$q_latctr;
    UINT64 lsb$q_fcectr;
    UINT64 lsb$q_alictr;
    UINT64 lsb$q_ftlctr;
    UINT64 lsb$q_uidctr;
    UINT64 lsb$q_dorctr;
    UINT64 lsb$q_usbctr;
    UINT64 lsb$q_uubctr;
    UINT64 lsb$q_cdcctr;
    UINT64 lsb$q_motctr;
    UINT64 lsb$q_mptctr;
    UINT64 lsb$q_fsectr;
    UINT64 lsb$q_slectr;
    UINT64 lsb$q_rdlctr;
    UINT64 lsb$q_umdctr;
    UINT64 lsb$q_stfctr;
    UINT64 lsb$q_turctr;
    UINT64 lsb$q_tflctr;
    UINT64 lsb$q_rsectr;
    UINT64 lsb$q_rlectr;
    UINT64 lsb$q_mfcctr;
    UINT64 lsb$q_mecctr;
    UINT64 lsb$q_mlcctr;
    UINT64 lsb$q_riictr;
    UINT64 lsb$q_rirctr;
    UINT64 lsb$q_rbictr;
    UINT64 lsb$q_datctr;
    UINT64 lsb$q_dtdctr;
    UINT64 lsb$q_rprctr;
    UINT64 lsb$q_fcictr;
    UINT64 lsb$q_trictr;
    UINT64 lsb$q_trrctr;
    UINT64 lsb$q_dbcctr;
    UINT64 lsb$q_ebectr;
    UINT64 lsb$q_lctctr;
    UINT64 lsb$q_lemctr;
    UINT64 lsb$q_lnkctr;
    UINT64 lsb$q_cncctr;
    UINT64 lsb$q_berctr;
    UINT64 lsb$q_raectr;
    UINT64 lsb$q_lerctr;
    UINT64 lsb$q_tlfctr;
    UINT64 lsb$q_fcpctr;
    UINT64 lsb$q_terctr;
    UINT64 lsb$q_sigctr;
    UINT64 lsb$q_rbectr;
    UINT64 lsb$q_serctr;
    UINT64 lsb$q_lwfctr;
    UINT64 lsb$q_arectr;
    UINT64 lsb$q_rmrctr;
    UINT64 lsb$q_sstctr;
    UINT64 lsb$q_rrectr;
    UINT64 lsb$q_adsctr;
    UINT64 lsb$q_ierctr;
    UINT64 lsb$q_zerclr;
    UINT64 lsb$q_ocrclr;
    UINT64 lsb$q_octclr;
    UINT64 lsb$q_pdrclr;
    UINT64 lsb$q_pdtclr;
    UINT64 lsb$q_morclr;
    UINT64 lsb$q_mprclr;
    UINT64 lsb$q_defclr;
    UINT64 lsb$q_sinclr;
    UINT64 lsb$q_mulclr;
    UINT64 lsb$q_excclr;
    UINT64 lsb$q_ccfclr;
    UINT64 lsb$q_shcclr;
    UINT64 lsb$q_opcclr;
    UINT64 lsb$q_ttlclr;
    UINT64 lsb$q_latclr;
    UINT64 lsb$q_fceclr;
    UINT64 lsb$q_aliclr;
    UINT64 lsb$q_ftlclr;
    UINT64 lsb$q_uidclr;
    UINT64 lsb$q_dorclr;
    UINT64 lsb$q_usbclr;
    UINT64 lsb$q_uubclr;
    UINT64 lsb$q_cdcclr;
    UINT64 lsb$q_motclr;
    UINT64 lsb$q_mptclr;
    UINT64 lsb$q_fseclr;
    UINT64 lsb$q_sleclr;
    UINT64 lsb$q_rdlclr;
    UINT64 lsb$q_umdclr;
    UINT64 lsb$q_stfclr;
    UINT64 lsb$q_turclr;
    UINT64 lsb$q_tflclr;
    UINT64 lsb$q_rseclr;
    UINT64 lsb$q_rleclr;
    UINT64 lsb$q_mfcclr;
    UINT64 lsb$q_mecclr;
    UINT64 lsb$q_mlcclr;
    UINT64 lsb$q_riiclr;
    UINT64 lsb$q_rirclr;
    UINT64 lsb$q_rbiclr;
    UINT64 lsb$q_datclr;
    UINT64 lsb$q_dtdclr;
    UINT64 lsb$q_rprclr;
    UINT64 lsb$q_fciclr;
    UINT64 lsb$q_triclr;
    UINT64 lsb$q_trrclr;
    UINT64 lsb$q_dbcclr;
    UINT64 lsb$q_ebeclr;
    UINT64 lsb$q_lctclr;
    UINT64 lsb$q_lemclr;
    UINT64 lsb$q_lnkclr;
    UINT64 lsb$q_cncclr;
    UINT64 lsb$q_berclr;
    UINT64 lsb$q_raeclr;
    UINT64 lsb$q_lerclr;
    UINT64 lsb$q_tlfclr;
    UINT64 lsb$q_fcpclr;
    UINT64 lsb$q_terclr;
    UINT64 lsb$q_sigclr;
    UINT64 lsb$q_rbeclr;
    UINT64 lsb$q_serclr;
    UINT64 lsb$q_lwfclr;
    UINT64 lsb$q_areclr;
    UINT64 lsb$q_rmrclr;
    UINT64 lsb$q_sstclr;
    UINT64 lsb$q_rreclr;
    UINT64 lsb$q_adsclr;
    UINT64 lsb$q_ierclr;
    INT16 lsb$w_extra_w_0;
    UINT16 lsb$w_zerctr;
    UINT32 lsb$l_brcctr;
    UINT32 lsb$l_bsnctr;
    UINT32 lsb$l_dbrctr;
    UINT32 lsb$l_dbsctr;
    UINT32 lsb$l_mbyctr;
    UINT32 lsb$l_mblctr;
    UINT32 lsb$l_bidctr;
    UINT32 lsb$l_bs1ctr;
    UINT32 lsb$l_bsmctr;
    UINT16 lsb$w_sflctr;
    union
    {
        UINT16 lsb$w_sflmap;
        struct
        {
            unsigned lsb$v_sfl_exc      : 1;
            unsigned lsb$v_sfl_ccf      : 1;
            unsigned lsb$v_sfl_shc      : 1;
            unsigned lsb$v_sfl_opc      : 1;
            unsigned lsb$v_sfl_ftl      : 1;
            unsigned lsb$v_sfl_rfd      : 1;
            unsigned lsb$v_sfl_tur      : 1;
            unsigned lsb$v_sfl_tfl      : 1;
        };
    };
    UINT16 lsb$w_rflctr;
    union
    {
        UINT16 lsb$w_rflmap;
        struct
        {
            unsigned lsb$v_rfl_bce      : 1;
            unsigned lsb$v_rfl_fme      : 1;
            unsigned lsb$v_rfl_ftl      : 1;
            unsigned lsb$v_rfl_rse      : 1;
            unsigned lsb$v_rfl_rle      : 1;
            unsigned lsb$v_fill_46_     : 3;
        };
    };
    UINT16 lsb$w_ufdctr;
    UINT16 lsb$w_ovrctr;
    UINT16 lsb$w_sbuctr;
    UINT16 lsb$w_ubuctr;
    UINT32 lsb$l_mbsctr;
    UINT32 lsb$l_msnctr;
    UINT32 lsb$l_mfcctr;
    UINT32 lsb$l_mecctr;
    UINT32 lsb$l_mlcctr;
    UINT32 lsb$l_riictr;
    UINT32 lsb$l_rirctr;
    UINT32 lsb$l_rbictr;
    UINT32 lsb$l_datctr;
    UINT32 lsb$l_dtdctr;
    UINT32 lsb$l_rprctr;
    UINT32 lsb$l_fcictr;
    UINT32 lsb$l_trictr;
    UINT32 lsb$l_trrctr;
    UINT32 lsb$l_dbcctr;
    UINT32 lsb$l_ebectr;
    UINT32 lsb$l_lctctr;
    UINT32 lsb$l_lemctr;
    UINT32 lsb$l_lnkctr;
    UINT32 lsb$l_cncctr;
    UINT32 lsb$l_sfrctr;
    union
    {
        UINT32 lsb$l_trsflmap;
        struct
        {
            unsigned lsb$v_trsfl_tur        : 1;
            unsigned lsb$v_trsfl_le     : 1;
            unsigned lsb$v_trsfl_abs        : 1;
            unsigned lsb$v_trsfl_lf     : 1;
            unsigned lsb$v_trsfl_te     : 1;
            unsigned lsb$v_trsfl_uf     : 1;
            unsigned lsb$v_trsfl_rc     : 1;
            unsigned lsb$v_fill_47_     : 1;
        };
    };
    UINT32 lsb$l_rfrctr;
    union
    {
        UINT32 lsb$l_trrflmap;
        struct
        {
            unsigned lsb$v_trrfl_rc     : 1;
            unsigned lsb$v_trrfl_fce        : 1;
            unsigned lsb$v_fill_48_     : 6;
        };
    };
    UINT32 lsb$l_ifrctr;
    union
    {
        UINT32 lsb$l_triflmap;
        struct
        {
            unsigned lsb$v_trifl_lwf        : 1;
            unsigned lsb$v_trifl_sle        : 1;
            unsigned lsb$v_trifl_tmo        : 1;
            unsigned lsb$v_trifl_rpt        : 1;
            unsigned lsb$v_trifl_bcn        : 1;
            unsigned lsb$v_trifl_dad        : 1;
            unsigned lsb$v_trifl_psf        : 1;
            unsigned lsb$v_trifl_rr     : 1;
        };
    };
    UINT32 lsb$l_rgfctr;
    union
    {
        UINT32 lsb$l_trtflmap;
        struct
        {
            unsigned lsb$v_trtfl_lwf        : 1;
            unsigned lsb$v_trtfl_ssd        : 1;
            unsigned lsb$v_trtfl_arf        : 1;
            unsigned lsb$v_trtfl_rr     : 1;
            unsigned lsb$v_fill_49_     : 4;
        };
    };
    UINT32 lsb$l_rpgctr;
    UINT32 lsb$l_mncctr;
    UINT32 lsb$l_bcnctr;
    UINT32 lsb$l_lerctr;
    UINT32 lsb$l_ierctr;
    UINT32 lsb$l_berctr;
    UINT32 lsb$l_raectr;
    UINT32 lsb$l_adsctr;
    UINT32 lsb$l_piectr;
    UINT32 lsb$l_tlfctr;
    UINT32 lsb$l_rcectr;
    UINT32 lsb$l_fcectr;
    UINT32 lsb$l_ferctr;
    UINT32 lsb$l_terctr;
    UINT32 lsb$l_pnectr;
    UINT16 lsb$w_cdcctr;
    INT16 lsb$w_extra_w_1;
    UINT32 lsb$l_chlmsg;
    UINT32 lsb$l_chlerr;
    UINT32 lsb$l_sidmsg;
    UINT32 lsb$l_siderr;
    UINT32 lsb$l_rqcmsg;
    UINT32 lsb$l_rqcerr;
    UINT32 lsb$l_ravctr;
    UINT32 lsb$l_rnactr;
    union
    {
        UINT32 lsb$l_char;
        struct
        {
            unsigned lsb$v_devctr       : 1;
            unsigned lsb$v_devxidtest       : 1;
            unsigned lsb$v_mulseg       : 1;
            unsigned lsb$v_rcvmgt       : 1;
            unsigned lsb$v_fill_50_     : 4;
        };
    };
    union
    {
        UINT32 lsb$l_sts;
        struct
        {
            unsigned lsb$v_run      : 1;
            unsigned lsb$v_inited       : 1;
            unsigned lsb$v_mca_ovf      : 1;
            unsigned lsb$v_fat_err      : 1;
            unsigned lsb$v_blkctl       : 1;
            unsigned lsb$v_pchact       : 1;
            unsigned lsb$v_need_dat_lock    : 1;
            unsigned lsb$v_got_dat_lock : 1;
            unsigned lsb$v_ring_avail       : 1;
            unsigned lsb$v_full_duplex  : 1;
            unsigned lsb$v_sts_filler       : 22;
        };
    };
    UINT32 lsb$l_mlt;
    UINT32 lsb$l_prm;
    UINT32 lsb$l_prmuser;
    UINT32 lsb$l_media;
    UINT32 lsb$l_speed;
    INT32 lsb$l_extra_l_2;
    UINT32 lsb$l_con;
    UINT32 lsb$l_ilp;
    UINT32 lsb$l_crc;
    INT32 lsb$l_extra_l_3;
    UINT16 lsb$g_pha [4];
    UINT16 lsb$g_hwa [4];
    UINT16 lsb$g_nmpha [4];
    UINT32 lsb$l_line_param;
    UINT32 lsb$l_circuit_param;
    UINT32 lsb$l_untcnt;
    UINT32 lsb$l_gen_multi;
    UINT32 lsb$l_mca_max;
    UINT32 lsb$l_mca_cur;
    UINT32 lsb$l_dev_opr;
    UINT32 lsb$l_fdx_ena;
    UINT32 lsb$l_fdx_opr;
    UINT32 lsb$l_maxfsz;
    UINT32 lsb$l_qos;
    UINT32 lsb$l_reg_hwa;
    UINT32 lsb$l_valid_cnm;
    UINT32 lsb$l_valid_fmt;
    UINT32 lsb$l_valid_gsp;
    UINT32 lsb$l_valid_mca;
    UINT32 lsb$l_valid_src;
    UINT32 lsb$l_valid_pha;
    UINT32 lsb$l_valid_pid;
    UINT32 lsb$l_valid_prm;
    UINT32 lsb$l_valid_pty;
    UINT32 lsb$l_valid_sap;
    UINT32 lsb$l_set_cnm;
    UINT32 lsb$l_set_des;
    UINT32 lsb$l_set_mca;
    UINT32 lsb$l_set_src;
    UINT32 lsb$l_set_pha;
    UINT32 lsb$l_set_mnr;
    UINT32 lsb$l_set_mxr;
    UINT32 lsb$l_set_80ns;
    UINT32 lsb$l_set_rtto;
    UINT32 lsb$l_sho_80ns;
    UINT32 lsb$l_sho_rtto;
    UINT32 lsb$l_sysid_msg;
    UINT32 lsb$l_dev_timer;
    UINT32 lsb$l_process_ctl;
    UINT32 lsb$l_dev_transmit;
    UINT32 lsb$l_sho_lnk;
    void (*lsb$a_fork_proc)(void);
    void (*lsb$a_chngpro)(void);
    void (*lsb$a_counter)(void);
    void (*lsb$a_diag)(void);
    void (*lsb$a_init)(void);
    void (*lsb$a_port)(void);
    void (*lsb$a_power_fail)(void);
    void (*lsb$a_receive)(void);
    void (*lsb$a_sense_mac)(void);
    void (*lsb$a_set_mac)(void);
    void (*lsb$a_stop)(void);
    void (*lsb$a_stoppro)(void);
    void (*lsb$a_strtpro)(void);
    void (*lsb$a_strtpropha)(void);
    void (*lsb$a_timer)(void);
    int (*lsb$a_transmit)(void);
    void (*lsb$a_unit_init)(void);
    void (*lsb$a_mask)(void);
    union
    {
        void *lsb$a_elan_lsbs;
        void *lsb$a_port_lsbs;
        void *lsb$a_clip_lsbs;
    };
    UINT32 lsb$l_internal_ctrs;
    UINT32 lsb$l_internal_ctrsize;
    UINT32 lsb$l_internal_ctrstr;
    UINT32 lsb$l_mon;
    UINT32 lsb$l_monprm;
    void *lsb$a_mon_user1;
    void *lsb$a_mon_user2;
    INT32 lsb$l_extra_l_4;
    UINT32 lsb$l_prm_done;
    UINT32 lsb$l_delivered;
    UINT32 lsb$l_next_802;
    UINT32 lsb$l_rsp_802;
    UINT32 lsb$l_pty_ucb;
    UINT32 lsb$l_pty_last;
    UINT32 lsb$l_sap_ucb;
    UINT32 lsb$l_sap_last;
    UINT32 lsb$l_pid_ucb;
    UINT64 lsb$q_pid_last;
    INT64 lsb$q_mbx_base;
    UINT32 lsb$l_mbx_read;
    UINT32 lsb$l_mbx_write;
    UINT32 lsb$l_mbx_num;
    UINT32 lsb$l_mbx_enable;
    UINT32 lsb$g_mbx_array [12];
    union
    {
        struct
        {
            UINT32 lsb$l_csmacd_elan_state_req;
            union
            {
                UINT32 lsb$l_csmacd_elan_state;
                struct
                {
                    unsigned lsb$v_csmacd_elan_active       : 1;
                    unsigned lsb$v_csmacd_elan_start        : 1;
                    unsigned lsb$v_csmacd_elan_shut     : 1;
                    unsigned lsb$v_csmacd_elan_unavailable  : 1;
                    unsigned lsb$v_fill_51_         : 4;
                };
            };
            UINT32 lsb$l_csmacd_event_mask_req;
            UINT32 lsb$l_csmacd_event_mask;
            UINT32 lsb$l_csmacd_ext_sense_req;
            UINT32 lsb$l_csmacd_ext_sense;
            UINT32 lsb$l_csmacd_pvc_req;
            UINT32 lsb$l_csmacd_pvc;
            UINT32 lsb$l_csmacd_pvc_num;
            UINT32 lsb$l_csmacd_max_pktsize;
            UINT32 lsb$l_csmacd_medium;
            char lsb$t_csmacd_elan_name [64];
            char lsb$t_csmacd_elan_desc [64];
            char lsb$t_csmacd_parent_dev [8];
            struct
            {
                UINT8 lsb$b_csmacd_les_prefix [13];
                UINT8 lsb$b_csmacd_les_esi [6];
                UINT8 lsb$b_csmacd_les_sel [1];
            };
            void *lsb$a_csmacd_lec_attr;
            UINT32 lsb$l_csmacd_lec_len;
            UINT32 lsb$l_csmacd_clip_state_req;
            union
            {
                UINT32 lsb$l_csmacd_clip_state;
                struct
                {
                    unsigned lsb$v_csmacd_lis_active    : 1;
                    unsigned lsb$v_csmacd_lis_start : 1;
                    unsigned lsb$v_csmacd_lis_shut  : 1;
                    unsigned lsb$v_csmacd_lis_unavailable   : 1;
                    unsigned lsb$v_fill_52_     : 4;
                };
            };
            void *lsb$a_csmacd_clip_attr;
            UINT32 lsb$l_csmacd_clip_len;
            UINT32 lsb$l_csmacd_clip_user_type;
            struct
            {
                UINT8 lsb$b_csmacd_clip_prefix [13];
                UINT8 lsb$b_csmacd_clip_esi [6];
                UINT8 lsb$b_csmacd_clip_sel [1];
            };
            struct
            {
                UINT8 lsb$b_csmacd_clip_atm_prefix [13];
                UINT8 lsb$b_csmacd_clip_atm_esi [6];
                UINT8 lsb$b_csmacd_clip_atm_sel [1];
            };
            UINT8 lsb$t_csmacd_clip_proto_addr [6];
            INT8 lsb$b_csmacd_clip_pad1 [2];
            UINT8 lsb$t_csmacd_clip_subnet_mask [6];
            INT8 lsb$b_csmacd_clip_pad2 [2];
            char lsb$t_csmacd_clip_name [64];
            void *lsb$g_clip_pvc;
            UINT32 lsb$l_csmacd_get_pvc;
            UINT32 lsb$l_csmacd_set_pvc;
            UINT32 lsb$l_csmacd_sho_pvc;
            UINT32 lsb$l_reserved;
            void *lsb$a_csmacd_lecs_cfg_handle;
            void *lsb$a_csmacd_lec_cfg_handle;
            void *lsb$a_csmacd_cbrock_handle;
            void *lsb$a_csmacd_elan_handle;
            void *lsb$a_csmacd_lec_iface_handle;
            UINT32 lsb$l_csmacd_reserved_1;
            UINT32 lsb$l_csmacd_reserved_2;
            UINT32 lsb$l_csmacd_reserved_3;
            UINT32 lsb$l_csmacd_reserved_4;
            UINT32 lsb$l_csmacd_reserved_5;
            UINT32 lsb$l_csmacd_reserved_6;
            void *lsb$a_csmacd_clip_handle;
            void *lsb$a_csmacd_clip_cfg_handle;
            void *lsb$a_csmacd_clip_iface_handle;
            UINT32 lsb$l_csmacd_reserved_7;
            UINT32 lsb$l_csmacd_reserved_8;
            UINT32 lsb$l_csmacd_reserved_9;
            UINT32 lsb$l_csmacd_reserved_10;
            UINT32 lsb$l_csmacd_reserved_11;
            UINT32 lsb$l_csmacd_reserved_12;
        };
        struct
        {
            UINT32 lsb$l_fddi_dlver;
            UINT32 lsb$l_fddi_t_max;
            UINT32 lsb$l_fddi_t_neg;
            UINT32 lsb$l_fddi_t_req;
            UINT32 lsb$l_fddi_tvx;
            UINT32 lsb$l_fddi_rtto;
            UINT32 lsb$l_fddi_rpe;
            UINT32 lsb$l_fddi_lem;
            UINT32 lsb$l_fddi_rer;
            UINT32 lsb$l_fddi_rjr;
            UINT32 lsb$l_fddi_lee;
            UINT32 lsb$l_fddi_una_dat;
            UINT16 lsb$g_fddi_una [4];
            UINT16 lsb$g_fddi_old_una [4];
            UINT16 lsb$g_fddi_sif_conf_targ [4];
            UINT16 lsb$g_fddi_sif_op_targ [4];
            UINT16 lsb$g_fddi_nif_targ [4];
            UINT16 lsb$g_fddi_echo_targ [4];
            UINT16 lsb$g_fddi_dna [4];
            UINT16 lsb$g_fddi_old_dna [4];
            UINT32 lsb$l_fddi_echo_dat;
            UINT32 lsb$l_fddi_echo_len;
            UINT32 lsb$l_fddi_phy_state;
            UINT32 lsb$l_fddi_link_state;
            UINT32 lsb$l_fddi_rp_state;
            UINT32 lsb$l_fddi_port_type;
            UINT32 lsb$l_fddi_nbr_phy;
            UINT32 lsb$l_fddi_dat;
            union
            {
                UINT32 lsb$l_fddi_flags;
                struct
                {
                    unsigned lsb$v_fddi_boo     : 1;
                    unsigned lsb$v_fddi_cty     : 1;
                    unsigned lsb$v_fddi_unto        : 1;
                    unsigned lsb$v_fill_53_     : 5;
                };
            };
            UINT32 lsb$l_fddi_smt;
            UINT32 lsb$l_fddi_smt_prm;
            UINT32 lsb$l_fddi_smt_type;
        };
        struct
        {
            INT32 lsb$l_extra_l_5;
            UINT32 lsb$l_tr_speed;
            UINT32 lsb$l_tr_etr;
            UINT32 lsb$l_tr_moncon;
            UINT64 lsb$q_tr_naun;
            UINT32 lsb$l_tr_rer;
            UINT32 lsb$l_tr_ier;
            UINT32 lsb$l_tr_pdn;
            UINT32 lsb$l_tr_rgn;
            UINT32 lsb$l_tr_maxac;
            UINT32 lsb$l_tr_fcfield;
            UINT32 lsb$l_tr_srcatmo;
            UINT32 lsb$l_tr_srdtmo;
            UINT32 lsb$l_tr_sr;
            void *lsb$a_tr_sr_tab;
            UINT32 lsb$l_tr_sr_size;
            UINT32 lsb$l_tr_sr_tabsiz;
            UINT32 lsb$l_tr_sr_tabbyte;
            UINT32 lsb$l_tr_sr_inuse;
            UINT32 lsb$l_tr_sr_garbage;
            UINT32 lsb$l_tr_sr_deleted;
            UINT64 lsb$q_tr_sr_xplorq;
            UINT32 lsb$l_tr_sr_xplorb;
            UINT32 lsb$l_tr_sr_xplorc;
            void *lsb$a_tr_fca;
            UINT32 lsb$l_tr_fca_used;
            UINT32 lsb$l_tr_fcmask;
            void *lsb$a_tr_hdr;
            UINT32 lsb$l_tr_hdrsize;
            UINT32 lsb$l_tr_lnkstate;
        };
        struct
        {
            void *lsb$a_atm_drv_cm_vc_setup;
            void *lsb$a_atm_drv_cm_vc_teardown;
            void *lsb$a_atm_drv_cm_vc_flush;
            void *lsb$a_atm_drv_cm_tm_command;
            void *lsb$a_atm_drv_clip_xmt_done;
            void *lsb$a_atm_drv_clip_rcv;
            void *lsb$a_atm_drv_handles;
            void *lsb$a_atm_usr_handles;
            void *lsb$a_atm_amm_handle;
            void *lsb$a_atm_addr_reg_handle;
            void *lsb$a_atm_tm_an2_handle;
            void *lsb$a_atm_clip_handle;
            void *lsb$a_atm_lec_handle;
            void *lsb$a_atm_rcv_handle;
            void *lsb$a_atm_xmt_handle;
            UINT32 lsb$l_atm_tqe_alloc;
            UINT32 lsb$l_atm_tqe_used;
            UINT32 lsb$l_atm_mem_alloc;
            UINT32 lsb$l_atm_mem_used;
            UINT32 lsb$l_atm_hwanum;
            UINT32 lsb$l_atm_hwanum_used;
            void *lsb$a_atm_cd_handle;
            void *lsb$a_atm_cd_callbacks;
            void *lsb$a_atm_module_profilep;
            UINT32 lsb$l_atm_max_vc;
            UINT32 lsb$l_atm_reserved_1;
            UINT32 lsb$l_atm_reserved_2;
            UINT32 lsb$l_atm_reserved_3;
            UINT32 lsb$l_atm_reserved_4;
            UINT32 lsb$l_atm_reserved_5;
        };
    };
};

#define     SR$C_LOCAL      0
#define     SR$C_UNKNOWN        2
#define     SR$C_KNOWN      4
#define     SR$C_STALE      1
#define     SR$C_DELETED        3
#define     SR$C_EXPLORING      5
#define     SR$C_SR_SIZE        64
#define     SR$C_SR_SHIFT       6

struct _sr
{
    UINT64 sr$g_addr;
    UINT32 sr$l_state;
    UINT32 sr$l_ri_s;
    UINT8 sr$g_ri [32];
    UINT32 sr$l_lstxmttim;
    UINT32 sr$l_lstrcvtim;
    UINT32 sr$l_staletim;
    UINT32 sr$l_discvtim;
};

#define     FCA$C_FCA_ENTRIES   200
#define     FCA$C_FCA_SIZE      16

struct _fca
{
    UINT64 fca$g_addr;
    UINT32 fca$l_fc_mask;
    UINT32 fca$l_fc_fill;
};

#define     TRC$Q_PK_DATA       0
#define     TRC$G_PK_DATA       12
#define     TRC$C_INIT_ENTRIES  256
#define     TRC$C_DEF_ENTRIES   2048
#define     TRC$C_NUM_INT       8
#define     TRC$C_LENGTH        32

#define     TRC$C_EXTENSION     128

#define     TRC$C_TIMER     1
#define     TRC$C_INTR      2
#define     TRC$C_FRK_START     3
#define     TRC$C_FRK_DONE      4
#define     TRC$C_FRK_ERROR     5
#define     TRC$C_FRK_SOFT      6
#define     TRC$C_STATE     7
#define     TRC$C_INIT      8
#define     TRC$C_CHNGPRO       9
#define     TRC$C_STOP      10
#define     TRC$C_SHUTDOWN      11
#define     TRC$C_RCV_ISS       12
#define     TRC$C_RCV_DONE      13
#define     TRC$C_RCV_ERR       14
#define     TRC$C_RCV_PKT       15
#define     TRC$C_XMT_ISS       16
#define     TRC$C_XMT_QUE       17
#define     TRC$C_XMT_DONE      18
#define     TRC$C_XMT_ERR       19
#define     TRC$C_XMT_PKT       20
#define     TRC$C_SEGMENT       21
#define     TRC$C_SR_ENTRY      22
#define     TRC$C_VCI_ACTION    23
#define     TRC$C_LAN24     24
#define     TRC$C_LAN25     25
#define     TRC$C_LAN26     26
#define     TRC$C_LAN27     27
#define     TRC$C_LAN28     28
#define     TRC$C_LAN29     29
#define     TRC$C_LAN30     30
#define     TRC$C_LAN31     31
#define     TRC$C_LAN32     32

struct _trc
{
    UINT64 trc$q_time;
    UINT32 trc$l_sequence;
    UINT8 trc$b_type;
    UINT8 trc$b_xmtcnt;
    UINT8 trc$b_rcvcnt;
    UINT8 trc$b_misc;
    UINT32 trc$l_param1;
    UINT32 trc$l_param2;
    UINT32 trc$l_param3;
    UINT32 trc$l_param4;
};

#endif

