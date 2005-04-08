!
!	****************************************************************
!
!		Copyright (c) 1992, Carnegie Mellon University
!
!		All Rights Reserved
!
!	Permission  is  hereby  granted   to  use,  copy,  modify,  and
!	distribute  this software  provided  that the  above  copyright
!	notice appears in  all copies and that  any distribution be for
!	noncommercial purposes.
!
!	Carnegie Mellon University disclaims all warranties with regard
!	to this software.  In no event shall Carnegie Mellon University
!	be liable for  any special, indirect,  or consequential damages
!	or any damages whatsoever  resulting from loss of use, data, or
!	profits  arising  out of  or in  connection  with  the  use  or
!	performance of this software.
!
!	****************************************************************
!
!++
! Descrip.Mms	Copyright (c) 1986	Carnegie Mellon University
!
! Description:
!
!	Descrip.Mms for IP/TCP Telnet program.
!
! Written By:	Dale Moore	02-MAY-1986	CMU-CS/RI
!
! Modifications:
!
!	40-Sep-1990	Bruce R. Miller		CMU NetDev
!	Modified to work with 6.5 kitinstal.com
!	Include .OLB files in saveset
!
!	10-MAR-1989	Dale Moore	CMU-CS
!	Removed PRINT.L32 from EXE dependencies.
!
!	09-JAN-1989	Dale Moore	CMU-CS
!	Added a few commas to be consistent.
!	Modified the module name here to correspond to the source
!	Added TELNET.HLB to the B saveset.
!
!	20-DEC-1988	Bruce Miller	CMU Network Development
!	Added module Telnet_Term to contain terminal-specific code.
!
!	12-DEC-1988	Bruce Miller	CMU Network Development
!	Added module OPTIONS to contain option-specific code.
!
!	06-MAY-1986	Dale Moore	CMU-CS/RI
!	Change the object files to be in a library.
!--

DEBUG		= /NODEBUG /NOTRACE
LDEBUG		= /NODEBUG /NOTRACE

BFLAGS		= /OBJECT=$(MMS$TARGET_NAME) $(DEBUG) /TERM=STATISTICS /LIST
LINKFLAGS	= /EXEC=$(MMS$TARGET_NAME) $(LDEBUG) /MAP/SYM

Telnet	: TELNET.EXE
	WRITE SYS$OUTPUT "$ ! Done building Telnet"

Both	: TELNET.HLB, TELNET.EXE
	WRITE SYS$OUTPUT "$ ! Done building Telnet"

TELNET_SERVER.EXE	: -
		Telnet_Server.OLB(TELNET_SERVER=TELNET_SERVER.OBJ), -
		Telnet_Server.OLB(TELNET_COMMON=TELNET_COMMON.OBJ), -
		Telnet_Server.OLB(PRINT=PRINT.OBJ), -
		Telnet_Server.OLB(TELNETMSG=TELNETMSG.OBJ), -
		Telnet_Server.OPT
	$(LINK) $(LINKFLAGS) -
		Telnet_Server.OLB/lib/include=TELNET_SERVER, -
		Telnet_Server.OPT/OPT, -
		CMUIP_SRC:[CENTRAL]NETERROR.OBJ, -
		CMUIP_SRC:[CENTRAL]NETLIB.OLB/lib

TELNET.EXE	: -
		Telnet.OLB(TELNET=TELNET.OBJ), -
		Telnet.OLB(TELNETMSG=TELNETMSG.OBJ), -
		Telnet.OLB(TELNET_COMMON=TELNET_COMMON.OBJ), -
		Telnet.OLB(OPTIONS=OPTIONS.OBJ), -
		Telnet.OLB(Telnet_Parse=TELNET_PARSE.OBJ), -
		Telnet.OLB(Telnet_ROUTINES=ROUTINES.OBJ), -
		Telnet.OLB(KEY_Parse=KEY.OBJ), -
		Telnet.OLB(PORT_Parse=PORT.OBJ), -
		Telnet.OLB(PRINT=PRINT.OBJ), -
		Telnet.OLB(TELNET_Term=TELNET_Term.OBJ), -
		Telnet.OLB(Condition=CONDITION.OBJ), -
		Telnet.OLB(LOG_UTIL=LOG_UTIL.OBJ), -
		Telnet.OPT
	$(LINK) $(LINKFLAGS) TELNET.OLB/lib/include=TELNET, -
		Telnet.OPT/OPT,-
		CMUIP_SRC:[CENTRAL]NETERROR.OBJ, -
		CMUIP_SRC:[CENTRAL]NETLIB.OLB/lib

TELNET_SERVER.OBJ	: TELNET_SERVER.B32, TELNET.L32, OPTION.L32

TELNET.OBJ	: TELNET.B32, TELNET.L32, PRINT.L32, OPTION.L32, CLI.L32

TELNET_Term.OBJ	: TELNET_Term.B32, PRINT.L32

TELNET_COMMON.OBJ	: TELNET_COMMON.B32, OPTION.L32, PRINT.L32

TELNET_PARSE.OBJ	: TELNET_PARSE.CLD

TELNET.HLB	: TELNETCMD.HLP
	LIBRARY /CREATE /HELP TELNET.HLB TELNETCMD.HLP

TTYLOC.OBJ	: TTYLOC.B32, TTYLOC.L32

OPTIONS.OBJ	: OPTIONS.B32, TELNET.L32, OPTION.L32

OPTION.L32	: OPTION.R32, TELNET.L32

ROUTINES.OBJ	: ROUTINES.B32, TELNET.L32, PRINT.L32

CONDITION.OBJ	: Condition.B32, TELNET.L32, PRINT.L32

LOG_UTIL.OBJ	: LOG_UTIL.B32

KIT		: TELNET032.B, TELNET.SRC
	CONTINUE

TELNET032.B	: TELNET_LINK.COM, -
		Telnet.OPT, -
		TELNET.OLB, -
		TELNET.HLP, TELNET.CLD, TELNET.HLB
	SET PROT=(W:RE) $(MMS$SOURCE_LIST)
	PURGE $(MMS$SOURCE_LIST)
	BACKUP $(MMS$SOURCE_LIST) $(MMS$TARGET)/SAVE/BLOCK=8192/INTER

TELNET.SRC	: -
	TELNET_LINK.COM, -
	Telnet.OPT, -
	Telnet_Exe.COM, -
	DESCRIP.MMS, -
	TELNET.CLD, -
	TELNET_PARSE.CLD, -
	TELNET.HLP, -
	TELNETCMD.HLP, -
	TELNETMSG.MSG, -
	CLI.R32, -
	OPTION.R32, -
	PRINT.R32, -
	TELNET.R32, -
	TTYLOC.R32, -
	CONDITION.B32, -
	KEY.B32, -
	OPTIONS.B32, -
	PORT.B32, -
	PRINT.B32, -
	ROUTINES.B32, -
	TELNET.B32, -
	TELNET_COMMON.B32, -
	TELNET_TERM.B32, -
	TTYLOC.B32, -
	LOG_UTIL.B32
	PURGE $(MMS$SOURCE_LIST)
	BACKUP $(MMS$SOURCE_LIST) $(MMS$TARGET)/SAVE/BLOCK=8192/INTER

!  DEC/CMS REPLACEMENT HISTORY, Element DESCRIP.MMS
!  *2     5-MAR-1991 19:49:00 SYNFUL "Included checking for TELNET.OPT and TELNET_SERVER.OPT"
!  *1     5-MAR-1991 18:57:06 SYNFUL "Descrip.Mms for IP/TCP Telnet program"
!  DEC/CMS REPLACEMENT HISTORY, Element DESCRIP.MMS
