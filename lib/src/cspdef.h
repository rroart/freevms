#ifndef CSPDEF_H
#define CSPDEF_H

#include <vms_types.h>

CSP$K_MAX_FLWCTL = 00000008
CSP$K_MAX_WAITING = 00000032

CSP$M_CLEANUP = 00000001

CSP$V_CLEANUP = 00000000

CSP$_ABORT = 00000002
CSP$_BADCSD = 00000003
CSP$_DONE = 00000004
CSP$_LOCAL = 00000007
CSP$_REJECT = 00000006
CSP$_REPLY = 00000005

struct _csp
{
    /* first? */
    UINT8 csp$b_waitcnt;
    UINT8 csp$b_rcvcsdcnt;
    /* 10 empty? */
    UINT8 csp$b_inited;
    UINT8 csp$b_status;
    UINT8 csp$b_rcvcwpscnt;
    /* more? */
}

#endif

