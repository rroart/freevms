#ifndef xabdatdef_h
#define xabdatdef_h

#define XAB$C_DAT 18
	
#define XAB$K_DATLEN 60
#define XAB$C_DATLEN 60

#define XAB$K_DATLEN_V2 36
#define XAB$C_DATLEN_V2 36
#define XAB$K_DATLEN_V54 44
#define XAB$C_DATLEN_V54 44
#define XAB$K_DATLEN 60
#define XAB$C_DATLEN 60

struct _xabdatdef {
  unsigned char xab$b_cod;
  unsigned char xab$b_bln;
  short int xabdatdef$$_fill_1;
  void *xab$l_nxt;
  unsigned short int xab$w_rvn;
  short int xabdatdef$$_fill_2;
  long long xab$q_rdt;
  long long xab$q_cdt;
  long long xab$q_edt;
  long long xab$q_bdt;
  long long xab$q_rcd;
  long long xab$q_eff;
};

extern struct _xabdatdef cc$rms_xabdat;

#endif
 
