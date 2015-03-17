#ifndef csdtdef_h
#define csdtdef_h

#define CSDTE$C_LENGTH   64
#define CSDTE$K_LENGTH   64
#define CSDTE$S_CSDTEDEF 64

struct _csdte
{
    unsigned long long csdte$q_req_iq;
    unsigned long long csdte$q_waitq;
    unsigned int csdte$l_ipid;
    unsigned int csdte$l_queued;
    unsigned int csdte$l_requests;
    unsigned int csdte$l_waitcnt;
    unsigned int csdte$l_maxactive;
    unsigned int csdte$l_active;
    char csdte$t_fillme_4 [24];
};

#define CSDT$C_LENGTH 16
#define CSDT$K_LENGTH 16
#define CSDT$T_CSDTEVEC 16
#define CSDT$S_CSDTDEF 16

typedef struct _csdt
{
    int csdt$l_filler_1;
    int csdt$l_filler_2;
    unsigned short int csdt$w_size;
    unsigned char csdt$b_type;
    unsigned char csdt$b_subtype;
    char csdt$t_align [4];
};

#endif

