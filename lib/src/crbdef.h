#ifndef crbdef_h
#define crbdef_h

#define CRB$M_XZA_CHAN0 0x1
#define CRB$M_XZA_CHAN1 0x2
#define CRB$M_XZA_ADPERR 0x4
#define CRB$M_BSY 0x1
#define CRB$M_UNINIT 0x2
#define CRB$K_VEC1_OFFSET 112           
#define CRB$K_LENGTH 128                
#define CRB$C_LENGTH 128                
	
#define CRB$S_CRBDEF 144                
 
struct _crb {
  struct _fkb *crb$l_fqfl;            
  struct _fkb *crb$l_fqbl;            
  unsigned short int crb$w_size;      
  unsigned char crb$b_type;           
  unsigned char crb$b_flck;           
  void (*crb$l_fpc)();                
  unsigned long crb$l_fr3;                  
  unsigned long crb$l_fr4;                  
  struct _fkb *crb$l_wqfl;            
  struct _fkb *crb$l_wqbl;            
  union  {
    union  {
      unsigned char crb$b_tt_type;
      unsigned int crb$l_tt_type; 
    };
    unsigned int crb$l_xza_sts;     
    struct  {
      unsigned crb$v_xza_chan0 : 1; 
      unsigned crb$v_xza_chan1 : 1; 
      unsigned crb$v_xza_adperr : 1; 
      unsigned crb$v_fill_0_ : 5;
    };
  };
  union  {
    unsigned int crb$l_refc;        
    unsigned short int crb$w_refc;
  };
  union  {
    unsigned char crb$b_mask;
    unsigned int crb$l_mask;        
    struct  {
      unsigned crb$v_bsy : 1;     
      unsigned crb$v_uninit : 1;  
      unsigned crb$v_fill_1_ : 6;
    };
  };
  void *crb$ps_busarray;              
  union  {
    long long crb$q_auxstruc;         
    unsigned int crb$l_auxstruc;
    void *crb$ps_auxstruc;
  };
  union  {
    long long crb$q_lan_struc;        
    unsigned int crb$l_lan_struc;
    void *crb$ps_lan_struc;
  };
  union  {                          
    long long crb$q_scs_struc;
    unsigned int crb$l_scs_struc;
    void *crb$ps_scs_struc;
  };
  union  {
    struct _crb *crb$l_timelink;    
    void *crb$l_tt_modem;           
  };
  unsigned int crb$l_node;            
  union  {
    unsigned int crb$l_duetime;     
    void *crb$ps_sysg_dblk;         
  };
  union  {
    void (*crb$l_toutrout)();       
    unsigned int crb$l_tt_timrefc;  
  };
  struct _spl *crb$ps_dlck;           
  struct _crb *crb$ps_crb_link;       
  union  {
    long long crb$q_ctrlr_shutdown;   
    int (*crb$ps_ctrlr_shutdown)();
  };
  unsigned int crb$l_intd [4];        
  unsigned int crb$l_intd2 [4];       
};
 
#endif 
 
