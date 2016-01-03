#ifndef EXE_ROUTINES_H
#define EXE_ROUTINES_H

#include <adpdef.h>
#include <arbdef.h>
#include <cbbdef.h>
#include <ccbdef.h>
#include <cebdef.h>
#include <chpctldef.h>
#include <chpretdef.h>
#include <cpudef.h>
#include <descrip.h>
#include <fabdef.h>
#include <far_pointers.h>
#include <fiddef.h>
#include <fkbdef.h>
#include <gen64def.h>
#include <iosbdef.h>
#include <jibdef.h>
#include <kpbdef.h>
#include <mmgdef.h>
#include <pcbdef.h>
#include <rabdef.h>
#include <seciddef.h>
#include <ucbdef.h>
#include <va_rangedef.h>
#include <wccdef.h>

int exe_std$abortio(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, int qio_sts);
int exe_std$alloc_bufio_32(struct _irp *irp, struct _pcb *pcb, void *uva32, int pktdatsiz);
int exe_std$alloc_bufio_64(struct _irp *irp, struct _pcb *pcb, VOID_PQ uva64, int pktdatsiz);
int exe_std$alloc_diagbuf(struct _irp *irp, VOID_PQ uva64, int pktdatsiz);
int exe_std$allocbuf(int reqsize, INT32 *alosize_p, void **bufptr_p);
int exe_std$allocceb(int *alosize_p, struct _ceb **ceb_p);
int exe_std$allocirp(struct _irp **irp_p);
int exe_std$allocjib(INT32 *alosize_p, struct _jib **jib_p);
int exe_std$allocpcb(INT32 *alosize_p, struct _pcb **pcb_p);
int exe_std$alloctqe(INT32 *alosize_p, struct _tqe **tqe_p);
int exe_std$alononpaged(int reqsize, INT32 *alosize_p, void **pool_p);
int exe_std$alopaged(int reqsize, INT32 *alosize_p, void **pool_p);
int exe_std$alop1imag(int reqsize, INT32 *alosize_p, void **pool_p);
int exe_std$alop1proc(int reqsize, INT32 *alosize_p, void **pool_p);
int exe_std$alophycntg(int npages, void **sva_p);
int exe_std$alophycntg_s2(int npages, VOID_PPQ sva_p);
int exe_std$alophycntg_color(int npages, int rad, int byte_align, void **sva_p);
int exe_std$alophycntg_color_s2(int npages, int rad, int byte_align, VOID_PPQ sva_p);
void exe_std$altquepkt(struct _irp *irp, struct _ucb *ucb);
int exe$bugchk_cancel_remove_va(VOID_PQ start_va, unsigned long long size_in_bytes);
int exe$bugchk_remove_va(VOID_PQ start_va, unsigned long long size_in_bytes);
int exe$bus_delay(struct _adp *adp);
int exe$delay(long long *delay_nanos);
void exe_std$carriage(struct _irp *irp);
int exe$cbb_allocate(CBB_PPQ cbb, int unitsize, int bits, int ipl, unsigned long long flags, int timeout_value);
int exe$cbb_clear_bit(int flags, int bitpos, CBB_PQ src, CBB_PQ dst, INT_PQ ccode);
int exe$cbb_empty(CBB_PQ cbb, int flags, INT_PQ state);
int exe$cbb_find_first_clear(CBB_PQ cbb, int start, int flags, INT_PQ bitpos, INT_PQ ccode);
int exe$cbb_find_first_set(CBB_PQ cbb, int start, int flags, INT_PQ bitpos, INT_PQ ccode);
int exe$cbb_get_size(int unitsize, int bits, INT_PQ resultsize);
int exe$cbb_initialize(CBB_PQ cbb, int blksiz, int unitsize, int bits, int ipl, unsigned long long flags, int timeout_value);
int exe$cbb_lock(CBB_PQ cbb);
int exe$cbb_rebuild(CBB_PQ cbb, int flags);
int exe$cbb_set_bit(int flags, int bitpos, CBB_PQ src, CBB_PQ dst);
int exe$cbb_test_bit(CBB_PQ cbb, int bitpos, int flags);
int exe$cbb_unlock(CBB_PQ cbb);
int exe$cbb_copy(CBB_PQ src, CBB_PQ dst, int length, int flags);
int exe$cbb_extract_bitmask(CBB_PQ src, int start, int length, int flags, VOID_PQ bitmask_addr);
int exe$cbb_insert_bitmask(int start, int length, int flags, VOID_PQ bitmask_addr, CBB_PQ cbb);
int exe$cbb_validate(CBB_PQ cbb, int unitsize, int bits, int ipl, unsigned long long flags, int timeout_value, UINT64_PQ ret_flags);
int exe$cbb_zero(CBB_PQ cbb, int flags);
int exe$cbb_boolean_oper(int flags, int function, CBB_PQ cbb1, CBB_PQ cbb2, CBB_PQ dst, INT_PQ ccode);
int exe_std$check_device_access(int, int, struct _pcb *pcb, struct _ucb *ucb);
int exe_std$chkflupages(void);
int exe_std$chkpro_int(struct _arb *arb, struct _orb *orb, struct _chpctl *chpctl, struct _chpret *chpret);
int exe_std$chkcreacces(struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb);
int exe_std$chkdelacces(struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb);
int exe_std$chkexeacces(struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb);
int exe_std$chklogacces(struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb);
int exe_std$chkphyacces(struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb);
int exe_std$chkrdacces(struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb);
int exe_std$chkwrtacces(struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb);
void exe_std$credit_bytcnt(int credit, struct _pcb *pcb);
void exe_std$credit_bytcnt_bytlm(int credit, struct _pcb *pcb);
int exe_std$cvt_ipid_to_epid(int ipid);
struct _pcb *exe_std$cvt_ipid_to_pcb(int ipid);
void exe_std$deapaged(void *pool);
int exe_std$deanonpaged(void *pool);
int exe_std$deanonpgdsiz(void *pool, int size);
int exe_std$deap1(void *pool, int size);
int exe_std$deap1block(void *pool);
int exe_std$debit_bytcnt(int debit, struct _pcb *pcb);
int exe_std$debit_bytcnt_alo(int debit, struct _pcb *pcb, INT32 *alosize_p, void **pool_p);
int exe_std$debit_bytcnt_bytlm_alo(int debit, struct _pcb *pcb, INT32 *alosize_p, void **pool_p);
int exe_std$debit_bytcnt_bytlm_nw(int debit, struct _pcb *pcb);
int exe_std$finishio(struct _irp *irp, struct _ucb *ucb);
int exe$illiofunc(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int exe_std$insert_irp(struct _irp **irp_lh, struct _irp *irp);
void exe_std$insioq(struct _irp *irp, struct _ucb *ucb);
void exe_std$insioqc(struct _irp *irp, struct _ucb *ucb);
void exe_std$insioq_simple(struct _irp *irp, struct _ucb *ucb);
void exe_std$instimq(int duetim_lo, int duetim_hi, struct _tqe *tqe);
void exe_std$iofork_cpu(struct _fkb *fkb, struct _cpu *destcpudb);
int exe_std$iorsnwait(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, int qio_sts, int rsn);
int exe$kp_allocate_kpb(struct _kpb **kpb_p, int stksiz, int flags, int paramsiz);
int exe$kp_deallocate_kpb(struct _kpb *kpb);
int exe$kp_end(struct _kpb *kpb);
struct _kpb *exe$kp_find_kpb(void);
int exe$kp_fork(struct _kpb *kpb, struct _fkb *fkb);
int exe$kp_fork_wait(struct _kpb *kpb, struct _fkb *fkb);
int exe$kp_restart(struct _kpb *kpb, int thread_sts);
int exe$kp_stall_general(struct _kpb *kpb);
int exe$kp_start(struct _kpb *kpb, void (*rout)(struct _kpb *kpb), long long regmask);
void exe_std$kp_startio(struct _irp *irp, struct _ucb *ucb);
int exe$kp_tqe_wait(struct _kpb *kpb, long long *ticks, int spnlidx);
int exe_std$lcldskvalid(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
void exe_std$lock_err_cleanup(struct _irp *irp);
int exe_std$maxacmode(int acmode);
void exe$lal_insert_first(void *packet, void *listhead);
void *exe$lal_remove_first(void *listhead);

void exe_std$mntversio(void (*rout)(void), struct _irp *irp, struct _ucb *ucb);
int exe_std$modify(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int exe_std$modifylock(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, void *buf, int bufsiz,
        void (*err_rout)(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, int errsts));
int exe_std$mount_ver(int iost1, int iost2, struct _irp *irp, struct _ucb *ucb);
int exe_std$nam_to_pcb(int *pid_p, void *prcnam_p, int nsa_id, struct _pcb *pcb, int *rpid_p, struct _ktb **rktb_p,
        struct _pcb **rpcb_p);
int exe_std$oneparm(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
void exe_std$outzstring(char *string);
void exe_std$primitive_fork(long long fr3, long long fr4, struct _fkb *fkb);
void exe_std$primitive_fork_wait(long long fr3, long long fr4, struct _fkb *fkb);
int exe_std$prober(VOID_PQ buf, int bufsiz, int acmode);
int exe_std$prober_dsc(void *dsc_p);
int exe_std$probew(VOID_PQ buf, int bufsiz, int acmode);
int exe_std$probew_dsc(void *dsc_p);
int exe_std$prober_dsc64(VOID_PQ dsc_p, UINT64_PQ ret_length, CHAR_PPQ ret_bufadr);
int exe_std$probew_dsc64(VOID_PQ dsc_p, UINT64_PQ ret_length, CHAR_PPQ ret_bufadr);
int exe_std$qioacppkt(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb);
int exe_std$qiodrvpkt(struct _irp *irp, struct _ucb *ucb);
int exe_std$qioserver_new_unit(struct _ucb *ucb);
void exe_std$queue_fork(struct _fkb *fkb);
int exe$psx_resume_and_wait(struct _pcb *child_pcb);
void exe$psx_set_fork_status(struct _pcb *child_pcb, int status);
int exe_std$read(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int exe_std$readchk(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, void *buf, int bufsiz);
int exe_std$readlock(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, void *buf, int bufsiz,
        void (*err_rout)(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, int errsts));
int exe_std$rmvtimq(int acmode, int reqid, int remval, int ipid);
int exe_std$sensemode(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int exe_std$setchar(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int exe_std$setmode(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int exe_std$sndevmsg(struct _mb_ucb *mb_ucb, int msgtyp, struct _ucb *ucb);
int exe_std$snglequota_long(INT32 *quota_p, struct _pcb *pcb);
int exe_std$synch_loop(int efn, VOID_PQ iosb);
int exe$timedwait_complete(long long *end_value_p);
int exe$timedwait_setup(long long *delay_nanos, long long *end_value_p);
int exe$timedwait_setup_10us(long long *delay_10us, long long *end_value_p);
int exe_std$write(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int exe_std$writechk(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, void *buf, int bufsiz);
int exe_std$writelock(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, void *buf, int bufsiz,
        void (*err_rout)(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, int errsts));
int exe_std$wrtmailbox(struct _mb_ucb *mb_ucb, int msgsiz, void *msg, ...);
int exe_std$zeroparm(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
void ini$brk(void);

int exe$lock_pkta(void);
int exe$unlock_pkta(void);

void exe$check_for_mem_error(struct _cpu *cpudb);
int exe$primitive_mcheck(int vec);
int exe$setup_memtest_env(void);
int exe$clear_memtest_env(void);

void exe$event_notify(unsigned long long event_mask);

struct _pcb * exe$ipid_to_pcb(unsigned long pid); // check int
struct _pcb * exe$epid_to_pcb(unsigned long pid);
int exe$epid_to_ipid(unsigned long pid);
int exe$ipid_to_epid(unsigned long pid);
int exe$a_pid_to_ipid(unsigned long pid);
void exe$iofork(struct _irp * i, struct _ucb * u);
int name_delim(char *str, int len, int size[5]);
int dircache(struct _vcb *vcb, char *dirnam, int dirlen, struct _fiddef *dirid);
int do_search(struct _fabdef *fab, struct _wccfile *wccfile);
int exe$search(struct _fabdef *fab);
int do_parse(struct _fabdef *fab, struct _wccfile **wccret);
int exe$parse(struct _fabdef *fab);
int exe$setddir(struct dsc$descriptor *newdir, UINT16 *oldlen, struct dsc$descriptor *olddir);
int exe$connect(struct _rabdef *rab);
int exe$disconnect(struct _rabdef *rab);
int exe$get(struct _rabdef *rab);
int exe$read(struct _rabdef *rab);
int exe$put(struct _rabdef *rab);
int exe$display(struct _fabdef *fab);
int exe$close(struct _fabdef *fab);
int exe$open(struct _fabdef *fab);
int exe$erase(struct _fabdef *fab);
int exe$create(struct _fabdef *fab);
int exe$extend(struct _fabdef *fab);
int exe$nampid(struct _pcb *p, unsigned long *pidadr, void *prcnam, struct _pcb ** retpcb, unsigned long * retipid,
        unsigned long *retepid);
void exe$instimq(struct _tqe * t);
int exe$synch(unsigned int efn, struct _iosb *iosb);
int exe$qioacppkt(struct _irp * i, struct _pcb * p, struct _ucb * u);
int exe$qiodrvpkt(struct _irp * i, struct _pcb * p, struct _ucb * u);
int exe$finishio(long status1, long status2, struct _irp * i, struct _pcb * p, struct _ucb * u);
int exe$finishioc(long status, struct _irp * i, struct _pcb * p, struct _ucb * u);
void com$post(struct _irp * i, struct _ucb * u);
int exe$asctim(UINT16 *timlen, struct dsc$descriptor *timbuf, struct _generic_64 *timadr, unsigned long cvtflg);
int exe$imgact_elf(void * name, void * hdrbuf);
int exe$pscan_next_id(struct _pcb ** p);
int exe$alophycntg(unsigned long * va, unsigned long num);
int exe$alononpaged(void);
int exe$deanonpaged(void);
int exe$allocate(int requestsize, void ** poolhead, int alignment, unsigned int * allocatedsize, void ** returnblock);
int exe$deallocate(void * returnblock, void ** poolhead, int size);
int exe_std$allocxyz(int *alosize_p, struct _tqe **tqe_p, int type, int size);
int exe_std$allocbuf(int reqsize, int *alosize_p, void **bufptr_p);
int exe_std$allocceb(int *alosize_p, struct _ceb **ceb_p);
int exe_std$allocirp(struct _irp **irp_p);
int exe_std$alloctqe(int *alosize_p, struct _tqe **tqe_p);
int exe$alononpagvar(int reqsize, int *alosize_p, void **pool_p);
int exe_std$alononpaged(int reqsize, int *alosize_p, void **pool_p);
int exe_std$deanonpgdsiz(void *pool, int size);
int exe_std$deanonpaged(void *pool);
int exe$flushlists(void * pool, int size);
int exe$allocate_pool(int requestsize, int pooltype, int alignment, unsigned int * allocatedsize, void ** returnblock);
int exe$extendpool(void * pool);
void exe$reclaim_pool_aggressive(void * pool);
void exe$reclaim_pool_gentle(void * pool);
void exe$insertirp(struct _ucb * u, struct _irp * i);
int exe$prober_simple(void * addr);

#include <linux/linkage.h>

asmlinkage int exe$assign(void *devnam, UINT16 *chan, unsigned int acmode, void *mbxnam, int flags);
asmlinkage int exe$exit(unsigned int code);
asmlinkage int exe$setpri(unsigned int *pidadr, void *prcnam, unsigned int pri, unsigned int *prvpri, unsigned int *pol,
        unsigned int *prvpol);
asmlinkage int exe$setimr(unsigned int efn, struct _generic_64 *daytim, void (*astadr)(long), unsigned long reqidt,
        unsigned int flags);
asmlinkage int exe$clref(unsigned int efn);
asmlinkage int exe$gettim(struct _generic_64 *timadr);
int exe$numtim(UINT16 timbuf[7], struct _generic_64 *timadr);
int exe$bintim(struct dsc$descriptor *timbuf, struct _generic_64 *timadr);
asmlinkage int exe$cancel(UINT16 chan);
asmlinkage int exe$crelnm(unsigned int *attr, void *tabnam, void *lognam, unsigned char *acmode, void *itmlst);
asmlinkage int exe$crelnt(unsigned int *attr, void *resnam, unsigned int *reslen, unsigned int *quota, UINT16 *promsk, void *tabnam,
        void *partab, unsigned char *acmode);
asmlinkage int exe$dellnm(void *tabnam, void *lognam, unsigned char *acmode);
asmlinkage int exe$trnlnm(unsigned int *attr, void *tabnam, void *lognam, unsigned char *acmode, void *itmlst);
asmlinkage int exe$create_region_32(unsigned long length, unsigned int region_prot, unsigned int flags,
        unsigned long long *return_region_id, void **return_va, unsigned long *return_length, unsigned long start_va);
asmlinkage int exe$deltva(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode);
asmlinkage int exe$imgsta(void * transfer, void * parseinfo, void * header, void * file, unsigned long linkstatus,
        unsigned long clistatus);
asmlinkage int exe$imgact(void * name, void * dflnam, void * hdrbuf, unsigned long imgctl, unsigned long long * inadr,
        unsigned long long * retadr, unsigned long long * ident, unsigned long acmode);
asmlinkage int exe$mgblsc(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode, unsigned int flags, void *gsdnam,
        struct _secid *ident, unsigned int relpag);
asmlinkage int exe$crmpsc(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode, unsigned int flags, void *gsdnam,
        unsigned long long * ident, unsigned int relpag, unsigned short int chan, unsigned int pagcnt, unsigned int vbn,
        unsigned int prot, unsigned int pfc);
asmlinkage int exe$schdwk(unsigned int *pidadr, void *prcnam, struct _generic_64 * daytim, signed long long * reptim);
asmlinkage int exe$cretva(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode);
asmlinkage int exe$dassgn(UINT16 chan);
int exe$insioq(struct _irp * i, struct _ucb * u);
asmlinkage int exe$forcex(unsigned int *pidadr, void *prcnam, unsigned int code);
asmlinkage int exe$mount(void *itmlst);
asmlinkage int exe$creprc(unsigned int *pidadr, void *image, void *input, void *output, void *error, struct _generic_64 *prvadr,
        unsigned int *quota, void*prcnam, unsigned int baspri, unsigned int uic, UINT16 mbxunt, unsigned int stsflg, ...);
asmlinkage int exe$crembx(char prmflg, UINT16 *chan, unsigned int maxmsg, unsigned int bufquo, unsigned int promsk,
        unsigned int acmode, void *lognam, long flags, ...);
asmlinkage int exe$setprn(struct dsc$descriptor *s);
asmlinkage int exe$setprv(char enbflg, struct _generic_64 *prvadr, char prmflg, struct _generic_64 *prvprv);

#endif
