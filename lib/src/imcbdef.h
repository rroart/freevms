#ifndef IMCBDEF_H
#define IMCBDEF_H

#define     IMCB$K_IMCB_TYPE_CODE       127
#define     IMCB$K_MAIN_PROGRAM     1
#define     IMCB$K_MERGED_IMAGE     2
#define     IMCB$K_GLOBAL_IMAGE_SECTION 3
#define     IMCB$M_EXPREG       0x1
#define     IMCB$M_SHAREABLE    0x2
#define     IMCB$M_OPEN_FOR_WRITE   0x4
#define     IMCB$M_RES_HEADER   0x8
#define     IMCB$M_LOAD_IMAGE   0x10
#define     IMCB$M_INITIALIZE   0x20
#define     IMCB$M_DONE     0x40
#define     IMCB$M_SYS_STB      0x80
#define     IMCB$M_IN_CIRCULARITY   0x100
#define     IMCB$M_MAPPED       0x200
#define     IMCB$M_PROTECTED    0x400
#define     IMCB$M_PARENT_PROT  0x800
#define     IMCB$M_CMOD_VECTOR_MAPPED   0x1000
#define     IMCB$M_XLATED       0x2000
#define     IMCB$M_PROTSECT     0x4000
#define     IMCB$M_NOTPROTSECT  0x8000
#define     IMCB$M_DISCONTIGUOUS    0x10000
#define     IMCB$M_FORKABLE     0x20000
#define     IMCB$M_COMPRESS_DATASEC 0x40000
#define     IMCB$M_VERSION_SAFE 0x80000
#define     IMCB$M_PRIMARY_FIX  0x100000
#define     IMCB$M_DATA_RESIDENT    0x200000
#define     IMCB$M_SHARE_LINK   0x400000
#define     IMCB$M_AUTOACT      0x800000
#define     IMCB$M_MKTHREADS    0x1000000
#define     IMCB$M_UPCALLS      0x2000000
#define     IMCB$M_SYSTEM_IMAGE 0x4000000
#define     IMCB$M_MATCH_CONTROL    0x7
#define     IMCB$M_MINOR_ID     0xFFFFFF
#define     IMCB$M_MAJOR_ID     0xFF000000
#define     IMCB$C_LENGTH       280
#define     IMCB$K_LENGTH       280
#define     IMCB$S_IMCB$DEF     280

struct _imcb
{
    struct _imcb *imcb$l_flink;
    struct _imcb *imcb$l_blink;
    unsigned short int imcb$w_size;
    unsigned char imcb$b_type;
    char imcb$b_imcb_1;
    unsigned char imcb$b_access_mode;
    unsigned char imcb$b_act_code;
    unsigned short int imcb$w_chan;
    union
    {
        unsigned int imcb$l_flags;
        struct
        {
            unsigned imcb$v_expreg      : 1;
            unsigned imcb$v_shareable       : 1;
            unsigned imcb$v_open_for_write  : 1;
            unsigned imcb$v_res_header  : 1;
            unsigned imcb$v_load_image  : 1;
            unsigned imcb$v_initialize  : 1;
            unsigned imcb$v_done        : 1;
            unsigned imcb$v_sys_stb     : 1;
            unsigned imcb$v_in_circularity  : 1;
            unsigned imcb$v_mapped      : 1;
            unsigned imcb$v_protected       : 1;
            unsigned imcb$v_parent_prot : 1;
            unsigned imcb$v_cmod_vector_mapped: 1;
            unsigned imcb$v_xlated      : 1;
            unsigned imcb$v_protsect        : 1;
            unsigned imcb$v_notprotsect : 1;
            unsigned imcb$v_discontiguous   : 1;
            unsigned imcb$v_forkable        : 1;
            unsigned imcb$v_compress_datasec    : 1;
            unsigned imcb$v_version_safe    : 1;
            unsigned imcb$v_primary_fix : 1;
            unsigned imcb$v_data_resident   : 1;
            unsigned imcb$v_share_link  : 1;
            unsigned imcb$v_autoact     : 1;
            unsigned imcb$v_mkthreads       : 1;
            unsigned imcb$v_upcalls     : 1;
            unsigned imcb$v_system_image    : 1;
            unsigned imcb$v_fill_22_        : 5;
        };
    };
    char imcb$t_image_name [40];
    unsigned int imcb$l_symbol_vector_size;
    union
    {
        unsigned long long imcb$q_ident;
        struct
        {
            union
            {
                unsigned int imcb$l_match_control;
                struct
                {
                    unsigned imcb$v_match_control       : 3;
                    unsigned imcb$v_fill_23_        : 5;
                };
            };
            union
            {
                unsigned int imcb$l_version;
                struct
                {
                    unsigned imcb$v_minor_id        : 24;
                    unsigned imcb$v_major_id        : 8;
                };
            };
        };
    };
    union
    {
        unsigned long long imcb$q_address_range;
        struct
        {
            void *imcb$l_starting_address;
            void *imcb$l_end_address;
        };
    };
    struct _eihd *imcb$l_ihd;
    struct _kfe *imcb$l_kfe;
    void *imcb$l_context;
    void *imcb$l_base_address;
    void (*imcb$l_initialize)(void);
    unsigned int imcb$l_active_sons;
    union
    {
        long long imcb$q_fixup_vector_address;
        struct
        {
            struct _eiaf *imcb$ps_fixup_vector_address;
        };
    };
    union
    {
        long long imcb$q_symbol_vector_address;
        struct
        {
            void *imcb$ps_symbol_vector_address;
        };
    };
    union
    {
        long long imcb$q_plv_address;
        struct
        {
            struct _plv *imcb$ps_plv_address;
        };
    };
    union
    {
        long long imcb$q_cmod_kernel_address;
        struct
        {
            void *imcb$ps_cmod_kernel_address;
        };
    };
    union
    {
        long long imcb$q_cmod_exec_address;
        struct
        {
            void *imcb$ps_cmod_exec_address;
        };
    };
    union
    {
        long long imcb$q_ssi_plv;
        struct
        {
            struct _plv *imcb$ps_ssi_plv;
        };
    };
    struct _kferes *imcb$l_kferes_ptr;
    char imcb$t_log_image_name [40];
    char imcb$t_dvi [16];
    unsigned short int imcb$w_fid [3];
    char imcb$b_risig [32];
    short int imcb$w_fill1;
    struct _kferes *imcb$l_kferes64_ptr;
    void *imcb$pq_starting_address_64;
    void *imcb$pq_end_address_64;
    unsigned long long imcb$q_linktime;
};

#endif

