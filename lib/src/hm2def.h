#ifndef hm2def_h
#define hm2def_h

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

struct _hm2 {
    vmslong hm2$l_homelbn;
    vmslong hm2$l_alhomelbn;
    vmslong hm2$l_altidxlbn;
    vmsword hm2$w_struclev;
    vmsword hm2$w_cluster;
    vmsword hm2$w_homevbn;
    vmsword hm2$w_alhomevbn;
    vmsword hm2$w_altidxvbn;
    vmsword hm2$w_ibmapvbn;
    vmslong hm2$l_ibmaplbn;
    vmslong hm2$l_maxfiles;
    vmsword hm2$w_ibmapsize;
    vmsword hm2$w_resfiles;
    vmsword hm2$w_devtype;
    vmsword hm2$w_rvn;
    vmsword hm2$w_setcount;
    vmsword hm2$w_volchar;
    struct _uicdef hm2$w_volowner;
    vmslong hm2$l_reserved1;
    vmsword hm2$w_protect;
    vmsword hm2$w_fileprot;
    vmsword hm2$w_reserved2;
    vmsword hm2$w_checksum1;
    VMSTIME hm2$q_credate;
    vmsbyte hm2$b_window;
    vmsbyte hm2$b_lru_lim;
    vmsword hm2$w_extend;
    VMSTIME hm2$q_retainmin;
    VMSTIME hm2$q_retainmax;
    VMSTIME hm2$q_revdate;
    vmsbyte hm2$r_min_class[20];
    vmsbyte hm2$r_max_class[20];
    vmsbyte hm2$t_reserved3[320];
    vmslong hm2$l_serialnum;
    char hm2$t_strucname[12];
    char hm2$t_volname[12];
    char hm2$t_ownername[12];
    char hm2$t_format[12];
    vmsword hm2$w_reserved4;
    vmsword hm2$w_checksum2;
};

#endif
