#ifndef FH2DEF_H
#define FH2DEF_H

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

#include <fatdef.h>
#include <fiddef.h>
#include <uicdef.h>

#define FH2$M_NOBACKUP   0x2
#define FH2$M_CONTIG     0x80
#define FH2$M_DIRECTORY  0x2000
#define FH2$M_MARKDEL    0x8000
#define FH2$M_ERASE      0x20000

struct _fh2
{
    _ubyte fh2$b_idoffset;
    _ubyte fh2$b_mpoffset;
    _ubyte fh2$b_acoffset;
    _ubyte fh2$b_rsoffset;
    _uword fh2$w_seg_num;
    _uword fh2$w_struclev;
    struct _fiddef fh2$w_fid;
    struct _fiddef fh2$w_ext_fid;
    struct _fatdef fh2$w_recattr;
    _ulongword fh2$l_filechar;
    _uword fh2$w_reserved1;
    _ubyte fh2$b_map_inuse;
    _ubyte fh2$b_acc_mode;
    struct _uicdef fh2$l_fileowner;
    _uword fh2$w_fileprot;
    struct _fiddef fh2$w_backlink;
    _ubyte fh2$b_journal;
    _ubyte fh2$b_ru_active;
    _uword fh2$w_reserved2;
    _ulongword fh2$l_highwater;
    _ubyte fh2$b_reserved3[8];
    _ubyte fh2$r_class_prot[20];
    _ubyte fh2$r_restofit[402];
    _uword fh2$w_checksum;
};

#endif
