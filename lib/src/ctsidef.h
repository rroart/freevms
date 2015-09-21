#ifndef CTSIDEF_H
#define CTSIDEF_H

#define CTSI$K_REVISION 1
#define CTSI$C_REVISION 1
#define CTMD$K_SIZE 8
#define CTMD$C_SIZE 8

#define CTCB$K_SIZE 28
#define CTCB$C_SIZE 28

#define CTIOS$K_SIZE 8
#define CTIOS$C_SIZE 8

#define CTSI$K_MODULE_COUNT 6
#define CTSI$C_MODULE_COUNT 6
#define CTSI$K_CHN_COUNT 6
#define CTSI$C_CHN_COUNT 6
#define CTSI$M_CMUSE 0x3
#define CTSI$M_INUSE 0x4
#define CTSI$M_CM 0x10
#define CTSI$S_CTSIDEF 316

struct _ctmd
{
    UINT16 ctmd$w_pgcount;
    INT16 ctmd$w_spare_1;
    void *ctmd$l_baseaddr;
};

struct _ctcb
{
    UINT8 ctcb$b_dvatr;
    UINT8 ctcb$b_chatr;
    UINT16 ctcb$w_statesz;
    INT32 (*ctcb$l_phy_entry)(void);
    INT32 (*ctcb$l_vir_entry)(void);
    void *ctcb$l_phy_segment;
    void *ctcb$l_vir_segment;
    void *ctcb$l_phy_extend;
    void *ctcb$l_vir_extend;
};

struct _ctios
{

    union
    {
        struct
        {
            UINT16 ctios$w_sgmt_count;
            INT16 ctios$w_spare_1;
            UINT8 ctios$b_base_segment;
        };
        struct
        {
            UINT16 ctios$w_pgcount;
            INT16 ctios$w_spare_2;
            void *ctios$l_segment;
        };
    };
};

struct _ctsi
{
    struct _ctsi *ctsi$l_base;
    UINT16 ctsi$w_size;
    UINT16 ctsi$w_ident;
    UINT8 ctsi$b_spare0;
    UINT8 ctsi$b_chksum;
    union
    {
        UINT8 ctsi$b_flags;
        struct
        {
            unsigned ctsi$v_cmuse : 2;
            unsigned ctsi$v_inuse : 1;
            unsigned ctsi$v_spare0 : 1;
            unsigned ctsi$v_cm : 1;
            unsigned ctsi$v_fill_0_ : 3;
        };
    };
    UINT8 ctsi$b_revisn;
    UINT64 ctsi$q_module_desc [6];
    UINT32 ctsi$l_chnblk [42];
    UINT64 ctsi$q_save;
    UINT64 ctsi$q_restore;
    UINT64 ctsi$q_trans;
    UINT64 ctsi$q_getchr;
    UINT32 ctsi$l_getchr_state [4];
    UINT64 ctsi$q_putchr;
    UINT32 ctsi$l_putchr_state [4];
    UINT64 ctsi$q_putmsg;
    UINT64 ctsi$q_readprompt;
};

#endif

