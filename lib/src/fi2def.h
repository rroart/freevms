#ifndef FI2DEF_H
#define FI2DEF_H

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

struct _fi2
{
    char fi2$t_filename[20];
    UINT16 fi2$w_revision;
    UINT64 fi2$q_credate;
    UINT64 fi2$q_revdate;
    UINT64 fi2$q_expdate;
    UINT64 fi2$q_bakdate;
    char fi2$t_filenamext[66];
};

#endif
