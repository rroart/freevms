#ifndef ihadef_h
#define ihadef_h

#define IHA$K_LENGTH 20
#define IHA$C_LENGTH 20
#define IHA$S_IHADEF 20
	
struct _iha {
  unsigned int iha$l_tfradr1;
  unsigned int iha$l_tfradr2;
  unsigned int iha$l_tfradr3;
  int ihadef$$_fill_1;
  unsigned int iha$l_inishr;
};
 
#endif
 
