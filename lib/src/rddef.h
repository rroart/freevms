#ifndef rddef_h
#define rddef_h

#define RD$K_LENGTH 8
#define RD$C_LENGTH 8
#define RD$S_RDDEF 8
 
struct _scs_rd {
  union  {
    void *rd$l_cdrp;
    struct _rd *rd$l_link;
  } rd$r_cdrp_ovl;
  union  {
    unsigned short int rd$w_state;
    struct  {
      unsigned rd$v_busy    : 1;
      unsigned rd$v_perm    : 1;
      unsigned rd$v_fill_0_ : 6;
    } rd$r_state_bits;
  } rd$r_state_ovl;
  unsigned short int rd$w_seqnum;
};

#define		rd$l_cdrp		rd$r_cdrp_ovl.rd$l_cdrp
#define		rd$l_link		rd$r_cdrp_ovl.rd$l_link
#define		rd$v_busy		rd$r_state_ovl.rd$r_state_bits.rd$v_busy
#define		rd$v_perm		rd$r_state_ovl.rd$r_state_bits.rd$v_perm
#define		rd$w_state		rd$r_state_ovl.rd$w_state

#endif

