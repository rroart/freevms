#ifndef orbdef_h
#define orbdef_h

#define ORB$M_PROT_16 0x1
#define ORB$M_ACL_QUEUE 0x2
#define ORB$M_MODE_VECTOR 0x4
#define ORB$M_NOACL 0x8
#define ORB$M_CLASS_PROT 0x10
#define ORB$M_NOAUDIT 0x20
#define ORB$M_MODE_VALID 0x80
#define ORB$M_PROFILE_LOCKED 0x100
#define ORB$M_INDIRECT_ACL 0x200
#define ORB$M_BOOTTIME 0x400
#define ORB$M_UNMODIFIED 0x800
#define ORB$M_DAMAGED 0x1000
#define ORB$M_TEMPLATE 0x2000
#define ORB$M_TRANSITION 0x4000
#define ORB$M_EXT_NAMEBLOCK 0x8000
#define ORB$K_LENGTH 124               
#define ORB$C_LENGTH 124               
#define ORB$K_DEVNAM_LENGTH 64         
#define ORB$C_DEVNAM_LENGTH 64         
#define ORB$S_ORBDEF 125               
	
struct _orb {
  union  {
    unsigned int orb$l_owner;      
    struct  {
      unsigned short int orb$w_uicmember;
      unsigned short int orb$w_uicgroup;
    };
  };
  unsigned int orb$l_acl_mutex;      
  unsigned short int orb$w_size;     
  unsigned char orb$b_type;          
  unsigned char orb$b_subtype;       
  union  {
    union  {
      unsigned short int orb$w_flags;
      struct  {
	unsigned char orb$b_flags_1;
	unsigned char orb$b_flags_2;
      };
    };
    unsigned char orb$b_flags;     
    union  {
      unsigned short int orb$w_flag_bits;
      struct  {
	unsigned orb$v_prot_16		: 1;
	unsigned orb$v_acl_queue	: 1;
	unsigned orb$v_mode_vector	: 1;
	unsigned orb$v_noacl		: 1;
	unsigned orb$v_class_prot	: 1;
	unsigned orb$v_noaudit		: 1;
	unsigned orb$v_reserved_1	: 1;
	unsigned orb$v_mode_valid	: 1;
	unsigned orb$v_profile_locked	: 1;
	unsigned orb$v_indirect_acl	: 1;
	unsigned orb$v_boottime		: 1;
	unsigned orb$v_unmodified	: 1;
	unsigned orb$v_damaged		: 1;
	unsigned orb$v_template		: 1;
	unsigned orb$v_transition	: 1;
	unsigned orb$v_ext_nameblock	: 1;
      };
    };
  };
  unsigned short int orb$w_refcount; 
  union  {
    union  {
      unsigned unsigned long long orb$q_mode_prot;
      struct  {
	unsigned int orb$l_mode_protl;
	unsigned int orb$l_mode_proth;
      };
    };
    unsigned int orb$l_mode;       
  };
  union  {
    unsigned int orb$l_sys_prot;   
    unsigned short int orb$w_prot; 
  };
  unsigned int orb$l_own_prot;       
  unsigned int orb$l_grp_prot;       
  unsigned int orb$l_wor_prot;       
  union  {
    struct _ace *orb$l_aclfl;      
    unsigned int orb$l_acl_count;  
  };
  union  {
    struct _ace *orb$l_aclbl;      
    void *orb$l_acl_desc;          
  };
  struct  {
    char orb$b_fill_2 [20];        
  };
  struct  {
    char orb$b_fill_3 [20];        
  };
  unsigned short int orb$w_name_length;
  short int orb$w_fill_3;            
  void *orb$l_name_pointer;          
  struct _ocb *orb$l_ocb;            
  struct _orb *orb$l_template_orb;   
  int orb$l_object_specific;         
  struct _orb *orb$l_original_orb;   
  unsigned int orb$l_updseq;         
  void *orb$l_mutex_address;         
  int orb$l_reserve2;                
  char orb$t_object_name;            
};

#endif
 
