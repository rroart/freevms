#ifndef cdrpdef_h
#define cdrpdef_h

//#include "../../freevms/lib/src/diobmdef.h"

#define CDRP$M_WLE_REUSE 0x1
#define CDRP$M_WLE_SUPWL 0x2
#define CDRP$K_CDRPBASE 0
#define CDRP$C_CDRPBASE 0
#define CDRP$C_RSPID_WAIT 1
#define CDRP$C_POOL_WAIT 2
#define CDRP$C_CREDIT_WAIT 3
#define CDRP$C_BD_WAIT 4
#define CDRP$M_SYSAP_STALLED 0x1
#define CDRP$M_RBUN_WANTED 0x2
#define CDRP$K_LENGTH 80
#define CDRP$C_LENGTH 80

#define CDRP$K_BT_LEN 96
#define CDRP$C_BT_LEN 96
#define CDRP$M_CAND 0x1
#define CDRP$M_CANIO 0x2
#define CDRP$M_ERLIP 0x4
#define CDRP$M_PERM 0x8
#define CDRP$M_HIRT 0x10
#define CDRP$M_DENSCK 0x20
#define CDRP$M_CONNWALK 0x40
#define CDRP$M_COPYSHAD 0x80
#define CDRP$M_IVCMD 0x100
#define CDRP$M_WALK_2P 0x200
#define CDRP$M_LOC_ONLY 0x400
#define CDRP$M_LOADBAL 0x800
#define CDRP$K_CD_LEN 104
#define CDRP$C_CD_LEN 104
#define CDRP$K_NORMAL 0
#define CDRP$K_REQUESTOR 1
#define CDRP$K_PARTNER 2
#define CDRP$K_PART_IDLE 3
#define CDRP$K_REQ_MAP 4
#define CDRP$K_PART_MAP 5
#define CDRP$K_PART_FORK_WAIT 6
#define CDRP$K_CNX_MSG 0
#define CDRP$K_CNX_BLKRD 1
#define CDRP$K_CNX_BLKWRT 2
#define CDRP$K_CNX_REQ 3
#define CDRP$M_HAVE_SYNCH 0x1
#define CDRP$M_MSGBLD_SYNCH 0x2
#define CDRP$M_CPL_SYNCH 0x4
#define CDRP$M_STALL_SYNCH 0x8
#define CDRP$M_RM_BLKRD_DONE 0x10
#define CDRP$M_RM_XFRPROC_DONE 0x20
#define CDRP$K_CM_LENGTH 176
#define CDRP$K_VCNX_NORMAL 0
#define CDRP$K_VCNX_REQUESTOR 1
#define CDRP$K_VCNX_PARTNER 2
#define CDRP$K_VCNX_PART_IDLE 3
#define CDRP$K_VCNX_REQ_MAP 4
#define CDRP$K_VCNX_PART_MAP 5
#define CDRP$M_CDRP_PARTNER_VALID 0x1
#define CDRP$M_XMT_CDRP_BLKXFR 0x2
#define CDRP$M_XMT_REQ_SUCCESS 0x4
#define CDRP$M_XMT_XFER_DONE 0x8
#define CDRP$M_PARTNER_ABORT 0x10
#define CDRP$M_XMT_NOTIFIED 0x20
#define CDRP$M_XMT_SEGMENT 0x40
#define CDRP$M_XMT_MUX_MSG 0x80
#define CDRP$K_VCNX_MSG 0
#define CDRP$K_VCNX_BLKRD 1
#define CDRP$K_VCNX_BLKWRT 2
#define CDRP$K_VCNX_REQ 3
#define CDRP$K_SCATP_LENGTH 168
#define CDRP$K_IDLE 0
#define CDRP$K_ALLOC_MSG_BUF 1
#define CDRP$K_RECYCLE_MSG_BUF 2
#define CDRP$K_MAP 3
#define CDRP$K_SEND_DATA 4
#define CDRP$K_SEND_MSG 5
#define CDRP$K_ALLOC_RSPID 6
#define CDRP$K_SDA_LEN 160
#define CDRP$C_SDA_LEN 160
#define CDRP$K_QSRV_HANDSHAKE 1
#define CDRP$K_QSRV_QSLQ 2
#define CDRP$K_QSRV_CANCEL 3
#define CDRP$M_QSRV_STALLED 0x1
#define CDRP$M_QSRV_PERM 0x2
#define CDRP$M_QSRV_CANIO 0x4
#define CDRP$M_QSRV_CAND 0x8
#define CDRP$M_QSRV_COMPLEX_HEAD 0x10
#define CDRP$K_QSRV_INITIAL 0
#define CDRP$K_QSRV_RSPID_ALLOCATED 1
#define CDRP$K_QSRV_MSG_ALLOCATED 2
#define CDRP$K_QSRV_BUFFER_CONFIGURED 3
#define CDRP$K_QSRV_IO_COMPLETED 4
#define CDRP$K_QSRV_IO_RECLAIM 5
#define CDRP$K_QSRV_IO_RESTART 6
#define CDRP$K_QSRV_IO_CANCELED 7
#define CDRP$K_QSRV_IO_ABORTED 8
#define CDRP$S_QSRVR_EXT 152
	
struct _cdrp {
  struct _cdrp *cdrp$l_fqfl;
  struct _cdrp *cdrp$l_fqbl;
  unsigned short int cdrp$w_cdrpsize;
  unsigned char cdrp$b_cd_type;
  unsigned char cdrp$b_flck;
  void (*cdrp$l_fpc)();
  unsigned long cdrp$q_fr3;
  unsigned long cdrp$q_fr4;
  void (*cdrp$l_savd_rtn)();
  void *cdrp$l_msg_buf;
  unsigned int cdrp$l_rspid;
  struct _cdt *cdrp$l_cdt;
  short int *cdrp$l_rwcptr;

  union {
    struct {
      void *cdrp$l_lbufh_ad;
      unsigned int cdrp$l_lboff;
      void *cdrp$l_rbufh_ad;
      unsigned int cdrp$l_rboff;
      unsigned int cdrp$l_xct_len;
    };
   struct  {
      unsigned long something;
      char cdrp$t_lbufhndl [12];
      unsigned int cdrp$l_ubarsrce;
      unsigned int cdrp$l_dutuflags;
      unsigned short int cdrp$w_dutucntr;
      unsigned short int cdrp$w_endmsgsiz;
   };
  }; 
};
 
#define CDRP$S_CDRPDEF 480
 
#endif
 
