#ifndef EMBHDDEF_H
#define EMBHDDEF_H

#define EMB$K_HD_REV_V50 2
#define EMB$K_HD_REV_V51 3
#define EMB$K_HD_REV_HICKORY 4
#define EMB$K_HD_REV_V10 5
#define EMB$K_HD_REV_V20 6
#define EMB$K_HD_REV_V30 7

#define EMB$C_HD_REV_V50 2
#define EMB$C_HD_REV_V51 3
#define EMB$C_HD_REV_HICKORY 4
#define EMB$C_HD_REV_V10 5
#define EMB$C_HD_REV_V20 6
#define EMB$C_HD_REV_V30 7

#define EMB$C_OS_RESERVED 0
#define EMB$C_OS_VAXVMS 1
#define EMB$C_OS_VAXELN 2
#define EMB$C_OS_ALPHAVMS 3

#define EMB$K_OS_RESERVED 0
#define EMB$K_OS_VAXVMS 1
#define EMB$K_OS_VAXELN 2
#define EMB$K_OS_ALPHAVMS 3

typedef char SCS_NAME_LEN [16];

#define EMB$C_SCS_NAME_LENGTH 16
#define EMB$K_SCS_NAME_LENGTH 16
#define EMB$K_HD_LENGTH 96
#define EMB$C_HD_LENGTH 96

#define EMB$K_LENGTH 16
#define EMB$S_EMBHDDEF 112

#define EMB$M_EM_BUS 0x1
#define EMB$M_EM_CPU 0x2
#define EMB$M_EM_MEMORY 0x4
#define EMB$M_EM_ADAPTER 0x8
#define EMB$M_EM_CACHE 0x10
#define EMB$M_EM_VECTOR 0x20
#define EMB$M_RSRVD1 0x40

#define EMB$M_FL_DDR 0x1
#define EMB$M_FL_OVWRT 0x2

struct _embhd_nonfixed
{
    UINT32 emb$l_hd_sid;
    UINT16 emb$w_hd_hdr_rev;
    union
    {
        UINT32 emb$l_hd_systype;
        struct
        {
            UINT16 emb$w_hd_xsid_rsv;
            UINT8 emb$b_hd_xsid_rev;
            UINT8 emb$b_hd_xsid_typ;
        };
    };
    UINT32 emb$l_cpuid;
    UINT8 emb$b_dev_class;
    UINT8 emb$b_dev_type;
    SCS_NAME_LEN emb$t_scs_name;
    UINT16 emb$w_flags;
    UINT8 emb$b_os_id;
    UINT8 emb$b_hdrsz;
    union
    {
        UINT16 emb$w_hd_entry;
        struct
        {
            UINT8 emb$b_devtyp;
            UINT8 emb$b_devcls;
        };
    };
    UINT64 emb$q_hd_time;
    UINT16 emb$w_hd_errseq;
    UINT64 emb$q_hd_swvers;
    UINT32 emb$l_hd_errmsk;
    UINT32 emb$l_hd_abstim;
    UINT8 emb$b_hd_hw_name_len;
    char emb$t_hd_hw_name [31];
};

struct _embhd
{
    struct _embhd_nonfixed emb$r_embhd_nonfixed;
};

struct _embtrailer
{
    INT32 emb$l_tr_spare1;
    INT32 emb$l_tr_spare2;
    INT32 emb$l_tr_spare3;
    INT32 emb$l_tr_actual_size;
    UINT32 emb$l_tr_active_cpus;
    UINT32 emb$l_tr_logging_cpu;
    UINT64 emb$q_tr_tdf;
};

union _errmsk_fields
{
    INT32 emb$l_errmsk;
    struct
    {
        unsigned emb$v_em_bus : 1;
        unsigned emb$v_em_cpu : 1;
        unsigned emb$v_em_memory : 1;
        unsigned emb$v_em_adapter : 1;
        unsigned emb$v_em_cache : 1;
        unsigned emb$v_em_vector : 1;
        unsigned emb$v_rsrvd1 : 1;
        unsigned emb$v_rsrvd : 25;
    };
};

union _flags_fields
{
    INT32 emb$l_flags;
    struct
    {
        unsigned emb$v_fl_ddr : 1;
        unsigned emb$v_fl_ovwrt : 1;
        unsigned emb$v_rsrvd : 14;
    };
};

#endif

