// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/kernel.h>
#include <string.h>

#include <descrip.h>
#include <lckdef.h>
#include <lksbdef.h>
#include <ssdef.h>
#include <sys$routines.h>

#include "xqp.h"

static int continue_thread() {}

int volume_lock(char * prefix, char *volname) {
  int sts = 0;
  int acmode = 1;
  int parid = 0;
  long blkastadr = 0;
  long astadr = continue_thread;
  astadr = 0; // not yet
  int astprm = 0;
  int flags = 0;
  struct dsc$descriptor resnam;
  char name[256];

  sprintf(name, "%s%s", prefix, volname);
  resnam.dsc$a_pointer = name;
  resnam.dsc$w_length = strlen(name);

  struct _lksb lksb;

  while (sts == 0) {
    sts = sys$enq (0, LCK$K_NLMODE, &lksb, flags, &resnam, parid, astadr, astprm, blkastadr, acmode, 0);
  }
  sts = 0;
  flags |= LCK$M_CONVERT;
  while (sts == 0) {
    sts = sys$enq (0, LCK$K_PWMODE, &lksb, flags, &resnam, parid, astadr, astprm, blkastadr, acmode, 0);
  }

  int lkid = lksb.lksb$l_lkid;

  return lkid;
}

int serial_file(char * prefix, struct __xqp * xqp, int fid) {
  int sts = 0;
  int acmode = 1;
  int parid = 0;
  long blkastadr = 0;
  long astadr = continue_thread;
  astadr = 0; // not yet
  int astprm = 0;
  int flags = 0;
  struct dsc$descriptor resnam;
  char name[256];
  int lkid;

  if (xqp->lb_lockid_idx >= 5)
    return -1;

  sprintf(name, "%s%x", prefix, fid);
  resnam.dsc$a_pointer = name;
  resnam.dsc$w_length = strlen(name);

  struct _lksb lksb;

  while (sts == 0) {
    sts = sys$enq (0, LCK$K_NLMODE, &lksb, flags, &resnam, parid, astadr, astprm, blkastadr, acmode, 0);
  }

  sts = 0;
  flags |= LCK$M_CONVERT;
  while (sts == 0) {
    sts = sys$enq (0, LCK$K_PWMODE, &lksb, flags, &resnam, parid, astadr, astprm, blkastadr, acmode, 0);
  }

  lkid = lksb.lksb$l_lkid;
  xqp->lb_lockid[xqp->lb_lockid_idx++] = lkid;

  return lkid;
}

int unlock_xqp(char * prefix, struct __xqp * xqp) {
  int i = 0;
  int acmode = 1;
  int flags = 0;
  for (i = 0; i < xqp->lb_lockid_idx; i++) {
    char valblk[16];
    int lkid = xqp->lb_lockid[i];
    int sts = sys$deq (lkid, &valblk[0], acmode, flags);
  }
  xqp->lb_lockid_idx = 0;
  return SS$_NORMAL;
}
