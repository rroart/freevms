#ifndef shldef_h
#define shldef_h
 
#define SHL$M_FIXUP 0x1
#define SHL$C_OLD_SHL_SIZE 56
#define SHL$C_MAXNAMLNG 39
#define SHL$K_LENGTH 64
#define SHL$C_LENGTH 64
#define SHL$S_SHLDEF 64
	
struct _shl {
  void *shl$l_baseva;
  struct _shl *shl$l_shlptr;

  unsigned int shl$l_ident;
  void *shl$l_permctx;
  char shl$b_shl_size;
  short int shldef$$_fill_1;
  union  {
    unsigned char shl$b_flags;
    struct  {
      unsigned shl$v_fixup	: 1;
      unsigned shl$v_fill_2_	: 7;
    };
  };
  void *shl$l_icb;
  union  {
    char shl$t_imgnam [40];
    char shl$b_namlng;
  };
};
 
#endif
