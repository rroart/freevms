#ifndef xmdef_h
#define xmdef_h

#define		XM$M_CHR_MOP		0x1
#define		XM$M_CHR_LOOPB		0x2
#define		XM$M_CHR_HDPLX		0x4
#define		XM$M_CHR_SLAVE		0x8
#define		XM$M_CHR_MBX		0x10
#define		XM$M_CHR_DMC		0x20
#define		XM$M_CHR_CTRL		0x40
#define		XM$M_CHR_TRIB		0x80
#define		XM$M_STS_DCHK		0x100
#define		XM$M_STS_TIMO		0x200
#define		XM$M_STS_ORUN		0x400
#define		XM$M_STS_ACTIVE		0x800
#define		XM$M_STS_BUFFAIL	0x1000
#define		XM$M_STS_RUNNING	0x2000
#define		XM$M_STS_DISC		0x4000
#define		XM$M_ERR_FATAL		0x10000
#define		XM$M_ERR_MAINT		0x80000
#define		XM$M_ERR_LOST		0x100000
#define		XM$M_ERR_THRESH		0x200000
#define		XM$M_ERR_TRIB		0x400000
#define		XM$M_ERR_START		0x800000
#define		XM$M_MDM_RTSHLD		0x1
#define		XM$M_MDM_STNDBY		0x2
#define		XM$M_MDM_MAINT2		0x4
#define		XM$M_MDM_MAINT1		0x8
#define		XM$M_MDM_FREQ		0x20
#define		XM$M_MDM_RDY		0x40
#define		XM$M_MDM_POLL		0x80
#define		XM$M_MDM_SELM		0x100
#define		XM$M_MDM_INT		0x800
#define		XM$M_MDM_V35		0x1000
#define		XM$M_MDM_RS232		0x4000
#define		XM$M_MDM_RS422		0x8000
#define		XM$M_MDM_CARRDET	0x1
#define		XM$M_MDM_MSTNDBY	0x2
#define		XM$M_MDM_CTS		0x4
#define		XM$M_MDM_DSR		0x8
#define		XM$M_MDM_HDX		0x10
#define		XM$M_MDM_RTS		0x20
#define		XM$M_MDM_DTR		0x40
#define		XM$M_MDM_RING		0x80
#define		XM$M_MDM_CHRMOD		0x100
#define		XM$M_MDM_MCLOCK		0x200
#define		XM$M_MDM_MODTEST	0x400
#define		XM$M_MDM_SIGQUAL	0x4000
#define		XM$M_MDM_SIGRATE	0x8000
	
struct _xmdev {
  unsigned char xmdev$b_p1_class;
  unsigned char xmdev$b_p1_type;
  unsigned short int xmdev$w_p1_mms;
  unsigned char xmdev$b_p1_char;
  unsigned char xmdev$b_p1_sts;
  unsigned char xmdev$b_p1_esum;
  unsigned char xmdev$b_p1_tpi;
};

union _xmdef {
  struct  {
    unsigned xm$v_chr_mop		: 1;
    unsigned xm$v_chr_loopb		: 1;
    unsigned xm$v_chr_hdplx		: 1;
    unsigned xm$v_chr_slave		: 1;
    unsigned xm$v_chr_mbx		: 1;
    unsigned xm$v_chr_dmc		: 1;
    unsigned xm$v_chr_ctrl		: 1;
    unsigned xm$v_chr_trib		: 1;
    unsigned xm$v_sts_dchk		: 1;
    unsigned xm$v_sts_timo		: 1;
    unsigned xm$v_sts_orun		: 1;
    unsigned xm$v_sts_active		: 1;
    unsigned xm$v_sts_buffail		: 1;
    unsigned xm$v_sts_running		: 1;
    unsigned xm$v_sts_disc		: 1;
    unsigned xmdef$$_fill_1		: 1;
    unsigned xm$v_err_fatal		: 1;
    unsigned xmdef$$_fill_2		: 2;
    unsigned xm$v_err_maint		: 1;
    unsigned xm$v_err_lost		: 1;
    unsigned xm$v_err_thresh		: 1;
    unsigned xm$v_err_trib		: 1;
    unsigned xm$v_err_start		: 1;
  };
  struct  {
    unsigned xm$v_mdm_rtshld		: 1;
    unsigned xm$v_mdm_stndby		: 1;
    unsigned xm$v_mdm_maint2		: 1;
    unsigned xm$v_mdm_maint1		: 1;
    unsigned xmdef$$_fill_3		: 1;
    unsigned xm$v_mdm_freq		: 1;
    unsigned xm$v_mdm_rdy		: 1;
    unsigned xm$v_mdm_poll		: 1;
    unsigned xm$v_mdm_selm		: 1;
    unsigned xmdef$$_fill_4		: 2;
    unsigned xm$v_mdm_int		: 1;
    unsigned xm$v_mdm_v35		: 1;
    unsigned xmdef$$_fill_5		: 1;
    unsigned xm$v_mdm_rs232		: 1;
    unsigned xm$v_mdm_rs422		: 1;
  };
  struct  {
    unsigned xm$v_mdm_carrdet		: 1;
    unsigned xm$v_mdm_mstndby		: 1;
    unsigned xm$v_mdm_cts		: 1;
    unsigned xm$v_mdm_dsr		: 1;
    unsigned xm$v_mdm_hdx		: 1;
    unsigned xm$v_mdm_rts		: 1;
    unsigned xm$v_mdm_dtr		: 1;
    unsigned xm$v_mdm_ring		: 1;
    unsigned xm$v_mdm_chrmod		: 1;
    unsigned xm$v_mdm_mclock		: 1;
    unsigned xm$v_mdm_modtest		: 1;
    unsigned xmdef$$_fill_6		: 3;
    unsigned xm$v_mdm_sigqual		: 1;
    unsigned xm$v_mdm_sigrate		: 1;
  };
};
 
#endif
 
