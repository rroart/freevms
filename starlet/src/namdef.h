#ifndef namdef_h
#define namdef_h

#define		NAM$C_BID		2
#define		NAM$C_MAXRSS		255
#define		NAM$C_MAXRSSLCL		255
#define		NAM$M_PWD		0x1
#define		NAM$M_FILL_1		0x2
#define		NAM$M_FILL_2		0x4
#define		NAM$M_SYNCHK		0x8
#define		NAM$M_NOCONCEAL		0x10
#define		NAM$M_SLPARSE		0x20
#define		NAM$M_SRCHXABS		0x40
#define		NAM$M_NO_SHORT_UPCASE	0x80
#define		NAM$C_UFS		0

#define		NAM$C_NO_RFS		0
#define		NAM$C_RMS11		1
#define		NAM$C_RMS20		2
#define		NAM$C_RMS32		3
#define		NAM$C_FCS11		4
#define		NAM$C_RT11FS		5
#define		NAM$C_TOPS20FS		7
#define		NAM$C_TOPS10FS		8
#define		NAM$C_RMS32S		10
#define		NAM$C_CPMFS		11
#define		NAM$C_MS_DOSFS		12
#define		NAM$C_ULTRIX32_FS	13
#define		NAM$C_ULTRIX11_FS	14
#define		DAP$K_SNADTF_FS		15

#define		NAM$C_DVI		16

#define		NAM$M_IFI		0x10000
#define		NAM$M_SRCHNMF		0x40000000
#define		NAM$M_SVCTX		0x80000000
#define		NAM$K_BLN_V2		56
#define		NAM$C_BLN_V2		56
#define		NAM$M_EXP_VER		0x1
#define		NAM$M_EXP_TYPE		0x2
#define		NAM$M_EXP_NAME		0x4
#define		NAM$M_WILD_VER		0x8
#define		NAM$M_WILD_TYPE		0x10
#define		NAM$M_WILD_NAME		0x20
#define		NAM$M_EXP_DIR		0x40
#define		NAM$M_EXP_DEV		0x80
#define		NAM$M_WILDCARD		0x100
#define		NAM$M_DIR_LVLS_G7	0x200
#define		NAM$M_WILD_SFDG7	0x400
#define		NAM$M_SEARCH_LIST	0x800
#define		NAM$M_CNCL_DEV		0x1000
#define		NAM$M_ROOT_DIR		0x2000
#define		NAM$M_LOWVER		0x4000
#define		NAM$M_HIGHVER		0x8000
#define		NAM$M_PPF		0x10000
#define		NAM$M_NODE		0x20000
#define		NAM$M_QUOTED		0x40000
#define		NAM$M_GRP_MBR		0x80000
#define		NAM$M_WILD_DIR		0x100000
#define		NAM$M_DIR_LVLS		0xE00000
#define		NAM$M_WILD_UFD		0x1000000
#define		NAM$M_WILD_SFD1		0x2000000
#define		NAM$M_WILD_SFD2		0x4000000
#define		NAM$M_WILD_SFD3		0x8000000
#define		NAM$M_WILD_SFD4		0x10000000
#define		NAM$M_WILD_SFD5		0x20000000
#define		NAM$M_WILD_SFD6		0x40000000
#define		NAM$M_WILD_SFD7		0x80000000
#define		NAM$M_WILD_GRP		0x1000000
#define		NAM$M_WILD_MBR		0x2000000
#define		NAM$M_DID		0x1
#define		NAM$M_FID		0x2
#define		NAM$M_RES_DID		0x4
#define		NAM$M_RES_FID		0x8
#define		NAM$M_RES_ESCAPE	0x10
#define		NAM$M_RES_UNICODE	0x20
	
#define		NAM$K_BLN_DIRWC		96
#define		NAM$C_BLN_DIRWC		96
#define		NAM$K_BLN		96
#define		NAM$C_BLN		96
	
#define		NAM$V_PWD	0
#define		NAM$V_FILL_1	1
#define		NAM$V_FILL_2	2
#define		NAM$V_SYNCHK	3
#define		NAM$V_NOCONCEAL	4
#define		NAM$V_SLPARSE	5
#define		NAM$V_SRCHXABS	6
#define		NAM$V_NO_SHORT_UPCASE	7
#define		NAM$V_IFI	16
#define		NAM$V_SRCHNMF	30
#define		NAM$V_SVCTX	31
#define		NAM$V_EXP_VER	0
#define		NAM$V_EXP_TYPE	1
#define		NAM$V_EXP_NAME	2
#define		NAM$V_WILD_VER	3
#define		NAM$V_WILD_TYPE	4
#define		NAM$V_WILD_NAME	5
#define		NAM$V_EXP_DIR	6
#define		NAM$V_EXP_DEV	7
#define		NAM$V_WILDCARD	8
#define		NAM$V_DIR_LVLS_G7 9
#define		NAM$V_WILD_SFDG7 10
#define		NAM$V_SEARCH_LIST 11
#define		NAM$V_CNCL_DEV	12
#define		NAM$V_ROOT_DIR	13
#define		NAM$V_LOWVER	14
#define		NAM$V_HIGHVER	15
#define		NAM$V_PPF	16
#define		NAM$V_NODE	17
#define		NAM$V_QUOTED	18
#define		NAM$V_GRP_MBR	19
#define		NAM$V_WILD_DIR	20
#define		NAM$V_DIR_LVLS	21
#define		NAM$S_DIR_LVLS	3
#define		NAM$V_WILD_UFD	24
#define		NAM$V_WILD_SFD1	25
#define		NAM$V_WILD_SFD2	26
#define		NAM$V_WILD_SFD3	27
#define		NAM$V_WILD_SFD4	28
#define		NAM$V_WILD_SFD5	29
#define		NAM$V_WILD_SFD6	30
#define		NAM$V_WILD_SFD7	31
#define		NAM$V_WILD_GRP	24
#define		NAM$V_WILD_MBR	25
#define		NAM$V_DID	0
#define		NAM$V_FID	1
#define		NAM$V_RES_DID	2
#define		NAM$V_RES_FID	3
#define		NAM$V_RES_ESCAPE		4
#define		NAM$V_RES_UNICODE		5
#define		NAML$V_NO_SHORT_OUTPUT		0
#define		NAML$V_LONG_RESULT_ESCAPE	0
#define		NAML$V_FILESYS_NAME_UCS2	1
#define		NAML$V_LONG_RESULT_DID		2
#define		NAML$V_LONG_RESULT_FID		3
#define		NAML$V_LONG_RESULT_UNICODE	4

#define		NAML$C_BID		4

#define		NAML$C_MAXRSS		4095
#define		NAML$M_PWD		0x1
#define		NAML$M_FILL_1		0x2
#define		NAML$M_FILL_2		0x4
#define		NAML$M_SYNCHK		0x8
#define		NAML$M_NOCONCEAL	0x10
#define		NAML$M_SLPARSE		0x20
#define		NAML$M_SRCHXABS		0x40
#define		NAML$M_NO_SHORT_UPCASE	0x80
#define		NAML$C_UFS		0

#define		NAML$C_NO_RFS		0
#define		NAML$C_RMS11		1
#define		NAML$C_RMS20		2
#define		NAML$C_RMS32		3
#define		NAML$C_FCS11		4
#define		NAML$C_RT11FS		5
#define		NAML$C_TOPS20FS		7
#define		NAML$C_TOPS10FS		8
#define		NAML$C_RMS32S		10
#define		NAML$C_CPMFS		11
#define		NAML$C_MS_DOSFS		12
#define		NAML$C_ULTRIX32_FS	13
#define		NAML$C_ULTRIX11_FS	14

#define		NAML$C_DVI		16

#define		NAML$M_IFI		0x10000
#define		NAML$M_SRCHNMF		0x40000000
#define		NAML$M_SVCTX		0x80000000
#define		NAML$K_BLN_V2		56
#define		NAML$C_BLN_V2		56
#define		NAML$M_EXP_VER		0x1
#define		NAML$M_EXP_TYPE		0x2
#define		NAML$M_EXP_NAME		0x4
#define		NAML$M_WILD_VER		0x8
#define		NAML$M_WILD_TYPE	0x10
#define		NAML$M_WILD_NAME	0x20
#define		NAML$M_EXP_DIR		0x40
#define		NAML$M_EXP_DEV		0x80
#define		NAML$M_WILDCARD		0x100
#define		NAML$M_DIR_LVLS_G7	0x200
#define		NAML$M_WILD_SFDG7	0x400
#define		NAML$M_SEARCH_LIST	0x800
#define		NAML$M_CNCL_DEV		0x1000
#define		NAML$M_ROOT_DIR		0x2000
#define		NAML$M_LOWVER		0x4000
#define		NAML$M_HIGHVER		0x8000
#define		NAML$M_PPF		0x10000
#define		NAML$M_NODE		0x20000
#define		NAML$M_QUOTED		0x40000
#define		NAML$M_GRP_MBR		0x80000
#define		NAML$M_WILD_DIR		0x100000
#define		NAML$M_DIR_LVLS		0xE00000
#define		NAML$M_WILD_UFD		0x1000000
#define		NAML$M_WILD_SFD1	0x2000000
#define		NAML$M_WILD_SFD2	0x4000000
#define		NAML$M_WILD_SFD3	0x8000000
#define		NAML$M_WILD_SFD4	0x10000000
#define		NAML$M_WILD_SFD5	0x20000000
#define		NAML$M_WILD_SFD6	0x40000000
#define		NAML$M_WILD_SFD7	0x80000000
#define		NAML$M_WILD_GRP		0x1000000
#define		NAML$M_WILD_MBR		0x2000000
#define		NAML$M_DID		0x1
#define		NAML$M_FID		0x2
#define		NAML$M_RES_DID		0x4
#define		NAML$M_RES_FID		0x8
#define		NAML$M_RES_ESCAPE		0x10
#define		NAML$M_RES_UNICODE		0x20
#define		NAML$M_LONG_RESULT_ESCAPE	0x1
#define		NAML$M_FILESYS_NAME_UCS2	0x2
#define		NAML$M_LONG_RESULT_DID		0x4
#define		NAML$M_LONG_RESULT_FID		0x8
#define		NAML$M_LONG_RESULT_UNICODE	0x10
#define		NAML$M_NO_SHORT_OUTPUT		0x1
	
#define		NAML$K_BLN		248
#define		NAML$C_BLN		248

struct _namdef {
  unsigned char nam$b_bid;
  unsigned char nam$b_bln;
  unsigned char nam$b_rss;
  unsigned char nam$b_rsl;
  char *nam$l_rsa;
  union  {
    unsigned char nam$b_nop;
    struct  {
      unsigned nam$v_pwd		: 1;
      unsigned nam$v_fill_1		: 1;
      unsigned nam$v_fill_2		: 1;
      unsigned nam$v_synchk		: 1;
      unsigned nam$v_noconceal		: 1;
      unsigned nam$v_slparse		: 1;
      unsigned nam$v_srchxabs		: 1;
      unsigned nam$v_no_short_upcase	: 1;
    };
  };
  unsigned char nam$b_rfs;
  unsigned char nam$b_ess;
  unsigned char nam$b_esl;
  char *nam$l_esa;
  union  {
    struct _namdef *nam$l_rlf;
    struct _namldef *nam$l_rlf_naml;
  };
  char nam$t_dvi [16];
  union  {
    unsigned short int nam$w_fid [3];
    struct  {
      unsigned short int nam$w_fid_num;
      unsigned short int nam$w_fid_seq;
      union  {
	unsigned short int nam$w_fid_rvn;
	struct  {
	  unsigned char nam$b_fid_rvn;
	  unsigned char nam$b_fid_nmx;
	};
      };
    };
  };
  union  {
    unsigned short int nam$w_did [3];
    struct  {
      unsigned short int nam$w_did_num;
      unsigned short int nam$w_did_seq;
      union  {
	unsigned short int nam$w_did_rvn;
	struct  {
	  unsigned char nam$b_did_rvn;
	  unsigned char nam$b_did_nmx;
	};
      };
    };
  };
  union  {
    unsigned long nam$l_wcc; // check. is index. drop long later. was int.
    struct  {
      unsigned namdef$$_fill_1		: 16;
      unsigned nam$v_ifi		: 1;
      unsigned namdef$$_fill_2		: 13;
      unsigned nam$v_srchnmf		: 1;
      unsigned nam$v_svctx		: 1;
    };
  };
  union  {
    unsigned int nam$l_fnb;
    struct  {
      unsigned nam$v_exp_ver		: 1;
      unsigned nam$v_exp_type		: 1;
      unsigned nam$v_exp_name		: 1;
      unsigned nam$v_wild_ver		: 1;
      unsigned nam$v_wild_type		: 1;
      unsigned nam$v_wild_name		: 1;
      unsigned nam$v_exp_dir		: 1;
      unsigned nam$v_exp_dev		: 1;
      unsigned nam$v_wildcard		: 1;
      unsigned nam$v_dir_lvls_g7	: 1;
      unsigned nam$v_wild_sfdg7		: 1;
      unsigned nam$v_search_list	: 1;
      unsigned nam$v_cncl_dev		: 1;
      unsigned nam$v_root_dir		: 1;
      unsigned nam$v_lowver		: 1;
      unsigned nam$v_highver		: 1;
      unsigned nam$v_ppf		: 1;
      unsigned nam$v_node		: 1;
      unsigned nam$v_quoted		: 1;
      unsigned nam$v_grp_mbr		: 1;
      unsigned nam$v_wild_dir		: 1;
      unsigned nam$v_dir_lvls		: 3;
    };
    struct  {
      unsigned namdef$$_fill_4		: 24;
      unsigned nam$v_wild_ufd		: 1;
      unsigned nam$v_wild_sfd1		: 1;
      unsigned nam$v_wild_sfd2		: 1;
      unsigned nam$v_wild_sfd3		: 1;
      unsigned nam$v_wild_sfd4		: 1;
      unsigned nam$v_wild_sfd5		: 1;
      unsigned nam$v_wild_sfd6		: 1;
      unsigned nam$v_wild_sfd7		: 1;
    };
    struct  {
      unsigned namdef$$_fill_5		: 24;
      unsigned nam$v_wild_grp		: 1;
      unsigned nam$v_wild_mbr		: 1;
      unsigned nam$v_fill_5_		: 6;
    };
  };
  unsigned char nam$b_node;
  unsigned char nam$b_dev;
  unsigned char nam$b_dir;
  unsigned char nam$b_name;
  unsigned char nam$b_type;
  unsigned char nam$b_ver;
  union  {
    unsigned char nam$b_nmc;
    struct  {
      unsigned nam$v_did		: 1;
      unsigned nam$v_fid		: 1;
      unsigned nam$v_res_did		: 1;
      unsigned nam$v_res_fid		: 1;
      unsigned nam$v_res_escape		: 1;
      unsigned nam$v_res_unicode	: 1;
      unsigned namdef$$_fill_3		: 2;
    };
  };
  char namdef$$_fill_6 [1];
  char *nam$l_node;
  char *nam$l_dev;
  char *nam$l_dir;
  char *nam$l_name;
  char *nam$l_type;
  char *nam$l_ver;
  unsigned short int nam$w_first_wild_dir;
  unsigned short int nam$w_long_dir_levels;
  unsigned int namdef$$_fill_7 [1];
};
 
struct _namldef {
  unsigned char naml$b_bid;
  unsigned char naml$b_bln;
  unsigned char naml$b_rss;
  unsigned char naml$b_rsl;
  char *naml$l_rsa;
  union  {
    unsigned char naml$b_nop;
    struct  {
      unsigned naml$v_pwd		: 1;
      unsigned naml$v_fill_1		: 1;
      unsigned naml$v_fill_2		: 1;
      unsigned naml$v_synchk		: 1;
      unsigned naml$v_noconceal		: 1;
      unsigned naml$v_slparse		: 1;
      unsigned naml$v_srchxabs		: 1;
      unsigned naml$v_no_short_upcase	: 1;
    };
  };
  unsigned char naml$b_rfs;
  unsigned char naml$b_ess;
  unsigned char naml$b_esl;
  char *naml$l_esa;
  union  {
    struct _namdef *naml$l_rlf;
    struct _namldef *naml$l_rlf_naml;
  };
  char naml$t_dvi [16];
  union  {
    unsigned short int naml$w_fid [3];
    struct  {
      unsigned short int naml$w_fid_num;
      unsigned short int naml$w_fid_seq;
      union  {
	unsigned short int naml$w_fid_rvn;
	struct  {
	  unsigned char naml$b_fid_rvn;
	  unsigned char naml$b_fid_nmx;
	};
      };
    };
  };
  union  {
    unsigned short int naml$w_did [3];
    struct  {
      unsigned short int naml$w_did_num;
      unsigned short int naml$w_did_seq;
      union  {
	unsigned short int naml$w_did_rvn;
	struct  {
	  unsigned char naml$b_did_rvn;
	  unsigned char naml$b_did_nmx;
	};
      };
    };
  };
  union  {
    unsigned int naml$l_wcc;
    struct  {
      unsigned namldef$$_fill_1		: 16;
      unsigned naml$v_ifi		: 1;
      unsigned namldef$$_fill_2		: 13;
      unsigned naml$v_srchnmf		: 1;
      unsigned naml$v_svctx		: 1;
    };
  };
  union  {
    unsigned int naml$l_fnb;
    struct  {
      unsigned naml$v_exp_ver		: 1;
      unsigned naml$v_exp_type		: 1;
      unsigned naml$v_exp_name		: 1;
      unsigned naml$v_wild_ver		: 1;
      unsigned naml$v_wild_type		: 1;
      unsigned naml$v_wild_name		: 1;
      unsigned naml$v_exp_dir		: 1;
      unsigned naml$v_exp_dev		: 1;
      unsigned naml$v_wildcard		: 1;
      unsigned naml$v_dir_lvls_g7	: 1;
      unsigned naml$v_wild_sfdg7	: 1;
      unsigned naml$v_search_list	: 1;
      unsigned naml$v_cncl_dev		: 1;
      unsigned naml$v_root_dir		: 1;
      unsigned naml$v_lowver		: 1;
      unsigned naml$v_highver		: 1;
      unsigned naml$v_ppf		: 1;
      unsigned naml$v_node		: 1;
      unsigned naml$v_quoted		: 1;
      unsigned naml$v_grp_mbr		: 1;
      unsigned naml$v_wild_dir		: 1;
      unsigned naml$v_dir_lvls		: 3;
    };
    struct  {
      unsigned namldef$$_fill_4		: 24;
      unsigned naml$v_wild_ufd		: 1;
      unsigned naml$v_wild_sfd1		: 1;
      unsigned naml$v_wild_sfd2		: 1;
      unsigned naml$v_wild_sfd3		: 1;
      unsigned naml$v_wild_sfd4		: 1;
      unsigned naml$v_wild_sfd5		: 1;
      unsigned naml$v_wild_sfd6		: 1;
      unsigned naml$v_wild_sfd7		: 1;
    };
    struct  {
      unsigned namldef$$_fill_5		: 24;
      unsigned naml$v_wild_grp		: 1;
      unsigned naml$v_wild_mbr		: 1;
      unsigned naml$v_fill_6_		: 6;
    };
  };
  unsigned char naml$b_node;
  unsigned char naml$b_dev;
  unsigned char naml$b_dir;
  unsigned char naml$b_name;
  unsigned char naml$b_type;
  unsigned char naml$b_ver;
  union  {
    unsigned char naml$b_nmc;
    struct  {
      unsigned naml$v_did		: 1;
      unsigned naml$v_fid		: 1;
      unsigned naml$v_res_did		: 1;
      unsigned naml$v_res_fid		: 1;
      unsigned naml$v_res_escape	: 1;
      unsigned naml$v_res_unicode	: 1;
      unsigned namldef$$_fill_3		: 2;
    };
  };
  char namldef$$_fill_6 [1];
  char *naml$l_node;
  char *naml$l_dev;
  char *naml$l_dir;
  char *naml$l_name;
  char *naml$l_type;
  char *naml$l_ver;
  unsigned short int naml$w_first_wild_dir;
  unsigned short int naml$w_long_dir_levels;
  unsigned int namldef$$_fill_7 [1];
  unsigned int naml$l_long_defname_size;
  char *naml$l_long_defname;
  unsigned int naml$l_long_filename_size;
  char *naml$l_long_filename;
  unsigned int naml$l_long_node_size;
  char *naml$l_long_node;
  unsigned int naml$l_long_dev_size;
  char *naml$l_long_dev;
  unsigned int naml$l_long_dir_size;
  char *naml$l_long_dir;
  unsigned int naml$l_long_name_size;
  char *naml$l_long_name;
  unsigned int naml$l_long_type_size;
  char *naml$l_long_type;
  unsigned int naml$l_long_ver_size;
  char *naml$l_long_ver;
  unsigned int naml$l_long_expand_size;
  char *naml$l_long_expand;
  unsigned int naml$l_long_expand_alloc;
  int naml$l_reserved_mbz_9;
  unsigned int naml$l_long_result_size;
  char *naml$l_long_result;
  unsigned int naml$l_long_result_alloc;
  int naml$l_reserved_mbz_8;
  unsigned int naml$l_filesys_name_size;
  char *naml$l_filesys_name;
  unsigned int naml$l_filesys_name_alloc;
  union  {
    unsigned int naml$l_output_flags;
    struct  {
      unsigned naml$v_long_result_escape	: 1;
      unsigned naml$v_filesys_name_ucs2		: 1;
      unsigned naml$v_long_result_did		: 1;
      unsigned naml$v_long_result_fid		: 1;
      unsigned naml$v_long_result_unicode	: 1;
      unsigned naml$v_fill_7_		: 3;
    };
  };
  union  {
    unsigned int naml$l_input_flags;
    struct  {
      unsigned naml$v_no_short_output	: 1;
      unsigned naml$v_fill_8_		: 7;
    };
  };
  int naml$l_reserved_mbz_7;
  unsigned long long naml$q_user_context;
  int naml$l_reserved_mbz_6;
  int naml$l_reserved_mbz_5;
  int naml$l_reserved_mbz_4;
  int naml$l_reserved_mbz_3;
  int naml$l_reserved_mbz_2;
  int naml$l_reserved_mbz_1;
};
 
extern struct _namdef cc$rms_nam;

extern struct _namldef cc$rms_naml;

#endif
 
