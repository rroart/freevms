#ifndef xabtrmdef_h
#define xabtrmdef_h

#define XAB$C_TRM 31
	
#define XAB$K_TRMLEN 36
#define XAB$C_TRMLEN 36
	
struct _xabtrmdef {
  unsigned char xab$b_cod;
  unsigned char xab$b_bln;
  short int xabtrmdef$$_fill_1;
  void *xab$l_nxt;
  void *xab$l_itmlst;
  unsigned short int xab$w_itmlst_len;
  short int xabtrmdef$$_fill_5;
  unsigned int xabtrmdef$$_fill_6;
  unsigned int xabtrmdef$$_fill_7;
  unsigned int xabtrmdef$$_fill_8;
  unsigned int xabtrmdef$$_fill_9;
  unsigned int xabtrmdef$$_fill_10;
};

extern struct _xabtrmdef cc$rms_xabtrm;

#endif
 
