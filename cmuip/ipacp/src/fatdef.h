/*
	****************************************************************

		Copyright (c) 1992, Carnegie Mellon University

		All Rights Reserved

	Permission  is  hereby  granted   to  use,  copy,  modify,  and
	distribute  this software  provided  that the  above  copyright
	notice appears in  all copies and that  any distribution be for
	noncommercial purposes.

	Carnegie Mellon University disclaims all warranties with regard
	to this software.  In no event shall Carnegie Mellon University
	be liable for  any special, indirect,  or consequential damages
	or any damages whatsoever  resulting from loss of use, data, or
	profits  arising  out of  or in  connection  with  the  use  or
	performance of this software.

	****************************************************************
*/
#ifndef _FATDEF_H_
#define _FATDEF_H_ 1

/* Definitions for file (record) attributes */

struct fatdef {
	struct {
		unsigned fat$v_rtype : 4;
		unsigned fat$v_fileorg : 4;
	} fat$b_rtype;
	unsigned char fat$b_rattrib;
	unsigned short int fat$w_rsize;
	struct {
		unsigned short int fat$w_hiblkh;
		unsigned short int fat$w_hiblkl;
	} fat$l_hiblk;
	struct {
		unsigned short int fat$w_efblkh;
		unsigned short int fat$w_efblkl;
	} fat$l_efblk;
	unsigned short int fat$w_ffbyte;
	unsigned char fat$b_bktsize;
	unsigned char fat$b_vfcsize;
	unsigned short int fat$w_maxrec;
	unsigned short int fat$w_defext;
	unsigned short int fat$w_gbc;
	char unused_1[8];
	unsigned short int fat$w_versions;
};

#define FAT$C_UNDEFINED	0
#define FAT$C_FIXED	1
#define FAT$C_VARIABLE	2
#define FAT$C_VFC	3
#define FAT$C_STREAM	4
#define FAT$C_STREAMLF	5
#define FAT$C_STREAMCR	6
#define FAT$C_SEQUENTIAL	0
#define FAT$C_RELATIVE	1
#define FAT$C_INDEXED	2
#define FAT$C_DIRECT	3
#define FAT$M_FORTRANCC	1
#define FAT$M_IMPLIEDCC	2
#define FAT$M_PRINTCC	4
#define FAT$M_NOSPAN	4
#define FAT$K_LENGTH	32
#define FAT$C_LENGTH	32
#define FAT$S_LENGTH	32

#endif _FATDEF_H_

