#include<ipl.h>

#undef VMSLOCK_DEBUG
#define VMSLOCK_DEBUG

#ifdef VMSLOCK_DEBUG
extern long locki;
extern long locks[1024];
#endif

void qhead_init(void * l);

static int inline vmslock(spinlock_t * lockname,signed int lockipl) {
  int savipl=getipl();
  spin_lock(lockname);
  if (lockipl>=0 && lockipl<=31) setipl(lockipl);
#ifdef VMSLOCK_DEBUG
  long  x;// = &lockname;
  long get_eip(long);
  locks[locki++]=get_eip(0);
  locks[locki++]=lockname;
  locks[locki++]=((long)&lockname)&0xfffff000;
  locks[locki++]=0;
  if (locki>1000)locki=0;
#endif
  return savipl;
}

static void inline vmsunlock(spinlock_t * lockname,signed int lockipl) {
  spin_unlock(lockname);
  if (lockipl>=0 && lockipl<=31) setipl(lockipl);
#ifdef VMSLOCK_DEBUG
  long  x;// = &lockname;
  long get_eip(long);
  locks[locki++]=get_eip(0);
  locks[locki++]=lockname;
  locks[locki++]=((long)&lockname)&0xfffff000;
  locks[locki++]=1;
  if (locki>1000)locki=0;
#endif
}

