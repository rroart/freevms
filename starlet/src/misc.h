#ifndef misc_h
#define misc_h

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

#endif
