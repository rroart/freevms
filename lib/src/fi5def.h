#ifndef fi5def_h
#define fi5def_h

#define FI5$C_ODS2 0
#define FI5$C_ISL1 1
#define FI5$$_TYPE_RESERVED 2
#define FI5$C_UCS2 3
#define FI5$M_FIXED_LENGTH 0x10
#define FI5$C_MIN_LENGTH 120
#define FI5$C_MAX_LENGTH 324
#define FI5$S_FI5DEF 324
	
struct _fi5 {
  union  {
    unsigned char fi5$b_control;
    struct  {
      unsigned fi5$v_nametype : 2;
      unsigned fi5$$_fill_1 : 2;
      unsigned fi5$v_fixed_length : 1;
      unsigned fi5$v_fill_2_ : 3;
    };
  };
  unsigned char fi5$b_namelen;
  unsigned short int fi5$w_revision;
  unsigned long long fi5$q_credate;
  unsigned long long fi5$q_revdate;
  unsigned long long fi5$q_expdate;
  unsigned long long fi5$q_bakdate;
  unsigned long long fi5$q_accdate;
  unsigned long long fi5$q_attdate;
  unsigned long long fi5$q_ex_recattr;
  struct  {
    unsigned long long fi5$q_hint_lo_qw;
    unsigned long long fi5$q_hint_hi_qw;
  };
  char fi5$t_filename [44];
  char fi5$t_filenamext [204];
};
 
#endif
 
