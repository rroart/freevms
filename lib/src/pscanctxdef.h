#ifndef pscanctxdef_h
#define pscanctxdef_h

#define		PSCANCTX$M_SUPER		0x1
#define		PSCANCTX$M_CSID_VEC		0x2
#define		PSCANCTX$M_LOCKED		0x4
#define		PSCANCTX$M_MULTI_NODE		0x8
#define		PSCANCTX$M_BUSY		        0x10
#define		PSCANCTX$M_RELOCK		0x20
#define		PSCANCTX$M_THREAD		0x40
#define		PSCANCTX$M_NEED_THREAD_CAP	0x80
#define		PSCANCTX$M_SCHED_CLASS_CAP	0x100
#define		PSCANCTX$K_LENGTH		64
#define		PSCANCTX$M_THREAD_ITEM		1073741824
#define		PSCANCTX$V_THREAD_ITEM		30
#define		PSCANCTX$M_NODE_ITEM		-2147483648
#define		PSCANCTX$V_NODE_ITEM		31
#define		PSCANCTX$S_$PSCANCTXDEF		64
	
#define		PSCANBUF$M_SPARE0		0x1
#define		PSCANBUF$K_LENGTH		32
#define		PSCANBUF$S_$PSCANBUFDEF		32
	
#define		PSCANITM$K_LENGTH		12
#define		PSCANITM$S_$PSCANITMDEF		13
 
struct _pscanctx {
  void *pscanctx$l_flink;
  void *pscanctx$l_blink;
  unsigned short int pscanctx$w_size;
  unsigned char pscanctx$b_type;
  unsigned char pscanctx$b_subtype;
  unsigned short int pscanctx$w_maj_vers;
  unsigned short int pscanctx$w_min_vers;
  union  {
    unsigned int pscanctx$l_flags;
    struct  {
      unsigned pscanctx$v_super : 1;
      unsigned pscanctx$v_csid_vec : 1;
      unsigned pscanctx$v_locked : 1;
      unsigned pscanctx$v_multi_node : 1;
      unsigned pscanctx$v_busy : 1;
      unsigned pscanctx$v_relock : 1;
      unsigned pscanctx$v_thread : 1;
      unsigned pscanctx$v_need_thread_cap : 1;
      unsigned pscanctx$v_sched_class_cap : 1;
      unsigned pscanctx$v_fill_2_ : 7;
    };
  };
  unsigned int pscanctx$l_cur_csid;
  unsigned int pscanctx$l_cur_ipid;
  unsigned int pscanctx$l_next_ipid;
  unsigned int pscanctx$l_cur_epid;
  unsigned short int pscanctx$w_itmlstoff;
  unsigned short int pscanctx$w_bufferoff;
  unsigned short int pscanctx$w_csidoff;
  unsigned short int pscanctx$w_csididx;
  unsigned int pscanctx$l_svapte;
  struct _cwpssrv *pscanctx$l_cwpssrv;
  void *pscanctx$l_jpibufadr;
  unsigned short int pscanctx$w_seqnum;
  unsigned char pscanctx$b_acmode;
  unsigned char pscanctx$b_spare0;
  unsigned int pscanctx$l_spare1;
};
	
struct _pscanbuf {
  unsigned int pscanbuf$l_buflen;
  unsigned int pscanbuf$l_spare0;
  unsigned short int pscanbuf$w_size;
  unsigned char pscanbuf$b_type;
  unsigned char pscanbuf$b_subtype;
  unsigned short int pscanbuf$w_maj_vers;
  unsigned short int pscanbuf$w_min_vers;
  union  {
    unsigned int pscanbuf$l_flags;
    struct  {
      unsigned pscanbuf$v_spare0  : 1;
      unsigned pscanbuf$v_fill_5_ : 7;
    };
  };
  void *pscanbuf$l_itmlstadr;
  unsigned int pscanbuf$l_buffer_offset;
  unsigned int pscanbuf$l_free_offset;
};
 
struct _pscanitm {
  unsigned int pscanitm$l_alloc_length;
  unsigned int pscanitm$l_itmlen;
  unsigned short int pscanitm$w_size;
  unsigned char pscanitm$b_type;
  unsigned char pscanitm$b_subtype;
  unsigned char pscanitm$b_itmlst;
};
 
#endif
 
