#ifndef CWPSDEF_H
#define CWPSDEF_H

#define     CWPSSRV$M_BTX_DONE      0x1
#define     CWPSSRV$M_SEC_CLASS     0x1
#define     CWPSSRV$M_JPIBUF        0x2
#define     CWPSSRV$M_NOQUOTA       0x4
#define     CWPSSRV$M_GROUP_PRIV        0x8
#define     CWPSSRV$M_WORLD_PRIV        0x10
#define     CWPSSRV$M_SECAUDIT      0x20
#define     CWPSSRV$M_NOAUDIT       0x40
#define     CWPSSRV$M_OPER_PRIV     0x80
#define     CWPSSRV$K_RETRY_CNT     5

#define     CWPSSRV$K_LENGTH        112
#define     CWPSSRV$K_SRVCODE_BEGIN     0
#define     CWPSSRV$K_CANWAK        1
#define     CWPSSRV$K_DELPRC        2
#define     CWPSSRV$K_FORCEX        3
#define     CWPSSRV$K_RESUME        4
#define     CWPSSRV$K_SCHDWK        5
#define     CWPSSRV$K_SETPRI        6
#define     CWPSSRV$K_SUSPND        7
#define     CWPSSRV$K_WAKE              8
#define     CWPSSRV$K_SRVCODE_END       9

#define     CWPSSRV$K_GETJPI        20
#define     CWPSSRV$K_CREPRC        21
#define     CWPSSRV$K_TERMIN        22

#define     CWPSSRV$K_GRANTID       28
#define     CWPSSRV$K_REVOKID       29
#define     CWPSSRV$K_VERSION_1     1
#define     CWPSSRV$K_VERSION_2     2
#define     CWPSSRV$K_VERSION_3     3
#define     CWPSSRV$K_VERSION_4     4
#define     CWPSSRV$K_VERSION_5     5
#define     CWPSSRV$K_VERSION_6     6
#define     CWPSSRV$K_VERSION_7     7
#define     CWPSSRV$K_VERSION_8     8
#define     CWPSSRV$K_VERSION_9     9
#define     CWPSSRV$K_VERSION_10        10
#define     CWPSSRV$K_VERSION_11        11
#define     CWPSSRV$K_VERSION_12        12
#define     CWPSSRV$K_INITIAL_MAJ_VERS  1
#define     CWPSSRV$K_INITIAL_MIN_VERS  1
#define     CWPSSRV$S_$CWPSSRVDEF       112

struct _cwpssrv
{
    unsigned int cwpssrv$l_send_length;
    unsigned int cwpssrv$l_return_length;
    unsigned short int cwpssrv$w_size;
    unsigned char cwpssrv$b_type;
    unsigned char cwpssrv$b_subtype;
    unsigned short int cwpssrv$w_btx_status;
    union
    {
        unsigned char cwpssrv$b_btx_flags;
        struct
        {
            unsigned cwpssrv$v_btx_done     : 1;
            unsigned cwpssrv$v_fill_4_      : 7;
        };
    };
    unsigned char cwpssrv$b_func;
    unsigned short int cwpssrv$w_srv_maj_vers;
    unsigned short int cwpssrv$w_srv_min_vers;
    unsigned short int cwpssrv$w_ext_maj_vers;
    unsigned short int cwpssrv$w_ext_min_vers;
    unsigned int cwpssrv$l_status;
    unsigned int cwpssrv$l_bxfr_status;
    unsigned int cwpssrv$l_maximum_length;
    union
    {
        unsigned int cwpssrv$l_flags;
        struct
        {
            unsigned cwpssrv$v_sec_class        : 1;
            unsigned cwpssrv$v_jpibuf               : 1;
            unsigned cwpssrv$v_noquota      : 1;
            unsigned cwpssrv$v_group_priv       : 1;
            unsigned cwpssrv$v_world_priv       : 1;
            unsigned cwpssrv$v_secaudit     : 1;
            unsigned cwpssrv$v_noaudit      : 1;
            unsigned cwpssrv$v_oper_priv        : 1;
        };
    };
    unsigned int cwpssrv$l_ext_offset;
    void *cwpssrv$a_post_routine;
    unsigned int cwpssrv$l_rqstr_csid;
    unsigned int cwpssrv$l_rqstr_pid;
    unsigned int cwpssrv$l_rqstr_epid;
    unsigned int cwpssrv$l_rqstr_imgcnt;
    unsigned int cwpssrv$l_rqstr_rightslen;
    unsigned int cwpssrv$l_rqstr_rightsoff;
    void *cwpssrv$l_rqstr_pidadr;
    unsigned int cwpssrv$l_partner_csid;
    unsigned int cwpssrv$l_sought_epid;
    unsigned int cwpssrv$l_return_epid;
    unsigned short int cwpssrv$w_prcnamlen;
    unsigned short int cwpssrv$w_prcnamoff;
    unsigned int cwpssrv$l_free_offset;
    unsigned int cwpssrv$l_spare1;
    unsigned short int cwpssrv$w_spare2;
    unsigned char cwpssrv$b_spare3;
    unsigned char cwpssrv$b_retries_left;
    unsigned long long cwpssrv$q_time;
};

#define     CWPS$_ENABLED       1
#define     CWPS$_NERVOUS       0
#define     CWPS$_SCARED        0
#define     CWPS$_PARANOID      0
#define     CWPS$_STATISTICS    1
#define     CWPSCAN$K_LENGTH    0
#define     CWPSDEL$K_LENGTH    0
#define     CWPSRES$K_LENGTH    0
#define     CWPSSUS$K_LENGTH    0
#define     CWPSWAK$K_LENGTH    0
#define     CWPSFEX$K_LENGTH    4
#define     CWPSFEX$S_$CWPSFEXDEF   4

struct _cwpsfex
{
    unsigned int cwpsfex$l_code;
};

#define     CWPSSRV$K_SETPRI_MIN_VERS2  2
#define     CWPSPRI$K_LENGTH        24
#define     CWPSPRI$S_$CWPSPRIDEF       24

struct _cwpspri
{
    void *cwpspri$l_user_prvpri;
    unsigned int cwpspri$l_pri;
    unsigned int cwpspri$l_prvpri;
    void *cwpspri$l_user_prvpol;
    unsigned int cwpspri$l_pol;
    unsigned int cwpspri$l_prvpol;
};

#define     CWPSSWK$K_LENGTH        16
#define     CWPSSWK$S_$CWPSSWKDEF       16

struct _cwpsswk
{
    unsigned long long cwpsswk$q_daytim;
    unsigned long long cwpsswk$q_reptim;
};

#define     CWPSJPI$K_LENGTH        60
#define     CWPSJPI$S_$CWPSJPIDEF       60

struct _cwpsjpi
{
    unsigned int cwpsjpi$l_pscan_off;
    unsigned int cwpsjpi$l_itmoff;
    unsigned int cwpsjpi$l_bufoff;
    unsigned int cwpsjpi$l_vecoff;
    unsigned int cwpsjpi$l_acboff;
    void *cwpsjpi$l_itmlst;
    unsigned int cwpsjpi$l_buflen;
    void *cwpsjpi$l_iosbadr;
    unsigned long long cwpsjpi$q_iosb;
    void (*cwpsjpi$l_astadr)(void);
    unsigned int cwpsjpi$l_astprm;
    void *cwpsjpi$l_pscanctx_addr;
    unsigned short int cwpsjpi$w_pscan_seqnum;
    unsigned char cwpsjpi$b_efn;
    unsigned char cwpsjpi$b_acmode;
    unsigned short int cwpsjpi$w_ctlflags;
    unsigned short int cwpsjpi$w_spare0;
};

#define CWPSJPI_64$K_LENGTH 40

struct _cwpsjpi_64
{
    void *cwpsjpi_64$pq_rqstr_pidadr;
    void *cwpsjpi_64$pq_itmlst;
    void *cwpsjpi_64$pq_iosbadr;
    void *cwpsjpi_64$pq_astadr;
    void *cwpsjpi_64$pq_astprm;
};

#define     CWPSCRE$M_IMGDMP        0x1
#define     CWPSCRE$M_DEBUG             0x2
#define     CWPSCRE$M_DBGTRU        0x4
#define     CWPSCRE$M_PARSE_EXTENDED    0x8
#define     CWPSCRE$K_LENGTH        2100
#define     CWPSCRE$S_$CWPSCREDEF       2100

struct _cwpscreprc
{
    unsigned int cwpscre$l_base_offset;
    unsigned int cwpscre$l_acboff;
    unsigned long long cwpscre$q_prvmsk;
    unsigned long long cwpscre$q_iosb;
    unsigned int cwpscre$l_priority;
    unsigned int cwpscre$l_uic;
    unsigned int cwpscre$l_defprot;
    unsigned int cwpscre$l_tmbu;
    unsigned int cwpscre$l_spare1;
    unsigned int cwpscre$l_spare2;
    unsigned short int cwpscre$w_maxjobs;
    unsigned short int cwpscre$w_maxdetach;
    unsigned int cwpscre$l_astlm;
    unsigned int cwpscre$l_biolm;
    unsigned int cwpscre$l_bytlm;
    unsigned int cwpscre$l_cpulm;
    unsigned int cwpscre$l_diolm;
    unsigned int cwpscre$l_fillm;
    unsigned int cwpscre$l_pgflquota;
    unsigned int cwpscre$l_prclm;
    unsigned int cwpscre$l_tqelm;
    unsigned int cwpscre$l_wsquota;
    unsigned int cwpscre$l_wsdefault;
    unsigned int cwpscre$l_enqlm;
    unsigned int cwpscre$l_wsextent;
    unsigned int cwpscre$l_jtquota;
    unsigned int cwpscre$l_spare_quota1;
    unsigned int cwpscre$l_spare_quota2;
    unsigned int cwpscre$l_input_att;
    unsigned int cwpscre$l_output_att;
    unsigned int cwpscre$l_error_att;
    unsigned int cwpscre$l_msgmask;
    unsigned int cwpscre$l_uaf_flags;
    unsigned int cwpscre$l_creprc_flags;
    union
    {
        unsigned int cwpscre$l_flags;
        struct
        {
            unsigned cwpscre$v_imgdmp       : 1;
            unsigned cwpscre$v_debug        : 1;
            unsigned cwpscre$v_dbgtru       : 1;
            unsigned cwpscre$v_parse_extended   : 1;
            unsigned cwpscre$v_fill_7_  : 4;
        };
    };
    char cwpscre$t_input [256];
    char cwpscre$t_output [256];
    char cwpscre$t_error [256];
    char cwpscre$t_disk [256];
    char cwpscre$t_image [256];
    char cwpscre$t_ddstring [256];
    char cwpscre$t_username [12];
    char cwpscre$t_account [8];
    char cwpscre$t_pqb_account [8];
    struct
    {
        unsigned char cwpscre$$$_fill_2 [20];
    };
    struct
    {
        unsigned char cwpscre$$$_fill_3 [20];
    };
    char cwpscre$t_cli_name [32];
    char cwpscre$t_cli_table [32];
    char cwpscre$t_spawn_cli [32];
    char cwpscre$t_spawn_table [256];
};

#define CWPSTERM$K_LENGTH       108
#define CWPSTERM$S_$CWPSTERMDEF 108

struct _cwpsterm
{
    unsigned int cwpsterm$l_base_offset;
    unsigned int cwpsterm$l_acboff;
    unsigned long long cwpsterm$q_iosb;
    unsigned int cwpsterm$l_deleted_proc_epid;
    unsigned int cwpsterm$l_spare;
    char cwpsterm$t_term_msg [84];
};

#define CWPSVEC$M_64_BIT_ILE  0x1
#define CWPSVEC$K_LENGTH      24
#define CWPSVEC$S_$CWPSVECDEF 24

struct _cwpsvec
{
    void *cwpsvec$pq_usr_bufadr;
    void *cwpsvec$pq_usr_lenadr;
    union
    {
        unsigned int cwpsvec$l_flags;
        struct
        {
            unsigned cwpsvec$v_64_bit_ile       : 1;
            unsigned cwpsvec$v_fill_10_     : 7;
        };
    };
    int cwpsvec$l_fill_1;
};

#define     CWPSACB$K_ACB_LENGTH        36
#define     CWPSACB$K_LENGTH        80
#define     CWPSACB$S_$CWPSACBDEF       81

struct _cwpsacb
{
    struct _acb *cwpsacb$l_astqfl;
    struct _acb *cwpsacb$l_astqbl;
    unsigned short cwpsacb$w_size;
    unsigned char cwpsacb$b_type;
    unsigned char cwpsacb$b_rmod;
    unsigned int cwpsacb$l_pid;
    void (*cwpsacb$l_ast)(void);
    unsigned int cwpsacb$l_astprm;
    int cwpsacb$l_fkb_fill [2];
    void (*cwpsacb$l_kast)(void);
    void *cwpsacb$l_bufadr;
    unsigned int cwpsacb$l_buflen;
    void *cwpsacb$l_msgbuf;
    struct _csb *cwpsacb$l_csb;
    struct _cdrp *cwpsacb$l_cdrp;
    unsigned int cwpsacb$l_read_length;
    unsigned int cwpsacb$l_write_length;
    unsigned int cwpsacb$l_rightsdesc [2];
    unsigned char cwpsacb$b_func;
    unsigned char cwpsacb$b_spare0;
    unsigned short cwpsacb$w_spare1;
    struct _psb *cwpsacb$ar_remote_psb;
    unsigned char cwpsacb$b_trojan_pcb;
};

#define CWPSSQH$K_LENGTH      32
#define CWPSSQH$S_$CWPSSQHDEF 32

struct _cwpssqh
{
    struct _cwpssqh *cwpssqh$l_flink;
    struct _cwpssqh *cwpssqh$l_blink;
    unsigned short cwpssqh$w_size;
    unsigned char cwpssqh$b_type;
    unsigned char cwpssqh$b_subtype;
    unsigned int cwpssqh$l_alloc_length;
    unsigned int cwpssqh$l_mpid;
    unsigned int cwpssqh$l_spare0;
    unsigned int cwpssqh$l_spare1;
    unsigned int cwpssqh$l_spare3;
};

#define CWPSNODIH$K_HEADER        16
#define CWPSNODIH$S_$CWPSNODIHDEF 17

struct _cwpsnodih
{
    struct _cwpsnodih *cwpsnodih$l_flink;
    struct _cwpsnodih *cwpsnodih$l_blink;
    unsigned short cwpsnodih$w_size;
    unsigned char cwpsnodih$b_type;
    unsigned char cwpsnodih$b_subtype;
    unsigned int cwpsnodih$l_count;
    unsigned char cwpsnodih$b_node_list;
};

#define CWPSNODI$K_LENGTH       32
#define CWPSNODI$S_$CWPSNODIDEF 32

struct _cwpsnodi
{
    unsigned int cwpsnodi$l_hwtype;
    unsigned short cwpsnodi$w_hw_model;
    unsigned short cwpsnodi$w_sparew;
    unsigned int cwpsnodi$l_csid;
    unsigned char cwpsnodi$b_name [16];
    struct _sb *cwpsnodi$l_sb;
};

#endif

