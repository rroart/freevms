#ifndef prcdef_h
#define prcdef_h

#define		PRC$M_SSRWAIT		0x1
#define		PRC$M_SSFEXCU		0x2
#define		PRC$M_PSWAPM		0x4
#define		PRC$M_NOACNT		0x8
#define		PRC$M_BATCH		0x10
#define		PRC$M_HIBER		0x20
#define		PRC$M_NOUAF		0x40
#define		PRC$M_NETWRK		0x80
#define		PRC$M_DISAWS		0x100
#define		PRC$M_DETACH		0x200
#define		PRC$M_INTER		0x400
#define		PRC$M_IMGDMP		0x800
#define		PRC$M_CLISPEC		0x1000
#define		PRC$M_NOPASSWORD	0x2000
#define		PRC$M_DEBUG		0x4000
#define		PRC$M_DBGTRU		0x8000
#define		PRC$M_SUBSYSTEM		0x10000
#define		PRC$M_TCB		0x20000
#define		PRC$M_NO_IMAGE_PRIVS	0x40000
#define		PRC$M_PERM_SUBSYSTEM	0x80000
#define		PRC$M_PARSE_EXTENDED	0x100000
#define		PRC$M_INHERIT_PERSONA	0x200000
#define		PRC$M_HOME_RAD		0x400000
#define		PRC$M_LOGIN		0x40
	
#define		PRC$_LISTEND		0
#define		PRC$_PGFLCHAR		1
#define		PRC$_PGFLINDEX		2
#define		PRC$_INPUT_ATT		3
#define		PRC$_OUTPUT_ATT		4
#define		PRC$_ERROR_ATT		5
#define		PRC$_CLASS		6
		
union _prcdef {
  struct {
    unsigned prc$v_ssrwait		: 1;
    unsigned prc$v_ssfexcu		: 1;
    unsigned prc$v_pswapm		: 1;
    unsigned prc$v_noacnt		: 1;
    unsigned prc$v_batch		: 1;
    unsigned prc$v_hiber		: 1;
    unsigned prc$v_nouaf		: 1;
    unsigned prc$v_netwrk		: 1;
    unsigned prc$v_disaws		: 1;
    unsigned prc$v_detach		: 1;
    unsigned prc$v_inter		: 1;
    unsigned prc$v_imgdmp		: 1;
    unsigned prc$v_clispec		: 1;
    unsigned prc$v_nopassword		: 1;
    unsigned prc$v_debug		: 1;
    unsigned prc$v_dbgtru		: 1;
    unsigned prc$v_subsystem		: 1;
    unsigned prc$v_tcb		        : 1;
    unsigned prc$v_no_image_privs	: 1;
    unsigned prc$v_perm_subsystem	: 1;
    unsigned prc$v_parse_extended	: 1;
    unsigned prc$v_inherit_persona	: 1;
    unsigned prc$v_home_rad		: 1;
    unsigned prc$v_fill_me		: 1;
  } ;
  struct  {
    unsigned prc$v_fill_0		: 6;
    unsigned prc$v_login		: 1;
    unsigned prc$v_fill_me_again	: 1;
  };
};
 
#endif
 
