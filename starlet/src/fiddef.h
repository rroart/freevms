#ifndef fiddef_h
#define fiddef_h

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

struct _fiddef {
    vmsword fid$w_num;
    vmsword fid$w_seq;
  union {
    vmsword fid$w_rvn;
    struct {
    vmsbyte fid$b_rvn;
    vmsbyte fid$b_nmx;
    };
  };
};

#endif
