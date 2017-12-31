#ifndef ctsidef_h
#define ctsidef_h

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
    unsigned short int ctmd$w_pgcount;
    short int ctmd$w_spare_1;
    void *ctmd$l_baseaddr;
};

struct _ctcb
{
    unsigned char ctcb$b_dvatr;
    unsigned char ctcb$b_chatr;
    unsigned short int ctcb$w_statesz;
    int (*ctcb$l_phy_entry)();
    int (*ctcb$l_vir_entry)();
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
            unsigned short int ctios$w_sgmt_count;
            short int ctios$w_spare_1;
            unsigned char ctios$b_base_segment;
        };
        struct
        {
            unsigned short int ctios$w_pgcount;
            short int ctios$w_spare_2;
            void *ctios$l_segment;
        };
    };
};

struct _ctsi
{
    struct _ctsi *ctsi$l_base;
    unsigned short int ctsi$w_size;
    unsigned short int ctsi$w_ident;
    unsigned char ctsi$b_spare0;
    unsigned char ctsi$b_chksum;
    union
    {
        unsigned char ctsi$b_flags;
        struct
        {
            unsigned ctsi$v_cmuse : 2;
            unsigned ctsi$v_inuse : 1;
            unsigned ctsi$v_spare0 : 1;
            unsigned ctsi$v_cm : 1;
            unsigned ctsi$v_fill_0_ : 3;
        };
    };
    unsigned char ctsi$b_revisn;
    unsigned long long ctsi$q_module_desc [6];
    unsigned int ctsi$l_chnblk [42];
    unsigned long long ctsi$q_save;
    unsigned long long ctsi$q_restore;
    unsigned long long ctsi$q_trans;
    unsigned long long ctsi$q_getchr;
    unsigned int ctsi$l_getchr_state [4];
    unsigned long long ctsi$q_putchr;
    unsigned int ctsi$l_putchr_state [4];
    unsigned long long ctsi$q_putmsg;
    unsigned long long ctsi$q_readprompt;
};

#endif

