// $Id$
// $Locker$

// Author. Roar Thronæs.

#include"../../freevms/sys/src/system_data_cells.h"
#include"../../freevms/sys/src/internals.h"

// definitely needs reimplementation

void sch$lockw(spinlock_t * s) {
  vmslock(s,-1);
}

void sch$iolockw(void) {
  // probably wrong?
  sch$lockw(&ioc$gq_mutex);
}

void sch$unlockw(spinlock_t * s) {
  vmsunlock(s,-1);
}

void sch$iounlockw(void) {
  // probably wrong?
  sch$unlockw(&ioc$gq_mutex);
}

void sch$iounlock(void) {
  // probably wrong?
  sch$unlockw(&ioc$gq_mutex);
}

