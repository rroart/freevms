#ifndef CWPSDEF_H
#define CWPSDEF_H

#include <vms_types.h>

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
    UINT32 cwpssrv$l_send_length;
    UINT32 cwpssrv$l_return_length;
    UINT16 cwpssrv$w_size;
    UINT8 cwpssrv$b_type;
    UINT8 cwpssrv$b_subtype;
    UINT16 cwpssrv$w_btx_status;
    union
    {
        UINT8 cwpssrv$b_btx_flags;
        struct
        {
            unsigned cwpssrv$v_btx_done     : 1;
            unsigned cwpssrv$v_fill_4_      : 7;
        };
    };
    UINT8 cwpssrv$b_func;
    UINT16 cwpssrv$w_srv_maj_vers;
    UINT16 cwpssrv$w_srv_min_vers;
    UINT16 cwpssrv$w_ext_maj_vers;
    UINT16 cwpssrv$w_ext_min_vers;
    UINT32 cwpssrv$l_status;
    UINT32 cwpssrv$l_bxfr_status;
    UINT32 cwpssrv$l_maximum_length;
    union
    {
        UINT32 cwpssrv$l_flags;
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
    UINT32 cwpssrv$l_ext_offset;
    void *cwpssrv$a_post_routine;
    UINT32 cwpssrv$l_rqstr_csid;
    UINT32 cwpssrv$l_rqstr_pid;
    UINT32 cwpssrv$l_rqstr_epid;
    UINT32 cwpssrv$l_rqstr_imgcnt;
    UINT32 cwpssrv$l_rqstr_rightslen;
    UINT32 cwpssrv$l_rqstr_rightsoff;
    void *cwpssrv$l_rqstr_pidadr;
    UINT32 cwpssrv$l_partner_csid;
    UINT32 cwpssrv$l_sought_epid;
    UINT32 cwpssrv$l_return_epid;
    UINT16 cwpssrv$w_prcnamlen;
    UINT16 cwpssrv$w_prcnamoff;
    UINT32 cwpssrv$l_free_offset;
    UINT32 cwpssrv$l_spare1;
    UINT16 cwpssrv$w_spare2;
    UINT8 cwpssrv$b_spare3;
    UINT8 cwpssrv$b_retries_left;
    UINT64 cwpssrv$q_time;
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
    UINT32 cwpsfex$l_code;
};

#define     CWPSSRV$K_SETPRI_MIN_VERS2  2
#define     CWPSPRI$K_LENGTH        24
#define     CWPSPRI$S_$CWPSPRIDEF       24

struct _cwpspri
{
    void *cwpspri$l_user_prvpri;
    UINT32 cwpspri$l_pri;
    UINT32 cwpspri$l_prvpri;
    void *cwpspri$l_user_prvpol;
    UINT32 cwpspri$l_pol;
    UINT32 cwpspri$l_prvpol;
};

#define     CWPSSWK$K_LENGTH        16
#define     CWPSSWK$S_$CWPSSWKDEF       16

struct _cwpsswk
{
    UINT64 cwpsswk$q_daytim;
    UINT64 cwpsswk$q_reptim;
};

#define     CWPSJPI$K_LENGTH        60
#define     CWPSJPI$S_$CWPSJPIDEF       60

struct _cwpsjpi
{
    UINT32 cwpsjpi$l_pscan_off;
    UINT32 cwpsjpi$l_itmoff;
    UINT32 cwpsjpi$l_bufoff;
    UINT32 cwpsjpi$l_vecoff;
    UINT32 cwpsjpi$l_acboff;
    void *cwpsjpi$l_itmlst;
    UINT32 cwpsjpi$l_buflen;
    void *cwpsjpi$l_iosbadr;
    UINT64 cwpsjpi$q_iosb;
    void (*cwpsjpi$l_astadr)(void);
    UINT32 cwpsjpi$l_astprm;
    void *cwpsjpi$l_pscanctx_addr;
    UINT16 cwpsjpi$w_pscan_seqnum;
    UINT8 cwpsjpi$b_efn;
    UINT8 cwpsjpi$b_acmode;
    UINT16 cwpsjpi$w_ctlflags;
    UINT16 cwpsjpi$w_spare0;
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
    UINT32 cwpscre$l_base_offset;
    UINT32 cwpscre$l_acboff;
    UINT64 cwpscre$q_prvmsk;
    UINT64 cwpscre$q_iosb;
    UINT32 cwpscre$l_priority;
    UINT32 cwpscre$l_uic;
    UINT32 cwpscre$l_defprot;
    UINT32 cwpscre$l_tmbu;
    UINT32 cwpscre$l_spare1;
    UINT32 cwpscre$l_spare2;
    UINT16 cwpscre$w_maxjobs;
    UINT16 cwpscre$w_maxdetach;
    UINT32 cwpscre$l_astlm;
    UINT32 cwpscre$l_biolm;
    UINT32 cwpscre$l_bytlm;
    UINT32 cwpscre$l_cpulm;
    UINT32 cwpscre$l_diolm;
    UINT32 cwpscre$l_fillm;
    UINT32 cwpscre$l_pgflquota;
    UINT32 cwpscre$l_prclm;
    UINT32 cwpscre$l_tqelm;
    UINT32 cwpscre$l_wsquota;
    UINT32 cwpscre$l_wsdefault;
    UINT32 cwpscre$l_enqlm;
    UINT32 cwpscre$l_wsextent;
    UINT32 cwpscre$l_jtquota;
    UINT32 cwpscre$l_spare_quota1;
    UINT32 cwpscre$l_spare_quota2;
    UINT32 cwpscre$l_input_att;
    UINT32 cwpscre$l_output_att;
    UINT32 cwpscre$l_error_att;
    UINT32 cwpscre$l_msgmask;
    UINT32 cwpscre$l_uaf_flags;
    UINT32 cwpscre$l_creprc_flags;
    union
    {
        UINT32 cwpscre$l_flags;
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
        UINT8 cwpscre$$$_fill_2 [20];
    };
    struct
    {
        UINT8 cwpscre$$$_fill_3 [20];
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
    UINT32 cwpsterm$l_base_offset;
    UINT32 cwpsterm$l_acboff;
    UINT64 cwpsterm$q_iosb;
    UINT32 cwpsterm$l_deleted_proc_epid;
    UINT32 cwpsterm$l_spare;
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
        UINT32 cwpsvec$l_flags;
        struct
        {
            unsigned cwpsvec$v_64_bit_ile       : 1;
            unsigned cwpsvec$v_fill_10_     : 7;
        };
    };
    INT32 cwpsvec$l_fill_1;
};

#define     CWPSACB$K_ACB_LENGTH        36
#define     CWPSACB$K_LENGTH        80
#define     CWPSACB$S_$CWPSACBDEF       81

struct _cwpsacb
{
    struct _acb *cwpsacb$l_astqfl;
    struct _acb *cwpsacb$l_astqbl;
    UINT16 cwpsacb$w_size;
    UINT8 cwpsacb$b_type;
    UINT8 cwpsacb$b_rmod;
    UINT32 cwpsacb$l_pid;
    void (*cwpsacb$l_ast)(void);
    UINT32 cwpsacb$l_astprm;
    INT32 cwpsacb$l_fkb_fill [2];
    void (*cwpsacb$l_kast)(void);
    void *cwpsacb$l_bufadr;
    UINT32 cwpsacb$l_buflen;
    void *cwpsacb$l_msgbuf;
    struct _csb *cwpsacb$l_csb;
    struct _cdrp *cwpsacb$l_cdrp;
    UINT32 cwpsacb$l_read_length;
    UINT32 cwpsacb$l_write_length;
    UINT32 cwpsacb$l_rightsdesc [2];
    UINT8 cwpsacb$b_func;
    UINT8 cwpsacb$b_spare0;
    UINT16 cwpsacb$w_spare1;
    struct _psb *cwpsacb$ar_remote_psb;
    UINT8 cwpsacb$b_trojan_pcb;
};

#define CWPSSQH$K_LENGTH      32
#define CWPSSQH$S_$CWPSSQHDEF 32

struct _cwpssqh
{
    struct _cwpssqh *cwpssqh$l_flink;
    struct _cwpssqh *cwpssqh$l_blink;
    UINT16 cwpssqh$w_size;
    UINT8 cwpssqh$b_type;
    UINT8 cwpssqh$b_subtype;
    UINT32 cwpssqh$l_alloc_length;
    UINT32 cwpssqh$l_mpid;
    UINT32 cwpssqh$l_spare0;
    UINT32 cwpssqh$l_spare1;
    UINT32 cwpssqh$l_spare3;
};

#define CWPSNODIH$K_HEADER        16
#define CWPSNODIH$S_$CWPSNODIHDEF 17

struct _cwpsnodih
{
    struct _cwpsnodih *cwpsnodih$l_flink;
    struct _cwpsnodih *cwpsnodih$l_blink;
    UINT16 cwpsnodih$w_size;
    UINT8 cwpsnodih$b_type;
    UINT8 cwpsnodih$b_subtype;
    UINT32 cwpsnodih$l_count;
    UINT8 cwpsnodih$b_node_list;
};

#define CWPSNODI$K_LENGTH       32
#define CWPSNODI$S_$CWPSNODIDEF 32

struct _cwpsnodi
{
    UINT32 cwpsnodi$l_hwtype;
    UINT16 cwpsnodi$w_hw_model;
    UINT16 cwpsnodi$w_sparew;
    UINT32 cwpsnodi$l_csid;
    UINT8 cwpsnodi$b_name [16];
    struct _sb *cwpsnodi$l_sb;
};

#endif

