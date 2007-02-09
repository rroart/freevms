#ifndef lkidef_h
#define lkidef_h

#define LKI$K_LENGTH 24
#define LKI$C_LENGTH 24
	
struct _lkidef {
  union  {
    unsigned int lki$l_mstlkid;
    unsigned int lki$l_lockid;
  };
  unsigned int lki$l_pid;
  union  {
    unsigned int lki$l_mstcsid;
    unsigned int lki$l_sysid;
  };
  unsigned char lki$b_rqmode;
  unsigned char lki$b_grmode;
  char lki$b_queue;
  char lki$$$_spare;
  union  {
    unsigned int lki$l_lkid;
    unsigned int lki$l_remlkid;
  };
  union  {
    unsigned int lki$l_csid;
    unsigned int lki$l_remsysid;
  };
};

#define LKI$K_BR_LENGTH 56
#define LKI$C_BR_LENGTH 56
	
struct _lkidef_br {
  struct _lkidef lki$r_lki_base;
  unsigned long long lki$q_rqstart;
  unsigned long long lki$q_rqlength;
  unsigned long long lki$q_grstart;
  unsigned long long lki$q_grlength;
};

#define LKI$M_SYSNAM 0x80000000
	
struct _namspace {
  union  {
    struct  {
      unsigned short int lki$w_group;
      unsigned char lki$b_rmod;
      unsigned char lki$b_status;
    };
    struct  {
      unsigned lki$$_fill	: 31;
      unsigned lki$v_sysnam	: 1;
    };
  };
};
 
struct _statef {
  unsigned char lki$b_state_rqmode;
  unsigned char lki$b_state_grmode;
  char lki$b_state_queue;
};

#define		LKIUSR$K_LENGTH		20
#define		LKIUSR$C_LENGTH		20
#define		LKIUSR$K_BLOCKER_START	20
	
struct _lkiusrdef {
  unsigned long long lkiusr$q_start;
  unsigned long long lkiusr$q_length;
  unsigned char lkiusr$b_rqmode;
  unsigned char lkiusr$b_ignore_own;
  short int lki$$_fill_1;
};

struct _rngdef {
  unsigned long long lki$_rng_q_rqstart;
  unsigned long long lki$_rng_q_rqlength;
  unsigned long long lki$_rng_q_grstart;
  unsigned long long lki$_rng_q_grlength;
};

#define		LKI$C_GRANTED		1
#define		LKI$C_CONVERT		0
#define		LKI$C_WAITING		-1
#define		LKI$C_RETRY		-2
#define		LKI$C_SCSWAIT		-3
#define		LKI$C_RSPNOTQED		-4
#define		LKI$C_RSPQUEUED		-5
#define		LKI$C_RSPGRANTD		-6
#define		LKI$C_RSPDOLOCL		-7
#define		LKI$C_RSPRESEND		-8

#define		LKI$C_LKBTYPE		1
#define		LKI$C_RSBTYPE		2
#define		LKI$C_LISTEND		0

#define		LKI$_PID		256
#define		LKI$_STATE		257
#define		LKI$_PARENT		258
#define		LKI$_LCKREFCNT		259
#define		LKI$_LOCKID		260
#define		LKI$_REMLKID		261
#define		LKI$_MSTLKID		262
#define		LKI$_LKID		263
#define		LKI$_CSID		264
#define		LKI$_BRL		265

#define		LKI$_RANGE		266
#define		LKI$_LASTLKB		267

#define		LKI$_NAMSPACE		512
#define		LKI$_RESNAM		513
#define		LKI$_RSBREFCNT		514
#define		LKI$_VALBLK		515
#define		LKI$_SYSTEM		516
#define		LKI$_LCKCOUNT		517
#define		LKI$_BLOCKEDBY		518
#define		LKI$_BLOCKING		519
#define		LKI$_LOCKS		520
#define		LKI$_CVTCOUNT		521
#define		LKI$_WAITCOUNT		522
#define		LKI$_GRANTCOUNT		523
#define		LKI$_MSTCSID		524
#define		LKI$_VALBLKST		525
#define		LKI$_BLOCKEDBY_BR	526
#define		LKI$_BLOCKING_BR	527
#define		LKI$_LOCKS_BR		528
#define		LKI$_BLOCKER_BR		529

#define		LKI$_LASTRSB		530
#define		LKISND$K_HDRLEN		16
#define		LKISND$C_HDRLEN		16
	
struct _lkisnddef {
  unsigned short int lkisnd$w_retsize;
  unsigned short int lkisnd$w_status;
  unsigned short int lkisnd$w_flags;
  unsigned short int lkisnd$w_seqnum;
  unsigned short int lkisnd$w_size;
  unsigned char lkisnd$b_type;
  unsigned char lkisnd$b_fill_2;
  unsigned char lkisnd$b_item_length;
  unsigned char lkisnd$b_fill_3;
  unsigned short int lkisnd$w_fill_4;
};
 
#endif
