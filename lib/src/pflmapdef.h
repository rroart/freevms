#ifndef pflmapdef_h
#define pflmapdef_h

#define PFLMAP$M_PGFLPAG 0xFFFFFF
#define PFLMAP$M_PGFLX 0xFF000000
#define PFLMAP$M_PAGCNT 0xFFFFFFFF00000000
	
struct _pflmapentry {
  unsigned pflmap$v_pgflpag : 24;
  unsigned pflmap$v_pgflx : 8;
  unsigned pflmap$v_pagcnt : 32;
};

#define PFLMAP$C_SHIFT_COUNT 6
#define PFLMAP$C_MAXPTRS 64
#define PFLMAP$K_LENGTH 536
#define PFLMAP$C_LENGTH 536
#define PFLMAP$S_PFLMAPDEF 536
	
struct _pflmap {
  unsigned int pflmap$l_pagecnt;
  int pflmap$$_fill_2;
  unsigned short int pflmap$w_size;
  unsigned char pflmap$b_type;
  unsigned char pflmap$b_actptrs;
  int pflmap$$_fill_3;
  struct _pflmapentry pflmap$q_ptr [65];
};
 
#endif
 
