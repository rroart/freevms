#ifndef irpdef_h
#define irpdef_h

#include<cdrpdef.h>

//#include "../../freevms/lib/src/diobmdef.h"

#define	IRP$M_WLE_REUSE	0x1
#define	IRP$M_WLE_SUPWL	0x2
#define	IRP$M_WLE_READ_CONTID	0x4
#define	IRP$M_WLE_WROTE_CONTID	0x8
#define	IRP$M_HIST_LOGGED	0x10
#define	IRP$M_ALLO_FAIL	0x20
#define	IRP$M_HIST_LOST	0x40
#define	IRP$M_TABFU	0x80
#define	IRP$M_WHL_WHLB	0x1
#define	IRP$M_WHL_CNID_REGISTRY	0x2
#define	IRP$M_WHL_ERASE	0x4
#define	IRP$M_WHL_COMMAND	0x8
#define	IRP$M_WHL_04	0x10
#define	IRP$M_WHL_05	0x20
#define	IRP$M_WHL_06	0x40
#define	IRP$M_WHL_07	0x80
#define	IRP$M_BUFIO	0x1
#define	IRP$M_FUNC	0x2
#define	IRP$M_PAGIO	0x4
#define	IRP$M_COMPLX	0x8
#define	IRP$M_VIRTUAL	0x10
#define	IRP$M_CHAINED	0x20
#define	IRP$M_SWAPIO	0x40
#define	IRP$M_DIAGBUF	0x80
#define	IRP$M_PHYSIO	0x100
#define	IRP$M_TERMIO	0x200
#define	IRP$M_MBXIO	0x400
#define	IRP$M_EXTEND	0x800
#define	IRP$M_FILACP	0x1000
#define	IRP$M_MVIRP	0x2000
#define	IRP$M_SRVIO	0x4000
#define	IRP$M_CCB_LOOKED_UP	0x8000
#define	IRP$M_CACHE_PAGIO	0x10000
#define	IRP$M_FILL_BIT	0x20000
#define	IRP$M_BUFOBJ	0x40000
#define	IRP$M_TRUSTED	0x80000
#define	IRP$M_FASTIO_DONE	0x100000
#define	IRP$M_FASTIO	0x200000
#define	IRP$M_FAST_FINISH	0x400000
#define	IRP$M_DOPMS	0x800000
#define	IRP$M_HIFORK	0x1000000
#define	IRP$M_SRV_ABORT	0x2000000
#define	IRP$M_LOCK_RELEASEABLE	0x4000000
#define	IRP$M_DID_FAST_FDT	0x8000000
#define	IRP$M_SYNCSTS	0x10000000
#define	IRP$M_FINIPL8	0x20000000
#define	IRP$M_FILE_FLUSH	0x40000000
#define	IRP$M_BARRIER	0x80000000
#define	IRP$M_START_PAST_HWM	0x1
#define	IRP$M_END_PAST_HWM	0x2
#define	IRP$M_ERASE	0x4
#define	IRP$M_PART_HWM	0x8
#define	IRP$M_LCKIO	0x10
#define	IRP$M_SHDIO	0x20
#define	IRP$M_CACHEIO	0x40
#define	IRP$M_WLE	0x80
#define	IRP$M_CACHE_SAFE	0x100
#define	IRP$M_NOCACHE	0x200
#define	IRP$M_ABORTIO	0x400
#define	IRP$M_FORCEMV	0x800
#define	IRP$M_HBRIO	0x1000
#define	IRP$M_ON_ACT_Q	0x2000
#define	IRP$M_MPDEV_RETRIED	0x4000
#define	IRP$M_WHL	0x8000
#define	IRP$M_QCOMPLEX	0x10000
#define	IRP$M_NORETRY	0x20000
#define	IRP$M_QBARRIER	0x40000
#define	IRP$M_PVIRP	0x80000
#define	IRP$M_USEALTDDT	0x100000
#define	IRP$M_PID_S0_MV	0x200000
#define	IRP$M_CACHE_RESUME	0x400000
#define	IRP$M_QCNTRL	0x800000
#define	IRP$M_QRQT_SRVR_HLPR	0x1000000
#define	IRP$M_QSVD	0x2000000
#define	IRP$M_FCODE	0x3F
#define	IRP$K_CDRP	304
#define	IRP$C_CDRP	304
#define	IRP$M_PIO_ERROR	0x1
#define	IRP$M_PIO_FANOUT	0x2
#define	IRP$M_PIO_NOQUE	0x4
#define	IRP$M_PIO_CANCEL	0x8
#define	IRP$M_PIO_CTHRDOK	0x10
#define	IRP$M_PIO_PHASEII	0x20
#define	IRP$M_PIO_BBR	0x40
#define	IRP$M_SHD_EXPEL_REMOVED	0x1
#define	IRP$M_SHD_RETRY	0x2
#define	IRP$M_CLN_READY	0x1
#define	IRP$M_CLN_DONE	0x2
#define	IRP$M_CPY_FINI	0x4
#define	IRP$M_RTN_ALT_SUCCESS	0x8
#define	IRP$M_WBM_DELETE	0x10
#define	IRP$M_CLN_WHL	0x20
#define	IRP$K_BT_LEN	400
#define	IRP$C_BT_LEN	400
#define	IRP$K_CD_LEN	408
#define	IRP$C_CD_LEN	408
#define	IRP$K_QSRV_HANDSHAKE	1
#define	IRP$K_QSRV_QSLQ	2
#define	IRP$K_QSRV_CANCEL	3
#define	IRP$M_QSRV_STALLED	0x1
#define	IRP$M_QSRV_PERM	0x2
#define	IRP$M_QSRV_CANIO	0x4
#define	IRP$M_QSRV_CAND	0x8
#define	IRP$M_QSRV_COMPLEX_HEAD	0x10
#define	IRP$K_QSRV_INITIAL	0
#define	IRP$K_QSRV_RSPID_ALLOCATED	1
#define	IRP$K_QSRV_MSG_ALLOCATED	2
#define	IRP$K_QSRV_BUFFER_CONFIGURED	3
#define	IRP$K_QSRV_IO_COMPLETED	4
#define	IRP$K_QSRV_IO_RECLAIM	5
#define	IRP$K_QSRV_IO_RESTART	6
#define	IRP$K_QSRV_IO_CANCELED	7
#define	IRP$K_QSRV_IO_ABORTED	8
#define	IRP$S_QSRVR_EXT	456
	
#define	IRP$K_LENGTH	536
#define	IRP$C_LENGTH	536
#define	IRP$S_IRPDEF	536
	
struct	_irp	{
  struct _irp *irp$l_ioqfl;
  struct _irp *irp$l_ioqbl;
  unsigned short int irp$w_size;
  unsigned char irp$b_type;
  unsigned char irp$b_rmod;
  unsigned long irp$l_pid;
  unsigned long irp$l_ast;
  unsigned long irp$l_astprm;
  unsigned long irp$l_wind;
  struct _ucb * irp$l_ucb;
  union {
    unsigned long irp$l_func;
    struct {
      unsigned irp$v_fcode : 6;
      unsigned irp$v_fmod : 10;
    };
  };
  unsigned char irp$b_efn;
  unsigned char irp$b_pri;
  unsigned long irp$l_iosb;
  unsigned short irp$w_chan;
  union {
    unsigned long irp$l_svapte;
    struct _bufio *irp$ps_bufio_pkt;
  };
  /* temporary work-around */ 
  unsigned long useraddress; /* do not know the pte/buffer stuff yet */
  unsigned long irp$l_boff;
  unsigned short irp$w_empty;
  unsigned long irp$l_bcnt;
  unsigned short irp$w_emptyagain;
  union {
    unsigned long irp$l_iost1;
    long irp$l_media;
  };
  unsigned long irp$l_iost2;
  unsigned long irp$l_abcnt;
  unsigned long irp$l_obcnt;
  unsigned long irp$l_segvbn;
  unsigned long irp$l_diagbuf;
  unsigned long irp$l_seqnum;
  unsigned long irp$l_extend;
  unsigned long irp$l_arb;
  unsigned long irp$l_keydesc;
  struct _ccb *irp$ps_ccb;
  unsigned long irp$l_qio_p1;
  unsigned long irp$l_qio_p2;
  unsigned long irp$l_qio_p3;
  unsigned long irp$l_qio_p4;
  unsigned long irp$l_qio_p5;
  unsigned long irp$l_qio_p6;

  union {
    unsigned long long irp$q_status;
    struct {
      union {
	unsigned int irp$l_sts;
	struct {
	  unsigned irp$v_bufio : 1;
	  unsigned irp$v_func : 1;
	  unsigned irp$v_pagio : 1;
	  unsigned irp$v_complx : 1;
	  unsigned irp$v_virtual : 1;
	  unsigned irp$v_chained : 1;
	  unsigned irp$v_swapio : 1;
	  unsigned irp$v_diagbuf : 1;
	  unsigned irp$v_physio : 1;
	  unsigned irp$v_termio : 1;
	  unsigned irp$v_mbxio : 1;
	  unsigned irp$v_extend : 1;
	  unsigned irp$v_filacp : 1;
	  unsigned irp$v_mvirp : 1;
	  unsigned irp$v_srvio : 1;
	  unsigned irp$v_ccb_looked_up : 1;
	  unsigned irp$v_cache_pagio : 1;
	  unsigned irp$v_fill_bit : 1;
	  unsigned irp$v_bufobj : 1;
	  unsigned irp$v_trusted : 1;
	  unsigned irp$v_fastio_done : 1;
	  unsigned irp$v_fastio : 1;
	  unsigned irp$v_fast_finish : 1;
	  unsigned irp$v_dopms : 1;
	  unsigned irp$v_hifork : 1;
	  unsigned irp$v_srv_abort : 1;
	  unsigned irp$v_lock_releaseable : 1;
	  unsigned irp$v_did_fast_fdt : 1;
	  unsigned irp$v_syncsts : 1;
	  unsigned irp$v_finipl8 : 1;
	  unsigned irp$v_file_flush : 1;
	  unsigned irp$v_barrier : 1;
	};
      };
      union {
	unsigned int irp$l_sts2;
	struct {
	  unsigned irp$v_start_past_hwm : 1;
	  unsigned irp$v_end_past_hwm : 1;
	  unsigned irp$v_erase : 1;
	  unsigned irp$v_part_hwm : 1;
	  unsigned irp$v_lckio : 1;
	  unsigned irp$v_shdio : 1;
	  unsigned irp$v_cacheio : 1;
	  unsigned irp$v_wle : 1;
	  unsigned irp$v_cache_safe : 1;
	  unsigned irp$v_nocache : 1;
	  unsigned irp$v_abortio : 1;
	  unsigned irp$v_forcemv : 1;
	  unsigned irp$v_hbrio : 1;
	  unsigned irp$v_on_act_q : 1;
	  unsigned irp$v_mpdev_retried : 1;
	  unsigned irp$v_whl : 1;
	  unsigned irp$v_qcomplex : 1;
	  unsigned irp$v_noretry : 1;
	  unsigned irp$v_qbarrier : 1;
	  unsigned irp$v_pvirp : 1;
	  unsigned irp$v_usealtddt : 1;
	  unsigned irp$v_pid_s0_mv : 1;
	  unsigned irp$v_cache_resume : 1;
	  unsigned irp$v_qcntrl : 1;
	  unsigned irp$v_qrqt_srvr_hlpr : 1;
	  unsigned irp$v_qsvd : 1;
	  unsigned irp$v_fill_23_ : 6;
	};
      };
    };
  };

  /* copy from cdrp to this area ?*/
  struct _cdrp irp_cdrp;

  // put at back to avoid recompile this time
  // sort in right later

  unsigned long long irp$q_tt_state;

};

#define irp$l_fqfl irp_cdrp.cdrp$l_fqfl
#define irp$l_fqbl irp_cdrp.cdrp$l_fqbl
#define irp$w_cdrpsize irp_cdrp.cdrp$w_cdrpsize
#define irp$b_cd_type irp_cdrp.cdrp$b_cd_type
#define irp$b_flck irp_cdrp.cdrp$b_flck
#define irp$l_fpc irp_cdrp.cdrp$l_fpc
#define irp$l_fr3 irp_cdrp.cdrp$l_fr3
#define irp$l_fr4 irp_cdrp.cdrp$l_fr4
#define irp$l_savd_rtn irp_cdrp.cdrp$l_savd_rtn
#define irp$l_msg_buf irp_cdrp.cdrp$l_msg_buf
#define irp$l_rspid irp_cdrp.cdrp$l_rspid
#define irp$l_cdt irp_cdrp.cdrp$l_cdt
#define irp$l_rwcptr irp_cdrp.cdrp$l_rwcptr
#define irp$l_lbufh_ad irp_cdrp.cdrp$l_lbufh_ad
#define irp$l_lboff irp_cdrp.cdrp$l_lboff
#define irp$l_rbufh_ad irp_cdrp.cdrp$l_rbufh_ad
#define irp$l_rboff irp_cdrp.cdrp$l_rboff
#define irp$l_xct_len irp_cdrp.cdrp$l_xct_len
#define irp$l_something irp_cdrp.cdrp$l_something
#define irp$t_lbufhndl irp_cdrp.cdrp$t_lbufhndl
#define irp$l_ubarsrce irp_cdrp.cdrp$l_ubarsrce
#define irp$l_dutuflags irp_cdrp.cdrp$l_dutuflags
#define irp$w_dutucntr irp_cdrp.cdrp$w_dutucntr
#define irp$w_endmsgsiz irp_cdrp.cdrp$w_endmsgsiz

#endif
