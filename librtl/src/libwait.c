// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <libdef.h>
#include <ssdef.h>
#include <starlet.h>

static int dummyast(int dummy) {}

// just ordinary float. float_type is ignored for now

// should use get_ef?

unsigned int lib$wait(float *seconds, int flags, int float_type) {
  // check pointer
  if (*seconds<0 || *seconds>100000)
    return LIB$_INVARG;
  float hundreds_f = *seconds * 100;
  long long hundreds = hundreds_f;
  long long vmstime = - hundreds * 100000;
  int sts;
  if (flags & 1) { // LIB$K_NOWAKE is 1
    sts = sys$setimr (0, &vmstime, dummyast, 0, 0);
    if ((sts & 1) == 0)
      return sts;
    sts = sys$synch (0, 0);
    if ((sts & 1) == 0)
      return sts;
  } else {
    sts = sys$schdwk(0, 0, &vmstime, 0);
    if ((sts & 1) == 0)
      return sts;
    return sys$hiber();
  }
  return SS$_NORMAL;
}
