#ifndef prvdef_h
#define prvdef_h

#define		PRV$M_CMKRNL		0x1
#define		PRV$M_CMEXEC		0x2
#define		PRV$M_SYSNAM		0x4
#define		PRV$M_GRPNAM		0x8
#define		PRV$M_ALLSPOOL		0x10
#define		PRV$M_IMPERSONATE		0x20
#define		PRV$M_DIAGNOSE		0x40
#define		PRV$M_LOG_IO		0x80
#define		PRV$M_GROUP		0x100
#define		PRV$M_NOACNT		0x200
#define		PRV$M_PRMCEB		0x400
#define		PRV$M_PRMMBX		0x800
#define		PRV$M_PSWAPM		0x1000
#define		PRV$M_SETPRI		0x2000
#define		PRV$M_SETPRV		0x4000
#define		PRV$M_TMPMBX		0x8000
#define		PRV$M_WORLD		0x10000
#define		PRV$M_MOUNT		0x20000
#define		PRV$M_OPER		0x40000
#define		PRV$M_EXQUOTA		0x80000
#define		PRV$M_NETMBX		0x100000
#define		PRV$M_VOLPRO		0x200000
#define		PRV$M_PHY_IO		0x400000
#define		PRV$M_BUGCHK		0x800000
#define		PRV$M_PRMGBL		0x1000000
#define		PRV$M_SYSGBL		0x2000000
#define		PRV$M_PFNMAP		0x4000000
#define		PRV$M_SHMEM		0x8000000
#define		PRV$M_SYSPRV		0x10000000
#define		PRV$M_BYPASS		0x20000000
#define		PRV$M_SYSLCK		0x40000000
#define		PRV$M_SHARE		0x80000000
#define		PRV$M_UPGRADE		0x100000000
#define		PRV$M_DOWNGRADE		0x200000000
#define		PRV$M_GRPPRV		0x400000000
#define		PRV$M_READALL		0x800000000
#define		PRV$M_IMPORT		0x1000000000
#define		PRV$M_AUDIT		0x2000000000
#define		PRV$M_SECURITY		0x4000000000
#define		PRV$K_NUMBER_OF_PRIVS		39	

#define		PRV$M_ACNT		0x200
#define		PRV$M_ALTPRI		0x2000
#define		PRV$M_DETACH		0x20
	
#define		PRV$K_PRVMASK_WORKING		0
#define		PRV$K_PRVMASK_PERMANENT		1
#define		PRV$K_PRVMASK_IMAGE		2
 
#define		PRV$M_SORTED		0x1
#define		PRV$M_BRIEF		0x2
#define		PRV$M_FILLED		0x4
	
union _prvdef {
  struct  {
    unsigned prv$v_cmkrnl		: 1;
    unsigned prv$v_cmexec		: 1;
    unsigned prv$v_sysnam		: 1;
    unsigned prv$v_grpnam		: 1;
    unsigned prv$v_allspool		: 1;
    unsigned prv$v_impersonate		: 1;
    unsigned prv$v_diagnose		: 1;
    unsigned prv$v_log_io		: 1;
    unsigned prv$v_group		: 1;
    unsigned prv$v_noacnt		: 1;
    unsigned prv$v_prmceb		: 1;
    unsigned prv$v_prmmbx		: 1;
    unsigned prv$v_pswapm		: 1;
    unsigned prv$v_setpri		: 1;
    unsigned prv$v_setprv		: 1;
    unsigned prv$v_tmpmbx		: 1;
    unsigned prv$v_world		: 1;
    unsigned prv$v_mount		: 1;
    unsigned prv$v_oper		        : 1;
    unsigned prv$v_exquota		: 1;
    unsigned prv$v_netmbx		: 1;
    unsigned prv$v_volpro		: 1;
    unsigned prv$v_phy_io		: 1;
    unsigned prv$v_bugchk		: 1;
    unsigned prv$v_prmgbl		: 1;
    unsigned prv$v_sysgbl		: 1;
    unsigned prv$v_pfnmap		: 1;
    unsigned prv$v_shmem		: 1;
    unsigned prv$v_sysprv		: 1;
    unsigned prv$v_bypass		: 1;
    unsigned prv$v_syslck		: 1;
    unsigned prv$v_share		: 1;
    unsigned prv$v_upgrade		: 1;
    unsigned prv$v_downgrade		: 1;
    unsigned prv$v_grpprv		: 1;
    unsigned prv$v_readall		: 1;
    unsigned prv$v_import		: 1;
    unsigned prv$v_audit		: 1;
    unsigned prv$v_security		: 1;

    unsigned prv$v_fill_1		: 25;
  };

  struct  {
    unsigned prv$v_fill_2		: 9;
    unsigned prv$v_acnt		        : 1;
    unsigned prv$v_fill_3		: 3;
    unsigned prv$v_altpri		: 1;
    unsigned prv$v_fill_69_		: 2;
  };
  struct  {
    unsigned int prv$l_l1_bits;
    unsigned int prv$l_l2_bits;
  };
  struct  {
    unsigned prv$v_fill_4		: 5;
    unsigned prv$v_detach		: 1;
    unsigned prv$v_fill_70_		: 2;
  };
};
 
struct _prvdsp_bits {
  unsigned prv$v_sorted		: 1;
  unsigned prv$v_brief		: 1;
  unsigned prv$v_filled		: 1;
  unsigned prv$v_fill_71_	: 5;
};

#endif
 
