#ifndef acedef_h
#define acedef_h

#define		ACE$C_KEYID		1
#define		ACE$C_RMSJNL_BI		2
#define		ACE$C_RMSJNL_AI		3
#define		ACE$C_RMSJNL_AT		4
#define		ACE$C_AUDIT		5
#define		ACE$C_ALARM		6
#define		ACE$C_INFO		7
#define		ACE$C_RMSJNL_RU_DEFAULT	8
#define		ACE$C_DIRDEF		9
#define		ACE$C_RMSJNL_RU		10
#define		ACE$C_SUBSYSTEM_IDS	11
#define		ACE$C_OIL		12
#define		ACE$C_NEW_OWNER		13
#define		ACE$C_MAX_CODE		14
#define		ACE$C_RESERVED		255
#define		ACE$C_APPLICATION	7
#define		ACE$C_CUST		1
#define		ACE$C_CSS		2
#define		ACE$C_VMS		3
#define		ACE$M_SUCCESS		0x1
#define		ACE$M_FAILURE		0x2
#define		ACE$M_DEFAULT		0x100
#define		ACE$M_PROTECTED		0x200
#define		ACE$M_HIDDEN		0x400
#define		ACE$M_NOPROPAGATE	0x800
#define		ACE$M_READ		0x1
#define		ACE$M_WRITE		0x2
#define		ACE$M_EXECUTE		0x4
#define		ACE$M_DELETE		0x8
#define		ACE$M_CONTROL		0x10
#define		ACE$K_LENGTH		8
#define		ACE$C_LENGTH		8
#define		ACE$C_RMS_ATTRIBUTES	0
#define		ACE$K_RMS_ATTRIBUTES	0
#define		ACE$K_RMSATR_MINOR_V50	2
#define		ACE$K_RMSATR_MINOR_VDW	2
#define		ACE$C_RMSATR_MINOR_ID	2
#define		ACE$K_RMSATR_MINOR_ID	2
#define		ACE$C_RMSATR_MAJOR_ID	1
#define		ACE$K_RMSATR_MAJOR_ID	1
#define		ACE$M_STATISTICS	0x1
#define		ACE$M_XLATE_DEC		0x2
#define		ACE$S_RMSATR		20
#define		ACE$S_RMSATR_V50	20
#define		ACE$S_RMSATR_VDW	20

#define		ACE$_RMSATR_ORGANIZATION	1
#define		ACE$_RMSATR_SEMANTICS		2
#define		ACE$K_RMSATR_ORG_MAX_LEN	64
#define		ACE$K_RMSATR_SEM_MAX_LEN	64
#define		ACE$S_RMSJNL_RU_DEFAULT		17
#define		ACE$M_JOURNAL_DISABLED		0x1
#define		ACE$M_BACKUP_DONE		0x2
#define		ACE$S_RMSJNL_RU			38
#define		ACE$S_RMSJNL			50
#define		ACE$S_OIL_S64I64		32
#define		ACE$S_OIL_S128			32
#define		ACE$S_OIL			32
	
struct _acedef {
  unsigned char ace$b_size;
  unsigned char ace$b_type;
  union  {
    unsigned short int ace$w_flags;
    struct  {
      unsigned ace$v_info_type	: 4;
      unsigned ace$v_fill_6_	: 4;
    };
    struct  {
      unsigned ace$v_reserved	: 4;
      unsigned ace$v_fill_7_	: 4;
    };
    struct  {
      unsigned ace$v_success	: 1;
      unsigned ace$v_failure	: 1;
      unsigned ace$v_fill_8_	: 6;
    };
    struct  {
      unsigned ace$v_filler_1		: 8;
      unsigned ace$v_default		: 1;
      unsigned ace$v_protected		: 1;
      unsigned ace$v_hidden		: 1;
      unsigned ace$v_nopropagate	: 1;
      unsigned ace$v_fill_9_		: 4;
    };
  };
  union  {
    struct  {
      union  {
	unsigned int ace$l_access;
	struct  {
	  unsigned ace$v_read		: 1;
	  unsigned ace$v_write		: 1;
	  unsigned ace$v_execute	: 1;
	  unsigned ace$v_delete		: 1;
	  unsigned ace$v_control	: 1;
	  unsigned ace$v_fill_10_	: 3;
	};
      };
      union  {
	unsigned int ace$l_key;
	char ace$t_auditname [16];
      };
    };
    struct  {
      union  {
	unsigned int ace$l_info_flags;
	struct  {
	  unsigned short int ace$w_application_flags;
	  unsigned short int ace$w_application_facility;
	};
      };
      union  {
	char ace$t_info_start;
	struct  {
	  unsigned short int ace$w_rmsatr_variant;
	  unsigned char ace$b_rmsatr_fixlen;
	  char ace$b_rmsatr_spare1;
	  unsigned short int ace$w_rmsatr_minor_id;
	  unsigned short int ace$w_rmsatr_major_id;
	  union  {
	    unsigned int ace$l_rms_attribute_flags;
	    struct  {
	      unsigned ace$v_statistics	: 1;
	      unsigned ace$v_xlate_dec	: 1;
	      unsigned ace$v_fill_11_	: 6;
	    };
	  };
	};
      };
    };
    struct  {
      char ace$t_volnam [12];
      unsigned char ace$b_volnam_len;
      unsigned char ace$b_rjrver;
      union  {
	unsigned short int ace$w_fid [3];
	struct  {
	  unsigned short int ace$w_fid_num;
	  unsigned short int ace$w_fid_seq;
	  union  {
	    unsigned short int ace$w_fid_rvn;
	    struct  {
	      unsigned char ace$b_fid_rvn;
	      unsigned char ace$b_fid_nmx;
	    };
	  };
	};
      };
      union  {
	unsigned short int ace$w_rmsjnl_flags;
	struct  {
	  unsigned ace$v_journal_disabled	: 1;
	  unsigned ace$v_backup_done		: 1;
	  unsigned ace$v_fill_12_		: 6;
	};
      };
      unsigned int ace$l_jnlidx;
      unsigned long long ace$q_cdate;
      unsigned int ace$l_backup_seqno;
      unsigned long long ace$q_modification_time;
    };
    struct  {
      unsigned int ace$l_spare1;
      unsigned int ace$l_sys_prot;
      unsigned int ace$l_own_prot;
      unsigned int ace$l_grp_prot;
      unsigned int ace$l_wor_prot;
    };
    struct  {
      unsigned int ace$l_spare2;
      unsigned long long ace$q_image_ids;
    };
    struct  {
      unsigned int ace$l_oil_flags;
      unsigned int ace$l_brand_name;
      char ace$r_class_prot;
    };
  };
};
 
#endif
 
