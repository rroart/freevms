#ifndef ifbdef_h
#define ifbdef_h

// Author. Roar Thronæs.

// based on / taken from rmsint2.doc

struct _irbdef {
  unsigned char irb$b_id;
  struct {
    unsigned irb$v_busy : 1;
    unsigned irb$v_iop 	: 1;
    unsigned irb$v_dup 	: 1;
    union {
      unsigned irb$v_ssk 	: 1;
      unsigned irb$v_eof 	: 1;
    };
    unsigned irb$v_seq 	: 1;
    unsigned irb$v_blk 	: 1;
    union {
      unsigned irb$v_idx 	: 1;
      unsigned irb$v_find 	: 1;
    };
    unsigned irb$v_asy 	: 1;
  };
  unsigned long irb$l_irab_lnk ;
  unsigned long irb$l_ifab_lnk ;
  unsigned long irb$l_curbdb ;
  unsigned long irb$l_nxtbdb ;
  unsigned long long irb$q_nrp ;
  unsigned char irb$b_rp_id;
  unsigned char irb$b_rp_kref;
  unsigned short irb$w_offset;
  unsigned char irb$b_rrv_id;
  unsigned char irb$b_bcnt;
  unsigned short irb$w_mbc;
  unsigned long irb$l_rrv;
  unsigned long irb$l_nkad;
  unsigned short irb$w_spos;
};

#endif
