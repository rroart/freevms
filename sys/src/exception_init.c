// $Id$
// $Locker$

// Author. Roar Thronæs.

#include "../../freevms/starlet/src/ssdef.h"
#include "../../freevms/starlet/src/iosbdef.h"

int exe$synch(unsigned int efn, struct _iosb *iosb) {
  if (!iosb) {
    exe$waitfr(efn);
    return SS$_NORMAL;
  }
  if (iosb->iosb$w_status) {
    return SS$_NORMAL;
  }
 again:
  exe$waitfr(efn);
  if (iosb->iosb$w_status & 0xff)
    return iosb->iosb$w_status;
  exe$clref(efn);
  goto again;
}

int exe$clrast(void) {
  printk("this does not work yet (how to implement?), and is strong discouraged in real VMS too\n");
}
