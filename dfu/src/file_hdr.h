/*  File_hdr.h

    Defines all structures for normal ODS2 File headers.
    Used by DFU
*/


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
#define FH2$S_FID         6
#define FH2$S_RECATTR        32
#define FH2$S_BACKLINK        6

/*
** Misc. File Attribute Area definitions
*/

#define FAT$K_LENGTH         32
#define FAT$C_LENGTH         32
#define FAT$S_FATDEF         32
#define FAT$S_RTYPE       4
#define FAT$S_FILEORG         4

/*
**  Record type
*/

#define FAT$C_UNDEFINED       0
#define FAT$C_FIXED       1
#define FAT$C_VARIABLE        2
#define FAT$C_VFC         3
#define FAT$C_STREAM          4
#define FAT$C_STREAMLF        5
#define FAT$C_STREAMCR        6

/*
** File organization
*/

#define FAT$C_SEQUENTIAL      0
#define FAT$C_RELATIVE        1
#define FAT$C_INDEXED         2
#define FAT$C_DIRECT          3

/*
** Record attributes bit and mask definitions
*/

#define FAT$V_FORTRANCC       0
#define FAT$V_IMPLIEDCC       1
#define FAT$V_PRINTCC         2
#define FAT$V_NOSPAN          3

#define FAT$M_FORTRANCC       1
#define FAT$M_IMPLIEDCC       2
#define FAT$M_PRINTCC         4
#define FAT$M_NOSPAN          8

/*
**  File characteristics bit and mask definitions
*/

#define FH2$M_WASCONTIG     1
#define FH2$M_NOBACKUP      2
#define FH2$M_WRITEBACK     4
#define FH2$M_READCHECK     8
#define FH2$M_WRITCHECK     16
#define FH2$M_CONTIGB       32
#define FH2$M_LOCKED        64
#define FH2$M_CONTIG        128
#define FH2$M_VCC_STATE     1792        /* 3 bit field      */
#define FH2$M_BADACL        2048
#define FH2$M_SPOOL     4096
#define FH2$M_DIRECTORY     8192
#define FH2$M_BADBLOCK      16384
#define FH2$M_MARKDEL       32768
#define FH2$M_NOCHARGE      65536
#define FH2$M_ERASE     131072
#define FH2$M_ALM_AIP       262144
#define FH2$M_SHELVED       524288
#define FH2$M_SCRATCH       1048576
#define FH2$M_NOMOVE        2097152
#define FH2$M_NOSHELVABLE   4194304

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
#define FH2$V_SPOOL      12
#define FH2$V_DIRECTORY      13
#define FH2$V_BADBLOCK       14
#define FH2$V_MARKDEL        15
#define FH2$V_NOCHARGE       16
#define FH2$V_ERASE      17
#define FH2$V_ALM_AIP        18
#define FH2$V_SHELVED        19
#define FH2$V_SCRATCH        20
#define FH2$V_NOMOVE         21
#define FH2$V_NOSHELVABLE    22

/*
**  Journaling bit and mask definitions
*/

#define FH2$V_ONLY_RU         0
#define FH2$V_RUJNL       1
#define FH2$V_BIJNL       2
#define FH2$V_AIJNL       3
#define FH2$V_ATJNL       4
#define FH2$V_NEVER_RU        5
#define FH2$V_JOURNAL_FILE    6

#define FH2$M_ONLY_RU        1
#define FH2$M_RUJNL      2
#define FH2$M_BIJNL      4
#define FH2$M_AIJNL      8
#define FH2$M_ATJNL      16
#define FH2$M_NEVER_RU       32
#define FH2$M_JOURNAL_FILE   64

#define FH2$S_CLASS_PROT     20
#define FH2$R_CLASS_PROT     88


struct header_area_struct
{

    /*
    **  The following four entries are offsets, in words, to the specified
    **  region in the file header
    */

    unsigned char  idoffset;    /* Identification area offset       */
    unsigned char  mpoffset;    /* Extent mapping area          */
    unsigned char    acoffset;  /* ACE list area            */
    unsigned char    rsoffset;  /* Reserved area - Not used by Files-11 */

    unsigned short seg_num; /* Extension segment number     */

    union
    {
        unsigned short struclev;    /* Files-11 structure level for file    */
        struct
        {
            unsigned char  strucver;    /*   Structure version number field */
            unsigned char  struclev;    /*   Structure level number field   */
        } struclev_fields;
    } struclev_overlay;

    /*
    **  Fid of current file header
    */

    union
    {
        unsigned short fid[3];  /* File id              */
        struct
        {
            unsigned short fid_num; /*   File number            */
            unsigned short fid_seq; /*   File sequence number       */
            unsigned char fid_rvn;  /*   Short form RVN field       */
            unsigned char fid_nmx;  /*   RVN extension field        */
        } fid_fields;
    } fid_overlay;

    /*
    **  Fid of next extension header.  If there aren't any more extensions,
    **  then these values are [0,0,0]
    */

    union
    {
        unsigned short ext_fid[3];  /* File id              */
        struct
        {
            unsigned short ext_fid_num; /*   File number            */
            unsigned short ext_fid_seq; /*   File sequence number       */
            unsigned char ext_fid_rvn;/*      Short form RVN field      */
            unsigned char ext_fid_nmx;/*    RVN extension field     */
        } ext_fid_fields;
    } ext_fid_overlay;

    /*
    ** The record attributes information was taken from the I/O User's Guide
    ** and the $FATDEF macro.
    */

    union           /* Record type - consists of 2 parts    */
    {
        unsigned char  rtype;
        struct
        {
            unsigned char rtype:4;      /*   Record type field -
                    FAT$C_UNDEFINED     0
                    FAT$C_FIXED     1
                    FAT$C_VARIABLE      2
                    FAT$C_VFC       3
                    FAT$C_STREAM        4
                    FAT$C_STREAMLF      5
                    FAT$C_STREAMCR      6   */

            unsigned char fileorg:4;    /*   File organization -
                    FAT$C_SEQUENTIAL    0
                    FAT$C_RELATIVE      1
                    FAT$C_INDEXED       2
                    FAT$C_DIRECT        3   */
        } rtype_fields;
    } rtype_overlay;

    unsigned char  rattrib; /* Record attributes -
                    FAT$M_FORTRANCC     1
                    FAT$M_IMPLIEDCC     2
                    FAT$M_PRINTCC       4
                    FAT$M_NOSPAN        8   */

    unsigned short rsize;       /* Record size in bytes         */

    union
    {
        unsigned int  hiblk;    /* Highest allocated VBN (note order)   */
        struct
        {
            unsigned short hiblkh;  /*   High order is FIRST, not last  */
            unsigned short hiblkl;
        } hiblk_fields;
    } hiblk_overlay;

    union
    {
        unsigned int  efblk;    /*End of file VBN (note order)      */
        struct
        {
            unsigned short efblkh;  /*  High order is FIRST, not last   */
            unsigned short efblkl;
        } efblk_fields;
    } efblk_overlay;

    unsigned short ffbyte;  /* First free byte in efblk     */
    unsigned char  bktsize; /* Bucket size in blocks        */
    unsigned char  vfcsize; /* Fixed-length contropl size for VFC   */
    unsigned short maxrec;  /* Maximum record size in bytes     */
    unsigned short defext;  /* Default exent quantity in blocks */
    unsigned short gbc;     /* Global buffer count          */
    unsigned short resvd_rat[4];    /* Reserved - not currently used    */
    unsigned short versions;    /* Default version limit (dirs only)    */

    /* Back to the FH2 definitions */

    unsigned int   filechar;    /* File characteristics bits        */
    unsigned short recprot; /* Record protection - not documented   */
    unsigned char  map_inuse;   /* Number of used to map extents    */
    unsigned char  acc_mode;    /* Accessor priv type/mode      */

    union
    {
        unsigned int   fileowner;   /* File owner's UIC         */
        struct
        {
            unsigned short uicmember;   /*   UIC member field           */
            unsigned short uicgroup;  /*   UIC group field          */
        } fileowner_fields;
    } fileowner_overlay;

    union
    {
        unsigned short   fileprot;  /* File protection code         */
        struct
        {
            unsigned short system : 4;  /*    System access rights      */
            unsigned short owner  : 4;  /*    Owner access rights       */
            unsigned short group  : 4;  /*    Group access rights       */
            unsigned short world  : 4;  /*    World access rights       */
            /*     Bit  Meaning if set
                0   Deny read access
                1   Deny write access
                2   Deny execute access
                3   Deny delete access  */
        } fileprot_fields;
    } fileprot_overlay;

    /*
    ** Fid of the primary directory or previous header (if the file has
    ** multiple headers) pointing to this header
    */

    union
    {
        unsigned short backlink[3]; /* File id              */
        struct
        {
            unsigned short bk_fid_num;/*   File number          */
            unsigned short bk_fid_seq;/*   File sequence number     */
            unsigned char bk_fid_rvn; /*   Short form RVN field     */
            unsigned char bk_fid_nmx; /*   File number extension field  */
        } bk_fid_fields;
    } bk_fid_overlay;

    union
    {
        unsigned char  journal;     /* Journal control flags    */
        struct
        {
            unsigned char only_ru       : 1; /* 0 - Access only via recovery unit   */
            unsigned char rujnl     : 1; /* 1 - Recovery unit journaling enabled*/
            unsigned char bijnl     : 1; /* 2 - Before-image journaling enabled */
            unsigned char aijnl     : 1; /* 3 - After-image journaling enabled  */
            unsigned char atjnl     : 1; /* 4 - Audit-trail journaling enabled  */
            unsigned char never_ru      : 1; /* 5 - No access via recovery unit */
            unsigned char journal_file : 1; /* 6 - File is an RMS journal file  */
        } journal_bits;
    } journal_overlay;

    unsigned char  ru_active;   /* Recovery facility ID number      */
    unsigned int     highwater; /* VBN+1 of highest block written or 0
                   if highwater marking is disabled */

    struct              /* Security classification block    */
    {
        unsigned char secur_lev;    /*  Secrecy level           */
        unsigned char integ_lev;    /*  Integrity level         */
        unsigned int  secur_cat[2]; /*  Mask of applicable secrecy classes  */
        unsigned int  integ_cat[2]; /*  Mask of applicable integrity classes*/
        unsigned char dummy[2]; /*  Not used                */
    } class_prot;
};

/*
**  Identification area structures and definitions
*/

struct ident_area_struct
{
    unsigned char  filename[20];    /* First 20 bytes of filename       */
    unsigned short revision;    /* # times file was changed     */
    unsigned int  credate[2];   /* Creation date            */
    unsigned int  revdate[2];   /* Last revision date           */
    unsigned int  expdate[2];   /* Expiration date          */
    unsigned int  bakdate[2];   /* Date of last backup/record       */
    unsigned char  filenamext[66];/* Rest of filename           */
};

/*
**  Mapping area structures and definitions
*/

/* Mapping pointer type codes               */

#define FM2$C_PLACEMENT 0
#define FM2$C_FORMAT1   1
#define FM2$C_FORMAT2   2
#define FM2$C_FORMAT3   3

/* Map pointer FM2$C_PLACEMENT bit definitions  */

#define FM2$V_EXACT 0
#define FM2$V_ONCYL 1
#define FM2$V_LBN   12
#define FM2$V_RVN   13
#define FM2$V_FORMAT    14

/*
**  This mapping pointer is rarely, if ever, used.  It could be used
**  to physically locate a boot block.  Old versions of the RABBIT-7
**  Disk Defragmentation software used these pointers to save optim-
**  ization placement information.  For disk analysis purposes, this
**  data type doesn't contain any useful information.
*/

struct map_0_struct
{
    union
    {
        unsigned short map_type;
        struct
        {
            unsigned short exact  :  1; /* Placement is exact           */
            unsigned short oncyl  :  1; /* Space allocated on a single cylinder */
            unsigned short fill   : 10; /* Unused bits              */
            unsigned short lbn    :  1; /* Allocate space using LBN in next ptr */
            unsigned short rvn    :  1; /* Allocate space on this rel. volume   */
            unsigned short format :  2; /* Must be FM2$C_PLACEMENT (0)      */
        } format_bits;
    } map_code;
};

/*
**  This mapping pointer would be used to map small extents/files
**  that point to less than 256 blocks that are on a volume of less
**  than 4 Mblocks.  As a result, this pointer type is often used.
*/

struct map_1_struct
{
    union
    {
        unsigned short map_type;
        struct
        {
            unsigned short count   : 8; /* Number of blocks pointed to (< 256)  */
            unsigned short highlbn : 6; /* High order 6 bits of LBN     */
            unsigned short format  : 2; /* Must be FM2$C_FORMAT1 (1)        */
        } format_bits;
    } map_code;
    unsigned short lowlbn;  /* Low order 16 bit of LBN      */
};

/*
**  This mapping pointer would be used to map up to 16 Kblocks at a
**  time on a volume of up to 4 Gblocks.  It is also somewhat commonly
**  used, especially for executables, database files, and BOOKREADER
**  documents
*/

struct map_2_struct
{
    union
    {
        unsigned short map_type;
        struct
        {
            unsigned short count  : 14; /* Number of blocks pointed to (< 16384)*/
            unsigned short format :  2; /* Must be FM2$C_FORMAT2 (2)        */
        } format_bits;
    } map_code;
    unsigned int lbn;       /* Starting LBN for this extent     */
};

/*
**  This mapping pointer is not often used because it maps large
**  extents, up to 1 Gblock, on a volume of up to 4 Gblocks. However,
**  some large files, commonly databases, page and swap files, etc.
**  will often use this type of pointer.
*/

struct map_3_struct
{
    union
    {
        unsigned short map_type;
        struct
        {
            unsigned short hicount : 14;    /* High order part of the block count   */
            unsigned short format  :  2;    /* Must be FM2$C_FORMAT3 (3)        */
        } format_bits;
    } map_code;
    unsigned short lowcount;    /* Low order part of the block count    */
    unsigned int   lbn;     /* Starting LBN for this extent     */
};
