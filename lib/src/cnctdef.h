#ifndef cnctdef_h
#define cnctdef_h

#define CNCT$K_ECOLEVEL                0x00
#define CNCT$K_INITIAL                 0x01
#define CNCT$K_RECONNECT               0x02
#define CNCT$K_ECOLEVEL_V52            0x13
#define CNCT$K_PROTOCOL_V50            0x15
#define CNCT$K_PROTOCOL_P22            0x16
#define CNCT$K_PROTOCOL                0x17
#define CNCT$K_PROTOCOL_P23            0x17

#define CNCT$V_CLUSTER                 0x00
#define CNCT$V_LONG_BREAK              0x00
#define CNCT$V_REMOVED                 0x02

#define CNCT$M_CLUSTER                 0x01
#define CNCT$M_LONG_BREAK              0x01
#define CNCT$M_MEMBER                  0x02
#define CNCT$M_REMOVED                 0x04

struct _cnct {

unsigned char cnct$b_ecolvl;
unsigned char cnct$b_vernum;
unsigned char cnct$b_type;
unsigned char cnct$b_acklim;
unsigned short cnct$w_quorum;
unsigned short cnct$w_votes;
unsigned short cnct$w_nodes;
unsigned char cnct$b_clssts;
unsigned char cnct$b_cnxsts;
unsigned short cnct$w_rcvdseqnm;
unsigned char cnct$b_cluver;

}

#endif

