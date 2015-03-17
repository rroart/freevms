#ifndef clsmsgdef_h
#define clsmsgdef_h

#define CLMCNX$K_DATA                  0x30
#define CLMCNX$K_FNC_DESC              0x05
#define CLMCNX$K_FNC_DOSTEP            0x0C
#define CLMCNX$K_FNC_ENTER             0x02
#define CLMCNX$K_FNC_FORM              0x07
#define CLMCNX$K_FNC_IGNORE            0x13
#define CLMCNX$K_FNC_JOIN              0x09
#define CLMCNX$K_FNC_LB                0x11
#define CLMCNX$K_FNC_LOCK              0x03
#define CLMCNX$K_FNC_PH2               0x0A
#define CLMCNX$K_FNC_QFINQ             0x10
#define CLMCNX$K_FNC_QUORUM            0x0D
#define CLMCNX$K_FNC_READY             0x0B
#define CLMCNX$K_FNC_RECONFIG          0x08
#define CLMCNX$K_FNC_RESLOCK           0x12
#define CLMCNX$K_FNC_STATUS            0x01
#define CLMCNX$K_FNC_TOPOLOGY          0x0F
#define CLMCNX$K_FNC_TRNSTS            0x0E
#define CLMCNX$K_FNC_UNLOCK            0x04
#define CLMCNX$K_FNC_VEC               0x06
#define CLMCNX$K_IDLE                  0x10
#define CLMCNX$K_LOCK                  0x20
#define CLMCNX$K_PH1                   0x40
#define CLMCNX$K_PH2                   0x60
#define CLMCNX$K_RB_DIR                0x03
#define CLMCNX$K_RB_FULL               0x04
#define CLMCNX$K_RB_MERGE              0x01
#define CLMCNX$K_RB_NORBLD             0x00
#define CLMCNX$K_RB_PARTIAL            0x02
#define CLMCNX$K_RP_TRNSTS_PH0         0x01
#define CLMCNX$K_RP_TRNSTS_PH1         0x03
#define CLMCNX$K_RP_TRNSTS_PH1B        0x02
#define CLMCNX$K_RP_TRNSTS_PH2         0x04
#define CLMCNX$K_UNLOCK                0x50
#define CLMCNX$K_XTN_FORM              0x01
#define CLMCNX$K_XTN_JOIN              0x02
#define CLMCNX$K_XTN_QUORUM            0x04
#define CLMCNX$K_XTN_RECONFIG          0x03
#define CLMCNX$L_XTN_ID                0x0C

#define CLMCNX$V_RP_TRNSTS_CMT         0x00
#define CLMCNX$V_RP_TRNSTS_CMT         0x00

#define CLMCNX$B_ACK                   0x12
#define CLMCNX$B_REPLY                 0x13
#define CLMCNX$B_XTN_CODE              0x11
#define CLMCNX$B_XTN_PHASE             0x10

#define CLSMSG$K_FAC_CNX 0x01
#define CLSMSG$K_FAC_LCK 0x02
#define CLSMSG$K_FAC_ACK 0x04
#define CLSMSG$K_FAC_LKI 0x05
#define CLSMSG$K_FAC_CSP 0x06
#define CLSMSG$K_FAC_BLK 0x07
#define CLSMSG$K_FAC_CWPS 0x09
#define CLSMSG$M_RESPMSG 0x80
#define CLSMSG$K_MAXMSG	0x84

struct _clsmsgdef
{
    unsigned short clsmsg$w_seqnum;
    unsigned short clsmsg$w_ackseq;
    unsigned long clsmsg$l_rspid;
    unsigned char clsmsg$b_facility;
    unsigned char clsmsg$b_func;
    /* two missing? */
    unsigned long clsmsg$l_reqr_bufh;
    /* more? */
};

#endif
