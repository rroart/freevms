#ifndef rabdef_h
#define rabdef_h

#define		RAB$C_BID		1
#define		RAB$M_PPF_RAT		0x3FC0
#define		RAB$M_PPF_IND		0x4000
#define		RAB$M_PPISI		0x8000
#define		RAB$M_ASY		0x1
#define		RAB$M_TPT		0x2
#define		RAB$M_REA		0x4
#define		RAB$M_RRL		0x8
#define		RAB$M_UIF		0x10
#define		RAB$M_MAS		0x20
#define		RAB$M_FDL		0x40
#define		RAB$M_REV		0x80
#define		RAB$M_EOF		0x100
#define		RAB$M_RAH		0x200
#define		RAB$M_WBH		0x400
#define		RAB$M_BIO		0x800
#define		RAB$M_CDK		0x1000
#define		RAB$M_LOA		0x2000
#define		RAB$M_LIM		0x4000
#define		RAB$M_SYNCSTS		0x8000
#define		RAB$M_LOC		0x10000
#define		RAB$M_WAT		0x20000
#define		RAB$M_ULK		0x40000
#define		RAB$M_RLK		0x80000
#define		RAB$M_NLK		0x100000
#define		RAB$M_KGE		0x200000
#define		RAB$M_KGT		0x400000
#define		RAB$M_NXR		0x800000
#define		RAB$M_RNE		0x1000000
#define		RAB$M_TMO		0x2000000
#define		RAB$M_CVT		0x4000000
#define		RAB$M_RNF		0x8000000
#define		RAB$M_ETO		0x10000000
#define		RAB$M_PTA		0x20000000
#define		RAB$M_PMT		0x40000000
#define		RAB$M_CCO		0x80000000
#define		RAB$M_EQNXT		0x200000
#define		RAB$M_NXT		0x400000
#define		RAB$M_NQL		0x1
#define		RAB$M_NODLCKWT		0x2
#define		RAB$M_NODLCKBLK		0x4
#define		RAB$C_SEQ		0
#define		RAB$C_KEY		1
#define		RAB$C_RFA		2
#define		RAB$C_STM		3
#define		RAB$C_MAXRAC		2
	
#define RAB$K_BLN 68
#define RAB$C_BLN 68
	
#define RAB$V_PPF_RAT	6
#define RAB$S_PPF_RAT	8
#define RAB$V_PPF_IND	14
#define RAB$V_ASY	0
#define RAB$V_TPT	1
#define RAB$V_REA	2
#define RAB$V_RRL	3
#define RAB$V_UIF	4
#define RAB$V_MAS	5
#define RAB$V_FDL	6
#define RAB$V_REV	7
#define RAB$V_EOF	8
#define RAB$V_RAH	9
#define RAB$V_WBH	10
#define RAB$V_BIO	11
#define RAB$V_CDK	12
#define RAB$V_LOA	13
#define RAB$V_LIM	14
#define RAB$V_SYNCSTS	15
#define RAB$V_LOC	16
#define RAB$V_WAT	17
#define RAB$V_ULK	18
#define RAB$V_RLK	19
#define RAB$V_NLK	20
#define RAB$V_KGE	21
#define RAB$V_KGT	22
#define RAB$V_NXR	23
#define RAB$V_RNE	24
#define RAB$V_TMO	25
#define RAB$V_CVT	26
#define RAB$V_RNF	27
#define RAB$V_ETO	28
#define RAB$V_PTA	29
#define RAB$V_PMT	30
#define RAB$V_CCO	31
#define RAB$V_EQNXT 	21
#define RAB$V_NXT  	22

#define		RAB64$C_BID		1
#define		RAB64$M_PPF_RAT		0x3FC0
#define		RAB64$M_PPF_IND		0x4000
#define		RAB64$M_PPISI		0x8000
#define		RAB64$M_ASY		0x1
#define		RAB64$M_TPT		0x2
#define		RAB64$M_REA		0x4
#define		RAB64$M_RRL		0x8
#define		RAB64$M_UIF		0x10
#define		RAB64$M_MAS		0x20
#define		RAB64$M_FDL		0x40
#define		RAB64$M_REV		0x80
#define		RAB64$M_EOF		0x100
#define		RAB64$M_RAH		0x200
#define		RAB64$M_WBH		0x400
#define		RAB64$M_BIO		0x800
#define		RAB64$M_CDK		0x1000
#define		RAB64$M_LOA		0x2000
#define		RAB64$M_LIM		0x4000
#define		RAB64$M_SYNCSTS		0x8000
#define		RAB64$M_LOC		0x10000
#define		RAB64$M_WAT		0x20000
#define		RAB64$M_ULK		0x40000
#define		RAB64$M_RLK		0x80000
#define		RAB64$M_NLK		0x100000
#define		RAB64$M_KGE		0x200000
#define		RAB64$M_KGT		0x400000
#define		RAB64$M_NXR		0x800000
#define		RAB64$M_RNE		0x1000000
#define		RAB64$M_TMO		0x2000000
#define		RAB64$M_CVT		0x4000000
#define		RAB64$M_RNF		0x8000000
#define		RAB64$M_ETO		0x10000000
#define		RAB64$M_PTA		0x20000000
#define		RAB64$M_PMT		0x40000000
#define		RAB64$M_CCO		0x80000000
#define		RAB64$M_EQNXT		0x200000
#define		RAB64$M_NXT		0x400000
#define		RAB64$M_NQL		0x1
#define		RAB64$M_NODLCKWT	0x2
#define		RAB64$M_NODLCKBLK	0x4
#define		RAB64$C_SEQ		0
#define		RAB64$C_KEY		1
#define		RAB64$C_RFA		2
#define		RAB64$C_STM		3
#define		RAB64$C_MAXRAC		2
#define		RAB64$M_RESERVED29		0x20000000
#define		RAB64$M_RESERVED30		0x40000000
#define		RAB64$M_RESERVED31		0x80000000
#define		RAB$M_RESERVED29		536870912
#define		RAB$M_RESERVED30		1073741824
#define		RAB$M_RESERVED31		-2147483648
	
#define RAB64$K_BLN64 144
#define RAB64$C_BLN64 144
#define RAB$K_BLN64   144
#define RAB$C_BLN64   144
#define RAB$K_MAXBLN  144
#define RAB$C_MAXBLN  144
	
#define RAB64$V_PPF_RAT	6
#define RAB64$S_PPF_RAT	8
#define RAB64$V_PPF_IND	14
#define RAB64$V_ASY	0
#define RAB64$V_TPT	1
#define RAB64$V_REA	2
#define RAB64$V_RRL	3
#define RAB64$V_UIF	4
#define RAB64$V_MAS	5
#define RAB64$V_FDL	6
#define RAB64$V_REV	7
#define RAB64$V_EOF	8
#define RAB64$V_RAH	9
#define RAB64$V_WBH	10
#define RAB64$V_BIO	11
#define RAB64$V_CDK	12
#define RAB64$V_LOA	13
#define RAB64$V_LIM	14
#define RAB64$V_SYNCSTS	15
#define RAB64$V_LOC	16
#define RAB64$V_WAT	17
#define RAB64$V_ULK	18
#define RAB64$V_RLK	19
#define RAB64$V_NLK	20
#define RAB64$V_KGE	21
#define RAB64$V_KGT	22
#define RAB64$V_NXR	23
#define RAB64$V_RNE	24
#define RAB64$V_TMO	25
#define RAB64$V_CVT	26
#define RAB64$V_RNF	27
#define RAB64$V_ETO	28
#define RAB64$V_PTA	29
#define RAB64$V_PMT	30
#define RAB64$V_CCO	31
#define RAB64$V_EQNXT 	21
#define RAB64$V_NXT  	22

struct _rabdef {
  unsigned char rab$b_bid;
  unsigned char rab$b_bln;
  union  {
    unsigned short int rab$w_isi;
    struct  {
      unsigned rabdef$$_fill_1		: 6;
      unsigned rab$v_ppf_rat		: 8;
      unsigned rab$v_ppf_ind		: 1;
      unsigned rab$v_ppisi		: 1;
    };
  };
  union  {
    unsigned int rab$l_rop;
    struct  {
      unsigned rab$v_asy		: 1;
      unsigned rab$v_tpt		: 1;
      unsigned rab$v_rea		: 1;
      unsigned rab$v_rrl		: 1;
      unsigned rab$v_uif		: 1;
      unsigned rab$v_mas		: 1;
      unsigned rab$v_fdl		: 1;
      unsigned rab$v_rev		: 1;
      unsigned rab$v_eof		: 1;
      unsigned rab$v_rah		: 1;
      unsigned rab$v_wbh		: 1;
      unsigned rab$v_bio		: 1;
      unsigned rab$v_cdk		: 1;
      unsigned rab$v_loa		: 1;
      unsigned rab$v_lim		: 1;
      unsigned rab$v_syncsts		: 1;
      unsigned rab$v_loc		: 1;
      unsigned rab$v_wat		: 1;
      unsigned rab$v_ulk		: 1;
      unsigned rab$v_rlk		: 1;
      unsigned rab$v_nlk		: 1;
      unsigned rab$v_kge		: 1;
      unsigned rab$v_kgt		: 1;
      unsigned rab$v_nxr		: 1;
      unsigned rab$v_rne		: 1;
      unsigned rab$v_tmo		: 1;
      unsigned rab$v_cvt		: 1;
      unsigned rab$v_rnf		: 1;
      unsigned rab$v_eto		: 1;
      unsigned rab$v_pta		: 1;
      unsigned rab$v_pmt		: 1;
      unsigned rab$v_cco		: 1;
    };
    struct  {
      unsigned rabdef$$_fill_6		: 21;
      unsigned rab$v_eqnxt		: 1;
      unsigned rab$v_nxt		: 1;
      unsigned rab$v_fill_3_		: 1;
    };
    struct  {
      char rabdef$$_fill_3;
      unsigned char rab$b_rop1;
      unsigned char rab$b_rop2;
      unsigned char rab$b_rop3;
    };
  };
  unsigned int rab$l_sts;
  union  {
    unsigned int rab$l_stv;
    struct  {
      unsigned short int rab$w_stv0;
      unsigned short int rab$w_stv2;
    };
  };
  union  {
    unsigned short int rab$w_rfa [3];
    struct  {
      unsigned int rab$l_rfa0;
      unsigned short int rab$w_rfa4;
    };
  };
  short int rabdef$$_fill_4;
  unsigned int rab$l_ctx;
  union  {
    unsigned short int rab$w_rop_2;
    struct  {
      unsigned rab$v_nql		: 1;
      unsigned rab$v_nodlckwt		: 1;
      unsigned rab$v_nodlckblk		: 1;
      unsigned rabdef$$_fill_5		: 13;
    };
  };
  unsigned char rab$b_rac;
  unsigned char rab$b_tmo;
  unsigned short int rab$w_usz;
  unsigned short int rab$w_rsz;
  char *rab$l_ubf;
  char *rab$l_rbf;
  char *rab$l_rhb;
  union  {
    char *rab$l_kbf;
    char *rab$l_pbf;
  };
  union  {
    unsigned char rab$b_ksz;
    unsigned char rab$b_psz;
  };
  unsigned char rab$b_krf;
  char rab$b_mbf;
  unsigned char rab$b_mbc;
  union  {
    unsigned int rab$l_bkt;
    unsigned int rab$l_dct;
  };
  struct _fabdef *rab$l_fab;
  void *rab$l_xab;
};
 
struct _rab64def {
  unsigned char rab64$b_bid;
  unsigned char rab64$b_bln;
  union  {
    unsigned short int rab64$w_isi;
    struct  {
      unsigned rab64def$$_fill_1	: 6;
      unsigned rab64$v_ppf_rat		: 8;
      unsigned rab64$v_ppf_ind		: 1;
      unsigned rab64$v_ppisi		: 1;
    };
  };
  union  {
    unsigned int rab64$l_rop;
    struct  {
      unsigned rab64$v_asy		: 1;
      unsigned rab64$v_tpt		: 1;
      unsigned rab64$v_rea		: 1;
      unsigned rab64$v_rrl		: 1;
      unsigned rab64$v_uif		: 1;
      unsigned rab64$v_mas		: 1;
      unsigned rab64$v_fdl		: 1;
      unsigned rab64$v_rev		: 1;
      unsigned rab64$v_eof		: 1;
      unsigned rab64$v_rah		: 1;
      unsigned rab64$v_wbh		: 1;
      unsigned rab64$v_bio		: 1;
      unsigned rab64$v_cdk		: 1;
      unsigned rab64$v_loa		: 1;
      unsigned rab64$v_lim		: 1;
      unsigned rab64$v_syncsts		: 1;
      unsigned rab64$v_loc		: 1;
      unsigned rab64$v_wat		: 1;
      unsigned rab64$v_ulk		: 1;
      unsigned rab64$v_rlk		: 1;
      unsigned rab64$v_nlk		: 1;
      unsigned rab64$v_kge		: 1;
      unsigned rab64$v_kgt		: 1;
      unsigned rab64$v_nxr		: 1;
      unsigned rab64$v_rne		: 1;
      unsigned rab64$v_tmo		: 1;
      unsigned rab64$v_cvt		: 1;
      unsigned rab64$v_rnf		: 1;
      unsigned rab64$v_eto		: 1;
      unsigned rab64$v_pta		: 1;
      unsigned rab64$v_pmt		: 1;
      unsigned rab64$v_cco		: 1;
    };
    struct  {
      unsigned rab64def$$_fill_6	: 21;
      unsigned rab64$v_eqnxt		: 1;
      unsigned rab64$v_nxt		: 1;
      unsigned rab64$v_fill_4_		: 1;
    };
    struct  {
      char rab64def$$_fill_3;
      unsigned char rab64$b_rop1;
      unsigned char rab64$b_rop2;
      unsigned char rab64$b_rop3;
    };
  };
  unsigned int rab64$l_sts;
  union  {
    unsigned int rab64$l_stv;
    struct  {
      unsigned short int rab64$w_stv0;
      unsigned short int rab64$w_stv2;
    };
  };
  union  {
    unsigned short int rab64$w_rfa [3];
    struct  {
      unsigned int rab64$l_rfa0;
      unsigned short int rab64$w_rfa4;
    };
  };
  short int rab64def$$_fill_4;
  unsigned int rab64$l_ctx;
  union  {
    unsigned short int rab64$w_rop_2;
    struct  {
      unsigned rab64$v_nql		: 1;
      unsigned rab64$v_nodlckwt		: 1;
      unsigned rab64$v_nodlckblk	: 1;
      unsigned rab64def$$_fill_5	: 13;
    };
  };
  unsigned char rab64$b_rac;
  unsigned char rab64$b_tmo;
  unsigned short int rab64$w_usz;
  unsigned short int rab64$w_rsz;
  char *rab64$l_ubf;
  char *rab64$l_rbf;
  char *rab64$l_rhb;
  union  {
    char *rab64$l_kbf;
    char *rab64$l_pbf;
  };
  union  {
    unsigned char rab64$b_ksz;
    unsigned char rab64$b_psz;
  };
  unsigned char rab64$b_krf;
  char rab64$b_mbf;
  unsigned char rab64$b_mbc;
  union  {
    unsigned int rab64$l_bkt;
    unsigned int rab64$l_dct;
  };
  struct _fabdef *rab64$l_fab;
  void *rab64$l_xab;
  union  {
    unsigned int rab64$l_version;
    unsigned int rab$l_version;
    struct  {
      unsigned short int rab64$w_verminor;
      unsigned short int rab64$w_vermajor;
    };
    struct  {
      unsigned short int rab$w_verminor;
      unsigned short int rab$w_vermajor;
    };
  };
  union  {
    void *rab64$pq_ubf;
    void *rab$pq_ubf;
  };
  union  {
    unsigned long long rab64$q_usz;
    unsigned long long rab$q_usz;
  };
  union  {
    void *rab64$pq_rbf;
    void *rab$pq_rbf;
  };
  union  {
    unsigned long long rab64$q_rsz;
    unsigned long long rab$q_rsz;
  };
  union  {
    void *rab64$pq_kbf;
    void *rab$pq_kbf;
  };
  union  {
    void *rab64$pq_rhb;
    void *rab$pq_rhb;
  };
  union  {
    unsigned long long rab64$q_ctx;
    unsigned long long rab$q_ctx;
  };
  union  {
    unsigned int rab64$l_ropext;
    unsigned int rab$l_ropext;
    struct  {
      unsigned rab64def$$_fill_7_1		: 29;
      unsigned rab64$v_reserved29		: 1;
      unsigned rab64$v_reserved30		: 1;
      unsigned rab64$v_reserved31		: 1;
    };
    struct  {
      unsigned rab64def$$_fill_7_2	: 29;
      unsigned rab$v_reserved29		: 1;
      unsigned rab$v_reserved30		: 1;
      unsigned rab$v_reserved31		: 1;
    };
  };
  union  {
    unsigned int rab64$l_mbf;
    unsigned int rab$l_mbf;
  };
  union  {
    unsigned int rab64$l_mbc;
    unsigned int rab$l_mbc;
  };
  union  {
    unsigned int rab64$l_reserved_1;
    unsigned int rab$l_reserved_1;
  };
};
 
extern struct _rabdef cc$rms_rab;

extern struct _rab64def cc$rms_rab64;

#endif
 
