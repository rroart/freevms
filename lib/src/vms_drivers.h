#ifndef vms_drivers_h
#define vms_drivers_h

//#include "../../../freevms/lib/src/builtins.h"

#include "../../../freevms/lib/src/fkbdef.h"
#include "../../../freevms/lib/src/irpdef.h"
#include "../../../freevms/starlet/src/pdscdef.h"
#include "../../../freevms/lib/src/splcoddef.h"
#include "../../../freevms/lib/src/spldef.h"
#include "../../../freevms/starlet/src/ssdef.h"
#include "../../../freevms/starlet/src/stsdef.h"
#include "../../../freevms/lib/src/ucbdef.h"
#include "../../../freevms/lib/src/vecdef.h"

#include "../../../freevms/lib/src/exe_routines.h"
#include "../../../freevms/lib/src/ioc_routines.h"
#include "../../../freevms/lib/src/smp_routines.h"

#define call_abortio(irp,pcb,ucb,final_status) (exe_std$abortio(irp,pcb,ucb,final_status))

#define call_finishio(irp,ucb,iost1,iost2)		\
    (							\
	irp->irp$l_iost1 = iost1,			\
	irp->irp$l_iost2 = iost2,                   	\
	exe_std$finishio(irp,ucb) 			\
    )

#define call_finishioc(irp,ucb,iost1)			\
    (							\
	irp->irp$l_iost1 = iost1,			\
	irp->irp$l_iost2 = 0,	                   	\
	exe_std$finishio(irp,ucb) 			\
    )

#define call_finishio_noiost(irp,ucb)			\
    (							\
	exe_std$finishio(irp,ucb) 			\
    )



#define call_qioacppkt(irp,pcb,ucb) 			\
    (							\
	exe_std$qioacppkt(irp,pcb,ucb) 			\
    )


#define call_qiodrvpkt(irp,ucb) 			\
    (							\
	irp->irp$ps_fdt_context = 0,			\
	exe_std$insioq(irp,ucb), 			\
	SS$_FDT_COMPL					\
    )

#define call_iorsnwait(irp,pcb,ucb,ccb,final_status,resource_num)	\
    (								  	\
     exe_std$iorsnwait(irp,pcb,ucb,ccb,final_status,resource_num) 	\
    )


#define NOSAVE_IPL	((int *) 0)	
#define NOLOWER_IPL	-1		
#define NORAISE_IPL	0		
#define RAISE_IPL	1		
#define SMP_RELEASE	0		
#define SMP_RESTORE	1		


#define device_lock(lockaddr, raise_ipl, savipl_p)	\
          {						\
            extern SMP smp$gl_flags;			\
							\
            if(savipl_p != NOSAVE_IPL)			\
              *savipl_p = __PAL_MFPR_IPL();		\
							\
            if(raise_ipl == NORAISE_IPL) {		\
              if(smp$gl_flags.smp$v_enabled)		\
                smp_std$acqnoipl( lockaddr );		\
              }						\
                else {					\
              if(smp$gl_flags.smp$v_enabled)		\
                smp_std$acquirel( lockaddr );		\
              else					\
                __PAL_MTPR_IPL( lockaddr->spl$l_ipl );	\
              }						\
          }

#define device_lock_shr(lockaddr, raise_ipl, savipl_p)	\
          {						\
            extern SMP smp$gl_flags;			\
							\
            if(savipl_p != NOSAVE_IPL)			\
              *savipl_p = __PAL_MFPR_IPL();		\
							\
            if(raise_ipl == NORAISE_IPL) {		\
              if(smp$gl_flags.smp$v_enabled)		\
                smp_std$acqnoipl_shr( lockaddr );	\
              }						\
                else {					\
              if(smp$gl_flags.smp$v_enabled)		\
                smp_std$acquirel_shr( lockaddr );	\
              else					\
                __PAL_MTPR_IPL( lockaddr->spl$l_ipl );	\
              }						\
          }

#define device_lock_nospin(lockaddr, raise_ipl, savipl_p, status)	\
          {						\
            extern SMP smp$gl_flags;			\
							\
            if(savipl_p != NOSAVE_IPL)			\
              *savipl_p = __PAL_MFPR_IPL();		\
							\
            if(raise_ipl == NORAISE_IPL) {		\
              if(smp$gl_flags.smp$v_enabled)		\
                *(int *)status = smp_std$acqnoipl_nospin( lockaddr );	\
              }						\
                else {					\
              if(smp$gl_flags.smp$v_enabled)		\
                *(int *)status = smp_std$acquirel_nospin( lockaddr );	\
              else					\
		{					\
                __PAL_MTPR_IPL( lockaddr->spl$l_ipl );	\
		*(int *)status = SS$_NORMAL;		\
		}					\
              }						\
          }

#define device_lock_shr_nospin(lockaddr, raise_ipl, savipl_p, status)	\
          {						\
            extern SMP smp$gl_flags;			\
							\
            if(savipl_p != NOSAVE_IPL)			\
              *savipl_p = __PAL_MFPR_IPL();		\
							\
            if(raise_ipl == NORAISE_IPL) {		\
              if(smp$gl_flags.smp$v_enabled)		\
                *(int *)status = smp_std$acqnoipl_shr_nospin( lockaddr );	\
              }						\
                else {					\
              if(smp$gl_flags.smp$v_enabled)		\
                *(int *)status = smp_std$acquirel_shr_nospin( lockaddr );	\
              else					\
		{					\
                __PAL_MTPR_IPL( lockaddr->spl$l_ipl );	\
		*(int *)status = SS$_NORMAL;		\
		}					\
              }						\
          }

#define device_lock_cvt_to_shr(lockaddr )		\
          {						\
            extern SMP smp$gl_flags;			\
							\
            if(smp$gl_flags.smp$v_enabled)		\
              smp_std$cvt_to_shared( lockaddr );	\
            else					\
              __PAL_MTPR_IPL( lockaddr->spl$l_ipl );	\
          }

#define device_lock_cvt_to_ex(lockaddr, status )	\
          {						\
            extern SMP smp$gl_flags;			\
							\
            if(smp$gl_flags.smp$v_enabled)		\
              *(int *)status = smp_std$cvt_to_ex( lockaddr );	\
            else					\
	      {						\
              __PAL_MTPR_IPL( lockaddr->spl$l_ipl );	\
	      *(int *)status = SS$_NORMAL;		\
	      }						\
          }


#define device_unlock(lockaddr, newipl, restore)	\
          {						\
            extern SMP smp$gl_flags;			\
							\
            if(smp$gl_flags.smp$v_enabled) {		\
              if(restore == SMP_RELEASE)		\
                smp_std$releasel( lockaddr );		\
              else					\
                smp_std$restorel( lockaddr );		\
            }						\
            if(newipl >= 0)				\
              __PAL_MTPR_IPL( newipl );			\
          }

#define device_unlock_shr(lockaddr, newipl, restore)	\
          {						\
            extern SMP smp$gl_flags;			\
							\
            if(smp$gl_flags.smp$v_enabled) {		\
              if(restore == SMP_RELEASE)		\
                smp_std$releasel_shr( lockaddr );	\
              else					\
                smp_std$restorel_shr( lockaddr );	\
            }						\
            if(newipl >= 0)				\
              __PAL_MTPR_IPL( newipl );			\
          }

#define dpt_store_isr_vec( crb, vecno, isr ) {				\
    struct __pd { int :32; int :32; void *entry; }			\
        *_pdp = ( struct __pd *)(isr);					\
    ((VEC *)&((crb)->crb$l_intd) + (vecno))->vec$ps_isr_pd = (isr);	\
    ((VEC *)&((crb)->crb$l_intd) + (vecno))->vec$ps_isr_code = _pdp->entry; }
#define dpt_store_isr( crb, isr ) dpt_store_isr_vec( crb, 0, isr )


#define fork(fork_routine, fr3, fr4, fkb)		\
          {						\
            ((struct _fkb *) fkb)->fkb$l_fpc = fork_routine;	\
            ((struct _fkb *) fkb)->fkb$q_fr3 = (long long) fr3;	\
            ((struct _fkb *) fkb)->fkb$q_fr4 = (long long) fr4;	\
            exe_std$queue_fork( (struct _fkb *) fkb );		\
          }


#define fork_lock(lockidx, savipl_p)			\
          {						\
            extern SMP smp$gl_flags;			\
            extern int smp$al_iplvec[];			\
							\
            if(savipl_p != NOSAVE_IPL)			\
              *savipl_p = __PAL_MFPR_IPL();		\
							\
            if(smp$gl_flags.smp$v_enabled)		\
              smp_std$acquire( lockidx );		\
            else					\
              __PAL_MTPR_IPL( smp$al_iplvec[lockidx] );	\
          }


#define fork_unlock(lockidx, newipl, restore)		\
          {						\
            extern SMP smp$gl_flags;			\
							\
            if(smp$gl_flags.smp$v_enabled) {		\
              if(restore == SMP_RELEASE)		\
                smp_std$release( lockidx );		\
              else					\
                smp_std$restore( lockidx );		\
            }						\
            if(newipl >= 0)				\
              __PAL_MTPR_IPL( newipl );			\
          }


#define fork_wait(fork_routine, fr3, fr4, fkb)				\
          {								\
            ((struct _fkb *) fkb)->fkb$l_fpc = fork_routine;			\
            exe_std$primitive_fork_wait( (long long) fr3, (long long) fr4,	\
                                         (struct _fkb *) fkb );			\
          }


#define iofork(fork_routine, fr3, fr4, ucb)		\
          {						\
            ((struct _ucb *) ucb)->ucb$v_tim = 0;		\
            fork( fork_routine, fr3, fr4, ucb);		\
          }


#define rfi(irp, fr4, ucb)				\
          ( *((struct _ucb *) ucb)->ucb$l_fpc ) (irp, fr4, ucb)


#define wfikpch(resume_rout, tout_rout, irp, fr4, ucb, tmo, restore_ipl)  \
          {								\
            ((struct _ucb *) ucb)->ucb$l_fpc = resume_rout;			\
            ((struct _ucb *) ucb)->ucb$ps_toutrout = tout_rout;			\
            ioc_std$primitive_wfikpch (irp, (long long) fr4, (struct _ucb *) ucb,	\
                                       tmo, restore_ipl );		\
          }


#define wfirlch(resume_rout, tout_rout, irp, fr4, ucb, tmo, restore_ipl)  \
          {								\
            ((struct _ucb *) ucb)->ucb$l_fpc = resume_rout;			\
            ((struct _ucb *) ucb)->ucb$ps_toutrout = tout_rout;			\
            ioc_std$primitive_wfirlch (irp, (long long) fr4, (struct _ucb *) ucb,	\
                                       tmo, restore_ipl );		\
          }
	
int       driver$ini_ddt_altstart( struct _ddt *ddt, void (*func)() );
int       driver$ini_ddt_aux_routine( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_aux_storage( struct _ddt *ddt, void *addr );
int       driver$ini_ddt_cancel( struct _ddt *ddt, void	( *func )() );
int       driver$ini_ddt_cancel_selective( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_change_preferred( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_channel_assign( struct _ddt *ddt, void ( *func )() );
int       driver$ini_ddt_cloneducb( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_configure( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_ctrlinit( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_csr_mapping( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_deconfigure( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_diagbf( struct _ddt *ddt, unsigned short value );
int       driver$ini_ddt_erlgbf( struct _ddt *ddt, unsigned short value );
int       driver$ini_ddt_fast_fdt( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_functab( struct _ddt *ddt, struct _fdt *fdt );
int       driver$ini_ddt_kp_reg_mask( struct _ddt *ddt, unsigned long value );
int       driver$ini_ddt_kp_stack_size( struct _ddt *ddt, unsigned long	value );
int       driver$ini_ddt_kp_startio( struct _ddt *ddt, void ( *func )() );
int       driver$ini_ddt_make_devpath( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_mntv_for( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_mntver( struct _ddt *ddt, void	( *func )() );
int       driver$ini_ddt_pending_io( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_qsrv_helper( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_qsrv_evnt_notify( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_mgt_register( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_mgt_deregister( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_regdmp( struct _ddt *ddt, void	( *func )() );
int       driver$ini_ddt_setprfpath( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_start( struct _ddt *ddt, void	( *func )() );
int       driver$ini_ddt_unitinit( struct _ddt *ddt, int ( *func )() );
int       driver$ini_ddt_end( struct _ddt *ddt );
int       driver$ini_dpt_adapt( struct _dpt *dpt, unsigned long	value );
int       driver$ini_dpt_bt_order( struct _dpt *dpt, long value );
int       driver$ini_dpt_decode( struct _dpt *dpt, long	value );
int       driver$ini_dpt_defunits( struct _dpt *dpt, unsigned short value );
int       driver$ini_dpt_deliver( struct _dpt *dpt, int ( *func )() );
int       driver$ini_dpt_devpath_size( struct _dpt *dpt, unsigned long value );
int       driver$ini_dpt_devpath_ucb_ofs( struct _dpt *dpt, unsigned long value );
int       driver$ini_dpt_dsplypath_size( struct _dpt *dpt, unsigned long value );
int       driver$ini_dpt_dsplypath_ucb_of( struct _dpt *dpt, unsigned long value );
int       driver$ini_dpt_flags( struct _dpt *dpt, unsigned long	value );
int       driver$ini_dpt_idb_crams( struct _dpt *dpt, unsigned short value );
int       driver$ini_dpt_iohandles( struct _dpt *dpt, unsigned short value );
int       driver$ini_dpt_maxunits( struct _dpt *dpt, unsigned short value );
int       driver$ini_dpt_name( struct _dpt *dpt, char *string_ptr );
int       driver$ini_dpt_struc_init( struct _dpt *dpt, void ( *func )() );
int       driver$ini_dpt_struc_reinit( struct _dpt *dpt, void ( *func )() );
int       driver$ini_dpt_ucb_crams( struct _dpt *dpt, unsigned short value );
int       driver$ini_dpt_ucbsize( struct _dpt *dpt, unsigned short value );
int       driver$ini_dpt_unload( struct _dpt *dpt, int ( *func )() );
int       driver$ini_dpt_vector( struct _dpt *dpt, void( **func )() );
int       driver$ini_dpt_end( struct _dpt *dpt );
int       driver$ini_fdt_act( struct _fdt *fdt,  unsigned long iofunc, int ( *action )(), unsigned long buf_64_flags );
int       driver$ini_fdt_qsrv( struct _fdt *fdt,  unsigned long iofunc, int qsrv_mask );
int       driver$ini_fdt_end( struct _fdt *fdt );

#define FDT_BUFFERED 1
#define FDT_NOT_BUFFERED 0
#define FDT_DIRECT 0
#define __FDT_64 2
#define FDT_BUFFERED_64     (FDT_BUFFERED     | __FDT_64)
#define FDT_NOT_BUFFERED_64 (FDT_NOT_BUFFERED | __FDT_64)
#define FDT_DIRECT_64       (FDT_DIRECT       | __FDT_64)


/* Device driver table initialization
 *  Define macros corresponding to each of the driver table
 *  initialization functions that invoke the function and continue
 *  if it returns success status; else, return the error status.
 */
#define ini_ddt_altstart( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_altstart( ddt, func ))) return _status; }

#define ini_ddt_aux_routine( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_aux_routine( ddt, func ))) return _status; }

#define ini_ddt_aux_storage( ddt, addr ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_aux_storage( ddt, addr ))) return _status; }

#define ini_ddt_cancel( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_cancel( ddt, func ))) return _status; }

#define ini_ddt_cancel_selective( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_cancel_selective( ddt, func ))) return _status; }

#define ini_ddt_change_preferred( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_change_preferred( ddt, func ))) return _status; }

#define ini_ddt_channel_assign( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_channel_assign( ddt, func ))) return _status; }

#define ini_ddt_cloneducb( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_cloneducb( ddt, func ))) return _status; }

#define ini_ddt_configure( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_configure( ddt, func ))) return _status; }

#define ini_ddt_ctrlinit( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_ctrlinit( ddt, func ))) return _status; }

#define ini_ddt_csr_mapping( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_csr_mapping( ddt, func ))) return _status; }

#define ini_ddt_deconfigure( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_deconfigure( ddt, func ))) return _status; }

#define ini_ddt_diagbf( ddt,  value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_diagbf( ddt,  value ))) return _status; }

#define ini_ddt_erlgbf( ddt,  value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_erlgbf( ddt,  value ))) return _status; }

#define ini_ddt_fast_fdt( ddt, func ) {	\
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_fast_fdt( ddt, func ))) return _status; }

#define ini_ddt_functab( ddt, fdt ) {	\
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_functab( ddt, fdt ))) return _status; }

#define ini_ddt_kp_reg_mask( ddt, value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_kp_reg_mask( ddt, value ))) return _status; }

#define ini_ddt_kp_stack_size( ddt, value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_kp_stack_size( ddt, value ))) return _status; }

#define ini_ddt_kp_startio( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_kp_startio( ddt, func ))) return _status; }

#define ini_ddt_make_devpath( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_make_devpath( ddt, func ))) return _status; }

#define ini_ddt_mntv_for( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_mntv_for( ddt, func ))) return _status; }

#define ini_ddt_mntver( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_mntver( ddt, func ))) return _status; }

#define ini_ddt_pending_io( ddt, func ) {	\
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_pending_io( ddt, func ))) return _status; }

#define ini_ddt_qsrv_helper( ddt, func ) {	\
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_qsrv_helper( ddt, func ))) return _status; }

#define ini_ddt_qsrv_evnt_notify( ddt, func ) {	\
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_qsrv_evnt_notify( ddt, func ))) return _status; }

#define ini_ddt_mgt_register( ddt, func ) {	\
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_mgt_register( ddt, func ))) return _status; }

#define ini_ddt_mgt_deregister( ddt, func ) {	\
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_mgt_deregister( ddt, func ))) return _status; }

#define ini_ddt_regdmp( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_regdmp( ddt, func ))) return _status; }

#define ini_ddt_setprfpath( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_setprfpath( ddt, func ))) return _status; }

#define ini_ddt_start( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_start( ddt, func ))) return _status; }

#define ini_ddt_unitinit( ddt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_unitinit( ddt, func ))) return _status; }

#define ini_ddt_end( ddt ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_ddt_end( ddt ))) return _status; }

#define ini_dpt_adapt( dpt, value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_adapt( dpt, value ))) return _status; }

#define ini_dpt_bt_order( dpt, value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_bt_order( dpt, value ))) return _status; }

#define ini_dpt_decode( dpt, value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_decode( dpt, value ))) return _status; }

#define ini_dpt_defunits( dpt, value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_defunits( dpt, value ))) return _status; }

#define ini_dpt_deliver( dpt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_deliver( dpt, func ))) return _status; }

#define ini_dpt_devpath_size( dpt, value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_devpath_size( dpt, value ))) return _status; }

#define ini_dpt_devpath_ucb_ofs( dpt, value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_devpath_ucb_ofs( dpt, value ))) return _status; }

#define ini_dpt_flags( dpt, value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_flags( dpt, value ))) return _status; }

#define ini_dpt_idb_crams( dpt, value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_idb_crams( dpt, value ))) return _status; }

#define ini_dpt_iohandles( dpt, value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_iohandles( dpt, value ))) return _status; }

#define ini_dpt_maxunits( dpt,  value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_maxunits( dpt,  value ))) return _status; }

#define ini_dpt_name( dpt, string_ptr ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_name( dpt, string_ptr ))) return _status; }

#define ini_dpt_struc_init( dpt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_struc_init( dpt, func ))) return _status; }

#define ini_dpt_struc_reinit( dpt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_struc_reinit( dpt, func ))) return _status; }

#define ini_dpt_ucb_crams( dpt,  value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_ucb_crams( dpt,  value ))) return _status; }

#define ini_dpt_ucbsize( dpt,  value ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_ucbsize( dpt,  value ))) return _status; }

#define ini_dpt_unload( dpt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_unload( dpt, func ))) return _status; }

#define ini_dpt_vector( dpt, func ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_vector( dpt, func ))) return _status; }

#define ini_dpt_end( dpt ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_dpt_end( dpt ))) return _status; }

#define ini_fdt_act( fdt, func, action, buf_64_flags ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_fdt_act( fdt, func, action, FDT_##buf_64_flags ))) return _status; }

#define ini_fdt_qsrv( fdt, func, qsrv_mask ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_fdt_qsrv( fdt, func, qsrv_mask ))) return _status; }

#define ini_fdt_end( fdt ) { \
    int	  _status; \
    if( !$VMS_STATUS_SUCCESS( _status =  driver$ini_fdt_end( fdt ))) return _status; }

#endif
