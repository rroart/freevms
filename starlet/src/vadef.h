#ifndef vadef_h
#define vadef_h

#define VA$M_P1 0x40000000
#define VA$M_SYSTEM 0x80000000
#define VA$M_BYTES_PER_PAGELET 0x1FF
#define VA$M_BYTE_IN_PAGELET 0x1FF
#define VA$S_VADEF 4
#define VA$C_PAGELET_SIZE 512
#define VA$C_PAGELET_SHIFT_SIZE 9
	
#define VA$C_P0 0                       
#define VA$C_P1 1                       
#define VA$C_P2 2                       
#define VA$M_DESCEND 0x1
#define VA$M_P0_SPACE 0x2
#define VA$M_P1_SPACE 0x4
#define VA$M_PERMANENT 0x8
#define VA$M_EXPAND_ON_ACCVIO 0x10
#define VA$M_SHARED_PTS 0x40
#define VA$M_OWNER_MODE 0xF
#define VA$M_CREATE_MODE 0xF0
#define VA$C_REGSUM_LENGTH 40
#define VA$_REGSUM_BY_ID 0
#define VA$_REGSUM_BY_VA 1
#define VA$_NEXT_REGSUM_BY_VA 2

#define VA$C_REGION_UCREATE_UOWN 51
#define VA$C_REGION_UCREATE_SOWN 50
#define VA$C_REGION_UCREATE_EOWN 49
#define VA$C_REGION_UCREATE_KOWN 48
#define VA$C_REGION_SCREATE_SOWN 34
#define VA$C_REGION_SCREATE_EOWN 33
#define VA$C_REGION_SCREATE_KOWN 32
#define VA$C_REGION_ECREATE_EOWN 17
#define VA$C_REGION_ECREATE_KOWN 16
#define VA$C_REGION_KCREATE_KOWN 0
	
#define VA$M_NO_OVERMAP 0x1
	
struct _va {
  union  {
    struct  {
      unsigned vadef$$_fill_1 : 30;
      unsigned va$v_p1 : 1;       
      unsigned va$v_system : 1;   
    };
    struct  {
      unsigned va$v_bytes_per_pagelet : 9; 
      unsigned va$v_fill_74_ : 7;
    };
    struct  {
      unsigned va$v_byte_in_pagelet : 9; 
      unsigned va$v_fill_75_ : 7;
    };
  };
};
 
struct _regsum {
  unsigned long long va$q_region_id;    
  union  {
    unsigned int va$l_flags;        
    struct  {
      unsigned va$v_descend : 1;  
      unsigned va$v_p0_space : 1; 
      unsigned va$v_p1_space : 1; 
      unsigned va$v_permanent : 1; 
      unsigned va$v_expand_on_accvio : 1; 
      unsigned va$v_no_clone : 1; 

      unsigned va$v_shared_pts : 1; 
      unsigned va$v_fill_76_ : 1;
    };
  };
  union  {
    int va$l_region_protection;
    struct  {
      unsigned va$v_owner_mode : 4; 
      unsigned va$v_create_mode : 4; 
    };
  };
  union  {
    void *va$pq_start_va;           
    void *va$ps_start_va;           
  };
  union  {
    unsigned long long va$q_region_size; 
    unsigned int va$l_region_size;  
  };
  union  {
    void *va$pq_first_free_va;      
    void *va$ps_first_free_va;      
  };
};
 
struct _va_flags {
  unsigned va$v_no_overmap : 1;       
  unsigned va$v_fill_77_ : 7;
};

typedef struct _regsum *regsum_pq;
 
#endif 
 
