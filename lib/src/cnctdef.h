#ifndef CNCTDEF_H
#define CNCTDEF_H

#include <vms_types.h>

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

struct _cnct
{

    UINT8 cnct$b_ecolvl;
    UINT8 cnct$b_vernum;
    UINT8 cnct$b_type;
    UINT8 cnct$b_acklim;
    UINT16 cnct$w_quorum;
    UINT16 cnct$w_votes;
    UINT16 cnct$w_nodes;
    UINT8 cnct$b_clssts;
    UINT8 cnct$b_cnxsts;
    UINT16 cnct$w_rcvdseqnm;
    UINT8 cnct$b_cluver;

}

#endif

