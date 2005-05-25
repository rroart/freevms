#ifndef iafdef_h
#define iafdef_h
 
#define IAF$K_LENGTH 64
#define IAF$C_LENGTH 64
#define IAF$S_IAFDEF 64
	
struct _iaf {
  void *iaf$l_iaflink;
  void *iaf$l_fixuplnk;
  unsigned short int iaf$w_size;
  union  {
    unsigned short int iaf$w_flags;
    struct  {
      unsigned iaf$v_shr	: 1;
      unsigned iaf$v_fill_0_	: 7;
    };
  };
  unsigned int iaf$l_g_fixoff;
  unsigned int iaf$l_dotadroff;
  unsigned int iaf$l_chgprtoff;
  unsigned int iaf$l_shlstoff;
  unsigned int iaf$l_shrimgcnt;
  unsigned int iaf$l_shlextra;
  void *iaf$l_permctx;
  int iafdef$$_fill_1;
  int iafdef$$_fill_2;
  int iafdef$$_fill_3;
  int iafdef$$_fill_4;
  int iafdef$$_fill_5;
  int iafdef$$_fill_6;
};
 
#endif
