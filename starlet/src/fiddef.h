#ifndef FIDDEF_H
#define FIDDEF_H

/* Access.h v1.3    Definitions for file access routines */

/*
        This is part of ODS2 written by Paul Nankervis,
        email address:  Paulnank@au1.ibm.com

        ODS2 is distributed freely for all members of the
        VMS community to use. However all derived works
        must maintain comments in their source to acknowledge
        the contibution of the original author.

    Originally part of access.h
*/

#include <vms_types.h>

struct _fiddef
{
    UINT16 fid$w_num;
    UINT16 fid$w_seq;
    union
    {
        UINT16 fid$w_rvn;
        struct
        {
            UINT8 fid$b_rvn;
            UINT8 fid$b_nmx;
        };
    };
};

#endif
