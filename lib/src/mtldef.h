#ifndef mtldef_h
#define mtldef_h

#define MTL$K_LENGTH 32                 
#define MTL$C_LENGTH 32                 
#define MTL$S_MTLDEF 32
	
struct _mtl {
  struct _mtl *mtl$l_mtlfl;           
  struct _mtl *mtl$l_mtlbl;           
  unsigned short int mtl$w_size;      
  unsigned char mtl$b_type;           
  union  {
    unsigned char mtl$b_status;     
    struct  {
      unsigned mtl$v_volset : 1;  
      unsigned mtl$v_rvtvcb : 1;  
      unsigned mtl$v_fill_0_ : 6;
    };
  };
  struct _ucb *mtl$l_ucb;             
  void *mtl$l_logname;                
  void *mtl$l_lognam2;                
  int mtldef$$_fill_1;                
  unsigned int mtl$l_uic;             
};
 
#endif 
 
