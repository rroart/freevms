#ifndef seciddef_h
#define seciddef_h

#define SECID$M_MINOR_ID 0xFFFFFF
#define SECID$M_MAJOR_ID 0xFF000000
	
#define SECID$K_LENGTH 8
#define SECID$C_LENGTH 8
	
struct _secid {
  unsigned int secid$l_match;
  union  {
    unsigned int secid$l_version;
    struct  {
      unsigned secid$v_minor_id : 24;
      unsigned secid$v_major_id : 8;
    };
  };
};
 
#endif
 
