#include<linux/linkage.h>

asmlinkage int exe$exit(unsigned int code) {
  return sys_exit(code);
}
