#ifndef uicdef_h
#define uicdef_h

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

struct _uicdef {
    vmsword uic$w_mem;
    vmsword uic$w_grp;
};

#endif
