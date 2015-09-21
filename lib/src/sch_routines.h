#ifndef SCH_ROUTINES_H
#define SCH_ROUTINES_H

#include <acbdef.h>
#include <far_pointers.h>
#include <mtxdef.h>
#include <mutexdef.h>
#include <pcbdef.h>

int sch$add_cpu_cap(UINT32 cpu_id, UINT32 mask, UINT64_PQ prev_mask_p);
int sch$clear_affinity(UINT32 cpu_mask, struct _ktb *ktb, UINT32 flags,
                       UINT64_PQ prev_mask_p);
int sch$clear_capability(struct _ktb *ktb, UINT32 mask, UINT32 cpu_id,
                         UINT32 flags, UINT64_PQ prev_mask_p);
void sch$wait_kernel_mode(void);
void sch$wait_proc(struct _ktb *wait_queue, struct _ktb *ktb);
void sch$resource_wait(struct _ktb * const ktb, const UINT32 rsn);
void sch$resource_wait_ps(struct _ktb * const ktb, const UINT32 rsn,
                          const UINT32 psl);
int sch$resource_wait_setup(struct _ktb * const ktb, const UINT32 rsn);
int sch$remove_cpu_cap(UINT32 cpu_id, UINT32 mask, UINT64_PQ prev_mask_p);
int sch$set_affinity(UINT32 cpu_mask, struct _ktb *ktb, UINT32 flags,
                     UINT64_PQ prev_mask_p);
int sch$set_capability(struct _ktb *ktb, UINT32 mask, UINT32 cpu_id,
                       UINT32 flags, UINT64_PQ prev_mask_p);
int sch$acquire_implicit_affinity(struct _ktb *ktb, int obsolete, int cpu_id);
int sch$release_implicit_affinity(struct _ktb *ktb);

int sch_std$clrefr(int efn, struct _pcb *pcb);
struct _mutex *sch_std$iolockr(struct _pcb *pcb);
struct _mutex *sch_std$iolockw(struct _pcb *pcb);
void sch_std$iounlock(struct _pcb *pcb);
void sch_std$lockr(struct _mtx *mtx, struct _pcb *pcb);
void sch_std$lockr_quad(struct _mutex *mutex, struct _pcb *pcb);
INT32 sch_std$lockrexec(struct _mtx *mtx);
INT32 sch_std$lockrexec_quad(struct _mutex *mutex);
void sch_std$lockw(struct _mtx *mtx, struct _pcb *pcb);
INT32 sch_std$lockwexec(struct _mtx *mtx);
INT32 sch_std$lockwexec_quad(struct _mutex *mutex);
INT32 sch_std$lockwnowait(struct _mtx *mtx, struct _pcb *pcb);
INT32 sch_std$lockwnowait_quad(struct _mutex *mutex, struct _pcb *pcb);
int sch_std$postef(int ipid, int incpri, int efn, struct _pcb **pcb_p);
int sch_std$qast(int incpri, struct _acb *acb, struct _pcb **pcb_p);
void sch_std$ravail(int rsn);
void sch_std$unlock(struct _mtx *mtx, struct _pcb *pcb);
void sch_std$unlockexec(struct _mtx *mtx);
void sch_std$unlockexec_quad(struct _mutex *mutex);
int sch_std$wake(int ipid, struct _pcb **pcb_p);

#include <linux/spinlock.h>

void sch$rse(struct _pcb * p, unsigned char class, unsigned char event);
void sch$change_cur_priority(struct _pcb *p, unsigned char newpri);
int sch$wait(struct _pcb * p, struct _wqh * wq);
int sch$waitl(struct _pcb * p, struct _wqh * wq);
void sch$wake(unsigned long pid);
void sch$swpwake(void);
int sch$qast(unsigned long pid, int priclass, struct _acb * a);
void sch$one_sec(void);
int sch$postef(unsigned long ipid, unsigned long priclass, unsigned long efn);
void sch_std$ravail(int rsn);
void sch$newlvl(struct _pcb *p);
void sch$lockw(struct _mtx * m);
void sch$iolockw(void);
void sch$unlockw(struct _mtx * m);
void sch$unlock(struct _mtx * m);
void sch$iounlockw(void);
void sch$iounlock(void);
void sch$chsep(struct _pcb * p, unsigned char newpri);
void sch$chse(struct _pcb * p, unsigned char class);
void sch$chsep2(struct _pcb * p, unsigned char newpri);
int sch$calculate_affinity(int mask);
int sch$acquire_affinity(struct _pcb *pcb, int obsolete, int cpu_id);
int sch$release_affinity(struct _pcb *pcb);

#endif
