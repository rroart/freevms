#ifndef fcbdef_h
#define fcbdef_h

#define FCB$M_FILE_ATTRIBUTES 0xF
#define FCB$M_FILE_CONTENTS 0xF0
#define FCB$M_FLUSH_ON_CLOSE 0xF00
#define FCB$M_CACHING_OPTIONS_MBZ 0xFFFFF000
#define FCB$C_DEFAULT 0
#define FCB$C_WRITETHROUGH 1
#define FCB$C_WRITEBEHIND 2
#define FCB$C_NOCACHING 3
#define FCB$C_FLUSH 1
#define FCB$C_NOFLUSH 2
#define FCB$M_VCC_STATE 0x7
#define FCB$K_LENGTH 320
#define FCB$C_LENGTH 320
#define FCB$S_FCBDEF 320
	
struct _fcb {
  struct _fcb *fcb$l_fcbfl;
  struct _fcb *fcb$l_fcbbl;
  unsigned short int fcb$w_size;
  unsigned char fcb$b_type;
  unsigned char fcb$b_acclkmode;
  void *fcb$l_exfcb;
  struct _wcb *fcb$l_wlfl;
  struct _wcb *fcb$l_wlbl;
  unsigned int fcb$l_refcnt;
  unsigned int fcb$l_acnt;
  unsigned int fcb$l_wcnt;
  unsigned int fcb$l_lcnt;
  unsigned int fcb$l_tcnt;
  union  {
    unsigned int fcb$l_status;
    struct  {
      unsigned fcb$v_dir : 1;
      unsigned fcb$v_markdel : 1;
      unsigned fcb$v_badblk : 1;
      unsigned fcb$v_excl : 1;
      unsigned fcb$v_spool : 1;
      unsigned fcb$v_rmslock : 1;
      unsigned fcb$v_erase : 1;
      unsigned fcb$v_badacl : 1;
      unsigned fcb$v_stale : 1;
      unsigned fcb$v_delaytrnc : 1;
      unsigned fcb$v_limbo : 1;
      unsigned fcb$v_isdir : 1;
      unsigned fcb$v_nomove : 1;
      unsigned fcb$v_shelved : 1;
      unsigned fcb$v_noshelvable : 1;
      unsigned fcb$v_preshelved : 1;
    } fcb$r_status_bits;
  } fcb$r_status_overlay;
  union  {
    unsigned short int fcb$w_fid [3];
    struct  {
      unsigned short int fcb$w_fid_num;
      unsigned short int fcb$w_fid_seq;
      union  {
	unsigned short int fcb$w_fid_rvn;
	struct  {
	  unsigned char fcb$b_fid_rvn;
	  unsigned char fcb$b_fid_nmx;
	} fcb$r_fid_rvn_fields;
      } fcb$r_fid_rvn_overlay;
    } fcb$r_fid_fields;
    struct  {
      unsigned short int fcb$w_fid_dirnum;
      unsigned int fcb$l_fid_recnum;
    } fcb$r_fid_iso_9660_fields;
  } fcb$r_fid_overlay;
  unsigned short int fcb$w_segn;
  unsigned int fcb$l_stvbn;
  unsigned int fcb$l_stlbn;
  unsigned int fcb$l_hdlbn;
  unsigned int fcb$l_filesize;
  unsigned int fcb$l_efblk;
  unsigned int fcb$l_versions;
  unsigned int fcb$l_dirseq;
  unsigned int fcb$l_dirindx;
  unsigned int fcb$l_acclkid;
  unsigned int fcb$l_lockbasis;
  union  {
    unsigned int fcb$l_truncvbn;
    unsigned int fcb$l_numextents;
  } fcb$r_fcb1_overlay;
  unsigned int fcb$l_cachelkid;
  unsigned int fcb$l_highwater;
  unsigned int fcb$l_newhighwater;
  unsigned int fcb$l_hwm_update;
  unsigned int fcb$l_hwm_erase;
  unsigned int fcb$l_hwm_partial;
  unsigned int fcb$l_revision;
  union  {
    struct  {
      void *fcb$l_hwm_waitfl;
      void *fcb$l_hwm_waitbl;
    } fcb$q_hwmqhd;
    struct  {
      void *fcb$l_limbofl;
      void *fcb$l_limbobl;
    } fcb$q_limboqhd;
  } fcb$r_queue_overlay;
  struct  {
    union  {
      unsigned int fcb$l_fileowner;
      struct  {
	unsigned short int fcb$w_uicmember;
	unsigned short int fcb$w_uicgroup;
      } fcb$r_fill_1_;
    } fcb$r_fill_0_;
    unsigned int fcb$l_fill_5;
    unsigned int fcb$l_fill_3;
    unsigned int fcb$l_fill_6;
    unsigned long long fcb$q_acmode;
    union  {
      unsigned int fcb$l_sys_prot;
      struct  {
	unsigned short int fcb$w_fileprot;
	unsigned short int fcb$w_fill_4;
      } fcb$r_fill_3_;
    } fcb$r_fill_2_;
    unsigned int fcb$l_own_prot;
    unsigned int fcb$l_grp_prot;
    unsigned int fcb$l_wor_prot;
    void *fcb$l_aclfl;
    void *fcb$l_aclbl;
    struct  {
      char fcb$b_fill_1 [20];
    } fcb$r_min_class_prot;
    struct  {
      char fcb$b_fill_2 [20];
    } fcb$r_max_class_prot;
    unsigned short int fcb$w_name_length;
    short int fcb$w_fill_7;
    void *fcb$l_name_pointer;
    struct _ocb *fcb$l_ocb;
    struct _orb *fcb$l_template_orb;
    unsigned int fcb$l_object_specific;
    struct _orb *fcb$l_original_orb;
    unsigned short int fcb$w_updseq;
    unsigned short int fcb$w_fill_8;
    unsigned int fcb$l_reserve1;
    unsigned int fcb$l_reserve2;
  } fcb$r_orb;
  char fcb$b_fill_9 [4];
  union  {
    char fcb$b_fill_10 [40];
    struct  {
      char fcb$b_fill_11 [24];
      unsigned long long fcb$q_cfb;
      union  {
	unsigned int fcb$l_caching_options;
	struct  {
	  unsigned fcb$v_file_attributes : 4;
	  unsigned fcb$v_file_contents : 4;
	  unsigned fcb$v_flush_on_close : 4;
	  unsigned fcb$v_caching_options_mbz : 20;
	} fcb$r_caching_options_bits;
      } fcb$r_caching_options_overlay;
      union  {
	unsigned int fcb$l_status2;
	struct  {
	  unsigned fcb$v_vcc_state : 3;
	  unsigned fcb$v_fill_4_ : 5;
	} fcb$r_status2_bits;
      } fcb$r_status2_overlay;
    } fcb$r_extra_fields;
  } fcb$r_orb_extension_overlay;
  struct _orb *fcb$l_orb;
  void *fcb$l_cfcb;
  struct _fcb *fcb$l_primfcb;
  unsigned int fcb$l_dirlckid;
};
 
#define		fcb$b_fid_nmx		fcb$r_fid_overlay.fcb$r_fid_fields.fcb$r_fid_rvn_overlay.fcb$r_fid_rvn_fields.fcb$b_fid_nmx
#define		fcb$b_fid_rvn		fcb$r_fid_overlay.fcb$r_fid_fields.fcb$r_fid_rvn_overlay.fcb$r_fid_rvn_fields.fcb$b_fid_rvn
#define		fcb$l_aclbl		fcb$r_orb.fcb$l_aclbl
#define		fcb$l_aclfl		fcb$r_orb.fcb$l_aclfl
#define		fcb$l_caching_options	fcb$r_orb_extension_overlay.fcb$r_extra_fields.fcb$r_caching_options_overlay.fcb$l_caching_options
#define		fcb$l_fid_recnum	fcb$r_fid_overlay.fcb$r_fid_iso_9660_fields.fcb$l_fid_recnum
#define		fcb$l_fileowner		fcb$r_orb.fcb$r_fill_0_.fcb$l_fileowner
#define		fcb$l_grp_prot		fcb$r_orb.fcb$l_grp_prot
#define		fcb$l_hwm_waitbl	fcb$q_hwmqhd.fcb$l_hwm_waitbl
#define		fcb$l_hwm_waitfl	fcb$q_hwmqhd.fcb$l_hwm_waitfl
#define		fcb$l_limbobl		fcb$q_limboqhd.fcb$l_limbobl
#define		fcb$l_limbofl		fcb$q_limboqhd.fcb$l_limbofl
#define		fcb$l_numextents	fcb$r_fcb1_overlay.fcb$l_numextents
#define		fcb$l_own_prot		fcb$r_orb.fcb$l_own_prot
#define		fcb$l_status		fcb$r_status_overlay.fcb$l_status
#define		fcb$l_status2		fcb$r_orb_extension_overlay.fcb$r_extra_fields.fcb$r_status2_overlay.fcb$l_status2
#define		fcb$l_sys_prot		fcb$r_orb.fcb$r_fill_2_.fcb$l_sys_prot
#define		fcb$l_truncvbn		fcb$r_fcb1_overlay.fcb$l_truncvbn
#define		fcb$l_wor_prot		fcb$r_orb.fcb$l_wor_prot
#define		fcb$q_acmode		fcb$r_orb.fcb$q_acmode
#define		fcb$q_cfb		fcb$r_orb_extension_overlay.fcb$r_extra_fields.fcb$q_cfb
#define		fcb$q_hwmqhd		fcb$r_queue_overlay.fcb$q_hwmqhd
#define		fcb$q_limboqhd		fcb$r_queue_overlay.fcb$q_limboqhd
#define		fcb$r_max_class_prot	fcb$r_orb.fcb$r_max_class_prot
#define		fcb$r_min_class_prot	fcb$r_orb.fcb$r_min_class_prot
#define		fcb$v_badacl		fcb$r_status_overlay.fcb$r_status_bits.fcb$v_badacl
#define		fcb$v_badblk		fcb$r_status_overlay.fcb$r_status_bits.fcb$v_badblk
#define		fcb$v_caching_options_mbz	fcb$r_orb_extension_overlay.fcb$r_extra_fields.fcb$r_caching_options_overlay.fcb$r_caching_options_bits.fcb$v_caching_options_mbz
#define		fcb$v_delaytrnc		fcb$r_status_overlay.fcb$r_status_bits.fcb$v_delaytrnc
#define		fcb$v_dir		fcb$r_status_overlay.fcb$r_status_bits.fcb$v_dir
#define		fcb$v_erase		fcb$r_status_overlay.fcb$r_status_bits.fcb$v_erase
#define		fcb$v_excl		fcb$r_status_overlay.fcb$r_status_bits.fcb$v_excl
#define		fcb$v_file_attributes	fcb$r_orb_extension_overlay.fcb$r_extra_fields.fcb$r_caching_options_overlay.fcb$r_caching_options_bits.fcb$v_file_attributes
#define		fcb$v_file_contents	fcb$r_orb_extension_overlay.fcb$r_extra_fields.fcb$r_caching_options_overlay.fcb$r_caching_options_bits.fcb$v_file_contents
#define		fcb$v_flush_on_close	fcb$r_orb_extension_overlay.fcb$r_extra_fields.fcb$r_caching_options_overlay.fcb$r_caching_options_bits.fcb$v_flush_on_close
#define		fcb$v_isdir		fcb$r_status_overlay.fcb$r_status_bits.fcb$v_isdir
#define		fcb$v_limbo		fcb$r_status_overlay.fcb$r_status_bits.fcb$v_limbo
#define		fcb$v_markdel		fcb$r_status_overlay.fcb$r_status_bits.fcb$v_markdel
#define		fcb$v_nomove		fcb$r_status_overlay.fcb$r_status_bits.fcb$v_nomove
#define		fcb$v_noshelvable	fcb$r_status_overlay.fcb$r_status_bits.fcb$v_noshelvable
#define		fcb$v_preshelved	fcb$r_status_overlay.fcb$r_status_bits.fcb$v_preshelved
#define		fcb$v_rmslock		fcb$r_status_overlay.fcb$r_status_bits.fcb$v_rmslock
#define		fcb$v_shelved		fcb$r_status_overlay.fcb$r_status_bits.fcb$v_shelved
#define		fcb$v_spool		fcb$r_status_overlay.fcb$r_status_bits.fcb$v_spool
#define		fcb$v_stale		fcb$r_status_overlay.fcb$r_status_bits.fcb$v_stale
#define		fcb$v_vcc_state		fcb$r_orb_extension_overlay.fcb$r_extra_fields.fcb$r_status2_overlay.fcb$r_status2_bits.fcb$v_vcc_state
#define		fcb$w_fid		fcb$r_fid_overlay.fcb$w_fid
#define		fcb$w_fid_dirnum	fcb$r_fid_overlay.fcb$r_fid_iso_9660_fields.fcb$w_fid_dirnum
#define		fcb$w_fid_num		fcb$r_fid_overlay.fcb$r_fid_fields.fcb$w_fid_num
#define		fcb$w_fid_rvn		fcb$r_fid_overlay.fcb$r_fid_fields.fcb$r_fid_rvn_overlay.fcb$w_fid_rvn
#define		fcb$w_fid_seq		fcb$r_fid_overlay.fcb$r_fid_fields.fcb$w_fid_seq
#define		fcb$w_fileprot		fcb$r_orb.fcb$r_fill_2_.fcb$r_fill_3_.fcb$w_fileprot
#define		fcb$w_uicgroup		fcb$r_orb.fcb$r_fill_0_.fcb$r_fill_1_.fcb$w_uicgroup
#define		fcb$w_uicmember		fcb$r_orb.fcb$r_fill_0_.fcb$r_fill_1_.fcb$w_uicmember

#endif
