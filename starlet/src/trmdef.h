#ifndef trmdef_h
#define trmdef_h

#define TRM$_MODIFIERS 0
#define TRM$_EDITMODE 1
#define TRM$_TIMEOUT 2
#define TRM$_TERM 3
#define TRM$_PROMPT 4
#define TRM$_INISTRNG 5
#define TRM$_PICSTRNG 6
#define TRM$_FILLCHR 7
#define TRM$_INIOFFSET 8
#define TRM$_ALTECHSTR 9
#define TRM$_ESCTRMOVR 10
#define TRM$_LASTITM 11
#define TRM$_RESERVE12 12
#define TRM$_RESERVE13 13
#define TRM$_RESERVE14 14
#define TRM$_RESERVE15 15
#define TRM$_RESERVE16 16
#define TRM$_RESERVE17 17
#define TRM$_RESERVE18 18
#define TRM$_RESERVE19 19
#define TRM$_RESERVE20 20
#define TRM$_RESERVE21 21
#define TRM$_RESERVE22 22
#define TRM$_RESERVE23 23
#define TRM$_RESERVE24 24
#define TRM$_RESERVE25 25
#define TRM$_RESERVE26 26
#define TRM$_RESERVE27 27
#define TRM$_RESERVE28 28
#define TRM$_RESERVE29 29
#define TRM$_RESERVE30 30
#define TRM$_RECLINE 31

#define TRM$K_EM_DEFAULT 0
#define TRM$K_EM_RDVERIFY 1
#define TRM$M_TM_NOECHO 0x40
#define TRM$M_TM_TIMED 0x80
#define TRM$M_TM_CVTLOW 0x100
#define TRM$M_TM_NOFILTR 0x200
#define TRM$M_TM_DSABLMBX 0x400
#define TRM$M_TM_PURGE 0x800
#define TRM$M_TM_TRMNOECHO 0x1000
#define TRM$M_TM_REFRESH 0x2000
#define TRM$M_TM_ESCAPE 0x4000
#define TRM$M_TM_NOEDIT 0x8000
#define TRM$M_TM_NORECALL 0x10000
#define TRM$M_TM_R_JUST 0x20000
#define TRM$M_TM_AUTO_TAB 0x40000
#define TRM$M_TM_ARROWS 0x80000
#define TRM$M_TM_TERM_ARROW 0x100000
#define TRM$M_TM_TERM_DEL 0x200000
#define TRM$M_TM_TOGGLE 0x400000
#define TRM$M_TM_OTHERWAY 0x800000
#define TRM$M_TM_NOCLEAR 0x1000000
#define TRM$M_CV_UPPER 0x1
#define TRM$M_CV_LOWER 0x2
#define TRM$M_CV_NUMERIC 0x4
#define TRM$M_CV_NUMPUNC 0x8
#define TRM$M_CV_PRINTABLE 0x10
#define TRM$M_CV_ANY 0x20
#define TRM$M_ST_FIELD_FULL 0x1
#define TRM$M_ST_OTHERWAY 0x2
	
union _trmdef {
  struct  {
    unsigned trmdef$$_fill_1		: 6;
    unsigned trm$v_tm_noecho		: 1;
    unsigned trm$v_tm_timed		: 1;
    unsigned trm$v_tm_cvtlow		: 1;
    unsigned trm$v_tm_nofiltr		: 1;
    unsigned trm$v_tm_dsablmbx		: 1;
    unsigned trm$v_tm_purge		: 1;
    unsigned trm$v_tm_trmnoecho		: 1;
    unsigned trm$v_tm_refresh		: 1;
    unsigned trm$v_tm_escape		: 1;
    unsigned trm$v_tm_noedit		: 1;
    unsigned trm$v_tm_norecall		: 1;
    unsigned trm$v_tm_r_just		: 1;
    unsigned trm$v_tm_auto_tab		: 1;
    unsigned trm$v_tm_arrows		: 1;
    unsigned trm$v_tm_term_arrow	: 1;
    unsigned trm$v_tm_term_del		: 1;
    unsigned trm$v_tm_toggle		: 1;
    unsigned trm$v_tm_otherway		: 1;
    unsigned trm$v_tm_noclear		: 1;
    unsigned trm$v_fill_40_		: 7;
  };
  struct  {
    unsigned trm$v_cv_upper		: 1;
    unsigned trm$v_cv_lower		: 1;
    unsigned trm$v_cv_numeric		: 1;
    unsigned trm$v_cv_numpunc		: 1;
    unsigned trm$v_cv_printable		: 1;
    unsigned trm$v_cv_any		: 1;
    unsigned trm$v_fill_41_		: 2;
  };
  struct  {
    unsigned trm$v_st_field_full	: 1;
    unsigned trm$v_st_otherway		: 1;
    unsigned trm$v_fill_42_		: 6;
  };
};
 
#endif
 
