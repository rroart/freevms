#ifndef namdef_h
#define namdef_h

/* RMS.h v1.3   RMS routine definitions */

/*
        This is part of ODS2 written by Paul Nankervis,
        email address:  Paulnank@au1.ibm.com

        ODS2 is distributed freely for all members of the
        VMS community to use. However all derived works
        must maintain comments in their source to acknowledge
        the contibution of the original author.

	Originally part of rms.h
*/

#include "vmstime.h"

#define NAM$C_MAXRSS 255
#define NAM$M_SYNCHK 1

#define NAM$M_WILDCARD 0x100

struct namdef {
    unsigned short nam$w_did_num;
    unsigned short nam$w_did_seq;
    unsigned char nam$b_did_rvn;
    unsigned char nam$b_did_nmx;
    unsigned short nam$w_fid_num;
    unsigned short nam$w_fid_seq;
    unsigned char nam$b_fid_rvn;
    unsigned char nam$b_fid_nmx;
    int nam$b_ess;
    int nam$b_rss;
    int nam$b_esl;
    char *nam$l_esa;
    int nam$b_rsl;
    char *nam$l_rsa;
    int nam$b_dev;
    char *nam$l_dev;
    int nam$b_dir;
    char *nam$l_dir;
    int nam$b_name;
    char *nam$l_name;
    int nam$b_type;
    char *nam$l_type;
    int nam$b_ver;
    char *nam$l_ver;
    void *nam$l_wcc;
    int nam$b_nop;
    int nam$l_fnb;
};

#endif
