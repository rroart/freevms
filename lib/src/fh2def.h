#ifndef fh2def_h
#define fh2def_h

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
#include "mytypes.h"

#define FH2$M_NOBACKUP   0x2
#define FH2$M_CONTIG     0x80
#define FH2$M_DIRECTORY  0x2000
#define FH2$M_MARKDEL    0x8000
#define FH2$M_ERASE      0x20000

struct _fh2 {
    vmsbyte fh2$b_idoffset;
    vmsbyte fh2$b_mpoffset;
    vmsbyte fh2$b_acoffset;
    vmsbyte fh2$b_rsoffset;
    vmsword fh2$w_seg_num;
    vmsword fh2$w_struclev;
    struct _fiddef fh2$w_fid;
    struct _fiddef fh2$w_ext_fid;
    struct _fatdef fh2$w_recattr;
    vmslong fh2$l_filechar;
    vmsword fh2$w_reserved1;
    vmsbyte fh2$b_map_inuse;
    vmsbyte fh2$b_acc_mode;
    struct _uicdef fh2$l_fileowner;
    vmsword fh2$w_fileprot;
    struct _fiddef fh2$w_backlink;
    vmsbyte fh2$b_journal;
    vmsbyte fh2$b_ru_active;
    vmsword fh2$w_reserved2;
    vmslong fh2$l_highwater;
    vmsbyte fh2$b_reserved3[8];
    vmsbyte fh2$r_class_prot[20];
    vmsbyte fh2$r_restofit[402];
    vmsword fh2$w_checksum;
};

#endif
