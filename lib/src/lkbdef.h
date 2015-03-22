#ifndef LKBDEF_H
#define LKBDEF_H

#define LKB$M_FLAGS_VALID 0x4
#define LKB$M_PKAST 0x10
#define LKB$M_NODELETE 0x20
#define LKB$M_QUOTA 0x40
#define LKB$M_KAST 0x80
#define LKB$K_ACB64LEN 64
#define LKB$M_DCPLAST 0x1
#define LKB$M_ASYNC 0x4
#define LKB$M_BLKASTQED 0x8
#define LKB$M_MSTCPY 0x10
#define LKB$M_NOQUOTA 0x20
#define LKB$M_TIMOUTQ 0x40
#define LKB$M_WASSYSOWN 0x80
#define LKB$M_CVTTOSYS 0x100
#define LKB$M_PROTECT 0x200
#define LKB$M_RESEND 0x400
#define LKB$M_RM_RBRQD 0x800
#define LKB$M_FLOCK 0x1000
#define LKB$M_IP 0x2000
#define LKB$M_CACHED 0x4000
#define LKB$M_RNGBLK 0x8000
#define LKB$M_BRL 0x10000
#define LKB$M_NEWSUBRNG 0x20000
#define LKB$M_CVTSUBRNG 0x40000
#define LKB$M_RNGCHG 0x80000
#define LKB$M_2PC_IP 0x100000
#define LKB$M_2PC_PEND 0x200000
#define LKB$M_BLKASTFLG 0x400000
#define LKB$M_GRSUBRNG 0x800000
#define LKB$M_PCACHED 0x1000000
#define LKB$M_VALBLKRD 0x2000000
#define LKB$M_VALBLKWRT 0x4000000
#define LKB$M_DPC 0x8000000
#define LKB$M_PERCPU 0x10000000
#define LKB$M_INDEX 0xFFFFFF
#define LKB$K_GRANTED 1
#define LKB$K_CONVERT 0
#define LKB$K_WAITING -1
#define LKB$K_RETRY -2
#define LKB$K_SCSWAIT -3
#define LKB$K_RSPNOTQED -4
#define LKB$K_RSPQUEUED -5
#define LKB$K_RSPGRANTD -6
#define LKB$K_RSPDOLOCL -7
#define LKB$K_RSPRESEND -8
#define LKB$K_RSPWAIT -9
#define LKB$K_RSP2PCRDY -10
#define LKB$K_RSPFATAL -11
#define LKB$M_DBLKAST 0x2
#define LKB$K_LENGTH 248
#define LKB$C_LENGTH 248
#define LKB$S_LKBDEF 248

struct _lkb
{
    struct _acb *lkb$l_astqfl;
    struct _acb *lkb$l_astqbl;
    unsigned short int lkb$w_size;
    unsigned char lkb$b_type;
    unsigned char lkb$b_rmod;
    unsigned long lkb$l_pid;
    unsigned long lkb$l_ast;
    unsigned long lkb$l_astprm;
    unsigned long lkb$l_kast;
    unsigned long lkb$l_cplastadr;
    unsigned long lkb$l_blkastadr;
    unsigned int lkb$l_flags;
    unsigned int lkb$l_status;
    unsigned int lkb$l_lkst1;
    unsigned int lkb$l_lkst2;
    unsigned char lkb$b_rqmode;
    unsigned char lkb$b_grmode;
    unsigned char lkb$b_state;
    unsigned char lkb$b_efn;
    void *lkb$l_sqfl;
    void *lkb$l_sqbl;
    void *lkb$l_ownqfl;
    void *lkb$l_ownqbl;
    struct _lkb *lkb$l_parent;
    unsigned short lkb$w_refcnt;
    unsigned char lkb$b_tslt;
    unsigned char lib$b_res;
    struct _rsb *lkb$l_rsb;
    unsigned long lkb$l_lksb;
    unsigned int lkb$l_remlkid;
    union
    {
        unsigned int lkb$l_csid;
        unsigned long lkb$l_oldastprm;
    };
    int (*lkb$pq_oldblkast)(void);
    unsigned int lkb$l_duetime;
};

#endif

