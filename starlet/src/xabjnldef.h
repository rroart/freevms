#ifndef xabjnldef_h
#define xabjnldef_h

#define XAB$C_JNL 34
	
#define XAB$C_JNL_TYPE_NONE 0
#define XAB$C_AI 1
#define XAB$C_BI 2
#define XAB$C_AT 3
#define XAB$C_RU_DEFAULT 4
#define XAB$M_JOURNAL_DISABLED 0x1
#define XAB$M_BACKUP_DONE 0x2
	
struct _xabjnldef {
  unsigned char xab$b_cod;
  unsigned char xab$b_bln;
  short int xabjnldef$$_fill_1;
  void *xab$l_nxt;
  unsigned char xab$b_xabjnl_type;
  char xabjnldef$$_fill_5;
  union  {
    unsigned short int xab$w_jnl_flags;
    struct  {
      unsigned xab$v_journal_disabled	: 1;
      unsigned xab$v_backup_done	: 1;
      unsigned xab$v_fill_16_		: 6;
    };
  };
  struct _fabdef *xab$l_jnl_fab;
  char *xab$l_volnam_buf;
  unsigned short int xab$w_volnam_siz;
  unsigned short int xab$w_volnam_len;
  unsigned long long xab$q_jnl_verify_cdate;
  unsigned int xab$l_jnlidx;
  unsigned int xab$l_backup_seqno;
  unsigned long long xab$q_jnl_mod_time;
  unsigned int xabjnldef$$_fill_7;
  unsigned int xabjnldef$$_fill_8;
  unsigned int xabjnldef$$_fill_9;
  unsigned int xabjnldef$$_fill_10;
  unsigned int xabjnldef$$_fill_11;
  unsigned int xabjnldef$$_fill_12;
  unsigned int xabjnldef$$_fill_13;
  unsigned int xabjnldef$$_fill_14;
};
 
#endif
 
