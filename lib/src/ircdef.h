#ifndef ircdef_h
#define ircdef_h

#define		IRC$M_PTRSZ		0x3
#define		IRC$M_RECORDCB		0xFC
#define		IRC$M_DELETED		0x4
#define		IRC$M_NOPTRSZ		0x10
#define		IRC$M_FIRST_KEY		0x80
#define		IRC$M_RRV		0x8
#define		IRC$M_NODUPCNT		0x10
#define		IRC$M_RU_DELETE		0x20
#define		IRC$M_RU_UPDATE		0x40
#define		IRC$C_IDXPTRBAS		2
#define		IRC$C_IDXOVHDSZ		1

#define		IRC$S_IRCDEF		1
	
#define		IRC$S_IRCDEF1		3
	
#define		IRC$C_DATSZFLD		2
#define		IRC$C_DATPTRBAS		3
#define		IRC$C_DCNTSZFLD		4
#define		IRC$C_DATOVHDSZ		2
#define		IRC$C_FIXOVHDSZ		7
#define		IRC$C_VAROVHDSZ		9
#define		IRC$C_RRVOVHDSZ		7

#define		IRC$C_DATPTRBS3		4
#define		IRC$C_DATOVHSZ3		3
#define		IRC$C_FIXOVHSZ3		9
#define		IRC$C_VAROVHSZ3		11
#define		IRC$C_RRVOVHSZ3		9
#define		IRC$C_SDROVHSZ3		2
#define		IRC$C_KEYCMPOVH		2
#define		IRC$C_DATCMPOVH		3
#define		IRC$S_IRCDEF2		5

// maybe this is the stuff around General Data Bucket Record/Record Reference Vector in rmsint2.doc?
	
struct _irc {
  union  {
    unsigned char irc$b_control;
#if 0
    struct  {
      unsigned irc$v_ptrsz		: 2;
      unsigned irc$v_recordcb		: 6;
    };
    struct  {
      unsigned irc$$_fill_1		: 2;
      unsigned irc$v_deleted		: 1;
      unsigned irc$$_fill_2		: 1;
      unsigned irc$v_noptrsz		: 1;
      unsigned irc$$_fill_3		: 2;
      unsigned irc$v_first_key		: 1;
    };
    struct  {
      unsigned irc$$_fill_3		: 3;
      unsigned irc$v_rrv		: 1;
      unsigned irc$v_fill_5_		: 4;
    };
    struct  {
      unsigned irc$$_fill_5		: 4;
      unsigned irc$v_nodupcnt		: 1;
      unsigned irc$v_fill_6_		: 3;
    };
    struct  {
      unsigned irc$$_fill_6		: 5;
      unsigned irc$v_ru_delete		: 1;
      unsigned irc$v_ru_update		: 1;
      unsigned irc$v_fill_7_		: 1;
    };
    struct  {
      char irc$$_fill_7;
    };
#endif
  };
};
 
struct _irc1 {
  struct _irc irc$r_irc;
  unsigned char irc$b_id;
  unsigned char irc$b_rrv_id;
};

struct _irc2 {
  struct _irc irc$r_irc;
  unsigned short int irc$w_id;
  unsigned short int irc$w_rrv_id;
};

#endif
 
