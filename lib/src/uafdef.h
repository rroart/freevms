#ifndef UAFDEF_H
#define UAFDEF_H

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
    unsigned char uaf$b_rtype;
    unsigned char uaf$b_version;
    unsigned short int uaf$w_usrdatoff;
    union
    {
        char uaf$t_username [32];
        struct
        {
            char uaf$t_fill_0 [31];
            char uaf$t_username_tag;
        };
    };
    union
    {
        unsigned int uaf$l_uic;
        struct
        {
            unsigned short int uaf$w_mem;
            unsigned short int uaf$w_grp;
        };
    };
    unsigned int uaf$l_sub_id;
    unsigned long long uaf$q_parent_id;
    char uaf$t_account [32];
    char uaf$t_owner [32];
    char uaf$t_defdev [32];
    char uaf$t_defdir [64];
    char uaf$t_lgicmd [64];
    char uaf$t_defcli [32];
    char uaf$t_clitables [32];
    union
    {
        unsigned long long uaf$q_pwd;
        struct
        {
            unsigned int uaf$l_pwd;
        };
    };
    unsigned long long uaf$q_pwd2;
    unsigned short int uaf$w_logfails;
    unsigned short int uaf$w_salt;
    unsigned char uaf$b_encrypt;
    unsigned char uaf$b_encrypt2;
    unsigned char uaf$b_pwd_length;
    char uaf$$$_fill_1 [1];
    unsigned long long uaf$q_expiration;
    unsigned long long uaf$q_pwd_lifetime;
    unsigned long long uaf$q_pwd_date;
    unsigned long long uaf$q_pwd2_date;
    unsigned long long uaf$q_lastlogin_i;
    unsigned long long uaf$q_lastlogin_n;
    unsigned long long uaf$q_priv;
    unsigned long long uaf$q_def_priv;
    struct
    {
        char uaf$b_fill_2 [20];
    };
    struct
    {
        char uaf$b_fill_3 [20];
    };
    union
    {
        unsigned int uaf$l_flags;
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
    unsigned char uaf$b_network_access_p [3];
    unsigned char uaf$b_network_access_s [3];
    unsigned char uaf$b_batch_access_p [3];
    unsigned char uaf$b_batch_access_s [3];
    unsigned char uaf$b_local_access_p [3];
    unsigned char uaf$b_local_access_s [3];
    unsigned char uaf$b_dialup_access_p [3];
    unsigned char uaf$b_dialup_access_s [3];
    unsigned char uaf$b_remote_access_p [3];
    unsigned char uaf$b_remote_access_s [3];
    char uaf$$$_fill_4 [12];
    union
    {
        unsigned char uaf$b_primedays;
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
    char uaf$$$_fill_5 [1];
    unsigned char uaf$b_pri;
    unsigned char uaf$b_quepri;
    unsigned short int uaf$w_maxjobs;
    unsigned short int uaf$w_maxacctjobs;
    unsigned short int uaf$w_maxdetach;
    unsigned short int uaf$w_prccnt;
    unsigned short int uaf$w_biolm;
    unsigned short int uaf$w_diolm;
    unsigned short int uaf$w_tqcnt;
    unsigned short int uaf$w_astlm;
    unsigned short int uaf$w_enqlm;
    unsigned short int uaf$w_fillm;
    unsigned short int uaf$w_shrfillm;
    unsigned int uaf$l_wsquota;
    unsigned int uaf$l_dfwscnt;
    unsigned int uaf$l_wsextent;
    unsigned int uaf$l_pgflquota;
    unsigned int uaf$l_cputim;
    unsigned int uaf$l_bytlm;
    unsigned int uaf$l_pbytlm;
    unsigned int uaf$l_jtquota;
    unsigned short int uaf$w_proxy_lim;
    unsigned short int uaf$w_proxies;
    unsigned short int uaf$w_account_lim;
    unsigned short int uaf$w_accounts;
    struct
    {
        char uaf$b_fill_6 [20];
    };
    char uaf$$$_fill_99 [44];
    char uaf$$$_fill_100 [768];
};

#endif

