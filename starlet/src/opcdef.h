#ifndef opcdef_h
#define opcdef_h
 
#define		OPC$_RQ_TERME		1
#define		OPC$_RQ_LOGI		2
#define		OPC$_RQ_RQST		3
#define		OPC$_RQ_REPLY		4
#define		OPC$_RQ_CANCEL		5
#define		OPC$_RQ_STATUS		6
#define		OPC$_RQ_SECURITY	7
#define		OPC$_RQ_LICENSE		8
#define		OPC$M_RQ_NOOCS		128
#define		OPC$M_NM_CENTRL		0x1
#define		OPC$M_NM_PRINT		0x2
#define		OPC$M_NM_TAPES		0x4
#define		OPC$M_NM_DISKS		0x8
#define		OPC$M_NM_DEVICE		0x10
#define		OPC$M_NM_CARDS		0x20
#define		OPC$M_NM_NTWORK		0x40
#define		OPC$M_NM_CLUSTER	0x80
#define		OPC$M_NM_SECURITY	0x100
#define		OPC$M_NM_REPLY		0x200
#define		OPC$M_NM_SOFTWARE	0x400
#define		OPC$M_NM_LICENSE	0x800
#define		OPC$M_NM_OPER1		0x1000
#define		OPC$M_NM_OPER2		0x2000
#define		OPC$M_NM_OPER3		0x4000
#define		OPC$M_NM_OPER4		0x8000
#define		OPC$M_NM_OPER5		0x10000
#define		OPC$M_NM_OPER6		0x20000
#define		OPC$M_NM_OPER7		0x40000
#define		OPC$M_NM_OPER8		0x80000
#define		OPC$M_NM_OPER9		0x100000
#define		OPC$M_NM_OPER10		0x200000
#define		OPC$M_NM_OPER11		0x400000
#define		OPC$M_NM_OPER12		0x800000
	
struct _opcdef {
  unsigned char opc$b_ms_type;
  union  {
    struct  {
      union  {
	unsigned char opc$b_ms_target;
	unsigned char opc$b_ms_enab;
      };
      unsigned short int opc$w_ms_status;
    };
    unsigned char opc$z_ms_target_classes [3];
    unsigned char opc$z_ms_enab_terminals [3];
  };
  union  {
    unsigned int opc$l_ms_rplyid;
    union  {
      unsigned int opc$l_ms_mask;
      unsigned int opc$l_ms_rqstid;
    };
  };
  union  {
    unsigned int opc$l_ms_text;
    struct  {
      unsigned short int opc$w_ms_ounit;
      char opc$t_ms_oname;
    };
  };
  short int opcdef$$_fill_1 [7];
  unsigned int opc$l_ms_otext [32];
  union  {
    unsigned int opc$l_ms_maxsz;

    struct  {
      unsigned opc$v_nm_centrl		: 1;
      unsigned opc$v_nm_print		: 1;
      unsigned opc$v_nm_tapes		: 1;
      unsigned opc$v_nm_disks		: 1;
      unsigned opc$v_nm_device		: 1;
      unsigned opc$v_nm_cards		: 1;
      unsigned opc$v_nm_ntwork		: 1;
      unsigned opc$v_nm_cluster		: 1;
      unsigned opc$v_nm_security	: 1;
      unsigned opc$v_nm_reply		: 1;
      unsigned opc$v_nm_software	: 1;
      unsigned opc$v_nm_license		: 1;
      unsigned opc$v_nm_oper1		: 1;
      unsigned opc$v_nm_oper2		: 1;
      unsigned opc$v_nm_oper3		: 1;
      unsigned opc$v_nm_oper4		: 1;
      unsigned opc$v_nm_oper5		: 1;
      unsigned opc$v_nm_oper6		: 1;
      unsigned opc$v_nm_oper7		: 1;
      unsigned opc$v_nm_oper8		: 1;
      unsigned opc$v_nm_oper9		: 1;
      unsigned opc$v_nm_oper10		: 1;
      unsigned opc$v_nm_oper11		: 1;
      unsigned opc$v_nm_oper12		: 1;
    };
  };
};
 

#define OPC$M_OPR_CENTRAL 0x1
#define OPC$M_OPR_PRINTER 0x2
#define OPC$M_OPR_TAPES 0x4
#define OPC$M_OPR_DISKS 0x8
#define OPC$M_OPR_DEVICES 0x10
#define OPC$M_OPR_CARDS 0x20
#define OPC$M_OPR_NETWORK 0x40
#define OPC$M_OPR_CLUSTER 0x80
#define OPC$M_OPR_SECURITY 0x100
#define OPC$M_OPR_REPLY 0x200
#define OPC$M_OPR_SOFTWARE 0x400
#define OPC$M_OPR_LICENSE 0x800
#define OPC$M_OPR_USER1 0x1000
#define OPC$M_OPR_USER2 0x2000
#define OPC$M_OPR_USER3 0x4000
#define OPC$M_OPR_USER4 0x8000
#define OPC$M_OPR_USER5 0x10000
#define OPC$M_OPR_USER6 0x20000
#define OPC$M_OPR_USER7 0x40000
#define OPC$M_OPR_USER8 0x80000
#define OPC$M_OPR_USER9 0x100000
#define OPC$M_OPR_USER10 0x200000
#define OPC$M_OPR_USER11 0x400000
#define OPC$M_OPR_USER12 0x800000
	
union _opc_oprdef {
  struct  {
    unsigned opc$v_opr_central		: 1;
    unsigned opc$v_opr_printer		: 1;
    unsigned opc$v_opr_tapes		: 1;
    unsigned opc$v_opr_disks		: 1;
    unsigned opc$v_opr_devices		: 1;
    unsigned opc$v_opr_cards		: 1;
    unsigned opc$v_opr_network		: 1;
    unsigned opc$v_opr_cluster		: 1;
    unsigned opc$v_opr_security		: 1;
    unsigned opc$v_opr_reply		: 1;
    unsigned opc$v_opr_software		: 1;
    unsigned opc$v_opr_license		: 1;
    unsigned opc$v_opr_user1		: 1;
    unsigned opc$v_opr_user2		: 1;
    unsigned opc$v_opr_user3		: 1;
    unsigned opc$v_opr_user4		: 1;
    unsigned opc$v_opr_user5		: 1;
    unsigned opc$v_opr_user6		: 1;
    unsigned opc$v_opr_user7		: 1;
    unsigned opc$v_opr_user8		: 1;
    unsigned opc$v_opr_user9		: 1;
    unsigned opc$v_opr_user10		: 1;
    unsigned opc$v_opr_user11		: 1;
    unsigned opc$v_opr_user12		: 1;
  };
};
 
	
#define OPC opcdef

#include <opcmsg.h>

#define		OPC$K_SYSTEM		1
#define		OPC$K_GROUP		2
#define		OPC$K_USER		3
#define		OPC$K_UNSPEC		4
#define		OPC$C_SYSTEM		1
#define		OPC$C_GROUP		2
#define		OPC$C_USER		3
#define		OPC$C_UNSPEC		4
#define		OPC$K_SW_VERSION	9
#define		OPC$K_MAXREAD		2560
#define		OPC$K_MAXMESSAGE	2048
#define		OPC$K_COMHDRSIZ		38
#define		OPC$_OPCOMERROR		99999
#define		OPC$_X_OPRENABLE	10
#define		OPC$_X_LOGFILE		11
#define		OPC$_X_REQUEST		12
#define		OPC$_X_REPLY		13
#define		OPC$_X_CANCEL		14
#define		OPC$_X_STATUS		15
#define		OPC$_X_SHUTDOWN		16
#define		OPC$_X_TIMESTAMP	17
#define		OPC$_X_SECURITY		18
#define		OPC$_X_CLUSMSG		19
#define		OPC$_X_DEBUG		20
#define		OPC$_X_CNXMAN		21
#define		OPC$_X_REQUEST_END_MARK	22
#define		OPC$K_CLMACKNOWLEDGEMENT	1
#define		OPC$K_CLMACKNOWLEDGE_PLEASE	2
#define		OPC$K_CLMCANCEL			3
#define		OPC$K_CLMCHECK_OPERATOR		4
#define		OPC$K_CLMCHECK_REQUEST		5
#define		OPC$K_CLMCLUMBX			6
#define		OPC$K_CLMCLUSTER		7
#define		OPC$K_CLMDEVICE			8
#define		OPC$K_CLMIMP_CANCEL		9
#define		OPC$K_CLMIMP_DISABLE		10
#define		OPC$K_CLMOPRENABLE		11
#define		OPC$K_CLMREPLY			12
#define		OPC$K_CLMREPLY_COMPLETE		13
#define		OPC$K_CLMREQUEST		14
#define		OPC$K_CLMRPYBRD			15
#define		OPC$K_CLMRPYBRD_LOCAL		16
#define		OPC$K_CLMRPYNOT			17
#define		OPC$K_CLMSECURITY		18
#define		OPC$K_CLMSHUTDOWN		19
#define		OPC$K_CLMLOGFILE		20
#define		OPC$K_CLMREQUEST_END_MARK	21
#define		OPC$K_HDR_SIZE			26
	
struct _header_message {
  unsigned char opc$b_rqstcode;
  unsigned char opc$b_scope;
  unsigned int opc$l_options;
  union  {
    unsigned int opc$l_rq_options;
    struct  {
      unsigned char opc$b_rqopt_code;
      unsigned char opc$b_rqopt_fillb;
      unsigned short int opc$w_rqopt_fillw;
    };
  };
  unsigned int opc$l_attnmask1;
  unsigned int opc$l_attnmask2;
  unsigned int opc$l_rqstid;
  unsigned int opc$l_uic;
};
 
#define OPC$M_NOLOG 0x1
#define OPC$M_NOBRD 0x2
#define OPC$M_NOCLUSTER 0x4
	
union _fill_0_ {
  unsigned int opc$l_header_options;
  struct  {
    unsigned opc$v_nolog		: 1;
    unsigned opc$v_nobrd		: 1;
    unsigned opc$v_nocluster		: 1;
    unsigned opc$v_fill_2_		: 5;
  };
};
 
#define OPC$K_OPRENABLE_MIN_SIZE 30
	
struct _oprenable_message {
  char opc$b_oprenable_fill [26];

};

#define OPC$M_DISABLE 0x1
#define OPC$M_PERMOPER 0x2
#define OPC$M_NOREMIND 0x4
	
union _fill_3_ {
  unsigned int opc$l_oprenable_options;
  struct  {
    unsigned opc$v_disable		: 1;
    unsigned opc$v_permoper		: 1;
    unsigned opc$v_noremind		: 1;
    unsigned opc$v_fill_5_		: 5;
  };
};
 
#define OPC$K_LOGFILE_MIN_SIZE 30
#define OPC$K_INITLOG 0
#define OPC$K_CLOSELOG 1
#define OPC$K_ENABLELOG 2

#define OPC$K_DISABLELOG 3

#define OPC$K_MAX_LOGOPT 4
	
struct _logfile_message {
  char opc$b_logfile_fill [26];
};

#define OPC$K_REQUEST_MIN_SIZE 28
	
struct _request_message {
  char opc$b_request_fill [26];
  unsigned short int opc$w_request_length;
};

#define OPC$K_SECURITY_MIN_SIZE 28
	
struct _security_message {
  char opc$b_security_fill [26];
  unsigned short int opc$w_security_length;
};

#define OPC$K_REPLY_MIN_SIZE 32
	
struct _reply_message {
  char opc$b_reply_fill [26];
};

#define OPC$K_STATUS_MIN_SIZE 30
	
struct _status_message {
  char opc$b_status_fill [26];
};

#define OPC$K_TIMESTAMP_MIN_SIZE 26
	
struct _timestamp_message {
  char opc$b_timestamp_fill [26];
};

#define OPC$K_SHUTDOWN_MIN_SIZE 26
	
struct _shutdown_message {
  char opc$b_shutdown_fill [26];
};

#define OPC$M_CLUSTER 0x1
	
union _fill_6_ {
  unsigned int opc$l_shutdown_options;
  struct  {
    unsigned opc$v_cluster		: 1;
    unsigned opc$v_fill_8_		: 7;
  };
};

#define OPC$K_CANCEL_MIN_SIZE 26
	
struct _cancel_message {
  char opc$b_cancel_fill [26];
};

#define OPC$M_RQSTDONE 0x1
	
union _fill_9_ {
  unsigned int opc$l_cancel_options;
  struct  {
    unsigned opc$v_rqstdone		: 1;
    unsigned opc$v_fill_11_		: 7;
  };
};
 
union _opcf_arg {
  int opcf_arg$l_arg_data;
  struct  {
    unsigned short int opcf_arg$w_arg_offset;
    unsigned short int opcf_arg$w_arg_cntrl;
  };
};
 
struct _opcfao {
  unsigned short int opcf$w_msgtype;
  unsigned short int opcf$w_spare_1;
  unsigned int opcf$l_opc_msg;
  unsigned int opcf$l_opc_class_msk;
  unsigned int opcf$l_arg_count;
  union  {
    union _opcf_arg opcf$r_item_arg [60];
    char opcf$t_item_cd [240];
    unsigned int opcf$l_item_ld [60];
  };
};
 
#define		OPCF$K_AC		1
#define		OPCF$K_AD		2
#define		OPCF$K_AF		3
#define		OPCF$K_AS		4
#define		OPCF$K_AS_DEVICE_NAME	5
#define		OPCF$K_AZ		6
#define		OPCF$K_D		7
#define		OPCF$K_I		8
#define		OPCF$K_OA		9
#define		OPCF$K_OB		10
#define		OPCF$K_OH		11
#define		OPCF$K_OI		12
#define		OPCF$K_OJ		13
#define		OPCF$K_OL		14
#define		OPCF$K_OQ		15
#define		OPCF$K_OW		16
#define		OPCF$K_SA		17
#define		OPCF$K_SB		18
#define		OPCF$K_SH		19
#define		OPCF$K_SI		20
#define		OPCF$K_SJ		21
#define		OPCF$K_SL		22
#define		OPCF$K_SQ		23
#define		OPCF$K_SW		24
#define		OPCF$K_T		25
#define		OPCF$K_U		26
#define		OPCF$K_UA		27
#define		OPCF$K_UB		28
#define		OPCF$K_UH		29
#define		OPCF$K_UI		30
#define		OPCF$K_UJ		31
#define		OPCF$K_UL		32
#define		OPCF$K_UQ		33
#define		OPCF$K_UW		34
#define		OPCF$K_XA		35
#define		OPCF$K_XB		36
#define		OPCF$K_XH		37
#define		OPCF$K_XI		38
#define		OPCF$K_XJ		39
#define		OPCF$K_XL		40
#define		OPCF$K_XQ		41
#define		OPCF$K_XW		42
#define		OPCF$K_ZA		43
#define		OPCF$K_ZB		44
#define		OPCF$K_ZH		45
#define		OPCF$K_ZI		46
#define		OPCF$K_ZJ		47
#define		OPCF$K_ZL		48
#define		OPCF$K_ZQ		49
#define		OPCF$K_ZW		50
#define		OPCF$K_MIN_OPCFAO_CNTRL	1
#define		OPCF$K_MAX_OPCFAO_CNTRL	50
#define		OPCF$K_BY_REF		1
#define		OPCF$K_BY_VAL		2
#define		OPCF$K_BY_DESC		3
		
#endif
		
