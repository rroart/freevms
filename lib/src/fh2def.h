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

#define FH2$C_LEVEL1    257     /* Structure level 1        */
#define FH2$C_LEVEL2    512     /* Structure level 2        */

#define FH2$C_RU_FACILITY_RMS     1
#define FH2$C_RU_FACILITY_DBMS    2
#define FH2$C_RU_FACILITY_RDB     3
#define FH2$C_RU_FACILITY_CHKPNT  4

#define FH2$K_LENGTH         80
#define FH2$C_LENGTH         80

#define FH2$K_SUBSET0_LENGTH     88
#define FH2$C_SUBSET0_LENGTH     88

#define FH2$K_FULL_LENGTH   108
#define FH2$C_FULL_LENGTH   108

#define FH2$S_FH2DEF        512
#define FH2$S_FID             6
#define FH2$S_RECATTR        32
#define FH2$S_BACKLINK        6

/*
 *  File characteristics bit and mask definitions
 */
#define FH2$M_WASCONTIG     0x000001
#define FH2$M_NOBACKUP      0x000002
#define FH2$M_WRITEBACK     0x000004
#define FH2$M_READCHECK     0x000008
#define FH2$M_WRITCHECK     0x000010
#define FH2$M_CONTIGB       0x000020
#define FH2$M_LOCKED        0x000040
#define FH2$M_CONTIG        0x000080
#define FH2$M_VCC_STATE     0x000700        /* 3 bit field      */
#define FH2$M_BADACL        0x000800
#define FH2$M_SPOOL         0x001000
#define FH2$M_DIRECTORY     0x002000
#define FH2$M_BADBLOCK      0x004000
#define FH2$M_MARKDEL       0x008000
#define FH2$M_NOCHARGE      0x010000
#define FH2$M_ERASE         0x020000
#define FH2$M_ALM_AIP       0x040000
#define FH2$M_SHELVED       0x080000
#define FH2$M_SCRATCH       0x100000
#define FH2$M_NOMOVE        0x200000
#define FH2$M_NOSHELVABLE   0x400000

#define FH2$S_VCC_STATE       3 /* Size of VCC_STATE bit field  */

#define FH2$V_WASCONTIG       0
#define FH2$V_NOBACKUP        1
#define FH2$V_WRITEBACK       2
#define FH2$V_READCHECK       3
#define FH2$V_WRITCHECK       4
#define FH2$V_CONTIGB         5
#define FH2$V_LOCKED          6
#define FH2$V_CONTIG          7
#define FH2$V_VCC_STATE       8 /* Field is 3 bits long     */
#define FH2$V_BADACL         11
#define FH2$V_SPOOL          12
#define FH2$V_DIRECTORY      13
#define FH2$V_BADBLOCK       14
#define FH2$V_MARKDEL        15
#define FH2$V_NOCHARGE       16
#define FH2$V_ERASE          17
#define FH2$V_ALM_AIP        18
#define FH2$V_SHELVED        19
#define FH2$V_SCRATCH        20
#define FH2$V_NOMOVE         21
#define FH2$V_NOSHELVABLE    22

/*
 *  Journaling bit and mask definitions
 */
#define FH2$V_ONLY_RU         0
#define FH2$V_RUJNL           1
#define FH2$V_BIJNL           2
#define FH2$V_AIJNL           3
#define FH2$V_ATJNL           4
#define FH2$V_NEVER_RU        5
#define FH2$V_JOURNAL_FILE    6

#define FH2$M_ONLY_RU        0x01
#define FH2$M_RUJNL          0x02
#define FH2$M_BIJNL          0x04
#define FH2$M_AIJNL          0x08
#define FH2$M_ATJNL          0x10
#define FH2$M_NEVER_RU       0x20
#define FH2$M_JOURNAL_FILE   0x40

#define FH2$S_CLASS_PROT     20
#define FH2$R_CLASS_PROT     88

struct _fh2
{
    /*
     *  The following four entries are offsets, in words, to the specified
     *  region in the file header
     */
    UINT8 fh2$b_idoffset;  /* Identification area offset */
    UINT8 fh2$b_mpoffset;  /* Extent mapping area */
    UINT8 fh2$b_acoffset;  /* ACE list area */
    UINT8 fh2$b_rsoffset;  /* Reserved area - Not used by Files-11 */
    UINT16 fh2$w_seg_num;  /* Extension segment number */
    union
    {
        UINT16 fh2$w_struclev; /* Files-11 structure level for file    */
        struct
        {
            UINT8 fh2$b_strucver; /* Structure version number field */
            UINT8 fh2$b_struclev; /* Structure level number field   */
        };
    };
    struct _fiddef fh2$w_fid;
    struct _fiddef fh2$w_ext_fid;
    struct _fatdef fh2$w_recattr;
    UINT32 fh2$l_filechar; /* File characteristics bits */
    UINT16 fh2$w_recprot; /* Record protection - not documented */
    UINT8 fh2$b_map_inuse; /* Number of used to map extents */
    UINT8 fh2$b_acc_mode; /* Accessor priv type/mode */
    struct _uicdef fh2$l_fileowner;
    union
    {
        UINT16 fh2$w_fileprot; /* File protection code */
        struct
        {
            UINT16 fh2$_system : 4;  /*    System access rights      */
            UINT16 fh2$_owner  : 4;  /*    Owner access rights       */
            UINT16 fh2$_group  : 4;  /*    Group access rights       */
            UINT16 fh2$_world  : 4;  /*    World access rights       */
            /*     Bit  Meaning if set
                0   Deny read access
                1   Deny write access
                2   Deny execute access
                3   Deny delete access  */
        };
    };
    struct _fiddef fh2$w_backlink;
    union
    {
        UINT8 fh2$b_journal; /* Journal control flags */
        struct
        {
            UINT8 fh2$_only_ru      : 1; /* 0 - Access only via recovery unit   */
            UINT8 fh2$_rujnl        : 1; /* 1 - Recovery unit journaling enabled*/
            UINT8 fh2$_bijnl        : 1; /* 2 - Before-image journaling enabled */
            UINT8 fh2$_aijnl        : 1; /* 3 - After-image journaling enabled  */
            UINT8 fh2$_atjnl        : 1; /* 4 - Audit-trail journaling enabled  */
            UINT8 fh2$_never_ru     : 1; /* 5 - No access via recovery unit */
            UINT8 fh2$_journal_file : 1; /* 6 - File is an RMS journal file  */
        };
    };
    UINT8 fh2$b_ru_active; /* Recovery facility ID number */
    UINT16 fh2$w_reserved2;
    UINT32 fh2$l_highwater; /* VBN+1 of highest block written or 0 if highwater marking is disabled */
    UINT8 fh2$b_reserved3[8];
    struct /* Security classification block    */
    {
        UINT8 fh2$b_secur_lev;  /* Secrecy level           */
        UINT8 fh2$b_integ_lev;  /* Integrity level         */
        UINT64 fh2$q_secur_cat; /* Mask of applicable secrecy classes  */
        UINT64 fh2$q_integ_cat; /* Mask of applicable integrity classes*/
        UINT16 fh2$w_dummy;   /* Not used                */
    } fh2$r_class_prot;
    UINT8 fh2$r_restofit[402];
    UINT16 fh2$w_checksum;
};

#endif
