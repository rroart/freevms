#ifndef pdtdef_h
#define pdtdef_h
	
#define PDT$M_SNGLHOST 0x1
#define PDT$C_PA 1
#define PDT$C_PU 2
#define PDT$C_PE 3
#define PDT$C_PS 4

#define PDT$C_PI 6
#define PDT$C_PL 7
#define PDT$C_PW 8
#define PDT$C_PN 9
#define PDT$C_PC 10
#define PDT$C_PB 11
#define PDT$C_PM 12
#define PDT$M_CNTBSY 0x1
#define PDT$M_CNTRLS 0x2
#define PDT$M_XCNTRS 0x4
#define PDT$M_NON_CI_BHANDLE 0x8
#define PDT$M_AFFINITY 0x10
#define PDT$C_BASEVER 0
#define PDT$C_LISTENVER 1
#define PDT$C_BALANCEVER 2
#define PDT$C_REORGVER 1
#define PDT$C_CREDITVER 2
#define PDT$C_NI_CLASS 10
#define PDT$C_DSSI_MEDIUM_CLASS 48
#define PDT$C_CI_CLASS 140
#define PDT$C_MC_CLASS 800
#define PDT$C_SM_CLASS 32767

#define PDT$C_YELLOW 4
#define PDT$C_RED 6
#define PDT$C_UNEQUAL_PATH 7
#define PDT$C_CTRSTART 636
#define PDT$C_STD_CTREND 668
#define PDT$C_STDNO_CTR 7
#define PDT$C_EXT_CTRSTART 668
#define PDT$C_EXT_CTREND 776
#define PDT$C_EXTNO_CTR 26
#define SCS$C_EB_MAX_INDEX 9
#define PDT$K_LENGTH 896
#define PDT$C_LENGTH 896

#define PDT$S_PDTDEF 896
	
struct _pdt {
  struct _pdt *pdt$l_flink;
  unsigned short int pdt$w_portchar;
  char pdtdef$$_fill_2;
  unsigned char pdt$b_pdt_type;
  unsigned short int pdt$w_size;
  unsigned char pdt$b_type;
  unsigned char pdt$b_subtyp;
  int (*pdtvec$l_allocdg)();
  int (*pdtvec$l_allocmsg)();
  int (*pdtvec$l_deallocdg)();
  int (*pdtvec$l_deallomsg)();
  int (*pdtvec$l_ins_pes_mfreeq)();
  int (*pdtvec$l_ins_opt_mfreeq)();
  int (*pdtvec$l_rem_opt_mfreeq)();
  int (*pdtvec$l_rem_pes_mfreeq)();
  int (*pdtvec$l_add_free_dg)();
  int (*pdtvec$l_rem_free_dg)();
  int (*pdtvec$l_queue_dg)();
  int (*pdtvec$l_reqdata)();
  int (*pdtvec$l_senddata)();
  int (*pdtvec$l_senddatawmsg)();
  int (*pdtvec$l_senddg)();
  int (*pdtvec$l_sendmsg)();
  int (*pdtvec$l_sendmsgl)();
  int (*pdtvec$l_trnmsgh)();
  int (*pdtvec$l_trnmsgl)();
  int (*pdtvec$l_readcount)();
  int (*pdtvec$l_rlscount)();
  int (*pdtvec$l_mreset)();
  int (*pdtvec$l_mstart)();
  int (*pdtvec$l_stop_vcs)();
  int (*pdtvec$l_shut_all_vc)();
  int (*pdtvec$l_crash_vc)();
  int (*pdtvec$l_crash_port)();
  int (*pdtvec$l_reinit_port)();
  int (*pdtvec$l_flush_vc)();
  int (*pdtvec$l_log_error_scs)();
  int (*pdtvec$l_cleanup_pkt)();
  int (*pdtvec$l_pb_from_msg)();
  int (*pdtvec$l_chk_lost_ack)();
  int (*pdtvec$l_remove_pb)();
  int (*pdt$l_connect)();
  int (*pdt$l_dconnect)();
  int (*pdt$l_add_scs_hdr)();
  int (*pdt$l_cancel_wait)();
  int (*pdt$l_map)();
  int (*pdt$l_unmap)();
  int (*pdt$l_fast_sendmsg_request)();
  int (*pdt$l_fast_sendmsg_ass_res_pm)();
  int (*pdt$l_fast_sendmsg_pm)();
  int (*pdt$l_alloc_rbun)();
  int (*pdt$l_dealloc_rbun)();
  int (*pdt$l_fast_recvmsg_chk_res)();
  int (*pdt$l_test_crash_port)();
  int (*pdt$l_test_ins_comqh)();
  int (*pdt$l_test_1_port)();
  int (*pdt$l_test_2_port)();
  int (*pdtdef$$_fill_4 [7])();
  struct _fkb *pdt$l_waitqfl;
  struct _fkb *pdt$l_waitqbl;
  void *pdt$l_pm_portlock;
  struct _rbun *pdt$l_rbun_list;
  unsigned int pdt$l_rbun_length;
  unsigned int pdt$l_rbun_pooltype;
  unsigned int pdt$l_non_fp_sendmsgs;
  union  {
    unsigned int pdt$l_dghdrsz;
    unsigned int pdt$l_msghdrsz;
  } pdthdrsz;
  unsigned int pdt$l_dgovrhd;
  unsigned int pdt$l_maxbcnt;
  unsigned short int pdt$w_flags;
  short int pdtdef$$_fill_5;
  char pdt$t_cntowner [16];
  struct _cdrp *pdt$l_cntcdrp;
  unsigned int pdt$l_pollsweep;
  struct _ucb *pdt$l_ucb0;
  struct _adp *pdt$l_adp;
  unsigned int pdt$l_max_vctmo;
  unsigned short int pdt$w_scsversion;
  unsigned short int pdt$w_ppdversion;
  int (*pdt$l_load_vector)();
  unsigned short int pdt$w_load_class;
  short int pdtdef$$_fill_6;
  unsigned long long pdt$q_pb;
  unsigned long long pdt$q_conn_wait;
  unsigned long long pdt$q_yellow;
  unsigned long long pdt$q_red;
  unsigned long long pdt$q_disabled;
  unsigned int pdt$l_port_map;
  int pdt$l_avail_thruput;
  unsigned int pdt$l_load_rating;
  unsigned int pdt$l_time_stamp;
  unsigned int pdt$l_saturation_pt;
  unsigned int pdt$l_max_thruput_threshold;
  unsigned int pdt$l_min_thruput_threshold;
  unsigned int pdt$l_tolerance_threshold;
  unsigned int pdt$l_bytes_dg_xmt;
  unsigned int pdt$l_bytes_dg_rcv;
  unsigned int pdt$l_bytes_msg_xmt;
  unsigned int pdt$l_bytes_msg_rcv;
  unsigned int pdt$l_bytes_mapped;
  unsigned int pdt$l_dg_xmt;
  unsigned int pdt$l_dg_rcv;
  unsigned int pdt$l_msg_xmt;
  unsigned int pdt$l_msg_rcv;
  unsigned long long pdt$q_bytes_xfer;
  unsigned int pdt$l_num_map;
  unsigned int pdt$l_port_cmd;
  char pdtdef$$_fill_55 [4];
  unsigned int pdt$l_bytes_dg_xmt_last;
  unsigned int pdt$l_bytes_dg_rcv_last;
  unsigned int pdt$l_bytes_msg_xmt_last;
  unsigned int pdt$l_bytes_msg_rcv_last;
  unsigned int pdt$l_bytes_mapped_last;
  unsigned int pdt$l_dg_xmt_last;
  unsigned int pdt$l_dg_rcv_last;
  unsigned int pdt$l_msg_xmt_last;
  unsigned int pdt$l_msg_rcv_last;
  unsigned long long pdt$q_bytes_xfer_last;
  unsigned int pdt$l_num_map_last;
  unsigned int pdt$l_port_cmd_last;
  int pdtdef$$_fill_66;
  unsigned int pdt$l_bytes_xfer_int;
  unsigned int pdt$l_equal_path_call_count;
  unsigned int pdt$l_unequal_path_call_count;
  unsigned int pdt$l_connection_move_count;
  unsigned int pdt$l_bytes_dg_xmt_peak;
  unsigned int pdt$l_bytes_dg_rcv_peak;
  unsigned int pdt$l_bytes_msg_xmt_peak;
  unsigned int pdt$l_bytes_msg_rcv_peak;
  unsigned int pdt$l_bytes_mapped_peak;
  unsigned int pdt$l_dg_xmt_peak;
  unsigned int pdt$l_dg_rcv_peak;
  unsigned int pdt$l_msg_xmt_peak;
  unsigned int pdt$l_msg_rcv_peak;
  unsigned long long pdt$q_bytes_xfer_peak;
  unsigned int pdt$l_port_cmd_peak;
  unsigned int pdt$l_bytes_dg_xmt_avg;
  unsigned int pdt$l_bytes_dg_rcv_avg;
  unsigned int pdt$l_bytes_msg_xmt_avg;
  unsigned int pdt$l_bytes_msg_rcv_avg;
  unsigned int pdt$l_bytes_mapped_avg;
  unsigned int pdt$l_dg_xmt_avg;
  unsigned int pdt$l_dg_rcv_avg;
  unsigned int pdt$l_msg_xmt_avg;
  unsigned int pdt$l_msg_rcv_avg;
  unsigned long long pdt$q_bytes_xfer_avg;
  unsigned int pdt$l_port_cmd_avg;
  unsigned char pdt$b_ls_flag;
  char pdt$t_fill_0 [3];
  unsigned int pdt$l_stdno_ctr;
  unsigned int pdt$l_path0_ack;
  unsigned int pdt$l_path0_nak;
  unsigned int pdt$l_path0_nrsp;
  unsigned int pdt$l_path1_ack;
  unsigned int pdt$l_path1_nak;
  unsigned int pdt$l_path1_nrsp;
  unsigned int pdt$l_dg_disc;
  unsigned int pdt$l_extno_ctr;
  unsigned int pdt$l_spare1_cnt;
  unsigned int pdt$l_spare2_cnt;
  unsigned int pdt$l_spare3_cnt;
  unsigned int pdt$l_spare4_cnt;
  union  {
    struct  {
      unsigned int pdt$l_snddat_oper_snt;
      unsigned int pdt$l_snddat_data_snt;
      unsigned int pdt$l_snddat_bodies_snt;
      unsigned int pdt$l_reqdat_oper_snt;
      unsigned int pdt$l_retdat_data_rcv;
      unsigned int pdt$l_retdat_bodies_rcv;
      unsigned int pdt$l_sntdat_bodies_rec;
      unsigned int pdt$l_sntdat_data_rec;
      unsigned int pdt$l_cnf_snt;
      unsigned int pdt$l_datreq_bodies_rcv;
      unsigned int pdt$l_retdat_bodies_snt;
      unsigned int pdt$l_retdat_data_snt;
    } pdt1;
    struct  {
      unsigned int pdt$l_np_sntdat_bodies_snt;
      unsigned int pdt$l_np_sntdat_data_snt;
      unsigned int pdt$l_np_cnf_bodies_rcv;
      unsigned int pdt$l_np_reqdat_oper_cmp;
      unsigned int pdt$l_np_retdat_bodies_rcv;
      unsigned int pdt$l_np_retdat_data_rcv;
      unsigned int pdt$l_np_sntdat_bodies_rcv;
      unsigned int pdt$l_np_sntdat_data_rcv;
      unsigned int pdt$l_np_cnf_bodies_snt;
      unsigned int pdt$l_np_reqdat_bodies_rcv;
      unsigned int pdt$l_np_retdat_bodies_snt;
      unsigned int pdt$l_np_retdat_data_snt;
    } pdt2;
  } pdtx;
  unsigned int pdt$l_dgsnt;
  unsigned int pdt$l_dg_txt_snt;
  unsigned int pdt$l_msg_snt;
  unsigned int pdt$l_msg_txt_snt;
  unsigned int pdt$l_misc_snt;
  unsigned int pdt$l_dg_rec;
  unsigned int pdt$l_dg_txtrec;
  unsigned int pdt$l_msg_rec;
  unsigned int pdt$l_msg_txt_rec;
  unsigned int pdt$l_misc_rec;
  unsigned int pdt$l_snddat_data_snt_last;
  unsigned int pdt$l_snddat_oper_snt_last;
  unsigned int pdt$l_retdat_data_rcv_last;
  unsigned int pdt$l_reqdat_oper_snt_last;
  unsigned int pdt$l_cnf_snt_last;
  unsigned int pdt$l_sntdat_data_rec_last;
  unsigned int pdt$l_datreq_bodies_rcv_last;
  unsigned int pdt$l_retdat_data_snt_last;
  unsigned int pdt$l_avg_xfer_size;
  unsigned int pdt$l_eb_table;
  int pdtdef$$_fill_77;
  void *pdt$q_formpb [2];
  unsigned short int pdt$w_pbcount;
  short int pdtdef$$_fill_7;
  union  {
    unsigned char pdt$b_port_num;
    unsigned char pdt$t_port_num [6];
  } pdtport;
  union  {
    unsigned char pdt$b_max_port;
    unsigned char pdt$t_max_port [6];
  } pdtmaxport;
  unsigned int pdt$l_curcnt;
  unsigned int pdt$l_pooldue;
  void *pdt$l_bdlt;
  unsigned char pdt$b_scs_maint_block [16];
  unsigned int pdt$l_tqeaddr;
  unsigned int pdt$l_timvcfail;
  union  {
    unsigned long long pdt$q_mgt_handles;
    struct  {
      unsigned int pdt$l_mgt_handle;
      unsigned int pdt$l_mgt_mgr_handle;
    };
  };
  int pdt$l_mgt_priority;
  char pdt$b_szalign1 [4];
  unsigned long long pdt$q_comql;
  unsigned long long pdt$q_comqh;
  unsigned long long pdt$q_comq2;
  unsigned long long pdt$q_comq3;
  unsigned long long pdt$q_rspq;
  void *pdt$l_dfqhdr;
  void *pdt$l_mfqhdr;
};
 
#define PDT$M_CUR_LBS 0x1
#define PDT$M_PRV_LBS 0x2
#define PDT$M_X_LBS 0x4
	
struct _lbsts {
  union  {
    struct  {
      unsigned pdt$v_cur_lbs : 1;
      unsigned pdt$v_prv_lbs : 1;
      unsigned pdt$v_x_lbs : 1;
      unsigned pdt$v_fill_19_ : 5;
    } pdt$r_lbsts_bits;
    unsigned char pdt$b_lbsts;
  } pdt$r_lbsts_overlay;
} ;
 
#endif
 
