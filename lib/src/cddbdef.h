#ifndef cddbdef_h
#define cddbdef_h

#define		CDDB$M_SNGLSTRM		0x1
#define		CDDB$M_IMPEND		0x2
#define		CDDB$M_INITING		0x4
#define		CDDB$M_RECONNECT	0x8
#define		CDDB$M_RESYNCH		0x10
#define		CDDB$M_POLLING		0x20
#define		CDDB$M_ALCLS_SET	0x40
#define		CDDB$M_NOCONN		0x80
#define		CDDB$M_RSTRTWAIT	0x100
#define		CDDB$M_QUORLOST		0x200
#define		CDDB$M_DAPBSY		0x400
#define		CDDB$M_2PBSY		0x800
#define		CDDB$M_BSHADOW		0x1000
#define		CDDB$M_DISABLED		0x2000
#define		CDDB$M_PATHMOVE		0x4000
#define		CDDB$M_PRMBSY		0x8000
#define		CDDB$M_DISC_PEND	0x10000
#define		CDDB$M_CRNSET		0x20000
#define		CDDB$M_FIRM_WREV	0x40000
#define		CDDB$M_PRMBSY_CLEANUP_PERMITTED		0x80000
#define		CDDB$K_LENGTH		152
#define		CDDB$C_LENGTH		152

#define		CDDB$S_CDDBDEF		152

struct _cddb {
  struct _cdrp *cddb$l_cdrpqfl;
  struct _cdrp *cddb$l_cdrpqbl;
  unsigned short int cddb$w_size;
  unsigned char cddb$b_type;
  unsigned char cddb$b_subtype;
  unsigned char cddb$b_systemid [8];
  union  {
    unsigned int cddb$l_status;
    struct  {
      unsigned cddb$v_snglstrm	: 1;
      unsigned cddb$v_impend	: 1;
      unsigned cddb$v_initing	: 1;
      unsigned cddb$v_reconnect	: 1;
      unsigned cddb$v_resynch	: 1;
      unsigned cddb$v_polling	: 1;
      unsigned cddb$v_alcls_set	: 1;
      unsigned cddb$v_noconn	: 1;
      unsigned cddb$v_rstrtwait	: 1;
      unsigned cddb$v_quorlost	: 1;
      unsigned cddb$v_dapbsy	: 1;
      unsigned cddb$v_2pbsy	: 1;
      unsigned cddb$v_bshadow	: 1;
      unsigned cddb$v_disabled	: 1;
      unsigned cddb$v_pathmove	: 1;
      unsigned cddb$v_prmbsy	: 1;
      unsigned cddb$v_disc_pend	: 1;
      unsigned cddb$v_crnset	: 1;
      unsigned cddb$v_firm_wrev : 1;
      unsigned cddb$v_prmbsy_cleanup_permitted : 1;
      unsigned cddb$v_fill_0_	: 4;
    };
  };
  struct _pdt *cddb$l_pdt;
  struct _crb *cddb$l_crb;
  struct _ddb *cddb$l_ddb;
  union  {
    unsigned long long cddb$q_cntrlid;
    struct  {
      char cddbdef$$_fill_2 [6];
      unsigned char cddb$b_cntrlmdl;
      unsigned char cddb$b_cntrlcls;
    };
  };
  unsigned short int cddb$w_cntrlflgs;
  unsigned short int cddb$w_cntrltmo;
  unsigned int cddb$l_oldrspid;
  unsigned int cddb$l_oldcmdsts;
  struct _cdrp *cddb$l_rstrtcdrp;
  unsigned int cddb$l_retrycnt;
  unsigned int cddb$l_dapcount;
  unsigned int cddb$l_rstrtcnt;
  struct _cdrp *cddb$l_rstrtqfl;
  struct _cdrp *cddb$l_rstrtqbl;
  void (*cddb$l_saved_pc)();
  void (*cddb$l_saved_pc1)();
  struct _ucb *cddb$l_ucbchain;
  struct _ucb *cddb$l_origucb;
  unsigned int cddb$l_allocls;
  struct _cdrp *cddb$l_dapcdrp;
  struct _cddb *cddb$l_cddblink;
  unsigned int cddb$l_fover_ctr;
  unsigned int cddb$l_wtucbctr;
  unsigned int cddb$l_maxbcnt;
  unsigned int cddb$l_ctrltr_mask;
  unsigned int cddb$l_cpyseqnum;
  unsigned int cddb$l_dap_limit;
  unsigned char cddb$b_csvrsn;
  unsigned char cddb$b_chvrsn;
  unsigned short int cddb$w_load_avail;
  unsigned int cddb$l_counter;
  unsigned int cddb$l_conid;
  unsigned int cddb$l_cdt;
  struct _cdrp *cddb$l_prmcdrp;
};
 
#endif
 
