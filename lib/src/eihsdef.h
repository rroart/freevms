#ifndef eihsdef_h
#define eihsdef_h

#define EIHS$K_MAJORID 1
#define EIHS$K_MINORID 1
#define EIHS$K_LENGTH 32
#define EIHS$C_LENGTH 32
#define EIHS$S_EIHSDEF 32
	
struct _eihs {
  struct  {
    unsigned int eihs$l_majorid;
    unsigned int eihs$l_minorid;
  };
  unsigned int eihs$l_dstvbn;
  unsigned int eihs$l_dstsize;
  unsigned int eihs$l_gstvbn;
  unsigned int eihs$l_gstsize;
  unsigned int eihs$l_dmtvbn;
  unsigned int eihs$l_dmtbytes;
};

#endif
 
