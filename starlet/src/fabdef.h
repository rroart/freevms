#ifndef fabdef_h
#define fabdef_h

#define		FAB$C_BID		3
#define		FAB$M_PPF_RAT		0x3FC0
#define		FAB$M_PPF_IND		0x4000
#define		FAB$M_PPIFI		0x8000
#define		FAB$M_ASY		0x1
#define		FAB$M_MXV		0x2
#define		FAB$M_SUP		0x4
#define		FAB$M_TMP		0x8
#define		FAB$M_TMD		0x10
#define		FAB$M_DFW		0x20
#define		FAB$M_SQO		0x40
#define		FAB$M_RWO		0x80
#define		FAB$M_POS		0x100
#define		FAB$M_WCK		0x200
#define		FAB$M_NEF		0x400
#define		FAB$M_RWC		0x800
#define		FAB$M_DMO		0x1000
#define		FAB$M_SPL		0x2000
#define		FAB$M_SCF		0x4000
#define		FAB$M_DLT		0x8000
#define		FAB$M_NFS		0x10000
#define		FAB$M_UFO		0x20000
#define		FAB$M_PPF		0x40000
#define		FAB$M_INP		0x80000
#define		FAB$M_CTG		0x100000
#define		FAB$M_CBT		0x200000
#define		FAB$M_SYNCSTS		0x400000
#define		FAB$M_RCK		0x800000
#define		FAB$M_NAM		0x1000000
#define		FAB$M_CIF		0x2000000
#define		FAB$M_ESC		0x8000000
#define		FAB$M_TEF		0x10000000
#define		FAB$M_OFP		0x20000000
#define		FAB$M_KFO		0x40000000
#define		FAB$M_PUT		0x1
#define		FAB$M_GET		0x2
#define		FAB$M_DEL		0x4
#define		FAB$M_UPD		0x8
#define		FAB$M_TRN		0x10
#define		FAB$M_BIO		0x20
#define		FAB$M_BRO		0x40
#define		FAB$M_EXE		0x80
#define		FAB$M_SHRPUT		0x1
#define		FAB$M_SHRGET		0x2
#define		FAB$M_SHRDEL		0x4
#define		FAB$M_SHRUPD		0x8
#define		FAB$M_MSE		0x10
#define		FAB$M_NIL		0x20
#define		FAB$M_UPI		0x40
#define		FAB$M_NQL		0x80
#define		FAB$M_ORG		0xF0
#define		FAB$C_SEQ		0
#define		FAB$C_REL		16
#define		FAB$C_IDX		32
#define		FAB$C_HSH		48
#define		FAB$M_FTN		0x1
#define		FAB$M_CR		0x2
#define		FAB$M_PRN		0x4
#define		FAB$M_BLK		0x8
#define		FAB$M_MSB		0x10
#define		FAB$C_RFM_DFLT		2
#define		FAB$C_UDF		0
#define		FAB$C_FIX		1
#define		FAB$C_VAR		2
#define		FAB$C_VFC		3
#define		FAB$C_STM		4
#define		FAB$C_STMLF		5
#define		FAB$C_STMCR		6
#define		FAB$C_MAXRFM		6
#define		FAB$M_ONLY_RU		0x1
#define		FAB$M_RU		0x2
#define		FAB$M_BI		0x4
#define		FAB$M_AI		0x8
#define		FAB$M_AT		0x10
#define		FAB$M_NEVER_RU		0x20
#define		FAB$M_JOURNAL_FILE		0x40
#define		FAB$M_RCF_RU		0x1
#define		FAB$M_RCF_AI		0x2
#define		FAB$M_RCF_BI		0x4
	
struct _fabdef {
  unsigned char fab$b_bid;
  unsigned char fab$b_bln;
  union  {
    unsigned short int fab$w_ifi;
    struct  {
      unsigned fabdef$$_fill_1		: 6;
      unsigned fab$v_ppf_rat		: 8;
      unsigned fab$v_ppf_ind		: 1;
      unsigned fab$v_ppifi		: 1;
    };
  };
  union  {
    unsigned int fab$l_fop;
    struct  {
      unsigned fab$v_asy		: 1;
      unsigned fab$v_mxv		: 1;
      unsigned fab$v_sup		: 1;
      unsigned fab$v_tmp		: 1;
      unsigned fab$v_tmd		: 1;
      unsigned fab$v_dfw		: 1;
      unsigned fab$v_sqo		: 1;
      unsigned fab$v_rwo		: 1;
      unsigned fab$v_pos		: 1;
      unsigned fab$v_wck		: 1;
      unsigned fab$v_nef		: 1;
      unsigned fab$v_rwc		: 1;
      unsigned fab$v_dmo		: 1;
      unsigned fab$v_spl		: 1;
      unsigned fab$v_scf		: 1;
      unsigned fab$v_dlt		: 1;
      unsigned fab$v_nfs		: 1;
      unsigned fab$v_ufo		: 1;
      unsigned fab$v_ppf		: 1;
      unsigned fab$v_inp		: 1;
      unsigned fab$v_ctg		: 1;
      unsigned fab$v_cbt		: 1;
      unsigned fab$v_syncsts		: 1;
      unsigned fab$v_rck		: 1;
      unsigned fab$v_nam		: 1;
      unsigned fab$v_cif		: 1;
      unsigned fabdef$$_fill_3		: 1;
      unsigned fab$v_esc		: 1;
      unsigned fab$v_tef		: 1;
      unsigned fab$v_ofp		: 1;
      unsigned fab$v_kfo		: 1;
      unsigned fabdef$$_fill_4		: 1;
    };
  };
  unsigned int fab$l_sts;
  unsigned int fab$l_stv;
  unsigned int fab$l_alq;
  unsigned short int fab$w_deq;
  union  {
    unsigned char fab$b_fac;
    struct  {
      unsigned fab$v_put		: 1;
      unsigned fab$v_get		: 1;
      unsigned fab$v_del		: 1;
      unsigned fab$v_upd		: 1;
      unsigned fab$v_trn		: 1;
      unsigned fab$v_bio		: 1;
      unsigned fab$v_bro		: 1;
      unsigned fab$v_exe		: 1;
    };
  };
  union  {
    unsigned char fab$b_shr;
    struct  {
      unsigned fab$v_shrput		: 1;
      unsigned fab$v_shrget		: 1;
      unsigned fab$v_shrdel		: 1;
      unsigned fab$v_shrupd		: 1;
      unsigned fab$v_mse		: 1;
      unsigned fab$v_nil		: 1;
      unsigned fab$v_upi		: 1;
      unsigned fab$v_nql		: 1;
    };
  };
  unsigned int fab$l_ctx;
  char fab$b_rtv;
  union  {
    unsigned char fab$b_org;
    struct  {
      unsigned fabdef$$_fill_5		: 4;
      unsigned fab$v_org		: 4;
    };
  };
  union  {
    unsigned char fab$b_rat;
    struct  {
      unsigned fab$v_ftn	: 1;
      unsigned fab$v_cr		: 1;
      unsigned fab$v_prn	: 1;
      unsigned fab$v_blk	: 1;
      unsigned fab$v_msb	: 1;
      unsigned fab$v_fill_0_	: 3;
    };
  };
  unsigned char fab$b_rfm;
  union  {
    unsigned int fab$l_jnl;
    struct  {
      union  {
	unsigned char fab$b_journal;
	struct  {
	  unsigned fab$v_only_ru	: 1;
	  unsigned fab$v_ru		: 1;
	  unsigned fab$v_bi		: 1;
	  unsigned fab$v_ai		: 1;
	  unsigned fab$v_at		: 1;
	  unsigned fab$v_never_ru	: 1;
	  unsigned fab$v_journal_file	: 1;
	  unsigned fab$v_fill_1_	: 1;
	};
      };
      unsigned char fab$b_ru_facility;
      short int fabdef$$_fill_7;
    };
  };
  void *fab$l_xab;
  union  {
    struct _namdef *fab$l_nam;
    struct _namldef *fab$l_naml;
  };
  char *fab$l_fna;
  char *fab$l_dna;
  unsigned char fab$b_fns;
  unsigned char fab$b_dns;
  unsigned short int fab$w_mrs;
  int fab$l_mrn;
  unsigned short int fab$w_bls;
  unsigned char fab$b_bks;
  unsigned char fab$b_fsz;
  unsigned int fab$l_dev;
  unsigned int fab$l_sdc;
  unsigned short int fab$w_gbc;
  union  {
    unsigned char fab$b_acmodes;
    struct  {
      unsigned fab$v_lnm_mode		: 2;
      unsigned fab$v_chan_mode		: 2;
      unsigned fab$v_file_mode		: 2;
      unsigned fab$v_callers_mode	: 2;
    };
  };
  union  {
    unsigned char fab$b_rcf;
    struct  {
      unsigned fab$v_rcf_ru		: 1;
      unsigned fab$v_rcf_ai		: 1;
      unsigned fab$v_rcf_bi		: 1;
      unsigned fab$v_fill_2_		: 5;
    };
  };
  unsigned int fabdef$$_fill_9;
};
 
#define FAB$K_BLN 80
#define FAB$C_BLN 80
	
#define FAB$V_PPF_RAT	6
#define FAB$S_PPF_RAT	8
#define FAB$V_PPF_IND	14
#define FAB$V_ASY	0
#define FAB$V_MXV	1
#define FAB$V_SUP	2
#define FAB$V_TMP	3
#define FAB$V_TMD	4
#define FAB$V_DFW	5
#define FAB$V_SQO	6
#define FAB$V_RWO	7
#define FAB$V_POS	8
#define FAB$V_WCK	9
#define FAB$V_NEF	10
#define FAB$V_RWC	11
#define FAB$V_DMO	12
#define FAB$V_SPL	13
#define FAB$V_SCF	14
#define FAB$V_DLT	15
#define FAB$V_NFS	16
#define FAB$V_UFO	17
#define FAB$V_PPF	18
#define FAB$V_INP	19
#define FAB$V_CTG	20
#define FAB$V_CBT	21
#define FAB$V_SYNCSTS	22
#define FAB$V_RCK	23
#define FAB$V_NAM	24
#define FAB$V_CIF	25
#define FAB$V_ESC	27
#define FAB$V_TEF	28
#define FAB$V_OFP	29
#define FAB$V_KFO	30
#define FAB$V_PUT	0
#define FAB$V_GET	1
#define FAB$V_DEL	2
#define FAB$V_UPD	3
#define FAB$V_TRN	4
#define FAB$V_BIO	5
#define FAB$V_BRO	6
#define FAB$V_EXE	7
#define FAB$V_SHRPUT	0
#define FAB$V_SHRGET	1
#define FAB$V_SHRDEL	2
#define FAB$V_SHRUPD	3
#define FAB$V_MSE	4
#define FAB$V_NIL	5
#define FAB$V_UPI	6
#define FAB$V_ORG	4
#define FAB$S_ORG	4
#define FAB$V_FTN	0
#define FAB$V_CR	1
#define FAB$V_PRN	2
#define FAB$V_BLK	3
#define FAB$V_MSB	4
#define fab$b_dsbmsk	fab$b_acmodes
#define FAB$S_LNM_MODE	2
#define FAB$V_LNM_MODE	0
#define FAB$S_CHAN_MODE	2
#define FAB$V_CHAN_MODE	2
#define FAB$S_FILE_MODE	2
#define FAB$V_FILE_MODE	4

#define FAB$V_RU	1
#define FAB$V_BI	2
#define FAB$V_AI	3

extern struct _fabdef cc$rms_fab;

#endif
 
