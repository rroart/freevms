#ifndef xabcxrdef_h
#define xabcxrdef_h

#define XAB$C_CXR 33
#define XAB$M_CXRRST 0x1
#define XAB$C_CXB_VER1 1
#define XAB$C_CXRBLEN 512
	
#define XAB$K_CXRLEN 84
#define XAB$C_CXRLEN 84
	
struct _xabcxrdef {
  unsigned char xab$b_cod;
  unsigned char xab$b_bln;
  short int xabcxrdef$$_fill_1;
  void *xab$l_nxt;
  unsigned int xab$l_cxrsts;
  unsigned int xab$l_cxrstv;
  union  {
    unsigned int xab$l_cxrcop;
    struct  {
      unsigned xab$v_cxrrst       : 1;
      unsigned xabcxrdef$$_fill_5 : 27;
      unsigned xab$v_cxrbver      : 4;
    };
  };
  unsigned int xab$l_cxrbkp;
  unsigned short int xab$w_cxrisi;
  unsigned char xab$b_cxrver;
  char xabcxrdef$$_fill_6;
  unsigned int xabcxrdef$$_fill_7;
  unsigned char xab$b_cxrmbf;
  unsigned char xab$b_cxrmbc;
  unsigned short int xab$w_cxrbfz;
  unsigned int xab$l_cxrvbn;
  unsigned short int xab$w_cxroff;
  unsigned short int xab$w_fill_8;
  unsigned int xab$l_cxrpos0;
  unsigned short int xab$w_cxrpos4;
  short int xabcxrdef$$_fill_9;
  unsigned int xab$l_cxrcur0;
  unsigned short int xab$w_cxrcur4;
  short int xabcxrdef$$_fill_10;
  unsigned int xab$l_cxrsid0;
  unsigned short int xab$w_cxrsid4;
  short int xabcxrdef$$_fill_11;
  unsigned short int xab$w_cxrcnt;
  unsigned char xab$b_cxrkref;
  unsigned char xab$b_cxrklen;
  char *xab$l_cxrbuf;
  unsigned int xabcxrdef$$_fill_12 [2];
};
 
#endif
 
