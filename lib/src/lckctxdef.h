#ifndef lckctxdef_h
#define lckctxdef_h

#define LCKCTX$M_BUSY 0x1
#define LCKCTX$M_CANCEL 0x2
#define LCKCTX$M_CMP_RQD 0x4
#define LCKCTX$K_LEN 200
#define LCKCTX$C_LEN 200
#define LCKCTX$S_LCKCTXDEF 200
	
struct _lckctx {
  struct  {
    void *lckctx$l_fqfl;
    void *lckctx$l_fqbl;
    unsigned short int lckctx$w_size;
    unsigned char lckctx$b_type;
    unsigned char lckctx$b_flck;
    void (*lckctx$l_fpc)(void);
    unsigned long lckctx$l_fr3;
    unsigned long lckctx$l_fr4;
  }; /* fkb */
  union  {
    unsigned int lckctx$l_flags;
    struct  {
      unsigned lckctx$v_busy : 1;
      unsigned lckctx$v_cancel : 1;
      unsigned lckctx$v_cmp_rqd : 1;
      unsigned lckctx$v_fill_0_ : 5;
    } ;
  } ;
  char lckctx$b_fill_1_ [4];
  struct _lkb *lckctx$q_lkb;
  union  {
    struct  {
      unsigned long long lckctx$q_cr3;
      unsigned long long lckctx$q_cr4;
      unsigned long long lckctx$q_cr5;
      void *lckctx$pq_ret1;
      unsigned long long lckctx$q_tmp1;
    };
    struct  {
      void *lckctx$pq_cpladr;
      unsigned long long lckctx$q_cplprm;
    };
  };
  union  {
    char lckctx$b_args [112];
    struct  {
      unsigned long long lckctx$q_enq_lock_acmode;
      unsigned long long lckctx$q_enq_lkmode;
      unsigned long long lckctx$q_enq_lksb;
      unsigned long long lckctx$q_enq_flags;
      unsigned long long lckctx$q_enq_resnam;
      unsigned long long lckctx$q_enq_parid;
      unsigned long long lckctx$q_enq_cmp_adr;
      unsigned long long lckctx$q_enq_ctx_prm1;
      unsigned long long lckctx$q_enq_ctx_prm2;
      unsigned long long lckctx$q_enq_ctx_prm3;
      unsigned long long lckctx$q_enq_blk_adr;
      unsigned long long lckctx$q_enq_name_acmode;
      unsigned long long lckctx$q_enq_priority;
      void *lckctx$pq_enq_req_acpted_adr;
    };
    struct  {
      unsigned long long lckctx$q_deq_lockid;
      unsigned long long lckctx$q_deq_valblk;
      unsigned long long lckctx$q_deq_flags;
      unsigned long long lckctx$q_deq_ctx_prm1;
      unsigned long long lckctx$q_deq_ctx_prm2;
      unsigned long long lckctx$q_deq_ctx_prm3;
      void *lckctx$pq_deq_retadr;
    } ;
  };
};
 
#endif
 
