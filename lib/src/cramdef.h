#ifndef cramdef_h
#define cramdef_h
	
#define CRAM$M_IN_USE 0x1
#define CRAM$M_DER 0x2
#define CRAM$M_CMD_VAL 0x3FFFFFFF
#define CRAM$M_CMD_BRIDGE 0x40000000
#define CRAM$M_CMD_WRITE 0x80000000
#define CRAM$M_MBX_DONE 0x1
#define CRAM$M_MBX_ERROR 0x2
#define CRAM$K_LENGTH 128               
#define CRAM$S_CRAMDEF 128              

#define CRAMCMD$K_RDQUAD32 1            
#define CRAMCMD$K_RDLONG32 2            
#define CRAMCMD$K_RDWORD32 3            
#define CRAMCMD$K_RDBYTE32 4            
#define CRAMCMD$K_WTQUAD32 5            
#define CRAMCMD$K_WTLONG32 6            
#define CRAMCMD$K_WTWORD32 7            
#define CRAMCMD$K_WTBYTE32 8            
#define CRAMCMD$K_RDQUAD64 9            
#define CRAMCMD$K_RDLONG64 10           
#define CRAMCMD$K_RDWORD64 11           
#define CRAMCMD$K_RDBYTE64 12           
#define CRAMCMD$K_WTQUAD64 13           
#define CRAMCMD$K_WTLONG64 14           
#define CRAMCMD$K_WTWORD64 15           
#define CRAMCMD$K_WTBYTE64 16           
#define CRAMCMD$K_RDTRIBYTE32 17        
#define CRAMCMD$K_WTTRIBYTE32 18        
#define CRAMCMD$K_RDTRIBYTE64 19        
#define CRAMCMD$K_WTTRIBYTE64 20        
#define CRAMCMD$K_MININDEX 1
#define CRAMCMD$K_MAXINDEX 20
	
#define HW_CRAM$M_MBX_DONE 0x1
#define HW_CRAM$M_MBX_ERROR 0x2
	
#define HW_CRAM$K_LENGTH 64             
#define HW_CRAM$S_HW_CRAMDEF 64         
	
#define CMDTABLEHEADER$K_LENGTH 12
	
#define CMDTABLE$K_LENGTH 96            
#define CMDTABLE$S_CMDTABLEDEF 96       
 
struct _cram {
  struct _cram *cram$l_flink;         
  struct _cram *cram$l_blink;         
  unsigned short int cram$w_size;     
  unsigned char cram$b_type;          
  unsigned char cram$b_subtype;       
  void *cram$l_mbpr;                  
  unsigned long long cram$q_hw_mbx;     
  unsigned long long cram$q_queue_time; 
  unsigned long long cram$q_wait_time;  
  unsigned int cram$l_driver;         
  struct _idb *cram$l_idb;            
  struct _ucb *cram$l_ucb;            
  union  {
    union  {
      unsigned int cram$l_cram_flags; 
      struct  {
	unsigned cram$v_in_use : 1; 
	unsigned cram$v_der : 1; 
	unsigned cram$v_fill_6_ : 6;
      };
    };
  };
  struct _adp *cram$l_adp;            
  char cram$b_filler1 [4];

  union  {
    unsigned int cram$l_command;    
    union  {
      unsigned int cram$l_cmd_bits;
      struct  {
	unsigned cram$v_cmd_val : 30; 
	unsigned cram$v_cmd_bridge : 1; 
	unsigned cram$v_cmd_write : 1; 
      };
    };
  };
  unsigned char cram$b_byte_mask;     
  unsigned char cram$b_filler2;       
  unsigned char cram$b_hose;          
  unsigned char cram$b_filler3;       
  unsigned long long cram$q_rbadr;      
  union  {
    unsigned long long cram$q_wdata;  
    unsigned int cram$l_wdata;      
    unsigned short int cram$w_wdata; 
    unsigned char cram$b_wdata;     
  };
  unsigned long long cram$q_filler4;    
  union  {
    unsigned long long cram$q_rdata;  
    unsigned int cram$l_rdata;
    unsigned short int cram$w_rdata;
    unsigned char cram$b_rdata;
  };
  union  {
    union  {
      unsigned short int cram$w_mbx_flags; 
      struct  {
	unsigned cram$v_mbx_done : 1; 
	unsigned cram$v_mbx_error : 1; 
	unsigned cram$v_fill_7_ : 6;
      };
    };
  };
  unsigned short int cram$w_error_bits [3]; 
  unsigned long long cram$q_filler5 [2]; 
};
 
struct _hw_cram {
  unsigned int hw_cram$l_command;     
  unsigned char hw_cram$b_byte_mask;  
  unsigned char hw_cram$b_filler6;    
  unsigned char hw_cram$b_hose;       
  unsigned char hw_cram$b_filler7;    
  unsigned long long hw_cram$q_rbadr;   
  unsigned long long hw_cram$q_wdata;   
  unsigned long long hw_cram$q_filler8; 
  unsigned long long hw_cram$q_rdata;   
  union  {
    union  {
      unsigned short int hw_cram$w_mbx_flags; 
      struct  {
	unsigned hw_cram$v_mbx_done : 1; 
	unsigned hw_cram$v_mbx_error : 1; 
	unsigned hw_cram$v_fill_10_ : 6;
      };
    };
  };
  unsigned short int hw_cram$w_error_bits [3]; 
  unsigned long long hw_cram$q_filler9 [2]; 
};
 
struct _cmdtable {
  struct _adp *cmdtable$ps_adp;
  unsigned int cmdtable$l_bus_type;   
  unsigned short int cmdtable$w_size; 
  unsigned char cmdtable$b_type;      
  unsigned char cmdtable$b_subtype;   
  //  char cmdtable$l_cmd_vector[];
  #define cmdtable$l_cmd_vector cmdtable$r_cmdarray
  struct  {
    unsigned int cramcmd$l_fill2;   
    unsigned int cramcmd$l_rdquad32; 
    unsigned int cramcmd$l_rdlong32; 
    unsigned int cramcmd$l_rdword32; 
    unsigned int cramcmd$l_rdbyte32; 
    unsigned int cramcmd$l_wtquad32; 
    unsigned int cramcmd$l_wtlong32; 
    unsigned int cramcmd$l_wtword32; 
    unsigned int cramcmd$l_wtbyte32; 
    unsigned int cramcmd$l_rdquad64; 
    unsigned int cramcmd$l_rdlong64; 
    unsigned int cramcmd$l_rdword64; 
    unsigned int cramcmd$l_rdbyte64; 
    unsigned int cramcmd$l_wtquad64; 
    unsigned int cramcmd$l_wtlong64; 
    unsigned int cramcmd$l_wtword64; 
    unsigned int cramcmd$l_wtbyte64; 
    unsigned int cramcmd$l_rdtribyte32; 
    unsigned int cramcmd$l_wttribyte32; 
    unsigned int cramcmd$l_rdtribyte64; 
    unsigned int cramcmd$l_wttribyte64; 
  } cmdtable$r_cmdarray;;
};
 
#endif 
 
