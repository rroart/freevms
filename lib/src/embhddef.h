#ifndef embhddef_h
#define embhddef_h

#define EMB$K_HD_REV_V50 2
#define EMB$K_HD_REV_V51 3
#define EMB$K_HD_REV_HICKORY 4
#define EMB$K_HD_REV_V10 5
#define EMB$K_HD_REV_V20 6
#define EMB$K_HD_REV_V30 7

#define EMB$C_HD_REV_V50 2
#define EMB$C_HD_REV_V51 3
#define EMB$C_HD_REV_HICKORY 4
#define EMB$C_HD_REV_V10 5
#define EMB$C_HD_REV_V20 6
#define EMB$C_HD_REV_V30 7

#define EMB$C_OS_RESERVED 0
#define EMB$C_OS_VAXVMS 1
#define EMB$C_OS_VAXELN 2
#define EMB$C_OS_ALPHAVMS 3

#define EMB$K_OS_RESERVED 0
#define EMB$K_OS_VAXVMS 1
#define EMB$K_OS_VAXELN 2
#define EMB$K_OS_ALPHAVMS 3

typedef char SCS_NAME_LEN [16];         

#define EMB$C_SCS_NAME_LENGTH 16        
#define EMB$K_SCS_NAME_LENGTH 16        
#define EMB$K_HD_LENGTH 96              
#define EMB$C_HD_LENGTH 96              
	
#define EMB$K_LENGTH 16                 
#define EMB$S_EMBHDDEF 112              
	
#define EMB$M_EM_BUS 0x1
#define EMB$M_EM_CPU 0x2
#define EMB$M_EM_MEMORY 0x4
#define EMB$M_EM_ADAPTER 0x8
#define EMB$M_EM_CACHE 0x10
#define EMB$M_EM_VECTOR 0x20
#define EMB$M_RSRVD1 0x40
	
#define EMB$M_FL_DDR 0x1
#define EMB$M_FL_OVWRT 0x2
	
struct _embhd_nonfixed {
  unsigned int emb$l_hd_sid;          
  unsigned short int emb$w_hd_hdr_rev; 
  union  {                          
    unsigned int emb$l_hd_systype;  
    struct  {
      unsigned short int emb$w_hd_xsid_rsv; 
      unsigned char emb$b_hd_xsid_rev; 
      unsigned char emb$b_hd_xsid_typ; 
    };
  };
  unsigned int emb$l_cpuid;           
  unsigned char emb$b_dev_class;      
  unsigned char emb$b_dev_type;       
  SCS_NAME_LEN emb$t_scs_name;        
  unsigned short int emb$w_flags;     
  unsigned char emb$b_os_id;          
  unsigned char emb$b_hdrsz;          
  union  {
    unsigned short int emb$w_hd_entry; 
    struct  {
      unsigned char emb$b_devtyp; 
      unsigned char emb$b_devcls; 
    };
  };
  unsigned long long emb$q_hd_time;     
  unsigned short int emb$w_hd_errseq; 
  unsigned long long emb$q_hd_swvers;   
  unsigned int emb$l_hd_errmsk;       
  unsigned int emb$l_hd_abstim;       
  unsigned char emb$b_hd_hw_name_len; 
  char emb$t_hd_hw_name [31];         
};
 
struct _embhd { 
  struct _embhd_nonfixed emb$r_embhd_nonfixed; 
};
	
struct _embtrailer {
  int emb$l_tr_spare1;
  int emb$l_tr_spare2;
  int emb$l_tr_spare3;
  int emb$l_tr_actual_size;
  unsigned int emb$l_tr_active_cpus;
  unsigned int emb$l_tr_logging_cpu;
  unsigned long long emb$q_tr_tdf;
};

union _errmsk_fields {
  int emb$l_errmsk;
  struct  {
    unsigned emb$v_em_bus : 1;      
    unsigned emb$v_em_cpu : 1;      
    unsigned emb$v_em_memory : 1;   
    unsigned emb$v_em_adapter : 1;  
    unsigned emb$v_em_cache : 1;    
    unsigned emb$v_em_vector : 1;   
    unsigned emb$v_rsrvd1 : 1;      
    unsigned emb$v_rsrvd : 25;
  };
};
 
union _flags_fields {
  int emb$l_flags;
  struct  {
    unsigned emb$v_fl_ddr : 1;      
    unsigned emb$v_fl_ovwrt : 1;    
    unsigned emb$v_rsrvd : 14;
  };
};
 
#endif 
 
