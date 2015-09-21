#ifndef DPTDEF_H
#define DPTDEF_H

#include <vms_types.h>

#define DPT$S_DPTDEF 160

#define     DPT$K_STEP_1        1
#define     DPT$K_STEP_2        2
#define     DPT$K_STEP1_V1      1
#define     DPT$K_STEP1_V2      2
#define     DPT$K_STEP2_V1      1
#define     DPT$K_STEP2_V2      2
#define     DPT$K_STEP2_V3      3
#define     DPT$K_STEP2_V4      4
#define     DPT$K_STEP2_V5      5
#define     DPT$K_IOGEN_REQ_STEP2VER        5
#define     DPT$M_SUBCNTRL      0x1
#define     DPT$M_SVP       0x2
#define     DPT$M_NOUNLOAD      0x4
#define     DPT$M_SCS       0x8
#define     DPT$M_DUSHADOW      0x10
#define     DPT$M_SCSCI     0x20
#define     DPT$M_BVPSUBS       0x40
#define     DPT$M_UCODE     0x80
#define     DPT$M_SMPMOD        0x100
#define     DPT$M_DECW_DECODE       0x200
#define     DPT$M_TPALLOC       0x400
#define     DPT$M_SNAPSHOT      0x800
#define     DPT$M_NO_IDB_DISPATCH       0x1000
#define     DPT$M_SCSI_PORT     0x2000
#define     DPT$M_ATM       0x4000
#define     DPT$M_CSMACD        0x8000
#define     DPT$M_FDDI      0x10000
#define     DPT$M_TR        0x20000
#define     DPT$M_SHARED_INT        0x40000
#define     DPT$M_DEVPATH_SUP       0x80000
#define     DPT$M_MULTIPATH_SUP     0x100000
#define     DPT$M_QSVR      0x200000
#define     DPT$M_ATM_FORE      0x400000
#define     DPT$M_USB_SUP       0x800000
#define     DPT$M_HOTSWAP_SUP       0x1000000
#define     DPT$K_BASE_LEN      152
#define     DPT$C_BASE_LEN      152

struct _dpt
{
    struct _dpt *dpt$ps_flink;
    struct _dpt *dpt$ps_blink;
    UINT16 dpt$w_size;
    UINT8 dpt$b_type;
    UINT8 dpt$b_refc;
    UINT16 dpt$iw_step;
    UINT16 dpt$iw_stepver;
    UINT8 dpt$b_adptype;
    UINT8 dpt$b_res;
    UINT16 dpt$w_ucbsize;
    UINT32 dpt$l_flags;

    UINT16 dpt$w_defunits;
    UINT16 dpt$w_maxunits;
    void (*dpt$ps_init_pd)(void);
    void (*dpt$ps_reinit_pd)(void);
    int (*dpt$ps_deliver_2)(void);
    int (*dpt$ps_unload)(void);
    struct _ddt *dpt$ps_ddt;
    struct _ddb *dpt$ps_ddb_list;
    int dpt$is_btorder;
    union
    {
        unsigned long dpt$l_vector;
        struct
        {
            void *dpt$ps_vector;
        };
    };
    union
    {
        char dpt$t_name [16];
        struct
        {
            UINT8 dpt$b_name_len;
            char dpt$t_name_str [15];
        };
    };
    union
    {
        UINT32 dpt$l_ecolevel;
        struct
        {
            UINT32 dpt$il_ecolevel;
        };
    };
    UINT32 dpt$l_ucode;
    UINT64 dpt$q_linktime;
    union
    {
        UINT64 dpt$iq_image_name;
        struct
        {
            UINT16 dpt$iw_iname_len;
            UINT8 dpt$ib_iname_type;
            UINT8 dpt$ib_iname_class;
            void *dpt$ps_iname_ptr;
        };
    };
    UINT32 dpt$il_loader_handle [4];
    union
    {
        unsigned long dpt$l_decw_sname;
        struct
        {
            void *dpt$ps_decw_sname;
        };
    };
    void *dpt$ps_customer;
    UINT32 dpt$il_devpath_size;
    UINT32 dpt$il_devpath_ucb_ofs;
    UINT32 dpt$il_dsplypath_size;
    UINT32 dpt$il_dsplypath_ucb_ofs;
    unsigned char dpt$t_image_name;
    INT8 dpt$b_fill_23_ [7];
};

#if 0
#define dpt$t_name dpt$dptunion.dpt$t_name
#endif

#endif

