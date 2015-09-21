#ifndef CONFIG_TABLE_H
#define CONFIG_TABLE_H

struct _config_header
{
    union
    {
        struct
        {
            UINT32 iogen_cfg_tbl$l_hw_id_mask_lo;
            UINT32 iogen_cfg_tbl$l_hw_id_mask_hi;
        };
        UINT64 iogen_cfg_tbl$q_hw_id_mask;
    };
};

#define IOGEN_CFG_TBL$K_HEADER_LEN 8

struct _config_memdsc
{
    char *config_memdsc$ps_fragment;
    INT16 config_memdsc$w_free;
    INT8 config_memdsc$b_refcnt;
    INT8 config_memdsc$b_fill;
    INT16 config_memdsc$w_size;
    INT8 config_memdsc$b_type;
    INT8 config_memdsc$b_subtype;
    INT32 config_memdsc$l_actual_size;
};

#define IOGEN_CFG_TBL$M_MSCP 0x1
#define IOGEN_CFG_TBL$M_SCSI 0x2
#define IOGEN_CFG_TBL$M_NOVECTOR 0x4
#define IOGEN_CFG_TBL$M_DISK 0x8
#define IOGEN_CFG_TBL$M_NETWORK 0x10
#define IOGEN_CFG_TBL$M_PORT 0x20
#define IOGEN_CFG_TBL$M_CLASS 0x40
#define IOGEN_CFG_TBL$M_PSEUDO 0x80
#define IOGEN_CFG_TBL$M_BOOT 0x100
#define IOGEN_CFG_TBL$M_CASE_BLIND 0x200
#define IOGEN_CFG_TBL$M_NO_TABLE 0x400
#define IOGEN_CFG_TBL$M_EXTENDED_ID 0x800
#define IOGEN_CFG_TBL$M_FOREIGN 0x1000
#define IOGEN_CFG_TBL$M_FIBRE 0x2000
#define IOGEN_CFG_TBL$M_ULP 0x4000
#define IOGEN_CFG_TBL$M_LOAD_DRV_CLASS 0x8000
#define IOGEN_CFG_TBL$M_NISCA 0x10000
#define IOGEN_CFG_TBL$M_ISA_ON_EISA 0x20000
#define IOGEN_CFG_TBL$M_SYS_DEV 0x1
#define IOGEN_CFG_TBL$M_REMOTE 0x2
#define IOGEN_CFG_TBL$M_ALT_PREFIX 0x4
#define IOGEN_CFG_TBL$M_NO_CTRL_LTR 0x8
#define IOGEN_CFG_TBL$M_HW_CTRL_LTR 0x10
#define IOGEN_CFG_TBL$M_UNIT_0 0x20
#define IOGEN_CFG_TBL$M_CTRL_LTR_A 0x40
#define IOGEN_CFG_TBL$M_CREATE_DEV 0x80

struct _iogen_cfg_tbl
{
    union
    {
        struct
        {
            UINT32 iogen_cfg_tbl$l_hw_id_lo;
            UINT32 iogen_cfg_tbl$l_hw_id_hi;
        };
        UINT64 iogen_cfg_tbl$q_hw_id;
        INT8 iogen_cfg_tbl$b_hw_id_string [8];
    };
    void *iogen_cfg_tbl$ps_driver_name;
    char *iogen_cfg_tbl$ps_devnam;
    UINT32 iogen_cfg_tbl$l_vector_cnt;
    UINT32 iogen_cfg_tbl$l_vector_align;
    UINT32 iogen_cfg_tbl$l_num_units;
    union
    {
        UINT32 iogen_cfg_tbl$l_flags;
        struct
        {
            unsigned iogen_cfg_tbl$v_mscp : 1;
            unsigned iogen_cfg_tbl$v_scsi : 1;
            unsigned iogen_cfg_tbl$v_novector : 1;
            unsigned iogen_cfg_tbl$v_disk : 1;
            unsigned iogen_cfg_tbl$v_network : 1;
            unsigned iogen_cfg_tbl$v_port : 1;
            unsigned iogen_cfg_tbl$v_class : 1;
            unsigned iogen_cfg_tbl$v_pseudo : 1;
            unsigned iogen_cfg_tbl$v_boot : 1;
            unsigned iogen_cfg_tbl$v_case_blind : 1;
            unsigned iogen_cfg_tbl$v_no_table : 1;
            unsigned iogen_cfg_tbl$v_extended_id : 1;
            unsigned iogen_cfg_tbl$v_foreign : 1;
            unsigned iogen_cfg_tbl$v_fibre : 1;
            unsigned iogen_cfg_tbl$v_ulp : 1;
            unsigned iogen_cfg_tbl$v_load_drv_class : 1;
            unsigned iogen_cfg_tbl$v_nisca : 1;
            unsigned iogen_cfg_tbl$v_isa_on_eisa : 1;
            unsigned iogen_cfg_tbl$v_fill_0_ : 6;
        };
    };
    char *iogen_cfg_tbl$ps_description;
    UINT32 iogen_cfg_tbl$l_adp_type;
    char *iogen_cfg_tbl$ps_assoc_drv;
    char *iogen_cfg_tbl$ps_dtype;
    char *iogen_cfg_tbl$ps_boot_class;
    union
    {
        UINT32 iogen_cfg_tbl$l_boot_flags;
        struct
        {
            unsigned iogen_cfg_tbl$v_sys_dev : 1;
            unsigned iogen_cfg_tbl$v_remote : 1;
            unsigned iogen_cfg_tbl$v_alt_prefix : 1;
            unsigned iogen_cfg_tbl$v_no_ctrl_ltr : 1;
            unsigned iogen_cfg_tbl$v_hw_ctrl_ltr : 1;
            unsigned iogen_cfg_tbl$v_unit_0 : 1;
            unsigned iogen_cfg_tbl$v_ctrl_ltr_a : 1;
            unsigned iogen_cfg_tbl$v_create_dev : 1;
        };
    };
    char *iogen_cfg_tbl$ps_private;
    INT32 iogen_cfg_tbl$l_avail;
    INT32 iogen_cfg_tbl$l_avail2;
    INT8 iogen_cfg_tbl$b_fill_1_ [4];
};

#define IOGEN_CFG_TBL$K_ENTRY_SIZE 72

struct _config_table
{
    struct _config_header config_table$r_header;
    struct _iogen_cfg_tbl config_table$r_entries;
};

struct _config_mem
{
    struct _config_memdsc config_mem$r_memdsc;
    struct _config_header config_mem$r_header;
    struct _iogen_cfg_tbl config_mem$r_entries;
};

#endif

