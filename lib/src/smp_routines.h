#ifndef smp_routines_h
#define smp_routines_h

#include <ctddef.h>
#include <iosbdef.h>
#include <spldef.h>

void  smp_std$acqnoipl (struct _spl *spl);
void  smp_std$acquire (int spl_index);
void  smp_std$acquirel (struct _spl *spl);
int   smp_std$acqnoipl_nospin (struct _spl *spl);
int   smp_std$acquire_nospin (int spl_index);
int   smp_std$acquirel_nospin (struct _spl *spl);
void  smp_std$acqnoipl_shr (struct _spl *spl);
void  smp_std$acquire_shr (int spl_index);
void  smp_std$acquirel_shr (struct _spl *spl);
int   smp_std$acqnoipl_shr_nospin (struct _spl *spl);
int   smp_std$acquire_shr_nospin (int spl_index);
int   smp_std$acquirel_shr_nospin (struct _spl *spl);
int   smp_std$allocate_portlock (int spl_index, struct _spl **portlock_ptr);
void  smp_std$cvt_to_shared (struct _spl *spl);
int   smp_std$cvt_to_ex (struct _spl *spl);
void  smp_std$release (int spl_index);
void  smp_std$releasel (struct _spl *spl);
void  smp_std$restore (int spl_index);
void  smp_std$restorel (struct _spl *spl);
void  smp_std$release_shr (int spl_index);
void  smp_std$releasel_shr (struct _spl *spl);
void  smp_std$restore_shr (int spl_index);
void  smp_std$restorel_shr (struct _spl *spl);
int   smp_std$setup_cpu(int);

int	  smp$cpu_orphan_check(int,unsigned long long,unsigned long long);
int	  smp$request_shutdown_cpu(int, int);
int       smp$system_event_notify(struct _ctd_pq ctd, int event);
void      smp$timeout (void);
int	  smp$validate_hw_configuration(int, char **, char **);

void smp_send_work(int work, int cpu);
void smp_work();

#endif
