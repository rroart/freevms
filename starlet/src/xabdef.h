#ifndef xabdef_h
#define xabdef_h

/* RMS.h v1.3   RMS routine definitions */

/*
        This is part of ODS2 written by Paul Nankervis,
        email address:  Paulnank@au1.ibm.com

        ODS2 is distributed freely for all members of the
        VMS community to use. However all derived works
        must maintain comments in their source to acknowledge
        the contibution of the original author.

	Originally part of rms.h
*/

#include "vmstime.h"

#define XAB$C_DAT 18
#define XAB$C_FHC 29
#define XAB$C_PRO 19

struct xabdef {
    void *xab$l_nxt;
    int xab$b_cod;
    int xab$w_rvn;
    VMSTIME xab$q_bdt;
    VMSTIME xab$q_cdt;
    VMSTIME xab$q_edt;
    VMSTIME xab$q_rdt;
};

#endif
