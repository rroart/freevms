#ifndef SBDEF_H
#define SBDEF_H

#include <vms_types.h>

#define SB$M_LOCAL 0x1
#define SB$M_LOCAL_DIRECTORY 0x2

#define SB$K_LENGTH 120
#define SB$C_LENGTH 120
#define SB$S_SBDEF 120

struct _sb
{
    void *sb$l_flink;
    void *sb$l_blink;
    UINT16 sb$w_size;
    UINT8 sb$b_type;
    UINT8 sb$b_subtyp;
    struct _pb *sb$l_pbfl;
    struct _pb *sb$l_pbbl;
    struct _pb *sb$l_pbconnx;

    INT32 sb$$_fill_2;
    UINT8 sb$b_systemid [6];
    INT16 sb$$_fill_1;
    UINT16 sb$w_maxdg;
    UINT16 sb$w_maxmsg;
    char sb$t_swtype [4];
    char sb$t_swvers [4];
    UINT64 sb$q_swincarn;
    char sb$t_hwtype [4];
    UINT8 sb$b_hwvers [12];
    char sb$t_nodename [16];
    struct _ddb *sb$l_ddb;
    INT16 sb$w_timeout;
    UINT8 sb$b_enbmsk [2];
    struct _csb *sb$l_csb;
    UINT32 sb$l_port_map;
    UINT32 sb$l_status;
    void *sb$ps_proc_names;
    UINT32 sb$l_mount_lkid;
    void *sb$ps_allocls_list;
};

#endif

