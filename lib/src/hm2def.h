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
    _ulongword hm2$l_homelbn;
    _ulongword hm2$l_alhomelbn;
    _ulongword hm2$l_altidxlbn;
    _uword hm2$w_struclev;
    _uword hm2$w_cluster;
    _uword hm2$w_homevbn;
    _uword hm2$w_alhomevbn;
    _uword hm2$w_altidxvbn;
    _uword hm2$w_ibmapvbn;
    _ulongword hm2$l_ibmaplbn;
    _ulongword hm2$l_maxfiles;
    _uword hm2$w_ibmapsize;
    _uword hm2$w_resfiles;
    _uword hm2$w_devtype;
    _uword hm2$w_rvn;
    _uword hm2$w_setcount;
    _uword hm2$w_volchar;
    struct _uicdef hm2$w_volowner;
    _ulongword hm2$l_reserved1;
    _uword hm2$w_protect;
    _uword hm2$w_fileprot;
    _uword hm2$w_reserved2;
    _uword hm2$w_checksum1;
    _vms_time hm2$q_credate;
    _ubyte hm2$b_window;
    _ubyte hm2$b_lru_lim;
    _uword hm2$w_extend;
    _vms_time hm2$q_retainmin;
    _vms_time hm2$q_retainmax;
    _vms_time hm2$q_revdate;
    _ubyte hm2$r_min_class[20];
    _ubyte hm2$r_max_class[20];
    _ubyte hm2$t_reserved3[320];
    _ulongword hm2$l_serialnum;
    char hm2$t_strucname[12];
    char hm2$t_volname[12];
    char hm2$t_ownername[12];
    char hm2$t_format[12];
    _uword hm2$w_reserved4;
    _uword hm2$w_checksum2;
};

#endif
