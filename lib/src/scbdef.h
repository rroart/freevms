#ifndef SCBDEF_H
#define SCBDEF_H

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

struct _scbdef
{
    _uword scb$w_struclev;
    _uword scb$w_cluster;
    _ulongword scb$l_volsize;
    _ulongword scb$l_blksize;
    _ulongword scb$l_sectors;
    _ulongword scb$l_tracks;
    _ulongword scb$l_cylinders;
    _ulongword scb$l_status;
    _ulongword scb$l_status2;
    _uword scb$w_writecnt;
    char scb$t_volockname[12];
    _vms_time scb$q_mounttime;
    _uword scb$w_backrev;
    _ulongword scb$q_genernum[2]; /* error? */
    char scb$b_reserved[446];
    _uword scb$w_checksum;
};

#endif
