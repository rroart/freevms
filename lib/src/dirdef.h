#ifndef dirdef_h
#define dirdef_h

/* Direct.h v1.3    Definitions for directory access routines */

/*
        This is part of ODS2 written by Paul Nankervis,
        email address:  Paulnank@au1.ibm.com

        ODS2 is distributed freely for all members of the
        VMS community to use. However all derived works
        must maintain comments in their source to acknowledge
        the contibution of the original author.
*/



struct dir {
    vmsword dir$w_size;
    vmsword dir$w_verlimit;
    vmsbyte dir$b_flags;
    vmsbyte dir$b_namecount;
    char dir$t_name[1];
};

struct dir$ent {
    vmsword dir$w_version;
    struct fiddef dir$fid;
};

#endif
