#ifndef crctxdef_h
#define crctxdef_h
 
#define CRCTX$M_HIGH_PRIO 0x1
#define CRCTX$M_ITEM_VALID 0x2
	
#define CRCTX$K_LENGTH 96               
#define CRCTX$S_CRCTXDEF 96             
	
#define CRCTX_BUF$K_LENGTH 32

struct _crctx {
  struct _crctx *crctx$l_flink;       
  struct _crctx *crctx$l_blink;       
  unsigned short int crctx$w_size;    
  unsigned char crctx$b_type;         
  unsigned char crctx$b_subtype;      
  struct _crab *crctx$l_crab;         
  union  {
    unsigned char crctx$b_flck;     
    unsigned int crctx$l_flck;      
  };
  union  {                          
    int crctx$l_flags;              
    struct  {                     
      unsigned crctx$v_high_prio : 1; 
      unsigned crctx$v_item_valid : 1; 
      unsigned crctx$v_fill_0_ : 6;
    };
  };
  void *crctx$l_wqfl;                 
  void *crctx$l_wqbl;                 
  union  {
    int crctx$l_context1;
    long long crctx$q_context1;
  };
  union  {
    int crctx$l_context2;
    long long crctx$q_context2;
  };
  union  {
    int crctx$l_context3;
    long long crctx$q_context3;
  };
  int crctx$l_item_cnt;               
  int crctx$l_item_num;               
  int crctx$l_up_bound;               
  int crctx$l_low_bound;              
  int (*crctx$l_callback)();          
  int (*crctx$l_saved_callback)();    
  void *crctx$l_aux_context;          
  int crctx$l_reserved1;              
  int crctx$l_dma_adr;                
  void *crctx$ps_caller_pc;           
};
 
struct _crctx_buf {
  unsigned int crctx_buf$l_xaction;   
  unsigned int crctx_buf$l_item_num;  
  unsigned int crctx_buf$l_item_cnt;  
  struct _crctx *crctx_buf$ps_crctx;  
  void *crctx_buf$ps_caller_pc;       
  unsigned int crctx_buf$l_status;    
  unsigned int crctx_buf$l_dma_addr;  
  unsigned int crctx_buf$l_count;     
};
	
struct _crctx_buf_h {
  long long crctx_buf_h$q_free;         
  unsigned int crctx_buf_h$l_size;    
  int crctx_buf_h$l_reserved;
  int crctx_buf_h$l_count;            
  int crctx_buf_h$l_reserved2;
};
 
#endif 
 
