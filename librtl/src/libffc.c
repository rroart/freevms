#include<libdef.h>
#include<ssdef.h>

#include<linux/bitops.h>

int lib$ffc(int * pos, char * size, int * base,int * ret) {
  int retval;
  if ((*size)<=32) {
    if ((*base)==0xffffffff) {
      *ret=*size;
      return LIB$_NOTFOU;
    }
#if 0
    retval=ffz(*base);
#endif
    retval = find_next_zero_bit (base, *size, *pos);

  } else {
    printf("ffc retval for > 32 bit size not yet implemented");
    return LIB$_NOTFOU;
    retval = find_next_zero_bit (base, *size, *pos);
  }
  if (retval>=(*size))
    return LIB$_NOTFOU;
  *ret=retval;
  return SS$_NORMAL;
}
