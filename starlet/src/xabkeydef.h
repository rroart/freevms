#ifndef xabkeydef_h
#define xabkeydef_h

#define		XAB$C_KEY		21
#define		XAB$M_DUP		0x1
#define		XAB$M_CHG		0x2
#define		XAB$M_NUL		0x4
#define		XAB$M_IDX_NCMPR		0x8
#define		XAB$M_KEY_NCMPR		0x40
#define		XAB$M_DAT_NCMPR		0x80
#define		XAB$C_STG		0
#define		XAB$C_IN2		1
#define		XAB$C_BN2		2
#define		XAB$C_IN4		3
#define		XAB$C_BN4		4
#define		XAB$C_PAC		5
#define		XAB$C_IN8		6
#define		XAB$C_BN8		7
#define		XAB$C_COL		8
#define		XAB$C_MAX_ASCEND	8
#define		XAB$C_DSTG		32
#define		XAB$C_DIN2		33
#define		XAB$C_DBN2		34
#define		XAB$C_DIN4		35
#define		XAB$C_DBN4		36
#define		XAB$C_DPAC		37
#define		XAB$C_DIN8		38
#define		XAB$C_DBN8		39
#define		XAB$C_DCOL		40
#define		XAB$C_MAXDTP		40
#define		XAB$K_KEYLEN_V2		64
#define		XAB$C_KEYLEN_V2		64

#define		XAB$C_PRG3		3
#define		XAB$C_PRG2		2
#define		XAB$C_PRG1		1
#define		XAB$K_KEYLEN_V4		76
#define		XAB$C_KEYLEN_V4		76
	
#define		XAB$K_KEYLEN		100
#define		XAB$C_KEYLEN		100
	
#define		XAB$V_DUP	0
#define		XAB$V_CHG	1
#define		XAB$V_NUL	2
#define		XAB$V_IDX_NCMPR	3
#define		XAB$V_KEY_NCMPR	6
#define		XAB$V_DAT_NCMPR	7

// like Indexed File Prologue Block 1 in rmsint2.doc?
// or Key Definition Extended Attribute Block?
	
struct _xabkeydef {
  unsigned char xab$b_cod;
  unsigned char xab$b_bln;
  short xabkeydef$$_fill_1;
  void *xab$l_nxt;
  unsigned char xab$b_ian;
  unsigned char xab$b_lan;
  unsigned char xab$b_dan;
  unsigned char xab$b_lvl;
  unsigned char xab$b_ibs;
  unsigned char xab$b_dbs;
  //unsigned int xab$l_rvb;
  unsigned short xab$l_rvb_l;
  unsigned short xab$l_rvb_h;
  union  {
    unsigned char xab$b_flg;
#if 0
    // gcc bitfield problems
    struct  {
      unsigned xab$v_dup		: 1;
      unsigned xab$v_chg		: 1;
      unsigned xab$v_nul		: 1;
      unsigned xab$v_idx_ncmpr		: 1;
      unsigned xabkeydef$$_fill_5	: 2;
      unsigned xab$v_key_ncmpr		: 1;
      unsigned xab$v_fill_14_		: 1;
    };
    struct  {
      unsigned xabkeydef$$_fill_6	: 1;
      unsigned xabkeydef$$_fill_7	: 2;
      unsigned xabkeydef$$_fill_8	: 1;
      unsigned xabkeydef$$_fill_9	: 2;
      unsigned xabkeydef$$_fill_10	: 1;
      unsigned xab$v_dat_ncmpr		: 1;
    };
#endif
  };
  unsigned char xab$b_dtp;
  unsigned char xab$b_nsg;
  unsigned char xab$b_nul;
  unsigned char xab$b_tks;
  unsigned char xab$b_ref;
  unsigned short xab$w_mrl;
  unsigned short xab$w_ifl;
  unsigned short xab$w_dfl;
  union  {
    unsigned short xab$w_pos [8];
    struct  {
      unsigned short xab$w_pos0;
      unsigned short xab$w_pos1;
      unsigned short xab$w_pos2;
      unsigned short xab$w_pos3;
      unsigned short xab$w_pos4;
      unsigned short xab$w_pos5;
      unsigned short xab$w_pos6;
      unsigned short xab$w_pos7;
    };
  };
  union  {
    unsigned char xab$b_siz [8];
    struct  {
      unsigned char xab$b_siz0;
      unsigned char xab$b_siz1;
      unsigned char xab$b_siz2;
      unsigned char xab$b_siz3;
      unsigned char xab$b_siz4;
      unsigned char xab$b_siz5;
      unsigned char xab$b_siz6;
      unsigned char xab$b_siz7;
    };
  };
  short xabkeydef$$_fill_11;
  char *xab$l_knm;
  unsigned int xab$l_dvb;
  union  {
    unsigned char xab$b_typ [8];
    struct  {
      unsigned char xab$b_typ0;
      unsigned char xab$b_typ1;
      unsigned char xab$b_typ2;
      unsigned char xab$b_typ3;
      unsigned char xab$b_typ4;
      unsigned char xab$b_typ5;
      unsigned char xab$b_typ6;
      unsigned char xab$b_typ7;
    };
  };
  unsigned char xab$b_prolog;
  char xabkeydef$$_fill_12;
  short xabkeydef$$_fill_13;
  void *xab$l_coltbl;
  unsigned int xab$l_colsiz;
  char *xab$l_colnam;
  unsigned int xabkeydef$$_fill_14;
  unsigned int xabkeydef$$_fill_15;
  unsigned int xabkeydef$$_fill_16;
};

// cc$rms_xabkey = cod= 0x15 (C_KEY) bln=0x64 (C_KEYLEN)

extern struct _xabkeydef cc$rms_xabkey;
 
#endif
 
