#ifndef mbxdef_h
#define mbxdef_h

#define MBX$M_ALLOC 0x1
#define MBX$M_VALID 0x2
#define MBX$M_DELPEND 0x4
#define MBX$M_QUOTALCK 0x8
#define MBX$K_LENGTH 48
#define MBX$C_LENGTH 48
#define MBX$S_MBXDEF 48
	
struct _mbx {
  unsigned long long mbx$q_msg;
  //  unsigned short int mbx$w_size;
  //  unsigned char mbx$b_type;
  union  {
    unsigned char mbx$b_flags;
    struct  {
      unsigned mbx$v_alloc : 1;
      unsigned mbx$v_valid : 1;
      unsigned mbx$v_delpend : 1;
      unsigned mbx$v_quotalck : 1;
      unsigned mbx$v_fill_0_ : 4;
    };
  };
  unsigned char mbx$b_creatport;
  unsigned short int mbx$w_unit;
  unsigned short int mbx$w_ref;
  unsigned short int mbx$w_reader;
  unsigned short int mbx$w_readast;
  unsigned short int mbx$w_writast;
  unsigned short int mbx$w_maxmsg;
  unsigned short int mbx$w_msgcnt;
  unsigned short int mbx$w_buffquo;
  unsigned short int mbx$w_prot;
  unsigned int mbx$l_ownuic;
  char mbx$t_name [16];
};

#endif
 
