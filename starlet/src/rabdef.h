#ifndef rabdef_h
#define rabdef_h

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

#define RAB$C_SEQ 0
#define RAB$C_RFA 2

struct _rabdef {
    struct _fabdef *rab$l_fab;
    char *rab$l_ubf;
    char *rab$l_rhb;
    char *rab$l_rbf;
    unsigned rab$w_usz;
    unsigned rab$w_rsz;
    int rab$b_rac;
    unsigned short rab$w_rfa[3];
};

#endif
