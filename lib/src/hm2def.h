#ifndef HM2DEF_H
#define HM2DEF_H

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

struct _hm2
{
    UINT32 hm2$l_homelbn;
    UINT32 hm2$l_alhomelbn;
    UINT32 hm2$l_altidxlbn;
    UINT16 hm2$w_struclev;
    UINT16 hm2$w_cluster;
    UINT16 hm2$w_homevbn;
    UINT16 hm2$w_alhomevbn;
    UINT16 hm2$w_altidxvbn;
    UINT16 hm2$w_ibmapvbn;
    UINT32 hm2$l_ibmaplbn;
    UINT32 hm2$l_maxfiles;
    UINT16 hm2$w_ibmapsize;
    UINT16 hm2$w_resfiles;
    UINT16 hm2$w_devtype;
    UINT16 hm2$w_rvn;
    UINT16 hm2$w_setcount;
    UINT16 hm2$w_volchar;
    struct _uicdef hm2$w_volowner;
    UINT32 hm2$l_reserved1;
    UINT16 hm2$w_protect;
    UINT16 hm2$w_fileprot;
    UINT16 hm2$w_reserved2;
    UINT16 hm2$w_checksum1;
    _vms_time hm2$q_credate;
    UINT8 hm2$b_window;
    UINT8 hm2$b_lru_lim;
    UINT16 hm2$w_extend;
    _vms_time hm2$q_retainmin;
    _vms_time hm2$q_retainmax;
    _vms_time hm2$q_revdate;
    UINT8 hm2$r_min_class[20];
    UINT8 hm2$r_max_class[20];
    UINT8 hm2$t_reserved3[320];
    UINT32 hm2$l_serialnum;
    char hm2$t_strucname[12];
    char hm2$t_volname[12];
    char hm2$t_ownername[12];
    char hm2$t_format[12];
    UINT16 hm2$w_reserved4;
    UINT16 hm2$w_checksum2;
};

#endif
