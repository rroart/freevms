#ifndef ddtdef_h
#define ddtdef_h

#define DDT$M_DIAGBUF64 32768
#define DDT$K_ITCLVL_DRVR 0
#define DDT$K_ITCLVL_MPDEV 4096
#define DDT$K_ITCLVL_HSM 24576
#define DDT$K_ITCLVL_TOP 32767
#define DDT$K_LENGTH_MIN 120
	
#define DDT$K_LENGTH 152
#define DDT$C_LENGTH 152
#define DDT$S_DDTDEF 152

struct _ddt {
  unsigned short int ddt$w_size;
  unsigned char ddt$b_type;
  unsigned char ddt$b_subtype;
  short int ddt$w_intercept_level;
  unsigned long ddt$l_start;
  unsigned long ddt$l_unsolint;
  unsigned long ddt$l_fdt;
  unsigned long ddt$l_cancel;
  unsigned long ddt$l_regdump;
  unsigned short int ddt$w_diagbuf;
  unsigned short int ddt$w_errorbuf;
  unsigned long ddt$l_unitinit;
  unsigned long ddt$l_altstart;
  unsigned long ddt$l_mntver;
  unsigned long ddt$l_cloneducb;
  unsigned short int ddt$w_fdtsize;
  unsigned short int ddt$w_reserved;
  void (*ddt$ps_start_2)();
  void (*ddt$ps_start_jsb)();
  int (*ddt$ps_ctrlinit_2)();
  int (*ddt$ps_unitinit_2)();
  int (*ddt$ps_cloneducb_2)();
  struct _fdt *ddt$ps_fdt_2;
  void (*ddt$ps_cancel_2)();
  void (*ddt$ps_regdump_2)();
  void (*ddt$ps_altstart_2)();
  void (*ddt$ps_altstart_jsb)();
  void (*ddt$ps_mntver_2)();
  int ddt$l_mntv_sssc;
  int ddt$l_mntv_for;
  int ddt$l_mntv_sqd;
  int ddt$l_aux_storage;
  int ddt$l_aux_routine;
  void (*ddt$ps_channel_assign_2)();
  int (*ddt$ps_cancel_selective_2)();
  unsigned int ddt$is_stack_bcnt;
  unsigned int ddt$is_reg_mask;
  void (*ddt$ps_kp_startio)();
  int (*ddt$ps_csr_mapping)();
  int (*ddt$ps_fast_fdt)();
  int (*ddt$ps_pending_io)();
  void *ddt$ps_customer;
  int (*ddt$ps_make_devpath)();
  int (*ddt$ps_setprfpath)();
  int (*ddt$ps_change_preferred)();
  int (*ddt$ps_qsrv_helper)();
  int (*ddt$ps_qsrv_evnt_notify)();
  union  {
    int (*ddt$ps_mgt_register)();
    int (*ddt$ps_configure)();
  } ddt$r_config_overlay;
  union  {
    int (*ddt$ps_mgt_deregister)();
    int (*ddt$ps_deconfigure)();
  } ddt$r_deconfig_overlay;
};
 
#endif
 
