#ifndef scs_routines_h
#define scs_routines_h

#include <ints.h>
#include <bddef.h>
#include <bdlptrdef.h>
#include <bnamdef.h>
#include <cdrpdef.h>
#include <cdtdef.h>
#include <far_pointers.h>
#include <fkbdef.h>
#include <kpbdef.h>
#include <pbdef.h>
#include <pbodef.h>
#include <pdtdef.h>
#include <rddef.h>
#include <rdtdef.h>
#include <sbdef.h>
#include <sbodef.h>
#include <sppbdef.h>

int   scs_std$accept ( void (*msgadr_p)( unsigned int msg_length, void *msg_buf_p, struct _cdt *cdt_p, struct _pdt *pdt_p ),
	void (*dgadr_p)( unsigned int dg_status, unsigned int dg_length, void *dg_buf_p, struct _cdt *cdt_p, struct _pdt *pdt_p ),
	void (*erradr_p)( unsigned int err_status, unsigned int reason, struct _cdt *cdt_p, struct _pdt *pdt_p),
	int initcr,
	int minscr,
	int initdg,
	int blkpri,
	void *condat_p,
	void *auxstr_p,
	void (*badrsp_p)( __unknown_params ),
	void (*movadr_p)( unsigned int status, unsigned int move_status, struct _cdt *cdt_p, struct _pdt *pdt_p ),
	int load_rating,
	void (*complete_p)( unsigned int accept_status, void *unknown_1, void *unknown_2, struct _cdt *cdt_p,
	                    struct _pdt *pdt_p, int accept_parameter ),
	struct _cdt *cdt_p,
	int accept_parameter
		       );

int   scs_std$allocdg ( struct _pdt *pdt_p, struct _cdrp *cdrp_p );


int   scs_std$alloc_msgbuf ( struct _pdt *pdt_p, struct _cdrp *cdrp_p,
	      void (*ravail_p)( unsigned int stall_return_status, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p ) );


int   scs_std$alloc_rspid ( struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p,
	     void (*ravail_p)( unsigned int stall_return_status, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p ) );


void  scs_std$cancel_mbx ( struct _sppb *sppb );


int   scs_std$cancel_wait ( int rwcptr_upd, struct _pdt *pdt_p, struct _cdrp *cdrp_p );


int   scs_std$change_affinity( struct _ucb *ucb_p );


void  scs_std$cleanup_rbuns( int pooltype, struct _pdt *pdt_p );


int   scs_std$config_pth ( void *rmst_lclprt_p, struct _pbo *pbo_p, struct _pb **pb_p );


int   scs_std$config_sys ( void *scssystemid_p, struct _sbo *sbo_p, struct _sb **sb_p );


int   scs_std$connect ( void (*msgadr_p)( unsigned int msg_length, void *msg_buf_p, struct _cdt *cdt_p, struct _pdt *pdt_p ),
	 void (*dgadr_p)( unsigned int dg_status, unsigned int dg_length, void *dg_buf_p, struct _cdt *cdt_p, struct _pdt *pdt_p ),
	 void (*erradr_p)( unsigned int err_status, unsigned int reason, struct _cdt *cdt_p, struct _pdt *pdt_p),
	 void *rsysid_p,
	 void *rstadr_p,
	 void *rprnam_p,
	 void *lprnam_p,
	 int initcr,
	 int minscr,
	 int initdg,
	 int blkpri,
	 void *condat_p,
	 void *auxstr_p,
	 void (*badrsp_p)( __unknown_params ),
	 void (*movadr_p)( unsigned int status, unsigned int move_status, struct _cdt *cdt_p, struct _pdt *pdt_p ),
	 int load_rating,
	 int (*req_fast_recvmsg_p)( int msg_length, void *msg_buf_p, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp **cdrp_p ),
	 void (*fast_recvmsg_pm_p)( void *msg_buf_p, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p ),
	 void (*change_aff_p)( struct _sb *sb_p, struct _pb *pb_p, struct _cdt *cdt_p, struct _pdt *pdt_p ),
	 void (*complete_p)( unsigned int connect_status, unsigned int reject_reason, void *msg_buf_p,
	                     struct _cdt *cdt_p, struct _pdt *pdt_p, int connect_parameter ),
	 int connect_parameter
			);


void  scs_std$credit_avail( struct _cdt *cdt_p, struct _pdt *pdt_p );


void  scs_std$deallocdg ( void *dgbuf, struct _pdt *pdt_p );


void  scs_std$deallocmsg ( struct _pdt *pdt_p, struct _cdrp *cdrp_p );


void  scs_std$deall_cdt ( struct _cdt *cdt_p );


int   scs_std$deall_rspid ( struct _cdrp *cdrp_p );


void  scs_std$dealrgmsg ( void *msg_buf_p, struct _cdt *cdt_p, struct _pdt *pdt_p );


int   scs_std$disconnect ( int distyp, struct _cdt *cdt_p, 
	    void (*complete_p)( unsigned int disconnect_status, void *unknown_1, void *unknown_2, void *unknown_3,
					       void *unknown_4, int disconnect_parameter ),
	    int disconnect_param );


int   scs_std$fast_recvmsg_chk_res ( struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p );


void  scs_std$fast_recvmsg_pm( struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p, struct _bufdesc **bd_p );


int   scs_std$fast_recvmsg_request( void *msg_buf_p, struct _pdt *pdt_p, struct _cdt **cdt_p, struct _cdrp **cdrp_p );


void  scs_std$fast_sendmsg_ass_res_pm ( void *svapte_boff_bcnt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p );


void  scs_std$fast_sendmsg_pm ( int msg_buf_len, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p,
	         void (*complete)( __unknown_params ) );


int   scs_std$fast_sendmsg_request ( void *svapte_boff_bcnt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p );


int   scs_std$find_bd( struct _bdlptr *bdlt_p, struct _bnam bnam, struct _bufdesc **bd_p );


int   scs_std$find_rdte ( int rspid, struct _scs_struct _rd **rdte_p );


int   scs_std$init_bdlt( struct _pdt *pdt_p );


int   scs_std$initialize_pdt( struct _pdt *pdt_p );


int   scs_std$listen ( void (*msgadr_p)(void *msg_buf_p, struct _cdt *cdt_p, struct _pdt *pdt_p ),
	void (*erradr_p)( unsigned int err_status, unsigned int reason, struct _cdt *cdt_p, struct _pdt *pdt_p),
	void *lprnam_p,
	void *prinfo_p,
	struct _cdt **cdt_p );


void  scs_std$lkp_msgwait ( void (*action)( void *action_param, struct _cdt *cdt_p, struct _cdrp *cdrp_p ),
	     void *action_param, struct _cdt *cdt_p );


void  scs_std$lkp_pb_pdt ( void (*action)( struct _sb *sb_p, struct _pb *pb_p, struct _pdt *pdt_p ), struct _pdt *pdt_p );


void  scs_std$lkp_rdtcdrp ( void (*action)( void *action_param, struct _cdt *cdt_p, struct _cdrp *cdrp_p ),
	     void *action_param, struct _cdt *cdt_p );


void  scs_std$lkp_rdtwait ( void (*action)( void *action_param, struct _cdt *cdt_p, struct _cdrp *cdrp_p ),
	     void *action_param, struct _cdt *cdt_p );


int   scs_std$map ( void *svapte_boff_bcnt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p,
                    void (*ravail_p)( unsigned int stall_return_status, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p ) );


int   scs_std$mreset( int force_flag, void *rstation_p, struct _pdt *pdt_p );


void  scs_std$msg_buf_avail( struct _pdt *pdt_p );


int   scs_std$mstart( int boot_flag, void *rstation_p, void *boot_address, struct _pdt *pdt_p );


void  scs_std$new_pb( struct _pb *pb_p );


void  scs_std$new_sb( struct _sb *sb_p );


void  scs_std$notify_sysap( int status, struct _pb *pb_p, struct _pdt *pdt_p );


int   scs_std$poll_mbx ( int channel_number, void *sysap_name_p, struct _sppb **sppb_p );


int   scs_std$poll_mode ( int enable_disable, struct _sppb *sppb_p, void *scssystemid_p );


int   scs_std$poll_proc ( int (*notification_p)( unsigned int context_data, void *sysap_name_p,
		   void *scssystemid_p, void *process_info_p,
		   void *node_name_p ),
	   unsigned int context_data, void *sysap_name_p, struct _sppb **sppb_p );


void  scs_std$port_init_done( struct _pdt *pdt_p );


int   scs_std$queuedg ( void *dg_buf_p, struct _cdt *cdt_p, struct _pdt *pdt_p );


int   scs_std$queuemdgs ( int buffer_count, struct _cdt *cdt_p, struct _pdt *pdt_p, int *buffers_added_ );


int   scs_std$rchmsgbuf ( struct _pdt *pdt_p, struct _cdrp *cdrp_p,
	   void (*ravail_p)( unsigned int stall_return_status, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p ) );


int   scs_std$rclmsgbuf ( struct _pdt *pdt_p, struct _cdrp *cdrp_p,
	   void (*ravail_p)( unsigned int stall_return_status, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p ) );


void  scs_std$recyl_rspid ( struct _cdrp *cdrp_p );


int   scs_std$reject ( int rejtyp, struct _cdt *cdt_p,
	void (*complete_p)( unsigned int reject_status, void *unknown_1, void *unknown_2, void *unknown_3,
	                    void *unknown_4, int reject_parameter ),
	int reject_parameter );


void  scs_std$repossess_cdrp ( struct _pdt *pdt_p, struct _cdrp *cdrp_p );


int   scs_std$reqdata ( struct _pdt *pdt_p, struct _cdrp *cdrp_p,
	 void (*complete)( unsigned int completion_status, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p ) );


int   scs_std$request_data ( struct _pdt *pdt_p, struct _cdrp *cdrp_p,
	      void (*complete)( unsigned int completion_status, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p ) );


void  scs_std$restore_credit ( struct _pdt *pdt_p, struct _cdrp *cdrp_p );


void  scs_std$resumewaitr( unsigned int resume_status, struct _cdrp *cdrp_p );


void  scs_std$resume_thread( unsigned int resume_status, struct _cdrp *cdrp_p );


int   scs_std$senddata ( struct _pdt *pdt_p, struct _cdrp *cdrp_p,
	  void (*complete)( unsigned int completion_status, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p ) );


int   scs_std$senddata_wmsg ( struct _pdt *pdt_p, struct _cdrp *cdrp_p,
	       void (*complete)( unsigned int completion_status, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p ) );


int   scs_std$senddg ( int disposition_flag, int dg_msg_length, struct _cdrp *cdrp_p );


int   scs_std$sendmsg ( int msg_buf_len, struct _pdt *pdt_p, struct _cdrp *cdrp_p,
	 void (*complete)( unsigned int completion_status, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p ) );


int   scs_std$sendrgdg ( unsigned int disposition_flag, unsigned int dg_msg_length, void *dg_buf_p, struct _cdt *cdt_p, struct _pdt *pdt_p );


int   scs_std$send_data ( struct _pdt *pdt_p, struct _cdrp *cdrp_p,
	   void (*complete)( unsigned int completion_status, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p ) );


int   scs_std$send_data_wmsg ( int msg_buf_len, struct _pdt *pdt_p, struct _cdrp *cdrp_p,
	        void (*complete)( unsigned int completion_status, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p ) );


int   scs_std$set_load_rating ( int rating, struct _cdt *cdt_p );


void  scs_std$shutdown ();


int   scs_std$stall ( int stall_condition_code, struct _cdt *cdt_p, struct _pdt *pdt_p, struct _cdrp *cdrp_p,
                      void (*ravail)( unsigned int stall_return_status, void *cdt_p, void *pdt_p, struct _cdrp *cdrp_p ) );


void  scs_std$start_pwf_recov( struct _pb *pb_p, struct _pdt *pdt_p );


void  scs_std$unmap ( struct _pdt *pdt_p, struct _cdrp *cdrp_p );


void  scs_std$unstallucb ( struct _ucb *ucb_p );


void  scs_std$vc_flush( struct _pb *pb_p, struct _pdt *pdt_p );

#endif 
