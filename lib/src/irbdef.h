#ifndef IRBDEF_H
#define IRBDEF_H

// Author. Roar Thron�s.
// based on / taken from rmsint2.doc

#include <vms_types.h>

struct _irbdef
{
    UINT8 irb$b_id; // block id (11)
    struct
    {
        unsigned irb$v_busy : 1; // stream is busy
        unsigned irb$v_iop  : 1; // i/o pending on stream (seq org only)
        unsigned irb$v_dup  : 1; // duplicate records seen
        union
        {
            unsigned irb$v_ssk  : 1; // segmented search key
            unsigned irb$v_eof  : 1; // file at eof (seq org only)
        };
        unsigned irb$v_seq  : 1; // last op was a put or finid seq etc
        unsigned irb$v_blk  : 1; // connected for block i/o
        union
        {
            unsigned irb$v_idx  : 1; // index update error occured
            unsigned irb$v_find     : 1; // last op was a find
        };
        unsigned irb$v_asy  : 1; // async i/o stream
    };
    unsigned long irb$l_irab_lnk ;
    unsigned long irb$l_ifab_lnk ;
    unsigned long irb$l_curbdb ;
    unsigned long irb$l_nxtbdb ;
    UINT64 irb$q_nrp ;
    UINT8 irb$b_rp_id;
    UINT8 irb$b_rp_kref;
    UINT16 irb$w_offset;
    UINT8 irb$b_rrv_id;
    UINT8 irb$b_bcnt;
    UINT16 irb$w_mbc;
    unsigned long irb$l_rrv;
    unsigned long irb$l_nkad;
    UINT16 irb$w_spos;
};

#endif /* IRBDEF_H */
