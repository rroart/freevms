#ifndef dtndef_h
#define dtndef_h

#define DTN$K_BASE_LENGTH 24            
#define DTN$K_LENGTH 52                 
#define DTN$K_NAMELEN_MAX 27            
	
struct _dtn {
  struct _dtn *dtn$ps_flink;          
  struct _dtn *dtn$ps_blink;          
  unsigned short int dtn$w_size;      
  unsigned char dtn$b_type;           
  unsigned char dtn$b_subtype;        
  unsigned int dtn$l_flags;           
  unsigned char dtn$b_devtype;        
  unsigned char dtn$b_devclass;       
  short int dtn$w_spare1;             
  struct _ucb *dtn$ps_ucblist;        
  union  {                          
    char dtn$t_dtname [28];         
    struct  {
      unsigned char dtn$ib_dtname_len; 
      char dtn$t_dtname_str [27]; 
    };
  };
};
 
#endif 
 
