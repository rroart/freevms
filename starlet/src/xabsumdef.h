#ifndef xabsumdef_h
#define xabsumdef_h

#define XAB$C_SUM 22
	
#define XAB$K_SUMLEN 12
#define XAB$C_SUMLEN 12
	
struct _xabsumdef {
  unsigned char xab$b_cod;
  unsigned char xab$b_bln;
  short int xabsumdef$$_fill_1;
  void *xab$l_nxt;
  unsigned char xab$b_noa;
  unsigned char xab$b_nok;
  unsigned short int xab$w_pvn;
};

extern struct _xabsumdef cc$rms_xabsum;

#endif
 
