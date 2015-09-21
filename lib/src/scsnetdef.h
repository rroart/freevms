#ifndef SCSNETDEF_H
#define SCSNETDEF_H

#include <vms_types.h>

#define SCSNET$C_START_LEN 62
#define SCSNET$C_STACK_LEN 62
#define SCSNET$C_ACK_LEN 2
#define SCSNET$C_HSHUT_LEN 2
#define SCSNET$C_CACHE_LEN 2
#define SCSNET$C_START 0
#define SCSNET$C_STACK 1
#define SCSNET$C_ACK 2
#define SCSNET$C_SCS_DG 3
#define SCSNET$C_SCS_MSG 4
#define SCSNET$C_ELOG 5
#define SCSNET$C_HOSTSHUT 6
#define SCSNET$C_CACHECLR 32768

#define SCSNET$C_PRT_BASE 0
#define SCSNET$C_PRT_ELOG 1

#define SCSNET$C_MIN_DGSIZ 48

#define SCSNET$S_SCSNETDEF 80

struct _scsnet
{
    char scsnet$t_swtype [4];
    char scsnet$t_swvers [4];
    UINT64 scsnet$q_swincarn;
    char scsnet$t_hwtype [4];
    UINT8 scsnet$b_hwvers [12];
    UINT64 scsnet$q_nodename;
    UINT64 scsnet$q_curtime;
};

#endif

