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
! DESCRIP.MMS
!
!	Copyright (C) 1988	Carnegie Mellon University
!
! Description:
!
!	File for building IPACP. This file should be copied into the
!	target directory and then run through MMS. It will copy any
!	other files it needs from the appropriate places.
!
! Written By:
!
!	Vince Fuller	Oct-1986	CMU-CS/RI
!
! Modifications:
!
!	02-Feb-1990	Bruce R. Miller 	CMU NetDev
!	Made IP transport modules into seperate loadable images.
!
!	17-Nov-1989	Bruce R. Miller 	CMU NetDev
!	Added new ICMP,UDP,IP modules.
!
!	19-JAN-1989	Dale Moore	CMU-CS
!	Removed DECnet, SLIP and X25.  If you want 'em, add em.
!	IPACP.OLB(DECnet_Driver=DECNET_DRV.OBJ), -
!	IPACP.OLB(SLIP_Driver=SLIP_DRV.OBJ), -
!	IPACP.OLB(X25_Driver=X25_DRV.OBJ)
!
!	24-FEB-1988	Dale Moore	CMU-CS/RI
!	Added Release Notes.
!--

! Define switches and executable name

!CDEBUG           = /DEBUG/list/show=(EXPANSION)/MACH/CROSS
!BDEBUG           = /DEBUG/list/trace
CDEBUG		= /NODEBUG /NOLIST
BDEBUG		= /NODEBUG /NOLIST /TRACE

INCLUDE_DIR     = (CMUIP_SRC:[CENTRAL.include],CMUIP_SRC:[CENTRAL.include.netinet])

BFLAGS = /OBJECT=$(MMS$TARGET_NAME) $(BDEBUG) /TERM=STAT
CFLAGS = /OBJECT=$(MMS$TARGET_NAME) $(CDEBUG)/INCLUDE=$(INCLUDE_DIR)/noopt

MFLAGS = /LIST

LINKFLAGS = /NODEBUG/NOTRACE /EXECUTABLE=$(MMS$TARGET_NAME)

EXENAME = IPACP.EXE

MAPNAME = IPACP.MAP

LIBS = [-.central]NETERROR.OBJ,[-.central]NETFNAMES.OBJ,SYS$SYSTEM:SYS.STB/SEL

DOIT : IPACP.EXE
	CONTINUE

! Route code test

ROUTE.EXE : ROUTE.OBJ
     $(LINK) /exe=ROUTE /TRACE/debug/SYM/MAP/FULL ROUTE.OBJ

ROUTE.OBJ : ROUTE.C

! The IPACP

IPACP.EXE : -
	SNMP.OLB,-
	RPC.OLB,-
	FAKE_VM.OBJ,-
	IPACP.OLB(IPACP=Main.OBJ),-
	IPACP.OLB(UDP=UDP.OBJ),-
	IPACP.OLB(ICMP=ICMP.OBJ),-
	IPACP.OLB(ICMP_User=ICMP_USER.OBJ),-
	IPACP.OLB(IP=IP.OBJ),-
	IPACP.OLB(IP_User=IP_User.OBJ),-
	IPACP.OLB(User=User.OBJ),-
	IPACP.OLB(Config=CONFIG.OBJ),-
	IPACP.OLB(IOUTIL=IOUTIL.OBJ),-
	IPACP.OLB(MACLIB=MACLIB.OBJ),-
	IPACP.OLB(MEMGR=MEMGR.OBJ),-
	IPACP.OLB(NMLOOK=NMLOOK.OBJ),-
	IPACP.OLB(PokeAddr=POKEADDR.OBJ),-
	IPACP.OLB(RPC=RPC.OBJ),-
	IPACP.OLB(Segin=TCP_SEGIN.OBJ),-
	IPACP.OLB(SNMP_HOOK=SNMP_HOOK.OBJ),-
	IPACP.OLB(TCP=TCP.OBJ),-
	IPACP.OLB(TCP_Mech=TCP_MECH.OBJ),-
	IPACP.OLB(TCP_User=TCP_USER.OBJ),-
	IPACP.OLB(Telnet=TCP_Telnet.OBJ),-
	IPACP.OLB(Telnet_MISC=TCP_Telnet_MISC.OBJ),-
	IPACP.OLB(XE_Driver=XEDRV.OBJ),-
	IPACP.OLB(ARP=XEDRV_ARP.OBJ),-
	IPACP.OLB(XDR=XDR.OBJ),-
	IPACP.OLB(Memory=MEM.OBJ)
     LINK /EXE=IPACP_DEB /TRACE/debug -
	FAKE_VM.OBJ, -
	IPACP.OLB/LIBRARY/INCLUDE=(IPACP), -
	SNMP.OLB/LIBRARY/INCLUDE=(SNMP_AGENT),-
	RPC.OLB/LIBRARY, -
	[]IPACP.OPT/OPT, -
	$(LIBS)
     $(LINK) /exe=IPACP /NOTRACE/NOdebug/SYM/MAP=$(MAPNAME)/FULL -
	IPACP.OLB/LIBRARY/INCLUDE=(IPACP), -
	SNMP.OLB/LIBRARY/INCLUDE=(SNMP_AGENT), -
	RPC.OLB/LIBRARY, -
	[]IPACP.OPT/OPT, -
	$(LIBS)

!     LINK /EXE=IPACP_PCA /DEBUG=SYS$LIBRARY:PCA$OBJ.OBJ  -
!	IPACP.OLB/LIBRARY/INCLUDE=(IPACP),$(LIBS)


ALL : IPACP.EXE, KIT

CLEANUP :
     DELETE *.OBJ;*, *.L32;*, *.EXE;*, *.MAP;*


! Define the standard libraries, used by most modules

TLIBS = STRUCTURE.L32,TCPMACROS.L32

!Temp
XEDRV.OBJ	: XEDRV.BLI, XEDRV.L32, XEARP.L32
XEDRV_ARP.OBJ	: XEDRV_ARP.BLI, XEDRV.L32, XEARP.L32
XEDRV.L32	: XEDRV.REQ
XEARP.L32	: XEARP.REQ

! Define the dependancies for each module.

USER.OBJ	: USER.BLI, $(TLIBS)
MAIN.OBJ	: MAIN.BLI, TCPMOD.L32, $(TLIBS)
TCP.OBJ		: TCP.BLI, TCP.L32, TCPMOD.L32, SNMP.L32, $(TLIBS)
TCP_SEGIN.OBJ	: TCP_SEGIN.BLI, TCP.L32, SNMP.L32, $(TLIBS)
TCP_USER.OBJ	: TCP_USER.BLI, TCP.L32, $(TLIBS)
TCP_MECH.OBJ	: TCP_MECH.BLI, TCP.L32, SNMP.L32, $(TLIBS)
TCP_TELNET.OBJ	: TCP_TELNET.BLI, TCP.REQ, TELNET.L32, $(TLIBS)

TCP_TELNET_MISC.OBJ : TCP_TELNET_MISC.BLI, TELNET.L32

CONFIG.OBJ 	: CONFIG.BLI, SNMP.L32, $(TLIBS)
MEMGR.OBJ 	: MEMGR.BLI, TCP.L32, $(TLIBS)
MACLIB.OBJ 	: MACLIB.MAR
IOUTIL.OBJ 	: IOUTIL.BLI
POKEADDR.OBJ 	: POKEADDR.MAR

IP.OBJ 		: IP.BLI, SNMP.L32, $(TLIBS)
IP_USER.OBJ 	: IP_USER.BLI, $(TLIBS)
ICMP.OBJ 	: ICMP.BLI, SNMP.L32, $(TLIBS)
ICMP_USER.OBJ 	: ICMP_USER.BLI, SNMP.L32, $(TLIBS)
UDP.OBJ 	: UDP.BLI, SNMP.L32, $(TLIBS)
NMLOOK.OBJ 	: NMLOOK.BLI, TCPMACROS.L32
SNMP_HOOK.OBJ 	: SNMP_HOOK.BLI SNMP.L32 ASN1.L32 
MEM.OBJ		: MEM.BLI

RPC.OBJ		: RPC.C, RPC.H
XDR.OBJ		: XDR.C

TCP.L32		: TCP.REQ
STRUCTURE.L32 :	STRUCTURE.REQ
TCPMACROS.L32 :	TCPMACROS.REQ, MACROSWI.REQ
SNMP.L32	: SNMP.REQ ASN1.L32
ASN1.L32	: ASN1.REQ


! Rules for building the SNMP library
snmp.OLB	: -
	SNMP.OLB(snmp_agent=snmp_agent.obj), -
	SNMP.OLB(asn1=asn1.obj), -
	SNMP.OLB(snmp=snmp.obj), -
	SNMP.OLB(snmp_auth=snmp_auth.obj)
	CONTINUE

# CMU-OpenVMS/IP specific module

snmp_agent.obj	: snmp_agent.c asn1.h snmp.h snmp_impl.h

#snmp_mib.obj	: snmp_mib.c snmp_vars.h asn1.h snmp.h snmp_impl.h mib.h

#snmp_vars.obj	: snmp_vars.bli asn1.req snmp_impl.req

# independant modules

asn1.obj	: asn1.h

snmp_auth.obj	: snmp_auth.c asn1.h snmp.h snmp_impl.h

snmp.obj	: snmp.c asn1.h snmp.h snmp_impl.h mib.h

! Rules for building the RPC library
RPC.OLB	: -
	RPC.OLB(MOUNT=MOUNT.obj),-
	RPC.OLB(NFS=NFS.obj)
	CONTINUE

# Rules for the RPC services.

MOUNT.OBJ	: MOUNT.C, NFS.H
NFS.OBJ		: NFS.C, NFS.H


!
! Rules for building the kit.
!

KIT	: IPACP066.B, IPACP.SRC
	CONTINUE

IPACP066.B	: IPACP_LINK.COM, -
		  IPACP.MAP, IPACP.STB, IPACP.OLB, IPACP.OPT, -
		  SNMP.OLB, RPC.OLB
	SET PROT=(W:RE) $(MMS$SOURCE_LIST)
	PURGE $(MMS$SOURCE_LIST)
	BACKUP $(MMS$SOURCE_LIST) $(MMS$TARGET)/SAVE/INTER/BLOCK=8192

IPACP.SRC	: -
	DESCRIP.MMS, -
	IPACP.OPT,-
	IPACP_LINK.COM, -
	FAKE_VM.B32, -
	CONFIG.BLI, -
	IOUTIL.BLI, -
	ICMP.BLI, -
	ICMP_USER.BLI, -
	IP.BLI, -
	IP_USER.BLI, -
	MAIN.BLI, -
	MEMGR.BLI, -
	NMLOOK.BLI, -
	SNMP_HOOK.BLI, -
	TCP.BLI, -
	TCP_MECH.BLI, -
	TCP_SEGIN.BLI, -
	TCP_TELNET.BLI, -
	TCP_TELNET_MISC.BLI, -
	TCP_USER.BLI, -
	UDP.BLI, -
	USER.BLI, -
	MOUNT.C, NFS.C, RPC.C, XDR.C,-
	ASN1.C, SNMP.c, SNMP_agent.c, SNMP_auth.c, -
	MACLIB.MAR, -
	POKEADDR.MAR, -
	MACROSWI.REQ, STRUCTURE.REQ, TCP.REQ, TCPMACROS.REQ, TCPMOD.REQ, -
	TELNET.REQ, SNMP.REQ, -
	NFS.H, RPC.H, -
	ASN1.H, MIB.H, SNMP.H, SNMP_IMPL.H, SNMP_VARS.H
	PURGE *.OPT,*.BLI,*.REQ,*.MAR,*.c,*.h,DESCRIP.MMS
	BACKUP *.BLI,*.REQ,*.c,*.h,*.MAR,-
			DESCRIP.MMS,*.OPT,-
			IPACP_LINK.COM, -
			FAKE_VM.B32 -
		 $(MMS$TARGET)/SAVE/INTER/BLOCK=8192

!	BACKUP *.REQ, *.BLI, *.MMS, *.COM, *.MAR, *.PAS -
!		 $(MMS$TARGET)/SAVE/INTER/BLOCK=8192

SRConly :
        purge /log
        del/log  *.EXE.,*.obj.,*.L32.,*.map.,*.lis.,*.STB.*,*.ckp.
