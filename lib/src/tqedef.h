#ifndef tqedef_h
#define tqedef_h
 
#define TQE$M_TQTYPE 0x3
#define TQE$M_REPEAT 0x4
#define TQE$M_ABSOLUTE 0x8
#define TQE$M_CHK_CPUTIM 0x10
#define TQE$M_EXTENDED_FORMAT 0x20
#define TQE$M_RSRVD_6_7 0xC0
#define TQE$C_TMSNGL 0
#define TQE$C_SSREPT 5
#define TQE$C_SSSNGL 1
#define TQE$C_WKREPT 6
#define TQE$C_WKSNGL 2
#define TQE$S_TQEDEF 64

#define TQE$K_LENGTH 64
#define TQE$C_LENGTH 64

struct _tqe {
  struct _tqe *tqe$l_tqfl;
  struct _tqe *tqe$l_tqbl;
  unsigned short int tqe$w_size;
  unsigned char tqe$b_type;
  unsigned char tqe$b_rqtype;
  union  {
    unsigned long tqe$l_pid;
    long tqe$l_fpc;
  } tqe$r_pid_union;
  union  {
    struct  {
      void (*tqe$l_ast)(unsigned long);
      unsigned long tqe$l_astprm;
    } tqe$r_ast_fields;
    struct {
      unsigned long tqe$q_fr3;
      unsigned long tqe$q_fr4;
    } tqe$r_fr_fields;
  } tqe$r_ast_union;
  unsigned long long tqe$q_time;
  unsigned long long tqe$q_delta;
  unsigned char tqe$b_rmod;
  unsigned char tqe$b_efn;
  unsigned short tqe$w_res;
  unsigned int tqe$l_rqpid;
  unsigned int tqe$l_cputim;
};

#define tqe$l_pid tqe$r_pid_union.tqe$l_pid
#define tqe$l_fpc tqe$r_pid_union.tqe$l_fpc
#define tqe$q_fr3 tqe$r_ast_union.tqe$r_fr_fields.tqe$q_fr3
#define tqe$q_fr4 tqe$r_ast_union.tqe$r_fr_fields.tqe$q_fr4
#define tqe$l_astprm tqe$r_ast_union.tqe$r_ast_fields.tqe$l_astprm
#define tqe$l_ast tqe$r_ast_union.tqe$r_ast_fields.tqe$l_ast

#endif
 
