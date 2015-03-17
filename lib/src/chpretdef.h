#ifndef chpretdef_h
#define chpretdef_h

#define CHPRET$M_ACMODE 0x1
#define CHPRET$M_MAC 0x2
#define CHPRET$M_DAC 0x4
#define CHPRET$M_MATCHED_ACE 0x8
#define CHPRET$M_SOGW 0x10
#define CHPRET$M_ACL_KEYID 0x20
#define CHPRET$M_IVBUFLEN 0x40
#define CHPRET$K_LENGTH 44
#define CHPRET$C_LENGTH 44

struct _chpret
{
    unsigned int chpret$l_auditlen;
    void *chpret$l_audit;
    void *chpret$l_auditret;
    unsigned int chpret$l_alarmlen;
    void *chpret$l_alarm;
    void *chpret$l_alarmret;
    unsigned int chpret$l_matched_acelen;
    void *chpret$l_matched_ace;
    void *chpret$l_matched_aceret;
    void *chpret$l_privs_used;
    union
    {
        unsigned int chpret$l_progress;
        struct
        {
            unsigned chpret$v_acmode : 1;
            unsigned chpret$v_mac : 1;
            unsigned chpret$v_dac : 1;
            unsigned chpret$v_matched_ace : 1;
            unsigned chpret$v_sogw : 1;
            unsigned chpret$v_acl_keyid : 1;
            unsigned chpret$v_ivbuflen : 1;
            unsigned chpret$v_fill_2_ : 1;
        };
    };
};

#endif

