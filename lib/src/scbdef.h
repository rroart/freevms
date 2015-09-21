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
    UINT16 scb$w_struclev;
    UINT16 scb$w_cluster;
    UINT32 scb$l_volsize;
    UINT32 scb$l_blksize;
    UINT32 scb$l_sectors;
    UINT32 scb$l_tracks;
    UINT32 scb$l_cylinders;
    UINT32 scb$l_status;
    UINT32 scb$l_status2;
    UINT16 scb$w_writecnt;
    char scb$t_volockname[12];
    _vms_time scb$q_mounttime;
    UINT16 scb$w_backrev;
    UINT32 scb$q_genernum[2]; /* error? */
    UINT8 scb$b_reserved[446];
    UINT16 scb$w_checksum;
};

#endif
