#ifndef ioc_routines_h
#define ioc_routines_h

#include "../../../freevms/lib/src/ints.h"
#include "../../../freevms/lib/src/acbdef.h"
#include "../../../freevms/lib/src/adpdef.h"
#include "../../../freevms/lib/src/busarraydef.h"
#include "../../../freevms/lib/src/ccbdef.h"
#include "../../../freevms/lib/src/cdrpdef.h"
#include "../../../freevms/lib/src/config_table.h"
#include "../../../freevms/lib/src/crabdef.h"
#include "../../../freevms/lib/src/cramdef.h"
#include "../../../freevms/lib/src/crctxdef.h"
#include "../../../freevms/lib/src/crbdef.h"
#include "../../../freevms/lib/src/ddbdef.h"
#include "../../../freevms/lib/src/ddtdef.h"
#include "../../../freevms/lib/src/diobmdef.h"
#include "../../../freevms/lib/src/dptdef.h"
#include "../../../freevms/lib/src/dtndef.h"
#include "../../../freevms/lib/src/far_pointers.h"
#include "../../../freevms/lib/src/fdtdef.h"
#include "../../../freevms/lib/src/idbdef.h"
#include "../../../freevms/lib/src/irpdef.h"
#include "../../../freevms/lib/src/iosbdef.h"
#include "../../../freevms/lib/src/kpbdef.h"
#include "../../../freevms/lib/src/mtldef.h"
#include "../../../freevms/lib/src/pcbdef.h"
#include "../../../freevms/lib/src/ptedef.h"
#include "../../../freevms/lib/src/sbdef.h"
#include "../../../freevms/lib/src/scqdef.h"
#include "../../../freevms/lib/src/ucbdef.h"
#include "../../../freevms/lib/src/wcbdef.h"
#include "../../../freevms/lib/src/scsidef.h"
#include "../../../freevms/lib/src/wwiddef.h"

int       ioc$add_adp (struct _adp *parent_adp, struct _adp *new_adp, int ba_index);
int       ioc$add_device_type (const char *, const int32, struct _ucb *, DTN **);
int       ioc$allocate_ccb (int16 *chan_p, struct _ccb **ccb_p);
int       ioc$allocate_cram (struct _cram **cram_p, IDB *idb, struct _ucb *ucb, struct _adp *adp);
int       ioc$alloc_adp (long long *adp, int adptype, int hose, int bustype, int nodecount);
int       ioc$alloc_cnt_res (struct _crab *crab, struct _crctx *crctx, long long cntxt1, long long cntxt2, long long cntxt3);
int       ioc$alloc_crab (int itemcnt, int gran, struct _crab **crab_p);
int       ioc$alloc_crctx (struct _crab *crab, struct _crctx **crctx_p, int flck);
void  ioc_std$altreqcom (int iost1, int iost2, struct _cdrp *cdrp,
                         struct _irp **irp_p, struct _ucb **ucb_p);
int   ioc_std$ascwwid_to_binwwid(char *strptr, int len, char *result, int *reslen); 
int   ioc_std$binwwid_to_ascwwid(WWID *wwid_ptr, char *wwid_string, int inlen, int *retlen);
int   ioc_std$broadcast (int msglen, void *msg_p, struct _ucb *ucb);
void  ioc_std$bufpost (struct _pcb *pcb, struct _irp *irp);
void  ioc_std$cancelio (int chan, struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb);
int       ioc$cancel_cnt_res (struct _crab *crab, struct _crctx *crctx, int resume);
int       ioc$cancel_cnt_res_nosync (struct _crab *crab, struct _crctx *crctx, int resume);
int       ioc$change_device_type (const char *, const int32, struct _ucb *, DTN **);
int       ioc$chan_to_ccb (int16 chan, struct _ccb **ccb_p);
int   ioc_std$check_hwm (struct _irp *irp);
int   ioc_std$chkmbxquota (struct _pcb *pcb, struct _ucb *ucb, int quo);
int   ioc_std$chkucbquota (struct _pcb *pcb, struct _ucb *ucb);  
int   ioc_std$clone_ucb (struct _ucb *tmpl_ucb, struct _ucb **new_ucb);
void  ioc_std$closebufwind (struct _ucb *ucb);
int   ioc_std$conbrdcst (int msglen, void *msg_p);
int   ioc_std$copy_ucb (struct _ucb *src_ucb, struct _ucb **new_ucb);
int       ioc$cram_cmd (int cmdidx, int byteoffset, struct _adp *adp, struct _cram *cram,
                        unsigned long long *iohandle);
int       ioc$cram_io (struct _cram *cram);
int       ioc$cram_queue (struct _cram *cram);
int       ioc$cram_wait (struct _cram *cram);
int       ioc$create_ccb_table (int num_chan);
int   ioc_std$create_diobm (const struct _pte_PQ va_pte, const uint32 pte_count,
                            const uint32 flags,
                            struct _pte **svapte_p, DIOBM **diobm_p);
int   ioc_std$create_ucb (struct _pcb *pcb, struct _ucb *ucb, struct _ucb **new_ucb_p);
void  ioc_std$credit_ucb (struct _ucb *ucb);
int       ioc$ctrl_init (struct _crb *crb, struct _ddb *ddb);
void  ioc_std$cvtlogphy (int lbn, struct _irp *irp, struct _ucb *ucb);
int   ioc_std$cvt_devnam (int buflen, char *buf, int form, struct _ucb *ucb, 
                          int32 *outlen_p );
int   ioc_std$dalloc_dev (struct _pcb *pcb, struct _ucb *ucb);
int   ioc_std$dalloc_dmt (struct _pcb *pcb, struct _ucb *ucb);
void  ioc_std$ddt_cancel_intercept (int chan, struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, int reason);
int       ioc$deallocate_ccb (struct _ccb *ccb);
int       ioc$deallocate_cram (struct _cram *cram);
int       ioc$dealloc_cnt_res (struct _crab *crab, struct _crctx *crctx);
int       ioc$dealloc_cnt_res_nosync (struct _crab *crab, struct _crctx *crctx);
int       ioc$dealloc_crab (struct _crab *crab);
int       ioc$dealloc_crctx (struct _crctx *crctx);
void  ioc_std$debit_ucb (struct _ucb *ucb, struct _pcb *pcb);
void  ioc_std$delete_ucb (struct _ucb *ucb);
void  ioc_std$diagbufill (int driver_param, struct _ucb *ucb);
int   ioc_std$dismount (int flags, struct _pcb *pcb, MTL *mtl);
int   ioc_std$errcnt_value (struct _ucb *ucb);
int   ioc_std$fill_diobm (DIOBM *const diobm, const struct _pte_PQ va_pte, 
                          const uint32 pte_count, const uint32 flags,
                          struct _pte **svapte_p);
void *ioc_std$filspt (struct _ucb *ucb);
void  ioc_std$free_ucb (struct _ucb *ucb);
int   ioc_std$getbyte (void *sva, struct _ucb *ucb, void **sva_p);
int       ioc$get_device_type (struct _ucb *, DTN **);
int       ioc$get_page80_wwid (INQUIRY_DATA *inq_data, PAGE80 *pg80_data, 
			       struct _sn_wwid *wwid);
int       ioc$get_page83_wwid (PAGE83 *pg83_data, struct _wwid *wwid);
int       ioc$get_udid (DEVID *devid_data, int *udid_p);
void  ioc_std$hwm_end (struct _pcb *pcb, struct _irp *irp);
void *ioc_std$initbufwind (struct _ucb *ucb);
int       ioc$init_cram (struct _adp *adp, struct _cram *cram);
int       ioc$init_crctx (struct _crab *crab, struct _crctx *crctx, int flck);
void  ioc_std$initiate (struct _irp *irp, struct _ucb *ucb);
void  ioc_std$initiate_lck_rel (struct _irp *irp, struct _ucb *ucb);
void  ioc_std$initiate_new_io (struct _irp *irp, struct _ucb *ucb);
int       ioc$kp_reqchan (struct _kpb *kpb, int pri);
int       ioc$kp_wfikpch (struct _kpb *kpb, int tmo, int newipl);
int       ioc$kp_wfirlch (struct _kpb *kpb, int tmo, int newipl);
void  ioc_std$last_chan (int chan, struct _pcb *pcb, struct _ucb *ucb);
void  ioc_std$last_chan_ambx (struct _pcb *pcb, struct _ucb *ucb);
int   ioc_std$link_ucb (struct _ucb *ucb, ... ); /* [ struct _ucb **outucb_p ] */
int       ioc$load_map (struct _adp *adp, struct _crctx *crctx, struct _pte *svapte, int boff,
                        void **dma_addr_p);
int   ioc_std$lock_dev (int lockmode, void *lock_val_p, struct _pcb *pcb, struct _ucb *ucb,
                        int32 *lockid_p);
int       ioc$map_io (struct _adp *adp, int node, unsigned long long *phys_offset, int num_bytes,
		      int attr, unsigned long long *iohandle);
int   ioc_std$mapvblk (unsigned int vbn, unsigned int numbytes, 
		       struct _wcb *wcb, struct _irp *irp, struct _ucb *ucb, 
		       uint32 *lbn_p, uint32 *notmapped_p, struct _ucb **new_ucb_p);
void  ioc_std$mntver (struct _irp *irp, struct _ucb *ucb);
void *ioc_std$movfruser (void *sysbuf, int numbytes, struct _ucb *ucb, void **sysbuf_p);
void *ioc_std$movfruser1 (void *sysbuf, int numbytes, struct _ucb *ucb, void *sva, void **sysbuf_p);
void *ioc_std$movfruser2 (void *sysbuf, int numbytes, struct _ucb *ucb, void *sva, void **sysbuf_p);
void *ioc_std$movtouser (void *sysbuf, int numbytes, struct _ucb *ucb, void **sysbuf_p);
void *ioc_std$movtouser1 (void *sysbuf, int numbytes, struct _ucb *ucb, void *sva, void **sysbuf_p);
void *ioc_std$movtouser2 (void *sysbuf, int numbytes, struct _ucb *ucb, void *sva, void **sysbuf_p);
int       ioc$node_data (struct _crb *crb, int func, void *buf_p);
int       ioc$node_function (struct _crb *crb, int func, ...);
int   ioc_std$opcnt_value (struct _ucb *ucb);
int   ioc_std$parsdevnam (int devnamlen, char *devnam, int flags,
                          int32 *unit_p, int32 *scslen_p, int32 *devnamlen_p,
                          char **devnam_p, int32 *flags_p);
void  ioc_std$post_irp (struct _irp *irp);
int   ioc_std$pre_simreqcom (int pri, int efn, struct _acb *acb);
int   ioc_std$primitive_reqchanh (struct _irp *irp, struct _ucb *ucb, IDB **idb_p);
int   ioc_std$primitive_reqchanl (struct _irp *irp, struct _ucb *ucb, IDB **idb_p);
void  ioc_std$primitive_wfikpch (struct _irp *irp, long long fr4, struct _ucb *ucb, 
                                 int tmo, int restore_ipl); 
void  ioc_std$primitive_wfirlch (struct _irp *irp, long long fr4, struct _ucb *ucb, 
                                 int tmo, int restore_ipl); 

#ifdef  __INITIAL_POINTER_SIZE	/* Defined if support for 64-bit pointers */
int   ioc_std$ptetopfn (struct _pte_PQ pte);
#else
int   ioc_std$ptetopfn (struct _pte *pte);
#endif /* __INITIAL_POINTER_SIZE */


void *ioc_std$putbyte (void *sva, char data, struct _ucb *ucb);
int   ioc_std$q_internal_irp(struct _irp *irp, struct _ucb *ucb, IOSB_PQ iosb, VOID_PQ mbz);
void  ioc_std$qnxtseg (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ucb **ucb_p);
void  ioc_std$qnxtseg1 (int vbn, int bcnt, struct _wcb *wcb, 
                        struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ucb **ucb_p);
int       ioc$read_io (struct _adp *adp, unsigned long long *iohandle, int offset,
		       int length, void *data_p);
int       ioc$read_pci_config (struct _adp *adp, int pci_node, int offset,
			       int length, int *data_p);
int       ioc$reallocate_ccb (int16 chan, struct _ccb **ccb_p);
void  ioc_std$relchan (struct _ucb *ucb);
int   ioc_std$release_diobm (DIOBM *const diobm);
int       ioc$remove_device_type (struct _ucb *ucb);
void  ioc_std$reqcom (int iost1, int iost2, struct _ucb *ucb);
void  ioc_std$reqcom_local (struct _cdrp *cdrp);
int       ioc$reserve_scb (int count, int min_vec, int max_vec, int align,
                           int32 *resvec_p);
void      ioc$return (void); 
int       ioc$return_success (void);
int       ioc$return_unsupported (void);
int       ioc$scan_ccb (int32 **context_p, struct _ccb **ccb_p);
int   ioc_std$scan_iodb (struct _ucb *ucb, struct _ddb *ddb, struct _ucb **ucb_p, struct _ddb **ddb_p);
int   ioc_std$scan_iodb_2p (int path, struct _ucb *ucb, struct _ddb *ddb, 
                            int32 *path_p, struct _ucb **ucb_p, struct _ddb **ddb_p);
int   ioc_std$scan_iodb_usrctx (int ctx, int unit, struct _ucb *ucb, struct _ddb *ddb, 
                                int32 *ctx_p, int32 *unit_p, struct _ucb **ucb_p, struct _ddb **ddb_p);
int       ioc$scsipath_connect (SCQ *scq);
int   ioc_std$search (VOID_PQ descr_p, int flags, void *lock_val_p, 
                      struct _ucb **ucb_p, struct _ddb **ddb_p, struct _sb **sb_p);
int   ioc_std$searchall (VOID_PQ descr_p, struct _ucb **ucb_p, struct _ddb **ddb_p, struct _sb **sb_p);
int   ioc_std$searchcont (int unit, int scslen, struct _ucb *ucb, struct _ddb *ddb, struct _sb *sb,
                          int devnamlen, char *devnam, int flags, 
                          struct _ucb **ucb_p, struct _ddb **ddb_p, struct _sb **sb_p, void **lock_val_p);
int   ioc_std$searchdev (VOID_PQ descr_p, struct _ucb **ucb_p, struct _ddb **ddb_p, struct _sb **sb_p);
int   ioc_std$searchint (int unit, int scslen, int devnamlen, char *devnam,
                         int flags, struct _ucb **ucb_p, struct _ddb **ddb_p, struct _sb **sb_p,
                         void **lock_val_p);
int       ioc$search_device_type (const char *, const int32, DTN **);
int   ioc_std$sensedisk (struct _irp *irp, struct _pcb *pcb, struct _ucb *ucb, struct _ccb *ccb);
void      ioc$setup_ucb_dipl (struct _ucb *ucb);
void  ioc_std$sever_ucb (struct _ucb *ucb);
int   ioc_std$simreqcom (VOID_PQ iosb, int pri, int efn, int32 iost[2],
                         struct _acb *acb, int acmode);
int       ioc$sndopcfao (unsigned int opc_msg_code, unsigned int opc_class_mask, ... );
long long     ioc$sva_to_pa (void *svapte, long long *pa, int buflen, int32 *retlen_p);
int   ioc_std$svapte_in_buf (struct _irp *irp, VOID_PQ va, struct _pte **svapte_p);
long long     ioc$svapte_to_pa (void *svapte, int boff, long long *pa, int buflen,
			    int32 *retlen_p, void **nxtsvapte_p);
int   ioc_std$testunit (int unit, int devtype, struct _ucb *ucb, int flags, 
                        void *lock_val_p, struct _ucb **ucb_p, int32 *flags_p);
void  ioc_std$threadcrb (struct _crb *crb);
int   ioc_std$trandevnam (VOID_PQ descr_p, int flags, char *buf, 
                          int32 *outlen, void **out_p);
struct _adp      *ioc$tr_to_adp (int tr);
int       ioc$unit_init (struct _ucb *ucb);
int   ioc_std$unlock_dev (struct _ucb *ucb);
int       ioc$unmap_io (struct _adp *adp, unsigned long long *iohandle);
int       ioc$unreserve_scb (int count, int32 vector_list[]);
int   ioc_std$update_dev_wwid_list (WWID *wwid_ptr, char *devnam); 
VOID_PQ ioc_std$va_to_pa (VOID_PQ va, VOID_PPQ pa_p);
int       ioc$verify_chan (int16 chan, struct _ccb **ccb_p);
void  ioc_std$wakacp (struct _ucb *ucb, struct _irp *irp); 
int       ioc$write_io (struct _adp *adp, unsigned long long *iohandle, int offset,
			int length, void *data_p);
int       ioc$write_pci_config (struct _adp *adp, int pci_node, int offset,
				int length, int wdata);
int	  ioc_std$getsyi_cpu_specific (int getsyi_code, unsigned char *buffer,
                                       int *buffer_length);

#ifdef __INITIAL_POINTER_SIZE			 /* Defined whenever ptr size pragmas supported */
#pragma __required_pointer_size __restore		 /* Restore the previously-defined required ptr size */
#endif

#endif

