#ifndef sysap_macros_h
#define sysap_macros_h

#include <scs_routines.h>

#define scs_init_cdrp( cdrp_p )                          
((struct _cdrp *) cdrp_p)->cdrp$q_res_wait_state = 0; \
((struct _cdrp *) cdrp_p)->cdrp$l_bd_addr = 0; \
((struct _cdrp *) cdrp_p)->cdrp$l_rbun = 0;           

#define scs_stall( stall_condition_code, cdt_p, pdt_p, cdrp_p, ravail_p ) \
            scs_std$stall( stall_condition_code, cdt_p, pdt_p, cdrp_p, ravail_p )
#define alloc_msg_buf( pdt_p, cdrp_p, ravail_p )  \
             scs_std$alloc_msgbuf( pdt_p, cdrp_p, ravail_p )

#define recych_msg_buf( pdt_p, cdrp_p, ravail_p ) \
             scs_std$rchmsgbuf( pdt_p, cdrp_p, ravail_p )

#define recycl_msg_buf( pdt_p, cdrp_p, ravail_p ) \
             scs_std$rclmsgbuf( pdt_p, cdrp_p, ravail_p )
#define dealloc_msg_buf( pdt_p, cdrp_p ) \
             scs_std$deallocmsg( pdt_p, cdrp_p )

#define dealloc_msg_buf_reg( msg_buf_p, cdt_p, pdt_p ) \
             scs_std$dealrgmsg( msg_buf_p, cdt_p, pdt_p )

#define restore_credit( pdt_p, cdrp_p ) \
             scs_std$restore_credit( pdt_p, cdrp_p )

#define alloc_dg_buf( pdt_p, cdrp_p ) \
             scs_std$allocdg( pdt_p, cdrp_p )

#define dealloc_dg_buf( dgbuf_p, pdt_p ) \
             scs_std$deallocdg( dgbuf_p, pdt_p )

#define queue_mlt_dgs( buffer_count, cdt_p, pdt_p, buffers_added_p ) \
             scs_std$queuemdgs( buffer_count, cdt_p, pdt_p, buffers_added_p )
#define queue_dg_buf( dg_buf_p, cdt_p, pdt_p ) \
             scs_std$queuedg( dg_buf_p, cdt_p, pdt_p )
#define alloc_rspid( cdt_p, pdt_p, cdrp_p, ravail_p ) \
             scs_std$alloc_rspid( cdt_p, pdt_p, cdrp_p, ravail_p )
#define recycl_rspid( cdrp_p ) \
             scs_std$recyl_rspid( cdrp_p )
#define dealloc_rspid( cdrp_p ) \
             scs_std$deall_rspid( cdrp_p )

#define map( svapte_boff_bcnt_p, pdt_p, cdrp_p, ravail_p )  \
             scs_std$map( svapte_boff_bcnt_p, pdt_p, cdrp_p, ravail_p )

#define unmap( pdt_p, cdrp_p ) \
             scs_std$unmap( pdt_p, cdrp_p )
#define scan_msgbuf_wait( action_p, action_param, cdt_p ) \
             scs_std$lkp_msgwait( action_p, action_param, cdt_p )

#define scan_rdt( action_p, action_param, cdt_p ) \
             scs_std$lkp_rdtcdrp( action_p, action_param, cdt_p )

#define scan_rspid_wait( action_p, action_param, cdt_p ) \
             scs_std$lkp_rdtwait( action_p, action_param, cdt_p )

#define cancel_wait( rwcptr_upd, pdt_p, cdrp_p ) \
             scs_std$cancel_wait( rwcptr_upd, pdt_p, cdrp_p )

#define find_rspid_rdte( rspid, rdte_p ) \
             scs_std$find_rdte( rspid, rdte_p )

#define resume_resource_waiter( resume_status, cdrp_p ) \
             scs_std$resumewaitr( resume_status, cdrp_p )
#define resume_thread( resume_status, cdrp_p ) \
             scs_std$resume_thread( resume_status, cdrp_p )

#define config_pth( rmst_lclprt_p, pbo_p, pb_p ) \
             scs_std$config_pth( rmst_lclprt_p, pbo_p, pb_p )

#define config_sys( scssystemid_p, sbo_p, sb_p ) \
             scs_std$config_sys( scssystemid_p, sbo_p, sb_p )
#define poll_proc( notification_p, context_data_p, sysap_name_p, sppb_p ) \
         scs_std$poll_proc( notification_p, context_data_p, sysap_name_p, sppb_p )

#define poll_mode( enable_disable, sppb_p, scssystemid_p ) \
         scs_std$poll_mode( enable_disable, sppb_p, scssystemid_p )

#define poll_mbx( channel_number, sysap_name_p, sppb_p ) \
         scs_std$poll_mbx( channel_number, sysap_name_p, sppb_p );

#define cancel_mbx( sppb_p ) \
         scs_std$cancel_mbx( sppb_p )
#define mreset( force_flag, rstation_p, pdt_p ) \
             scs_std$mreset( force_flag, rstation_p, pdt_p );

#define mstart( boot_flag, rstation_p, boot_address, pdt_p ) \
             scs_std$mstart( boot_flag, rstation_p, boot_address, pdt_p )

#define listen( msgadr_p, erradr_p, lprname_p, prinfo_p, cdt_p ) \
             scs_std$listen( msgadr_p, erradr_p, lprname_p, prinfo_p, cdt_p )
#define accept( msgadr_p, dgadr_p, erradr_p, initcr, minscr, initdg, blkpri, \
                condat_p, auxstr_p, badrsp_p, movadr_p, load_rating, \
                complete_p, cdt_p, cdrp_p ) \
             scs_std$accept( msgadr_p, dgadr_p, erradr_p, initcr, minscr, \
                             initdg, blkpri, condat_p, auxstr_p, badrsp_p, \
                             movadr_p, (load_rating ? load_rating : CDT$C_YELLOW), \
                             complete_p, cdt_p, cdrp_p )
#define reject( rejtyp, cdt_p, complete_p, cdrp_p ) \
         scs_std$reject( rejtyp, cdt_p, complete_p, cdrp_p )
#define connect( msgadr_p, dgadr_p, erradr_p, rsysid_p, rstadr_p, rprnam_p, \
                 lprnam_p, initcr, minscr, initdg, blkpri, condat_p, auxstr_p, \
                 badrsp_p, movadr_p, load_rating, req_fast_recvmsg_p, \
                 fast_recvmsg_pm_p, change_aff_p, complete_p, connect_parameter ) \
         scs_std$connect( msgadr_p, dgadr_p, erradr_p, rsysid_p, rstadr_p, \
                          rprnam_p, lprnam_p, initcr, minscr, initdg, blkpri, \
                          condat_p, auxstr_p, badrsp_p, movadr_p, \
                          (load_rating ? load_rating : CDT$C_YELLOW), \
                          req_fast_recvmsg_p, fast_recvmsg_pm_p, change_aff_p, \
                          complete_p, connect_parameter )

#define disconnect( distyp, cdt_p, complete_p, disparam ) \
         scs_std$disconnect( distyp, cdt_p, complete_p, disparam )

#define set_load_rating( rating, cdt_p ) \
         scs_std$set_load_rating( rating, cdt_p )

#define reqdata( pdt_p, cdrp_p, complete_p ) \
         scs_std$reqdata( pdt_p, cdrp_p, complete_p )

#define senddata( pdt_p, cdrp_p, complete_p ) \
         scs_std$senddata( pdt_p, cdrp_p, complete_p )

#define senddatawm( pdt_p, cdrp_p, complete_p ) \
         scs_std$senddata_wmsg( pdt_p, cdrp_p, complete_p )

#define request_data( pdt_p, cdrp_p, complete_p ) \
         scs_std$request_data( pdt_p, cdrp_p, complete_p )

#define send_data( pdt_p, cdrp_p, complete_p ) \
         scs_std$send_data( pdt_p, cdrp_p, complete_p )

#define send_data_wmsg( msg_buf_len, pdt_p, cdrp_p, complete_p ) \
         scs_std$send_data_wmsg( msg_buf_len, pdt_p, cdrp_p, complete_p )
#define send_dg_buf( disposition_flag, db_msg_length, cdrp_p ) \
         scs_std$senddg( disposition_flag, db_msg_length, cdrp_p )

#define send_dg_buf_reg( disposition_flag, dg_msg_length, dg_buf_p, cdt_p, pdt_p ) \
         scs_std$sendrgdg( disposition_flag, dg_msg_length, dg_buf_p, cdt_p, pdt_p )

#define sendmsgbuf( pdt_p, cdrp_p, complete_p ) \
         scs_std$sendmsg( SCS$GW_MAXMSG, pdt_p, cdrp_p, complete_p )

#define sendcntmsgbuf( buf_size, pdt_p, cdrp_p, complete_p ) \
         scs_std$sendmsg( buf_size, pdt_p, cdrp_p, complete_p )

#define FAST_SENDMSG_REQUEST( svapte_boff_bcnt_p, pdt_p, cdrp_p ) \
         scs_std$fast_sendmsg_request( svapte_boff_bcnt_p, pdt_p, cdrp_p )

#define fast_sendmsg_associate_pm( svapte_boff_bcnt_p, pdt_p, cdrp_p ) \
         scs_std$fast_sendmsg_ass_res_pm( svapte_boff_bcnt_p, pdt_p, cdrp_p )
#define fast_sendmsg_pm( msg_buf_len, cdt_p, pdt_p, cdrp_p, complete_p ) \
         scs_std$fast_sendmsg_pm( msg_buf_len, cdt_p, pdt_p, cdrp_p, complete_p )
#define fast_recvmsg_chk_res( cdt_p, pdt_p, cdrp_p ) \
         ( cdrp_p->cdrp$l_rbun ? SS$_NORMAL : \
         ( cdrp_p->cdrp$l_scs_state & cdrp$v_rbun_wanted ? \
           scs_std$fast_recvmsg_chk_res( cdt_p, pdt_p, cdrp_p ) : 0 ) )

#define repo_cdrp( pdt_p, cdrp_p ) \
         scs_std$repossess_cdrp( pdt_p, cdrp_p );

#endif 
