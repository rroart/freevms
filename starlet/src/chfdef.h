#ifndef chfdef_h
#define chddef_h

struct _chfdef {
  struct _chfdef1 *chf$l_sigarglst;
  int chfdef$$_fill_1;
  struct _chfdef2 *chf$l_mcharglst;
};

struct _chfdef1 {
  union  {
    int chf$is_sig_args;
    int chf$l_sig_args;
  };
  union  {
    int chf$is_sig_name;
    int chf$l_sig_name;
  };
  union  {
    int chf$is_sig_arg1;
    int chf$l_sig_arg1;
  };
};

#define CHF$M_FPREGS_VALID 0x1
	
struct _chfdef2 {
  union  {
    long long chf$q_mch_args;
    struct  {
      int chf$is_mch_args;
      union  {
	int chf$is_mch_flags;
	struct  {
	  unsigned chf$v_fpregs_valid : 1;
	  unsigned chf$v_fill_64_ : 7;
	};
      };
    };
  };
  long long chf$q_mch_frame;
  union  {
    long long chf$q_mch_depth;
    struct  {
      int chf$is_mch_depth;
      int chf$is_mch_resvd1;
    };
  };
  long long chf$q_mch_daddr;
  long long chf$q_mch_esf_addr;
  long long chf$q_mch_sig_addr;
  long long chf$q_mch_savr0;
  long long chf$q_mch_savr1;
  long long chf$q_mch_savr16;
  long long chf$q_mch_savr17;
  long long chf$q_mch_savr18;
  long long chf$q_mch_savr19;
  long long chf$q_mch_savr20;
  long long chf$q_mch_savr21;
  long long chf$q_mch_savr22;
  long long chf$q_mch_savr23;
  long long chf$q_mch_savr24;
  long long chf$q_mch_savr25;
  long long chf$q_mch_savr26;
  long long chf$q_mch_savr27;
  long long chf$q_mch_savr28;
  long long chf$q_mch_savf0;
  long long chf$q_mch_savf1;
  long long chf$q_mch_savf10;
  long long chf$q_mch_savf11;
  long long chf$q_mch_savf12;
  long long chf$q_mch_savf13;
  long long chf$q_mch_savf14;
  long long chf$q_mch_savf15;
  long long chf$q_mch_savf16;
  long long chf$q_mch_savf17;
  long long chf$q_mch_savf18;
  long long chf$q_mch_savf19;
  long long chf$q_mch_savf20;
  long long chf$q_mch_savf21;
  long long chf$q_mch_savf22;
  long long chf$q_mch_savf23;
  long long chf$q_mch_savf24;
  long long chf$q_mch_savf25;
  long long chf$q_mch_savf26;
  long long chf$q_mch_savf27;
  long long chf$q_mch_savf28;
  long long chf$q_mch_savf29;
  long long chf$q_mch_savf30;
  void *chf$ph_mch_sig64_addr;
};

#define CHF$M_CALEXT_CANCEL 0x1
#define CHF$M_CALEXT_LAST 0x2
#define CHF$K_CALEXT_LENGTH 16
#define CHF$C_CALEXT_LENGTH 16
	
struct _chfdef3 {
  int chf$l_calext_link;
  int chf$l_calext_address;
  union  {
    unsigned int chf$l_calext_flags;
    struct  {
      unsigned chf$v_calext_cancel : 1;
      unsigned chf$v_calext_last : 1;
      unsigned chf$v_fill_65_ : 6;
    };
  };
  unsigned int chf$l_calext_reserved;
};
 
struct _chfdef6 {
  union  {
    int chf64$l_sig_args;
    struct  {
      short int chf64$w_sig_arg_count;
      short int chf64$w_sig_msg_options;
    };
  };
  int chf64$l_signal64;
  long long chf64$q_sig_name;
  long long chf64$q_sig_arg1;
};
 
#define chf$arglist _chfdef
#define chf$signal_array _chfdef1
#define chf$mech_array _chfdef2
#define chf64$signal_array _chfdef6
#define CHF$ARGLIST CHFDEF
#define CHF$SIGNAL_ARRAY CHFDEF1
#define CHF$MECH_ARRAY CHFDEF2
#define CHF64$SIGNAL_ARRAY CHFDEF6

#endif
 
