#ifndef prcpoldef_h
#define prcpoldef_h

#define PRCPOL$C_SIZ 56
#define PRCPOL$S_PRCPOLDEF 56
	
struct _prcpol {
  unsigned int prcpol$l_sysidl;
  unsigned short int prcpol$w_sysidh;
  unsigned short int prcpol$w_fill_1;
  char prcpol$t_nodnam [16];
  unsigned char prcpol$b_prcnam [16];
  unsigned char prcpol$b_dirinf [16];
};
 
#endif
 
