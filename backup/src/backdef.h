#ifndef BACKDEF_H
#define BACKDEF_H


/*
 *
 *  Title:
 *	backdef.h
 *
 *  Author:
 *	Roar Thronæs (not really, based this on vmsbackup.c)
 *
 */

struct bbh {
	unsigned char	bbh$w_size[2];
	unsigned char	bbh$w_opsys[2];
	unsigned char	bbh$w_subsys[2];
	unsigned char	bbh$w_applic[2];
	unsigned char	bbh$l_number[4];
	char	bbh$t_spare_1[20];
	unsigned char	bbh$w_struclev[2];
	unsigned char	bbh$w_volnum[2];
	unsigned char	bbh$l_crc[4];
	unsigned char	bbh$l_blocksize[4];
	unsigned char	bbh$l_flags[4];
	char	bbh$t_ssname[32];
	unsigned char	bbh$w_fid[3][2];
	unsigned char	bbh$w_did[3][2];
	char	bbh$t_filename[128];
	char	bbh$b_rtype;
	char	bbh$b_rattrib;
	unsigned char	bbh$w_rsize[2];
	char	bbh$b_bktsize;
	char	bbh$b_vfcsize;
	unsigned char	bbh$w_maxrec[2];
	unsigned char	bbh$l_filesize[4];
	char	bbh$t_spare_2[22];
	unsigned char	bbh$w_checksum[2];
};

struct brh {
	unsigned char	brh$w_rsize[2];
	unsigned char	brh$w_rtype[2];
	unsigned char	brh$l_flags[4];
	unsigned char	brh$l_address[4];
	unsigned char	brh$l_spare[4];
};

/* define record types */

#define	brh$k_null	0
#define	brh$k_summary	1
#define	brh$k_volume	2
#define	brh$k_file	3
#define	brh$k_vbn	4
#define brh$k_physvol	5
#define brh$k_lbn	6
#define	brh$k_fid	7

struct bsa {
	unsigned char	bsa$w_size[2];
	unsigned char	bsa$w_type[2];
	char	bsa$t_text[1];
};

#define			FAB$C_UDF	0	/* undefined */
#define			FAB$C_FIX	1	/* fixed-length record */
#define			FAB$C_VAR	2	/* variable-length record */
#define			FAB$C_VFC	3	/* variable-length with fixed-length control record */
#define 		FAB$C_STM	4	/* RMS-11 stream record (valid only for sequential org) */
#define			FAB$C_STMLF	5	/* stream record delimited by LF (sequential org only) */
#define 		FAB$C_STMCR	6	/* stream record delimited by CR (sequential org only) */
#define			FAB$C_MAXRFM	6	/* maximum rfm supported */

#define			FAB$V_FTN	0	/* FORTRAN carriage control character */
#define			FAB$V_CR	1	/* line feed - record -carriage return */
#define			FAB$V_PRN	2	/* print-file carriage control */
#define			FAB$V_BLK	3	/* records don't cross block boundaries */
#define LABEL_SIZE      80

#endif
