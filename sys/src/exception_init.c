#include "../../freevms/starlet/src/ssdef.h"
#include "../../freevms/starlet/src/misc.h"

int exe$synch  (unsigned int efn, struct _iosb *iosb) {
  if (!iosb) {
    exe$waitfr(efn);
    return SS$_NORMAL;
  }
  if (iosb->iostat) {
    return SS$_NORMAL;
  }
 again:
  exe$waitfr(efn);
  if (iosb->iostat & 0xff)
    return iosb->iostat;
  goto again;
}
