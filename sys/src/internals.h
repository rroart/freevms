void qhead_init(void * l);

static int inline vmslock(spinlock_t * lockname,signed int lockipl) {
  int savipl=getipl();
  spin_lock(lockname);
  if (lockipl>=0 && lockipl<=31) setipl(lockipl);
  return savipl;
}

static void inline vmsunlock(spinlock_t * lockname,signed int lockipl) {
  spin_unlock(lockname);
  if (lockipl>=0 && lockipl<=31) setipl(lockipl);
}

