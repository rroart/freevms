$!
$!	****************************************************************
$!
$!		Copyright (c) 1992, Carnegie Mellon University
$!
$!		All Rights Reserved
$!
$!	Permission  is  hereby  granted   to  use,  copy,  modify,  and
$!	distribute  this software  provided  that the  above  copyright
$!	notice appears in  all copies and that  any distribution be for
$!	noncommercial purposes.
$!
$!	Carnegie Mellon University disclaims all warranties with regard
$!	to this software.  In no event shall Carnegie Mellon University
$!	be liable for  any special, indirect,  or consequential damages
$!	or any damages whatsoever  resulting from loss of use, data, or
$!	profits  arising  out of  or in  connection  with  the  use  or
$!	performance of this software.
$!
$!	****************************************************************
$!
$ !++
$ ! Telnet_EXE.Com for Telnet
$ !
$ ! Description:
$ !
$ !	VMS SYS$UPDATE:VMSINSTAL installation command procedure
$ !	for the CMU-OpenVMS/IP Telnet software.
$ !
$ ! Written By:	Bruce R. Miller		3/21/90		CMU NetDev
$ !
$ !--
$
$ WRITE SYS$OUTPUT "  Installing the Telnet images..."
$ BACKUP VMI$KWD:Telnet032.B/SAV VMI$KWD:
$
$ IF .NOT. CMUIP_LINK_IMAGES THEN GOTO Telnet_PROVIDE_IMAGES
$
$ WRITE SYS$OUTPUT ""
$ @VMI$KWD:Telnet_LINK.COM VMI$KWD:
$
$Telnet_PROVIDE_IMAGES:
$ IF CMUIP_DEBUG THEN VMI$CALLBACK MESSAGE I DEBUG "Provide Telnet client"
$ VMI$CALLBACK PROVIDE_IMAGE -
		Telnet_IMAGE -
		Telnet.EXE -
		'CMUIP_WHERE[SYSEXE]
$ VMI$CALLBACK PROVIDE_FILE -
		TELNET_CLD -
		TELNET.CLD -
		'CMUIP_WHERE[SYSMGR]
$ IF CMUIP_CLD_INSTALL THEN -
	VMI$CALLBACK PROVIDE_DCL_COMMAND Telnet.CLD
$ VMI$CALLBACK UPDATE_LIBRARY -
		VMI$ -
		'CMUIP_HELP_WHERE -
		HELP -
		"/REPLACE" -
		VMI$KWD:Telnet.HLP
$ VMI$CALLBACK PROVIDE_FILE -
		TELNET_HELP -
		TELNET.HLB -
		'CMUIP_WHERE[SYSHLP]
$ WRITE SYS$OUTPUT "  Installing the psuedo-terminal images..."
$ BACKUP VMI$KWD:PTY054.B/SAV VMI$KWD:
$
$ IF .NOT. CMUIP_LINK_IMAGES THEN GOTO Telnet_PROVIDE_IMAGES
$ IF CMUIP_DEBUG THEN VMI$CALLBACK MESSAGE I DEBUG "Linking Pseudo Terminal Driver"
$ @VMI$KWD:PTY_LINK.COM VMI$KWD:
$
$ IF CMUIP_DEBUG THEN VMI$CALLBACK MESSAGE I DEBUG "Provide PTY"
$ VMI$CALLBACK PROVIDE_FILE -
		PTY_IMAGE -
		PNDRIVER.EXE -
		'CMUIP_WHERE[SYS$LDR]
$ VMI$CALLBACK PROVIDE_FILE -
		TZ_IMAGE -
		TZDRIVER.EXE -
		'CMUIP_WHERE[SYS$LDR]
$
$!  DEC/CMS REPLACEMENT HISTORY, Element TELNET_EXE.COM
$!  *1     5-MAR-1991 19:05:39 SYNFUL "VMSINSTAL command procedure to install the Telnet images"
$!  DEC/CMS REPLACEMENT HISTORY, Element TELNET_EXE.COM
