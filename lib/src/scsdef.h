#ifndef scsdef_h
#define scsdef_h
	
#define SCS$C_OVHD 14
#define SCS$C_CON_REQL 66
#define SCS$C_CON_RSPL 22
#define SCS$C_ACCP_REQL 66
#define SCS$C_ACCP_RSPL 18
#define SCS$C_REJ_REQL 18
#define SCS$C_REJ_RSPL 14
#define SCS$C_DISC_REQL 18
#define SCS$C_DISC_RSPL 14
#define SCS$C_CR_REQL 14
#define SCS$C_CR_RSPL 14
#define SCS$C_CON_REQ 0
#define SCS$C_CON_RSP 1
#define SCS$C_ACCP_REQ 2
#define SCS$C_ACCP_RSP 3
#define SCS$C_REJ_REQ 4
#define SCS$C_REJ_RSP 5
#define SCS$C_DISC_REQ 6
#define SCS$C_DISC_RSP 7
#define SCS$C_CR_REQ 8
#define SCS$C_CR_RSP 9
#define SCS$C_APPL_MSG 10
#define SCS$C_APPL_DG 11
#define SCS$K_APPL_BASE 0
#define SCS$C_APPL_BASE 0
#define SCS$M_UAP 0x1
#define SCS$K_STNORMAL 1
#define SCS$C_STNORMAL 1
#define SCS$K_STNOMAT 10
#define SCS$C_STNOMAT 10
#define SCS$K_STNORS 18
#define SCS$C_STNORS 18
#define SCS$K_STDISC 25
#define SCS$C_STDISC 25
#define SCS$K_STINSFCR 33
#define SCS$C_STINSFCR 33
#define SCS$K_STBALANCE 41
#define SCS$C_STBALANCE 41
#define SCS$K_USE_ALTERNATE_PORT 42
#define SCS$C_USE_ALTERNATE_PORT 42
#define SCS$K_CON_BASE 4
#define SCS$C_CON_BASE 4

#define SCS$S_SCSDEF 84
	
struct _scs {
  unsigned short int scs$w_min_cr;
  unsigned short int scs$w_status;
  char scs$t_dst_proc [16];
  unsigned char scs$b_subnode;
  unsigned char scs$b_pgrp;
  unsigned short int scs$w_rsv;
  char scs$t_src_proc [16];
  unsigned char scs$b_con_dat [16];
};

#define SCS$S_SCSDEF1 28
	
struct _scs1 {
  unsigned int scs$l_snd_boff;
  unsigned int scs$l_rec_name;
  unsigned int scs$l_rec_boff;
} ;
 
 
#endif
 
