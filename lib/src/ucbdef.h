#ifndef ucbdef_h
#define ucbdef_h

#include "../../freevms/lib/src/tqedef.h"

#define UCB$M_TIM 0x1
#define UCB$M_INT 0x2
#define UCB$M_ERLOGIP 0x4
#define UCB$M_CANCEL 0x8
#define UCB$M_ONLINE 0x10
#define UCB$M_POWER 0x20
#define UCB$M_TIMOUT 0x40
#define UCB$M_INTTYPE 0x80
#define UCB$M_BSY 0x100
#define UCB$M_MOUNTING 0x200
#define UCB$M_DEADMO 0x400
#define UCB$M_VALID 0x800
#define UCB$M_UNLOAD 0x1000
#define UCB$M_TEMPLATE 0x2000
#define UCB$M_MNTVERIP 0x4000
#define UCB$M_WRONGVOL 0x8000
#define UCB$M_DELETEUCB 0x10000
#define UCB$M_LCL_VALID 0x20000
#define UCB$M_SUPMVMSG 0x40000
#define UCB$M_MNTVERPND 0x80000
#define UCB$M_DISMOUNT 0x100000
#define UCB$M_CLUTRAN 0x200000
#define UCB$M_WRTLOCKMV 0x400000
#define UCB$M_SVPN_END 0x800000
#define UCB$M_ALTBSY 0x1000000
#define UCB$M_SNAPSHOT 0x2000000
#define UCB$M_NO_ASSIGN 0x4000000
#define UCB$M_EXFUNC_SUPP 0x8000000
#define UCB$M_FAST_PATH 0x10000000
#define UCB$M_PATHVERIP 0x20000000
#define UCB$M_FP_HW_INT 0x40000000
#define UCB$M_JOB 0x1
#define UCB$M_TEMPL_BSY 0x40
#define UCB$M_PRMMBX 0x1
#define UCB$M_DELMBX 0x2
#define UCB$M_TT_TIMO 0x2
#define UCB$M_TT_NOTIF 0x4
#define UCB$M_TT_HANGUP 0x8
#define UCB$M_TT_NOLOGINS 0x8000
#define UCB$M_NT_BFROVF 0x4
#define UCB$M_NT_NAME 0x10
#define UCB$M_NT_BREAK 0x20
#define UCB$M_ECC 0x1
#define UCB$M_DIAGBUF 0x2
#define UCB$M_NOCNVRT 0x4
#define UCB$M_DX_WRITE 0x8
#define UCB$M_DATACACHE 0x10
#define UCB$M_MSCP_MNTVERIP 0x100
#define UCB$M_MSCP_INITING 0x200
#define UCB$M_MSCP_WAITBMP 0x400
#define UCB$M_MSCP_FLOVR 0x800
#define UCB$M_MSCP_PKACK 0x1000
#define UCB$M_MSCP_WRTP 0x2000
#define UCB$M_MSCP_IGNSRV 0x4000
#define UCB$M_MSCP_MVRESTART 0x8000
#define UCB$M_DU_SHMV_STRTD 0x8
#define UCB$M_DU_0MNOTE 0x20
#define UCB$M_MVFKBBSY 0x40
#define UCB$M_GTUNMBSY 0x80
#define UCB$M_TU_OVRSQCHK 0x1
#define UCB$M_TU_TRACEACT 0x2
#define UCB$M_TU_SEQNOP 0x4
#define UCB$M_TU_1DENS 0x8
#define UCB$M_TU_DENS_DETERMINED 0x10
#define UCB$M_TU_MEDIA_LOADED 0x20
#define UCB$M_SHD_WLG_INV 0x80
#define UCB$M_SHD_SEQCMD_HERE 0x400
#define UCB$M_SHD_SEQCMD_THERE 0x800
#define UCB$M_SHD_PASSIVE_MV 0x1000
#define UCB$M_SHD_NODE_FAILURE 0x2000
#define UCB$M_SHD_WLGSTA_CHA 0x4000
#define UCB$M_SHD_VCB_DEQUEUE 0x8000
#define UCB$M_PORT_ONLINE 0x1
#define UCB$M_FKLOCK 0x2
#define UCB$M_MSGFKLOCK 0x4
#define UCB$M_INIFKLOCK 0x8
#define UCB$M_BAD_REV 0x10
#define UCB$M_PA_ERLOGIP 0x20
#define UCB$M_MFQEFKLOCK 0x40
#define UCB$M_MFQE_LOST 0x80
#define UCB$M_ADMIN_ONLINE 0x100
#define UCB$M_ADMIN_INIT 0x200
#define UCB$M_ADMIN_TEAR_DOWN 0x400
#define UCB$M_CHAN_ONLINE 0x10000
#define UCB$M_CHAN_INIT 0x20000
#define UCB$M_CHAN_TEAR_DOWN 0x40000
#define UCB$M_PB_TQE_BUSY 0x1000000
#define UCB$M_MBR_CALLBACK 0x2000000
#define UCB$M_SHUTDOWN_REQ 0x4000000
#define UCB$M_SHUTDOWN_IP 0x8000000
#define UCB$M_PB_LAST_GASP_EMULATED 0x10000000
	
#define SUD$C_LENGTH 200
#define SUD$K_LENGTH 200
 
#define UCB$K_LENGTH 240
#define UCB$C_LENGTH 240
#define UCB$S_UCBDEF 240

#define UCB$K_LENGTH 240
#define UCB$C_LENGTH 240
#define UCB$S_UCBDEF 240	
 
#define UCB$K_MB_UCBLENGTH 296
#define UCB$C_MB_UCBLENGTH 296
#define UCB$C_MB_LENGTH 296
#define UCB$S_MB_EXTENSION 296
	
#define UCB$K_ERL_LENGTH 256
#define UCB$C_ERL_LENGTH 256
#define UCB$S_UCBDEF4 256
	
#define UCB$K_DP_LENGTH 272
#define UCB$C_DP_LENGTH 272
#define UCB$K_2P_LENGTH 272
#define UCB$C_2P_LENGTH 272
#define UCB$S_DUALPATH_EXTENSION 272
	
#define UCB$M_AST_ARMED 0x8000
#define UCB$K_LCL_DISK_LENGTH 308
#define UCB$C_LCL_DISK_LENGTH 308
#define UCB$K_LCL_TAPE_LENGTH 296
#define UCB$C_LCL_TAPE_LENGTH 296
	
#define UCB$S_DISKTAPE_UCB_EXTENSION 312

#define UCB$K_MSCP_DISK_LENGTH 368
#define UCB$K_MSCP_TAPE_LENGTH 368
#define UCB$S_MSCP_UCB_EXTENSION 368
	
#define UCB$K_DU_LENGTH 400

#define UCB$S_DUDRIVER_EXTENSION 400
	
#define UCB$M_TU_RPTREQ 0x1
#define UCB$M_TU_RPTPND 0x2
#define UCB$M_TU_DENSITY 0x4
	
#define UCB$K_TU_LENGTH 400

#define UCB$S_TUDRIVER_EXTENSION 400
	
#define UCB$M_PK_IFKB_LOCK 0x1
	
#define UCB$K_PK_LENGTH 304
#define UCB$C_PK_LENGTH 304
#define UCB$S_SCSI_PORT_EXTENSION 304
	
#define UCB$M_BACKP 0x20
#define UCB$C_LOGLNK 1
	
#define UCB$S_UCBDEF7 256
	
#define UCB$K_NI_LENGTH 248
#define UCB$C_NI_LENGTH 248
#define UCB$S_UCBDEF9 248
	
#define UCB$K_DAPDEV_LENGTH 272
#define UCB$C_DAPDEV_LENGTH 272
#define UCB$S_UCBDEF10 272
	
#define UCB$K_BGN_ADPTAB 336
#define UCB$M_LBDG 0x1
#define UCB$M_POLL 0x2
#define UCB$M_LOCAL 0x4
#define UCB$M_SINGLE_PATH 0x8
#define UCB$M_STORAGE 0x10
#define UCB$K_BGN_ADPSUB 364
#define UCB$K_BGN_PDTSUB 428
#define UCB$K_END_ADPTAB 468
#define UCB$K_TAB_LEN 132
#define UCB$K_LMPKTBYTS 64
#define UCB$K_ERRDGBYTS 180
#define UCB$K_LMBUFSIZ 104
#define UCB$K_ERRDGSIZ 220
#define UCB$M_RSP_FKB_IN_USE 0x1
	
#define UCB$C_PALENGTH 832
#define UCB$C_PA_LENGTH 832
#define UCB$S_PAUCBDEF 832
	
#define UCB$M_PI_ENABLE 0x1
#define UCB$M_PI_FKB_BUSY 0x2
#define UCB$M_PI_TQE_BUSY 0x4
	
#define UCB$C_PILENGTH 1024
#define UCB$C_PI_LENGTH 1024
#define UCB$S_PIUCBDEF 1024
	
#define UCB$M_PB_OPEN 0x1
#define UCB$M_PB_OPENING 0x2
#define UCB$M_PB_NIP 0x4
#define UCB$M_PB_START 0x8
#define UCB$M_PB_ABORTED 0x1
#define UCB$M_PB_TERMINATED 0x2
#define UCB$M_PB_RWF 0x4
#define UCB$M_PB_IWF 0x8
#define UCB$M_PB_SMSD 0x10
#define UCB$K_A_END 0
#define UCB$K_B_END 1
#define UCB$C_PB_LENGTH 1316
	
#define UCB$C_SD_LENGTH 360
#define UCB$S_SDUCBDEF 360
	
#define SUD$M_AUX_SUD_ALLOC 0x1
#define SUD$M_PATH_AVAILABLE 0x1
#define SUD$M_OK2UINIT 0x2
#define SUD$M_PV_TRIED 0x4
#define SUD$M_PACKACK_TRIED 0x8
#define SUD$M_SWITCH_TRIED 0x10
#define SUD$M_PATH_USER_DISABLED 0x20
#define SUD$M_NOTCURPATH_IOIP 0x40
#define SUD$M_POLL_ENABLED 0x80
#define SUD$M_WWID_PRESENT 0x1
#define SUD$M_FC_PORT_NAME_PRESENT 0x2
#define SUD$M_FC_NODE_NAME_PRESENT 0x4
#define SUD$M_FP_ON_PORT_QUEUE 0x1
#define SUD$M_FP_USER_PREF_CPU 0x2
#define SUD$M_FP_ON_HW_INT_PORT_QUEUE 0x4
#define SUD$M_FP_USR_HW_INT_CPU 0x8
	
struct _ucb2 {
  struct _fkb *ucb$l_fqfl;
  struct _fkb *ucb$l_fqbl;
  unsigned short int ucb$w_size;
  unsigned char ucb$b_type;
  unsigned char ucb$b_flck;
  void (*ucb$l_fpc)(void);
  unsigned long ucb$l_fr3;
  unsigned long ucb$l_fr4;
  unsigned short ucb$w_bufquo;
  unsigned short ucb$w_iniquo;
  struct _orb *ucb$l_orb;
  unsigned long ucb$l_lockid;
  struct _crb *ucb$l_crb;
  struct _spl *ucb$l_dlck;
  struct _ddb *ucb$l_ddb;
  unsigned long ucb$l_pid;
  struct _ucb *ucb$l_link;
  struct _vcb *ucb$l_vcb;
  unsigned long ucb$l_devchar;
  unsigned long ucb$l_devchar2;
  unsigned long ucb$l_affinity;
  unsigned long reserved;
  unsigned char ucb$b_devclass;
  unsigned char ucb$b_devtype;
  unsigned short ucb$w_devbufsiz;
  unsigned long ucb$l_devdepend;
  unsigned long ucb$l_devdepend2;
  struct _irp *ucb$l_ioqfl;
  struct _irp *ucb$l_ioqbl;
  unsigned short ucb$w_unit;
  unsigned short ucb$w_charge;
  struct _irp *ucb$l_irp;
  unsigned short ucb$w_refc;
  unsigned char ucb$b_dipl;
  unsigned char ucb$b_amod;
  struct _ucb *ucb$l_amb;
  unsigned long ucb$l_sts;
  unsigned short ucb$w_devsts;
  unsigned short ucb$w_qlen;
  unsigned long ucb$l_duetim;
  unsigned long ucb$l_opcnt;
  unsigned long ucb$l_svpn;
  void *ucb$l_svapte;
  unsigned short ucb$w_boff;
  unsigned short ucb$w_bcnt;
  unsigned char ucb$b_ertcnt;
  unsigned char ucb$b_ertmax;
  unsigned short ucb$w_errcnt;
  struct _pdt *ucb$l_pdt;
  struct _ddt *ucb$l_ddt;
  unsigned long ucb$l_media_id;
};

struct _ucb {
  union  {
    struct _fkb *ucb$l_fqfl;
    unsigned short int ucb$w_unit_seed;
    unsigned short int ucb$w_mb_seed;
    void *ucb$l_rqfl;
    void *ucb$l_mb_msgqfl;
  };
  union  {
    struct _fkb *ucb$l_fqbl;
    void *ucb$l_rqbl;
    void *ucb$l_mb_msgqbl;
  };
  unsigned short int ucb$w_size;
  unsigned char ucb$b_type;
  unsigned char ucb$b_flck;
  union  {
    void (*ucb$l_fpc)();
    int ucb$l_astqfl;
    struct _acb *ucb$l_mb_w_ast;
    char ucb$t_partner;
  };
  union  {
    unsigned long ucb$l_fr3;
    struct _acb *ucb$l_astqbl;
    struct _acb *ucb$l_mb_r_ast;
  };
  union  {
    unsigned long ucb$l_fr4;
    struct  {
      unsigned short int ucb$w_msgmax;
      unsigned short int ucb$w_msgcnt;
    };
    int ucb$l_first;
  };
  union  {
    unsigned short int ucb$w_bufquo;
    unsigned short int ucb$w_dstaddr;
  };
  union  {
    unsigned short int ucb$w_iniquo;
    unsigned short int ucb$w_srcaddr;
  };
  struct _orb *ucb$l_orb;
  union  {
    unsigned int ucb$l_lockid;
    unsigned int ucb$l_cpid;
  };
  struct _cram *ucb$ps_cram;
  struct _crb *ucb$l_crb;
  struct _spl *ucb$l_dlck;
  struct _ddb *ucb$l_ddb;
  unsigned int ucb$l_pid;
  struct _ucb *ucb$l_link;
  struct _vcb *ucb$l_vcb;
  union  {
    unsigned long long ucb$q_devchar;
    struct  {
      unsigned int ucb$l_devchar;
      unsigned int ucb$l_devchar2;
    };
  };
  unsigned int ucb$l_affinity;
  union  {
    unsigned int ucb$l_xtra;
    int ucb$l_altiowq;

  };
  unsigned char ucb$b_devclass;
  unsigned char ucb$b_devtype;
  unsigned short int ucb$w_devbufsiz;
  union  {
    unsigned long long ucb$q_devdepend;
    struct  {
      union  {
	unsigned int ucb$l_devdepend;
	struct  {
	  unsigned char ucb$b_sectors;
	  unsigned char ucb$b_tracks;
	  unsigned short int ucb$w_cylinders;
	};
	struct  {
	  char ucbdef$$_term_devdepend_fill [3];
	  unsigned char ucb$b_vertsz;
	};
	struct  {
	  unsigned char ucb$b_locsrv;
	  unsigned char ucb$b_remsrv;
	  unsigned short int ucb$w_bytestogo;
	};
      };
      union  {
	unsigned int ucb$l_devdepnd2;
	unsigned int ucb$l_tt_devdp1;
	unsigned short int ucb$w_tu_formenu;
      };
    };
  };
  union  {
    unsigned long long ucb$q_devdepend2;
    struct  {
      union  {
	unsigned int ucb$l_devdepnd3;
      };
      union  {
	unsigned int ucb$l_devdepnd4;
      };
    };
    struct  {
      unsigned short int ucb$w_tmv_bcnt1;
      unsigned short int ucb$w_tmv_bcnt2;
      unsigned short int ucb$w_tmv_bcnt3;
      unsigned short int ucb$w_tmv_bcnt4;
    };
  };
  struct _irp *ucb$l_ioqfl;
  struct _irp *ucb$l_ioqbl;
  unsigned short int ucb$w_unit;
  union  {
    unsigned short int ucb$w_charge;
    unsigned short int ucb$w_rwaitcnt;
    struct  {
      unsigned char ucb$b_cm1;
      unsigned char ucb$b_cm2;
    };
  };
  struct _irp *ucb$l_irp;
  unsigned int ucb$l_refc;
  union  {
    unsigned char ucb$b_dipl;
    unsigned char ucb$b_state;
  };
  unsigned char ucb$b_amod;
  short int ucb$w_fill_0;
  struct _ucb *ucb$l_amb;
  union  {
    unsigned int ucb$l_sts;
    struct  {
      unsigned ucb$v_tim : 1;
      unsigned ucb$v_int : 1;
      unsigned ucb$v_erlogip : 1;
      unsigned ucb$v_cancel : 1;
      unsigned ucb$v_online : 1;
      unsigned ucb$v_power : 1;
      unsigned ucb$v_timout : 1;
      unsigned ucb$v_inttype : 1;
      unsigned ucb$v_bsy : 1;
      unsigned ucb$v_mounting : 1;
      unsigned ucb$v_deadmo : 1;
      unsigned ucb$v_valid : 1;
      unsigned ucb$v_unload : 1;
      unsigned ucb$v_template : 1;
      unsigned ucb$v_mntverip : 1;
      unsigned ucb$v_wrongvol : 1;
      unsigned ucb$v_deleteucb : 1;
      unsigned ucb$v_lcl_valid : 1;
      unsigned ucb$v_supmvmsg : 1;
      unsigned ucb$v_mntverpnd : 1;
      unsigned ucb$v_dismount : 1;
      unsigned ucb$v_clutran : 1;
      unsigned ucb$v_wrtlockmv : 1;
      unsigned ucb$v_svpn_end : 1;
      unsigned ucb$v_altbsy : 1;
      unsigned ucb$v_snapshot : 1;
      unsigned ucb$v_no_assign : 1;
      unsigned ucb$v_exfunc_supp : 1;
      unsigned ucb$v_fast_path : 1;
      unsigned ucb$v_pathverip : 1;
      unsigned ucb$v_fp_hw_int : 1;
      unsigned ucb$v_fill_0_ : 1;
    };
  };
  union  {
    unsigned int ucb$l_devsts;
    struct  {
      unsigned ucb$v_job : 1;
      unsigned ucb$v_devsts_gen_fill : 5;
      unsigned ucb$v_templ_bsy : 1;
      unsigned ucb$v_fill_1_ : 1;
    };
    struct  {
      unsigned ucb$v_prmmbx : 1;
      unsigned ucb$v_delmbx : 1;
      unsigned ucb$v_fill_2_ : 6;
    };
    struct  {
      unsigned ucb$v_devsts_tt_fill : 1;
      unsigned ucb$v_tt_timo : 1;
      unsigned ucb$v_tt_notif : 1;
      unsigned ucb$v_tt_hangup : 1;
      unsigned ucb$v_tt_devsts_fill : 11;
      unsigned ucb$v_tt_nologins : 1;
    };
    struct  {
      unsigned ucb$v_devsts_net_fill1 : 2;
      unsigned ucb$v_nt_bfrovf : 1;
      unsigned ucb$v_devsts_net_fill2 : 1;
      unsigned ucb$v_nt_name : 1;
      unsigned ucb$v_nt_break : 1;
      unsigned ucb$v_fill_3_ : 2;
    };
    struct  {
      unsigned ucb$v_ecc : 1;
      unsigned ucb$v_diagbuf : 1;
      unsigned ucb$v_nocnvrt : 1;
      unsigned ucb$v_dx_write : 1;
      unsigned ucb$v_datacache : 1;
      unsigned ucb$v_fill_4_ : 3;
    };
    struct  {
      unsigned ucb$v_byte_fill_1 : 8;
      unsigned ucb$v_mscp_mntverip : 1;
      unsigned ucb$v_mscp_initing : 1;
      unsigned ucb$v_mscp_waitbmp : 1;
      unsigned ucb$v_mscp_flovr : 1;
      unsigned ucb$v_mscp_pkack : 1;
      unsigned ucb$v_mscp_wrtp : 1;
      unsigned ucb$v_mscp_ignsrv : 1;
      unsigned ucb$v_mscp_mvrestart : 1;
    };
    struct  {
      unsigned ucb$v_unused_fill : 3;
      unsigned ucb$v_du_shmv_strtd : 1;
      unsigned ucb$v_skip_datacache : 1;
      unsigned ucb$v_du_0mnote : 1;
      unsigned ucb$v_mvfkbbsy : 1;
      unsigned ucb$v_gtunmbsy : 1;
    };
    struct  {
      unsigned ucb$v_tu_ovrsqchk : 1;
      unsigned ucb$v_tu_traceact : 1;
      unsigned ucb$v_tu_seqnop : 1;
      unsigned ucb$v_tu_1dens : 1;
      unsigned ucb$v_tu_dens_determined : 1;



      unsigned ucb$v_tu_media_loaded : 1;
      unsigned ucb$v_unused_fill_2 : 2;
    };
    struct  {
      unsigned ucb$v_byte_fill_7 : 7;
      unsigned ucb$v_shd_wlg_inv : 1;
      unsigned ucb$v_byte_fill_2 : 2;
      unsigned ucb$v_shd_seqcmd_here : 1;
      unsigned ucb$v_shd_seqcmd_there : 1;
      unsigned ucb$v_shd_passive_mv : 1;
      unsigned ucb$v_shd_node_failure : 1;
      unsigned ucb$v_shd_wlgsta_cha : 1;
      unsigned ucb$v_shd_vcb_dequeue : 1;
    };
    struct  {
      unsigned ucb$v_port_online : 1;
      unsigned ucb$v_fklock : 1;
      unsigned ucb$v_msgfklock : 1;

      unsigned ucb$v_inifklock : 1;

      unsigned ucb$v_bad_rev : 1;
      unsigned ucb$v_pa_erlogip : 1;

      unsigned ucb$v_mfqefklock : 1;

      unsigned ucb$v_mfqe_lost : 1;

    };
    struct  {
      unsigned ucb$v_byte_fill_6 : 8;
      unsigned ucb$v_admin_online : 1;
      unsigned ucb$v_admin_init : 1;
      unsigned ucb$v_admin_tear_down : 1;
      unsigned ucb$v_byte_fill_8 : 5;
      unsigned ucb$v_chan_online : 1;
      unsigned ucb$v_chan_init : 1;
      unsigned ucb$v_chan_tear_down : 1;
      unsigned ucb$v_byte_fill_9 : 5;
      unsigned ucb$v_pb_tqe_busy : 1;
      unsigned ucb$v_mbr_callback : 1;

      unsigned ucb$v_shutdown_req : 1;
      unsigned ucb$v_shutdown_ip : 1;
      unsigned ucb$v_pb_last_gasp_emulated : 1;

      unsigned ucb$v_fill_5_ : 3;
    };
  };
  int ucb$l_qlen;    struct _ucb *ucb$ps_start_aff_qfl;
  struct _ucb *ucb$ps_start_aff_qbl;
  struct _cpu *ucb$l_port_cpudb;
  int (*ucb$ps_io_counters)();
  unsigned int ucb$l_duetim;
  unsigned int ucb$l_opcnt;
  unsigned int ucb$l_svpn;
  void *ucb$l_svapte;
  unsigned short ucb$w_bcnt;
  unsigned short ucb$w_boff;
  unsigned int ucb$l_softerrcnt;
  unsigned int ucb$l_ertcnt;
  unsigned int ucb$l_ertmax;
  unsigned int ucb$l_errcnt;
  struct _pdt *ucb$l_pdt;
  struct _ddt *ucb$l_ddt;
  struct _adp *ucb$ps_adp;
  struct _crctx *ucb$ps_crctx;
  union  {
    unsigned int ucb$l_media_id;
    struct  {
      unsigned ucb$v_media_id_nn : 7;
      unsigned ucb$v_media_id_n2 : 5;
      unsigned ucb$v_media_id_n1 : 5;
      unsigned ucb$v_media_id_n0 : 5;
      unsigned ucb$v_media_id_t1 : 5;
      unsigned ucb$v_media_id_t0 : 5;
    };
  };
  struct _dtn *ucb$ps_dtn;
  struct _ucb *ucb$ps_dtn_link;
  void (*ucb$ps_toutrout)();
  struct _sud *ucb$ps_sud;
  unsigned char ucb$b_second_time_in_startio;
  unsigned char ucb$b_third_time_in_startio;
};
 
struct _mb_ucb {

  struct _ucb ucb$r_ucb;
  unsigned int ucb$l_mb_readerrefc;
  unsigned int ucb$l_mb_writerrefc;
  struct _irp *ucb$l_mb_readqfl;
  struct _irp *ucb$l_mb_readqbl;
  int ucb$l_mb_writerwaitqfl;
  int ucb$l_mb_writerwaitqbl;
  int ucb$l_mb_readerwaitqfl;
  int ucb$l_mb_readerwaitqbl;
  int ucb$l_mb_nowriterwaitqfl;
  int ucb$l_mb_nowriterwaitqbl;
  int ucb$l_mb_noreaderwaitqfl;
  int ucb$l_mb_noreaderwaitqbl;
  int ucb$l_mb_room_notify;
  union  {
    void *ucb$l_logadr;
    struct  {
      void *ucb$l_mb_logadr;
    };
  };
};
 
struct _erl_ucb {

  struct _ucb ucb$r_ucb;
  void *ucb$l_emb;
  unsigned int ucb$l_func;
  void (*ucb$l_dpc)();
  unsigned short int ucb$w_mt3_density;
  char ucb$b_fill_8_ [2];
};

struct _dp_ucb {

  struct _erl_ucb ucb$r_erlucb;
  union  {
    struct  {
      struct _ddb *ucb$l_dp_ddb;
      struct _ucb *ucb$l_dp_link;
      struct _ucb *ucb$l_dp_altucb;
    };
    struct  {
      struct _ddb *ucb$l_2p_ddb;
      struct _ucb *ucb$l_2p_link;
      struct _ucb *ucb$l_2p_altucb;
    };
  };
  char ucb$b_fill_9_ [4];
};
 
struct _dt_ucb {

  struct _dp_ucb ucb$r_dpucb;
  union  {
    unsigned short int ucb$w_dirseq;
    struct  {
      unsigned ucb$v_filler : 15;
      unsigned ucb$v_ast_armed : 1;
    };
  };
  unsigned char ucb$b_onlcnt;
  union  {
    struct  {
      char ucb$b_reserved;
      unsigned int ucb$l_maxblock;
      unsigned int ucb$l_maxbcnt;
      int ucb$l_dccb;
      unsigned int ucb$l_qlenacc;
      unsigned int ucb$l_usn;
      void *ucb$ps_mount_list;
      char ucb$t_mscp_dsply_path [8];
    };
    struct  {
      unsigned char ucb$b_prev_record;
      unsigned int ucb$l_record;
      int ucb$l_reserved;
      unsigned int ucb$l_tmv_record;
      unsigned short int ucb$w_tmv_crc1;
      unsigned short int ucb$w_tmv_crc2;
      unsigned short int ucb$w_tmv_crc3;
      unsigned short int ucb$w_tmv_crc4;
    };
  };
  unsigned int ucb$l_alloclass;
};
 
struct _mscp_ucb {

  struct _dt_ucb ucb$r_dtucb;
  struct _cddb *ucb$l_cddb;
  struct _cddb *ucb$l_2p_cddb;
  struct _ucb *ucb$l_cddb_link;
  struct _cdt *ucb$l_cdt;
  struct _cddb *ucb$l_wait_cddb;
  struct _cddb *ucb$l_pref_cddb;
  unsigned long long ucb$q_unit_id;
  unsigned short int ucb$w_mscpunit;
  unsigned short int ucb$w_unit_flags;
  unsigned short int ucb$w_lcl_mscpunit;
  unsigned short int ucb$w_srv_mscpunit;
  unsigned int ucb$l_mscpdevparam;
  unsigned char ucb$b_freecap;
  unsigned char ucb$b_fail_mutex;
  unsigned short int ucb$w_mscp_resvdw;
  int ucb$l_shad;
  unsigned int ucb$l_dutufkblink;
};

struct _du_ucb {

  struct _mscp_ucb ucb$r_mscpucb;
  unsigned int ucb$l_du_volser;

  unsigned int ucb$l_du_usize;

  unsigned int ucb$l_du_totsz;

  unsigned short int ucb$w_du_rctsize;
  unsigned char ucb$b_du_rctcpys;
  unsigned char ucb$b_du_rbnptrk;
  unsigned short int ucb$w_du_lbnptrk;
  unsigned short int ucb$w_du_trkpgrp;
  unsigned short int ucb$w_du_grppcyl;
  unsigned short int ucb$w_du_muntc;
  unsigned char ucb$b_du_usvr;
  unsigned char ucb$b_du_uhvr;
  char ucb$b_fill_12_ [6];
};

struct _tu_ucb {

  struct _mscp_ucb ucb$r_mscpucb;
  unsigned int ucb$l_tu_maxwrcnt;
  unsigned short int ucb$w_tu_format;
  unsigned short int ucb$w_tu_speed;
  unsigned short int ucb$w_tu_noise;
  unsigned char ucb$b_tu_softerr;
  union  {
    unsigned char ucb$b_tu_softflags;
    struct  {
      unsigned ucb$v_tu_rptreq : 1;
      unsigned ucb$v_tu_rptpnd : 1;
      unsigned ucb$v_tu_density : 1;
      unsigned ucb$v_fill_13_ : 5;
    };
  };
  int ucb$l_tracebeg;
  int ucb$l_traceptr;
  int ucb$l_tracend;
  int ucb$l_trace_next_slot;
  char ucb$b_fill_14_ [4];
};
 
struct _scsi_ucb {

  struct _erl_ucb ucb$r_erlucb;
  unsigned int ucb$il_pk_cur_data;
  unsigned int ucb$il_pk_resel_id;
  union  {
    unsigned int ucb$il_pk_exflags;
    struct  {
      unsigned ucb$v_pk_ifkb_lock : 1;
      unsigned ucb$v_fill_17_ : 7;
    };
  };
  union  {
    char ucb$ib_pk_inifkblk [32];
    struct _kpb *ucb$ps_pk_inikpb;
  };
  char ucb$b_fill_18_ [4];
};
 
struct _net_ucb {

  struct _ucb ucb$r_ucb;
  void *ucb$l_nt_datssb;
  void *ucb$l_nt_intssb;
  unsigned short int ucb$w_nt_chan;
  union  {
    short int ucbdef$$_fill_3;
    struct  {
      unsigned ucb$v_ltype : 2;
      unsigned ucb$v_segflo : 1;
      unsigned ucb$v_msgflo : 1;
      unsigned ucb$v_msgack : 1;
      unsigned ucb$v_backp : 1;
      unsigned ucb$v_lnkpri : 2;
    };

  };
  char ucb$b_fill_19_ [4];
};
 
struct _ni_ucb {

  struct _ucb ucb$r_ucb;
  void *ucb$l_ni_hwaptr;
  void *ucb$l_ni_mltptr;
};

struct _dap_ucb {

  struct _ucb ucb$r_ucb;
  char ucb$t_dapdevnam [32];
};
 
struct _pa_ucb {

  //  ERL_struct _ucb ucb$r_erlucb; not yet
  unsigned int ucb$l_inifkblk [8];

  unsigned int ucb$l_mfqefkblk [8];    
  unsigned short int ucb$w_incarn;
  short int ucb$w_fill1;
  struct _adp *ucb$l_adp;
  unsigned int ucb$l_inireturn;

  int ucb$l_fill2;    
  unsigned int ucb$l_ndt;
  unsigned char ucb$b_dt;
  union  {
    struct  {
      unsigned ucb$v_lbdg : 1;
      unsigned ucb$v_poll : 1;
      unsigned ucb$v_local : 1;
      unsigned ucb$v_single_path : 1;
      unsigned ucb$v_storage : 1;
      unsigned ucb$v_fill_20_ : 3;
    };
    unsigned char ucb$b_ini_flags;
  };
  short int ucb$w_fill3;
  unsigned char ucb$b_msg_typ;
  unsigned char ucb$b_pdt_type;
  unsigned short int ucb$w_pdt_size;
  unsigned int ucb$l_ppd_size;
  void *ucb$l_lsindx;
  unsigned int ucb$l_lslength;
  void *ucb$l_pmc;    unsigned int ucb$l_rev_table;
  int (*ucb$l_power_fail)();
  int (*ucb$l_init_pdt)();
  int (*ucb$l_start_ucode)();
  int (*ucb$l_enb_int)();
  int (*ucb$l_format_rev)();
  int (*ucb$l_timer)();
  int (*ucb$l_poke_port)();
  int (*ucb$l_interrupt)();
  int (*ucb$l_stop_ucode)();
  int (*ucb$l_unmap_pdt)();
  int (*ucb$l_reg_dump)();
  int (*ucb$l_reg_disp)();
  int (*ucb$l_mem_dump)();
  int (*ucb$l_release_ps)();
  int ucb$l_fill4;
  unsigned int ucb$l_ins_comql;
  unsigned int ucb$l_ins_comqh;
  unsigned int ucb$l_ins_dfreq;
  unsigned int ucb$l_ins_mfreq;
  unsigned int ucb$l_ins_comqm;
  unsigned int ucb$l_init_ablk;
  unsigned int ucb$l_cmpl_int;
  unsigned int ucb$l_misc_int2;
  int ucb$l_fill5 [2];
  unsigned int ucb$l_msgfkblk [8];

  unsigned int ucb$t_opa0_temp [6];
  unsigned char ucb$b_lmest;
  unsigned char ucb$b_lmet;
  unsigned char ucb$b_lmertcnt;
  unsigned char ucb$b_lmertmax;
  unsigned short int ucb$w_lmerrcnt;
  short int ucb$w_fill6;
  unsigned char ucb$n_lsaddr [6];
  unsigned char ucb$n_lsid [6];
  unsigned char ucb$n_rsaddr [6];
  unsigned char ucb$n_rsid [6];
  unsigned int ucb$l_cicmd;
  unsigned short int ucb$w_msgbytcnt;
  unsigned short int ucb$w_msgppdtyp;
  unsigned char ucb$t_msgdata [64];
  char ucb$b_fill7 [116];
  int ucb$l_fill8 [2];
  char ucb$b_align_1 [16];
  unsigned int ucb$l_rspfkblk [8];

  union  {
    unsigned int ucb$l_rsp_sts;
    struct  {
      unsigned ucb$v_rsp_fkb_in_use : 1;
      unsigned ucb$v_fill_21_ : 7;
    };
  };
  int ucb$l_reserved2 [7];
};
 
struct _pi_ucb {

  struct _pa_ucb ucb$r_paucb;
  unsigned char ucb$b_pi_fkb [32];
  unsigned char ucb$b_pi_tqe [64];
  union  {
    unsigned long long ucb$q_pi_tfq;
    struct  {
      void *ucb$l_pi_tfq_flink;
      void *ucb$l_pi_tfq_blink;
    };
  };
  union  {
    unsigned long long ucb$q_pi_tq;
    struct  {
      void *ucb$l_pi_tq_flink;
      void *ucb$l_pi_tq_blink;
    };
  };
  union  {
    unsigned long long ucb$q_pi_ifq;
    struct  {
      void *ucb$l_pi_ifq_flink;
      void *ucb$l_pi_ifq_blink;
    };
  };
  union  {
    unsigned long long ucb$q_pi_iq;
    struct  {
      void *ucb$l_pi_iq_flink;
      void *ucb$l_pi_iq_blink;
    };
  };
  unsigned int ucb$l_pi_sva;
  unsigned int ucb$l_pi_svpn;
  unsigned int ucb$l_pi_script;
  unsigned int ucb$l_pi_tgt_script;
  unsigned int ucb$l_pi_dat_script;
  unsigned int ucb$l_pi_ini_script;
  unsigned int ucb$l_pi_rl_stat;
  unsigned int ucb$l_pi_wl_dsp;
  unsigned int ucb$l_pi_tgt_c710d;
  unsigned int ucb$l_pi_ini_c710d;
  unsigned int ucb$l_pi_curr_script;
  unsigned int ucb$l_pi_dsa;
  unsigned int ucb$l_pi_dsps;
  unsigned int ucb$l_pi_scratch;
  unsigned int ucb$l_pi_exp_int;
  union  {
    struct  {
      unsigned ucb$v_pi_enable : 1;
      unsigned ucb$v_pi_fkb_busy : 1;
      unsigned ucb$v_pi_tqe_busy : 1;
      unsigned ucb$v_fill_30_ : 5;
    };
    unsigned char ucb$b_pi_flags;
  };
  char ucb$b_fill_31_ [3];
};
 
 
// not yet #include <shm_iddef.h>

struct _pb_ucb {

  struct _pi_ucb ucb$r_piucb;
  struct _cbb *ucb$pq_pb_gnode_cbb;

  void *ucb$ps_pb_gnode_ucb_list;
  struct _ucb *ucb$ps_pb_template_ucb;
  //  SHM_ID ucb$r_pb_smci_section_id;   // not yet

  unsigned int ucb$l_pb_status;
  struct _tqe *ucb$ps_pb_tqe;
  struct _idb *ucb$ps_pb_idb;
  struct _ucb *ucb$ps_pb_admin_ucb;
  struct _tqe *ucb$ps_pb_discovery_tqe;
  unsigned int ucb$l_pb_remote_gnode;
  unsigned long long ucb$q_pb_incarnation;

  union  {
    unsigned int ucb$l_pb_chansts;
    struct  {
      union  {
	unsigned short int ucb$w_pb_chansts;
	struct  {
	  unsigned ucb$v_pb_open : 1;
	  unsigned ucb$v_pb_opening : 1;
	  unsigned ucb$v_pb_nip : 1;
	  unsigned ucb$v_pb_start : 1;
	  unsigned ucb$v_fill_32_ : 4;
	};
      };
      union  {
	unsigned short int ucb$w_pb_chanerr;
	struct  {
	  unsigned ucb$v_pb_aborted : 1;
	  unsigned ucb$v_pb_terminated : 1;
	  unsigned ucb$v_pb_rwf : 1;
	  unsigned ucb$v_pb_iwf : 1;
	  unsigned ucb$v_pb_smsd : 1;
	  unsigned ucb$v_fill_33_ : 3;
	};
      };
    };
  };
  unsigned int ucb$l_pb_lnode_idx;
  unsigned int ucb$l_pb_rnode_idx;
  void *ucb$ps_pb_fkb;
  void *ucb$pq_pb_nb;

  int ucb$l_pb_nb_len;
  void *ucb$ps_pb_smh;
  void *ucb$ps_pb_smh_self;
  unsigned int ucb$l_pb_gnode [2];
  unsigned int ucb$l_pb_node_block [2];
  unsigned int ucb$l_pb_workq [4];
  unsigned int ucb$l_pb_freeq;
  //  SHM_ID ucb$r_pb_reg_id; not yet

  unsigned int ucb$l_pb_msg_nworkq;
  unsigned int ucb$l_pb_msg_buffers;
  unsigned int ucb$l_pb_msg_dg_size;
  unsigned int ucb$l_pb_msg_scshdr_size;
  unsigned int ucb$l_pb_msg_smwe_size;
  unsigned int ucb$l_pb_msg_smch_size;
  unsigned int ucb$l_pb_msg_smnd_size;
  unsigned int ucb$l_pb_msg_netsize;
  unsigned int ucb$l_pb_channel_add;
  unsigned int ucb$l_dbg [30];
  char ucb$b_fill_34_ [4];
};
 
struct _sd_ucb {

  struct _dt_ucb ucb$r_dtucb;
	
  unsigned long long ucb$q_sd_export_queue;
  void *ucb$ps_sd_sdcb;
  void *ucb$ps_sd_pool_config;
  unsigned int ucb$l_sd_family_number;
  unsigned int ucb$l_sd_snap_number;
  unsigned int ucb$l_sd_dev_lockid;
  unsigned int ucb$l_sd_uid_lockid;
  unsigned int ucb$l_sd_spare1;
  unsigned int ucb$l_sd_spare2;
  unsigned int ucb$l_sd_spare3;
  unsigned int ucb$l_sd_spare4;
};
struct _sud {
  struct _ucb *sud$ps_ucblink;
  void *sud$ps_aux_sud;
  unsigned short int sud$w_size;
  unsigned char sud$b_type;
  unsigned char sud$b_subtype;

  union  {
    unsigned int sud$l_flags;
    struct  {
      unsigned sud$v_aux_sud_alloc : 1;
      unsigned sud$v_fill_41_ : 7;
    };
  };
  union  {
    unsigned int sud$l_sts;
    struct  {
      unsigned sud$v_path_available : 1;
      unsigned sud$v_ok2uinit : 1;

      unsigned sud$v_pv_tried : 1;

      unsigned sud$v_packack_tried : 1;

      unsigned sud$v_switch_tried : 1;

      unsigned sud$v_path_user_disabled : 1;
      unsigned sud$v_notcurpath_ioip : 1;

      unsigned sud$v_poll_enabled : 1;
    };
  };
  union  {
    unsigned int sud$l_devchar3;
    struct  {
      unsigned sud$v_wwid_present : 1;
      unsigned sud$v_fc_port_name_present : 1;
      unsigned sud$v_fc_node_name_present : 1;
      unsigned sud$v_fill_42_ : 5;
    };
  };
  unsigned long long sud$q_scssystemid;
  unsigned int sud$l_qslun;
  struct _qscub *sud$ps_qscub_ptr;
  struct _qssub *sud$ps_qssub_ptr;
  unsigned int sud$l_qsrv_path_prio;
  unsigned int sud$l_wwid_offset;
  unsigned int sud$l_wwid_length;
  struct _mpdev *sud$ps_mpdev;
  struct _ucb *sud$ps_mpdev_primary_ucb;
  struct _ucb *sud$ps_mpdev_next_ucb;
  char sud$b_fill_43_ [4];
  unsigned long long sud$q_mpdev_switch_to_time;
  unsigned long long sud$q_mpdev_switch_from_time;
  struct _mpdev_ppb *sud$ps_mpdev_ppb;
  union  {
    struct  {
      unsigned int sud$l_fp_flags;
    };
    struct  {
      unsigned sud$v_fp_on_port_queue : 1;
      unsigned sud$v_fp_user_pref_cpu : 1;
      unsigned sud$v_fp_on_hw_int_port_queue : 1;
      unsigned sud$v_fp_usr_hw_int_cpu : 1;
      unsigned sud$v_fill_44_ : 4;
    };
  };

  struct _cpu *sud$l_fp_usrprf_cpudb;
  struct _sud *sud$l_fp_ports_link;
  void *sud$l_fp_asgn_ports_fl;
  void *sud$l_fp_asgn_ports_bl;

  struct _cpu *sud$l_fp_usr_hw_int_cpu;
  struct _cpu *sud$l_fp_usrprf_hw_int_cpu;
  struct _sud *sud$l_fp_hw_int_ports_link;
  void *sud$l_fp_asgn_hw_int_ports_fl;
  void *sud$l_fp_asgn_hw_int_ports_bl;

  int sud$l_fp_spare1;
  int sud$l_fp_spare2;
  int sud$l_fp_spare3;
  int sud$l_fp_spare4;

  unsigned int sud$l_mpdev_pv_iost1;
  unsigned int sud$l_mpdev_pv_abstim;
  unsigned int sud$l_mpdev_tracking_start;
  unsigned int sud$l_mpdev_tracking_count;
  unsigned int sud$l_mpdev_pv_duration;
  int sud$l_mpdev_spare_4;
  int sud$l_mpdev_spare_5;
  int sud$l_mpdev_spare_6;
  int sud$l_mpdev_path_id;
  struct _fc_wwid_64b *sud$ps_fc_port_name;
  struct _fc_wwid_64b *sud$ps_fc_node_name;
  struct _irp *sud$l_busy_bit_irp_p;
  char sud$b_fill_45_ [4];
};

#define ucb$r_mscp_ucb ucb$r_dtucb.ucb$r_dpucb.ucb$r_erlucb.ucb$r_ucb

#endif
