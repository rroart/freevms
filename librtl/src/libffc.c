#include<libdef.h>
#include<ssdef.h>

#include<linux/bitops.h>

int lib$ffc(int * pos, int * size, int * base,int * ret) {
  int retval;
  if ((*size)<=32) {
    if ((*pos)==0xffffffff)
      return LIB$_NOTFOU;
    retval=ffz(*pos);
  } else {
    retval = find_next_zero_bit (base, *size, *pos);
  }
  if (retval>=(*size))
    return LIB$_NOTFOU;
  *ret=retval;
  return SS$_NORMAL;
}
