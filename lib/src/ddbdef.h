#ifndef ddbdef_h
#define ddbdef_h

#define DDB$M_NO_TIMEOUT 0x1
#define DDB$M_PAC 0x2
#define DDB$K_PACK 1                    
#define DDB$K_CART 2                    
#define DDB$K_SLOW 3                    
#define DDB$K_TAPE 4                    
	
 
#define DDB$K_LENGTH 72                 
#define DDB$C_LENGTH 72                 
#define DDB$S_DDBDEF 72                 

struct _ddb {
  union  {
    int ddb$l_link;                 
    int ddb$l_blink;                 
    struct  {
      struct _ddb *ddb$ps_link;   
      struct _ddb *ddb$ps_blink;   
    };
  };
  union  {
    int ddb$l_ucb;                  
    struct  {
      struct _ucb *ddb$ps_ucb;    
    };
  };
  union  {
    unsigned short int ddb$w_size;  
    struct  {
      unsigned short int ddb$iw_size; 
    };
  };
  union  {
    unsigned char ddb$b_type;       
    struct  {
      unsigned char ddb$ib_type;  
    };
  };
  union  {                          
    unsigned char ddb$b_flags;
    struct  {
      unsigned ddb$v_no_timeout : 1; 
      unsigned ddb$v_pac : 1;     
      unsigned ddb$v_fill_22_ : 6;
    };
  };
  union  {
    int ddb$l_ddt;                  
    struct  {
      struct _ddt *ddb$ps_ddt;    
    };
  };
  union  {
    unsigned int ddb$l_acpd;        
    unsigned int ddb$il_acpd;       
    struct  {
      char ddbdef$$_fill_4 [3];
      union  {
	unsigned char ddb$b_acpclass; 
	struct  {
	  unsigned char ddb$ib_acpclass; 
	};
      };
    };
  };
  union  {
    char ddb$t_name [16];           
    struct  {
      union  {
	unsigned char ddb$b_name_len; 
	struct  {
	  unsigned char ddb$ib_name_len; 
	};
      };
      char ddb$t_name_str [15];   
    };
  };
  struct _dpt *ddb$ps_dpt;            
  struct _ddb *ddb$ps_drvlink;        
  union  {
    int ddb$l_sb;                   
    struct  {
      int ddb$ps_sb;              
    };
  };
  union  {
    int ddb$l_conlink;              
    struct  {
      struct _ddb *ddb$ps_conlink; 
    };
  };
  union  {
    unsigned int ddb$l_allocls;     
    struct  {
      unsigned int ddb$il_allocls; 
    };
  };
  union  {
    struct _ucb *ddb$l_2p_ucb;      
    struct _ucb *ddb$ps_2p_ucb;     
    struct _ucb *ddb$l_dp_ucb;      
  };
  union  {
    unsigned int ddb$l_port_id;     
    struct  {
      char ddb$t_port_id [1];     
    };
  };
  unsigned int ddb$l_class_lkid;      
  void *ddb$ps_2p_ddb;                
};

#endif 
