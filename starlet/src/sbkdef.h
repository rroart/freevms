#ifndef sbkdef_h
#define sbkdef_h

#define SBK$K_LENGTH 32
#define SBK$C_LENGTH 32
	
struct _sbkdef {
  union  {
    unsigned int sbk$l_stlbn;
    struct  {
      unsigned short int sbk$w_stlbnh;
      unsigned short int sbk$w_stlbnl;
    };
  };
  union  {
    unsigned int sbk$l_filesize;
    struct  {
      unsigned short int sbk$w_filesizh;
      unsigned short int sbk$w_filesizl;
    };
  };
  unsigned char sbk$b_acnt;
  unsigned char sbk$b_lcnt;
  unsigned int sbk$l_fcb;
  short int sbkdef$$_fill_1;
  unsigned short int sbk$w_acnt;
  unsigned short int sbk$w_lcnt;
  unsigned short int sbk$w_wcnt;
  unsigned short int sbk$w_tcnt;
  unsigned int sbk$l_reads;
  unsigned int sbk$l_writes;
};
 
#endif
 
