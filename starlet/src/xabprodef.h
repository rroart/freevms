#ifndef xabprodef_h
#define xabprodef_h

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

struct xabprodef1 {
    void *xab$l_nxt;
    int xab$b_cod;
    int xab$w_pro;
    int xab$l_uic;
};

#endif
