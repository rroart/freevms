#ifndef lbrctltbl_h
#define lbrctltbl_h

#define LBR$C_CTLTBLID 203              
#define LBR$M_LOCATE 0x1
#define LBR$M_OPEN 0x2
#define LBR$K_LENGTH 30
#define LBR$C_LENGTH 30
	
struct _lbrctltbl {
  unsigned char lbr$b_id;             
  unsigned char lbr$b_tblsiz;         
  unsigned char lbr$b_type;           
  unsigned char lbr$b_func;           
  char lbrctltbl$$_fill_1 [2];        
  union  {
    unsigned int lbr$l_usrflg;      
    struct  {
      unsigned lbr$v_locate : 1;  
      unsigned lbr$v_open : 1;    
      unsigned lbr$v_fill_0_ : 6;
    };
  };
  unsigned int lbr$l_hdrptr;          
  unsigned int lbr$l_ctxptr;          
  unsigned int lbr$l_curidx;          
  unsigned int lbr$l_usrnam;          
  unsigned int lbr$l_oldhdrptr;       
};
 
#endif 
 
