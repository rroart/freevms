// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/kernel.h>

#include <ssdef.h>

asmlinkage int exe$setrwm(int flags) {
  printk("setrwm not implemented\n");
  return SS$_NORMAL;
}

asmlinkage int exe$setswm(int flags) {
  printk("setswm not implemented\n");
}

