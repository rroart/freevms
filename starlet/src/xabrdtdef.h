#ifndef xabrdtdef_h
#define xabrdtdef_h

#define XAB$C_RDT 30
	
#define XAB$K_RDTLEN 20
#define XAB$C_RDTLEN 20
	
struct _xabrdtdef {
  unsigned char xab$b_cod;
  unsigned char xab$b_bln;
  short int xabrdtdef$$_fill_1;
  void *xab$l_nxt;
  unsigned short int xab$w_rvn;
  short int xabrdtdef$$_fill_2;
  long long xab$q_rdt;
};

extern struct _xabrdtdef cc$rms_xabrdt;

#endif
 
