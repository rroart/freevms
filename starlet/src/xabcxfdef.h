#ifndef xabcxfdef_h
#define xabcxfdef_h

#define XAB$C_CXF 32
#define XAB$M_CXFRST 0x1
	
#define XAB$K_CXFLEN 60
#define XAB$C_CXFLEN 60
	
struct _xabcxfdef {
  unsigned char xab$b_cod;
  unsigned char xab$b_bln;
  short int xabcxfdef$$_fill_1;
  void *xab$l_nxt;
  unsigned int xab$l_cxfsts;
  unsigned int xab$l_cxfstv;
  union  {
    unsigned int xab$l_cxfcop;
    struct  {
      unsigned xab$v_cxfrst   : 1;
      unsigned xab$v_fill_15_ : 7;
    };
  };
  unsigned int xab$l_cxfbkp;
  unsigned short int xab$w_cxfifi;
  unsigned char xab$b_cxfver;
  char xabcxfdef$$_fill_5;
  unsigned int xabcxfdef$$_fill_6;
  unsigned short int xab$w_cxfdeq;
  unsigned char xab$b_cxffac;
  unsigned char xab$b_cxfshr;
  unsigned short int xab$w_cxfrte;
  char xabcxfdef$$_fill_7;
  unsigned char xab$b_cxforg;
  unsigned short int xab$w_cxfgbc;
  unsigned char xab$b_cxfrtv;
  char xabcxfdef$$_fill_8;
  unsigned int xabcxfdef$$_fill_9 [4];
};
 
#endif
 
