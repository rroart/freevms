#ifndef fatdef_h
#define fatdef_h

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

#define		FAT$C_UNDEFINED		0
#define		FAT$C_FIXED		1
#define		FAT$C_VARIABLE		2
#define		FAT$C_VFC		3
#define		FAT$C_STREAM		4
#define		FAT$C_STREAMLF		5
#define		FAT$C_STREAMCR		6
#define		FAT$C_SEQUENTIAL	0
#define		FAT$C_RELATIVE		1
#define		FAT$C_INDEXED		2
#define		FAT$C_DIRECT		3
#define		FAT$C_SPECIAL		4
#define		FAT$M_FORTRANCC		0x1
#define		FAT$M_IMPLIEDCC		0x2
#define		FAT$M_PRINTCC		0x4
#define		FAT$M_NOSPAN		0x8
#define		FAT$M_MSBRCW		0x10
#define		FAT$C_FIFO		1
#define		FAT$C_CHAR_SPECIAL	2
#define		FAT$C_BLOCK_SPECIAL	3
#define		FAT$C_SYMLINK		4
#define		FAT$K_LENGTH		32
#define		FAT$C_LENGTH		32
#define		FAT$S_FATDEF		32

/*		handling funny byte order */
/* socalled
Inverted format field. The high- and low-order 16 bits are transposed for compatibility with PDP-11 software.
*/

static inline unsigned long pdp_invert(unsigned long l) {
  unsigned short * s=&l;
  return (s[0]<<16) + s[1];
}

struct _fatdef {
    vmsbyte fat$b_rtype;
    vmsbyte fat$b_rattrib;
    vmsword fat$w_rsize;
  vmsswap fat$l_hiblk; /* funny byte order */
  vmsswap fat$l_efblk; /* funny byte order */
    vmsword fat$w_ffbyte;
    vmsbyte fat$b_bktsize;
    vmsbyte fat$b_vfcsize;
    vmsword fat$w_maxrec;
    vmsword fat$w_defext;
    vmsword fat$w_gbc;
    vmsbyte fat$_UU0[8];
    vmsword fat$w_versions;
};

#endif
