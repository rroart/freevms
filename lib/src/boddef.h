#ifndef boddef_h
#define boddef_h

#define BOD$M_DELPEN    0x1
#define BOD$M_NOQUOTA   0x2
#define BOD$M_S2_WINDOW 0x4
#define BOD$M_NOSVA     0x8
#define BOD$M_SYSBUFOBJ 0x10
	
#define BOD$K_LENGTH 64                 
#define BOD$C_LENGTH 64                 
#define BOD$S_BODDEF 64
 
truct _bod {
  struct _bod *bod$l_flink;           
  struct _bod *bod$l_blink;           
  unsigned short int bod$w_size;      
  unsigned char bod$b_type;           
  unsigned char bod$b_align_1;
  unsigned int bod$l_acmode;          
  unsigned int bod$l_seqnum;          
  unsigned int bod$l_refcnt;          
  union  {
    unsigned int bod$l_flags;       
    struct  {
      unsigned bod$v_delpen    : 1;  
      unsigned bod$v_noquota   : 1; 
      unsigned bod$v_s2_window : 1; 
      unsigned bod$v_nosva     : 1;   
      unsigned bod$v_sysbufobj : 1; 
      unsigned bod$v_fill_0_   : 3;
    };
  };
  unsigned int bod$l_pid;             
  void *bod$pq_basepva;               
  union  {
    void *bod$pq_basesva;           
    void *bod$l_basesva;            
  } bod$r_basesva_overlay;
  union  {
    struct _pte *bod$pq_va_pte;     
    struct _pte *bod$ps_svapte;     
  } bod$r_svapte_overlay;
  unsigned int bod$l_pagcnt;          
  char bod$b_fill_1_ [4];
};
 
#endif 
 
