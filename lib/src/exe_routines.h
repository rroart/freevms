#ifndef exe_routines_h
#define exe_routines_h

//#include <ints.h>
#include <adpdef.h>
#include <arbdef.h>
#include <cbbdef.h>
#include <ccbdef.h>
#include <cebdef.h>
#include <chpctldef.h>
#include <chpretdef.h>
#include <cpudef.h>
#include <far_pointers.h>
#include <fkbdef.h>
#include <irpdef.h>
#include <jibdef.h>
#include <kpbdef.h>
#include <mmgdef.h>
#include <orbdef.h>
#include <pcbdef.h>
#include <tqedef.h>
#include <ucbdef.h>

int   exe_std$abortio (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, int qio_sts);
int   exe_std$alloc_bufio_32 (struct _irp *irp, struct _pcb *pcb, void *uva32, int pktdatsiz);
int   exe_std$alloc_bufio_64 (struct _irp *irp, struct _pcb *pcb, VOID_PQ uva64, int pktdatsiz);
int   exe_std$alloc_diagbuf (struct _irp *irp, VOID_PQ uva64, int pktdatsiz);
int   exe_std$allocbuf (int reqsize, int32 *alosize_p, void **bufptr_p);
int   exe_std$allocceb (int32 *alosize_p, struct _ceb **ceb_p);
int   exe_std$allocirp (struct _irp **irp_p);
int   exe_std$allocjib (int32 *alosize_p, struct _jib **jib_p);
int   exe_std$allocpcb (int32 *alosize_p, struct _pcb **pcb_p);
int   exe_std$alloctqe (int32 *alosize_p, struct _tqe **tqe_p);
int   exe_std$alononpaged (int reqsize, int32 *alosize_p, void **pool_p);
int   exe_std$alopaged (int reqsize, int32 *alosize_p, void **pool_p);
int   exe_std$alop1imag  (int reqsize, int32 *alosize_p, void **pool_p);
int   exe_std$alop1proc  (int reqsize, int32 *alosize_p, void **pool_p);
int   exe_std$alophycntg (int npages, void **sva_p);
int   exe_std$alophycntg_s2 (int npages, VOID_PPQ sva_p);
int   exe_std$alophycntg_color (int npages, int rad, int byte_align, void **sva_p);
int   exe_std$alophycntg_color_s2 (int npages, int rad, int byte_align, VOID_PPQ sva_p);
void  exe_std$altquepkt (struct _irp *irp, struct _ucb *ucb);
int	  exe$bugchk_cancel_remove_va (VOID_PQ start_va, unsigned long long size_in_bytes);
int	  exe$bugchk_remove_va (VOID_PQ start_va, unsigned long long size_in_bytes);
int       exe$bus_delay (struct _adp *adp);
int       exe$delay (long long *delay_nanos);
void  exe_std$carriage (struct _irp *irp);
int       exe$cbb_allocate (CBB_PPQ cbb, int unitsize, int bits, int ipl, unsigned long long flags, int timeout_value);
int       exe$cbb_clear_bit (int flags, int bitpos, CBB_PQ src, CBB_PQ dst, INT_PQ ccode);
int       exe$cbb_empty (CBB_PQ cbb, int flags, INT_PQ state);
int       exe$cbb_find_first_clear (CBB_PQ cbb, int start, int flags, INT_PQ bitpos, INT_PQ ccode);
int       exe$cbb_find_first_set (CBB_PQ cbb, int start, int flags, INT_PQ bitpos, INT_PQ ccode);
int	  exe$cbb_get_size (int unitsize, int bits, INT_PQ resultsize);
int       exe$cbb_initialize (CBB_PQ cbb, int blksiz, int unitsize, int bits, int ipl, unsigned long long flags, int timeout_value);
int       exe$cbb_lock (CBB_PQ cbb);
int       exe$cbb_rebuild (CBB_PQ cbb, int flags);
int       exe$cbb_set_bit (int flags, int bitpos, CBB_PQ src, CBB_PQ dst);
int       exe$cbb_test_bit (CBB_PQ cbb, int bitpos, int flags);
int       exe$cbb_unlock (CBB_PQ cbb);
int	  exe$cbb_copy( CBB_PQ src, CBB_PQ dst, int length, int flags);
int   exe$cbb_extract_bitmask( CBB_PQ src, int start, int length, int flags, VOID_PQ bitmask_addr);
int   exe$cbb_insert_bitmask( int start, int length, int flags, VOID_PQ bitmask_addr, CBB_PQ cbb);
int   exe$cbb_validate (CBB_PQ cbb, int unitsize, int bits, int ipl, unsigned long long flags, int timeout_value, UINT64_PQ ret_flags);
int   exe$cbb_zero (CBB_PQ cbb, int flags);
int   exe$cbb_boolean_oper (int flags, int function, CBB_PQ cbb1, CBB_PQ cbb2, CBB_PQ dst, INT_PQ ccode);
int   exe_std$check_device_access(int, int, struct _pcb *pcb, struct _ucb *ucb);
int   exe_std$chkflupages (void);
int   exe_std$chkpro_int (struct _arb *arb, struct _orb *orb, struct _chpctl *chpctl, struct _chpret *chpret);
int   exe_std$chkcreacces (struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb);
int   exe_std$chkdelacces (struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb);
int   exe_std$chkexeacces (struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb);
int   exe_std$chklogacces (struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb);
int   exe_std$chkphyacces (struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb);
int   exe_std$chkrdacces (struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb);
int   exe_std$chkwrtacces (struct _arb *arb, struct _orb *orb, struct _pcb *pcb, struct _ucb *ucb);
void  exe_std$credit_bytcnt (int credit, struct _pcb *pcb);
void  exe_std$credit_bytcnt_bytlm (int credit, struct _pcb *pcb);
int   exe_std$cvt_ipid_to_epid (int ipid);
struct _pcb  *exe_std$cvt_ipid_to_pcb (int ipid);
void  exe_std$deapaged (void *pool);
int   exe_std$deanonpaged (void *pool);
int   exe_std$deanonpgdsiz (void *pool, int size);
int   exe_std$deap1 (void *pool, int size);
int   exe_std$deap1block (void *pool);
int   exe_std$debit_bytcnt (int debit, struct _pcb *pcb);
int   exe_std$debit_bytcnt_alo (int debit, struct _pcb *pcb, int32 *alosize_p,
                                void **pool_p);
int   exe_std$debit_bytcnt_bytlm_alo (int debit, struct _pcb *pcb, int32 *alosize_p,
				      void **pool_p);
int   exe_std$debit_bytcnt_bytlm_nw (int debit, struct _pcb *pcb);
int   exe_std$finishio (struct _irp *irp, struct _ucb *ucb);
int       exe$illiofunc (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int   exe_std$insert_irp (struct _irp **irp_lh, struct _irp *irp);
void  exe_std$insioq (struct _irp *irp, struct _ucb *ucb);
void  exe_std$insioqc (struct _irp *irp, struct _ucb *ucb);
void  exe_std$insioq_simple (struct _irp *irp, struct _ucb *ucb);
void  exe_std$instimq (int duetim_lo, int duetim_hi, struct _tqe *tqe);
void  exe_std$iofork_cpu (struct _fkb *fkb, struct _cpu *destcpudb);
int   exe_std$iorsnwait (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, 
                         int qio_sts, int rsn);
int       exe$kp_allocate_kpb (struct _kpb **kpb_p, int stksiz, int flags, int paramsiz);
int       exe$kp_deallocate_kpb (struct _kpb *kpb);
int       exe$kp_end (struct _kpb *kpb);
struct _kpb      *exe$kp_find_kpb (void);
int       exe$kp_fork (struct _kpb *kpb, struct _fkb *fkb);
int       exe$kp_fork_wait (struct _kpb *kpb, struct _fkb *fkb);
int       exe$kp_restart (struct _kpb *kpb, int thread_sts);
int       exe$kp_stall_general (struct _kpb *kpb);
int       exe$kp_start (struct _kpb *kpb, void (*rout)(struct _kpb *kpb), long long regmask);
void  exe_std$kp_startio (struct _irp *irp, struct _ucb *ucb);
int       exe$kp_tqe_wait (struct _kpb *kpb, long long *ticks, int spnlidx);
int   exe_std$lcldskvalid (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
void  exe_std$lock_err_cleanup (struct _irp *irp);
int   exe_std$maxacmode (int acmode);
void      exe$lal_insert_first(void *packet,void *listhead);
void     *exe$lal_remove_first(void *listhead);

void  exe_std$mntversio (void (*rout)(), struct _irp *irp, struct _ucb *ucb);
int   exe_std$modify (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);

#ifdef  __INITIAL_POINTER_SIZE	
int   exe_std$modifylock (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, 
                          VOID_PQ buf, int bufsiz, 
                          void (*err_rout)(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, 
                                           struct _ccb *ccb, int errsts));
#else
int   exe_std$modifylock (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, 
                          void *buf, int bufsiz, 
                          void (*err_rout)(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, 
                                           struct _ccb *ccb, int errsts));
#endif 

int   exe_std$mount_ver (int iost1, int iost2, struct _irp *irp, struct _ucb *ucb);

#ifdef  __INITIAL_POINTER_SIZE	
int   exe_std$nam_to_pcb(INT_PQ pid_p,VOID_PQ prcnam_p,int nsa_id,struct _pcb *pcb,
			 int *rpid_p,struct _ktb **rktb_p,struct _pcb **rpcb_p);
#else
int   exe_std$nam_to_pcb(int *pid_p,void *prcnam_p,int nsa_id,struct _pcb *pcb,
			 int *rpid_p,struct _ktb **rktb_p,struct _pcb **rpcb_p);
#endif 

int   exe_std$oneparm (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
void  exe_std$outzstring (char *string);
void  exe_std$primitive_fork (long long fr3, long long fr4, struct _fkb *fkb);
void  exe_std$primitive_fork_wait (long long fr3, long long fr4, struct _fkb *fkb);
int   exe_std$prober (VOID_PQ buf, int bufsiz, int acmode);
int   exe_std$prober_dsc (void *dsc_p);
int   exe_std$probew (VOID_PQ buf, int bufsiz, int acmode);
int   exe_std$probew_dsc (void *dsc_p);

int   exe_std$prober_dsc64 (VOID_PQ dsc_p, UINT64_PQ ret_length, 
			    CHAR_PPQ ret_bufadr);
int   exe_std$probew_dsc64 (VOID_PQ dsc_p, UINT64_PQ ret_length,
			    CHAR_PPQ ret_bufadr);

int   exe_std$qioacppkt (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb);
int   exe_std$qiodrvpkt (struct _irp *irp, struct _ucb *ucb);
int   exe_std$qioserver_new_unit (struct _ucb *ucb);
void  exe_std$queue_fork (struct _fkb *fkb);
int 	  exe$psx_resume_and_wait (struct _pcb *child_pcb);
void	  exe$psx_set_fork_status (struct _pcb *child_pcb, int status);
int   exe_std$read (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);

#ifdef  __INITIAL_POINTER_SIZE	
int   exe_std$readchk (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, VOID_PQ buf, int bufsiz);
#else
int   exe_std$readchk (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, void *buf, int bufsiz);
#endif 

#ifdef  __INITIAL_POINTER_SIZE	
int   exe_std$readlock (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, 
			VOID_PQ buf, int bufsiz, 
			void (*err_rout)(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, 
					 struct _ccb *ccb, int errsts));
#else
int   exe_std$readlock (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, 
			void *buf, int bufsiz, 
			void (*err_rout)(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, 
					 struct _ccb *ccb, int errsts));
#endif 

int   exe_std$rmvtimq (int acmode, int reqid, int remval, int ipid);
int   exe_std$sensemode (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int   exe_std$setchar (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int   exe_std$setmode (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
int   exe_std$sndevmsg (struct _mb_ucb *mb_ucb, int msgtyp, struct _ucb *ucb);
int   exe_std$snglequota_long (int32 *quota_p, struct _pcb *pcb);
int   exe_std$synch_loop (int efn, VOID_PQ iosb);
int   exe$timedwait_complete (long long *end_value_p);
int   exe$timedwait_setup (long long *delay_nanos, long long *end_value_p);
int   exe$timedwait_setup_10us (long long *delay_10us, long long *end_value_p);
int   exe_std$write (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);

#ifdef  __INITIAL_POINTER_SIZE	
int   exe_std$writechk (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, VOID_PQ buf, int bufsiz);
#else
int   exe_std$writechk (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, void *buf, int bufsiz);
#endif 

#ifdef  __INITIAL_POINTER_SIZE	
int   exe_std$writelock (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, 
			 VOID_PQ buf, int bufsiz, 
			 void (*err_rout)(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, 
					  struct _ccb *ccb, int errsts));
#else
int   exe_std$writelock (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb, 
			 void *buf, int bufsiz, 
			 void (*err_rout)(struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, 
					  struct _ccb *ccb, int errsts));
#endif 

int   exe_std$wrtmailbox (struct _mb_ucb *mb_ucb, int msgsiz, void *msg,...);
int   exe_std$zeroparm (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
void  ini$brk (void);

#if !defined(MEMORYALC_POOL_SRC)
#ifdef  __INITIAL_POINTER_SIZE	
int exe$allocate_pool(int requestSize, MMG$POOL_TYPE poolType, int alignment,
		      UINT64_PQ allocatedSize, VOID_PPQ returnBlock,...);
void exe$deallocate_pool(VOID_PQ returnBlock, MMG$POOL_TYPE poolType, int size,...);
int exe$trim_pool_list(int reqSize, MMG$POOL_TYPE poolType, int percent,...);

int exe$register_pool_info(int (*need_memory_callback)(), MMG$POOL_TYPE poolType, unsigned long long userParam, int maxSize, int minSize,
			   unsigned long long extra_param_1,unsigned long long extra_param_2);

#endif
#endif 

int	exe$lock_pkta(void);
int	exe$unlock_pkta(void);

void exe$check_for_mem_error(struct _cpu *cpudb);
int exe$primitive_mcheck(int vec);
int exe$setup_memtest_env(void);
int exe$clear_memtest_env(void);



#define exe$event_notify EXE$EVENT_NOTIFY
void exe$event_notify (unsigned long long event_mask);

#ifdef __INITIAL_POINTER_SIZE			 
#pragma __required_pointer_size __restore		 
#endif

#endif 
