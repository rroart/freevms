#ifndef sch_routines_h
#define sch_routines_h

#include <acbdef.h>
//#include <ints.h>
#include <far_pointers.h>
#include <mtxdef.h>
#include <mutexdef.h>
#include <pcbdef.h>

int    sch$add_cpu_cap(uint32 cpu_id, uint32 mask, UINT64_PQ prev_mask_p);
int    sch$clear_affinity(uint32 cpu_mask, struct _ktb *ktb, uint32 flags, UINT64_PQ prev_mask_p);
int    sch$clear_capability(struct _ktb *ktb, uint32 mask, uint32 cpu_id, uint32 flags, UINT64_PQ prev_mask_p);
void   sch$wait_kernel_mode (void);
void   sch$wait_proc (struct _ktb *wait_queue, struct _ktb *ktb);
void   sch$resource_wait (struct _ktb *const ktb, const uint32 rsn);
void   sch$resource_wait_ps (struct _ktb *const ktb,const uint32 rsn, const uint32 psl);
int    sch$resource_wait_setup (struct _ktb *const ktb, const uint32 rsn);
int    sch$remove_cpu_cap(uint32 cpu_id, uint32 mask, UINT64_PQ prev_mask_p);
int    sch$set_affinity(uint32 cpu_mask, struct _ktb *ktb, uint32 flags, UINT64_PQ prev_mask_p);
int    sch$set_capability(struct _ktb *ktb, uint32 mask, uint32 cpu_id, uint32 flags, UINT64_PQ prev_mask_p);
int    sch$acquire_implicit_affinity(struct _ktb *ktb, int obsolete, int cpu_id);
int    sch$release_implicit_affinity(struct _ktb *ktb);

int    sch_std$clrefr (int efn, struct _pcb *pcb);
struct _mutex *sch_std$iolockr (struct _pcb *pcb);
struct _mutex *sch_std$iolockw (struct _pcb *pcb);
void   sch_std$iounlock (struct _pcb *pcb);
void   sch_std$lockr (struct _mtx *mtx, struct _pcb *pcb);
void   sch_std$lockr_quad (struct _mutex *mutex, struct _pcb *pcb);
int    sch_std$lockrexec (struct _mtx *mtx);
int    sch_std$lockrexec_quad (struct _mutex *mutex);
void   sch_std$lockw (struct _mtx *mtx, struct _pcb *pcb);
int    sch_std$lockwexec (struct _mtx *mtx);
int    sch_std$lockwexec_quad (struct _mutex *mutex);
int    sch_std$lockwnowait (struct _mtx *mtx, struct _pcb *pcb);
int    sch_std$lockwnowait_quad (struct _mutex *mutex, struct _pcb *pcb);
int    sch_std$postef (int ipid, int incpri, int efn, struct _pcb **pcb_p);
int    sch_std$qast (int incpri, struct _acb *acb, struct _pcb **pcb_p);
void   sch_std$ravail (int rsn);
void   sch_std$unlock (struct _mtx *mtx, struct _pcb *pcb);
void   sch_std$unlockexec (struct _mtx *mtx);
void   sch_std$unlockexec_quad (struct _mutex *mutex);
int    sch_std$wake (int ipid, struct _pcb **pcb_p);

#endif
