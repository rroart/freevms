#include<libdef.h>
#include<ssdef.h>

#include<linux/bitops.h>

int lib$ffs(int * pos, int * size, int * base,int * ret) {
  int retval;
  if ((*size)<=32) {
    retval=ffs(*pos);
    if (retval==0)
      return LIB$_NOTFOU;
    retval--;
  } else {
    printf("ffs retval for > 32 bit size not yet implemented");
    return LIB$_NOTFOU;
  }
  if (retval>=(*size))
    return LIB$_NOTFOU;
  *ret=retval;
  return SS$_NORMAL;
}
