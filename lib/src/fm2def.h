#ifndef fm2def_h
#define fm2def_h

#define FM2$C_PLACEMENT 0
#define FM2$C_FORMAT1 1
#define FM2$C_FORMAT2 2
#define FM2$C_FORMAT3 3

#define FM2$K_LENGTH0 2
#define FM2$C_LENGTH0 2
#define FM2$K_LENGTH1 4
#define FM2$C_LENGTH1 4
#define FM2$S_FM2DEF 4
	
struct _fm2 {
  union  {
    unsigned short int fm2$w_word0;
    struct  {
      unsigned fm2$$_fill_1 : 14;
      unsigned fm2$v_format : 2;
    };
    struct  {
      unsigned fm2$v_exact : 1;
      unsigned fm2$v_oncyl : 1;
      unsigned fm2$$_fill_2 : 10;
      unsigned fm2$v_lbn : 1;
      unsigned fm2$v_rvn : 1;
      unsigned fm2$v_fill_3_ : 2;
    };
    struct  {
      unsigned fm2$$_fill_3 : 8;
      unsigned fm2$v_highlbn : 6;
      unsigned fm2$v_fill_4_ : 2;
    };
    struct  {
      unsigned fm2$v_count2 : 14;
      unsigned fm2$v_fill_5_ : 2;
    };
    unsigned char fm2$b_count1;
  };
  unsigned short int fm2$w_lowlbn;
};
 
#define FM2$K_LENGTH2 6
#define FM2$C_LENGTH2 6
#define FM2$S_FM2DEF1 6
	
struct _fm2_1 {
  char fm2$$_fill_4 [2];
  unsigned int fm2$l_lbn2;
};

#define FM2$K_LENGTH3 8
#define FM2$C_LENGTH3 8
#define FM2$S_FM2DEF2 8
	
struct _fm2_2 {
  char fm2$$_fill_5 [2];
  unsigned short int fm2$w_lowcount;
  unsigned int fm2$l_lbn3;
};

#endif
 
