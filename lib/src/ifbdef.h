#ifndef ifbdef_h
#define ifbdef_h

// Author. Roar Thronæs.

// based on / taken from rmsint2.doc

#include <fatdef.h>

struct _ifbdef {
  unsigned char irb$b_id;
  struct {
    unsigned irb$v_busy : 1;
    unsigned irb$v_iop 	: 1;
    unsigned irb$v_prv 	: 1;
    unsigned irb$v_eof 	: 1;
    unsigned irb$v_rwc 	: 1;
    unsigned irb$v_dfw 	: 1;
    unsigned irb$v_acc 	: 1;
    unsigned irb$v_ani 	: 1;
  };
  unsigned long ifb$l_irab_lnk ;
  unsigned char ifb$b_ubsz ;
  unsigned short ifb$w_chnl ;
  unsigned char ifb$b_facc;
  unsigned char ifb$b_dvch;
  unsigned long ifb$l_bdb;
  unsigned long ifb$l_ifab_lnk ;
  unsigned long long ifb$q_pool ;
  unsigned long ifb$l_mrn;

  struct _fatdef ifb$w_recattr;
  
};

#endif
