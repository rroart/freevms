#ifndef PCBDEF_H
#define PCBDEF_H

#include <vms_types.h>

#define PCB$M_RES 0x1
#define PCB$M_DELPEN 0x2
#define PCB$M_FORCPEN 0x4
#define PCB$M_INQUAN 0x8
#define PCB$M_PSWAPM 0x10
#define PCB$M_RESPEN 0x20
#define PCB$M_SSFEXC 0x40
#define PCB$M_SSFEXCE 0x80
#define PCB$M_SSFEXCS 0x100
#define PCB$M_SSFEXCU 0x200
#define PCB$M_SSRWAIT 0x400
#define PCB$M_SUSPEN 0x800
#define PCB$M_WAKEPEN 0x1000
#define PCB$M_WALL 0x2000
#define PCB$M_BATCH 0x4000
#define PCB$M_NOACNT 0x8000
#define PCB$M_NOSUSPEND 0x10000
#define PCB$M_ASTPEN 0x20000
#define PCB$M_PHDRES 0x40000
#define PCB$M_HIBER 0x80000
#define PCB$M_LOGIN 0x100000
#define PCB$M_NETWRK 0x200000
#define PCB$M_PWRAST 0x400000
#define PCB$M_NODELET 0x800000
#define PCB$M_DISAWS 0x1000000
#define PCB$M_INTER 0x2000000
#define PCB$M_RECOVER 0x4000000
#define PCB$M_SECAUDIT 0x8000000
#define PCB$M_HARDAFF 0x10000000
#define PCB$M_ERDACT 0x20000000
#define PCB$M_SOFTSUSP 0x40000000
#define PCB$M_PREEMPTED 0x80000000
#define PCB$M_QUANTUM_RESCHED 0x1
#define PCB$M_DISABLE_PREEMPT_PKTA_LOCK 0x2
#define PCB$M_FREDLOCK 0x4
#define PCB$M_PHDLOCK 0x8
#define PCB$M_TCB 0x10
#define PCB$M_TBS_STATE_PENDING 0x20
#define PCB$M_CLASS_SCHED_PERM 0x8000
#define PCB$M_TERM_NOTIFY 0x10000
#define PCB$M_BYTLM_LOAN 0x20000
#define PCB$M_DISABLE_PREEMPT 0x40000
#define PCB$M_NOUNSHELVE 0x80000
#define PCB$M_SHELVING_RESERVED 0x100000
#define PCB$M_CLASS_SCHEDULED 0x200000
#define PCB$M_CLASS_SUPPLIED 0x400000
#define PCB$M_IN_TBS_STATE 0x800000
#define PCB$M_WINDFALL 0x1000000
#define PCB$M_NOTIFY 0x2000000
#define PCB$M_SINGLE_THREADED 0x3C000000
#define PCB$M_RWAST 0x40000000
#define PCB$M_SOFT_SINGLE_THREAD 0x80000000
#define PCB$M_EPID_WILD 0x80000000
#define PCB$M_FORK 0x1
#define PCB$K_SCHED_OTHER 0
#define PCB$K_SCHED_FIFO 1
#define PCB$K_SCHED_RR 2

#define PCB$K_SCHED_POLICY_CNT 3
#define PCB$K_ALL_THREADS -2147483648
#define PCB$K_MAX_KT_COUNT 256
#define PCB$M_EVENT_NO_FLAG 0x1

#define PCB$K_LENGTH 952
#define PCB$C_LENGTH 952
#define PCB$S_PCBDEF 952
#define KTB$M_RES 0x1
#define KTB$M_DELPEN 0x2
#define KTB$M_FORCPEN 0x4
#define KTB$M_INQUAN 0x8
#define KTB$M_PSWAPM 0x10
#define KTB$M_RESPEN 0x20
#define KTB$M_SSFEXC 0x40
#define KTB$M_SSFEXCE 0x80
#define KTB$M_SSFEXCS 0x100
#define KTB$M_SSFEXCU 0x200
#define KTB$M_SSRWAIT 0x400
#define KTB$M_SUSPEN 0x800
#define KTB$M_WAKEPEN 0x1000
#define KTB$M_WALL 0x2000
#define KTB$M_BATCH 0x4000
#define KTB$M_NOACNT 0x8000
#define KTB$M_NOSUSPEND 0x10000
#define KTB$M_ASTPEN 0x20000
#define KTB$M_PHDRES 0x40000
#define KTB$M_HIBER 0x80000
#define KTB$M_LOGIN 0x100000
#define KTB$M_NETWRK 0x200000
#define KTB$M_PWRAST 0x400000
#define KTB$M_NODELET 0x800000
#define KTB$M_DISAWS 0x1000000
#define KTB$M_INTER 0x2000000
#define KTB$M_RECOVER 0x4000000
#define KTB$M_SECAUDIT 0x8000000
#define KTB$M_HARDAFF 0x10000000
#define KTB$M_ERDACT 0x20000000
#define KTB$M_SOFTSUSP 0x40000000
#define KTB$M_PREEMPTED 0x80000000
#define KTB$M_QUANTUM_RESCHED 0x1
#define KTB$M_PHDLOCK 0x8
#define KTB$M_TCB 0x10
#define KTB$M_TBS_STATE_PENDING 0x20
#define KTB$M_CLASS_SCHED_PERM 0x8000
#define KTB$M_TERM_NOTIFY 0x10000
#define KTB$M_BYTLM_LOAN 0x20000
#define KTB$M_NOUNSHELVE 0x80000
#define KTB$M_SHELVING_RESERVED 0x100000
#define KTB$M_CLASS_SCHEDULED 0x200000
#define KTB$M_CLASS_SUPPLIED 0x400000
#define KTB$M_IN_TBS_STATE 0x800000
#define KTB$M_WINDFALL 0x1000000
#define KTB$M_NOTIFY 0x2000000
#define KTB$M_SINGLE_THREADED 0x3C000000
#define KTB$M_EPID_WILD 0x80000000
#define KTB$K_SCHED_OTHER 0
#define KTB$K_SCHED_FIFO 1
#define KTB$K_SCHED_RR 2

#define KTB$K_SCHED_POLICY_CNT 3
#define KTB$M_DELETE_PENDING 0x1
#define KTB$M_SCHED_CONTEXT_SAVED 0x2
#define KTB$M_SINGLE_THREAD_ACT 0x3C
#define KTB$M_TOLERANT 0x40
#define KTB$M_SOFT_RAD_AFFINITY 0x80

#define KTB$K_LENGTH 952
#define KTB$C_LENGTH 952
#define KTB$S_KTBDEF 952

#include <linux/sched.h>

/* Do not use this struct now */
/* Redefined as task_struct in linux/sched.h */
/*
struct _pcb {
  struct _pcb *pcb$l_sqfl;
  struct _pcb *pcb$l_sqbl;
  UINT16 pcb$w_size;
  UINT8 pcb$b_type;
  UINT8 pcb$b_fill_1;
  UINT32 pcb$l_ast_pending;
  UINT64 pcb$q_phypcb;
  union  {
    UINT64 pcb$q_lefc_swapped;
    struct  {
      UINT32 pcb$l_lefc_0_swapped;
      UINT32 pcb$l_lefc_1_swapped;
    };
  };
  struct _acb *pcb$l_astqfl_spk;
  struct _acb *pcb$l_astqbl_spk;
  struct _acb *pcb$l_astqfl_k;
  struct _acb *pcb$l_astqbl_k;
  struct _acb *pcb$l_astqfl_e;
  struct _acb *pcb$l_astqbl_e;
  struct _acb *pcb$l_astqfl_s;
  struct _acb *pcb$l_astqbl_s;
  struct _acb *pcb$l_astqfl_u;
  struct _acb *pcb$l_astqbl_u;
  INT32 pcb$l_prvcpu;
  INT32 pcb$l_cpu_id;
  UINT64 pcb$q_prvasn;
  UINT64 pcb$q_prvasnseq;
  UINT64 pcb$q_oncpucnt;
  UINT32 pcb$l_astact;
  UINT32 pcb$l_state;
  UINT32 pcb$l_pri;
  UINT32 pcb$l_prib;
  UINT32 pcb$l_affinity_skip;
  UINT32 pcb$l_owner;
  union  {
    UINT32 pcb$l_sts;
    struct  {
      unsigned pcb$v_res : 1;
      unsigned pcb$v_delpen : 1;
      unsigned pcb$v_forcpen : 1;
      unsigned pcb$v_inquan : 1;
      unsigned pcb$v_pswapm : 1;
      unsigned pcb$v_respen : 1;
      unsigned pcb$v_ssfexc : 1;
      unsigned pcb$v_ssfexce : 1;
      unsigned pcb$v_ssfexcs : 1;
      unsigned pcb$v_ssfexcu : 1;
      unsigned pcb$v_ssrwait : 1;
      unsigned pcb$v_suspen : 1;
      unsigned pcb$v_wakepen : 1;
      unsigned pcb$v_wall : 1;
      unsigned pcb$v_batch : 1;
      unsigned pcb$v_noacnt : 1;
      unsigned pcb$v_nosuspend : 1;
      unsigned pcb$v_astpen : 1;
      unsigned pcb$v_phdres : 1;
      unsigned pcb$v_hiber : 1;
      unsigned pcb$v_login : 1;
      unsigned pcb$v_netwrk : 1;
      unsigned pcb$v_pwrast : 1;
      unsigned pcb$v_nodelet : 1;
      unsigned pcb$v_disaws : 1;
      unsigned pcb$v_inter : 1;
      unsigned pcb$v_recover : 1;
      unsigned pcb$v_secaudit : 1;
      unsigned pcb$v_hardaff : 1;
      unsigned pcb$v_erdact : 1;
      unsigned pcb$v_softsusp : 1;
      unsigned pcb$v_preempted : 1;
    };
  };
  union  {
    UINT32 pcb$l_sts2;
    struct  {
      unsigned pcb$v_quantum_resched : 1;
      unsigned pcb$v_disable_preempt_pkta_lock : 1;
      unsigned pcb$v_fredlock : 1;
      unsigned pcb$v_phdlock : 1;
      unsigned pcb$v_tcb : 1;
      unsigned pcb$v_tbs_state_pending : 1;
      unsigned pcb$v_fill_3 : 9;
      unsigned pcb$v_class_sched_perm : 1;
      unsigned pcb$v_term_notify : 1;
      unsigned pcb$v_bytlm_loan : 1;
      unsigned pcb$v_disable_preempt : 1;
      unsigned pcb$v_nounshelve : 1;
      unsigned pcb$v_shelving_reserved : 1;
      unsigned pcb$v_class_scheduled : 1;
      unsigned pcb$v_class_supplied : 1;


      unsigned pcb$v_in_tbs_state : 1;
      unsigned pcb$v_windfall : 1;
      unsigned pcb$v_notify : 1;
      unsigned pcb$v_single_threaded : 4;
      unsigned pcb$v_rwast : 1;
      unsigned pcb$v_soft_single_thread : 1;
    };
  };
  UINT32 pcb$l_prisav;
  UINT32 pcb$l_pribsav;
  UINT32 pcb$l_authpri;
  UINT32 pcb$l_onqtime;

  UINT32 pcb$l_waitime;
  UINT32 pcb$l_astcnt;
  UINT32 pcb$l_biocnt;
  UINT32 pcb$l_biolm;
  INT32 pcb$l_diocnt;
  INT32 pcb$l_diolm;
  UINT32 pcb$l_prccnt;
  char pcb$t_terminal [8];

  UINT32 pcb$l_wefc;
  UINT32 pcb$l_efwm;
  UINT32 pcb$l_efcs;
  UINT32 pcb$l_efcu;
  union  {
    struct  {
      INT32 pcb$l_efc2p;
      INT32 pcb$l_efc3p;
    };
    struct  {
      UINT16 pcb$w_pgflchar;
      UINT8 pcb$b_pgflindex;
    };
  };
  UINT32 pcb$l_pid;

  union  {
    UINT32 pcb$l_epid;
    struct  {
      unsigned pcb$v_epid_proc : 21;
      unsigned pcb$v_epid_node_idx : 8;
      unsigned pcb$v_epid_node_seq : 2;
      unsigned pcb$v_epid_wild : 1;
    };
  };

  UINT32 pcb$l_eowner;
  UINT32 pcb$l_aptcnt;
  UINT32 pcb$l_mtxcnt;
  UINT32 pcb$l_gpgcnt;
  UINT32 pcb$l_ppgcnt;
  void *pcb$l_wsswp;
  UINT32 pcb$l_swapsize;
  struct _phd *pcb$l_phd;
  struct _jib *pcb$l_jib;
  struct  {
    UINT64 pcb$q_priv;
    struct _arb *pcb$l_arb;
    INT8 pcb$$$_arb_fill_1 [48];
    union  {
      UINT32 pcb$l_uic;
      struct  {
    UINT16 pcb$w_mem;
    UINT16 pcb$w_grp;
      };
    };
    INT8 pcb$$$_arb_fill_2 [60];
  };
  struct _orb *pcb$l_orb;
  UINT32 pcb$l_tmbu;
  UINT32 pcb$l_home_rad;
  INT8 pcb$b_spare_1 [4];
  INT32 pcb$l_dlckpri;
  UINT32 pcb$l_defprot;
  struct _pmb *pcb$l_pmb;
  INT32 pcb$l_affinity;
  UINT32 pcb$l_capability;
  UINT32 pcb$l_cputim;
  char pcb$t_lname [16];
  struct _pdb *pcb$l_prcpdb;

  UINT32 pcb$l_pixhist;
  INT32 pcb$l_affinity_callback;
  UINT32 pcb$l_permanent_capability;
  INT32 pcb$l_permanent_cpu_affinity;
  UINT64 pcb$q_cwpssrv_queue;
  UINT32 pcb$l_current_affinity;
  INT32 pcb$l_capability_seq;
  UINT64 pcb$q_bufobj_list;
  UINT32 pcb$l_ast_blocked;
  void *pcb$l_class_quant;
  UINT16 pcb$w_class_extra_ticks;
  UINT8 pcb$b_pkta_lock;
  UINT8 pcb$b_fill_2;
  void *pcb$a_current_tx;
  void *pcb$a_current_cd;
  void *pcb$a_current_vertex;
  union  {
    UINT64 pcb$q_xscb_que;
    struct  {
      struct _xscb *pcb$a_xscb_flink;
      struct _xscb *pcb$a_xscb_blink;
    };
  };
  union  {
    UINT64 pcb$q_rmcb_que;
    struct  {
      struct _rmcb *pcb$a_rmcb_flink;
      struct _rmcb *pcb$a_rmcb_blink;
    };
  };
  union  {
    UINT64 pcb$q_cd_que;
    struct  {
      struct _cde *pcb$a_cd_flink;
      struct _cde *pcb$a_cd_blink;
    };
  };
  UINT32 pcb$l_dpc;
  UINT32 pcb$l_cputime_ref;
  UINT32 pcb$l_acc_waitime;
  INT32 pcb$l_prcstr;
  void *pcb$l_xpcb;
  UINT32 pcb$l_psx_fork_status;
  union  {
    UINT32 pcb$l_psx_flags;
    struct  {
      unsigned pcb$v_fork : 1;
      unsigned pcb$v_psx_flags_fill : 31;
    };
  };
  void (*pcb$l_psx_actrtn)(void);
  UINT64 pcb$q_psx_actprm;
  UINT32 pcb$l_kernel_counter;
  UINT32 pcb$l_exec_counter;
  UINT32 pcb$l_super_counter;
  UINT32 pcb$l_user_counter;
  UINT32 pcb$l_sched_policy;
  int (*pcb$a_frewsle_callout)(void);
  union  {
    UINT32 pcb$l_frewsle_param;
    INT32 pcb$l_pqb;

  };
  UINT32 pcb$l_bufobj_cnt;
  UINT32 pcb$l_noaudit;
  UINT32 pcb$l_source_epid;
  union  {
    UINT64 pcb$q_rdpb_que;
    struct  {
      struct _rdpb *pcb$a_rdpb_flink;
      struct _rdpb *pcb$a_rdpb_blink;
    };
  };
  UINT64 pcb$q_files_64;
  INT64 pcb$q_keep_in_ws;
  INT64 pcb$q_keep_in_ws2;
  UINT32 pcb$l_active_cpus;
  UINT32 pcb$l_tquantum;
  UINT32 pcb$l_multithread;
  UINT32 pcb$l_kt_count;
  UINT32 pcb$l_kt_high;
  void *pcb$l_ktbvec;
  struct _acb *pcb$l_wake_acb;
  UINT32 pcb$l_st_ack_count;
  union  {
    UINT32 pcb$l_thread_events;
    struct  {
      unsigned pcb$v_event_no_flag : 1;
      unsigned pcb$v_fill_20_ : 7;
    };
  };
  struct _acb *pcb$l_postef_acb;
  union  {
    UINT64 pcb$q_postef;
    struct  {
      UINT32 pcb$l_postef1;
      UINT32 pcb$l_postef2;
    };
  };
  UINT32 pcb$l_swp_seq;
  UINT32 pcb$l_swp_kt;
  struct _acb *pcb$l_im_astqfl_spk;
  struct _acb *pcb$l_im_astqbl_spk;
  struct _acb *pcb$l_im_astqfl_k;
  struct _acb *pcb$l_im_astqbl_k;
  struct _acb *pcb$l_im_astqfl_e;
  struct _acb *pcb$l_im_astqbl_e;
  void *pcb$ps_ccbsva;
  UINT32 pcb$l_maxfix;
  void *pcb$ps_fandle;
  UINT64 pcb$q_st_kt_array;
  struct _psb *pcb$ar_natural_psb;
  INT8 pcb$b_spare_2 [4];
  union  {
    struct _ktb *pcb$l_initial_ktb;
    struct _pcb *pcb$l_pcb;
  };
  INT8 pcb$b_ktb_padding_1 [116];
  void *pcb$l_ctx_waitq;
  struct _lkb *pcb$q_lockqfl;
  struct _lkb *pcb$q_lockqbl;
  INT8 pcb$b_ktb_padding_2 [40];
  struct _pcb *pcb$l_class_link;
  void *pcb$l_session_id;
  void *pcb$l_process_group;
  UINT32 pcb$l_creator;
  UINT32 pcb$l_lckrq;
  INT8 pcb$b_fill_21_ [4];
};
*/

/* Do not use this struct now */
/* Redefined as task_struct in linux/sched.h */
/*
struct ktb {
  struct _ktb *ktb$l_sqfl;
  struct _ktb *ktb$l_sqbl;
  UINT16 ktb$w_size;
  UINT8 ktb$b_type;
  UINT8 ktb$b_fill_1;
  UINT32 ktb$l_ast_pending;
  UINT64 ktb$q_phypcb;
  INT8 ktb$b_pcb_padding_1 [8];
  struct _acb *ktb$l_astqfl_spk;
  struct _acb *ktb$l_astqbl_spk;
  struct _acb *ktb$l_astqfl_k;
  struct _acb *ktb$l_astqbl_k;
  struct _acb *ktb$l_astqfl_e;
  struct _acb *ktb$l_astqbl_e;
  struct _acb *ktb$l_astqfl_s;
  struct _acb *ktb$l_astqbl_s;
  struct _acb *ktb$l_astqfl_u;
  struct _acb *ktb$l_astqbl_u;
  INT8 ktb$b_pcb_padding_2 [4];
  INT32 ktb$l_cpu_id;
  INT8 ktb$b_pcb_padding_3 [24];
  UINT32 ktb$l_astact;
  UINT32 ktb$l_state;
  UINT32 ktb$l_pri;
  UINT32 ktb$l_prib;
  UINT32 ktb$l_affinity_skip;
  UINT32 ktb$l_owner;
  union  {
    UINT32 ktb$l_sts;
    struct  {
      unsigned ktb$v_res : 1;
      unsigned ktb$v_delpen : 1;
      unsigned ktb$v_forcpen : 1;
      unsigned ktb$v_inquan : 1;
      unsigned ktb$v_pswapm : 1;
      unsigned ktb$v_respen : 1;
      unsigned ktb$v_ssfexc : 1;
      unsigned ktb$v_ssfexce : 1;
      unsigned ktb$v_ssfexcs : 1;
      unsigned ktb$v_ssfexcu : 1;
      unsigned ktb$v_ssrwait : 1;
      unsigned ktb$v_suspen : 1;
      unsigned ktb$v_wakepen : 1;
      unsigned ktb$v_wall : 1;
      unsigned ktb$v_batch : 1;
      unsigned ktb$v_noacnt : 1;
      unsigned ktb$v_nosuspend : 1;
      unsigned ktb$v_astpen : 1;
      unsigned ktb$v_phdres : 1;
      unsigned ktb$v_hiber : 1;
      unsigned ktb$v_login : 1;
      unsigned ktb$v_netwrk : 1;
      unsigned ktb$v_pwrast : 1;
      unsigned ktb$v_nodelet : 1;
      unsigned ktb$v_disaws : 1;
      unsigned ktb$v_inter : 1;
      unsigned ktb$v_recover : 1;
      unsigned ktb$v_secaudit : 1;
      unsigned ktb$v_hardaff : 1;
      unsigned ktb$v_erdact : 1;
      unsigned ktb$v_softsusp : 1;
      unsigned ktb$v_preempted : 1;
    };
  };
  union  {
    UINT32 ktb$l_sts2;
    struct  {
      unsigned ktb$v_quantum_resched : 1;
      unsigned ktb$v_fill_2 : 2;
      unsigned ktb$v_phdlock : 1;
      unsigned ktb$v_tcb : 1;
      unsigned ktb$v_tbs_state_pending : 1;
      unsigned ktb$v_fill_3 : 9;
      unsigned ktb$v_class_sched_perm : 1;
      unsigned ktb$v_term_notify : 1;
      unsigned ktb$v_bytlm_loan : 1;
      unsigned ktb$v_fill_4 : 1;
      unsigned ktb$v_nounshelve : 1;
      unsigned ktb$v_shelving_reserved : 1;
      unsigned ktb$v_class_scheduled : 1;
      unsigned ktb$v_class_supplied : 1;


      unsigned ktb$v_in_tbs_state : 1;
      unsigned ktb$v_windfall : 1;
      unsigned ktb$v_notify : 1;
      unsigned ktb$v_single_threaded : 4;
      unsigned ktb$v_fill_36_ : 2;
    };
  };
  UINT32 ktb$l_prisav;
  UINT32 ktb$l_pribsav;
  UINT32 ktb$l_authpri;
  UINT32 ktb$l_onqtime;

  UINT32 ktb$l_waitime;
  INT8 ktb$b_pcb_padding_4 [32];
  UINT32 ktb$l_wefc;
  UINT32 ktb$l_efwm;
  INT8 ktb$b_pcb_padding_5 [16];
  UINT32 ktb$l_pid;

  union  {
    UINT32 ktb$l_epid;
    struct  {
      unsigned ktb$v_epid_proc : 21;
      unsigned ktb$v_epid_node_idx : 8;
      unsigned ktb$v_epid_node_seq : 2;
      unsigned ktb$v_epid_wild : 1;
    };
  };

  INT8 ktb$b_pcb_padding_6 [8];
  UINT32 ktb$l_mtxcnt;
  INT8 ktb$b_pcb_padding_7 [16];
  struct _phd *ktb$l_phd;
  struct _jib *ktb$l_jib;
  INT8 ktb$b_pcb_padding_8 [132];
  UINT32 ktb$l_home_rad;
  UINT32 ktb$l_sra_skip_count;
  INT8 ktb$b_pcb_padding_81 [12];
  INT32 ktb$l_affinity;
  UINT32 ktb$l_capability;
  INT8 ktb$b_pcb_padding_9 [32];
  UINT32 ktb$l_permanent_capability;
  INT32 ktb$l_permanent_cpu_affinity;
  INT8 ktb$b_pcb_padding_10 [8];
  UINT32 ktb$l_current_affinity;
  INT32 ktb$l_capability_seq;
  INT8 ktb$b_pcb_padding_11 [8];
  UINT32 ktb$l_ast_blocked;
  void *ktb$l_class_quant;
  UINT16 ktb$w_class_extra_ticks;
  INT8 ktb$b_pcb_padding_12 [94];
  UINT32 ktb$l_sched_policy;
  INT8 ktb$b_pcb_padding_13 [160];
  union  {
    struct _pcb *ktb$l_pcb;
    struct _ktb *ktb$l_initial_ktb;
  };
  union  {
    UINT32 ktb$l_flags;
    struct  {
      unsigned ktb$v_delete_pending : 1;
      unsigned ktb$v_sched_context_saved : 1;
      unsigned ktb$v_single_thread_act : 4;
      unsigned ktb$v_tolerant : 1;
      unsigned ktb$v_soft_rad_affinity : 1;
    };
  };
  union  {
    struct _fred *ktb$l_fred;
    struct _fred *ktb$l_virpcb;
  };
  void *ktb$l_per_kt_area;
  struct _acb *ktb$l_tquant_acb;
  UINT32 ktb$l_tquant;
  UINT32 ktb$l_quant;
  void *ktb$l_tm_callbacks;
  UINT32 ktb$l_callback_err;
  union  {
    UINT64 ktb$q_capabilities;
    struct  {
      UINT32 ktb$l_capabilities;
      UINT32 ktb$l_affinities;
    };
  };
  union  {
    UINT64 ktb$q_permanent_capabilities;
    struct  {
      UINT32 ktb$l_permanent_capabilities;
      UINT32 ktb$l_permanent_affinities;
    };
  };
  union  {
    UINT64 ktb$q_saved_capabilities;
    struct  {
      UINT32 ktb$l_saved_capabilities;
      UINT32 ktb$l_saved_affinities;
    };
  };
  UINT32 ktb$l_bias_cell;
  UINT32 ktb$l_persona_id;
  struct _psb *ktb$ar_psb;
  UINT32 ktb$l_swp_seq;
  INT64 ktb$q_vol_waits;
  INT32 ktb$l_curr_vol_waits;
  INT32 ktb$l_qend_count;
  UINT64 ktb$q_comq_wait;

  UINT64 ktb$q_runtime_start;
  UINT64 ktb$q_inttime_start;

  INT32 ktb$l_soft_broken;
  INT8 ktb$b_pcb_padding_14 [20];
  UINT64 ktb$q_acc_run;
  UINT64 ktb$q_acc_wait;
  UINT64 ktb$q_acc_interrupt;
  INT32 ktb$l_run_count;
  UINT32 ktb$l_glock_wait_status;
  UINT64 ktb$q_glock;
  INT8 ktb$b_pcb_padding_15 [16];
  UINT32 ktb$l_lckrq;
  INT8 ktb$b_fill_37_ [4];
};
*/

#endif

