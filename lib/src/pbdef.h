#ifndef pbdef_h
#define pbdef_h
	
#define PB$C_CLOSED 0
#define PB$C_ST_SENT 1
#define PB$C_ST_REC 2
#define PB$C_OPEN 3

#define PB$C_STALL_SETCKT 4
#define PB$C_CLOSE_CKT 5
#define PB$C_NOTIFY_VCFAIL 6
#define PB$C_STALL_CACHE 7
#define PB$C_CACHE_CLEAR 8
#define PB$C_NOTIFY_PWFAIL 9

#define PB$C_VC_FAIL 32768
#define PB$C_PWR_FAIL 16384
#define PB$M_DUALPATH 0x80000000
#define PB$C_CI780 2
#define PB$C_CI750 2
#define PB$C_HSC 4
#define PB$C_KL10 6
#define PB$C_CINT 7
#define PB$C_NI 8
#define PB$C_PS 9
#define PB$C_BCA 11
#define PB$C_BVPSSP 12
#define PB$C_BVPNI 13
#define PB$C_CIXCD 14
#define PB$C_CIXCDAC 16
#define PB$C_CITCA 17
#define PB$C_CIPCA 18
#define PB$C_MC 19
#define PB$C_SMCI 20
#define PB$C_SII 32
#define PB$C_KFQSA 33
#define PB$C_SHAC 34
#define PB$C_XON 35
#define PB$C_SWIFT 36
#define PB$C_KFMSA 37
#define PB$C_N710 38
#define PB$C_KFMSB 39
#define PB$C_RF70 48
#define PB$C_RF71 48
#define PB$C_RF30 49
#define PB$C_RF31 50
#define PB$C_RF72 51
#define PB$C_RF32 52
#define PB$C_RF73 53
#define PB$C_RF31F 54
#define PB$C_RF35 55
#define PB$C_RF36 58
#define PB$C_RF37 59
#define PB$C_RF74 60
#define PB$C_RF75 61
#define PB$C_TF70 64
#define PB$C_TF30 65
#define PB$C_TF85 65
#define PB$C_TF86 66
#define PB$C_HSJ 80
#define PB$C_HSD 81
#define PB$C_HSF 82
#define PB$C_HSJ80 83
#define PB$C_EF51 96
#define PB$C_EF52 97
#define PB$C_EF53 98
#define PB$C_EF54 99
#define PB$C_EF58 100
#define PB$M_SRSNTDATWM 0x80
#define PB$M_MAINT 0x1
#define PB$C_UNINIT 0
#define PB$C_DISAB 1
#define PB$C_ENAB 2

#define PB$M_CUR_CBL 0x1
#define PB$M_CUR_PS 0x1
#define PB$M_TIM 0x1
#define PB$M_VCCHK_ENB 0x2
#define PB$M_SCS_EXP 0x4
#define PB$M_NEW_MSG 0x8
#define PB$M_UNUSED 0x10
#define PB$M_CREDIT 0x20
#define PB$M_DISC 0x40
#define PB$M_STORAGE 0x80
#define PB$M_CLONE 0x100
#define PB$C_SMCI_LOAD_CLASS 2147483647
#define PB$C_MC_LOAD_CLASS 800
#define PB$C_CI_LOAD_CLASS 140
#define PB$C_DSSI_LOAD_CLASS 48
#define PB$C_NI_LOAD_CLASS 10
#define PB$K_LENGTH 116
#define PB$C_LENGTH 116
#define PB$S_PBDEF 116
	
 
#ifdef __cplusplus		
struct _sb;
struct _cdt;
#endif		
 
typedef struct _pb {
  struct _pb *pb$l_flink;
  struct _pb *pb$l_blink;
  unsigned short int pb$w_size;
  unsigned char pb$b_type;
  unsigned char pb$b_subtyp;
  unsigned char pb$b_rstation [6];
  unsigned short int pb$w_state;
  unsigned int pb$l_rport_typ;
  unsigned int pb$l_rport_rev;
  unsigned int pb$l_rport_fcn;
  unsigned char pb$b_rst_port;
  unsigned char pb$b_rstate;
  unsigned short int pb$w_retry;
  char pb$t_lport_name [4];
  unsigned char pb$b_cbl_sts;
  unsigned char pb$b_p0_sts;
  unsigned char pb$b_p1_sts;
  char pb$$_fill_1;
  void *pb$l_pdt;
  struct _sb *pb$l_sblink;
  struct _cdt *pb$l_cdtlst;
  void *pb$l_waitqfl;
  union  {
    void *pb$l_waitqbl;
    unsigned int pb$l_duetime;
  } waitqbloverlay;
  void *pb$l_scsmsg;
  unsigned short int pb$w_sts;
  unsigned short int pb$w_vcfail_rsn;
  unsigned char pb$b_protocol;
  char pb$$_fill_2 [3];
  unsigned int pb$l_rport_mult;
  unsigned int pb$l_time_stamp;
  struct _pb *pb$l_share_flink;
  struct _pb *pb$l_share_blink;
  int pb$l_load_class;
  int pb$l_priority;
  int pb$l_mgt_priority;
  unsigned int pb$l_vc_addr;
  int pb$$_fill_3 [2];
} ;
 
#endif
 
