#ifndef ifbdef_h
#define ifbdef_h

// Author. Roar Thronæs.

// based on / taken from rmsint2.doc

#include <fatdef.h>

struct _ifbdef
{
    unsigned char ifb$b_bid; // block id (13)
    struct
    {
        unsigned ifb$v_busy : 1; // stream busy
        unsigned ifb$v_iop 	: 1; // i/o pending on stream (seq file only)
        unsigned ifb$v_prv 	: 1; // private i/o buffer pool for file
        unsigned ifb$v_eof 	: 1; // file positioned at eof
        unsigned ifb$v_rwc 	: 1; // rewind on close
        unsigned ifb$v_dfw 	: 1; // deferred write
        unsigned ifb$v_acc 	: 1; // file is accessed (cleared on close)
        unsigned ifb$v_ani 	: 1; // ansi d variable recored
    };
    unsigned long ifb$l_irab_lnk ;
    void * ifb$l_nirb; // next
    unsigned char ifb$b_ubsz ; // user buffer size
    unsigned short ifb$w_chnl ; // channel
    unsigned char ifb$b_facc; // file access
    unsigned char ifb$b_dvch; // device characteristics
    unsigned int ifb$l_devchar;
    unsigned int ifb$l_devchar2;
    unsigned long ifb$l_bdb;
    unsigned long ifb$l_ifab_lnk ;
    unsigned long long ifb$q_pool ;
    unsigned long ifb$l_mrn;

    struct _fatdef ifb$w_recattr;

};

#endif
