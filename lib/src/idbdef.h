#ifndef idbdef_h
#define idbdef_h

#define		IDB$M_CRAM_ALLOC	0x1
#define		IDB$M_VLE		0x2
#define		IDB$M_NORESIZE		0x4
#define		IDB$M_MCJ		0x8
#define		IDB$M_SHARED_INT	0x10
#define		IDB$M_DISTRIBUTED_INT	0x20
#define		IDB$M_ISR_CALLABLE	0x40
#define		IDB$K_BASE_LENGTH	56
#define		IDB$C_BASE_LENGTH	56

#define		IDB$K_LENGTH		88
#define		IDB$C_LENGTH		88                 
#define		IDB$S_IDBDEF		88                 
 
struct _idb {
  unsigned long long idb$q_csr;         
  unsigned short int idb$w_size;      
  unsigned char idb$b_type;           
  char idb$b_spare;
  unsigned short int idb$w_units;     
  unsigned short int idb$w_tt_enable; 
  struct _ucb *idb$ps_owner;          
  struct _cram *idb$ps_cram;          
  struct _spl *idb$ps_spl;            
  struct _adp *idb$ps_adp;            
  union  {
    unsigned int idb$l_flags;       
    struct  {
      unsigned idb$v_cram_alloc      : 1; 
      unsigned idb$v_vle             : 1;     
      unsigned idb$v_noresize        : 1; 
      unsigned idb$v_mcj             : 1;     
      unsigned idb$v_shared_int      : 1; 
      unsigned idb$v_distributed_int : 1; 
      unsigned idb$v_isr_callable    : 1; 
      unsigned idb$v_fill_0_         : 1;
    };
  };
  unsigned int idb$l_device_specific; 
  int idb$l_vector;                   
  void *idb$ps_auxstruc;              
  unsigned int idb$l_interrupt_cpu;   
  unsigned int idb$l_reserved;        
  struct _ucb *idb$l_ucblst [8];      
};
 
#endif 
 
