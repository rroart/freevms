#ifndef lksbdef_h
#define lksbdef_h

struct _lksb {
  unsigned short int lksb$w_status;
  unsigned short int lksb$w_reserved;
  unsigned int lksb$l_lkid;
  unsigned char lksb$b_valblk [16];
};

#endif
