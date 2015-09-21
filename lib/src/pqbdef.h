#ifndef PQBDEF_H
#define PQBDEF_H

#include <vms_types.h>

#define PQB$M_IMGDMP         0x1
#define PQB$M_DEBUG          0x2
#define PQB$M_DBGTRU         0x4
#define PQB$M_PARSE_EXTENDED 0x8
#define PQB$K_LENGTH 2264
#define PQB$C_LENGTH 2264
#define PQB$S_PQBDEF 2264

struct _pqb
{
    UINT64 pqb$q_prvmsk;
    UINT16 pqb$w_size;
    UINT8 pqb$b_type;
    UINT8 pqb$b_sts;
    UINT32 pqb$l_astlm;
    UINT32 pqb$l_biolm;
    UINT32 pqb$l_bytlm;
    UINT32 pqb$l_cpulm;
    UINT32 pqb$l_diolm;
    UINT32 pqb$l_fillm;
    UINT32 pqb$l_pgflquota;
    UINT32 pqb$l_prclm;
    UINT32 pqb$l_tqelm;
    UINT32 pqb$l_wsquota;
    UINT32 pqb$l_wsdefault;
    UINT32 pqb$l_enqlm;
    UINT32 pqb$l_wsextent;
    UINT32 pqb$l_jtquota;
    union
    {
        UINT16 pqb$w_flags;
        struct
        {
            unsigned pqb$v_imgdmp       : 1;
            unsigned pqb$v_debug        : 1;
            unsigned pqb$v_dbgtru       : 1;
            unsigned pqb$v_parse_extended   : 1;
            unsigned pqb$v_fill_2_      : 4;
        };
    };
    UINT8 pqb$b_msgmask;
    UINT8 pqb$b_fill_1;
    UINT32 pqb$l_uaf_flags;
    UINT32 pqb$l_creprc_flags;
    struct
    {
        UINT8 pqb$$$_fill_2 [20];
    };
    struct
    {
        UINT8 pqb$$$_fill_3 [20];
    };
    UINT32 pqb$l_input_att;
    UINT32 pqb$l_output_att;
    UINT32 pqb$l_error_att;
    UINT32 pqb$l_disk_att;
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
    UINT32 pqb$l_arb_support;
    UINT32 pqb$l_rms_lcs;
};

#endif

