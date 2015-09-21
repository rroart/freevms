#ifndef QSCSBDEF_H
#define QSCSBDEF_H

#include <cdrpdef.h>
#include <irpdef.h>
#include <qscpbdef.h>
#include <qscsedef.h>
#include <qscubdef.h>

#define QSCSB$K_DISABLED 0
#define QSCSB$K_WAIT 1
#define QSCSB$K_CLOSED 2
#define QSCSB$K_HANDSHAKE 3
#define QSCSB$K_OPEN 4
#define QSCSB$K_CONNECTION_ERROR 5
#define QSCSB$K_DISCONNECT 6
#define QSCSB$M_BOOTED 0x1

struct _qscsb
{
    struct _qscsb *qscsb$ps_flink;
    struct _qscsb *qscsb$ps_blink;
    UINT16 qscsb$w_size;
    UINT8 qscsb$b_type;
    UINT8 qscsb$b_subtype;
    UINT32 qscsb$l_state;
    UINT32 qscsb$l_flags;
    struct _sb *qscsb$ps_sb;
    struct _pdt *qscsb$ps_pdt;
    struct _cdt *qscsb$ps_cdt;
    struct _irp *qscsb$ps_permirp;
    INT8 qscsb$b_fill_0_ [4];
    void *qscsb$ps_allpaths_fl;
    void *qscsb$ps_allpaths_bl;
    struct _cdrp *qscsb$ps_active_fl;
    struct _cdrp *qscsb$ps_active_bl;
    struct _irp *qscsb$ps_cancel_fl;
    struct _irp *qscsb$ps_cancel_bl;
    struct _cdrp *qscsb$ps_restart_fl;
    struct _cdrp *qscsb$ps_restart_bl;
    QSCSE qscsb$r_synch;
    UINT32 qscsb$l_sendcredits;
    UINT32 qscsb$l_qscubcnt;
    UINT32 qscsb$l_maxmsg;
    INT8 qscsb$b_fill_1_ [4];
    UINT64 qscsb$q_nodename;
    UINT64 qscsb$q_systemid;
    UINT64 qscsb$q_connects;
    UINT64 qscsb$q_version;
    UINT64 qscsb$q_conntime;
    UINT64 qscsb$q_iocnt;
    UINT64 qscsb$q_abcnt;
    UINT64 qscsb$q_complexiocnt;
    UINT64 qscsb$q_controliocnt;
    UINT64 qscsb$q_simpleiocnt;
    UINT64 qscsb$q_stallediocnt;
    UINT64 qscsb$q_errmsgcnt;
} ;

#define QSCSB$C_LENGTH 208
#define QSCSB$K_LENGTH 208

#define QSCSB$S_QSCSB 208

#endif

