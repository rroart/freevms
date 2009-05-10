#include<ipl.h>
#include<spldef.h>

#undef VMSLOCK_DEBUG
#define VMSLOCK_DEBUG

#ifdef VMSLOCK_DEBUG
extern long locki;
extern long locks[1024];
#endif

void qhead_init(void * l);

inline int smp$acquire(struct _spl * spl);
inline int smp$restore(struct _spl * spl);

/**
   \brief acquire spinlock - see 5.2 8.3.7
   \param lockname
   \param lockipl -1 if no change
   \details not 100% according to spec
*/

static int inline vmslock(struct _spl * lockname,signed int lockipl) {
  int savipl=getipl();
  if (lockipl>=0 && lockipl<=31) setipl(lockipl);
  smp$acquire(lockname);
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

/**
   \brief release spinlock - see 5.2 8.3.7
   \param lockname
   \param lockipl -1 if no change
   \details not 100% according to spec
*/

static void inline vmsunlock(struct _spl * lockname,signed int lockipl) {
  smp$restore(lockname);
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

