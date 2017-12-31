#ifndef pqbdef_h
#define pqbdef_h

#define PQB$M_IMGDMP         0x1
#define PQB$M_DEBUG          0x2
#define PQB$M_DBGTRU         0x4
#define PQB$M_PARSE_EXTENDED 0x8
#define PQB$K_LENGTH 2264
#define PQB$C_LENGTH 2264
#define PQB$S_PQBDEF 2264

struct _pqb
{
    unsigned long long pqb$q_prvmsk;
    unsigned short int pqb$w_size;
    unsigned char pqb$b_type;
    unsigned char pqb$b_sts;
    unsigned int pqb$l_astlm;
    unsigned int pqb$l_biolm;
    unsigned int pqb$l_bytlm;
    unsigned int pqb$l_cpulm;
    unsigned int pqb$l_diolm;
    unsigned int pqb$l_fillm;
    unsigned int pqb$l_pgflquota;
    unsigned int pqb$l_prclm;
    unsigned int pqb$l_tqelm;
    unsigned int pqb$l_wsquota;
    unsigned int pqb$l_wsdefault;
    unsigned int pqb$l_enqlm;
    unsigned int pqb$l_wsextent;
    unsigned int pqb$l_jtquota;
    union
    {
        unsigned short int pqb$w_flags;
        struct
        {
            unsigned pqb$v_imgdmp		: 1;
            unsigned pqb$v_debug		: 1;
            unsigned pqb$v_dbgtru		: 1;
            unsigned pqb$v_parse_extended	: 1;
            unsigned pqb$v_fill_2_		: 4;
        };
    };
    unsigned char pqb$b_msgmask;
    unsigned char pqb$b_fill_1;
    unsigned int pqb$l_uaf_flags;
    unsigned int pqb$l_creprc_flags;
    struct
    {
        unsigned char pqb$$$_fill_2 [20];
    };
    struct
    {
        unsigned char pqb$$$_fill_3 [20];
    };
    unsigned int pqb$l_input_att;
    unsigned int pqb$l_output_att;
    unsigned int pqb$l_error_att;
    unsigned int pqb$l_disk_att;
    char pqb$t_cli_name [32];
    char pqb$t_cli_table [256];
    char pqb$t_spawn_cli [32];
    char pqb$t_spawn_table [256];
    char pqb$t_input [256];
    char pqb$t_output [256];
    char pqb$t_error [256];
    char pqb$t_disk [256];
    char pqb$t_ddstring [256];
    char pqb$t_image [256];
    char pqb$t_account [8];
    unsigned int pqb$l_arb_support;
    unsigned int pqb$l_rms_lcs;
};

#endif

