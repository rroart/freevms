#ifndef iosbdef_h
#define iosbdef_h

#define IOSB$K_LENGTH 8                 
#define IOSB$C_LENGTH 8                 
	
struct _iosb {
  union  {
    struct  {
      unsigned short int iosb$w_status; 
      union  {
	struct  {             
	  unsigned short int iosb$w_bcnt; 
	  union  {
	    unsigned int iosb$l_dev_depend; 
	    unsigned int iosb$l_pid; 
	  };
	};
	struct  {             
	  unsigned int iosb$l_bcnt; 
	  unsigned short int iosb$w_dev_depend_high; 
	};
      };
    };
    struct  {
      union  {
	unsigned int iosb$l_getxxi_status; 
	unsigned int iosb$l_reg_status; 
      };
      unsigned int iosb$l_reserved; 
    };
  };
};
 
typedef struct _iosb *  IOSB_PQ;	      
typedef struct _iosb ** IOSB_PPQ;	      

#endif 
 
