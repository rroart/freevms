#ifndef ihvndef_h
#define ihvndef_h

#define IHVN$M_SUBVERSION_MINOR_ID 0xFFFF
#define IHVN$M_SUBVERSION_MAJOR_ID 0xFFFF0000
	
struct _img_version_array {
  unsigned int ihvn$l_subsystem_mask;
  union  {
    unsigned int ihvn$l_subversion_array;
    struct  {
      unsigned ihvn$v_subversion_minor_id : 16;
      unsigned ihvn$v_subversion_major_id : 16;
    };
  };
};
 
#define IHVN$M_VERSION_MINOR_ID 0xFFFFFF
#define IHVN$M_VERSION_MAJOR_ID 0xFF000000
	
struct _img_overall_version {
  union  {
    int ihvn$l_version_bits;
    struct  {
      unsigned ihvn$v_version_minor_id : 24;
      unsigned ihvn$v_version_major_id : 8;
    };
  };
};

#endif
 
