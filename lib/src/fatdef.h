#ifndef fatdef_h
#define fatdef_h

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

#define NO_DOLLAR
#include "cache.h"
#include "vmstime.h"
#include "mytypes.h"

struct fat {
    vmsbyte fat$b_rtype;
    vmsbyte fat$b_rattrib;
    vmsword fat$w_rsize;
    vmsswap fat$l_hiblk;
    vmsswap fat$l_efblk;
    vmsword fat$w_ffbyte;
    vmsbyte fat$b_bktsize;
    vmsbyte fat$b_vfcsize;
    vmsword fat$w_maxrec;
    vmsword fat$w_defext;
    vmsword fat$w_gbc;
    vmsbyte fat$_UU0[8];
    vmsword fat$w_versions;
};

#endif
