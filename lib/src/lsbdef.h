#ifndef lsbdef_h
#define lsbdef_h

#define		LSB$M_CREATED		0x1
#define		LSB$M_ENABLED		0x2
#define		LSB$M_BENABLED		0x4
#define		LSB$M_NMDIS		0x8
#define		LSB$M_ELAN		0x10
#define		LSB$M_CLIP		0x20
#define		LSB$M_NM_MAC_ADDR	0x1
#define		LSB$C_TIMUSB		60

#define		LSB$C_MIN_RCV_MIN	32
#define		LSB$C_DEF_RCV_MIN	64
#define		LSB$C_MAX_RCV_MIN	512
#define		LSB$C_MIN_RCV_MAX	64
#define		LSB$C_DEF_RCV_MAX	128
#define		LSB$C_MAX_RCV_MAX	768
#define		LSB$C_RCV_INC		4
#define		LSB$C_RCV_DEC		1
#define		LSB$C_RCV_ADD		32

#define		LSB$C_QUEUES		936
#define		LSB$C_ABORT_QS		1

#define		LSB$C_QS_CLEAN_ACT	7

#define		LSB$C_QS_CLEAN_INACT	8

#define		LSB$C_INIT_QS		10

#define		LSB$G_V4_CTRSECT	1072
#define		LSB$G_END_V4_CTRSECT	1256

#define		LSB$G_PIVCLR		1616
#define		LSB$C_PIVSIZ		552

#define		LSB$G_CTRSECT		2170
#define		LSB$M_SFL_EXC		0x1
#define		LSB$M_SFL_CCF		0x2
#define		LSB$M_SFL_SHC		0x4
#define		LSB$M_SFL_OPC		0x8
#define		LSB$M_SFL_FTL		0x10
#define		LSB$M_SFL_RFD		0x20
#define		LSB$M_SFL_TUR		0x40
#define		LSB$M_SFL_TFL		0x80
#define		LSB$M_RFL_BCE		0x1
#define		LSB$M_RFL_FME		0x2
#define		LSB$M_RFL_FTL		0x4
#define		LSB$M_RFL_RSE		0x8
#define		LSB$M_RFL_RLE		0x10
#define		LSB$G_END_CTRSECT	2224
#define		LSB$M_TRSFL_TUR		0x1
#define		LSB$M_TRSFL_LE		0x2
#define		LSB$M_TRSFL_ABS		0x4
#define		LSB$M_TRSFL_LF		0x8
#define		LSB$M_TRSFL_TE		0x10
#define		LSB$M_TRSFL_UF		0x20
#define		LSB$M_TRSFL_RC		0x40
#define		LSB$M_TRRFL_RC		0x1
#define		LSB$M_TRRFL_FCE		0x2
#define		LSB$M_TRIFL_LWF		0x1
#define		LSB$M_TRIFL_SLE		0x2
#define		LSB$M_TRIFL_TMO		0x4
#define		LSB$M_TRIFL_RPT		0x8
#define		LSB$M_TRIFL_BCN		0x10
#define		LSB$M_TRIFL_DAD		0x20
#define		LSB$M_TRIFL_PSF		0x40
#define		LSB$M_TRIFL_RR		0x80
#define		LSB$M_TRTFL_LWF		0x1
#define		LSB$M_TRTFL_SSD		0x2
#define		LSB$M_TRTFL_ARF		0x4
#define		LSB$M_TRTFL_RR		0x8
#define		LSB$C_CTRSIZE		262

#define		LSB$M_DEVCTR		0x1
#define		LSB$M_DEVXIDTEST	0x2
#define		LSB$M_MULSEG		0x4
#define		LSB$M_RCVMGT		0x8
#define		LSB$M_RUN		0x1
#define		LSB$M_INITED		0x2
#define		LSB$M_MCA_OVF		0x4
#define		LSB$M_FAT_ERR		0x8
#define		LSB$M_BLKCTL		0x10
#define		LSB$M_PCHACT		0x20
#define		LSB$M_NEED_DAT_LOCK	0x40
#define		LSB$M_GOT_DAT_LOCK	0x80
#define		LSB$M_RING_AVAIL	0x100
#define		LSB$M_FULL_DUPLEX	0x200
#define		LSB$C_ALLPRM		2464
#define		LSB$C_ALLPRMSIZ		12

#define		LSB$C_CPYPRMSIZ		24

#define		LSB$C_MBX_MAX		12
#define		LSB$C_FIXED_LENGTH		2880

#define		LSB$M_CSMACD_ELAN_ACTIVE	0x1
#define		LSB$M_CSMACD_ELAN_START		0x2
#define		LSB$M_CSMACD_ELAN_SHUT		0x4
#define		LSB$M_CSMACD_ELAN_UNAVAILABLE	0x8
#define		LSB$M_CSMACD_LIS_ACTIVE		0x1
#define		LSB$M_CSMACD_LIS_START		0x2
#define		LSB$M_CSMACD_LIS_SHUT		0x4
#define		LSB$M_CSMACD_LIS_UNAVAILABLE	0x8
#define		LSB$C_CSMACD_LENGTH		3328
#define		LSB$M_FDDI_BOO			0x1
#define		LSB$M_FDDI_CTY			0x2
#define		LSB$M_FDDI_UNTO			0x4
#define		LSB$C_FDDI_LENGTH		3040

#define		LSB$C_FDDI_OFF_INIT		0
#define		LSB$C_FDDI_OFF_READY		1
#define		LSB$C_FDDI_OFF_FAULT_REC	2
#define		LSB$C_FDDI_ON_RING_INIT		3
#define		LSB$C_FDDI_ON_RING_RUN		4
#define		LSB$C_FDDI_BROKEN		5

#define		LSB$C_FDDI_OFF			0
#define		LSB$C_FDDI_CAND_PURGER		1
#define		LSB$C_FDDI_NON_PURGER		2
#define		LSB$C_FDDI_PURGER		3

#define		LSB$C_FDDI_UNK		0
#define		LSB$C_FDDI_SUC		1
#define		LSB$C_FDDI_FAIL		2

#define		LSB$C_FDDI_INT_LOOP		0
#define		LSB$C_FDDI_PHY_BROKEN		1
#define		LSB$C_FDDI_PHY_OFF_READY	2
#define		LSB$C_FDDI_WAITING		3
#define		LSB$C_FDDI_STARTING		4
#define		LSB$C_FDDI_FAILED		5
#define		LSB$C_FDDI_WATCH		6
#define		LSB$C_FDDI_IN_USE		7

#define		LSB$C_FDDI_T_REQ_DFLT		100000
#define		LSB$C_FDDI_TVX_DFLT		32768
#define		LSB$C_FDDI_RTTO_DFLT		12500000
#define		LSB$C_MAX_XPLOR_BUF		6
#define		LSB$C_TR_LENGTH			3008
#define		LSB$C_TR_SPEED_4		0
#define		LSB$C_TR_SPEED_16		1
#define		LSB$C_TR_LNK_OFF		1
#define		LSB$C_TR_LNK_INI		2
#define		LSB$C_TR_LNK_RUN		3
#define		LSB$C_TR_LNK_RCY		4
#define		LSB$C_TR_LNK_CNT		5
#define		LSB$C_TR_LNK_BRK		2
#define		LSB$C_ATM_LENGTH		3000

struct _lsbdef
{
    char lsb$g_efbisr [48];
    char lsb$g_efbipl8 [48];
    char lsb$g_efbreset [48];
    char lsb$g_efbnm [48];
    unsigned int lsb$l_error;
    int lsb$l_extra_l_0;
    unsigned int lsb$l_putrcv;
    unsigned int lsb$l_getrcv;
    unsigned int lsb$l_putxmt;
    unsigned int lsb$l_getxmt;
    unsigned int lsb$l_putcmd;
    unsigned int lsb$l_getcmd;
    unsigned int lsb$l_putuns;
    unsigned int lsb$l_getuns;
    unsigned int lsb$l_putsmt;
    unsigned int lsb$l_getsmt;
    unsigned int lsb$l_xmtavl;
    unsigned int lsb$l_rcvcnt;
    unsigned int lsb$l_xmtcnt;
    unsigned int lsb$l_cmdcnt;
    unsigned int lsb$l_unscnt;
    unsigned int lsb$l_smtcnt;
    void *lsb$a_rcvrng;
    unsigned int lsb$l_rcvent;
    unsigned int lsb$l_rcvsiz;
    void *lsb$a_xmtrng;
    unsigned int lsb$l_xmtent;
    unsigned int lsb$l_xmtsiz;
    void *lsb$a_cmdrng;
    unsigned int lsb$l_cmdent;
    unsigned int lsb$l_cmdsiz;
    void *lsb$a_unsrng;
    unsigned int lsb$l_unsent;
    unsigned int lsb$l_unssiz;
    void *lsb$a_smtrng;
    unsigned int lsb$l_smtent;
    unsigned int lsb$l_smtsiz;
    unsigned int lsb$l_scratch1;
    unsigned int lsb$l_scratch2;
    unsigned int lsb$l_scratch3;
    unsigned int lsb$l_scratch4;
    unsigned int lsb$l_scratch5;
    unsigned int lsb$l_lan_code;
    unsigned int lsb$l_drv_code;
    unsigned int lsb$l_dev1_code;
    unsigned int lsb$l_dev2_code;
    unsigned long long lsb$q_lan_version;
    unsigned long long lsb$q_drv_version;
    unsigned long long lsb$q_dev1_version;
    unsigned long long lsb$q_dev2_version;
    char lsb$t_hw_serial [12];
    unsigned int lsb$l_hw_version;
    unsigned long long lsb$q_hw_bvc;
    char lsb$g_ldc [32];
    unsigned int lsb$l_device;
    unsigned int lsb$l_next_lsb;
    void *lsb$a_baselsb;
    int lsb$l_extra_l_1;
    union
    {
        unsigned int lsb$l_flags;
        struct
        {
            unsigned lsb$v_created		: 1;
            unsigned lsb$v_enabled		: 1;
            unsigned lsb$v_benabled		: 1;
            unsigned lsb$v_nmdis		: 1;
            unsigned lsb$v_elan		: 1;
            unsigned lsb$v_clip		: 1;
            unsigned lsb$v_fill_44_		: 2;
        };
    };
    unsigned int lsb$l_adp;
    unsigned int lsb$l_idb;
    unsigned int lsb$l_ucb0;
    void *lsb$a_eib;
    unsigned int lsb$l_events;
    void *lsb$a_nm_llb;
    void *lsb$a_nm_lhb;
    unsigned long long lsb$q_nm_lpb;
    int lsb$l_nm_snm_len;
    void *lsb$a_nm_snm;
    unsigned int lsb$l_nm_last_port;
    union
    {
        unsigned int lsb$l_nm_flags;
        struct
        {
            unsigned lsb$v_nm_mac_addr	: 1;
            unsigned lsb$v_fill_45_		: 7;
        };
    };
    unsigned int lsb$l_nm_min_tid;
    unsigned int lsb$l_nm_max_tid;
    unsigned long long lsb$q_nm_desc;
    unsigned int lsb$l_station_buf;
    unsigned int lsb$l_nm_scratch;
    void *lsb$a_init_vcib;
    void *lsb$a_allocate_vcrp;
    unsigned int lsb$l_cretim;
    unsigned int lsb$l_deltim;
    unsigned int lsb$l_enatim;
    unsigned int lsb$l_distim;
    unsigned int lsb$l_lstrcvtim;
    unsigned int lsb$l_lstxmttim;
    unsigned int lsb$l_lstftltim;
    unsigned int lsb$l_prvftltim;
    unsigned int lsb$l_lstexctim;
    unsigned int lsb$l_lstcartim;
    unsigned int lsb$l_lstcoltim;
    unsigned int lsb$l_lstcrctim;
    unsigned int lsb$l_lstlentim;
    unsigned int lsb$l_lstusbtim;
    unsigned int lsb$l_lstuubtim;
    unsigned int lsb$l_lstxertim;
    unsigned int lsb$l_lstrertim;
    unsigned int lsb$l_datxmttim;
    unsigned int lsb$l_datrcvtim;
    unsigned int lsb$l_lstrnatim;
    unsigned int lsb$l_lstravtim;
    unsigned int lsb$l_lstriitim;
    unsigned int lsb$l_lstrirtim;
    unsigned int lsb$l_lstrbitim;
    unsigned int lsb$l_lstdattim;
    unsigned int lsb$l_lstdtdtim;
    unsigned int lsb$l_lstrprtim;
    unsigned int lsb$l_lstfcitim;
    unsigned int lsb$l_lsttritim;
    unsigned int lsb$l_lsttrrtim;
    unsigned int lsb$l_lstdbctim;
    unsigned int lsb$l_lstebetim;
    unsigned int lsb$l_lstlcttim;
    unsigned int lsb$l_lstlemtim;
    unsigned int lsb$l_lstlnktim;
    unsigned int lsb$l_lstcnctim;
    unsigned int lsb$l_lsteentim;
    unsigned int lsb$l_lsteditim;
    unsigned int lsb$l_lsteactim;
    unsigned int lsb$l_lsteuntim;
    unsigned int lsb$l_ctltim;
    unsigned int lsb$l_xmttim;
    unsigned int lsb$l_sidtim;
    unsigned int lsb$l_unatim;
    unsigned int lsb$l_usbtim;
    unsigned int lsb$l_usbsav;
    unsigned int lsb$l_rcvmnr;
    unsigned int lsb$l_rcvmxr;
    unsigned int lsb$l_rcvmin;
    unsigned int lsb$l_rcvmax;
    unsigned int lsb$l_rcvcur;
    unsigned int lsb$l_rcvfil;
    unsigned int lsb$l_rcvadd;
    unsigned int lsb$l_crectr;
    unsigned int lsb$l_enactr;
    unsigned int lsb$l_prtctr;
    unsigned int lsb$l_excctr;
    unsigned int lsb$l_ftlctr;
    unsigned int lsb$l_xmttmo;
    unsigned int lsb$l_ctltmo;
    unsigned int lsb$l_rsfctr;
    unsigned int lsb$l_powerf;
    unsigned int lsb$l_xnwctr;
    unsigned int lsb$l_xbpctr;
    unsigned int lsb$l_xgpctr;
    unsigned int lsb$l_xsbctr;
    unsigned int lsb$l_xbactr;
    unsigned int lsb$l_arpctr;
    unsigned int lsb$l_dat_inictr;
    unsigned int lsb$l_dat_errctr;
    unsigned int lsb$l_dat_stage;
    unsigned int lsb$l_dat_vcrp;
    unsigned long long lsb$q_dat_xmtsts;
    unsigned int lsb$l_dat_reqnum;
    void *lsb$a_dat_ucb;
    unsigned int lsb$l_devdepend;
    unsigned int lsb$l_lsterrcsr;
    unsigned int lsb$l_lstftlcod;
    unsigned int lsb$l_prvftlcod;
    unsigned long long lsb$q_lstcrcsrc;
    void *lsb$a_trace;
    unsigned int lsb$l_trclen;
    unsigned long long lsb$q_trcmask;
    unsigned long long lsb$q_trcstop;
    unsigned int lsb$l_trcindex;
    unsigned int lsb$l_trccount;
    unsigned int lsb$l_trcseq;
    unsigned int lsb$l_trcintp;
    unsigned int lsb$l_trcintc;
    void *lsb$a_trcdev;
    unsigned int lsb$l_trcdevlen;
    void *lsb$a_trcmaskdef;
    unsigned long long lsb$q_xmtreq;
    unsigned long long lsb$q_ctlhld;
    unsigned long long lsb$q_ctlreq;
    unsigned long long lsb$q_ctlpnd;
    unsigned long long lsb$q_post;
    unsigned long long lsb$q_delay;
    unsigned long long lsb$q_xmtpnd;
    unsigned long long lsb$q_rcvpnd;
    unsigned long long lsb$q_autors;
    unsigned long long lsb$q_nmhold;
    unsigned long long lsb$q_rcvbuf;
    unsigned int lsb$l_min_chain;
    unsigned int lsb$l_min_xmt;
    unsigned int lsb$l_align_phy;
    unsigned int lsb$l_align_mask;
    unsigned int lsb$l_bsz;
    unsigned int lsb$l_devhdr_len;
    unsigned int lsb$l_svapte;
    unsigned int lsb$l_sva;
    unsigned long long lsb$q_lbuctr;
    unsigned long long lsb$q_zerctr;
    unsigned long long lsb$q_ocrctr;
    unsigned long long lsb$q_octctr;
    unsigned long long lsb$q_pdrctr;
    unsigned long long lsb$q_pdtctr;
    unsigned long long lsb$q_morctr;
    unsigned long long lsb$q_mprctr;
    unsigned long long lsb$q_defctr;
    unsigned long long lsb$q_sinctr;
    unsigned long long lsb$q_mulctr;
    unsigned long long lsb$q_excctr;
    unsigned long long lsb$q_ccfctr;
    unsigned long long lsb$q_shcctr;
    unsigned long long lsb$q_opcctr;
    unsigned long long lsb$q_ttlctr;
    unsigned long long lsb$q_latctr;
    unsigned long long lsb$q_fcectr;
    unsigned long long lsb$q_alictr;
    unsigned long long lsb$q_ftlctr;
    unsigned long long lsb$q_uidctr;
    unsigned long long lsb$q_dorctr;
    unsigned long long lsb$q_usbctr;
    unsigned long long lsb$q_uubctr;
    unsigned long long lsb$q_cdcctr;
    unsigned long long lsb$q_motctr;
    unsigned long long lsb$q_mptctr;
    unsigned long long lsb$q_fsectr;
    unsigned long long lsb$q_slectr;
    unsigned long long lsb$q_rdlctr;
    unsigned long long lsb$q_umdctr;
    unsigned long long lsb$q_stfctr;
    unsigned long long lsb$q_turctr;
    unsigned long long lsb$q_tflctr;
    unsigned long long lsb$q_rsectr;
    unsigned long long lsb$q_rlectr;
    unsigned long long lsb$q_mfcctr;
    unsigned long long lsb$q_mecctr;
    unsigned long long lsb$q_mlcctr;
    unsigned long long lsb$q_riictr;
    unsigned long long lsb$q_rirctr;
    unsigned long long lsb$q_rbictr;
    unsigned long long lsb$q_datctr;
    unsigned long long lsb$q_dtdctr;
    unsigned long long lsb$q_rprctr;
    unsigned long long lsb$q_fcictr;
    unsigned long long lsb$q_trictr;
    unsigned long long lsb$q_trrctr;
    unsigned long long lsb$q_dbcctr;
    unsigned long long lsb$q_ebectr;
    unsigned long long lsb$q_lctctr;
    unsigned long long lsb$q_lemctr;
    unsigned long long lsb$q_lnkctr;
    unsigned long long lsb$q_cncctr;
    unsigned long long lsb$q_berctr;
    unsigned long long lsb$q_raectr;
    unsigned long long lsb$q_lerctr;
    unsigned long long lsb$q_tlfctr;
    unsigned long long lsb$q_fcpctr;
    unsigned long long lsb$q_terctr;
    unsigned long long lsb$q_sigctr;
    unsigned long long lsb$q_rbectr;
    unsigned long long lsb$q_serctr;
    unsigned long long lsb$q_lwfctr;
    unsigned long long lsb$q_arectr;
    unsigned long long lsb$q_rmrctr;
    unsigned long long lsb$q_sstctr;
    unsigned long long lsb$q_rrectr;
    unsigned long long lsb$q_adsctr;
    unsigned long long lsb$q_ierctr;
    unsigned long long lsb$q_zerclr;
    unsigned long long lsb$q_ocrclr;
    unsigned long long lsb$q_octclr;
    unsigned long long lsb$q_pdrclr;
    unsigned long long lsb$q_pdtclr;
    unsigned long long lsb$q_morclr;
    unsigned long long lsb$q_mprclr;
    unsigned long long lsb$q_defclr;
    unsigned long long lsb$q_sinclr;
    unsigned long long lsb$q_mulclr;
    unsigned long long lsb$q_excclr;
    unsigned long long lsb$q_ccfclr;
    unsigned long long lsb$q_shcclr;
    unsigned long long lsb$q_opcclr;
    unsigned long long lsb$q_ttlclr;
    unsigned long long lsb$q_latclr;
    unsigned long long lsb$q_fceclr;
    unsigned long long lsb$q_aliclr;
    unsigned long long lsb$q_ftlclr;
    unsigned long long lsb$q_uidclr;
    unsigned long long lsb$q_dorclr;
    unsigned long long lsb$q_usbclr;
    unsigned long long lsb$q_uubclr;
    unsigned long long lsb$q_cdcclr;
    unsigned long long lsb$q_motclr;
    unsigned long long lsb$q_mptclr;
    unsigned long long lsb$q_fseclr;
    unsigned long long lsb$q_sleclr;
    unsigned long long lsb$q_rdlclr;
    unsigned long long lsb$q_umdclr;
    unsigned long long lsb$q_stfclr;
    unsigned long long lsb$q_turclr;
    unsigned long long lsb$q_tflclr;
    unsigned long long lsb$q_rseclr;
    unsigned long long lsb$q_rleclr;
    unsigned long long lsb$q_mfcclr;
    unsigned long long lsb$q_mecclr;
    unsigned long long lsb$q_mlcclr;
    unsigned long long lsb$q_riiclr;
    unsigned long long lsb$q_rirclr;
    unsigned long long lsb$q_rbiclr;
    unsigned long long lsb$q_datclr;
    unsigned long long lsb$q_dtdclr;
    unsigned long long lsb$q_rprclr;
    unsigned long long lsb$q_fciclr;
    unsigned long long lsb$q_triclr;
    unsigned long long lsb$q_trrclr;
    unsigned long long lsb$q_dbcclr;
    unsigned long long lsb$q_ebeclr;
    unsigned long long lsb$q_lctclr;
    unsigned long long lsb$q_lemclr;
    unsigned long long lsb$q_lnkclr;
    unsigned long long lsb$q_cncclr;
    unsigned long long lsb$q_berclr;
    unsigned long long lsb$q_raeclr;
    unsigned long long lsb$q_lerclr;
    unsigned long long lsb$q_tlfclr;
    unsigned long long lsb$q_fcpclr;
    unsigned long long lsb$q_terclr;
    unsigned long long lsb$q_sigclr;
    unsigned long long lsb$q_rbeclr;
    unsigned long long lsb$q_serclr;
    unsigned long long lsb$q_lwfclr;
    unsigned long long lsb$q_areclr;
    unsigned long long lsb$q_rmrclr;
    unsigned long long lsb$q_sstclr;
    unsigned long long lsb$q_rreclr;
    unsigned long long lsb$q_adsclr;
    unsigned long long lsb$q_ierclr;
    short int lsb$w_extra_w_0;
    unsigned short int lsb$w_zerctr;
    unsigned int lsb$l_brcctr;
    unsigned int lsb$l_bsnctr;
    unsigned int lsb$l_dbrctr;
    unsigned int lsb$l_dbsctr;
    unsigned int lsb$l_mbyctr;
    unsigned int lsb$l_mblctr;
    unsigned int lsb$l_bidctr;
    unsigned int lsb$l_bs1ctr;
    unsigned int lsb$l_bsmctr;
    unsigned short int lsb$w_sflctr;
    union
    {
        unsigned short int lsb$w_sflmap;
        struct
        {
            unsigned lsb$v_sfl_exc		: 1;
            unsigned lsb$v_sfl_ccf		: 1;
            unsigned lsb$v_sfl_shc		: 1;
            unsigned lsb$v_sfl_opc		: 1;
            unsigned lsb$v_sfl_ftl		: 1;
            unsigned lsb$v_sfl_rfd		: 1;
            unsigned lsb$v_sfl_tur		: 1;
            unsigned lsb$v_sfl_tfl		: 1;
        };
    };
    unsigned short int lsb$w_rflctr;
    union
    {
        unsigned short int lsb$w_rflmap;
        struct
        {
            unsigned lsb$v_rfl_bce		: 1;
            unsigned lsb$v_rfl_fme		: 1;
            unsigned lsb$v_rfl_ftl		: 1;
            unsigned lsb$v_rfl_rse		: 1;
            unsigned lsb$v_rfl_rle		: 1;
            unsigned lsb$v_fill_46_		: 3;
        };
    };
    unsigned short int lsb$w_ufdctr;
    unsigned short int lsb$w_ovrctr;
    unsigned short int lsb$w_sbuctr;
    unsigned short int lsb$w_ubuctr;
    unsigned int lsb$l_mbsctr;
    unsigned int lsb$l_msnctr;
    unsigned int lsb$l_mfcctr;
    unsigned int lsb$l_mecctr;
    unsigned int lsb$l_mlcctr;
    unsigned int lsb$l_riictr;
    unsigned int lsb$l_rirctr;
    unsigned int lsb$l_rbictr;
    unsigned int lsb$l_datctr;
    unsigned int lsb$l_dtdctr;
    unsigned int lsb$l_rprctr;
    unsigned int lsb$l_fcictr;
    unsigned int lsb$l_trictr;
    unsigned int lsb$l_trrctr;
    unsigned int lsb$l_dbcctr;
    unsigned int lsb$l_ebectr;
    unsigned int lsb$l_lctctr;
    unsigned int lsb$l_lemctr;
    unsigned int lsb$l_lnkctr;
    unsigned int lsb$l_cncctr;
    unsigned int lsb$l_sfrctr;
    union
    {
        unsigned int lsb$l_trsflmap;
        struct
        {
            unsigned lsb$v_trsfl_tur		: 1;
            unsigned lsb$v_trsfl_le		: 1;
            unsigned lsb$v_trsfl_abs		: 1;
            unsigned lsb$v_trsfl_lf		: 1;
            unsigned lsb$v_trsfl_te		: 1;
            unsigned lsb$v_trsfl_uf		: 1;
            unsigned lsb$v_trsfl_rc		: 1;
            unsigned lsb$v_fill_47_		: 1;
        };
    };
    unsigned int lsb$l_rfrctr;
    union
    {
        unsigned int lsb$l_trrflmap;
        struct
        {
            unsigned lsb$v_trrfl_rc		: 1;
            unsigned lsb$v_trrfl_fce		: 1;
            unsigned lsb$v_fill_48_		: 6;
        };
    };
    unsigned int lsb$l_ifrctr;
    union
    {
        unsigned int lsb$l_triflmap;
        struct
        {
            unsigned lsb$v_trifl_lwf		: 1;
            unsigned lsb$v_trifl_sle		: 1;
            unsigned lsb$v_trifl_tmo		: 1;
            unsigned lsb$v_trifl_rpt		: 1;
            unsigned lsb$v_trifl_bcn		: 1;
            unsigned lsb$v_trifl_dad		: 1;
            unsigned lsb$v_trifl_psf		: 1;
            unsigned lsb$v_trifl_rr		: 1;
        };
    };
    unsigned int lsb$l_rgfctr;
    union
    {
        unsigned int lsb$l_trtflmap;
        struct
        {
            unsigned lsb$v_trtfl_lwf		: 1;
            unsigned lsb$v_trtfl_ssd		: 1;
            unsigned lsb$v_trtfl_arf		: 1;
            unsigned lsb$v_trtfl_rr		: 1;
            unsigned lsb$v_fill_49_		: 4;
        };
    };
    unsigned int lsb$l_rpgctr;
    unsigned int lsb$l_mncctr;
    unsigned int lsb$l_bcnctr;
    unsigned int lsb$l_lerctr;
    unsigned int lsb$l_ierctr;
    unsigned int lsb$l_berctr;
    unsigned int lsb$l_raectr;
    unsigned int lsb$l_adsctr;
    unsigned int lsb$l_piectr;
    unsigned int lsb$l_tlfctr;
    unsigned int lsb$l_rcectr;
    unsigned int lsb$l_fcectr;
    unsigned int lsb$l_ferctr;
    unsigned int lsb$l_terctr;
    unsigned int lsb$l_pnectr;
    unsigned short int lsb$w_cdcctr;
    short int lsb$w_extra_w_1;
    unsigned int lsb$l_chlmsg;
    unsigned int lsb$l_chlerr;
    unsigned int lsb$l_sidmsg;
    unsigned int lsb$l_siderr;
    unsigned int lsb$l_rqcmsg;
    unsigned int lsb$l_rqcerr;
    unsigned int lsb$l_ravctr;
    unsigned int lsb$l_rnactr;
    union
    {
        unsigned int lsb$l_char;
        struct
        {
            unsigned lsb$v_devctr		: 1;
            unsigned lsb$v_devxidtest		: 1;
            unsigned lsb$v_mulseg		: 1;
            unsigned lsb$v_rcvmgt		: 1;
            unsigned lsb$v_fill_50_		: 4;
        };
    };
    union
    {
        unsigned int lsb$l_sts;
        struct
        {
            unsigned lsb$v_run		: 1;
            unsigned lsb$v_inited		: 1;
            unsigned lsb$v_mca_ovf		: 1;
            unsigned lsb$v_fat_err		: 1;
            unsigned lsb$v_blkctl		: 1;
            unsigned lsb$v_pchact		: 1;
            unsigned lsb$v_need_dat_lock	: 1;
            unsigned lsb$v_got_dat_lock	: 1;
            unsigned lsb$v_ring_avail		: 1;
            unsigned lsb$v_full_duplex	: 1;
            unsigned lsb$v_sts_filler		: 22;
        };
    };
    unsigned int lsb$l_mlt;
    unsigned int lsb$l_prm;
    unsigned int lsb$l_prmuser;
    unsigned int lsb$l_media;
    unsigned int lsb$l_speed;
    int lsb$l_extra_l_2;
    unsigned int lsb$l_con;
    unsigned int lsb$l_ilp;
    unsigned int lsb$l_crc;
    int lsb$l_extra_l_3;
    unsigned short int lsb$g_pha [4];
    unsigned short int lsb$g_hwa [4];
    unsigned short int lsb$g_nmpha [4];
    unsigned int lsb$l_line_param;
    unsigned int lsb$l_circuit_param;
    unsigned int lsb$l_untcnt;
    unsigned int lsb$l_gen_multi;
    unsigned int lsb$l_mca_max;
    unsigned int lsb$l_mca_cur;
    unsigned int lsb$l_dev_opr;
    unsigned int lsb$l_fdx_ena;
    unsigned int lsb$l_fdx_opr;
    unsigned int lsb$l_maxfsz;
    unsigned int lsb$l_qos;
    unsigned int lsb$l_reg_hwa;
    unsigned int lsb$l_valid_cnm;
    unsigned int lsb$l_valid_fmt;
    unsigned int lsb$l_valid_gsp;
    unsigned int lsb$l_valid_mca;
    unsigned int lsb$l_valid_src;
    unsigned int lsb$l_valid_pha;
    unsigned int lsb$l_valid_pid;
    unsigned int lsb$l_valid_prm;
    unsigned int lsb$l_valid_pty;
    unsigned int lsb$l_valid_sap;
    unsigned int lsb$l_set_cnm;
    unsigned int lsb$l_set_des;
    unsigned int lsb$l_set_mca;
    unsigned int lsb$l_set_src;
    unsigned int lsb$l_set_pha;
    unsigned int lsb$l_set_mnr;
    unsigned int lsb$l_set_mxr;
    unsigned int lsb$l_set_80ns;
    unsigned int lsb$l_set_rtto;
    unsigned int lsb$l_sho_80ns;
    unsigned int lsb$l_sho_rtto;
    unsigned int lsb$l_sysid_msg;
    unsigned int lsb$l_dev_timer;
    unsigned int lsb$l_process_ctl;
    unsigned int lsb$l_dev_transmit;
    unsigned int lsb$l_sho_lnk;
    void (*lsb$a_fork_proc)();
    void (*lsb$a_chngpro)();
    void (*lsb$a_counter)();
    void (*lsb$a_diag)();
    void (*lsb$a_init)();
    void (*lsb$a_port)();
    void (*lsb$a_power_fail)();
    void (*lsb$a_receive)();
    void (*lsb$a_sense_mac)();
    void (*lsb$a_set_mac)();
    void (*lsb$a_stop)();
    void (*lsb$a_stoppro)();
    void (*lsb$a_strtpro)();
    void (*lsb$a_strtpropha)();
    void (*lsb$a_timer)();
    int (*lsb$a_transmit)();
    void (*lsb$a_unit_init)();
    void (*lsb$a_mask)();
    union
    {
        void *lsb$a_elan_lsbs;
        void *lsb$a_port_lsbs;
        void *lsb$a_clip_lsbs;
    };
    unsigned int lsb$l_internal_ctrs;
    unsigned int lsb$l_internal_ctrsize;
    unsigned int lsb$l_internal_ctrstr;
    unsigned int lsb$l_mon;
    unsigned int lsb$l_monprm;
    void *lsb$a_mon_user1;
    void *lsb$a_mon_user2;
    int lsb$l_extra_l_4;
    unsigned int lsb$l_prm_done;
    unsigned int lsb$l_delivered;
    unsigned int lsb$l_next_802;
    unsigned int lsb$l_rsp_802;
    unsigned int lsb$l_pty_ucb;
    unsigned int lsb$l_pty_last;
    unsigned int lsb$l_sap_ucb;
    unsigned int lsb$l_sap_last;
    unsigned int lsb$l_pid_ucb;
    unsigned long long lsb$q_pid_last;
    long long lsb$q_mbx_base;
    unsigned int lsb$l_mbx_read;
    unsigned int lsb$l_mbx_write;
    unsigned int lsb$l_mbx_num;
    unsigned int lsb$l_mbx_enable;
    unsigned int lsb$g_mbx_array [12];
    union
    {
        struct
        {
            unsigned int lsb$l_csmacd_elan_state_req;
            union
            {
                unsigned int lsb$l_csmacd_elan_state;
                struct
                {
                    unsigned lsb$v_csmacd_elan_active		: 1;
                    unsigned lsb$v_csmacd_elan_start		: 1;
                    unsigned lsb$v_csmacd_elan_shut		: 1;
                    unsigned lsb$v_csmacd_elan_unavailable	: 1;
                    unsigned lsb$v_fill_51_			: 4;
                };
            };
            unsigned int lsb$l_csmacd_event_mask_req;
            unsigned int lsb$l_csmacd_event_mask;
            unsigned int lsb$l_csmacd_ext_sense_req;
            unsigned int lsb$l_csmacd_ext_sense;
            unsigned int lsb$l_csmacd_pvc_req;
            unsigned int lsb$l_csmacd_pvc;
            unsigned int lsb$l_csmacd_pvc_num;
            unsigned int lsb$l_csmacd_max_pktsize;
            unsigned int lsb$l_csmacd_medium;
            char lsb$t_csmacd_elan_name [64];
            char lsb$t_csmacd_elan_desc [64];
            char lsb$t_csmacd_parent_dev [8];
            struct
            {
                unsigned char lsb$b_csmacd_les_prefix [13];
                unsigned char lsb$b_csmacd_les_esi [6];
                unsigned char lsb$b_csmacd_les_sel [1];
            };
            void *lsb$a_csmacd_lec_attr;
            unsigned int lsb$l_csmacd_lec_len;
            unsigned int lsb$l_csmacd_clip_state_req;
            union
            {
                unsigned int lsb$l_csmacd_clip_state;
                struct
                {
                    unsigned lsb$v_csmacd_lis_active	: 1;
                    unsigned lsb$v_csmacd_lis_start	: 1;
                    unsigned lsb$v_csmacd_lis_shut	: 1;
                    unsigned lsb$v_csmacd_lis_unavailable	: 1;
                    unsigned lsb$v_fill_52_		: 4;
                };
            };
            void *lsb$a_csmacd_clip_attr;
            unsigned int lsb$l_csmacd_clip_len;
            unsigned int lsb$l_csmacd_clip_user_type;
            struct
            {
                unsigned char lsb$b_csmacd_clip_prefix [13];
                unsigned char lsb$b_csmacd_clip_esi [6];
                unsigned char lsb$b_csmacd_clip_sel [1];
            };
            struct
            {
                unsigned char lsb$b_csmacd_clip_atm_prefix [13];
                unsigned char lsb$b_csmacd_clip_atm_esi [6];
                unsigned char lsb$b_csmacd_clip_atm_sel [1];
            };
            unsigned char lsb$t_csmacd_clip_proto_addr [6];
            char lsb$b_csmacd_clip_pad1 [2];
            unsigned char lsb$t_csmacd_clip_subnet_mask [6];
            char lsb$b_csmacd_clip_pad2 [2];
            char lsb$t_csmacd_clip_name [64];
            void *lsb$g_clip_pvc;
            unsigned int lsb$l_csmacd_get_pvc;
            unsigned int lsb$l_csmacd_set_pvc;
            unsigned int lsb$l_csmacd_sho_pvc;
            unsigned int lsb$l_reserved;
            void *lsb$a_csmacd_lecs_cfg_handle;
            void *lsb$a_csmacd_lec_cfg_handle;
            void *lsb$a_csmacd_cbrock_handle;
            void *lsb$a_csmacd_elan_handle;
            void *lsb$a_csmacd_lec_iface_handle;
            unsigned int lsb$l_csmacd_reserved_1;
            unsigned int lsb$l_csmacd_reserved_2;
            unsigned int lsb$l_csmacd_reserved_3;
            unsigned int lsb$l_csmacd_reserved_4;
            unsigned int lsb$l_csmacd_reserved_5;
            unsigned int lsb$l_csmacd_reserved_6;
            void *lsb$a_csmacd_clip_handle;
            void *lsb$a_csmacd_clip_cfg_handle;
            void *lsb$a_csmacd_clip_iface_handle;
            unsigned int lsb$l_csmacd_reserved_7;
            unsigned int lsb$l_csmacd_reserved_8;
            unsigned int lsb$l_csmacd_reserved_9;
            unsigned int lsb$l_csmacd_reserved_10;
            unsigned int lsb$l_csmacd_reserved_11;
            unsigned int lsb$l_csmacd_reserved_12;
        };
        struct
        {
            unsigned int lsb$l_fddi_dlver;
            unsigned int lsb$l_fddi_t_max;
            unsigned int lsb$l_fddi_t_neg;
            unsigned int lsb$l_fddi_t_req;
            unsigned int lsb$l_fddi_tvx;
            unsigned int lsb$l_fddi_rtto;
            unsigned int lsb$l_fddi_rpe;
            unsigned int lsb$l_fddi_lem;
            unsigned int lsb$l_fddi_rer;
            unsigned int lsb$l_fddi_rjr;
            unsigned int lsb$l_fddi_lee;
            unsigned int lsb$l_fddi_una_dat;
            unsigned short int lsb$g_fddi_una [4];
            unsigned short int lsb$g_fddi_old_una [4];
            unsigned short int lsb$g_fddi_sif_conf_targ [4];
            unsigned short int lsb$g_fddi_sif_op_targ [4];
            unsigned short int lsb$g_fddi_nif_targ [4];
            unsigned short int lsb$g_fddi_echo_targ [4];
            unsigned short int lsb$g_fddi_dna [4];
            unsigned short int lsb$g_fddi_old_dna [4];
            unsigned int lsb$l_fddi_echo_dat;
            unsigned int lsb$l_fddi_echo_len;
            unsigned int lsb$l_fddi_phy_state;
            unsigned int lsb$l_fddi_link_state;
            unsigned int lsb$l_fddi_rp_state;
            unsigned int lsb$l_fddi_port_type;
            unsigned int lsb$l_fddi_nbr_phy;
            unsigned int lsb$l_fddi_dat;
            union
            {
                unsigned int lsb$l_fddi_flags;
                struct
                {
                    unsigned lsb$v_fddi_boo		: 1;
                    unsigned lsb$v_fddi_cty		: 1;
                    unsigned lsb$v_fddi_unto		: 1;
                    unsigned lsb$v_fill_53_		: 5;
                };
            };
            unsigned int lsb$l_fddi_smt;
            unsigned int lsb$l_fddi_smt_prm;
            unsigned int lsb$l_fddi_smt_type;
        };
        struct
        {
            int lsb$l_extra_l_5;
            unsigned int lsb$l_tr_speed;
            unsigned int lsb$l_tr_etr;
            unsigned int lsb$l_tr_moncon;
            unsigned long long lsb$q_tr_naun;
            unsigned int lsb$l_tr_rer;
            unsigned int lsb$l_tr_ier;
            unsigned int lsb$l_tr_pdn;
            unsigned int lsb$l_tr_rgn;
            unsigned int lsb$l_tr_maxac;
            unsigned int lsb$l_tr_fcfield;
            unsigned int lsb$l_tr_srcatmo;
            unsigned int lsb$l_tr_srdtmo;
            unsigned int lsb$l_tr_sr;
            void *lsb$a_tr_sr_tab;
            unsigned int lsb$l_tr_sr_size;
            unsigned int lsb$l_tr_sr_tabsiz;
            unsigned int lsb$l_tr_sr_tabbyte;
            unsigned int lsb$l_tr_sr_inuse;
            unsigned int lsb$l_tr_sr_garbage;
            unsigned int lsb$l_tr_sr_deleted;
            unsigned long long lsb$q_tr_sr_xplorq;
            unsigned int lsb$l_tr_sr_xplorb;
            unsigned int lsb$l_tr_sr_xplorc;
            void *lsb$a_tr_fca;
            unsigned int lsb$l_tr_fca_used;
            unsigned int lsb$l_tr_fcmask;
            void *lsb$a_tr_hdr;
            unsigned int lsb$l_tr_hdrsize;
            unsigned int lsb$l_tr_lnkstate;
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
            unsigned int lsb$l_atm_tqe_alloc;
            unsigned int lsb$l_atm_tqe_used;
            unsigned int lsb$l_atm_mem_alloc;
            unsigned int lsb$l_atm_mem_used;
            unsigned int lsb$l_atm_hwanum;
            unsigned int lsb$l_atm_hwanum_used;
            void *lsb$a_atm_cd_handle;
            void *lsb$a_atm_cd_callbacks;
            void *lsb$a_atm_module_profilep;
            unsigned int lsb$l_atm_max_vc;
            unsigned int lsb$l_atm_reserved_1;
            unsigned int lsb$l_atm_reserved_2;
            unsigned int lsb$l_atm_reserved_3;
            unsigned int lsb$l_atm_reserved_4;
            unsigned int lsb$l_atm_reserved_5;
        };
    };
};

#define		SR$C_LOCAL		0
#define		SR$C_UNKNOWN		2
#define		SR$C_KNOWN		4
#define		SR$C_STALE		1
#define		SR$C_DELETED		3
#define		SR$C_EXPLORING		5
#define		SR$C_SR_SIZE		64
#define		SR$C_SR_SHIFT		6

struct _sr
{
    unsigned long long sr$g_addr;
    unsigned int sr$l_state;
    unsigned int sr$l_ri_s;
    unsigned char sr$g_ri [32];
    unsigned int sr$l_lstxmttim;
    unsigned int sr$l_lstrcvtim;
    unsigned int sr$l_staletim;
    unsigned int sr$l_discvtim;
};

#define		FCA$C_FCA_ENTRIES	200
#define		FCA$C_FCA_SIZE		16

struct _fca
{
    unsigned long long fca$g_addr;
    unsigned int fca$l_fc_mask;
    unsigned int fca$l_fc_fill;
};

#define		TRC$Q_PK_DATA		0
#define		TRC$G_PK_DATA		12
#define		TRC$C_INIT_ENTRIES	256
#define		TRC$C_DEF_ENTRIES	2048
#define		TRC$C_NUM_INT		8
#define		TRC$C_LENGTH		32

#define		TRC$C_EXTENSION		128

#define		TRC$C_TIMER		1
#define		TRC$C_INTR		2
#define		TRC$C_FRK_START		3
#define		TRC$C_FRK_DONE		4
#define		TRC$C_FRK_ERROR		5
#define		TRC$C_FRK_SOFT		6
#define		TRC$C_STATE		7
#define		TRC$C_INIT		8
#define		TRC$C_CHNGPRO		9
#define		TRC$C_STOP		10
#define		TRC$C_SHUTDOWN		11
#define		TRC$C_RCV_ISS		12
#define		TRC$C_RCV_DONE		13
#define		TRC$C_RCV_ERR		14
#define		TRC$C_RCV_PKT		15
#define		TRC$C_XMT_ISS		16
#define		TRC$C_XMT_QUE		17
#define		TRC$C_XMT_DONE		18
#define		TRC$C_XMT_ERR		19
#define		TRC$C_XMT_PKT		20
#define		TRC$C_SEGMENT		21
#define		TRC$C_SR_ENTRY		22
#define		TRC$C_VCI_ACTION	23
#define		TRC$C_LAN24		24
#define		TRC$C_LAN25		25
#define		TRC$C_LAN26		26
#define		TRC$C_LAN27		27
#define		TRC$C_LAN28		28
#define		TRC$C_LAN29		29
#define		TRC$C_LAN30		30
#define		TRC$C_LAN31		31
#define		TRC$C_LAN32		32

struct _trc
{
    unsigned long long trc$q_time;
    unsigned int trc$l_sequence;
    unsigned char trc$b_type;
    unsigned char trc$b_xmtcnt;
    unsigned char trc$b_rcvcnt;
    unsigned char trc$b_misc;
    unsigned int trc$l_param1;
    unsigned int trc$l_param2;
    unsigned int trc$l_param3;
    unsigned int trc$l_param4;
};

#endif

