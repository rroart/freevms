#ifndef rdefef_h
#define rdedef_h

#define REGPRT$M_OWNER_MODE         0xF
#define REGPRT$M_CREATE_MODE        0xF0
#define REGPRT$M_RESERVED_PROT_BITS 0xFFFFFF00
	
#define RDE$M_DESCEND 0x1
#define RDE$M_P0_SPACE 0x2
#define RDE$M_P1_SPACE 0x4
#define RDE$M_PERMANENT 0x8
#define RDE$M_EXPAND_ON_ACCVIO 0x10
#define RDE$M_NO_CLONE 0x20
#define RDE$M_SHARED_PTS 0x40
#define RDE$M_RESERVED_FLAGS 0xFFFFFF80
#define RDE$C_LENGTH 56                 
	
#define RDE$C_MIN_USER_ID 16            
#define RDE$C_REGION_TABLE_SIZE 16      

#define LOOKUP_RDE_EXACT 0
#define LOOKUP_RDE_HIGHER 1
 
union _region_prot {
  int regprt$l_region_prot;
  struct  {
    unsigned regprt$v_owner_mode         : 4; 
    unsigned regprt$v_create_mode        : 4; 
    unsigned regprt$v_reserved_prot_bits : 24;
  };
};
 
struct _rde {
  struct _rde *rde$ps_va_list_flink;  
  struct _rde *rde$ps_va_list_blink;  
  unsigned short rde$w_size;      
  unsigned char rde$b_type;           
  unsigned char rde$b_subtype;        
  struct _rde *rde$ps_table_link;     
  union  {
    unsigned int rde$l_flags;       
    struct  {
      unsigned rde$v_reserved_flags : 25; // really belongs at the bottom
      unsigned rde$v_descend : 1; 
      unsigned rde$v_p0_space : 1; 
      unsigned rde$v_p1_space : 1; 
      unsigned rde$v_permanent : 1; 
      unsigned rde$v_expand_on_accvio : 1; 
      unsigned rde$v_no_clone : 1; 
      unsigned rde$v_shared_pts : 1; 
    };
  };
  union _region_prot rde$r_regprot;          
  unsigned long long rde$q_region_id;   
  union  {
    void *rde$pq_start_va;          
    void *rde$ps_start_va;          
  };
  union  {
    unsigned long long rde$q_region_size; 
    unsigned int rde$l_region_size; 
  };
  union  {
    void *rde$pq_first_free_va;     
    void *rde$ps_first_free_va;     
  };
};
 
#endif 
 
