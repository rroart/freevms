#ifndef sbnbdef_h
#define sbnbdef_h

#define SBNB$K_LENGTH 36
#define SBNB$C_LENGTH 36
#define SBNB$S_SBNBDEF 36
	
typedef struct _sbnb {
  struct _sbnb *sbnb$l_flink;
  struct _sbnb *sbnb$l_blink;
  unsigned short int sbnb$w_size;
  unsigned char sbnb$b_type;
  unsigned char sbnb$b_subtyp;
  unsigned int sbnb$l_discon_count;
  unsigned char sbnb$b_procnam [16];
  unsigned short int sbnb$w_local_index;
  unsigned short int sbnb$w_unused_1;
};
 
#endif
 
