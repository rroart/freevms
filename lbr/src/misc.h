#ifndef misc_h
#define misc_h

struct iosb { unsigned short iostat; unsigned short iolen; unsigned int device_info; 
};

struct item_list_2 { short complen, item_code;
         void *compaddr;
};

struct item_list_3 { short buflen, item_code;
         void *bufaddr;
         void *retlenaddr;
};

#endif
