#ifndef ftucbdef_h
#define ftucbdef_h

#include <ttyucbdef.h>

#define UCB$M_FT_BSY 0x1
#define UCB$M_FT_DELPEND 0x2
#define UCB$M_FT_DELETE_ACT 0x4
#define UCB$M_FT_INPUT_CRIT 0x8
#define UCB$C_FT_LENGTH 544
#define UCB$K_FT_LENGTH 544
	
#define UCB$S_FTUCBDEF 544
 
struct _ft_ucb {
  struct _tty_ucb ucb$r_ttyucb;
  unsigned int ucb$l_ft_ipid;
  void *ucb$l_ft_1st_page;
  void *ucb$l_ft_lst_page;
  void *ucb$l_ft_s0_page;
  unsigned unsigned long long ucb$q_ft_buffer_handle;
  union  {
    unsigned short int ucb$w_ft_sts;
    struct  {
      unsigned ucb$v_ft_bsy		: 1;
      unsigned ucb$v_ft_delpend		: 1;
      unsigned ucb$v_ft_delete_act	: 1;
      unsigned ucb$v_ft_input_crit	: 1;
      unsigned ucb$v_fill_0_		: 4;
    };
  };
  unsigned short int ucb$w_ft_chan;
  struct _ftrd *ucb$l_ft_readqfl;
  struct _ftrd *ucb$l_ft_readqbl;
  struct _ftrd *ucb$l_ft_curr_read;
  union  {
    struct  {
      struct _acb *ucb$l_ft_hangup_ast;
      struct _acb *ucb$l_ft_xon_ast;
      struct _acb *ucb$l_ft_bell_ast;
      struct _acb *ucb$l_ft_dc3_ast;
      struct _acb *ucb$l_ft_stop_ast;
      struct _acb *ucb$l_ft_resume_ast;
      struct _acb *ucb$l_ft_set_ast;
      struct _acb *ucb$l_ft_abort_ast;
      struct _acb *ucb$l_ft_start_read_ast;
      struct _acb *ucb$l_ft_middle_read_ast;
      struct _acb *ucb$l_ft_end_read_ast;
    };
    struct  {
      struct _fkb *ucb$l_ft_fqfl;
      struct _fkb *ucb$l_ft_fqbl;
      unsigned short int ucb$w_ft_frksize;
      unsigned char ucb$b_ft_frk_type;
      unsigned char ucb$b_ft_flck;
      void (*ucb$l_ft_fpc)();
      unsigned long long ucb$q_ft_fr3;
      unsigned long long ucb$q_ft_fr4;
    };
  };
};
	
#endif
 
