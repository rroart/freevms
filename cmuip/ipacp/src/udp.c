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
//TITLE "User Datagram Protocol Handler"
//SBTTL "User Datagram Protocol Handler Overview"
/*

Module:

	UDP

Facility:

	User Datagram Protocol (UDP) handler

Abstract:

	UDP provides the user with a potentially unreliable datagram
	service via the Internet Protocol (IP). This module handles the
	UDP interface between the user and the IP layer.

Author:

	Vince Fuller, CMU-CSD, March, 1986
	Copyright (c) 1986, 1987, Vince Fuller and Carnegie-Mellon University

Modification History

4.0e	21-Jan-1992	Marc A. Shannon		CMU Group N
	Don't compute checksum for UDP packets going out from here to IP
	since a broadcast packet (to 255.255.255.255) will need its checksum
	recomputed when the destination address is changed to the local
	broadcast (something like 128.2.255.255).

4.0d	28-Aug-1991	Henry W. Miller		USBR
	Range check size of buffers and return NET$_IR if requested
	buffer size is greater than Max_UDP_Data_Size.

4.0c	18-Jul-1991	Henry W. Miller		USBR
	Use LIB$GET_VM_PAGE and LIB$FREE_VM_PAGE rather then LIB$GET_VM
	and LIB$FREE_VM, and check return status.

4.0b	30-May-1991	Henry W. Miller		USBR
	In UDP_COPEN_DONE(), log failures.

4.0a	13-Jan-1991	Henry W. Miller		USBR
	Make UDPTTL a configurable variable.
	Updated IDENT.

	10-Sep-1990     Henry W. Miller 	USBR
	Make UDP_CS}() use DEFTTL.

4.0   04-Dec-1989, Bruce R. Miller	CMU Network Development
	Restructured the UDP packet.  Wildcard receives are passed
	by the driver into a user supplied buffer instead of being
	appended to the begining of the user's data buffer.

	Removed the Internal UDP processing.  No one uses it anymore.
	Changed name UCB to UDPCB to avoid confusion with Unit
	Control Blocks.  Removed address mode.  UDP address can
	now be spedified with a user supplid buffer.

3.7   11-FEB-1988, Dale Moore
	On UDP$S} and ADDR_MODE check to see if sufficient bytes
	given by user for udp header. If not return BTS.

3.6  19-Nov-87, Edit by VAF
	Know about IP$S} failures and give user return status of NET$_NRT
	on failures (no route to destination).

3.5  23-Sep-87, Edit by VAF
	Don't check checksums for packets with no checksums (i.e. checksum=0)
	Believe it or not, there are still weenies out there who don't put
	checksums in their UDP packets...

3.4  30-Jul-87, Edit by VAF
	Use $$KCALL macro instead of using $CMKRNL directly.

3.3  26-Mar-87, Edit by VAF
	Check for aborted UCB status when receiving UDP messages from the
	network and when queuing user send and receive requests.

3.2  23-Mar-87, Edit by VAF
	Use TCP's packet buffer sizes for speed.

3.1  10-Mar-87, Edit by VAF
	Make "address-mode" also handle the UDP ports, so one connection can
	handle packets to/from multiple foreign ports. Note that the local
	port is always resolved at open time and cannot be set this way.

3.0   3-Mar-87, Edit by VAF
	Change calling sequence of User$Post_IO_Status. Add facility for
	returning ICMP messages to the user for "address mode" connections.

2.9   2-Mar-87, Edit by VAF
	Add support for klugy "address mode" - allows user to specify the IP
	addresses for each UDP$S} and returns the IP addresses of the input
	packet for each UDP$RECEIVE.

2.8  19-Feb-87, Edit by VAF
	Use new name lookup routines.
	Fix a bug in UCB_FIND - infinite loop if no UCB's...

2.7  18-Feb-87, Edit by VAF
	Support open with IP address instead of host name.

2.6  12-Feb-87, Edit by VAF
	Modifications for domain service.

2.5   5-Feb-87, Edit by VAF
	Call USER$CHECK_ACCESS for user UDP opens.

2.4   4-Feb-87, Edit by VAF
	Recode UDP open to avoid name lookup for internal opens.

2.3   3-Feb-87, Edit by VAF
	Fix bug in UDP_ADLOOK_DONE - was using the name pointer as the name
	length when copying it back to the UCB. Crashes ACP.

2.2  10-Dec-86, Edit by VAF
	Change order of arguments to Gen_Checksum.

2.1   2-Oct-86, Edit by VAF
	On ICMP errors, don"t close internal UCB"s - just abort all of their
	pending requests. Internal UCB's have to take care of themselves...

2.0  30-Sep-86, Edit by VAF
	Make IUDP_CLOSE give the correct number of arguments to UCB_CLOSE.

1.9  14-Aug-86, Edit by VAF
	Be NOINT when examining UCB->UCB$ARGBLK.
	Make Kill_UDP_Reqeuests also post UCB->UCB$ARGBLK.

1.8  13-Aug-86, Edit by VAF
	Add UDP debugging dump routines.

1.7  12-Aug-86, Edit by VAF
	Add internal entry points for use by ACP green protocol module.

1.6  12-Aug-86, Edit by VAF
	Make UDP$OPEN use the green protocol name lookup routines.

1.5   9-Aug-86, Edit by VAF
	Don't use SET_HOSTS any more - domain system makes things too
	complicated to use common routine for both TCP and UDP.

1.4   4-Aug-86, Edit by VAF
	Forgot to post request in UDP$CLOSE/UDP$ABORT.

1.3  31-Jul-86, Edit by VAF
	Debugging, general reorganization.

1.2  29-Jul-86, Edit by VAF
	Start working on the real implementation.

1.1  23-Jul-86, Edit by VAF
	Add stub routines for user interface.

1.0  24-Mar-86, Edit by VAF
	Original, nonfunctional stub routines for IP interface.
*/

//SBTTL "Module definition"	

MODULE UDP( IDENT="4.0e",LANGUAGE(BLISS32),
	    ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			    NONEXTERNAL=LONG_RELATIVE),
	    LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	    OPTIMIZE,OPTLEVEL=3,ZIP)=

{

// Include standard definition files

#include "SYS$LIBRARY:STARLET";
#include "CMUIP_SRC:[CENTRAL]NETERROR";
#include "CMUIP_SRC:[CENTRAL]NETXPORT";
#include "CMUIP_SRC:[CENTRAL]NETVMS";
#include "CMUIP_SRC:[CENTRAL]NETCOMMON";
#include "CMUIP_SRC:[CENTRAL]NETTCPIP";
#include "STRUCTURE";
#include "TCPMACROS";
#include "SNMP";

!*** Special literals from USER.BLI ***

extern signed long LITERAL
    UCB$Q_DDP,
    UCB$L_CBID,
    UCB$L_EXTRA;

// External data items

extern signed long
    INTDF,
    AST_In_Progress,
    UDP_User_LP,
    LOG_STATE,
    MIN_PHYSICAL_BUFSIZE,
    MAX_PHYSICAL_BUFSIZE;

// External routines

extern

// MACLIB.MAR

 void    Swapbytes,
 void    Movbyt,

// MEMGR.BLI

 void    MM$UArg_Free,
    MM$QBLK_Get,
 void    MM$QBLK_Free,
    MM$Seg_Get,
 void    MM$Seg_Free,

// USER.BLI

    USER$GET_LOCAL_PORT,
    USER$CHECK_ACCESS,
    USER$Err,
 VOID    IO$POST,
 void    User$Post_IO_Status,

// IP.BLI

 VOID    IP$SET_HOSTS,
    IP$S},
    Gen_Checksum,

// NMLOOK.BLI

 VOID    NML$CANCEL,
 VOID    NML$GETALST,
 VOID    NML$GETNAME,

// IOUTIL.BLI

    GET_IP_ADDR,
 VOID    ASCII_DEC_BYTES,
 VOID    ASCII_HEX_BYTES,
 VOID    LOG_FAO,
 VOID    QL_FAO,

// SNMP.BLI

    SNMP$NET_INPUT,

// RPC.BLI

    RPC$INPUT,
    RPC$CHECK_PORT;

signed long
    UDPTTL	: INITIAL (32);

extern signed long
    SNMP_SERVICE,
    RPC_SERVICE;


//SBTTL "UDP data structures"

LITERAL
    Max_UDP_Data_Size = 16384,	// Max UDP data size
    UDPTOS = 0,			// Type of service
    UDPDF = FALSE;		// Don't fragment flag (try fragmenting)

// Define the "UDPCB" - UDP analogue of TCB.

$FIELD  UDPCB_Fields (void)
    SET
    UDPCB$Foreign_Host	= [$Ulong],	// UDP foreign host number
    UDPCB$Foreign_Port	= [$Ulong],	//     foreign port
    UDPCB$Local_Host	= [$Ulong],	//     local host
    UDPCB$Local_Port	= [$Ulong],	//     local port
    UDPCB$Foreign_Hname	= [$Bytes(MAX_HNAME)],
    UDPCB$Foreign_Hnlen	= [$Short_Integer],
    UDPCB$USR_Qhead	= [$Address],	// User receive request queue
    UDPCB$USR_Qtail	= [$Address],
    UDPCB$NR_Qhead	= [$Address],	// Net receive queue
    UDPCB$NR_Qtail	= [$Address],
    UDPCB$NR_Qcount	= [$Short_Integer],
    UDPCB$Flags		= [$Bytes(2)],
    $OVERLAY(UDPCB$Flags)
	UDPCB$Wildcard	= [$Bit],	// UDPCB opened with wild FH/FP/LH
	UDPCB$Addr_Mode	= [$Bit],	// IP addresses in data buffer
	UDPCB$Aborting	= [$Bit],	// UDPCB is closing
	UDPCB$NMLook	= [$Bit],	// UDPCB has an outstanding name lookup
    $CONTINUE
    UDPCB$UDPCBID	= [$Address],	// UDPCB_Table index for this connection
    UDPCB$UCB_Adrs	= [$Address],	// Connection UDPCB address
    UDPCB$UArgs		= [$Address],	// Uarg block in pending open
    UDPCB$User_ID	= [$Bytes(4)],	// Process ID of owner
    UDPCB$PIOchan	= [$Bytes(2)]	// Process IO channel
    TES;

LITERAL
    UDPCB_Size = $Field_Set_Size;
MACRO
    UDPCB_Structure = BLOCK->UDPCB_Size FIELD(UDPCB_Fields) %;
//MESSAGE(%NUMBER(UDPCB_Size)," longwords per UDPCB")


//SBTTL "UDP data storage"

static signed long
    UDPIPID  = 1,	// Current IP packet ID
    UDPCB_Count  = 0,	// Count of active UDPCBs
    UDPCB_TABLE : VECTOR[MAX_UDPCB+1];// Table of UDPCBs

signed long
    UDP_MIB : UDP_MIB_struct;	// UDP Management Information Block


//SBTTL "UDP packet logger"
/*
    Queue up a log entry to dump out a UDP packet.
 )%

Log_UDP_Packet(Seg,SwapFlag,SendFlag) : NOVALUE (void)
    {
    MAP
	struct UDPkt_Structure * Seg;
    signed long
	sptr,
	segdata,
	segcopy : UDPkt_Structure,
	struct UDPkt_Structure * seghdr;

    seghdr = seg;		// Point at segment header
    segdata = seg + UDP_Header_Size;
    if (SwapFlag)		// Need to byteswap header?
	{
	CH$MOVE(UDP_Header_Size,CH$PTR(seg),CH$PTR(segcopy)); // Make a copy
	seghdr = segcopy;	// Point at this version...
	SwapBytes(UDP_Header_Size/2,seghdr); // Swap header bytes
	};

// Print first part of info

    if (SendFlag)
	sptr = %ASCID"Sent"
    else
	sptr = %ASCID"Received";

// Log the contents of the UDP header

    QL$FAO(%STRING("!%T !AS UDP packet, SEG=!XL, DATA=!XL!/",
		   "!_SrcPrt:!_!XL (!UL)!_DstPrt:!_!XL (!UL)!/",
		   "!_Length:!_!SL!_CKsum:!_!SL!/"),
	    0,sptr,seg,segdata,
	   seghdr->UP$Source_Port,seghdr->UP$Source_Port,
	   seghdr->UP$Dest_Port,seghdr->UP$Dest_Port,
	   seghdr->UP$Length,seghdr->UP$Checksum);

// If there is any data in the segment, then dump it, too

    if (seghdr->UP$Length > UDP_Header_Size)
	{
	LITERAL
	    maxhex = 20,
	    maxasc = 50;
	signed long
	    datalen,
	    asccnt,
	    hexcnt,
	    DESC$STR_ALLOC(dathex,maxhex*3);
	datalen = seghdr->UP$Length - UDP_Header_Size;
	if (datalen > maxasc)
	    asccnt = maxasc
	else
	    asccnt = datalen;
	if (datalen > maxhex)
	    hexcnt = maxhex
	else
	    hexcnt = datalen;
	ASCII_Hex_Bytes(dathex,hexcnt,segdata,dathex->DSC$W_LENGTH);
	QL$FAO(%STRING("!_Data Count: !SL!/",
		       "!_HEX:!_!AS!/",
		       "!_ASCII:!_!AF!/"),
	       datalen,dathex,asccnt,segdata);
	};
    }

//SBTTL "UDPCB_Find - look up UDP control block"

UDPCB_Find(Src$Adrs,Src$Port,Dest$Port)
    {
    signed long
	Ucount,
	UDPCBIX,
	struct UDPCB_Structure * UDPCB;

    Ucount = UDPCB_Count;
    UDPCBIX = 1;
    while ((Ucount > 0) && (UDPCBIX <= Max_UDPCB))
	{
	if ((UDPCB = UDPCB_Table[UDPCBIX]) != 0)
	    {
	    IF ((UDPCB->UDPCB$Foreign_Host == WILD) OR
		(UDPCB->UDPCB$Foreign_Host == Src$Adrs)) AND
	       ((UDPCB->UDPCB$Foreign_Port == WILD) OR
	        (UDPCB->UDPCB$Foreign_Port == Src$Port)) AND
	       (UDPCB->UDPCB$Local_Port == Dest$Port) THEN
		return UDPCB;
	    Ucount = Ucount-1;
	    };
	UDPCBIX = UDPCBIX + 1;
	};
    return 0;
    }


//SBTTL "UDP input handler"
/*
    Come here at AST level when input packet is determined to be UDP packet.
    At present, all UDP input handling is done at AST level, so we search
    the UDPCB list and queue the UDP packet for deliver here.
*/

FORWARD ROUTINE
    UDP_S},
    Queue_User_UDP;

UDP$Input(Src$Adrs,Dest$Adrs,BufSize,Buf,SegSize,Seg): NOvalue=
    {
    MAP
	struct UDPkt_Structure * Seg;
    signed long
	RC,
	Uptr,
	Ucount,
	RPC_index,
	UDPCBIX,
	sum,
	delete,
	struct IPADR$ADDRESS_BLOCK * Aptr,
	struct UDPCB_Structure * UDPCB;
    LABEL
	X1,X2;

// Assume this packet should be deleted

    delete = TRUE;

// Log the UDP packet if desired

    if ($$LOGF(LOG$UDP))
	Log_UDP_Packet(Seg,TRUE,FALSE);

// Verify UDP checksum, if there is one

    if (Seg->UP$Checksum != 0)
	{
	sum = Gen_Checksum(Segsize,Seg,Src$Adrs,Dest$Adrs,UDP_Protocol);
	if (sum NEQU %X"FFFF")
	    {			// Bad checkum - log & drop packet
	    UDP_MIB->MIB$udpInErrors = UDP_MIB->MIB$udpInErrors + 1;
	    if ($$LOGF(LOG$UDP))
		QL$FAO("!%T UDP seg !XL dropped on bad checksum (!XL)!/",0,
			.Seg,sum);
	    RETURN;
	    };
	};

// Fix byte order of the UDP header

    SwapBytes(UDP_Header_Size/2,Seg);

    
// Setup pointer to UDP data and UDP data size

    Uptr = Seg + UDP_Header_Size;
    Ucount = SegSize - UDP_Header_Size;

    // Check to see if it's an RPC port
    IF (RPC_SERVICE AND
	((RPC_index = RPC$CHECK_PORT(Seg->UP$Dest_Port)) GEQ 0)) THEN
	{
	signed long
	    out_len,
	    out_buff : VECTOR [16384,BYTE];

	// Keep count
	UDP_MIB->MIB$udpInDatagrams = UDP_MIB->MIB$udpInDatagrams + 1;

	out_len = 16384; // initial size of buffer

	// Pass the UDP data to the Port Mapper module
	// The results are returned in out_buff, size in out_len
	RC = RPC$INPUT( RPC_index, Src$Adrs,Dest$Adrs,
			     Seg->UP$Source_Port,Seg->UP$Dest_Port,
			     UCount,Uptr,out_buff,out_len);
        if ((RC && (out_len > 0)))
	    {
	    // Send off the reply datagram
	    RC = UDP_S}(Dest$Adrs,Src$Adrs,
			  Seg->UP$Dest_Port,Seg->UP$Source_Port,
		 	  out_buff,out_len);
	    if (RC != SS$_NORMAL)
		XLOG$FAO(LOG$UDP,"!%T UDP RPC reply error, RC=!XL!/",0,RC);
	    };

	// Release the input datagram buffer
	if (delete)
	    MM$Seg_Free(Bufsize,Buf);

	RETURN	// Don't pass this buffer on to the upper layers
	};

// Check to see if it's an SNMP packet
    if ((Seg->UP$Dest_Port == UDP_PORT_SNMP) && SNMP_SERVICE)
	{
	signed long
	    out_len,
	    out_buff : VECTOR [512,BYTE];

	// Keep count
	UDP_MIB->MIB$udpInDatagrams = UDP_MIB->MIB$udpInDatagrams + 1;

	out_len = 512; // initial size of buffer

	// Pass the UDP data to the SNMP module
	// The results are returned in out_buff, size in out_len
	RC = SNMP$NET_INPUT( Src$Adrs,Dest$Adrs,
			     Seg->UP$Source_Port,Seg->UP$Dest_Port,
			     UCount,Uptr,out_buff,out_len);

	// Send off the reply datagram
	RC = UDP_S}(Dest$Adrs,Src$Adrs,
		      Seg->UP$Dest_Port,Seg->UP$Source_Port,
		      out_buff,out_len);
	if (RC != SS$_NORMAL)
	    XLOG$FAO(LOG$UDP,"!%T UDP SNMP reply error, RC=!XL!/",0,RC);

	// Release the input datagram buffer
	if (delete)
	    MM$Seg_Free(Bufsize,Buf);

	RETURN	// Don't pass this buffer on to the upper layers
	};

// Try to match the input packet up with a UDPCB

    UDPCB = UDPCB_Find(Src$Adrs,Seg->UP$Source_Port,Seg->UP$Dest_Port);
    if (UDPCB == 0)
	{
	UDP_MIB->MIB$udpNoPorts = UDP_MIB->MIB$udpNoPorts + 1;
	if ($$LOGF(LOG$UDP))
	    QL$FAO("!%T No UDPCB found for segment !XL, SP=!SL, DP=!SL!/",
		   0,Seg,Seg->UP$Source_Port,Seg->UP$Dest_Port);
	}
    else
X2:	{

// Log that it was found

	if ($$LOGF(LOG$UDP))
	    QL$FAO("!%T UDPCB !XL found for UDP !XL, SP=!SL, DP=!SL!/",
		   0,Seg,UDPCB,Seg->UP$Source_Port,Seg->UP$Dest_Port);

// Make sure the UDPCB isn't aborted...

	if (UDPCB->UDPCB$Aborting)
	    {
	    XQL$FAO(LOG$UDP,"!%T UDP input !XL for aborted UDPCB !XL dropped!/",
		    0,Seg,UDPCB);
	    LEAVE X2;
	    };

// "Normal" UDPCB's stop being wildcarded when they receive something....

!	if (NOT UDPCB->UDPCB$ADDR_MODE)
!	    {

// If the connection was wildcarded, resolve hosts and ports now

!	    if (UDPCB->UDPCB$Wildcard)
	    if (0)
		{
		UDPCB->UDPCB$Wildcard = FALSE;
		SELECT TRUE OF
		SET
		[UDPCB->UDPCB$Foreign_Host == WILD]:
		    UDPCB->UDPCB$Foreign_Host = Src$Adrs;

		[UDPCB->UDPCB$Local_Host == WILD]:
		    UDPCB->UDPCB$Local_Host = Dest$Adrs;

		[UDPCB->UDPCB$Foreign_Port == WILD]:
		    UDPCB->UDPCB$Foreign_Port = Seg->UP$Source_Port;
		TES;
		};
!	    };		// (non ADDR_MODE case)

// Kluge. Overwrite the UDP/IP header in the buffer, since we don't need it.
!!!HACK!!!
	Aptr = Uptr - IPADR$UDP_ADDRESS_BLEN;
	Ucount = Ucount + IPADR$UDP_ADDRESS_BLEN;
	APTR->IPADR$SRC_PORT = Seg->UP$Source_Port;
	APTR->IPADR$DST_PORT = Seg->UP$Dest_Port;
	APTR->IPADR$SRC_HOST = Src$Adrs;
	APTR->IPADR$DST_HOST = Dest$Adrs;

// Give the segment to the user now.

	delete = Queue_User_UDP (UDPCB,Aptr,Ucount,Buf,Bufsize,0);
	}; // End of block X2

// If the packet hasn't been given to the user, delete it now

    if (delete)
	MM$Seg_Free(Bufsize,Buf);
    }

//SBTTL "ICMP input handler for UDP"
/*
    Handles ICMP messages received in response to our UDP packets.
    For "normal" UDP connections, ICMP messages generally cause connection
    aborts. For "address-mode" connections, ICMP messages are delivered to
    the user.
*/

FORWARD ROUTINE
 void    Deliver_UDP_Data,
 VOID    UDPCB_ABORT;

UDP$ICMP(ICMtype,ICMex,IPsrc,IPdest,UDPptr,UDPlen,
			buf,bufsize) : NOVALUE (void)
!ICMtype - ICMP packet type
!ICMex - extra data from ICMP packet (pointer for ICM_PPROBLEM)
!IPsrc - source address of offending packet
!IPdest - destination address of offending packet
!UDPptr - first 64-bits of data from offending packet
!UDPlen - calculated octet count of data
!Buf - address of network buffer
!Bufsize - size of network buffer

    {
    MAP
	struct UDPkt_Structure * UDPptr;
    signed long
	struct UDPCB_Structure * UDPCB,
	delete;
    LABEL
	X;

// Fix byte order of the UDP header

    if ($$LOGF(LOG$ICMP+LOG$UDP))
	QL$FAO("!%T UDP$ICMP UDPlen !SL Bufsize !SL!/",0,UDPlen,Bufsize);
    SwapBytes(UDPlen/2,UDPptr);

// Find the connection that this ICMP message is for
// Remember that the ICMP/IP message is what we originally sent, so the
// hosts and ports are reversed. UDPCB_FIND(FRN_HOST,FRN_PORT,LCL_PORT)

    delete = TRUE;
    UDPCB = UDPCB_Find(IPdest,UDPptr->UP$Dest_Port,UDPptr->UP$Source_Port);
    if (UDPCB == 0)
	{			// Bogus UDP/ICMP message
	if ($$LOGF(LOG$ICMP))
	    {
	    signed long
		DESC$STR_ALLOC(fhstr,20);

	    ASCII_DEC_BYTES(fhstr,4,IPdest,fhstr->DSC$W_LENGTH);
	    QL$FAO("!%T ICMP for unknown UDPCB, FH=!AS, FP=!XL, LP=!XL!/",
		    0,fhstr,UDPptr->UP$Dest_Port,UDPptr->UP$Source_Port);
	    };
	}
    else
X:	{			// Good UDP/ICMP message
	if ($$LOGF(LOG$ICMP+LOG$UDP))
	    QL$FAO("!%T ICMP type !SL for UDPCB !XL!/",0,ICMtype,UDPCB);

// Ignore the packet, if the connection is aborted

	if (UDPCB->UDPCB$Aborting)
	    {
	    XQL$FAO(LOG$UDP,"!%T ICMP message for aborted UDPCB !XL dropped!/",
		       0,UDPCB);
	    LEAVE X;
	    };

// ADDR_MODE UDPCB's get the ICMP message delivered to them, with IOSB flags set
// indicating that this is an ICMP message and the ICMP error code
!!!HACK!!// How will we send back the ICMP message?
!!!HACK!!// Are you sure???
	if (1)
	    {
	    signed long
		struct queue_blk_structure(QB_NR_Fields) * QB,
		struct IPADR$ADDRESS_BLOCK * Uptr,
		Ucount;
	    EXTERNAL ROUTINE
		MM$QBlk_Get;

// Allocate and setup the fields in the QB. ** N.B. We overwrite part of the IP
// header, so be careful if you change the size of the UDPUSER block **

	    QB = MM$QBLK_Get();
	    QB->NR$ICMP = TRUE;
	    QB->NR$ICM_TYPE = ICMtype;
	    QB->NR$ICM_CODE = 0;
	    QB->NR$ICM_EX = ICMex;
	    Uptr = UDPptr - IPADR$UDP_ADDRESS_BLEN;
	    Uptr->IPADR$SRC_HOST = IPDest;
	    Uptr->IPADR$DST_HOST = IPSrc;
	    Uptr->IPADR$SRC_PORT = UDPptr->UP$Dest_Port;
	    Uptr->IPADR$DST_PORT = UDPptr->UP$Source_Port;
	    Ucount = UDPlen + IPADR$UDP_ADDRESS_BLEN;
	    delete = Queue_User_UDP(UDPCB,Uptr,Ucount,Buf,Bufsize,QB);
	    }
	else
	    {

// Normal connections are generally aborted.

	    SELECTONE ICMtype OF
	    SET
	    [ICM_DUNREACH]:	// Destination Unreachable - abort connection
		{
		UDPCB_Abort(UDPCB,NET$_URC);
		if ($$LOGF(LOG$ICMP || LOG$UDP))
		    QL$FAO(
		       "!%T UDPCB !XL killed by ICMP Destination Unreachable!/",
		       0,UDPCB);
		};

	    [ICM_TEXCEED]:	// Time exceeded - abort
		{
		UDPCB_Abort(UDPCB,NET$_CTO);
		if ($$LOGF(LOG$ICMP || LOG$UDP))
		    QL$FAO("!%T UDPCB !XL killed by ICMP Time Exceeded!/",
			   0,UDPCB);
		};

	    [ICM_SQUENCH]:	// Source quench - currently unsupported
		{
		0;
		};

	    [ICM_REDIRECT]:	// Redirect - not supported in this module
		{
		0;
		};

	    [ICM_PPROBLEM]:	// Parameter problem - not yet supported
		{
		0;
		};
	    TES;
	    };
	};

// In any case, free up the buffer
// No, dont't clear the buffer.  What if ICMP needs it?
    if (delete)
	MM$Seg_Free(Bufsize,Buf);
    }

//SBTTL "Queue_User_UDP - Queue up UDP packet for delivery to user"
/*
    Called by UDP$Input at AST level when an input packet matches a
    user UDP "connection". Function of this routine is to either deliver
    the UDP packet to the user (if a user read request is available) or
    queue it for later deliver.
    Returns TRUE if the UDP packet has been fully disposed of (i.e. the
    caller may deallocate the packet), FALSE otherwise (i.e. the packet
    has been placed on a queue and may not be deallocated yet).
 )%

Queue_User_UDP(UDPCB,Uptr,Usize,Buf,Bufsize,QB)
    {
    MAP
	struct UDPCB_Structure * UDPCB,
	struct queue_blk_structure(QB_NR_Fields) * QB;
    signed long
	QBR;
    EXTERNAL ROUTINE
	MM$QBlk_Get;
    LITERAL
	UDPCB$NR_Qmax = 5;	// Max input packets permitted on input queue

// See if the input queue is full for this UDPCB

    if (UDPCB->UDPCB$NR_Qcount > UDPCB$NR_Qmax)
	{
	UDP_MIB->MIB$udpInErrors = UDP_MIB->MIB$udpInErrors + 1;
	if ($$LOGF(LOG$UDP))
	    QL$FAO("!%T UDP at !XL dropped - UDPCB NR queue full!/",0,Uptr);
	return TRUE;		// Drop the packet - no room
	};

// Allocate a queue block and insert onto user receive queue

    if (QB == 0)
	QB = MM$QBLK_Get();
    QB->NR$Buf_Size = Bufsize;	// Total size of network buffer
    QB->NR$Buf = Buf;		// Pointer to network buffer
    QB->NR$Ucount = Usize;	// Length of the data
    QB->NR$Uptr = Uptr;	// Pointer to the data

// If there is a user read outstanding, deliver data, else queue for later

    if (REMQUE(UDPCB->UDPCB$USR_Qhead,QBR) != Empty_Queue)
	Deliver_UDP_Data (UDPCB,QB,QBR)
    else
	INSQUE(QB,UDPCB->UDPCB$NR_Qtail);
    return FALSE;		// Don't deallocate this segment...
    }

//SBTTL "Deliver_UDP_Data - Deliver UDP data to user"
/*
    Perform actual delivery of UDP packet to a user request.
    UDP packet is copied into the user buffer and the user I/O request
    is posted.
 )%

Deliver_UDP_Data(UDPCB,QB,URQ) : NOVALUE (void)
    {
    MAP
	struct UDPCB_Structure * UDPCB,
	struct queue_blk_structure(QB_NR_Fields) * QB,
	struct queue_blk_structure(QB_UR_Fields) * URQ;
    signed long
	FLAGS,
	ICMTYPE,
	struct $BBLOCK * IRP[],
	PID,
	struct User_RECV_Args * UArgs,
	struct User_RECV_Args * Sargs,
	Aptr,
	Uptr,
	Ucount;

// Determine data start and data count

    Ucount = QB->NR$Ucount - IPADR$UDP_ADDRESS_BLEN;
    Uptr = QB->NR$Uptr + IPADR$UDP_ADDRESS_BLEN;
    Aptr = QB->NR$Uptr;

// Truncate to user receive request size

!!!HACK!!// You can't just drop data like that//  Or can you?
    if (Ucount > URQ->UR$Size)
	Ucount = URQ->UR$Size;

    if ($$LOGF(LOG$UDP))
	QL$FAO("!%T Posting UDP receive,Size=!SL,UDPCB=!XL,IRP=!XL,UDPCB_A=!XL PID=!XL!/",
	       0,Ucount,UDPCB,URQ->UR$IRP_Adrs,URQ->UR$UCB_Adrs,PID);

// Copy from our buffer to the user system buffer

    $$KCALL(MOVBYT,Ucount,Uptr,URQ->UR$Data);

// Copy UDP Source and destination addresses to system space Diag Buff

    UArgs = URQ->UR$UArgs;
    IRP = URQ->UR$IRP_Adrs;
    if (Uargs->RE$PH_Buff != 0)
	$$KCALL(MOVBYT,IPADR$UDP_ADDRESS_BLEN,
		.Aptr,Uargs->RE$PH_Buff);

// Post the I/O and free up memory

    ICMTYPE = 0;
    FLAGS = 0;
    if (QB->NR$ICMP)
	{
	ICMTYPE = QB->NR$ICM_TYPE;
	FLAGS = FLAGS || NSB$ICMPBIT;
	};
    User$Post_IO_Status(URQ->UR$Uargs,SS$_NORMAL,
			.Ucount,FLAGS,ICMTYPE);
    MM$UArg_Free(URQ->UR$Uargs);

    MM$QBLK_Free(URQ);
    MM$Seg_Free(QB->NR$Buf_Size,QB->NR$Buf);
    MM$QBLK_Free(QB);
    UDP_MIB->MIB$udpInDatagrams = UDP_MIB->MIB$udpInDatagrams + 1;
    }

//SBTTL "UDPCB_OK - Match connection ID to UDPCB address"

UDPCB_OK(Conn_ID,RCaddr,struct User_Default_Args * Uargs)
    {
    signed long
	struct UDPCB_Structure * UDPCB;
    MACRO
	UDPCBERR(EC) = (RCaddr = EC; return 0) %;

// Range check the connection id. This should never fail, since the user should
// not be fondling connection IDs.

    if ((Conn_ID <= 0) || (Conn_ID > MAX_UDPCB))
	UDPCBERR(NET$_CDE);	// Nonexistant connection ID
    UDPCB = UDPCB_Table[Conn_ID];

// Make sure the table had something reasonable for this connection ID

    if (UDPCB <= 0)
	UDPCBERR(NET$_CDE);	// UDPCB has been deleted (possible)

// Check consistancy of UDPCB back-pointer into table

    IF (UDPCB->UDPCB$UDPCBID != Conn_ID) OR
       (UDPCB->UDPCB$UCB_ADRS != Uargs->UD$UCB_Adrs) THEN
	UDPCBERR(NET$_CDE);	// Confusion (can this happen?)

// Everything is good - return the UDPCB address

    return UDPCB;
    }

//SBTTL "UDPCB_Get - Allocate and initialize one UDPCB"

UDPCB_Get(IDX,Src$Port)
    {
    EXTERNAL ROUTINE
	LIB$GET_VM	: ADDRESSING_MODE(GENERAL),
	LIB$GET_VM_PAGE	: ADDRESSING_MODE(GENERAL);
    signed long
	Ucount,
	struct UDPCB_Structure * UDPCB,
	UDPCBIDX,
	RC ;
    LABEL
	X;

// Check to make sure we haven't already allocated this local port

    Ucount = UDPCB_Count;
    UDPCBIDX = 1;
    while ((Ucount > 0) && (UDPCBIDX <= Max_UDPCB))
	{
	if ((UDPCB = UDPCB_Table[UDPCBIDX]) != 0)
	    {
	    if ((UDPCB->UDPCB$Local_Port == Src$Port))
		return 0;
	    Ucount = Ucount-1;
	    };
	UDPCBIDX = UDPCBIDX + 1;
	};

// Find a free slot in the UDPCB table

X:  {			// ** Block X **
    UDPCBIDX = 0;
    for (I=1;I<=MAX_UDPCB;I++)
	if (UDPCB_Table[I] == 0)
	    LEAVE X WITH (UDPCBIDX = I);
    return 0;			// Failed to allocate a UDPCB
    }			// ** Block X **

// Allocate some space for the UDPCB

//    LIB$GET_VM(%REF(UDPCB_Size*4),UDPCB);
    RC = LIB$GET_VM_PAGE(%REF(((UDPCB_Size * 4) / 512) + 1),UDPCB);
    if (NOT RC)
	FATAL$FAO("UDPCB_GET - LIB$GET_VM failure, RC=!XL",RC);

// Clear it out and set it in the table

    UDPCB_Table[UDPCBIDX] = UDPCB;
    CH$FILL(%CHAR(0),UDPCB_Size*4,UDPCB);
    UDPCB_Count = UDPCB_Count+1;

// Initialize queue headers for the UDPCB

    UDPCB->UDPCB$NR_Qhead = UDPCB->UDPCB$NR_Qtail = UDPCB->UDPCB$NR_Qhead;
    UDPCB->UDPCB$USR_Qhead = UDPCB->UDPCB$USR_Qtail = UDPCB->UDPCB$USR_Qhead;

// Set the connection ID

    UDPCB->UDPCB$UDPCBID = UDPCBIDX;

// Return the pointer

    IDX = UDPCBIDX;
    return UDPCB;
    }

//SBTTL "UDPCB_Free - Deallocate a UDPCB"

void UDPCB_Free(UDPCBIX,struct UDPCB_Structure * UDPCB) (void)
    {
    EXTERNAL ROUTINE
	LIB$FREE_VM		: ADDRESSING_MODE(GENERAL),
	LIB$FREE_VM_PAGE	: ADDRESSING_MODE(GENERAL);
    signed long
	RC ;

// Clear the table entry

    UDPCB_Table[UDPCBIX] = 0;

// Free the memory and decrement our counter.

//    LIB$FREE_VM(%REF(UDPCB_Size*4),UDPCB);
    RC = LIB$FREE_VM_PAGE(%REF(((UDPCB_Size * 4) / 512) + 1),UDPCB);
    if (NOT RC)
	FATAL$FAO("UDPCB_FREE - LIB$FREE_VM failure, RC=!XL",RC);
    UDPCB_Count = UDPCB_Count-1;
    }

//SBTTL "Kill_UDP_Requests - purge all I/O requests for a connection"

void Kill_UDP_Requests(struct UDPCB_Structure * UDPCB,RC) (void)
    {
    signed long
	struct queue_blk_structure(QB_UR_Fields) * URQ,
	struct queue_blk_structure(QB_NR_Fields) * QB;

// Make sure we aren't doing this more than once
!
//   if (UDPCB->UDPCB$Aborting)
!	RETURN;

// Say that this connection is aborting (prevent future requests)

    UDPCB->UDPCB$Aborting = TRUE;

// Cancel any name lookup in progess

    if (UDPCB->UDPCB$NMLOOK)
	{
	NML$CANCEL(UDPCB, 0, 0);
	UDPCB->UDPCB$NMLOOK = FALSE;
	};

// Kill any pending open

    NOINT;
    if (UDPCB->UDPCB$UARGS != 0)
	{
	USER$Err(UDPCB->UDPCB$UARGS,RC);
	UDPCB->UDPCB$UARGS = 0;
	};
    OKINT;

// Purge the user request queue, posting all requests

    while (REMQUE(UDPCB->UDPCB$USR_Qhead,URQ) != Empty_Queue)
	{
	    User$Post_IO_Status(URQ->UR$Uargs,RC,0,0,0);
	    MM$UArg_Free(URQ->UR$Uargs);
	    MM$QBlk_Free(URQ);	
	};

// Purge any received qblocks as well

    while (REMQUE(UDPCB->UDPCB$NR_Qhead,QB) != Empty_Queue)
	{
	MM$Seg_Free(QB->NR$Buf_Size,QB->NR$Buf);
	MM$QBlk_Free(QB);
	};
    }

//SBTTL "UDPCB_Close - Close/deallocate a UDPCB"

void UDPCB_Close(UIDX,struct UDPCB_Structure * UDPCB,RC) (void)
    {
    Kill_UDP_Requests(UDPCB,RC);
    UDPCB_FREE(UIDX,UDPCB);
    }

void UDPCB_Abort(struct UDPCB_Structure * UDPCB,RC) (void)
!
// Abort a UDPCB - called by ICMP code.
!
    {
    UDPCB_CLOSE(UDPCB->UDPCB$UDPCBID,UDPCB,RC)
    }



//SBTTL "Purge_All_UDP_IO - delete UDP database before network exits"

UDP$Purge_All_IO : NOVALUE (void)
    {
    signed long
	UDPCBIDX,
	struct UDPCB_Structure * UDPCB;

// Loop for all connections, purge them, and delete them.

    for (UDPCBIDX=1;UDPCBIDX<=MAX_UDPCB;UDPCBIDX++)
	if ((UDPCB = UDPCB_Table[UDPCBIDX]) != 0)
	    UDPCB_Close(UDPCBIDX,UDPCB,NET$_TE);
    }

//SBTTL "UDP_Conn_Unique - Check for unique UDP connection"
/*
    Verify that a given set of Local Port, Foreign Host, Foreign Port
    are not already in use by an existing UDP connection.
    Returns TRUE if the connection is unique, false otherwise.
 )%

UDP_Conn_Unique(LP,FH,FP)
    {
    signed long
	struct UDPCB_Structure * UDPCB,
	Ucount;

    Ucount = UDPCB_Count;
    for (I=1;I<=MAX_UDPCB;I++)
	if ((UDPCB = UDPCB_Table[I]) != 0)
	    {
	    IF (UDPCB->UDPCB$Foreign_Host == FH) AND
	       (UDPCB->UDPCB$Foreign_Port == FP) AND
	       (UDPCB->UDPCB$Local_Port == LP) THEN
		return FALSE;
	    if ((Ucount = Ucount-1) <= 0)
		return TRUE;
	    };
    return TRUE;
    }

//SBTTL "UDP$OPEN - open a UDP "connection""
/*
    Open a UDP "connection". Create a UDP Control Block, which serves as a
    place to hang incoming packets and user receive requests.
 )%

FORWARD ROUTINE
    UDP_COPEN_DONE,
 VOID    UDP_NMLOOK_DONE,
 VOID    UDP_ADLOOK_DONE;

void UDP$OPEN(struct User_Open_Args * Uargs) (void)
    {
    signed long
	struct IPADR$Address_Block * ProtoHdr,
	IPADDR,
	NAMLEN,
	NAMPTR : VECTOR[4,BYTE],
	UIDX,
	struct UDPCB_Structure * UDPCB,
	UDPCBPTR,
	Args : VECTOR[4];
    LABEL
	X;

    XLOG$FAO(LOG$USER,"!%T UDP$OPEN: PID=!XL,CHAN=!XL,FLAGS=!XL X1=!XL!/",
	     0,Uargs->OP$PID,Uargs->OP$PIOchan,Uargs->OP$FLAGS,
	     UArgs->OP$Ext1);

    ProtoHdr = Uargs->OP$ProtoHdrBlk;

// First create a UDPCB for this connection.

    if ((UDPCB = UDPCB_Get(UIDX,ProtoHdr->IPADR$SRC_PORT)) <= 0)
	{
	USER$Err(Uargs,NET$_UCT);
	RETURN;
	};

// Initialize user mode values

    UDPCB->UDPCB$UCB_ADRS = Uargs->OP$UCB_Adrs;
    UDPCB->UDPCB$User_ID = Uargs->OP$PID;
    UDPCB->UDPCB$PIOchan = Uargs->OP$PIOchan;

// At this point, the connection exists. Write the connection ID
// back into the Unit Control Block for this connection.

    UDPCBptr = Uargs->OP$UCB_Adrs + UCB$L_CBID;
    $$KCALL(MOVBYT,4,UIDX,UDPCBptr);

// Initialize queue headers for the UDPCB

    UDPCB->UDPCB$NR_Qhead = UDPCB->UDPCB$NR_Qtail = UDPCB->UDPCB$NR_Qhead;
    UDPCB->UDPCB$USR_Qhead = UDPCB->UDPCB$USR_Qtail = UDPCB->UDPCB$USR_Qhead;

// Copy user arguments to UDPCB

    if ((Uargs->OP$Mode == OP$MODE_UDPADDR))
	UDPCB->UDPCB$ADDR_MODE = TRUE;
    ProtoHdr = Uargs->OP$ProtoHdrBlk;
    UDPCB->UDPCB$Local_Port = ProtoHdr->IPADR$SRC_PORT;
    UDPCB->UDPCB$Foreign_Port = ProtoHdr->IPADR$DST_PORT;
    if (UDPCB->UDPCB$Foreign_Port == WILD)
	UDPCB->UDPCB$Wildcard = TRUE;

// Handle wildcard host

    NAMPTR = CH$PTR(Uargs->OP$Foreign_Host);
    NAMLEN = Uargs->OP$Foreign_Hlen;
    if ((NAMLEN == 0) && (NOT Uargs->OP$ADDR_FLAG))
	{
	UDPCB->UDPCB$Wildcard = TRUE;
	UDPCB->UDPCB$Foreign_Host = WILD;
	UDPCB->UDPCB$Foreign_Hnlen = 0;
	UDPCB->UDPCB$Local_Host = WILD;
	UDPCB->UDPCB$Uargs = Uargs;
	UDP_NMLOOK_DONE(UDPCB,SS$_NORMAL,0,0,0,0);
	RETURN;
	};

// Check for supplied IP address instead of name

X:  {			// *** Block X ***
    if (Uargs->OP$ADDR_FLAG)
	IPADDR = Uargs->OP$Foreign_Address
    else
	if (GET_IP_ADDR(NAMPTR,IPADDR) LSS 0)
	    LEAVE X;
    UDPCB->UDPCB$Foreign_Hnlen = 0;
    UDPCB->UDPCB$Uargs = Uargs;
    UDP_NMLOOK_DONE(UDPCB,SS$_NORMAL,1,IPADDR,0,0);
    UDPCB->UDPCB$NMLook = TRUE;
    NML$GETNAME(IPADDR,UDP_ADLOOK_DONE,UDPCB);
    RETURN;
    }			// *** Block X ***

// "standard" case, host name is supplied - start name lookup for it

    UDPCB->UDPCB$Uargs = Uargs;
    UDPCB->UDPCB$NMLook = TRUE;
    NML$GETALST(NAMPTR,NAMLEN,UDP_NMLOOK_DONE,UDPCB);
    }



//SBTTL "UDP_NMLOOK_DONE - Second phase of UDP$OPEN when namelookup done"
/*
    Come here when the foreign host name has been resolved.
    At this point, we set the local & foreign hosts/ports in the UDPCB
    and post the users open request.
*/

UDP_NMLOOK_DONE(UDPCB,STATUS,ADRCNT,ADRLST,NAMLEN,NAMPTR) : NOVALUE (void)
    {
    MAP
	struct UDPCB_Structure * UDPCB;
    signed long
	RC,
	struct User_Open_Args * Uargs,
	IOSB : NetIO_Status_Block;
    MACRO
	UOP_ERROR(EC) = 
	    {
	    USER$Err(Uargs,EC);
	    UDPCB_FREE(UDPCB->UDPCB$UDPCBID,UDPCB);
	    RETURN;
	    } %;

// Clear name lookup flag and get uargs

    NOINT;
    UDPCB->UDPCB$NMLook = FALSE;
    Uargs = UDPCB->UDPCB$Uargs;
    UDPCB->UDPCB$Uargs = 0;
    OKINT;

// Check status of the name lookup

    if (NOT STATUS)
	UOP_ERROR(STATUS);

// Finish up the common part of the open

    RC = UDP_COPEN_DONE(UDPCB,ADRCNT,ADRLST);
    if (NOT RC)
	UOP_ERROR(RC);

// Verify that we have access to the host/port set

    RC = USER$CHECK_ACCESS(UDPCB->UDPCB$USER_ID,UDPCB->UDPCB$Local_Host,
		      UDPCB->UDPCB$Local_Port,UDPCB->UDPCB$Foreign_Host,
		      UDPCB->UDPCB$Foreign_Port);
    if (NOT RC)
	UOP_ERROR(RC);

// Set the foreign host name in the UDPCB

    UDPCB->UDPCB$Foreign_Hnlen = NAMLEN;
    if (NAMLEN != 0)
	CH$MOVE(NAMLEN,NAMPTR,CH$PTR(UDPCB->UDPCB$Foreign_Hname));

// Finally, post the status

    IOSB->NSB$STATUS = SS$_NORMAL; // Success return
    IOSB->NSB$Byte_Count = 0;
    IOSB->NSB$XSTATUS = 0;
    IO$POST(IOSB,Uargs);
    MM$UArg_Free(Uargs);
    }

//SBTTL "UDP_COPEN_DONE - Common user UDP open done routine"

UDP_COPEN_DONE(UDPCB,ADRCNT,ADRLST)
    {
    signed long
	IP_Address ;
    MAP
	struct UDPCB_Structure * UDPCB;

// Set local and foreign host numbers according to our info

    if (ADRCNT > 0)
	IP$SET_HOSTS(ADRCNT,ADRLST,UDPCB->UDPCB$Local_Host,
		     UDPCB->UDPCB$Foreign_Host);

// Now, check that this connection is unique and get a local port, if needed.

    if (UDPCB->UDPCB$Local_Port != WILD)
	{
	IF NOT UDP_Conn_Unique(UDPCB->UDPCB$Local_Port,UDPCB->UDPCB$Foreign_Host,
			       UDPCB->UDPCB$Foreign_Port) THEN
	    return NET$_NUC;
	}
    else
	{
	LITERAL
	    Max_LP_Tries = 100;

// Try a bunch of times to find a unique local port...

	for (I=1;I<=Max_LP_Tries;I++)
	    {
	    signed long
		LP;
	    LP = USER$GET_LOCAL_PORT(UDP_User_LP);
	    IF UDP_Conn_Unique(LP,UDPCB->UDPCB$Foreign_Host,
			       UDPCB->UDPCB$Foreign_Port) THEN
		EXITLOOP (UDPCB->UDPCB$Local_Port = LP);
	    };

// If it failed, then no connections available - punt

	if (UDPCB->UDPCB$Local_Port == 0)
	    {
	    XLOG$FAO(LOG$USER,"!%T UDB_COPEN: Conn failed !/", 0);
	    ACT$FAO("!%D Open UDP Port failed !/", 0 );
	    return NET$_CSE;
	    } ;
	};

// Done at last - log success

    XLOG$FAO(LOG$USER,"!%T UDB_COPEN: Conn idx = !XL, UDPCB = !XL!/",
	     0,UDPCB->UDPCB$UDPCBID,UDPCB);
    IP_Address = UDPCB->UDPCB$Foreign_Host ;
    ACT$FAO("!%D Open UDP Port !UW (!UW) <!UB.!UB.!UB.!UB>!/",0,
	UDPCB->UDPCB$Local_Port,
	UDPCB->UDPCB$Foreign_Port,
	.IP_Address<0,8>,IP_Address<8,8>,
	.IP_Address<16,8>,IP_Address<24,8>
		   );
    return SS$_NORMAL;
    }

//SBTTL "UDP_ADLOOK_DONE - Finish UDP address to name lookup"

UDP_ADLOOK_DONE(UDPCB,STATUS,NAMLEN,NAMPTR) : NOVALUE (void)
    {
    MAP
	struct UDPCB_Structure * UDPCB;

// Clear pending name lookup flag

    UDPCB->UDPCB$NMLook = FALSE;

// Check status

    if (NOT STATUS)
	RETURN;

// Copy the hostname into the UDPCB

    UDPCB->UDPCB$Foreign_Hnlen = NAMLEN;
    CH$MOVE(NAMLEN,NAMPTR,CH$PTR(UDPCB->UDPCB$Foreign_Hname));
    }

//SBTTL "UDP$CLOSE - close UDP "connection""
/*
    Close a UDP "connection". Kills any receive requests that haven't
    finished yet and deallocates the UDPCB and any other data structures
    associated with a connection.
*/

void UDP$CLOSE(struct User_Close_Args * Uargs) (void)
    {
    signed long
	struct UDPCB_Structure * UDPCB,
	RC;

// Check for valid UDPCB

    if ((UDPCB = UDPCB_OK(Uargs->CL$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);
	RETURN;
	};

// Use common routine for closing

    UDPCB_Close(Uargs->CL$Local_Conn_ID,UDPCB,NET$_CC);

// Close done - post user request and free argblk

    User$Post_IO_Status(Uargs,SS$_NORMAL,0,0,0);
    MM$UArg_Free(Uargs);
    }

//SBTTL "UDP$ABORT - abort UDP "connection""
/*
    Abort a UDP "connection". Identical in functionality to UDP$CLOSE.
 )%

void UDP$ABORT(struct User_Abort_Args * Uargs) (void)
    {
    signed long
	struct UDPCB_Structure * UDPCB,
	RC;

// Check for valid UDPCB

    if ((UDPCB = UDPCB_OK(Uargs->AB$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);
	RETURN;
	};

// Use common routine for closing

    UDPCB_Close(Uargs->AB$Local_Conn_ID,UDPCB,NET$_CC);

// Done. Clean up.

    User$Post_IO_Status(Uargs,SS$_NORMAL,0,0,0);
    MM$UArg_Free(Uargs);
    }

//SBTTL "UDP$S} - send UDP packet"
/*
    Handle user send request for UDP connection. Form a UDP packet from the
    user's data buffer and hand it to IP layer for transmission.
 )%

void UDP$S}(struct User_Send_Args * Uargs) (void)
    {
    signed long
	RC,
	ForeignAddr,LocalAddr,
	ForeignPort,LocalPort,
	struct IPADR$ADDRESS_BLOCK * Uaddr,
	struct UDPCB_Structure * UDPCB;

// Validate connection ID and get UDPCB pointer

    if ((UDPCB = UDPCB_OK(Uargs->SE$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// No such connection
	RETURN;
	};
    XLOG$FAO(LOG$USER,"!%T UDP$S}: Conn=!XL, UDPCB=!XL, Size=!SL, X1=!XL, X2=!XL!/",
	     0,Uargs->SE$Local_Conn_ID,UDPCB,Uargs->SE$Buf_size,
	     Uargs->SE$EXT1,Uargs->SE$EXT2);

// Check for aborted connection

    if (UDPCB->UDPCB$Aborting)
	{
	XLOG$FAO(LOG$USER,"!%T UDP$S} for aborted UDPCB !XL!/",0,UDPCB);
	USER$Err(Uargs,NET$_CC);
	RETURN;
	};

// Check for invalid buffer size

    if (Uargs->SE$Buf_Size LSS 0)
	{
	USER$Err(Uargs,NET$_BTS);
	RETURN;
	};

    if (Uargs->SE$Buf_Size > Max_UDP_Data_Size)
	{
	USER$Err(Uargs,NET$_IR);
	RETURN;
	};

// Check for "address mode" connection and set host addresses from user buffer
// in that case.

    Uaddr = Uargs->SE$ProtoHdrBlk;

    ForeignAddr = UAddr->IPADR$DST_HOST;
    if (ForeignAddr == WILD)
	ForeignAddr = UDPCB->UDPCB$Foreign_Host;

    LocalAddr = UAddr->IPADR$SRC_HOST;
    if (LocalAddr == WILD)
    IP$SET_HOSTS(1,ForeignAddr,LocalAddr,ForeignAddr);

    LocalPort = UAddr->IPADR$SRC_PORT;
    if (LocalPort == WILD)
	LocalPort = UDPCB->UDPCB$Local_Port;

    ForeignPort = UAddr->IPADR$DST_PORT;
    if (ForeignPort == WILD)
	ForeignPort = UDPCB->UDPCB$Foreign_Port;

   if ((ForeignAddr == WILD) || (ForeignPort == WILD))
	{
	USER$Err(Uargs,NET$_NOPN);
	RETURN
	};

// Do common portion of the send

    RC = UDP_S}(LocalAddr, ForeignAddr, LocalPort, ForeignPort,
		  Uargs->SE$Data_Start, Uargs->SE$Buf_size );

// Post the I/O request back to the user

    User$Post_IO_Status(Uargs,RC,0,0,0);
    MM$UArg_Free(Uargs);
    }



//SBTTL "UDP_S} - Common routine for sending UDP datagrams"

UDP_S} ( LocalAddr, ForeignAddr, LocalPort, ForeignPort,
			  UData, USize )
!
// Returns success or failure of IP$S}.
!
    {
    signed long
	RC,
	Bufsize,
	Buf,
	struct UDPkt_Structure * Seg,
	Segsize;

// Allocate an output buffer and build an IP packet

    if (Usize > Max_UDP_Data_Size)
	Usize = Max_UDP_Data_Size;

// Use preallocated buffer sizes to reduce dynamic memory load

    bufsize = Usize + UDP_Header_Size + IP_hdr_byte_size + Device_header;
    if (bufsize <= MIN_PHYSICAL_BUFSIZE)
	bufsize = MIN_PHYSICAL_BUFSIZE
    else
	if (bufsize <= MAX_PHYSICAL_BUFSIZE)
	    bufsize = MAX_PHYSICAL_BUFSIZE;
    Buf = MM$Seg_Get(Bufsize);	// Get a buffer
    Seg = Buf + device_header + IP_hdr_byte_size; // Point at UDP segment
    Segsize = Usize+UDP_Header_Size; // Length of segment + UDP header

// Set up the UDP header

    Seg->UP$Source_Port = LocalPort;
    Seg->UP$Dest_Port = ForeignPort;
    Seg->UP$Length = Segsize;
    Seg->UP$Checksum = 0;

// Copy the user data into the data area

    $$KCALL(MOVBYT,Usize,UData,Seg->UP$Data);

// Log the UDP packet if desired

    if ($$LOGF(LOG$UDP))
	Log_UDP_Packet(Seg,FALSE,TRUE);

// Swap the header bytes and compute the checksum
// No longer compute the checksum as it is now done in IP (see header comments)

    SwapBytes(UDP_Header_Size/2,Seg);
//    Seg->UP$Checksum=Gen_Checksum(Segsize,Seg,LocalAddr,ForeignAddr,
!				  UDP_Protocol);

// Send the segment to IP (it will deallocate it)

    UDPIPID = UDPIPID+1;	// Increment packet ID
    RC = IP$S}(LocalAddr,ForeignAddr,UDPTOS,UDPTTL,
		   Seg,Segsize,UDPIPID,UDPDF,TRUE,UDP_Protocol,
		   Buf,Bufsize);

    UDP_MIB->MIB$udpOutDatagrams = UDP_MIB->MIB$udpOutDatagrams + 1;

    // Return an appropriate code.
    if (RC == 0)
	NET$_NRT
    else SS$_NORMAL
    }



//SBTTL "UDP$RECEIVE - receive a UDP packet"
/*
    Handle user receive request for UDP connection. If there is a packet
    available on the UDP receive queue, then deliver it to the user
    immediately. Otherwise, queue up the user receive for later.
 )%

void UDP$RECEIVE(struct User_Recv_Args * Uargs) (void)
    {
    signed long
	struct UDPCB_Structure * UDPCB,
	struct queue_blk_structure(QB_NR_Fields) * QB,
	struct queue_blk_structure(QB_UR_Fields) * URQ,
	RC;

// Validate connection ID and get UDPCB pointer

    if ((UDPCB = UDPCB_OK(Uargs->RE$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// No such connection
	RETURN;
	};
    XLOG$FAO(LOG$USER,"!%T UDP$RECEIVE: Conn=!XL, UDPCB=!XL, Size=!SL!/",
	     0,Uargs->RE$Local_Conn_ID,UDPCB,Uargs->RE$Buf_size);

// Check for aborted connection

    if (UDPCB->UDPCB$Aborting)
	{
	XLOG$FAO(LOG$USER,"!%T UDP$RECEIVE for aborted UDPCB !XL!/",0,UDPCB);
	USER$Err(Uargs,NET$_CC);
	RETURN;
	};

// Check for invalid buffer size

    if (Uargs->RE$Buf_Size <= 0)
	{
	USER$Err(Uargs,NET$_BTS);
	RETURN;
	};

    if (Uargs->RE$Buf_Size > Max_UDP_Data_Size)
	{
	USER$Err(Uargs,NET$_IR);
	RETURN;
	};

// Make a request block for the receive

    URQ = MM$QBLK_Get();		// Get a queue block
    URQ->UR$Size = Uargs->RE$Buf_size; // # of bytes this rq can take
    URQ->UR$Data = Uargs->RE$Data_Start; // Address of system buffer
    URQ->UR$IRP_Adrs = Uargs->RE$IRP_Adrs; // IO request packet address
    URQ->UR$UCB_Adrs = Uargs->RE$UCB_Adrs; // Unit Control Block address
    URQ->UR$Uargs = Uargs;	// User argument block address

// If anything is available on the queue, deliver it now, else queue for later

    NOINT;
    if (REMQUE(UDPCB->UDPCB$NR_Qhead,QB) != Empty_Queue)
	Deliver_UDP_Data(UDPCB,QB,URQ)
    else
	INSQUE(URQ,UDPCB->UDPCB$USR_Qtail);
    OKINT;
    }



//SBTTL "UDP$INFO - get info about UDP "connection""
/*
    Read the host names/numbers and the ports for a UDP connection.
 )%

void UDP$INFO(struct User_Info_Args * Uargs) (void)
    {
    EXTERNAL ROUTINE
	USER$Net_Connection_Info : NOVALUE;
    signed long
	struct UDPCB_Structure * UDPCB,
	RC;

// Validate the connection ID

    if ((UDPCB = UDPCB_OK(Uargs->IF$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Bad connection ID
	RETURN;
	};

// Give the information back (common TCP/UDP routine in USER.BLI)

    USER$Net_Connection_Info(Uargs,UDPCB->UDPCB$Local_Host,UDPCB->UDPCB$Foreign_Host,
			UDPCB->UDPCB$Local_Port,UDPCB->UDPCB$Foreign_Port,
			UDPCB->UDPCB$Foreign_Hname,UDPCB->UDPCB$Foreign_Hnlen);
    }


//SBTTL "UDP$STATUS - get status of UDP "connection""
/*
    This routine is a placeholder for the network STATUS command, which is
    currently implemented for the UDP protocol.
 )%

void UDP$STATUS(struct User_Status_Args * Uargs) (void)
    {
    USER$Err(Uargs,NET$_NYI);
    }

//SBTTL "UDP$CANCEL - Handle VMS cancel for UDP connection"
/*
    Handle process abort/$CANCEL request for a UDP connection. Identical
    in functionality to UDP$CLOSE/UDP$ABORT except for calling procedure.
 )%

UDP$CANCEL(struct VMS$Cancel_Args * Uargs)
    {
    signed long
	struct UDPCB_Structure * UDPCB,
	Fcount;

    Fcount = 0;

// Check all valid UDPCB's looking for a match on pid and channel #.

    for (I=1;I<=MAX_UDPCB;I++)
	if ((UDPCB = UDPCB_Table[I]) != 0)
	    {

// If the process doing the cancel owns this connection, then delete it.

	    IF (UDPCB->UDPCB$User_ID == Uargs->VC$PID) AND
	       (UDPCB->UDPCB$PIOchan == Uargs->VC$PIOchan) THEN
		{
		XLOG$FAO(LOG$USER,"!%T UDP$Cancel: UDPCB=!XL!/",0,UDPCB);
		UDPCB_Close(I,UDPCB,NET$_ccan);
		Fcount = Fcount + 1;
		};
	    };
    return Fcount;
    }

//SBTTL "UDP dump routines"

UDP$Connection_List(RB) : NOVALUE (void)
!
// Dump out the list of UDP connections.
!
    {
    MAP
	struct D$UDP_List_Return_Blk * RB;
    signed long
	RBIX;
    RBIX = 1;
    for (I=1;I<=MAX_UDPCB-1;I++)
	if (UDPCB_TABLE[I] != 0)
	    {
	    RB[RBIX] = I;
	    RBIX = RBIX + 1;
	    };
    RB[0] = RBIX - 1;
    }

UDP$UDPCB_DUMP(UDPCBIX,RB)
!
// Dump out a single UDP connection
!
    {
    MAP
	struct D$UDPCB_Dump_Return_BLK * RB;
    signed long
	struct UDPCB_Structure * UDPCB,
	QB,
	Qcount;

// Validate that there is a real UDPCB there

    IF (UDPCBIX LSS 1) || (UDPCBIX > MAX_UDPCB) OR
       ((UDPCB = UDPCB_TABLE[UDPCBIX]) == 0) THEN
	return FALSE;

// Copy the UDPCB contents

    RB->DU$UDPCB_Address = UDPCB;
    RB->DU$UDPCB_Foreign_Host = UDPCB->UDPCB$Foreign_Host;
    RB->DU$UDPCB_Foreign_Port = UDPCB->UDPCB$Foreign_Port;
    RB->DU$UDPCB_Local_Host = UDPCB->UDPCB$Local_Host;
    RB->DU$UDPCB_Local_Port = UDPCB->UDPCB$Local_Port;
    RB->DU$UDPCB_Flags = UDPCB->UDPCB$Flags;
    RB->DU$UDPCB_User_ID = UDPCB->UDPCB$User_ID;

// Get length of network receive queue

    QB = UDPCB->UDPCB$NR_Qhead;
    Qcount = 0;
    while ((QB NEQA UDPCB->UDPCB$NR_Qhead))
	{
	MAP
	    struct queue_blk_structure(QB_NR_Fields) * QB;
	Qcount = Qcount + 1;
	QB = QB->NR$NEXT;
	};
    RB->DU$UDPCB_NR_Qcount = Qcount;

// Get length of user receive queue

    QB = UDPCB->UDPCB$USR_Qhead;
    Qcount = 0;
    while ((QB NEQA UDPCB->UDPCB$USR_Qhead))
	{
	MAP
	    struct queue_blk_structure(QB_UR_Fields) * QB;
	Qcount = Qcount + 1;
	QB = QB->UR$NEXT;
	};
    RB->DU$UDPCB_UR_Qcount = Qcount;

// Done.

    return TRUE;
    }

}
ELUDOM
