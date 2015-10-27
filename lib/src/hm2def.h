#ifndef HM2DEF_H
#define HM2DEF_H

/* Access.h v1.3    Definitions for file access routines */

/*
 This is part of ODS2 written by Paul Nankervis,
 email address:  Paulnank@au1.ibm.com

 ODS2 is distributed freely for all members of the
 VMS community to use. However all derived works
 must maintain comments in their source to acknowledge
 the contribution of the original author.

 Originally part of access.h
 */

#include <fiddef.h>
#include <uicdef.h>
#include <vms_types.h>

#define HM2$C_LEVEL1        257 /* Structure level 1        */
#define HM2$C_LEVEL2        512 /* Structure level 2        */

/*
 *  Volume characteristics bit positions and mask values
 */
#define HM2$V_READCHECK       0 /* Verify all reads from volume */
#define HM2$V_WRITCHECK       1 /* Verify all writes to volume  */
#define HM2$V_ERASE           2 /* Erase/overwrite on delete    */
#define HM2$V_NOHIGHWATER     3 /* Disable highwater marking    */
#define HM2$V_CLASS_PROT      4 /* Enable classification checks */

#define HM2$M_READCHECK       0x01
#define HM2$M_WRITCHECK       0x02
#define HM2$M_ERASE           0x04
#define HM2$M_NOHIGHWATER     0x08
#define HM2$M_CLASS_PROT      0x10

/*
 **  Define the sizes of various fields
 */
#define HM2$S_HM2DEF        512 /* Size of home block in bytes  */

/* The following date fields are all quadwords  */
#define HM2$S_CREDATE         8 /* Creation date        */
#define HM2$S_RETAINMIN       8 /* Min file retention period    */
#define HM2$S_RETAINMAX       8 /* Max file retention period    */
#define HM2$S_REVDATE         8 /* Revision date        */

/* The following fields are ASCII strings       */
#define HM2$S_MIN_CLASS      20 /* Min security classification  */
#define HM2$S_MAX_CLASS      20 /* Max security classification  */

/* The following field is not defined in the reference*/
#define HM2$S_FILETAB_FID     3 /* Converted 6 bytes -> 3 words */

/* The following fields are ASCII strings       */
#define HM2$S_STRUCNAME      12 /* Volume set name      */
#define HM2$S_VOLNAME        12 /* Volume label         */
#define HM2$S_OWNERNAME      12 /* Volume owner         */
#define HM2$S_FORMAT         12 /* "DECFILE11B  "       */

struct _hm2
{
    UINT32 hm2$l_homelbn; /* LBN for this copy of the home block */
    UINT32 hm2$l_alhomelbn; /* LBN of secondary home block */
    UINT32 hm2$l_altidxlbn; /* Backup LBN for index file header */
    union
    {
        UINT16 hm2$w_struclev; /* ODS version and level */
        struct
        {
            UINT8 hm2$b_strucver; /* ODS version field */
            UINT8 hm2$b_struclev; /* ODS level field */
        };
    };
    UINT16 hm2$w_cluster; /* Volume cluster factor */
    UINT16 hm2$w_homevbn; /* VBN of home block in INDEXF.SYS */
    UINT16 hm2$w_alhomevbn; /* VBN of secondary home block in INDEXF */
    UINT16 hm2$w_altidxvbn; /* VBN of backup index file headerin INDEXF */
    UINT16 hm2$w_ibmapvbn; /* Starting VBN of index file bitmap */
    UINT32 hm2$l_ibmaplbn; /* Starting LBN of index file bitmap */
    UINT32 hm2$l_maxfiles; /* Max files allowed on volume */
    UINT16 hm2$w_ibmapsize; /* Size of index file bitmap (blocks) */
    UINT16 hm2$w_resfiles; /* Number of reserved files on volume */
    UINT16 hm2$w_devtype; /* Not currently used = Always = 0 */
    UINT16 hm2$w_rvn; /* Relative vol. number in a volume set */
    UINT16 hm2$w_setcount; /* No. volumes in vol. set (if RVN = 1) */
    union
    {
        UINT16 hm2$w_volchar; /* Volume characteristics */
        struct
        {
            UINT16 hm2$v_readcheck :1; /* 0 - Verify all reads from volume */
            UINT16 hm2$v_writcheck :1; /* 1 - Verify all writes to volume  */
            UINT16 hm2$v_erase :1; /* 2 - Erase/overwrite on delete    */
            UINT16 hm2$v_nohighwater :1; /* 3 - Disable highwater marking    */
            UINT16 hm2$v_class_prot :1; /* 4 - Enable classification checks */
        };
    };
    struct _uicdef hm2$l_volowner;
    UINT32 hm2$l_sec_mask; /* --Not defined.  Latent ?, SEVMS ? */
    union
    {
        UINT16 hm2$w_protect; /* Volume protection */
        struct
        {
            UINT16 hm2$v_system :4; /*    System access         */
            UINT16 hm2$v_owner :4; /*    Owner access          */
            UINT16 hm2$v_group :4; /*    Group access          */
            UINT16 hm2$v_world :4; /*    World access          */
            /*    Bit   Meaning if set
             0    No read access
             1    No write access
             2    No create access
             3    No delete access    */
        };
    };
    UINT16 hm2$w_fileprot; /* Def file protection - not supported */
    UINT16 hm2$w_recprot; /* Def record protection - supported ? */
    UINT16 hm2$w_checksum1; /* Checksum for all preceding entries */
    UINT64 hm2$q_credate; /* Volume creation date */
    UINT8 hm2$b_window; /* Default mapping window size */
    UINT8 hm2$b_lru_lim; /* #Dirs in directory access cache */
    UINT16 hm2$w_extend; /* Default file extend size */
    UINT64 hm2$q_retainmin; /* Min file retention period */
    UINT64 hm2$q_retainmax; /* Max file retention period */
    UINT64 hm2$q_revdate; /* Last time volume data was revised */
    UINT8 hm2$r_min_class[20]; /* Min security classification */
    UINT8 hm2$r_max_class[20]; /* Max security classification */
    struct _fiddef hm2$w_filetab;/* File lookup table FID ?? */
    union
    {
        UINT16 hm2$w_lowstruclev; /* Lowest structure level on vol */
        struct
        {
            UINT8 hm2$b_lowstrucver; /* Structure version field */
            UINT8 hm2$b_lowstruclev; /* Structure level field  */
        };
    };
    union
    {
        UINT16 hm2$w_highstruclev; /* Highest struct level on vol */
        struct
        {
            UINT8 hm2$b_highstrucver; /* Structure version field */
            UINT8 hm2$b_highstruclev; /* Structure level field */
        };
    };
    UINT8 hm2$t_reserved1[310]; /* Unused bytes */
    UINT32 hm2$l_serialnum; /* Serial no. of physical media */
    char hm2$t_strucname[12]; /* Volume set name */
    char hm2$t_volname[12]; /* Volume label */
    char hm2$t_ownername[12]; /* Volume owner */
    char hm2$t_format[12]; /* "DECFILE11B  " */
    UINT16 hm2$w_reserved2; /* Unused word */
    UINT16 hm2$w_checksum2; /* Checksum for preceeding 510 bytes */
};

#endif
