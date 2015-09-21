#ifndef UAFDEF_H
#define UAFDEF_H

#include <vms_types.h>

#define     UAF$C_USER_ID           1
#define     UAF$C_VERSION1          1
#define     UAF$C_KEYED_PART        52
#define     UAF$C_MAX_PWD_LENGTH        32
#define     UAF$K_MAX_PWD_LENGTH        32
#define     UAF$C_AD_II         0
#define     UAF$C_PURDY         1
#define     UAF$C_PURDY_V           2
#define     UAF$C_PURDY_S           3
#define     UAF$K_CURRENT_ALGORITHM     3
#define     UAF$C_CURRENT_ALGORITHM     3
#define     UAF$C_PREFERED_ALGORITHM    127
#define     UAF$K_PREFERED_ALGORITHM    127
#define     UAF$C_PREFERRED_ALGORITHM   127
#define     UAF$K_PREFERRED_ALGORITHM   127
#define     UAF$C_CUST_ALGORITHM        128
#define     UAF$K_CUST_ALGORITHM        128
#define     UAF$K_FIXED         644
#define     UAF$C_FIXED         644
#define     UAF$K_LENGTH            1412
#define     UAF$C_LENGTH            1412
#define     UAF$S_UAFDEF            1412

struct _uaf
{
    UINT8 uaf$b_rtype;
    UINT8 uaf$b_version;
    UINT16 uaf$w_usrdatoff;
    union
    {
        char uaf$t_username [32];
        struct
        {
            INT8 uaf$t_fill_0 [31];
            char uaf$t_username_tag;
        };
    };
    union
    {
        UINT32 uaf$l_uic;
        struct
        {
            UINT16 uaf$w_mem;
            UINT16 uaf$w_grp;
        };
    };
    UINT32 uaf$l_sub_id;
    UINT64 uaf$q_parent_id;
    char uaf$t_account [32];
    char uaf$t_owner [32];
    char uaf$t_defdev [32];
    char uaf$t_defdir [64];
    char uaf$t_lgicmd [64];
    char uaf$t_defcli [32];
    char uaf$t_clitables [32];
    union
    {
        UINT64 uaf$q_pwd;
        struct
        {
            UINT32 uaf$l_pwd;
        };
    };
    UINT64 uaf$q_pwd2;
    UINT16 uaf$w_logfails;
    UINT16 uaf$w_salt;
    UINT8 uaf$b_encrypt;
    UINT8 uaf$b_encrypt2;
    UINT8 uaf$b_pwd_length;
    INT8 uaf$$$_fill_1 [1];
    UINT64 uaf$q_expiration;
    UINT64 uaf$q_pwd_lifetime;
    UINT64 uaf$q_pwd_date;
    UINT64 uaf$q_pwd2_date;
    UINT64 uaf$q_lastlogin_i;
    UINT64 uaf$q_lastlogin_n;
    UINT64 uaf$q_priv;
    UINT64 uaf$q_def_priv;
    struct
    {
        INT8 uaf$b_fill_2 [20];
    };
    struct
    {
        INT8 uaf$b_fill_3 [20];
    };
    union
    {
        UINT32 uaf$l_flags;
        struct
        {
            unsigned uaf$v_disctly          : 1;
            unsigned uaf$v_defcli           : 1;
            unsigned uaf$v_lockpwd          : 1;
            unsigned uaf$v_restricted           : 1;
            unsigned uaf$v_disacnt          : 1;
            unsigned uaf$v_diswelcom            : 1;
            unsigned uaf$v_dismail          : 1;
            unsigned uaf$v_nomail           : 1;
            unsigned uaf$v_genpwd           : 1;
            unsigned uaf$v_pwd_expired      : 1;
            unsigned uaf$v_pwd2_expired     : 1;
            unsigned uaf$v_audit            : 1;
            unsigned uaf$v_disreport            : 1;
            unsigned uaf$v_disreconnect     : 1;
            unsigned uaf$v_autologin            : 1;
            unsigned uaf$v_disforce_pwd_change  : 1;
            unsigned uaf$v_captive          : 1;
            unsigned uaf$v_disimage         : 1;
            unsigned uaf$v_dispwddic            : 1;
            unsigned uaf$v_dispwdhis            : 1;
            unsigned uaf$v_defclsval            : 1;
            unsigned uaf$v_extauth          : 1;
            unsigned uaf$v_migratepwd           : 1;
            unsigned uaf$v_vmsauth          : 1;
            unsigned uaf$v_dispwdsynch      : 1;
            unsigned uaf$v_fill_10_         : 7;
        };
    };
    UINT8 uaf$b_network_access_p [3];
    UINT8 uaf$b_network_access_s [3];
    UINT8 uaf$b_batch_access_p [3];
    UINT8 uaf$b_batch_access_s [3];
    UINT8 uaf$b_local_access_p [3];
    UINT8 uaf$b_local_access_s [3];
    UINT8 uaf$b_dialup_access_p [3];
    UINT8 uaf$b_dialup_access_s [3];
    UINT8 uaf$b_remote_access_p [3];
    UINT8 uaf$b_remote_access_s [3];
    UINT8 uaf$$$_fill_4 [12];
    union
    {
        UINT8 uaf$b_primedays;
        struct
        {
            unsigned uaf$v_monday       : 1;
            unsigned uaf$v_tuesday      : 1;
            unsigned uaf$v_wednesday        : 1;
            unsigned uaf$v_thursday     : 1;
            unsigned uaf$v_friday       : 1;
            unsigned uaf$v_saturday     : 1;
            unsigned uaf$v_sunday       : 1;
            unsigned uaf$v_fill_11_     : 1;
        };
    };
    UINT8 uaf$$$_fill_5 [1];
    UINT8 uaf$b_pri;
    UINT8 uaf$b_quepri;
    UINT16 uaf$w_maxjobs;
    UINT16 uaf$w_maxacctjobs;
    UINT16 uaf$w_maxdetach;
    UINT16 uaf$w_prccnt;
    UINT16 uaf$w_biolm;
    UINT16 uaf$w_diolm;
    UINT16 uaf$w_tqcnt;
    UINT16 uaf$w_astlm;
    UINT16 uaf$w_enqlm;
    UINT16 uaf$w_fillm;
    UINT16 uaf$w_shrfillm;
    UINT32 uaf$l_wsquota;
    UINT32 uaf$l_dfwscnt;
    UINT32 uaf$l_wsextent;
    UINT32 uaf$l_pgflquota;
    UINT32 uaf$l_cputim;
    UINT32 uaf$l_bytlm;
    UINT32 uaf$l_pbytlm;
    UINT32 uaf$l_jtquota;
    UINT16 uaf$w_proxy_lim;
    UINT16 uaf$w_proxies;
    UINT16 uaf$w_account_lim;
    UINT16 uaf$w_accounts;
    struct
    {
        UINT8 uaf$b_fill_6 [20];
    };
    UINT8 uaf$$$_fill_99 [44];
    UINT8 uaf$$$_fill_100 [768];
};

#endif

