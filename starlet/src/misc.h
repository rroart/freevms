#ifndef misc_h
#define misc_h

#include"iosbdef.h"

struct iosb { unsigned short iostat; unsigned short iolen; unsigned int device_info; 
};

struct __iosb { unsigned short iostat; unsigned short iolen; unsigned int device_info; 
};

struct item_list_2 { short complen, item_code;
         void *compaddr;
};

struct item_list_3 { short buflen, item_code;
         void *bufaddr;
         void *retlenaddr;
};

struct return_values {
	void * val1;
	void * val2;
	void * val3;
	void * val4;
};

struct lock_blk { 
                   unsigned short  condition,reserved; 
                   unsigned int lock_id; 
};

#define vms_block_factor(x) (1<<(x-9))

#define vms_block_factor2(x) (x>>9)

// #define VMS_BLOCK_FACTOR 2 not! this is dangerous!

struct _exh {
  long exh$l_flink;
  long exh$l_handler;
  long exh$l_numargs;
  char exh$b_numargs;
  char exh$l_mbz[3];
  long exh$l_status_a;
  long exh$l_first_arg;
};

#endif
