#ifndef pdscdef_h
#define pdscdef_h

#define PDSC$K_KIND_BOUND 0             
#define PDSC$K_KIND_NULL 8              
#define PDSC$K_KIND_FP_STACK 9          
#define PDSC$K_KIND_FP_REGISTER 10      
#define PDSC$M_HANDLER_VALID 0x10
#define PDSC$M_HANDLER_REINVOKABLE 0x20
#define PDSC$M_HANDLER_DATA_VALID 0x40
#define PDSC$M_BASE_REG_IS_FP 0x80
#define PDSC$M_REI_RETURN 0x100
#define PDSC$M_STACK_RETURN_VALUE 0x200
#define PDSC$M_BASE_FRAME 0x400
#define PDSC$M_TARGET_INVO 0x800
#define PDSC$M_NATIVE 0x1000
#define PDSC$M_NO_JACKET 0x2000
#define PDSC$M_TIE_FRAME 0x4000
#define PDSC$M_FUNC_RETURN 0xF
#define PDSC$M_EXCEPTION_MODE 0x70
#define PDSC$K_EXC_MODE_SIGNAL 0        
#define PDSC$K_EXC_MODE_SIGNAL_ALL 1    
#define PDSC$K_EXC_MODE_SIGNAL_SILENT 2 
#define PDSC$K_EXC_MODE_FULL_IEEE 3     
#define PDSC$K_EXC_MODE_CALLER 4        
#define PDSC$K_NULL_SIZE 16             
#define PDSC$K_BOUND_SIZE 24            
#define PDSC$K_MIN_BOUND_SIZE 24        
#define PDSC$K_MIN_LENGTH_SF 32         
#define PDSC$K_MIN_STACK_SIZE 32        
#define PDSC$K_MAX_STACK_SIZE 48        
#define PDSC$K_MIN_LENGTH_RF 24         
#define PDSC$K_MIN_REGISTER_SIZE 24     
#define PDSC$K_MAX_REGISTER_SIZE 40     
#define PDSC$K_BOUND_ENVIRONMENT_SIZE 32 
	
 struct _pdscdef {
  union  {
    unsigned short int pdsc$w_flags; 
    struct  {
      unsigned pdsc$v_kind : 4;   
      unsigned pdsc$v_handler_valid : 1; 
      unsigned pdsc$v_handler_reinvokable : 1; 
      unsigned pdsc$v_handler_data_valid : 1; 
      unsigned pdsc$v_base_reg_is_fp : 1; 
      unsigned pdsc$v_rei_return : 1; 
      unsigned pdsc$v_stack_return_value : 1; 
      unsigned pdsc$v_base_frame : 1; 
      unsigned pdsc$v_target_invo : 1; 
      unsigned pdsc$v_native : 1; 
      unsigned pdsc$v_no_jacket : 1; 
      unsigned pdsc$v_tie_frame : 1; 
      unsigned pdsc$v_flags_mbz2 : 1; 
    };
  };
  union  {
    short int pdsc$w_rsa_offset;    
    struct  {
      unsigned char pdsc$b_save_fp; 
      unsigned char pdsc$b_save_ra; 
    };
  };
  char pdsc$b_entry_ra;               
  struct  {
    unsigned pdsc$v_func_return : 4; 
    unsigned pdsc$v_exception_mode : 3; 
    unsigned pdsc$v_more_flags_mbz1 : 1;
  };
  short int pdsc$w_signature_offset;  
  union  {
    long long pdsc$q_entry;           
    struct  {
      int pdsc$l_entry;           
    };
  };
  union  {                          
    unsigned int pdsc$l_size;       
    union  {
      long long pdsc$q_proc_value;  
      struct  {
	int pdsc$l_proc_value;  
	int pdsc$l_reserved1;   
      };
    };
  };
  union  {
    struct  {                     
      unsigned int pdsc$l_ireg_mask; 
      unsigned int pdsc$l_freg_mask; 
      long long pdsc$q_stack_handler; 
      long long pdsc$q_stack_handler_data; 
    };
    struct  {                     
      long long pdsc$q_reg_handler; 
      long long pdsc$q_reg_handler_data; 
    };
    union  {                      
      int pdsc$l_environment;
      long long pdsc$q_environment;
    };
  };
};
 
#define PDSC$K_LKP_LENGTH 16            
	
struct _pdsc_lkpdef {           
  union  {
    long long pdsc$q_lkp_entry;       
    struct  {
      void *pdsc$ps_lkp_entry;
    };
  };
  union  {
    long long pdsc$q_lkp_proc_value;  
    struct  {
      void *pdsc$ps_lkp_proc_value;
    };
  };
};
 
#define LKP$K_SIZE 16                   
	
struct _lkpdef {                
  union  {                          
    long long lkp$q_entry;
    void *lkp$ps_entry;
  };
  union  {                          
    long long lkp$q_proc_value;
    void *lkp$ps_proc_value;
  };
};
 
#endif 
 
