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
! DESCRIP.MMS	Copyright (c) 1989	Carnegie Mellon University
!
! Description:
!
!	An MMS file describing dependencies for TCP/IP central utilities.
!
! Written By:	Bruce R. Miller		CMU Network Development
! Date:		October 26, 1989
!
! Modifications:
!
!--

DEBUG = /NOTRACE/NODEBUG
LDEBUG = /TRACE/NODEBUG

BFLAGS		= /LIST $(DEBUG) /OBJECT=$(MMS$TARGET) /TERMINAL=STATISTICS
CDEBUG           = /DEBUG/list/show=(EXPANSION)/MACH/CROSS
LINKFLAGS	= $(LDEBUG) /EXEC=$(MMS$TARGET) /MAP

DEFAULT	: NETLIB.OLB, -
	  NETXPORT.L32,-
	  NETWORK.L32,-
	  NETCOMMON.L32,-
	  NETCNTRL.L32,-
	  NAMSYM.L32,-
	  NETERROR.OBJ, NETERROR.L32,-
	  NETAUX.OBJ,NETAUX.L32,-
	  NETConfig.L32,-
	  NETDevices.OBJ,NETDevices.L32,-
	  NETVMS.L32,-
	  NETMACLIB.OBJ,-
	  NETTCPIP.L32, -
	  NETFNAMES.OBJ
	CONTINUE

! Network Library of useful routines.
NETLIB.OLB	: -
		NETLIB.OLB(NETFNAMES=NETFNAMES.OBJ), -
		NETLIB.OLB(NETAUX=NETAUX.OBJ), -
		NETLIB.OLB(NETDEVICES=NETDevices.OBJ), -
		NETLIB.OLB(NETMACLIB=NETMACLIB.OBJ)
	CONTINUE

! Field definitions
NETXPORT.L32	: NETXPORT.REQ

! Network macros and literals, common definitions
NETCOMMON.L32 :  NETCOMMON.REQ NETXPORT.L32 NETCONFIG.L32

! Network macros, user definitions
NETWORK.L32 :  NETWORK.REQ, NETCOMMON.L32, NETXPORT.L32

! Network Control macros
NETCNTRL.L32 :	NETCNTRL.REQ

! NetError
NETERROR.OBJ :  NETERROR.MSG
NETERROR.L32 :  NETERROR.REQ

! NamSym
NAMSYM.L32 : NAMSYM.REQ NETWORK.L32 NETXPORT.L32

! NetAux
NETAUX.OBJ : NETAUX.BLI NETWORK.L32 NETCNTRL.L32
NETAUX.L32 : NETAUX.REQ

! NetConfig
NETConfig.L32 : NETConfig.REQ NETXPORT.L32

! NetDEVICES
NETDevices.OBJ	: NETDevices.BLI NETDevices.L32
NETDevices.L32	: NETDevices.REQ

! NetVMS
NetVMS.L32	: NetVMS.REQ

! Network Macro routines
NetMacLib.OBJ	: NetMacLib.mar

! Filename translation routines
NETFNames.OBJ	: NETFNames.c

! TCP/IP structure definitions
NetTCPIP.L32	: NetTCPIP.Req

! Build the savesets.

KIT	: CENTRAL065.B, CENTRAL.SRC
	CONTINUE

CENTRAL065.B	: -
	  NETXPORT.L32,-
	  NETWORK.L32,-
	  NETCOMMON.L32,-
	  NETCNTRL.L32,-
	  NAMSYM.L32,-
	  NETERROR.OBJ, NETERROR.L32,-
	  NETAUX.OBJ,NETAUX.L32,-
	  NETConfig.L32,-
	  NETDevices.OBJ,NETDevices.L32,-
	  NETVMS.L32,-
	  NETMACLIB.OBJ,-
	  NETTCPIP.L32,-
	  NETFNAMES.OBJ
	SET PROT=(W:RE) $(MMS$SOURCE_LIST)
	PURGE $(MMS$SOURCE_LIST)
	BACKUP $(MMS$SOURCE_LIST) $(MMS$TARGET)/SAVE_SET /BLOCK_SIZE=8192 /INTERCHANGE

CENTRAL.SRC	: -
	Descrip.mms, -
	NetAux.Bli, NetAux.Req, -
	NetDevices.Bli, NetDevices.Req, -
	NetConfig.req, -
	NetError.Msg, NetError.Req, -
	NetXport.Req, -
	Network.Req, -
	NetCommon.Req, -
	NetCntrl.Req, -
	NamSym.Req,-
	NetVMS.REQ,-
	NetMacLib.MAR,-
	NetTCPIP.Req,-
	NetFNames.c
	PURGE  $(MMS$SOURCE_LIST)
	BACKUP  $(MMS$SOURCE_LIST),[.include...]*.h -
		$(MMS$TARGET)/SAVE_SET /BLOCK_SIZE=8192 /INTERCHANGE

