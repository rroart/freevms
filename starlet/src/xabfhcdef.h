#ifndef xabfhcdef_h
#define xabfhcdef_h

#define XAB$C_FHC 29
#define XAB$M_FTN 0x1
#define XAB$M_CR 0x2
#define XAB$M_PRN 0x4
#define XAB$M_BLK 0x8
	
#define XAB$K_FHCLEN 44
#define XAB$C_FHCLEN 44

struct _xabfhcdef {
  unsigned char xab$b_cod;
  unsigned char xab$b_bln;
  short int xabfhcdef$$_fill_1;
  void *xab$l_nxt;
  unsigned char xab$b_rfo;
  union  {
    unsigned char xab$b_atr;
    struct  {
      unsigned xab$v_ftn : 1;
      unsigned xab$v_cr : 1;
      unsigned xab$v_prn : 1;
      unsigned xab$v_blk : 1;
      unsigned xab$v_fill_11_ : 4;
    };
  };
  unsigned short int xab$w_lrl;
  union  {
    unsigned int xab$l_hbk;
    struct  {
      unsigned short int xab$w_hbk0;
      unsigned short int xab$w_hbk2;
    };
  };
  union  {
    unsigned int xab$l_ebk;
    struct  {
      unsigned short int xab$w_ebk0;
      unsigned short int xab$w_ebk2;
    };
  };
  unsigned short int xab$w_ffb;
  unsigned char xab$b_bkz;
  unsigned char xab$b_hsz;
  unsigned short int xab$w_mrz;
  unsigned short int xab$w_dxq;
  unsigned short int xab$w_gbc;
  char xabfhcdef$$_fill_6 [8];
  unsigned short int xab$w_verlimit;
  unsigned int xab$l_sbn;
};

extern struct _xabfhcdef cc$rms_xabfhc;
 
#endif
 
