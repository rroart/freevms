#ifndef ccdef_h
#define ccdef_h

#define CC$C_EVT_HELLO                 0x00
#define CC$C_EVT_BYE                   0x01
#define CC$C_EVT_CCSTART               0x02
#define CC$C_EVT_VERF                  0x03
#define CC$C_EVT_VACK                  0x04
#define CC$C_EVT_DONE                  0x05
#define CC$C_EVT_SOLICIT_SRV           0x06
#define CC$C_EVT_RESERVED              0x07
#define CC$C_EVT_TRANS                 0x08
#define CC$C_EVT_CC_TIMER              0x09
#define CC$C_EVT_SHORT_MSG             0x0A
#define CC$C_EVT_BAD_ECO               0x0B
#define CC$C_EVT_AUTHORIZE             0x0C
#define CC$C_EVT_BAD_MC                0x0D
#define CC$C_EVT_INCOMP_CHNL           0x0E

#define CC$C_STATE_CLOSED              0x00
#define CC$C_STATE_CCSTART_SENT        0x01
#define CC$C_STATE_CCSTART_REC         0x02
#define CC$C_STATE_SOLICIT_REC         0x03
#define CC$C_STATE_OPEN                0x04
#define CC$C_STATE_HELLO_DLY           0x05
#define CC$C_STATE_SOLICIT_DLY         0x06
#define CC$C_STATE_RESERVED            0x07
#define CC$C_STATE_TOTAL               0x08


#endif
