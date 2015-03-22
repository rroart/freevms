#ifndef QSCSBDEF_H
#define QSCSBDEF_H

#include <cdrpdef.h>
#include <irpdef.h>
#include <qscpbdef.h>
#include <qscsbdef.h>
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
    unsigned short int qscsb$w_size;
    unsigned char qscsb$b_type;
    unsigned char qscsb$b_subtype;
    unsigned int qscsb$l_state;
    unsigned int qscsb$l_flags;
    struct _sb *qscsb$ps_sb;
    struct _pdt *qscsb$ps_pdt;
    struct _cdt *qscsb$ps_cdt;
    struct _irp *qscsb$ps_permirp;
    char qscsb$b_fill_0_ [4];
    void *qscsb$ps_allpaths_fl;
    void *qscsb$ps_allpaths_bl;
    struct _cdrp *qscsb$ps_active_fl;
    struct _cdrp *qscsb$ps_active_bl;
    struct _irp *qscsb$ps_cancel_fl;
    struct _irp *qscsb$ps_cancel_bl;
    struct _cdrp *qscsb$ps_restart_fl;
    struct _cdrp *qscsb$ps_restart_bl;
    QSCSE qscsb$r_synch;
    unsigned int qscsb$l_sendcredits;
    unsigned int qscsb$l_qscubcnt;
    unsigned int qscsb$l_maxmsg;
    char qscsb$b_fill_1_ [4];
    unsigned long long qscsb$q_nodename;
    unsigned long long qscsb$q_systemid;
    unsigned long long qscsb$q_connects;
    unsigned long long qscsb$q_version;
    unsigned long long qscsb$q_conntime;
    unsigned long long qscsb$q_iocnt;
    unsigned long long qscsb$q_abcnt;
    unsigned long long qscsb$q_complexiocnt;
    unsigned long long qscsb$q_controliocnt;
    unsigned long long qscsb$q_simpleiocnt;
    unsigned long long qscsb$q_stallediocnt;
    unsigned long long qscsb$q_errmsgcnt;
} ;

#define QSCSB$C_LENGTH 208
#define QSCSB$K_LENGTH 208

#define QSCSB$S_QSCSB 208

#endif

