#ifndef FATDEF_H
#define FATDEF_H

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

/*
 *  Record type
 */
#define FAT$C_UNDEFINED     0
#define FAT$C_FIXED         1
#define FAT$C_VARIABLE      2
#define FAT$C_VFC           3
#define FAT$C_STREAM        4
#define FAT$C_STREAMLF      5
#define FAT$C_STREAMCR      6

/*
 * File organization
 */
#define FAT$C_SEQUENTIAL    0
#define FAT$C_RELATIVE      1
#define FAT$C_INDEXED       2
#define FAT$C_DIRECT        3
#define FAT$C_SPECIAL       4

/*
 * Record attributes bit and mask definitions
 */
#define FAT$V_FORTRANCC       0
#define FAT$V_IMPLIEDCC       1
#define FAT$V_PRINTCC         2
#define FAT$V_NOSPAN          3
#define FAT$V_MSBRCW          4

#define FAT$M_FORTRANCC     0x01
#define FAT$M_IMPLIEDCC     0x02
#define FAT$M_PRINTCC       0x04
#define FAT$M_NOSPAN        0x08
#define FAT$M_MSBRCW        0x10

/*
 * File type?
 */
#define FAT$C_FIFO          1
#define FAT$C_CHAR_SPECIAL  2
#define FAT$C_BLOCK_SPECIAL 3
#define FAT$C_SYMLINK       4

/*
 * Misc. File Attribute Area definitions
 */
#define FAT$K_LENGTH        32
#define FAT$C_LENGTH        32
#define FAT$S_FATDEF        32
#define FAT$S_RTYPE          4
#define FAT$S_FILEORG        4

#define VMSSWAP(l) ((l & 0xffff) << 16 | l >> 16)

struct _fatdef
{
    union /* Record type - consists of 2 parts */
    {
        UINT8 fat$b_rtype;
        struct
        {
            UINT8 fat$_rtype:4;      /*   Record type field */
            UINT8 fat$_fileorg:4;    /*   File organization */
        };
    };
    UINT8 fat$b_rattrib; /* Record attributes */
    UINT16 fat$w_rsize; /* Record size in bytes */
    union
    {
        UINT32 fat$l_hiblk; /* Highest allocated VBN (note order) */
        struct
        {
            UINT16 fat$w_hiblkh; /* High order is FIRST, not last */
            UINT16 fat$w_hiblkl;
        };
    };
    union
    {
        UINT32 fat$l_efblk; /* End of file VBN (note order) */
        struct
        {
            UINT16 fat$w_efblkh; /* High order is FIRST, not last */
            UINT16 fat$w_efblkl;
        };
    };
    UINT16 fat$w_ffbyte; /* First free byte in efblk */
    UINT8 fat$b_bktsize; /* Bucket size in blocks */
    UINT8 fat$b_vfcsize; /* Fixed-length contropl size for VFC */
    UINT16 fat$w_maxrec; /* Maximum record size in bytes */
    UINT16 fat$w_defext; /* Default exent quantity in blocks */
    UINT16 fat$w_gbc; /* Global buffer count */
    UINT16 fat$w_resvd_rat[4]; /* Reserved - not currently used */
    UINT16 fat$w_versions; /* Default version limit (dirs only) */
};

#endif
