#ifndef phddef_h
#define phddef_h

#define PHD$M_ASTEN 0xF
#define PHD$M_ASTSR 0xF0
#define PHD$M_ASTEN_KEN 0x1
#define PHD$M_ASTEN_EEN 0x2
#define PHD$M_ASTEN_SEN 0x4
#define PHD$M_ASTEN_UEN 0x8
#define PHD$M_ASTSR_KPD 0x10
#define PHD$M_ASTSR_EPD 0x20
#define PHD$M_ASTSR_SPD 0x40
#define PHD$M_ASTSR_UPD 0x80
#define PHD$M_FEN 0x1
#define PHD$M_PME 0x4000000000000000
#define PHD$M_DATFX 0x8000000000000000
#define PHD$C_HWPCBLEN 128
#define PHD$K_HWPCBLEN 128
#define PHD$C_FPR_COUNT 32
#define PHD$K_FPR_COUNT 32
#define PHD$M_SW_FEN 0x1
#define PHD$M_AST_PENDING 0x80000000
#define PHD$M_PFMFLG 0x1
#define PHD$M_DALCSTX 0x2
#define PHD$M_WSPEAKCHK 0x4
#define PHD$M_NOACCVIO 0x8
#define PHD$M_IWSPEAKCK 0x10
#define PHD$M_IMGDMP 0x20
#define PHD$M_NO_WS_CHNG 0x40
#define PHD$M_SPARE_8 0x80
#define PHD$M_LOCK_HEADER 0x100
#define PHD$M_FREWSLE_ACTIVE 0x200
#define PHD$K_LENGTH 1008
#define PHD$C_LENGTH 1008
	
struct _phd {
  unsigned long long phd$q_privmsk;
  unsigned short int phd$w_size;
  unsigned char phd$b_type;
  char phd$$_spare_1;
#ifdef __i386__
#define PHD_INT_SIZE 4 
  unsigned int phd$l_wslist;
  unsigned int phd$l_wslock;
  unsigned int phd$l_wsdyn;
  unsigned int phd$l_wsnext;
  unsigned int phd$l_wslast;
  unsigned int phd$l_wsextent;
  unsigned int phd$l_wsquota;
  unsigned int phd$l_dfwscnt;
  unsigned int phd$l_cpulim;
  unsigned int phd$l_pst_base_offset;
  unsigned int phd$l_pst_last;
  unsigned int phd$l_pst_free;
  unsigned int phd$l_iorefc;
  unsigned long long phd$q_next_region_id;
  int phd$$_spare_2;
  unsigned int phd$l_emptpg;
  unsigned int phd$l_dfpfc;
  unsigned int phd$l_pgtbpfc;
  unsigned int phd$l_astlm;
#else
  // these are relative offset, not use yet
#define PHD_INT_SIZE 8 
  unsigned long phd$l_wslist;
  unsigned long phd$l_wslock;
  unsigned long phd$l_wsdyn;
  unsigned long phd$l_wsnext;
  unsigned long phd$l_wslast;
  unsigned long phd$l_wsextent;
  unsigned long phd$l_wsquota;
  unsigned long phd$l_dfwscnt;
  unsigned long phd$l_cpulim;
  unsigned long phd$l_pst_base_offset;
  unsigned long phd$l_pst_last;
  unsigned long phd$l_pst_free;
  unsigned long phd$l_iorefc;
  unsigned long long phd$q_next_region_id;
  long phd$$_spare_2;
  unsigned long phd$l_emptpg;
  unsigned long phd$l_dfpfc;
  unsigned long phd$l_pgtbpfc;
  unsigned long phd$l_astlm;
#endif
  union  {
    int phd$l_pst_base_max;
    int phd$l_fredoff;
    int phd$l_im_semaphore;
  };
  unsigned int phd$l_wssize;
  unsigned int phd$l_diocnt;
  unsigned int phd$l_biocnt;
  unsigned int phd$l_phvindex;
  long long phd$$_spare_18;
  union  {
    unsigned long long phd$q_lefc;
    struct  {
      unsigned int phd$l_lefc_0;
      unsigned int phd$l_lefc_1;
    };
  };
  union  {
    unsigned long long phd$q_hwpcb;
    unsigned long long phd$q_ksp;
  };
  unsigned long long phd$q_esp;
  unsigned long long phd$q_ssp;
  unsigned long long phd$q_usp;
  unsigned long long phd$q_ptbr;
  unsigned long long phd$q_asn;
  union  {
    unsigned long long phd$q_astsr_asten;
    struct  {
      unsigned phd$v_asten : 4;
      unsigned phd$v_astsr : 4;
    };
    struct  {
      unsigned phd$v_asten_ken : 1;
      unsigned phd$v_asten_een : 1;
      unsigned phd$v_asten_sen : 1;
      unsigned phd$v_asten_uen : 1;
      unsigned phd$v_astsr_kpd : 1;
      unsigned phd$v_astsr_epd : 1;
      unsigned phd$v_astsr_spd : 1;
      unsigned phd$v_astsr_upd : 1;
    };
  };
  union  {
    unsigned long long phd$q_fen_datfx;
    struct  {
      unsigned phd$v_fen : 1;
      unsigned phd$v_fill_61_1 : 32;
      unsigned phd$v_fill_61_2 : 29;
      unsigned phd$v_pme : 1;
      unsigned phd$v_datfx : 1;
    };
  };
  unsigned long long phd$q_cc;
  unsigned long long phd$q_unq;
  long long phd$q_pal_rsvd [6];
  union  {
    long long phd$q_fpr [32];
    struct  {
      unsigned long long phd$q_f0;
      unsigned long long phd$q_f1;
      unsigned long long phd$q_f2;
      unsigned long long phd$q_f3;
      unsigned long long phd$q_f4;
      unsigned long long phd$q_f5;
      unsigned long long phd$q_f6;
      unsigned long long phd$q_f7;
      unsigned long long phd$q_f8;
      unsigned long long phd$q_f9;
      unsigned long long phd$q_f10;
      unsigned long long phd$q_f11;
      unsigned long long phd$q_f12;
      unsigned long long phd$q_f13;
      unsigned long long phd$q_f14;
      unsigned long long phd$q_f15;
      unsigned long long phd$q_f16;
      unsigned long long phd$q_f17;
      unsigned long long phd$q_f18;
      unsigned long long phd$q_f19;
      unsigned long long phd$q_f20;
      unsigned long long phd$q_f21;
      unsigned long long phd$q_f22;
      unsigned long long phd$q_f23;
      unsigned long long phd$q_f24;
      unsigned long long phd$q_f25;
      unsigned long long phd$q_f26;
      unsigned long long phd$q_f27;
      unsigned long long phd$q_f28;
      unsigned long long phd$q_f29;
      unsigned long long phd$q_f30;
      unsigned long long phd$q_fpcr;
    };
  };
  union  {
    unsigned int phd$l_flags2;
    struct  {
      unsigned phd$v_sw_fen : 1;
      unsigned phd$v_fill_flags2 : 30;
      unsigned phd$v_ast_pending : 1;
    };
  };
  unsigned int phd$l_extracpu;
  unsigned long long phd$q_asnseq;
  unsigned int phd$l_extdynws;
  unsigned int phd$l_pageflts;
  unsigned int phd$l_fow_flts;
  unsigned int phd$l_for_flts;
  unsigned int phd$l_foe_flts;
  unsigned int phd$l_cputim;
  unsigned int phd$l_cpumode;
  unsigned int phd$l_awsmode;
  unsigned int phd$$_spare_16;
  unsigned int phd$$_spare_17;
  unsigned int phd$l_ptcntlck;
  unsigned int phd$l_ptcntval;
  unsigned int phd$l_ptcntact;
  unsigned int phd$l_ptcntmax;
  unsigned int phd$$_spare_12;
  unsigned int phd$$_spare_14;
  unsigned char phd$$_spare_15 [4];
  int phd$$_spare_3;
  int phd$$_spare_4;
  int phd$l_wsfluid;
  unsigned int phd$l_wsauth;
  unsigned int phd$l_wsauthext;
  void *phd$l_reslsth;
  unsigned int phd$l_authpri;
  unsigned long long phd$q_authpriv;
  unsigned long long phd$q_imagpriv;
  unsigned int phd$l_imgcnt;
  unsigned int phd$l_pfltrate;
  unsigned int phd$l_pflref;
  unsigned int phd$l_timref;
  unsigned int phd$l_pgfltio;
  struct  {
    unsigned char phd$$$_fill_3 [20];
  };
  struct  {
    unsigned char phd$$$_fill_4 [20];
  };
  unsigned int phd$$_spare_11 [4];
  int phd$$_spare_10;
  union  {
    long long phd$q_pagefile_refs;
    struct  {
      int phd$l_pagefile_refs_lo;
      int phd$l_pagefile_refs_hi;
    };
  };
  int phd$$_spare_13;
  unsigned int phd$$_spare_9;

  union  {
    unsigned int phd$l_flags;
    struct  {
      unsigned phd$v_pfmflg : 1;
      unsigned phd$v_dalcstx : 1;
      unsigned phd$v_wspeakchk : 1;
      unsigned phd$v_noaccvio : 1;
      unsigned phd$v_iwspeakck : 1;
      unsigned phd$v_imgdmp : 1;
      unsigned phd$v_no_ws_chng : 1;
      unsigned phd$$_spare_8 : 1;
      unsigned phd$v_lock_header : 1;
      unsigned phd$v_frewsle_active : 1;
      unsigned phd$v_fill_0_ : 6;
    };
  };
  unsigned int phd$l_pscanctx_seqnum;
  unsigned long long phd$q_pscanctx_queue;
  int phd$l_l2pt_wslx;
  int phd$l_l3pt_wslx;
  int phd$l_l3pt_count;
  int phd$l_l2pt_count;
  int phd$l_bufobj_wslx;
  int phd$$_spare_5;
  int phd$$_spare_6;
  int phd$$_spare_7;
  void *phd$pq_pt_no_delete1;
  void *phd$pq_pt_no_delete2;
  unsigned long long phd$q_free_pte_count;
  union  {
    long long phd$q_p0_rde;
    struct  {
      struct _rde *phd$ps_p0_va_list_flink;
      struct _rde *phd$ps_p0_va_list_blink;
    };
  };
  unsigned int phd$$$_p0_rde_fields [2];
  unsigned int phd$l_p0_flags;
  unsigned int phd$l_p0_region_prot;
  unsigned long long phd$q_p0_region_id;
  void *phd$pq_p0_start_va;
  unsigned long long phd$q_p0_region_size;
  union  {
    void *phd$pq_p0_first_free_va;
    int phd$l_frep0va;
  };
  union  {
    long long phd$q_p1_rde;
    struct  {
      struct _rde *phd$ps_p1_va_list_flink;
      struct _rde *phd$ps_p1_va_list_blink;
    };
  };
  unsigned int phd$$$_p1_rde_fields [2];
  unsigned int phd$l_p1_flags;
  unsigned int phd$l_p1_region_prot;
  unsigned long long phd$q_p1_region_id;
  void *phd$pq_p1_start_va;
  unsigned long long phd$q_p1_region_size;
  union  {
    void *phd$pq_p1_first_free_va;
    int phd$l_frep1va;
  };
  union  {
    long long phd$q_p2_rde;
    struct  {
      struct _rde *phd$ps_p2_va_list_flink;
      struct _rde *phd$ps_p2_va_list_blink;
    };
  };
  unsigned int phd$$$_p2_rde_fields [2];
  unsigned int phd$l_p2_flags;
  unsigned int phd$l_p2_region_prot;
  unsigned long long phd$q_p2_region_id;
  void *phd$pq_p2_start_va;
  unsigned long long phd$q_p2_region_size;
  void *phd$pq_p2_first_free_va;
  unsigned long long phd$q_image_authpriv;
  unsigned long long phd$q_image_permpriv;
  struct _rights *phd$ar_image_authrights;
  struct _rights *phd$ar_image_rights;
  struct _rights *phd$ar_subsystem_authrights;
  struct _rights *phd$ar_subsystem_rights;
  long long phd$q_bak_array;
};
 
#define PHD$S_PHDDEF 1016
 
#endif
 
