// $Id$
// $Locker$

#ifndef pcbdef_h
#define pcbdef_h

#define	PCB$M_RES	0x1
#define	PCB$M_DELPEN	0x2
#define	PCB$M_FORCPEN	0x4
#define	PCB$M_INQUAN	0x8
#define	PCB$M_PSWAPM	0x10
#define	PCB$M_RESPEN	0x20
#define	PCB$M_SSFEXC	0x40
#define	PCB$M_SSFEXCE	0x80
#define	PCB$M_SSFEXCS	0x100
#define	PCB$M_SSFEXCU	0x200
#define	PCB$M_SSRWAIT	0x400
#define	PCB$M_SUSPEN	0x800
#define	PCB$M_WAKEPEN	0x1000
#define	PCB$M_WALL	0x2000
#define	PCB$M_BATCH	0x4000
#define	PCB$M_NOACNT	0x8000
#define	PCB$M_NOSUSPEND	0x10000
#define	PCB$M_ASTPEN	0x20000
#define	PCB$M_PHDRES	0x40000
#define	PCB$M_HIBER	0x80000
#define	PCB$M_LOGIN	0x100000
#define	PCB$M_NETWRK	0x200000
#define	PCB$M_PWRAST	0x400000
#define	PCB$M_NODELET	0x800000
#define	PCB$M_DISAWS	0x1000000
#define	PCB$M_INTER	0x2000000
#define	PCB$M_RECOVER	0x4000000
#define	PCB$M_SECAUDIT	0x8000000
#define	PCB$M_HARDAFF	0x10000000
#define	PCB$M_ERDACT	0x20000000
#define	PCB$M_SOFTSUSP	0x40000000
#define	PCB$M_PREEMPTED	0x80000000
#define	PCB$M_QUANTUM_RESCHED	        0x1
#define	PCB$M_DISABLE_PREEMPT_PKTA_LOCK	0x2
#define	PCB$M_FREDLOCK	                0x4
#define	PCB$M_PHDLOCK	                0x8
#define	PCB$M_TCB	                0x10
#define	PCB$M_TBS_STATE_PENDING	        0x20
#define	PCB$M_CLASS_SCHED_PERM	0x8000
#define	PCB$M_TERM_NOTIFY	0x10000
#define	PCB$M_BYTLM_LOAN	0x20000
#define	PCB$M_DISABLE_PREEMPT	0x40000
#define	PCB$M_NOUNSHELVE	0x80000
#define	PCB$M_SHELVING_RESERVED	0x100000
#define	PCB$M_CLASS_SCHEDULED	0x200000
#define	PCB$M_CLASS_SUPPLIED	0x400000
#define	PCB$M_IN_TBS_STATE	0x800000
#define	PCB$M_WINDFALL	        0x1000000
#define	PCB$M_NOTIFY	        0x2000000
#define	PCB$M_SINGLE_THREADED	0x3C000000
#define	PCB$M_RWAST	        0x40000000
#define	PCB$M_SOFT_SINGLE_THREAD	0x80000000
#define	PCB$M_EPID_WILD	                0x80000000
#define	PCB$M_FORK	        0x1
#define	PCB$K_SCHED_OTHER	0
#define	PCB$K_SCHED_FIFO	1
#define	PCB$K_SCHED_RR	        2
#define	PCB$K_SCHED_POLICY_CNT	3
#define	PCB$K_ALL_THREADS	-2147483648			
#define	PCB$K_MAX_KT_COUNT	256
#define	PCB$M_EVENT_NO_FLAG	0x1	
#define	PCB$K_LENGTH	952
#define	PCB$C_LENGTH	952
#define	PCB$S_PCBDEF	952
#define	KTB$M_RES	0x1
#define	KTB$M_DELPEN	0x2
#define	KTB$M_FORCPEN	0x4
#define	KTB$M_INQUAN	0x8
#define	KTB$M_PSWAPM	0x10
#define	KTB$M_RESPEN	0x20
#define	KTB$M_SSFEXC	0x40
#define	KTB$M_SSFEXCE	0x80
#define	KTB$M_SSFEXCS	0x100
#define	KTB$M_SSFEXCU	0x200
#define	KTB$M_SSRWAIT	0x400
#define	KTB$M_SUSPEN	0x800
#define	KTB$M_WAKEPEN	0x1000
#define	KTB$M_WALL	0x2000
#define	KTB$M_BATCH	0x4000
#define	KTB$M_NOACNT	0x8000
#define	KTB$M_NOSUSPEND	0x10000
#define	KTB$M_ASTPEN	0x20000
#define	KTB$M_PHDRES	0x40000
#define	KTB$M_HIBER	0x80000
#define	KTB$M_LOGIN	0x100000
#define	KTB$M_NETWRK	0x200000
#define	KTB$M_PWRAST	0x400000
#define	KTB$M_NODELET	0x800000
#define	KTB$M_DISAWS	0x1000000
#define	KTB$M_INTER	0x2000000
#define	KTB$M_RECOVER	0x4000000
#define	KTB$M_SECAUDIT	0x8000000
#define	KTB$M_HARDAFF	0x10000000
#define	KTB$M_ERDACT	0x20000000
#define	KTB$M_SOFTSUSP	0x40000000
#define	KTB$M_PREEMPTED	0x80000000
#define	KTB$M_QUANTUM_RESCHED	0x1
#define	KTB$M_PHDLOCK	        0x8
#define	KTB$M_TCB	        0x10
#define	KTB$M_TBS_STATE_PENDING	0x20
#define	KTB$M_CLASS_SCHED_PERM	0x8000
#define	KTB$M_TERM_NOTIFY	0x10000
#define	KTB$M_BYTLM_LOAN	0x20000
#define	KTB$M_NOUNSHELVE	0x80000
#define	KTB$M_SHELVING_RESERVED	0x100000
#define	KTB$M_CLASS_SCHEDULED	0x200000
#define	KTB$M_CLASS_SUPPLIED	0x400000
#define	KTB$M_IN_TBS_STATE	0x800000
#define	KTB$M_WINDFALL	        0x1000000
#define	KTB$M_NOTIFY	        0x2000000
#define	KTB$M_SINGLE_THREADED	0x3C000000
#define	KTB$M_EPID_WILD	        0x80000000
#define	KTB$K_SCHED_OTHER	0
#define	KTB$K_SCHED_FIFO	1
#define	KTB$K_SCHED_RR	        2
#define	KTB$K_SCHED_POLICY_CNT	3
#define	KTB$M_DELETE_PENDING	        0x1
#define	KTB$M_SCHED_CONTEXT_SAVED	0x2
#define	KTB$M_SINGLE_THREAD_ACT	        0x3C
#define	KTB$M_TOLERANT	                0x40
#define	KTB$M_SOFT_RAD_AFFINITY	        0x80
	
#define	KTB$K_LENGTH	952
#define	KTB$C_LENGTH	952
#define	KTB$S_KTBDEF	952 
 
/* do not use this */
struct pcb {
  struct pcb *pcb$l_sqfl;            
  struct pcb *pcb$l_sqbl;            
  unsigned short int pcb$w_size;      
  unsigned char pcb$b_type;           
  unsigned char pcb$b_fill_1;
  unsigned int pcb$l_ast_pending;     
  unsigned long pcb$q_phypcb;      
  union  {
    unsigned long pcb$q_lefc_swapped; 
    struct  {
      unsigned int pcb$l_lefc_0_swapped; 
      unsigned int pcb$l_lefc_1_swapped; 
    };
  };
  struct acb *pcb$l_astqfl_spk;      
  struct acb *pcb$l_astqbl_spk;      
  struct acb *pcb$l_astqfl_k;        
  struct acb *pcb$l_astqbl_k;        
  struct acb *pcb$l_astqfl_e;        
  struct acb *pcb$l_astqbl_e;        
  struct acb *pcb$l_astqfl_s;        
  struct acb *pcb$l_astqbl_s;        
  struct acb *pcb$l_astqfl_u;        
  struct acb *pcb$l_astqbl_u;        
  int pcb$l_prvcpu;                   
  int pcb$l_cpu_id;                   
  unsigned long pcb$q_prvasn;      
  unsigned long pcb$q_prvasnseq;   
  unsigned long pcb$q_oncpucnt;    
  unsigned int pcb$l_astact;          
  unsigned int pcb$l_state;           
  unsigned int pcb$l_pri;             
  unsigned int pcb$l_prib;            
  unsigned int pcb$l_affinity_skip;   
  unsigned int pcb$l_owner;           
  union  {
    unsigned int pcb$l_sts;         
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
    unsigned int pcb$l_sts2;        
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
  unsigned int pcb$l_prisav;          
  unsigned int pcb$l_pribsav;         
  unsigned int pcb$l_authpri;         
  unsigned int pcb$l_onqtime;         
  unsigned int pcb$l_waitime;         
  unsigned int pcb$l_astcnt;          
  unsigned int pcb$l_biocnt;          
  unsigned int pcb$l_biolm;           
  int pcb$l_diocnt;                   
  int pcb$l_diolm;                    
  unsigned int pcb$l_prccnt;          
  char pcb$t_terminal [8];            
  unsigned int pcb$l_wefc;            
  unsigned int pcb$l_efwm;            
  unsigned int pcb$l_efcs;            
  unsigned int pcb$l_efcu;            
  union  {
    struct  {
      int pcb$l_efc2p;            
      int pcb$l_efc3p;            
    };
    struct  {                     
      unsigned short int pcb$w_pgflchar; 
      unsigned char pcb$b_pgflindex; 
    };
  };
  unsigned int pcb$l_pid;             
  union  {
    unsigned int pcb$l_epid;        
    struct  {
      unsigned pcb$v_epid_proc : 21; 
      unsigned pcb$v_epid_node_idx : 8; 
      unsigned pcb$v_epid_node_seq : 2; 
      unsigned pcb$v_epid_wild : 1; 
    };
  };
  unsigned int pcb$l_eowner;          
  unsigned int pcb$l_aptcnt;          
  unsigned int pcb$l_mtxcnt;          
  unsigned int pcb$l_gpgcnt;          
  unsigned int pcb$l_ppgcnt;          
  void *pcb$l_wsswp;                  
  unsigned int pcb$l_swapsize;        
  struct phd *pcb$l_phd;             
  struct jib *pcb$l_jib;             
  struct  {
    unsigned long pcb$q_priv;    
    struct arb *pcb$l_arb;         
    char pcb$$$_arb_fill_1 [48];    
    union  {
      unsigned int pcb$l_uic;     
      struct  {
	unsigned short int pcb$w_mem; 
	unsigned short int pcb$w_grp; 
      };
    };
    char pcb$$$_arb_fill_2 [60];    
  };
  struct orb *pcb$l_orb;             
  unsigned int pcb$l_tmbu;            
  unsigned int pcb$l_home_rad;        
  char pcb$b_spare_1 [4];             
  int pcb$l_dlckpri;                  
  unsigned int pcb$l_defprot;         
  struct pmb *pcb$l_pmb;             
  int pcb$l_affinity;                 
  unsigned int pcb$l_capability;      
  unsigned int pcb$l_cputim;          
  char pcb$t_lname [16];              
  struct pdb *pcb$l_prcpdb;          
  unsigned int pcb$l_pixhist;         
  int pcb$l_affinity_callback;        
  unsigned int pcb$l_permanent_capability; 
  int pcb$l_permanent_cpu_affinity;   
  unsigned long pcb$q_cwpssrv_queue; 
  unsigned int pcb$l_current_affinity; 
  int pcb$l_capability_seq;           
  unsigned long pcb$q_bufobj_list; 
  unsigned int pcb$l_ast_blocked;     
  void *pcb$l_class_quant;            
  unsigned short int pcb$w_class_extra_ticks; 
  unsigned char pcb$b_pkta_lock;      
  unsigned char pcb$b_fill_2;
  void *pcb$a_current_tx;             
  void *pcb$a_current_cd;             
  void *pcb$a_current_vertex;         
  union  {
    unsigned long pcb$q_xscb_que; 
    struct  {
      struct xscb *pcb$a_xscb_flink;
      struct xscb *pcb$a_xscb_blink;
    };
  };
  union  {
    unsigned long pcb$q_rmcb_que; 
    struct  {
      struct rmcb *pcb$a_rmcb_flink;
      struct rmcb *pcb$a_rmcb_blink;
    };
  };
  union  {
    unsigned long pcb$q_cd_que;  
    struct  {
      struct cde *pcb$a_cd_flink;
      struct cde *pcb$a_cd_blink;
    };
  };
  unsigned int pcb$l_dpc;             
  unsigned int pcb$l_cputime_ref;     
  unsigned int pcb$l_acc_waitime;     
  int pcb$l_prcstr;                   
  void *pcb$l_xpcb;                   
  unsigned int pcb$l_psx_fork_status; 
  union  {
    unsigned int pcb$l_psx_flags;   
    struct  {
      unsigned pcb$v_fork : 1;    
      unsigned pcb$v_psx_flags_fill : 31;
    };
  };
  void (*pcb$l_psx_actrtn)(void);         
  unsigned long pcb$q_psx_actprm;  
  unsigned int pcb$l_kernel_counter;  
  unsigned int pcb$l_exec_counter;    
  unsigned int pcb$l_super_counter;   
  unsigned int pcb$l_user_counter;    
  unsigned int pcb$l_sched_policy;    
  int (*pcb$a_frewsle_callout)(void);     
  union  {
    unsigned int pcb$l_frewsle_param; 
    int pcb$l_pqb;                  
  };
  unsigned int pcb$l_bufobj_cnt;      
  unsigned int pcb$l_noaudit;         
  unsigned int pcb$l_source_epid;     
  union  {
    unsigned long pcb$q_rdpb_que; 
    struct  {
      struct rdpb *pcb$a_rdpb_flink;
      struct rdpb *pcb$a_rdpb_blink;
    };
  };
  unsigned long pcb$q_files_64;    
  long pcb$q_keep_in_ws;           
  long pcb$q_keep_in_ws2;          
  unsigned int pcb$l_active_cpus;     
  unsigned int pcb$l_tquantum;        
  unsigned int pcb$l_multithread;     
  unsigned int pcb$l_kt_count;        
  unsigned int pcb$l_kt_high;         
  void *pcb$l_ktbvec;                 
  struct acb *pcb$l_wake_acb;        
  unsigned int pcb$l_st_ack_count;    
  union  {
    unsigned int pcb$l_thread_events; 
    struct  {
      unsigned pcb$v_event_no_flag : 1; 
      unsigned pcb$v_fill_20_ : 7;
    };
  };
  struct acb *pcb$l_postef_acb;      
  union  {
    unsigned long pcb$q_postef;  
    struct  {
      unsigned int pcb$l_postef1; 
      unsigned int pcb$l_postef2; 
    };
  };
  unsigned int pcb$l_swp_seq;         
  unsigned int pcb$l_swp_kt;          
  struct acb *pcb$l_im_astqfl_spk;   
  struct acb *pcb$l_im_astqbl_spk;   
  struct acb *pcb$l_im_astqfl_k;     
  struct acb *pcb$l_im_astqbl_k;     
  struct acb *pcb$l_im_astqfl_e;     
  struct acb *pcb$l_im_astqbl_e;     
  void *pcb$ps_ccbsva;                
  unsigned int pcb$l_maxfix;          
  void *pcb$ps_fandle;                
  unsigned long pcb$q_st_kt_array; 
  struct psb *pcb$ar_natural_psb;    
  char pcb$b_spare_2 [4];             
  union  {
    struct ktb *pcb$l_initial_ktb; 
    struct pcb *pcb$l_pcb;         
  };
  char pcb$b_ktb_padding_1 [116];     
  void *pcb$l_ctx_waitq;              
#ifdef INITIAL_POINTER_SIZE			 
#pragma required_pointer_size long		 
  struct lkb *pcb$q_lockqfl;         
#else
  unsigned long pcb$q_lockqfl;
#endif
#ifdef INITIAL_POINTER_SIZE			 
#pragma required_pointer_size long		 
  struct lkb *pcb$q_lockqbl;         
#else
  unsigned long pcb$q_lockqbl;
#endif
  char pcb$b_ktb_padding_2 [40];      
#ifdef INITIAL_POINTER_SIZE			 
#pragma required_pointer_size short		 
#endif
  struct pcb *pcb$l_class_link;      
  void *pcb$l_session_id;             
  void *pcb$l_process_group;          
  unsigned int pcb$l_creator;         
  unsigned int pcb$l_lckrq;           
  char pcb$b_fill_21_ [4];
};
 
struct ktb {
  struct ktb *ktb$l_sqfl;            
  struct ktb *ktb$l_sqbl;            
  unsigned short int ktb$w_size;      
  unsigned char ktb$b_type;           
  unsigned char ktb$b_fill_1;
  unsigned int ktb$l_ast_pending;     
  unsigned long ktb$q_phypcb;      
  char ktb$b_pcb_padding_1 [8];       
  struct acb *ktb$l_astqfl_spk;      
  struct acb *ktb$l_astqbl_spk;      
  struct acb *ktb$l_astqfl_k;        
  struct acb *ktb$l_astqbl_k;        
  struct acb *ktb$l_astqfl_e;        
  struct acb *ktb$l_astqbl_e;        
  struct acb *ktb$l_astqfl_s;        
  struct acb *ktb$l_astqbl_s;        
  struct acb *ktb$l_astqfl_u;        
  struct acb *ktb$l_astqbl_u;        
  char ktb$b_pcb_padding_2 [4];       
  int ktb$l_cpu_id;                   
  char ktb$b_pcb_padding_3 [24];      
  unsigned int ktb$l_astact;          
  unsigned int ktb$l_state;           
  unsigned int ktb$l_pri;             
  unsigned int ktb$l_prib;            
  unsigned int ktb$l_affinity_skip;   
  unsigned int ktb$l_owner;           
  union  {
    unsigned int ktb$l_sts;         
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
    unsigned int ktb$l_sts2;        
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
  unsigned int ktb$l_prisav;          
  unsigned int ktb$l_pribsav;         
  unsigned int ktb$l_authpri;         
  unsigned int ktb$l_onqtime;         
  unsigned int ktb$l_waitime;         
  char ktb$b_pcb_padding_4 [32];      
  unsigned int ktb$l_wefc;            
  unsigned int ktb$l_efwm;            
  char ktb$b_pcb_padding_5 [16];      
  unsigned int ktb$l_pid;             
  union  {
    unsigned int ktb$l_epid;        
    struct  {
      unsigned ktb$v_epid_proc : 21; 
      unsigned ktb$v_epid_node_idx : 8; 
      unsigned ktb$v_epid_node_seq : 2; 
      unsigned ktb$v_epid_wild : 1; 
    };
  };
  char ktb$b_pcb_padding_6 [8];       
  unsigned int ktb$l_mtxcnt;          
  char ktb$b_pcb_padding_7 [16];      
  struct phd *ktb$l_phd;             
  struct jib *ktb$l_jib;             
  char ktb$b_pcb_padding_8 [132];     
  unsigned int ktb$l_home_rad;        
  unsigned int ktb$l_sra_skip_count;  
  char ktb$b_pcb_padding_81 [12];     
  int ktb$l_affinity;                 
  unsigned int ktb$l_capability;      
  char ktb$b_pcb_padding_9 [32];      
  unsigned int ktb$l_permanent_capability; 
  int ktb$l_permanent_cpu_affinity;   
  char ktb$b_pcb_padding_10 [8];      
  unsigned int ktb$l_current_affinity; 
  int ktb$l_capability_seq;           
  char ktb$b_pcb_padding_11 [8];      
  unsigned int ktb$l_ast_blocked;     
  void *ktb$l_class_quant;            
  unsigned short int ktb$w_class_extra_ticks; 
  char ktb$b_pcb_padding_12 [94];     
  unsigned int ktb$l_sched_policy;    
  char ktb$b_pcb_padding_13 [160];    
  union  {
    struct pcb *ktb$l_pcb;         
    struct ktb *ktb$l_initial_ktb; 
  };
  union  {
    unsigned int ktb$l_flags;       
    struct  {
      unsigned ktb$v_delete_pending : 1; 
      unsigned ktb$v_sched_context_saved : 1; 
      unsigned ktb$v_single_thread_act : 4; 
      unsigned ktb$v_tolerant : 1; 
      unsigned ktb$v_soft_rad_affinity : 1; 
    };
  };
  union  {
    struct fred *ktb$l_fred;       
    struct fred *ktb$l_virpcb;     
  };
  void *ktb$l_per_kt_area;            
  struct acb *ktb$l_tquant_acb;      
  unsigned int ktb$l_tquant;          
  unsigned int ktb$l_quant;           
  void *ktb$l_tm_callbacks;           
  unsigned int ktb$l_callback_err;    
  union  {
    unsigned long ktb$q_capabilities; 
    struct  {
      unsigned int ktb$l_capabilities; 
      unsigned int ktb$l_affinities; 
    };
  };
  union  {
    unsigned long ktb$q_permanent_capabilities; 
    struct  {
      unsigned int ktb$l_permanent_capabilities; 
      unsigned int ktb$l_permanent_affinities; 
    };
  };
  union  {
    unsigned long ktb$q_saved_capabilities; 
    struct  {
      unsigned int ktb$l_saved_capabilities; 
      unsigned int ktb$l_saved_affinities; 
    };
  };
  unsigned int ktb$l_bias_cell;       
  unsigned int ktb$l_persona_id;      
  struct psb *ktb$ar_psb;            
  unsigned int ktb$l_swp_seq;         
  long ktb$q_vol_waits;            
  int ktb$l_curr_vol_waits;           
  int ktb$l_qend_count;               
  unsigned long ktb$q_comq_wait;   
  unsigned long ktb$q_runtime_start; 
  unsigned long ktb$q_inttime_start; 
  int ktb$l_soft_broken;              
  char ktb$b_pcb_padding_14 [20];     
  unsigned long ktb$q_acc_run;     
  unsigned long ktb$q_acc_wait;    
  unsigned long ktb$q_acc_interrupt; 
  int ktb$l_run_count;                
  unsigned int ktb$l_glock_wait_status; 
  unsigned long ktb$q_glock;       
  char ktb$b_pcb_padding_15 [16];     
  unsigned int ktb$l_lckrq;           
  char ktb$b_fill_37_ [4];
};
 
#endif 
