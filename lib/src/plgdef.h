#ifndef plgdef_h
#define plgdef_h

#define PLG$M_NOEXTEND 0x1
#define PLG$C_VER_NO 1
#define PLG$C_VER_IDX 2
#define PLG$C_VER_3 3
#define PLG$K_BLN 122
#define PLG$C_BLN 122
#define PLG$S_PLGDEF 122

// like Relative Files Prologue Block in rmsint2.doc?
	
struct _plg {
  char plg$$_fill_1 [11];
  unsigned char plg$b_dbktsiz;
  int plg$$_fill_2;
  union  {
    unsigned char plg$b_flags;
    struct  {
      unsigned plg$v_noextend : 1;
      unsigned plg$v_fill_2_ : 7;
    };
  };
  char plg$$_fill_3 [85];
  unsigned char plg$b_avbn;
  unsigned char plg$b_amax;
  unsigned short int plg$w_dvbn;
  short int plg$$_fill_4;
  unsigned int plg$l_mrn;
  unsigned int plg$l_eof;
  unsigned short int plg$w_ver_no;
  unsigned short int plg$w_gbc;
  unsigned short int plg$w_colvbn;
};
 
#endif
 
