#ifndef xabfhcdef_h
#define xabfhcdef_h

/* RMS.h v1.3   RMS routine definitions */

/*
        This is part of ODS2 written by Paul Nankervis,
        email address:  Paulnank@au1.ibm.com

        ODS2 is distributed freely for all members of the
        VMS community to use. However all derived works
        must maintain comments in their source to acknowledge
        the contibution of the original author.
*/

struct _xabfhcdef {
    void *xab$l_nxt;
    int xab$b_cod;
    int xab$b_atr;
    int xab$b_bkz;
    int xab$w_dxq;
    int xab$l_ebk;
    int xab$w_ffb;
    int xab$w_gbc;
    int xab$l_hbk;
    int xab$b_hsz;
    int xab$w_lrl;
    int xab$w_verlimit;
};

#endif
