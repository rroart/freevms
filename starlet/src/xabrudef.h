#ifndef xabrudef_h
#define xabrudef_h

#define XAB$C_RU 35
#define XAB$M_NOJOIN 0x1
	
#define XAB$K_RULEN 48
#define XAB$C_RULEN 48
	
struct _xabrudef {
  unsigned char xab$b_cod;
  unsigned char xab$b_bln;
  short int xabrudef$$_fill_1;
  void *xab$l_nxt;
  union  {
    unsigned short int xab$w_ru_flags;
    struct  {
      unsigned xab$v_nojoin   : 1;
      unsigned xab$v_fill_22_ : 7;
    };
  };
  short int xabrudef$$_fill_5;
  unsigned int xab$l_ru_handle;
  unsigned int xab$l_ru_handle_joined;
  unsigned int xabrudef$$_fill_7;
  unsigned int xabrudef$$_fill_8;
  unsigned int xabrudef$$_fill_9;
  unsigned int xabrudef$$_fill_10;
  unsigned int xabrudef$$_fill_11;
  unsigned int xabrudef$$_fill_12;
  unsigned int xabrudef$$_fill_13;
};
 
#endif
 
