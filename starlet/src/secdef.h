#ifndef secdef_h
#define secdef_h

#define		SEC$M_GBL		0x1
#define		SEC$M_CRF		0x2
#define		SEC$M_DZRO		0x4
#define		SEC$M_WRT		0x8
#define		SEC$M_SHMGS		0x10
#define		SEC$M_WRTMOD		0xC0
#define		SEC$M_AMOD		0x300
#define		SEC$M_READ_ONLY_SHPT	0x800
#define		SEC$M_SHARED_PTS	0x1000
#define		SEC$M_MRES		0x2000
#define		SEC$M_PERM		0x4000
#define		SEC$M_SYSGBL		0x8000
#define		SEC$M_PFNMAP		0x10000
#define		SEC$M_EXPREG		0x20000
#define		SEC$M_PROTECT		0x40000
#define		SEC$M_PAGFIL		0x80000
#define		SEC$M_EXECUTE		0x100000
#define		SEC$M_NOPROTCHK		0x200000
#define		SEC$M_NO_OVERMAP	0x400000
#define		SEC$M_INPROG		0x800000
#define		SEC$M_PARANOID		0x1000000
#define		SEC$M_GRANHINT		0x2000000
#define		SEC$M_MRES_ALLOC	0x4000000
#define		SEC$M_RAD_HINT		0x8000000
	
#define		SEC$K_LENGTH		40
#define		SEC$C_LENGTH		40

#define		SECFLG$M_GBL		0x1
#define		SECFLG$M_CRF		0x2
#define		SECFLG$M_DZRO		0x4
#define		SECFLG$M_WRT		0x8
#define		SECFLG$M_SHMGS		0x10
#define		SECFLG$M_WRTMOD		0xC0
#define		SECFLG$M_AMOD		0x300
#define		SECFLG$M_READ_ONLY_SHPT	0x800
#define		SECFLG$M_SHARED_PTS	0x1000
#define		SECFLG$M_MRES		0x2000
#define		SECFLG$M_PERM		0x4000
#define		SECFLG$M_SYSGBL		0x8000
#define		SECFLG$M_PFNMAP		0x10000
#define		SECFLG$M_EXPREG		0x20000
#define		SECFLG$M_PROTECT	0x40000
#define		SECFLG$M_PAGFIL		0x80000
#define		SECFLG$M_EXECUTE	0x100000
#define		SECFLG$M_NOPROTCHK	0x200000
#define		SECFLG$M_NO_OVERMAP	0x400000
#define		SECFLG$M_INPROG		0x800000
#define		SECFLG$M_PARANOID	0x1000000
#define		SECFLG$M_GRANHINT	0x2000000
#define		SECFLG$M_MRES_ALLOC	0x4000000
#define		SECFLG$M_RAD_HINT	0x8000000
	
#define		SEC$K_MATALL		0
#define		SEC$K_MATEQU		1
#define		SEC$K_MATLEQ		2
	
#define		SECI$_CHAIN		1
#define		SECI$_GSMODE		2
#define		SECI$_GSFLAGS		3
#define		SECI$_GSNAME		4
#define		SECI$_GSIDENT		5
#define		SECI$_GSRELPAG		6

#define		UPDFLG$M_WRT_MODIFIED	0x1
	
struct _secdef {
  union  {
    int sec$l_gsd;
    int sec$l_ccb;
  };
  unsigned int sec$l_secxfl;
  unsigned int sec$l_secxbl;
  unsigned int sec$l_pfc;
  long sec$l_window; // check. was: int
  unsigned int sec$l_vbn;
  union  {
    unsigned int sec$l_flags;
    struct  {
      unsigned sec$v_gbl		: 1;
      unsigned sec$v_crf		: 1;
      unsigned sec$v_dzro		: 1;
      unsigned sec$v_wrt		: 1;
      unsigned sec$v_shmgs		: 1;
      unsigned secdef$$_fill_2		: 1;
      unsigned sec$v_wrtmod		: 2;
      unsigned sec$v_amod		: 2;
      unsigned secdef$$_fill_3		: 1;
      unsigned sec$v_read_only_shpt	: 1;
      unsigned sec$v_shared_pts		: 1;
      unsigned sec$v_mres		: 1;
      unsigned sec$v_perm		: 1;
      unsigned sec$v_sysgbl		: 1;
      unsigned sec$v_expreg		: 1;
      unsigned sec$v_protect		: 1;
      unsigned sec$v_pagfil		: 1;
      unsigned sec$v_execute		: 1;
      unsigned sec$v_noprotchk		: 1;
      unsigned sec$v_no_overmap		: 1;
      unsigned sec$v_inprog		: 1;
      unsigned sec$v_paranoid		: 1;
      unsigned sec$v_granhint		: 1;
      unsigned sec$v_mres_alloc		: 1;
      unsigned sec$v_rad_hint		: 1;
      unsigned sec$v_fill_18_		: 4;
    };
  };
  int sec$l_refcnt;
  unsigned int sec$l_unit_cnt;
  unsigned int sec$l_vpx;
};
 
struct _secdef_flags {
  union  {
    unsigned short int secflg$w_saved_flags;
    unsigned int secflg$l_flags;
    struct  {
      unsigned secflg$v_gbl		: 1;
      unsigned secflg$v_crf		: 1;
      unsigned secflg$v_dzro		: 1;
      unsigned secflg$v_wrt		: 1;
      unsigned secflg$v_shmgs		: 1;
      unsigned secdef$$_fill_2		: 1;
      unsigned secflg$v_wrtmod		: 2;
      unsigned secflg$v_amod		: 2;
      unsigned secdef$$_fill_3		: 1;
      unsigned secflg$v_read_only_shpt	: 1;
      unsigned secflg$v_shared_pts	: 1;
      unsigned secflg$v_mres		: 1;
      unsigned secflg$v_perm		: 1;
      unsigned secflg$v_sysgbl		: 1;
      unsigned secflg$v_pfnmap		: 1;
      unsigned secflg$v_expreg		: 1;
      unsigned secflg$v_protect		: 1;
      unsigned secflg$v_pagfil		: 1;
      unsigned secflg$v_execute		: 1;
      unsigned secflg$v_noprotchk	: 1;
      unsigned secflg$v_no_overmap	: 1;
      unsigned secflg$v_inprog		: 1;
      unsigned secflg$v_paranoid	: 1;
      unsigned secflg$v_granhint	: 1;
      unsigned secflg$v_mres_alloc	: 1;
      unsigned secflg$v_rad_hint	: 1;
      unsigned secflg$v_fill_19_	: 4;
    };
  };
};
 
struct _updsec_flags {
  union  {
    unsigned int updflg$l_flags;
    struct  {
      unsigned updflg$v_wrt_modified		: 1;
      unsigned updflg$v_fill_20_		: 7;
    };
  };
};
 
struct _secdef1 {
  char secdef$$_fill_6 [25];
  unsigned char sec$b_amod;
};

#endif
 
