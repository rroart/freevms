#ifndef iogendef_h
#define iogendef_h

#define IOGEN$_LOAD 1
#define IOGEN$_RELOAD 2
#define IOGEN$_CONNECT 3
#define IOGEN$_INIT_CTRL 4
#define IOGEN$_INIT_UNIT 5
#define IOGEN$_DELIVER 6
#define IOGEN$K_MINFCODE 1
#define IOGEN$K_MAXFCODE 6
#define IOGEN$M_NOWAIT 0x10000
#define IOGEN$M_LDDB 0x20000
#define IOGEN$M_SYSDEVICE 0x40000
#define IOGEN$M_NOINIT 0x80000
#define IOGEN$S_IOGENDEF 4
	
typedef struct _iogen {
  union  {
    struct  {
      unsigned short int iogen$w_fcode;
      unsigned short int iogen$w_modifiers;
    };
    struct  {
      unsigned iogen$v_fcode_fill : 16;
      unsigned iogen$v_nowait : 1;
      unsigned iogen$v_lddb : 1;
      unsigned iogen$v_sysdevice : 1;
      unsigned iogen$v_noinit : 1;
      unsigned iogen$v_fill_0_ : 4;
    };
  };
};
 
#define IOGEN$_ADAPTER 16
#define IOGEN$_NOADAPTER 17
#define IOGEN$_CSR 18
#define IOGEN$_VECTOR 19
#define IOGEN$_MAXUNITS 20
#define IOGEN$_SYSID 21
#define IOGEN$_SYSLOA_CRB 22
#define IOGEN$_UNIT 23
#define IOGEN$_NUMUNITS 24
#define IOGEN$_DELIVER_DATA 25
#define IOGEN$_DDB 26
#define IOGEN$_CRB 27
#define IOGEN$_IDB 28
#define IOGEN$_UCB 29
#define IOGEN$_SB 30
#define IOGEN$_NODE 31
#define IOGEN$_ALLOCLS 32
#define IOGEN$_WWID 33
#define IOGEN$_DEVPATH 34
#define IOGEN$_DNP 35
#define IOGEN$_QIOSERVER 36
#define IOGEN$_QSLUN 37
#define IOGEN$_QSRV_PATH_PRIORITY 38
#define IOGEN$_QSRV_DEVICE_ALLOCLS 39
#define IOGEN$K_MINITEM 16
#define IOGEN$K_MAXITEM 39
#define IOGEN$M_AC_LOG 0x1
#define IOGEN$M_AC_SCA 0x2
#define IOGEN$M_AC_LAN 0x4
#define IOGEN$M_AC_LOG_ALL 0x8
#define IOGEN$M_AC_VERIFY 0x10
	
struct _autocfg {
  union  {
    unsigned int iogen$l_autocfg_flags;
    struct  {
      unsigned iogen$v_ac_log : 1;
      unsigned iogen$v_ac_sca : 1;
      unsigned iogen$v_ac_lan : 1;
      unsigned iogen$v_ac_log_all : 1;
      unsigned iogen$v_ac_verify : 1;
      unsigned iogen$v_fill_1_ : 3;
    };
  };
};
 
#define IOGEN$S_ABMDEF 8
	
struct _abm {
  int iogen$il_abm_adp;
  int (*iogen$ps_abm_bsr)();
};
 
#endif
 
