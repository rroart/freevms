#ifndef xabprodef_h
#define xabprodef_h

#define XAB$C_PRO 19
#define XAB$M_NOREAD 0x1
#define XAB$M_NOWRITE 0x2
#define XAB$M_NOEXE 0x4
#define XAB$M_NODEL 0x8
	
union _xabprodef {
  struct  {
    unsigned xab$v_noread : 1;
    unsigned xab$v_nowrite : 1;
    unsigned xab$v_noexe : 1;
    unsigned xab$v_nodel : 1;
    unsigned xab$v_fill_12_ : 4;
  };
};
 
#define XAB$M_PROPAGATE 0x1
#define XAB$K_PROLEN_V3 16
#define XAB$C_PROLEN_V3 16
	
struct _xabprodef1 {
  unsigned char xab$b_cod;
  unsigned char xab$b_bln;
  short int xabprodef$$_fill_1;
  void *xab$l_nxt;
  union  {
    unsigned short int xab$w_pro;
    struct  {
      unsigned xab$v_sys : 4;
      unsigned xab$v_own : 4;
      unsigned xab$v_grp : 4;
      unsigned xab$v_wld : 4;
    };
  };
  unsigned char xab$b_mtacc;
  union  {
    unsigned char xab$b_prot_opt;
    struct  {
      unsigned xab$v_propagate : 1;
      unsigned xab$v_fill_13_ : 7;
    };
  };
  union  {
    unsigned int xab$l_uic;
    struct  {
      unsigned short int xab$w_mbm;
      unsigned short int xab$w_grp;
    };
  };
  union  {
    long long xab$q_prot_mode;
    struct  {
      unsigned char xab$b_prot_mode;
    };
  };
  char *xab$l_aclbuf;
  unsigned short int xab$w_aclsiz;
  unsigned short int xab$w_acllen;
  unsigned int xab$l_aclctx;
  unsigned int xab$l_aclsts;
  unsigned int xabprodef$$_fill_10;
  unsigned int xabprodef$$_fill_11;
  unsigned int xabprodef$$_fill_12;
  unsigned int xabprodef$$_fill_13;
  unsigned int xabprodef$$_fill_14;
  unsigned int xabprodef$$_fill_15;
  unsigned int xabprodef$$_fill_16;
  unsigned int xabprodef$$_fill_17;
  unsigned int xabprodef$$_fill_18;
  unsigned int xabprodef$$_fill_19;
  unsigned int xabprodef$$_fill_20;
  unsigned int xabprodef$$_fill_21;
};
 
#define XAB$K_PROLEN 88
#define XAB$C_PROLEN 88

#define XAB$V_SYS	0
#define XAB$S_SYS	4
#define XAB$V_OWN	4
#define XAB$S_OWN	4
#define XAB$V_GRP	8
#define XAB$S_GRP	4
#define XAB$V_WLD	12
#define XAB$S_WLD	4
#define XAB$V_NOREAD	0
#define XAB$V_NOWRITE	1
#define XAB$V_NOEXE	2
#define XAB$V_NODEL	3
#define XAB$V_PROPAGATE	0
#define XAB$V_PROPOGATE	0
#define XAB$M_PROPOGATE	(1 << XAB$V_PROPOGATE)

extern struct _xabprodef1 cc$rms_xabpro;

#endif
 
