#ifndef dsrvdef_h
#define dsrvdef_h

#define		DSRV$M_LOG_ENABLD	0x1
#define		DSRV$M_LOG_PRESENT	0x2
#define		DSRV$M_PKT_LOGGED	0x4
#define		DSRV$M_PKT_LOST		0x8
#define		DSRV$M_LBSTEP1		0x10
#define		DSRV$M_LBSTEP2		0x20
#define		DSRV$M_LBEVENT		0x40
#define		DSRV$M_HULB_DEL		0x80
#define		DSRV$M_MON_ACTIVE	0x100
#define		DSRV$M_LB_REQ		0x200
#define		DSRV$M_CONFIG_WAIT	0x400
#define		DSRV$C_LENGTH		2968
#define		DSRV$K_LENGTH		2968
#define		DSRV$K_AR_ADD		2
#define		DSRV$K_MAX_UNITS	512
#define		DSRV$S_DSRVDEF		2968
	
struct _dsrv {
  void *dsrv$l_flink;
  void *dsrv$l_blink;
  unsigned short int dsrv$w_size;
  unsigned char dsrv$b_type;
  unsigned char dsrv$b_subtype;
  union  {
    unsigned short int dsrv$w_state;
    struct  {
      unsigned dsrv$v_log_enabld : 1;
      unsigned dsrv$v_log_present : 1;
      unsigned dsrv$v_pkt_logged : 1;
      unsigned dsrv$v_pkt_lost : 1;

      unsigned dsrv$v_lbstep1 : 1;
      unsigned dsrv$v_lbstep2 : 1;
      unsigned dsrv$v_lbevent : 1;

      unsigned dsrv$v_hulb_del : 1;
      unsigned dsrv$v_mon_active : 1;
      unsigned dsrv$v_lb_req : 1;
      unsigned dsrv$v_config_wait : 1;
      unsigned dsrv$v_fill_2_ : 5;
    };
  };
  unsigned short int dsrv$w_bufwait;
  void *dsrv$l_log_buf_start;
  void *dsrv$l_log_buf_end;
  void *dsrv$l_next_read;
  void *dsrv$l_next_write;
  unsigned short int dsrv$w_inc_lolim;
  unsigned short int dsrv$w_inc_hilim;
  unsigned short int dsrv$w_exc_lolim;
  unsigned short int dsrv$w_exc_hilim;
  void *dsrv$l_srvbuf;
  void *dsrv$l_free_list;
  unsigned int dsrv$l_avail;
  unsigned int dsrv$l_buffer_min;
  unsigned int dsrv$l_splitxfer;
  struct  {
    unsigned short int dsrv$w_version;
    unsigned short int dsrv$w_cflags;
    unsigned short int dsrv$w_ctimo;
    unsigned short int dsrv$w_reserved;
  };
  unsigned long long dsrv$q_ctrl_id;
  unsigned int dsrv$l_memw_tot;
  unsigned short int dsrv$w_memw_cnt;
  unsigned short int dsrv$w_memw_max;
  void *dsrv$l_memw_fl;
  void *dsrv$l_memw_bl;
  unsigned short int dsrv$w_num_host;
  unsigned short int dsrv$w_num_unit;
  void *dsrv$l_hqb_fl;
  void *dsrv$l_hqb_bl;
  void *dsrv$l_uqb_fl;
  void *dsrv$l_uqb_bl;
  unsigned short int dsrv$w_load_avail;
  unsigned short int dsrv$w_load_capacity;
  unsigned short int dsrv$w_lbload;
  unsigned short int dsrv$w_lbresp;
  unsigned short int dsrv$w_lm_load1;
  unsigned short int dsrv$w_lm_load2;
  unsigned short int dsrv$w_lm_load3;
  unsigned short int dsrv$w_lm_load4;
  unsigned short int dsrv$w_lbinit_cnt;
  unsigned short int dsrv$w_lbfail_cnt;
  unsigned short int dsrv$w_lbreq_cnt;
  unsigned short int dsrv$w_lbresp_cnt;
  unsigned int dsrv$l_lbreq_time;
  unsigned int dsrv$l_lbmon_time;
  struct _fkb *dsrv$l_lm_fkb;
  struct _fkb *dsrv$l_lb_fkb;
  unsigned short int dsrv$w_lm_interval;
  unsigned char dsrv$b_lb_count1;
  unsigned char dsrv$b_lb_count2;
  void *dsrv$l_hulb_fl;
  void *dsrv$l_hulb_bl;
  unsigned char dsrv$b_hosts [32];
  void *dsrv$l_units [512];
  struct  {
    unsigned int dsrv$l_opcount;
    unsigned int dsrv$l_abort_cnt;
    unsigned int dsrv$l_get_cmd_cnt;
    unsigned int dsrv$l_get_unt_cnt;
    unsigned int dsrv$l_set_con_cnt;
    unsigned int dsrv$l_acc_nvm_cnt;
    unsigned int dsrv$l_display_cnt;
    unsigned int dsrv$l_get_unn_cnt;
    unsigned int dsrv$l_avail_cnt;
    unsigned int dsrv$l_onlin_cnt;
    unsigned int dsrv$l_set_unt_cnt;
    unsigned int dsrv$l_det_acc_cnt;
    unsigned int dsrv$l_move_cnt;
    unsigned int dsrv$l_dcd_cnt;
    unsigned int dsrv$l_reserved14;
    unsigned int dsrv$l_reserved15;
    unsigned int dsrv$l_acces_cnt;
    unsigned int dsrv$l_cmp_con_cnt;
    unsigned int dsrv$l_erase_cnt;
    unsigned int dsrv$l_flush_cnt;
    unsigned int dsrv$l_replc_cnt;
    unsigned int dsrv$l_reserved21;
    unsigned int dsrv$l_eraseg_cnt;
    unsigned int dsrv$l_reserved22;
    unsigned int dsrv$l_format_cnt;
    unsigned int dsrv$l_wri_his_cnt;
    unsigned int dsrv$l_reserved26;
    unsigned int dsrv$l_reserved27;
    unsigned int dsrv$l_reserved28;
    unsigned int dsrv$l_reserved29;
    unsigned int dsrv$l_reserved30;
    unsigned int dsrv$l_reserved31;
    unsigned int dsrv$l_cmp_hst_cnt;
    unsigned int dsrv$l_read_cnt;
    unsigned int dsrv$l_write_cnt;
    unsigned int dsrv$l_rea_ced_cnt;
    unsigned int dsrv$l_wri_tm_cnt;
    unsigned int dsrv$l_repos_cnt;
    unsigned int dsrv$l_reserved38;
    unsigned int dsrv$l_reserved39;
    unsigned int dsrv$l_reserved40;
    unsigned int dsrv$l_reserved41;
    unsigned int dsrv$l_reserved42;
    unsigned int dsrv$l_reserved43;
    unsigned int dsrv$l_reserved44;
    unsigned int dsrv$l_reserved45;
    unsigned int dsrv$l_reserved46;
    unsigned int dsrv$l_reserved47;
    unsigned int dsrv$l_terco_cnt;
  };
  unsigned int dsrv$l_vcfail_cnt;
  unsigned int dsrv$l_blkcount [129];
  unsigned int dsrv$l_pcb;
  unsigned int dsrv$l_hrb_tmo_cntr;
};
 
#endif
 
