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
      union  {
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
	} vcb_0;
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
      };
    };
    struct  {
      unsigned char vcb$b_qnamecnt;
      char vcb$t_qname [20];
    };
  };
};

#define		vcb$l_fileprot		vcb_0.vcb$l_fileprot
#define		vcb$l_free		vcb_0.vcb$l_free
#define		vcb$l_home2lbn		vcb_0.vcb$l_home2lbn
#define		vcb$l_homelbn		vcb_0.vcb$l_homelbn
#define		vcb$l_ibmaplbn		vcb_0.vcb$l_ibmaplbn
#define		vcb$l_ibmapsize		vcb_0.vcb$l_ibmapsize
#define		vcb$l_ibmapvbn		vcb_0.vcb$l_ibmapvbn
#define		vcb$l_ixhdr2lbn		vcb_0.vcb$l_ixhdr2lbn

#if 0 
#define		vcb$b_acb		vcb$r_vcb_disks.vcb$b_acb
#define		vcb$b_cd_status2	vcb$r_vcb_cdrom.vcb$r_cd_status2_overlay.vcb$b_cd_status2
#define		vcb$b_cur_rvn		vcb$r_vcb_mtaacp.vcb$b_cur_rvn
#define		vcb$b_lblcnt		vcb$r_vcb_mtaacp.vcb$b_lblcnt
#define		vcb$b_qnamecnt		vcb$r_vcb_jacp.vcb$b_qnamecnt
#define		vcb$b_shad_sts		vcb$r_vcb_disks.vcb$b_shad_sts
#define		vcb$b_status		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$b_status
#define		vcb$b_status2		vcb$r_vcb_disks.vcb$r_status2_overlay.vcb$b_status2
#define		vcb$b_tm		vcb$r_vcb_mtaacp.vcb$b_tm
#define		vcb$l_activity		vcb$r_vcb_disks.vcb$l_activity
#define		vcb$l_aqb		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$l_aqb
#define		vcb$l_blockbl		vcb$r_backward_link.vcb$l_blockbl
#define		vcb$l_blockfact		vcb$r_vcb_disks.vcb$l_blockfact
#define		vcb$l_blockfl		vcb$r_forward_link.vcb$l_blockfl
#define		vcb$l_blockid		vcb$r_vcb_disks.vcb$l_blockid
#define		vcb$l_cache		vcb$r_vcb_disks.vcb$l_cache
#define		vcb$l_caching_options	vcb$r_vcb_disks.vcb$r_caching_options_overlay.vcb$l_caching_options
#define		vcb$l_cd_free		vcb$r_vcb_cdrom.vcb$l_cd_free
#define		vcb$l_cd_lru_lim	vcb$r_vcb_cdrom.vcb$l_cd_lru_lim
#define		vcb$l_cd_maxfiles	vcb$r_vcb_cdrom.vcb$l_cd_maxfiles
#define		vcb$l_cd_mcount		vcb$r_vcb_cdrom.vcb$l_cd_mcount
#define		vcb$l_cd_window		vcb$r_vcb_cdrom.vcb$l_cd_window
#define		vcb$l_cluster		vcb$r_vcb_disks.vcb$l_cluster
#define		vcb$l_cur_fid		vcb$r_vcb_mtaacp.vcb$r_cur_fid_overlay.vcb$l_cur_fid
#define		vcb$l_eofdelta		vcb$r_vcb_disks.vcb$l_eofdelta
#define		vcb$l_extend		vcb$r_vcb_disks.vcb$l_extend
#define		vcb$l_fat_mrs		vcb$r_vcb_cdrom.vcb$l_fat_mrs
#define		vcb$l_fat_rat		vcb$r_vcb_cdrom.vcb$l_fat_rat
#define		vcb$l_fat_rfm		vcb$r_vcb_cdrom.vcb$l_fat_rfm
#define		vcb$l_fcbbl		vcb$r_backward_link.vcb$l_fcbbl
#define		vcb$l_fcbfl		vcb$r_forward_link.vcb$l_fcbfl
#define		vcb$l_fileprot		vcb$r_vcb_disks.vcb$l_fileprot
#define		vcb$l_free		vcb$r_vcb_disks.vcb$l_free
#define		vcb$l_home2lbn		vcb$r_vcb_disks.vcb$l_home2lbn
#define		vcb$l_homelbn		vcb$r_vcb_disks.vcb$l_homelbn
#define		vcb$l_ibmaplbn		vcb$r_vcb_disks.vcb$l_ibmaplbn
#define		vcb$l_ibmapsize		vcb$r_vcb_disks.vcb$l_ibmapsize
#define		vcb$l_ibmapvbn		vcb$r_vcb_disks.vcb$l_ibmapvbn
#define		vcb$l_ixhdr2lbn		vcb$r_vcb_disks.vcb$l_ixhdr2lbn
#define		vcb$l_lastgrp		vcb$r_vcb_cdrom.vcb$l_lastgrp
#define		vcb$l_lbblocks		vcb$r_vcb_cdrom.vcb$l_lbblocks
#define		vcb$l_lbncache		vcb$r_vcb_cdrom.vcb$l_lbncache
#define		vcb$l_lbsize		vcb$r_vcb_cdrom.vcb$l_lbsize
#define		vcb$l_lru_lim		vcb$r_vcb_disks.vcb$l_lru_lim
#define		vcb$l_mast_ucb		vcb$r_vcb_shadow.vcb$l_mast_ucb
#define		vcb$l_mast_vcb		vcb$r_vcb_shadow.vcb$l_mast_vcb
#define		vcb$l_maxfiles		vcb$r_vcb_disks.vcb$l_maxfiles
#define		vcb$l_mcount		vcb$r_vcb_disks.vcb$l_mcount
#define		vcb$l_mem_ucb		vcb$r_vcb_shadow.vcb$l_mem_ucb
#define		vcb$l_memhdbl		vcb$r_vcb_disks.vcb$l_memhdbl
#define		vcb$l_memhdfl		vcb$r_vcb_disks.vcb$l_memhdfl
#define		vcb$l_memqbl		vcb$r_backward_link.vcb$l_memqbl
#define		vcb$l_memqfl		vcb$r_forward_link.vcb$l_memqfl
#define		vcb$l_minread		vcb$r_vcb_cdrom.vcb$l_minread
#define		vcb$l_mvl		vcb$r_vcb_mtaacp.vcb$l_mvl
#define		vcb$l_mxdirnm		vcb$r_vcb_cdrom.vcb$l_mxdirnm
#define		vcb$l_orb		vcb$r_min_class.vcb$l_orb
#define		vcb$l_orphaned_vcb	vcb$r_vcb_cdrom.vcb$l_orphaned_vcb
#define		vcb$l_penderr		vcb$r_vcb_disks.vcb$l_penderr
#define		vcb$l_ptindex		vcb$r_vcb_cdrom.vcb$l_ptindex
#define		vcb$l_ptrvn		vcb$r_vcb_cdrom.vcb$l_ptrvn
#define		vcb$l_ptvector		vcb$r_vcb_cdrom.vcb$l_ptvector
#define		vcb$l_quocache		vcb$r_vcb_disks.vcb$l_quocache
#define		vcb$l_quosize		vcb$r_vcb_disks.vcb$l_quosize
#define		vcb$l_quotafcb		vcb$r_vcb_disks.vcb$l_quotafcb
#define		vcb$l_rdbytes		vcb$r_vcb_cdrom.vcb$l_rdbytes
#define		vcb$l_recordsz		vcb$r_vcb_disks.vcb$l_recordsz
#define		vcb$l_resfiles		vcb$r_vcb_disks.vcb$l_resfiles
#define		vcb$l_rvn		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$l_rvn
#define		vcb$l_rvt		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$l_rvt
#define		vcb$l_sbmaplbn		vcb$r_vcb_disks.vcb$l_sbmaplbn
#define		vcb$l_sbmapsize		vcb$r_vcb_disks.vcb$l_sbmapsize
#define		vcb$l_sbmapvbn		vcb$r_vcb_disks.vcb$l_sbmapvbn
#define		vcb$l_sectors		vcb$r_vcb_cdrom.vcb$l_sectors
#define		vcb$l_serialnum		vcb$r_vcb_disks.vcb$l_serialnum
#define		vcb$l_shad_lkid		vcb$r_vcb_disks.vcb$l_shad_lkid
#define		vcb$l_spl_cnt		vcb$r_vcb_disks.vcb$l_spl_cnt
#define		vcb$l_st_record		vcb$r_vcb_mtaacp.vcb$l_st_record
#define		vcb$l_start_fid		vcb$r_vcb_mtaacp.vcb$r_start_fid_overlay.vcb$l_start_fid
#define		vcb$l_status3		vcb$r_vcb_disks.vcb$r_fill_0_.vcb$l_status3
#define		vcb$l_trans		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$l_trans
#define		vcb$l_usrlblast		vcb$r_vcb_mtaacp.vcb$l_usrlblast
#define		vcb$l_voldesc		vcb$r_vcb_cdrom.vcb$l_voldesc
#define		vcb$l_vollkid		vcb$r_vcb_disks.vcb$l_vollkid
#define		vcb$l_vpbl		vcb$r_vcb_mtaacp.vcb$l_vpbl
#define		vcb$l_vpfl		vcb$r_vcb_mtaacp.vcb$l_vpfl
#define		vcb$l_wcb		vcb$r_vcb_mtaacp.vcb$l_wcb
#define		vcb$l_window		vcb$r_vcb_disks.vcb$l_window
#define		vcb$q_exp_date		vcb$r_vcb_mtaacp.vcb$q_exp_date
#define		vcb$q_mounttime		vcb$r_vcb_disks.vcb$q_mounttime
#define		vcb$q_retainmax		vcb$r_vcb_disks.vcb$q_retainmax
#define		vcb$q_retainmin		vcb$r_vcb_disks.vcb$q_retainmin
#define		vcb$q_shdm_resv		vcb$r_vcb_shadow.vcb$q_shdm_resv
#define		vcb$q_work		vcb$r_vcb_shadow.vcb$q_work
#define		vcb$r_max_class		vcb$r_vcb_disks.vcb$r_security_overlay.vcb$r_max_class
#define		vcb$r_min_class		vcb$r_vcb_disks.vcb$r_security_overlay.vcb$r_min_class
#define		vcb$r_vcb_cdrom		vcb$r_vcb_extensions.vcb$r_vcb_cdrom
#define		vcb$r_vcb_disks		vcb$r_vcb_extensions.vcb$r_vcb_disks
#define		vcb$r_vcb_extensions	vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_vcb_extensions
#define		vcb$r_vcb_jacp		vcb$r_vcb_union.vcb$r_vcb_jacp
#define		vcb$r_vcb_mtaacp	vcb$r_vcb_extensions.vcb$r_vcb_mtaacp
#define		vcb$r_vcb_shadow	vcb$r_vcb_extensions.vcb$r_vcb_shadow
#define		vcb$t_fill_5		vcb$r_vcb_mtaacp.vcb$t_fill_5
#define		vcb$t_qname		vcb$r_vcb_jacp.vcb$t_qname
#define		vcb$t_volcknam		vcb$r_vcb_disks.vcb$t_volcknam
#define		vcb$t_volname		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$t_volname
#define		vcb$v_adding		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_shadow_bits.vcb$v_adding
#define		vcb$v_blank		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_blank
#define		vcb$v_blkastrec		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_shadow_bits.vcb$v_blkastrec
#define		vcb$v_caching_options_mbz	vcb$r_vcb_disks.vcb$r_caching_options_overlay.vcb$r_caching_options_bits.vcb$v_caching_options_mbz
#define		vcb$v_cancelio		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_tape_bits.vcb$v_cancelio
#define		vcb$v_cluslock		vcb$r_vcb_disks.vcb$r_status2_overlay.vcb$r_status2_bits.vcb$v_cluslock
#define		vcb$v_dsi		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_cdrom_bits.vcb$v_dsi
#define		vcb$v_ebcdic		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_ebcdic
#define		vcb$v_enusereot		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_enusereot
#define		vcb$v_erase		vcb$r_vcb_disks.vcb$r_status2_overlay.vcb$r_status2_bits.vcb$v_erase
#define		vcb$v_extfid		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_disk_bits.vcb$v_extfid
#define		vcb$v_failed		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_shadow_bits.vcb$v_failed
#define		vcb$v_fil_access	vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_fil_access
#define		vcb$v_file_attributes	vcb$r_vcb_disks.vcb$r_caching_options_overlay.vcb$r_caching_options_bits.vcb$v_file_attributes
#define		vcb$v_file_contents	vcb$r_vcb_disks.vcb$r_caching_options_overlay.vcb$r_caching_options_bits.vcb$v_file_contents
#define		vcb$v_flush_on_close	vcb$r_vcb_disks.vcb$r_caching_options_overlay.vcb$r_caching_options_bits.vcb$v_flush_on_close
#define		vcb$v_group		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_disk_bits.vcb$v_group
#define		vcb$v_high_sierra	vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_cdrom_bits.vcb$v_high_sierra
#define		vcb$v_homblkbad		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_disk_bits.vcb$v_homblkbad
#define		vcb$v_idxhdrbad		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_disk_bits.vcb$v_idxhdrbad
#define		vcb$v_init		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_init
#define		vcb$v_intchg		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_intchg
#define		vcb$v_logiceovs		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_tape_bits.vcb$v_logiceovs
#define		vcb$v_mountver		vcb$r_vcb_disks.vcb$r_status2_overlay.vcb$r_status2_bits.vcb$v_mountver
#define		vcb$v_mustclose		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_tape_bits.vcb$v_mustclose
#define		vcb$v_mvbegun		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_shadow_bits.vcb$v_mvbegun
#define		vcb$v_noalloc		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_disk_bits.vcb$v_noalloc
#define		vcb$v_noauto		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_noauto
#define		vcb$v_nocache		vcb$r_vcb_disks.vcb$r_status2_overlay.vcb$r_status2_bits.vcb$v_nocache
#define		vcb$v_nohdr3		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_nohdr3
#define		vcb$v_nohighwater	vcb$r_vcb_disks.vcb$r_status2_overlay.vcb$r_status2_bits.vcb$v_nohighwater
#define		vcb$v_noshare		vcb$r_vcb_disks.vcb$r_status2_overlay.vcb$r_status2_bits.vcb$v_noshare
#define		vcb$v_noswitch		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_cdrom_bits.vcb$v_noswitch
#define		vcb$v_novol2		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_novol2
#define		vcb$v_nowrite		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_tape_bits.vcb$v_nowrite
#define		vcb$v_ovracc		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_ovracc
#define		vcb$v_ovrexp		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_ovrexp
#define		vcb$v_ovrlbl		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_ovrlbl
#define		vcb$v_ovrsetid		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_ovrsetid
#define		vcb$v_ovrvolo		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_ovrvolo
#define		vcb$v_packacked		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_shadow_bits.vcb$v_packacked
#define		vcb$v_partfile		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_tape_bits.vcb$v_partfile
#define		vcb$v_rebldng		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_shadow_bits.vcb$v_rebldng
#define		vcb$v_shadmast		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_shadow_bits.vcb$v_shadmast
#define		vcb$v_starfile		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$r_mode_bits.vcb$v_starfile
#define		vcb$v_struc_ods5	vcb$r_vcb_disks.vcb$r_fill_0_.vcb$r_fill_1_.vcb$v_struc_ods5
#define		vcb$v_subset0		vcb$r_vcb_disks.vcb$r_status2_overlay.vcb$r_status2_bits.vcb$v_subset0
#define		vcb$v_subsystem		vcb$r_vcb_disks.vcb$r_fill_0_.vcb$r_fill_1_.vcb$v_subsystem
#define		vcb$v_system		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_disk_bits.vcb$v_system
#define		vcb$v_waimouvol		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_tape_bits.vcb$v_waimouvol
#define		vcb$v_wairewind		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_tape_bits.vcb$v_wairewind
#define		vcb$v_waiusrlbl		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_tape_bits.vcb$v_waiusrlbl
#define		vcb$v_write_if		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_disk_bits.vcb$v_write_if
#define		vcb$v_write_sm		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_disk_bits.vcb$v_write_sm
#define		vcb$v_writethru		vcb$r_vcb_disks.vcb$r_status2_overlay.vcb$r_status2_bits.vcb$v_writethru
#define		vcb$v_xar		vcb$r_vcb_union.vcb$r_vcb_union_member.vcb$r_volsts.vcb$r_cdrom_bits.vcb$v_xar
#define		vcb$w_copy_type		vcb$r_vcb_shadow.vcb$w_copy_type
#define		vcb$w_cpyseqnum		vcb$r_vcb_shadow.vcb$w_cpyseqnum
#define		vcb$w_cur_num		vcb$r_vcb_mtaacp.vcb$r_cur_fid_overlay.vcb$r_cur_fid_fields.vcb$w_cur_num
#define		vcb$w_cur_seq		vcb$r_vcb_mtaacp.vcb$r_cur_fid_overlay.vcb$r_cur_fid_fields.vcb$w_cur_seq
#define		vcb$w_mode		vcb$r_vcb_mtaacp.vcb$r_mode_overlay.vcb$w_mode
#define		vcb$w_start_num		vcb$r_vcb_mtaacp.vcb$r_start_fid_overlay.vcb$r_start_fid_fields.vcb$w_start_num
#define		vcb$w_start_seq		vcb$r_vcb_mtaacp.vcb$r_start_fid_overlay.vcb$r_start_fid_fields.vcb$w_start_seq
#endif
		
#endif
