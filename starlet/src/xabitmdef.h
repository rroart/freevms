#ifndef xabitmdef_h
#define xabitmdef_h

#define		XAB$C_ITM		36

#define		XAB$M_WASCONTIG		0x1
#define		XAB$M_NOBACKUP		0x2
#define		XAB$M_WRITEBACK		0x4
#define		XAB$M_READCHECK		0x8
#define		XAB$M_WRITCHECK		0x10
#define		XAB$M_CONTIGB		0x20
#define		XAB$M_LOCKED		0x40
#define		XAB$M_CONTIG		0x80
#define		XAB$M_RESERVED1		0x100
#define		XAB$M_RESERVED2		0x200
#define		XAB$M_RESERVED3		0x400
#define		XAB$M_BADACL		0x800
#define		XAB$M_SPOOL		0x1000
#define		XAB$M_DIRECTORY		0x2000
#define		XAB$M_BADBLOCK		0x4000
#define		XAB$M_MARKDEL		0x8000
#define		XAB$M_NOCHARGE		0x10000
#define		XAB$M_ERASE		0x20000
#define		XAB$M_RESERVED4		0x40000
#define		XAB$M_SHELVED		0x80000
#define		XAB$M_RESERVED6		0x100000
#define		XAB$M_NOMOVE		0x200000
#define		XAB$M_NOSHELVABLE	0x400000
#define		XAB$M_PRESHELVED	0x800000
	
struct _uchar_flags {
  unsigned xab$v_wascontig		: 1;
  unsigned xab$v_nobackup		: 1;
  unsigned xab$v_writeback		: 1;
  unsigned xab$v_readcheck		: 1;
  unsigned xab$v_writcheck		: 1;
  unsigned xab$v_contigb		: 1;
  unsigned xab$v_locked		        : 1;
  unsigned xab$v_contig		        : 1;
  unsigned xab$v_reserved1		: 1;
  unsigned xab$v_reserved2		: 1;
  unsigned xab$v_reserved3		: 1;
  unsigned xab$v_badacl		        : 1;
  unsigned xab$v_spool	        	: 1;
  unsigned xab$v_directory		: 1;
  unsigned xab$v_badblock		: 1;
  unsigned xab$v_markdel		: 1;
  unsigned xab$v_nocharge		: 1;
  unsigned xab$v_erase		        : 1;
  unsigned xab$v_reserved4		: 1;
  unsigned xab$v_shelved		: 1;
  unsigned xab$v_reserved6		: 1;
  unsigned xab$v_nomove		        : 1;
  unsigned xab$v_noshelvable		: 1;
  unsigned xab$v_preshelved		: 1;
};

#define		XAB$_NET_BUFFER_SIZE		1
#define		XAB$_NET_BLOCK_COUNT		2
#define		XAB$_NET_REMOTE_SYSTEM		3
#define		XAB$_NET_REMOTE_FILE_SYSTEM	4
#define		XAB$_NET_EXTPROT		5
#define		XAB$_NET_SYSCAP_LOCAL		6
#define		XAB$_NET_SYSCAP_REMOTE		7
#define		XAB$_NET_DAPVER_LOCAL		8
#define		XAB$_NET_DAPVER_REMOTE		9
#define		XAB$_NET_LINK_TIMEOUT		10
#define		XAB$_NET_DATA_CRC_ENABLE	11
#define		XAB$_NET_LINK_CACHE_ENABLE	12

#define		XAB$K_NET_MIN		        1
#define		XAB$K_NET_MAX		        12
#define		XAB$_STAT_ENABLE		64
#define		XAB$_FILE_LENGTH_HINT		65
#define		XAB$K_STAT_MIN		        64
#define		XAB$K_STAT_MAX		        65
#define		XAB$_UCHAR		        128
#define		XAB$_UCHAR_WASCONTIG		129
#define		XAB$_UCHAR_NOBACKUP		130
#define		XAB$_UCHAR_WRITEBACK		131
#define		XAB$_UCHAR_READCHECK		132
#define		XAB$_UCHAR_WRITECHECK		133
#define		XAB$_UCHAR_CONTIGB		134
#define		XAB$_UCHAR_LOCKED		135
#define		XAB$_UCHAR_CONTIG		136
#define		XAB$_UCHAR_BADACL		137
#define		XAB$_UCHAR_SPOOL		138
#define		XAB$_UCHAR_DIRECTORY		139
#define		XAB$_UCHAR_BADBLOCK		140
#define		XAB$_UCHAR_MARKDEL		141
#define		XAB$_UCHAR_NOCHARGE		142
#define		XAB$_UCHAR_ERASE		143
#define		XAB$_UCHAR_NOMOVE		144
#define		XAB$_UCHAR_SHELVED		145
#define		XAB$_UCHAR_NOSHELVABLE		146
#define		XAB$_UCHAR_PRESHELVED		147
#define		XAB$K_UCHAR_MIN		        128
#define		XAB$K_UCHAR_MAX		        147
#define		XAB$_STORED_SEMANTICS		192
#define		XAB$_ACCESS_SEMANTICS		193
#define		XAB$_RMS_EXTENSION		194
#define		XAB$K_XLATE_MIN		        192
#define		XAB$K_XLATE_MAX		        194
#define		XAB$C_SEMANTICS_MAX_LEN		64
#define		XAB$K_SEMANTICS_MAX_LEN		64
#define		XAB$_MULTIBUFFER_COUNT		256

#define		XAB$_NORECORD		257

#define		XAB$_RECOVERY		258

#define		XAB$_RUJVOLNAM		259
#define		XAB$_GBC_LONG		260
#define		XAB$K_MISC_MIN		256
#define		XAB$K_MISC_MAX		260
#define		XAB$_TID		320
#define		XAB$K_TID_MIN		320
#define		XAB$K_TID_MAX		320
#define		XAB$_CACHING_OPTIONS	384
#define		XAB$_CACHING_ATTRIBUTE	385
#define		XAB$K_CACHING_OPTIONS_MIN	384
#define		XAB$K_CACHING_OPTIONS_MAX	385
#define		XAB$K_SENSEMODE		1
#define		XAB$K_SETMODE		2
	
struct _xabitmdef {
  unsigned char xab$b_cod;
  unsigned char xab$b_bln;
  short int xabitmdef$$_fill_1;
  void *xab$l_nxt;
  void *xab$l_itemlist;
  unsigned char xab$b_mode;
  char xab$b_itm_fill1 [3];
  unsigned int xab$il_itm_fill2 [4];
};

#define		XAB$K_ITMLEN		32
#define		XAB$C_ITMLEN		32

#define		XAB$M_FILE_ATTRIBUTES	0xF
#define		XAB$M_FILE_CONTENTS	0xF0
#define		XAB$K_DEFAULT		0
#define		XAB$K_WRITETHROUGH	1
#define		XAB$K_WRITEBEHIND	2
#define		XAB$K_NOCACHING		3
#define		XAB$M_FLUSH_ON_CLOSE	0xF00
#define		XAB$K_FLUSH		1
#define		XAB$K_NOFLUSH		2
#define		XAB$M_CACHECTL_MBZ	0xFFFFF000
	
struct _xab_caching_options_flags {
  unsigned xab$v_file_attributes	: 4;
  unsigned xab$v_file_contents		: 4;
  unsigned xab$v_flush_on_close		: 4;
  unsigned xab$v_cachectl_mbz		: 20;
};

#define		XAB$M_RCF_RU		0x1
#define		XAB$M_RCF_AI		0x2
#define		XAB$M_RCF_BI		0x4
#define		XAB$M_RCF_NOPAD		0x8
	
struct _xab_rcf_flags {
  unsigned xab$v_rcf_ru		: 1;
  unsigned xab$v_rcf_ai		: 1;
  unsigned xab$v_rcf_bi		: 1;
  unsigned xab$v_rcf_nopad	: 1;
  unsigned xab$v_fill_17_	: 4;
};

#define		XAB$K_DISABLE		0
#define		XAB$K_ENABLE		1
#define		XAB$K_RT11		1
#define		XAB$K_RSTS		2
#define		XAB$K_RSX11S		3
#define		XAB$K_RSX11M		4
#define		XAB$K_RSX11D		5
#define		XAB$K_IAS		6
#define		XAB$K_VAXVMS		7
#define		XAB$K_TOPS20		8
#define		XAB$K_TOPS10		9
#define		XAB$K_RTS8		10
#define		XAB$K_OS8		11
#define		XAB$K_RSX11MP		12
#define		XAB$K_COPOS11		13
#define		XAB$K_P_OS		14
#define		XAB$K_VAXELN		15
#define		XAB$K_CPM		16
#define		XAB$K_MS_DOS		17
#define		XAB$K_ULTRIX_32		18
#define		XAB$K_ULTRIX_11		19
#define		XAB$K_RMS11		1
#define		XAB$K_RMS20		2
#define		XAB$K_RMS32		3
#define		XAB$K_FCS11		4
#define		XAB$K_RT11FS		5
#define		XAB$K_NO_FS		6
#define		XAB$K_TOPS20FS		7
#define		XAB$K_TOPS10FS		8
#define		XAB$K_OS8FS		9
#define		XAB$K_RMS32S		10
#define		XAB$K_CPMFS		11
#define		XAB$K_MS_DOSFS		12
#define		XAB$K_ULTRIX32_FS	13
#define		XAB$K_ULTRIX11_FS	14
#define		XAB$M_NET_NOREAD	0x1
#define		XAB$M_NET_NOWRITE	0x2
#define		XAB$M_NET_NOEXECUTE	0x4
#define		XAB$M_NET_NODELETE	0x8
#define		XAB$M_NET_NOAPPEND	0x10
#define		XAB$M_NET_NODIRECTORY	0x20
#define		XAB$M_NET_NOUPDATE	0x40
#define		XAB$M_NET_NOCHANGE	0x80
#define		XAB$M_NET_NOEXTEND	0x100
	
struct _xabnetextprot {
  union  {
    unsigned short int xab$w_system_acc;
    struct  {
      unsigned xab$v_net_noread		        : 1;
      unsigned xab$v_net_nowrite		: 1;
      unsigned xab$v_net_noexecute		: 1;
      unsigned xab$v_net_nodelete		: 1;
      unsigned xab$v_net_noappend		: 1;
      unsigned xab$v_net_nodirectory		: 1;
      unsigned xab$v_net_noupdate		: 1;
      unsigned xab$v_net_nochange		: 1;
      unsigned xab$v_net_noextend		: 1;
      unsigned xab$v_fill_20_		        : 7;
    };
  };
  unsigned short int xab$w_owner_acc;
  unsigned short int xab$w_group_acc;
  unsigned short int xab$w_world_acc;
};
 
struct _xabnetdapver {
  unsigned char xab$b_ver_dap;
  unsigned char xab$b_ver_eco;
  unsigned char xab$b_ver_cus;
  unsigned char xab$b_ver_dsv;
  unsigned char xab$b_ver_csv;
};

struct _xabnetcapdef {
  unsigned xab$v_cap_filall		: 1;
  unsigned xab$v_cap_seqorg		: 1;
  unsigned xab$v_cap_relorg		: 1;
  unsigned xab$v_cap_fill_21		: 1;
  unsigned xab$v_cap_extend		: 1;
  unsigned xab$v_cap_seqfil		: 1;
  unsigned xab$v_cap_ranrrn		: 1;
  unsigned xab$v_cap_ranvbn		: 1;
  unsigned xab$v_cap_rankey		: 1;
  unsigned xab$v_cap_fill_22		: 1;
  unsigned xab$v_cap_ranrfa		: 1;
  unsigned xab$v_cap_idxorg		: 1;
  unsigned xab$v_cap_swmode		: 1;
  unsigned xab$v_cap_append		: 1;
  unsigned xab$v_cap_submit		: 1;
  unsigned xab$v_cap_fill_23		: 1;
  unsigned xab$v_cap_mds		: 1;
  unsigned xab$v_cap_display		: 1;
  unsigned xab$v_cap_msgblk		: 1;
  unsigned xab$v_cap_unrblk		: 1;
  unsigned xab$v_cap_bigblk		: 1;
  unsigned xab$v_cap_dapcrc		: 1;
  unsigned xab$v_cap_keyxab		: 1;
  unsigned xab$v_cap_allxab		: 1;
  unsigned xab$v_cap_sumxab		: 1;
  unsigned xab$v_cap_directory		: 1;
  unsigned xab$v_cap_timxab		: 1;
  unsigned xab$v_cap_proxab		: 1;
  unsigned xab$v_cap_fill_24		: 1;
  unsigned xab$v_cap_fopspl		: 1;
  unsigned xab$v_cap_fopscf		: 1;
  unsigned xab$v_cap_fopdlt		: 1;
  unsigned xab$v_cap_fill_26		: 1;
  unsigned xab$v_cap_seqrac		: 1;
  unsigned xab$v_cap_fill_27		: 1;
  unsigned xab$v_cap_bitopt		: 1;
  unsigned xab$v_cap_warning		: 1;
  unsigned xab$v_cap_rename		: 1;
  unsigned xab$v_cap_wildcard		: 1;
  unsigned xab$v_cap_gngopt		: 1;
  unsigned xab$v_cap_nammsg		: 1;
  unsigned xab$v_cap_segmsg		: 1;
  unsigned xab$v_cap_chgattcls		: 1;
  unsigned xab$v_cap_chgtimcls		: 1;
  unsigned xab$v_cap_chgprocls		: 1;
  unsigned xab$v_cap_chgnamcls		: 1;
  unsigned xab$v_cap_modattcre		: 1;
  unsigned xab$v_cap_nam3part		: 1;
  unsigned xab$v_cap_chgattren		: 1;
  unsigned xab$v_cap_chgtimren		: 1;
  unsigned xab$v_cap_chgproren		: 1;
  unsigned xab$v_cap_ctlblkcnt		: 1;
  unsigned xab$v_cap_octalver		: 1;
  unsigned xab$v_fill_21_		: 3;
};

#define		XAB$S_BUFFER_SIZE		4
#define		XAB$S_NETWORK_BLOCK_COUNT	4
#define		XAB$S_REMOTE_SYSTEM		4
#define		XAB$S_REMOTE_FILE_SYSTEM	4
#define		XAB$S_EXTPROT_LOCAL		8
#define		XAB$S_EXTPROT_REMOTE		8
#define		XAB$S_CAPABIL_LOCAL		8
#define		XAB$S_CAPABIL_REMOTE		8
#define		XAB$S_DAPVER_LOCAL		5
#define		XAB$S_DAPVER_REMOTE		5
#define		XAB$S_CACHE_TMO		        4
#define		XAB$S_HINT_LENGTH		16
#define		XAB$S_GBC_LONG_LENGTH		4
	
#endif
 
