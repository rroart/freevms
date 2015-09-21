#ifndef IFBDEF_H
#define IFBDEF_H

// Author. Roar Thronæs.

// based on / taken from rmsint2.doc

#include <fatdef.h>

struct _ifbdef
{
    UINT8 ifb$b_bid; // block id (13)
    struct
    {
        unsigned ifb$v_busy : 1; // stream busy
        unsigned ifb$v_iop  : 1; // i/o pending on stream (seq file only)
        unsigned ifb$v_prv  : 1; // private i/o buffer pool for file
        unsigned ifb$v_eof  : 1; // file positioned at eof
        unsigned ifb$v_rwc  : 1; // rewind on close
        unsigned ifb$v_dfw  : 1; // deferred write
        unsigned ifb$v_acc  : 1; // file is accessed (cleared on close)
        unsigned ifb$v_ani  : 1; // ansi d variable recored
    };
    unsigned long ifb$l_irab_lnk ;
    void * ifb$l_nirb; // next
    UINT8 ifb$b_ubsz ; // user buffer size
    UINT16 ifb$w_chnl ; // channel
    UINT8 ifb$b_facc; // file access
    UINT8 ifb$b_dvch; // device characteristics
    UINT32 ifb$l_devchar;
    UINT32 ifb$l_devchar2;
    unsigned long ifb$l_bdb;
    unsigned long ifb$l_ifab_lnk ;
    UINT64 ifb$q_pool ;
    unsigned long ifb$l_mrn;

    struct _fatdef ifb$w_recattr;

};

#endif
