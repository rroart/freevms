#ifndef xaballdef_h
#define xaballdef_h

#define XAB$C_ALL 20
#define XAB$M_HRD 0x1
#define XAB$M_ONC 0x2
#define XAB$M_CBT 0x20
#define XAB$M_CTG 0x80
#define XAB$C_ANY 0
#define XAB$C_CYL 1
#define XAB$C_LBN 2
#define XAB$C_VBN 3
#define XAB$C_RFI 4

#define XAB$K_ALLLEN 32
#define XAB$C_ALLLEN 32

#define XAB$V_HRD	0
#define XAB$V_ONC	1
#define XAB$V_CBT	5
#define XAB$V_CTG	7

// like On-Disk Area Descriptor in rmsint2.doc?
	
struct _xaballdef {
  unsigned char xab$b_cod;
  unsigned char xab$b_bln;
  short int xaballdef$$_fill_1;
  void *xab$l_nxt;
  union  {
    unsigned char xab$b_aop;
    struct  {
      unsigned xab$v_hrd		: 1;
      unsigned xab$v_onc		: 1;
      unsigned xaballdef$$_fill_5	: 3;
      unsigned xab$v_cbt		: 1;
      unsigned xaballdef$$_fill_6	: 1;
      unsigned xab$v_ctg		: 1;
    };
  };
  unsigned char xab$b_aln;
  unsigned short int xab$w_vol;
  unsigned int xab$l_loc;
  unsigned int xab$l_alq;
  unsigned short int xab$w_deq;
  unsigned char xab$b_bkz;
  unsigned char xab$b_aid;
  union  {
    unsigned short int xab$w_rfi [3];
    struct  {
      unsigned short int xab$w_rfi0;
      unsigned short int xab$w_rfi2;
      unsigned short int xab$w_rfi4;
    };
  };
  short int xaballdef$$_fill_8;
};

extern struct _xaballdef cc$rms_xaball;
 
#endif
 
