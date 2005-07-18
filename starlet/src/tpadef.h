#ifndef tpadef_h
#define tpadef_h

#define		TPA$K_COUNT0		8
#define		TPA$M_BLANKS		0x1
#define		TPA$M_ABBREV		0x2
#define		TPA$M_ABBRFM		0x4
#define		TPA$M_AMBIG		0x10000
#define		TPA$K_LENGTH0		36
#define		TPA$C_LENGTH0		36
	
struct _tpadef {
  unsigned int tpa$l_count;
  union  {
    unsigned int tpa$l_options;
    struct  {
      unsigned tpa$v_blanks		: 1;
      unsigned tpa$v_abbrev		: 1;
      unsigned tpa$v_abbrfm		: 1;
      unsigned tpadef$$_fill_1		: 13;
      unsigned tpa$v_ambig		: 1;
      unsigned tpa$v_fill_30_		: 7;
    };
    struct  {
      char tpadef$$_fill_2 [3];
      unsigned char tpa$b_mcount;
    };
  };
  unsigned int tpa$l_stringcnt;
  void *tpa$l_stringptr;
  unsigned int tpa$l_tokencnt;
  void *tpa$l_tokenptr;
  union  {
    struct  {
      union  {
	unsigned int tpa$l_char;
	unsigned char tpa$b_char;
      };
      unsigned int tpa$l_number;
    };
    unsigned long long tpa$q_number;
  };
  unsigned int tpa$l_param;
};
 
#define		TPA64$K_COUNT0		-1
#define		TPA64$M_BLANKS		0x1
#define		TPA64$M_ABBREV		0x2
#define		TPA64$M_ABBRFM		0x4
#define		TPA64$M_AMBIG		0x10000
#define		TPA64$K_LENGTH0		80
#define		TPA64$C_LENGTH0		80
	
struct _tpa64def {
  unsigned int tpa64$l_count;
  union  {
    unsigned int tpa64$l_options;
    struct  {
      unsigned tpa64$v_blanks		: 1;
      unsigned tpa64$v_abbrev		: 1;
      unsigned tpa64$v_abbrfm		: 1;
      unsigned tpa64def$$_fill_1	: 13;
      unsigned tpa64$v_ambig		: 1;
      unsigned tpa64$v_fill_31_		: 7;
    };
    struct  {
      char tpa64def$$_fill_2 [3];
      unsigned char tpa64$b_mcount;
    };
  };
  unsigned long long tpa64$q_stringdesc;
  unsigned long long tpa64$q_stringcnt;
  void *tpa64$q_stringptr;
  unsigned long long tpa64$q_tokendesc;
  unsigned long long tpa64$q_tokencnt;
  void *tpa64$q_tokenptr;
  union  {
    unsigned long long tpa64$q_char;
    unsigned char tpa64$b_char;
  };
  unsigned long long tpa64$q_number;
  unsigned long long tpa64$q_param;
};
 
#endif
 
