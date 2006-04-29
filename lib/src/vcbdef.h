#ifndef vcbdef_h
#define vcbdef_h
 
#define		VCB$K_MRKLEN		11
#define		VCB$C_MRKLEN		11
#define		VCB$M_WRITE_IF		0x1
#define		VCB$M_WRITE_SM		0x2
#define		VCB$M_HOMBLKBAD		0x4
#define		VCB$M_IDXHDRBAD		0x8
#define		VCB$M_NOALLOC		0x10
#define		VCB$M_EXTFID		0x20
#define		VCB$M_GROUP		0x40
#define		VCB$M_SYSTEM		0x80
#define		VCB$M_HIGH_SIERRA	0x1
#define		VCB$M_NOSWITCH		0x2
#define		VCB$M_DSI		0x4
#define		VCB$M_XAR		0x8
#define		VCB$M_UNUSED_1		0x10
#define		VCB$M_UNUSED_2		0x20
#define		VCB$M_PARTFILE		0x1
#define		VCB$M_LOGICEOVS		0x2
#define		VCB$M_WAIMOUVOL		0x4
#define		VCB$M_WAIREWIND		0x8
#define		VCB$M_WAIUSRLBL		0x10
#define		VCB$M_CANCELIO		0x20
#define		VCB$M_MUSTCLOSE		0x40
#define		VCB$M_NOWRITE		0x80
#define		VCB$M_SHADMAST		0x1
#define		VCB$M_FAILED		0x2
#define		VCB$M_REBLDNG		0x8
#define		VCB$M_BLKASTREC		0x10
#define		VCB$M_MVBEGUN		0x20
#define		VCB$M_ADDING		0x40
#define		VCB$M_PACKACKED		0x80
#define		VCB$K_COMLEN		40
#define		VCB$C_COMLEN		40
#define		VCB$S_VCBDEF_COMMON	40
#define		VCB$M_SUBSYSTEM		0x1
#define		VCB$M_STRUC_ODS5	0x2
#define		VCB$K_LENGTH		304
#define		VCB$C_LENGTH		304
#define		VCB$S_VCBDEF_DISKS	304
#define		VCB$M_FILE_ATTRIBUTES	0xF
#define		VCB$M_FILE_CONTENTS	0xF0
#define		VCB$C_DEFAULT		0
#define		VCB$C_WRITETHROUGH	1
#define		VCB$C_WRITEBEHIND	2
#define		VCB$M_FLUSH_ON_CLOSE	0xF00
#define		VCB$C_FLUSH		1
#define		VCB$C_NOFLUSH		2
#define		VCB$M_CACHING_OPTIONS_MBZ	0xFFFFF000
#define		VCB$K_F64_LEN		304
#define		VCB$C_F64_LEN		304
#define		VCB$S_VCBDEF_F64	304
#define		VCB$K_SHAD_LEN		76
#define		VCB$S_VCBDEF_SHADOW	76
#define		VCB$S_VCBDEF_CDROM	128
#define		VCB$S_VCBDEF2		184
#define		VCB$S_VCBDEF3		32
	
struct _vcb {
  union  {
    struct _fcb *vcb$l_fcbfl;
    void *vcb$l_blockfl;
    struct _ucb *vcb$l_memqfl;
  };
  union  {
    struct _fcb *vcb$l_fcbbl;
    void *vcb$l_blockbl;
    struct _ucb *vcb$l_memqbl;
  };
  unsigned short int vcb$w_size;
  unsigned char vcb$b_type;
  union  {
    struct  {
      union {
	unsigned char vcb$b_status;
	struct  {
	  unsigned vcb$v_write_if : 1;
	  unsigned vcb$v_write_sm : 1;
	  unsigned vcb$v_homblkbad : 1;
	  unsigned vcb$v_idxhdrbad : 1;
	  unsigned vcb$v_noalloc : 1;
	  unsigned vcb$v_extfid : 1;
	  unsigned vcb$v_group : 1;
	  unsigned vcb$v_system : 1;
	};
	struct  {
	  unsigned vcb$v_high_sierra : 1;
	  unsigned vcb$v_noswitch : 1;
	  unsigned vcb$v_dsi : 1;
	  unsigned vcb$v_xar : 1;
	  unsigned vcb$v_unused_1 : 1;
	  unsigned vcb$v_unused_2 : 1;
	  unsigned vcb$v_fill_2_ : 2;
	};
	struct  {
	  unsigned vcb$v_partfile : 1;
	  unsigned vcb$v_logiceovs : 1;
	  unsigned vcb$v_waimouvol : 1;
	  unsigned vcb$v_wairewind : 1;
	  unsigned vcb$v_waiusrlbl : 1;
	  unsigned vcb$v_cancelio : 1;
	  unsigned vcb$v_mustclose : 1;
	  unsigned vcb$v_nowrite : 1;
	};
	struct  {
	  unsigned vcb$v_shadmast : 1;
	  unsigned vcb$v_failed : 1;
	  unsigned vcb$v_unused_bit : 1;
	  unsigned vcb$v_rebldng : 1;
	  unsigned vcb$v_blkastrec : 1;
	  unsigned vcb$v_mvbegun : 1;
	  unsigned vcb$v_adding : 1;
	  unsigned vcb$v_packacked : 1;
	};
      };
      unsigned int vcb$l_trans;
      unsigned int vcb$l_rvn;
      struct _aqb *vcb$l_aqb;
      char vcb$t_volname [12];
      void *vcb$l_rvt;
      union  {
	struct  {
	  unsigned int vcb$l_homelbn;
	  unsigned int vcb$l_home2lbn;
	  unsigned int vcb$l_ixhdr2lbn;
	  unsigned int vcb$l_ibmaplbn;
	  unsigned int vcb$l_sbmaplbn;
	  unsigned int vcb$l_ibmapsize;
	  unsigned int vcb$l_ibmapvbn;
	  unsigned int vcb$l_sbmapsize;
	  unsigned int vcb$l_sbmapvbn;
	  unsigned int vcb$l_cluster;
	  unsigned int vcb$l_extend;
	  unsigned int vcb$l_free;
	  unsigned int vcb$l_maxfiles;
	  unsigned int vcb$l_window;
	  unsigned int vcb$l_lru_lim;
	  unsigned int vcb$l_fileprot;
	  unsigned int vcb$l_mcount;
	  unsigned int vcb$l_eofdelta;
	  unsigned int vcb$l_resfiles;
	  unsigned int vcb$l_recordsz;
	  unsigned int vcb$l_blockfact;
	  union  {
	    unsigned char vcb$b_status2;
	    struct  {
	      unsigned vcb$v_writethru : 1;
	      unsigned vcb$v_nocache : 1;
	      unsigned vcb$v_mountver : 1;
	      unsigned vcb$v_erase : 1;
	      unsigned vcb$v_nohighwater : 1;
	      unsigned vcb$v_noshare : 1;
	      unsigned vcb$v_cluslock : 1;
	      unsigned vcb$v_subset0 : 1;
	    };
	    char vcb$b_fill_6 [4];
	  };
	  struct _fcb *vcb$l_quotafcb;
	  void *vcb$l_cache;
	  void *vcb$l_quocache;
	  unsigned int vcb$l_quosize;
	  unsigned int vcb$l_penderr;
	  unsigned int vcb$l_serialnum;
	  union  {
	    unsigned int vcb$l_status3;
	    struct  {
	      unsigned vcb$v_subsystem : 1;
	      unsigned vcb$v_struc_ods5 : 1;
	      unsigned vcb$v_fill_3_ : 6;
	    };
	  };
	  unsigned int vcb$l_vollkid;
	  char vcb$t_volcknam [12];
	  unsigned int vcb$l_blockid;
	  unsigned long long vcb$q_retainmin;
	  unsigned long long vcb$q_retainmax;
	  unsigned long long vcb$q_mounttime;
	  struct _vcb *vcb$l_memhdfl;
	  struct _vcb *vcb$l_memhdbl;
	  char vcb$b_fill_7 [3];
	  unsigned char vcb$b_shad_sts;
	  unsigned int vcb$l_activity;
	  unsigned int vcb$l_spl_cnt;
	  unsigned int vcb$l_shad_lkid;
	  unsigned char vcb$b_acb [36];
	  struct  {
	    union  {
	      char vcb$b_fill_2 [20];
	      struct _orb *vcb$l_orb;
	    };
	    struct  {
	      char vcb$b_fill_3 [20];
	    };
	  };
	  union  {
	    unsigned int vcb$l_caching_options;
	    struct  {
	      unsigned vcb$v_file_attributes : 4;
	      unsigned vcb$v_file_contents : 4;
	      unsigned vcb$v_flush_on_close : 4;
	      unsigned vcb$v_caching_options_mbz : 20;
	    };
	  };
	} vcb_1_2_1_1;
	struct  {
	  struct _ucb *vcb$l_mem_ucb;
	  struct _ucb *vcb$l_mast_ucb;
	  struct _vcb *vcb$l_mast_vcb;
	  unsigned short int vcb$w_copy_type;
	  unsigned short int vcb$w_cpyseqnum;
	  int vcb$l_fill_1;
	  unsigned long long vcb$q_work;
	  unsigned long long vcb$q_shdm_resv;
	};
	struct  {
	  unsigned int vcb$l_voldesc;
	  struct _vcb *vcb$l_orphaned_vcb;
	  void *vcb$l_ptvector;
	  void *vcb$l_lbncache;
	  unsigned int vcb$l_ptindex;
	  unsigned int vcb$l_lbsize;
	  unsigned int vcb$l_mxdirnm;
	  unsigned int vcb$l_lbblocks;
	  unsigned int vcb$l_minread;
	  unsigned int vcb$l_rdbytes;
	  unsigned int vcb$l_lastgrp;
	  unsigned int vcb$l_cd_free;
	  unsigned int vcb$l_cd_maxfiles;
	  unsigned int vcb$l_cd_window;
	  unsigned int vcb$l_cd_lru_lim;
	  unsigned int vcb$l_ptrvn;
	  unsigned int vcb$l_cd_mcount;
	  unsigned int vcb$l_fat_rfm;
	  unsigned int vcb$l_fat_rat;
	  unsigned int vcb$l_fat_mrs;
	  unsigned int vcb$l_sectors;
	  union  {
	    unsigned char vcb$b_cd_status2;
	    char vcb$b_fill_8 [4];
	  };
	};
	struct  {
	  union  {
	    unsigned int vcb$l_cur_fid;
	    struct  {
	      unsigned short int vcb$w_cur_num;
	      unsigned short int vcb$w_cur_seq;
	    };
	  };
	  union  {
	    unsigned int vcb$l_start_fid;
	    struct  {
	      unsigned short int vcb$w_start_num;
	      unsigned short int vcb$w_start_seq;
	    };
	  };
	  union  {
	    unsigned short int vcb$w_mode;
	    struct  {
	      unsigned vcb$v_ovrexp : 1;
	      unsigned vcb$v_ovracc : 1;
	      unsigned vcb$v_ovrlbl : 1;
	      unsigned vcb$v_ovrsetid : 1;
	      unsigned vcb$v_intchg : 1;
	      unsigned vcb$v_ebcdic : 1;
	      unsigned vcb$v_novol2 : 1;
	      unsigned vcb$v_nohdr3 : 1;
	      unsigned vcb$v_starfile : 1;
	      unsigned vcb$v_enusereot : 1;
	      unsigned vcb$v_blank : 1;
	      unsigned vcb$v_init : 1;
	      unsigned vcb$v_noauto : 1;
	      unsigned vcb$v_ovrvolo : 1;
	      unsigned vcb$v_fil_access : 1;
	      unsigned vcb$v_fill_4_ : 1;
	    };
	  };
	  unsigned char vcb$b_tm;
	  unsigned char vcb$b_cur_rvn;
	  unsigned int vcb$l_st_record;
	  struct _mvl *vcb$l_mvl;
	  struct _wcb *vcb$l_wcb;
	  void *vcb$l_vpfl;
	  void *vcb$l_vpbl;
	  struct _acb *vcb$l_usrlblast;
	  unsigned char vcb$b_lblcnt;
	  char vcb$t_fill_5 [99];
	  unsigned long long vcb$q_exp_date;
	};
      } vcb_1_2_1;
    } vcb_1_2;
    struct  {
      unsigned char vcb$b_qnamecnt;
      char vcb$t_qname [20];
    } vcb_1_1;
  } vcb_0;
};

#define		vcb$b_status		vcb_0.vcb_1_2.vcb$b_status
#define		vcb$l_trans		vcb_0.vcb_1_2.vcb$l_trans
#define		vcb$l_aqb		vcb_0.vcb_1_2.vcb$l_aqb
#define		vcb$t_volname		vcb_0.vcb_1_2.vcb$t_volname
#define		vcb$l_rvt		vcb_0.vcb_1_2.vcb$l_rvt
#define		vcb$l_cache		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_cache
#define		vcb$l_quocache		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_quocache
#define		vcb$l_cluster		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_cluster
#define		vcb$l_fileprot		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_fileprot
#define		vcb$l_free		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_free
#define		vcb$l_home2lbn		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_home2lbn
#define		vcb$l_homelbn		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_homelbn
#define		vcb$l_ibmaplbn		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_ibmaplbn
#define		vcb$l_ibmapsize		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_ibmapsize
#define		vcb$l_ibmapvbn		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_ibmapvbn
#define		vcb$l_ixhdr2lbn		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_ixhdr2lbn
#define		vcb$l_maxfiles		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_maxfiles
#define		vcb$l_sbmaplbn		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_sbmaplbn
#define		vcb$l_sbmapsize		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_sbmapsize
#define		vcb$l_sbmapvbn		vcb_0.vcb_1_2.vcb_1_2_1.vcb_1_2_1_1.vcb$l_sbmapvbn

#endif
