#include<libdef.h>
#include<ssdef.h>

#include<linux/bitops.h>

unsigned long long not_my_event_flags = 0xffffffff;
// 1 taken, 0 free

unsigned long my_event_flags = 0xffffffff; // because of ffs? 32-63
// 0 taken, 1 free. due to ffs.

// all 0-31 are system reserved or pre-reserved
// 0 is special. 24-31 are officially reserved, rest are pre-.

int lib$get_ef(long * l) {
  int flag;
  flag=ffs(my_event_flags);
  if (flag==0)
    return LIB$_INSEF;
  *l=flag-1+32;
  my_event_flags&=~(1<<(*l));
  // no error condition returned yet; 
  return SS$_NORMAL;
}

int lib$free_ef(int * event) {
  int flag;
  int ev=*event;
  if (ev<32)
    return LIB$_EF_RESSYS;
  ev-=32;
  flag=test_and_clear_bit(ev,&my_event_flags);
  if (flag==0)
    return LIB$_EF_ALRFRE;
  return SS$_NORMAL;
}
