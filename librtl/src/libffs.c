#include<libdef.h>
#include<ssdef.h>

#include<linux/bitops.h>

int lib$ffs(int * pos, char * size, int * base,int * ret) {
  int retval;
  int val=*base;
  if ((*pos)<=32) {
    if (*pos)
      val&=~(1<<((*pos)-1));
    retval=ffs(val);
    if (retval==0) {
      *ret=*size;
      return LIB$_NOTFOU;
    }
    retval--;
  } else {
    printf("ffs retval for > 32 bit size not yet implemented");
    return LIB$_NOTFOU;
  }
  if (retval>=(*size)) {
    *ret=*size;
    return LIB$_NOTFOU;
  }
  *ret=retval;
  return SS$_NORMAL;
}
