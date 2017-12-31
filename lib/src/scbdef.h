#ifndef scbdef_h
#define scbdef_h

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
struct _scbdef
{
    vmsword scb$w_struclev;
    vmsword scb$w_cluster;
    vmslong scb$l_volsize;
    vmslong scb$l_blksize;
    vmslong scb$l_sectors;
    vmslong scb$l_tracks;
    vmslong scb$l_cylinders;
    vmslong scb$l_status;
    vmslong scb$l_status2;
    vmsword scb$w_writecnt;
    char scb$t_volockname[12];
    VMSTIME scb$q_mounttime;
    vmsword scb$w_backrev;
    vmslong scb$q_genernum[2]; /* error? */
    char scb$b_reserved[446];
    vmsword scb$w_checksum;
};

#endif
