#ifndef structure_h
#define structure_h

#if 0
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
%TITLE 'Define TCP-System wide Data Structures'
/*

Module:

	STRUCTURE

Facility:

	Provide system-wide data structure definitions.  This file
	generates a BLISS LIBRARY file "STRUCTURE.Lxx".  The structure
	library file is used by all other modules as the data structure
	definition file.  This method was adopted in order that all of
	the dynamic data-structures definitions could be located quickly.
	The only other place you will find data declarations (primarily
	global) is in the TCP.BLI module.

Author:

	Original author Stan C. Smith  Fall 1981
	This version by Vince Fuller, CMU-CSD, Spring/Summer, 1986
	Copyright (c) 1986, 1987, Vince Fuller and Carnegie-Mellon University

Modification History:

*** Begin CMU change log ***

	28-Feb-1990	Bruce R. Miller		CMU Network Development
	Removed much, esp. TCP/IP protocol definitions.

	02-Feb-1990	Bruce R. Miller		CMU Network Development
	Removed structure definition for the device configuration
	table.  It now resides in device_inof.req.

	14-Jun-1989	Bruce R. Miller		CMU Network Development
	Added field definitions for RR (resource record) lookup
	subfunction of the GTHST user function.

	24-OCT-1088	Dale Moore	CMu-CS/RI
	Changed Device name from "Pointer to a quadword" to
	"quadword".

5.6  20-Jul-87, Edit by VAF
	Clean up definitions of circular byte queues.

5.5  23-Mar-87, Edit by VAF
	Flush most of the constants describing buffer sizes as they are now
	mostly dynamic.
	Flush things from the NSQ block that don't belong there.

5.4   3-Mar-87, Edit by VAF
	Flush the I/O request "tags". Requires parallel change to IPDRIVER
	and to "VMS.REQ".

5.3   2-Mar-87, Edit by VAF
	Add definitions for UDP open modes (ADDR + DATA)
	Reorganize GTHST block definitions a little.

5.2  19-Feb-87, Edit by VAF
	Add hostname string length for OPEN and GTHST functions. Set by
	IPDRIVER versions 6.2 and later (previous versions incompatable).

5.1  23-Jan-87, Edit by VAF
	Clean up user request block definitions. General cosmetic changes.
	Add UARGSIZE field to UARG block for use by MACLIB. Set by device
	version 2.8 and later (previous versions incompatable).

5.0  12-Nov-86, Edit by VAF
	More changes as outlined in TCPMOD.REQ. Among other things, a lot of
	structures have been removed from here since they don't need to be
	global any more.

3.0   4-Aug-86, Edit by VAF
	Many, many changes to support changes outlined in TCP.MOD.

2.0  20-Feb-86, Edit by VAF
	Flush "known hosts".
	Add IP address and netmask fields to device configuration entries.
	Add IP gateway table (not fully implemented yet).
	Flush NSQ$ROUTE - consequence of "known hosts".
	Add NSQ$IP_Dest - IP address of immediate destination.
	Add MAX_HNAME - max length of hostname string

*** End CMU change log ***

[Sept 1981] 1.1
	Orginal definitions.
[Jan 1982] 1.2
	net$dump user return blk definitions
[Apr 1983] 1.3
	segment options field & tcb max-seg-size field
[Sept 1983] 1.4
	host alias table.
*/

LIBRARY 'CMUIP_SRC:[CENTRAL]NETXPORT';	// Get the transportablity library
LIBRARY 'CMUIP_SRC:[central]netcommon';// System-wide definitions

// Define some standard literals

LITERAL
#endif
// Misc Time Values in Hundredths seconds.

#define    Timer_Delta  -10*1000*1000	// conversion factor to VMS delta time.
#define    Csec  100			// Convert seconds to hundredths.
#define    Minute  60*Csec		// Convert minutes to .01 sec
#define    DaySec  24*60*60*Csec	// One day
#define    Csec_Timer_Delta  -100*1000 // Delta time format converter.

// Local Port ranges

#define    Well_Known_LP_Start	 1
#define    Well_Known_LP_End	 255
#define    Common_LP_End	 100
#define    User_LP_Start	 1024	// missing range for bsd4.2 unix.
#define    User_LP_End		 32767
#define    AOUS_LP_Start	 32768// Active Open User specified Local Port
#define    AOUS_LP_End		 65535
#define    LP_Start		 1
#define    LP_End		 65535

// Misc.

#define    Error  -1			// Return value, indicates error............
#define    Not_Found  -1
#define    OK  0			// Valid TCB pointer: RTN, TCB_OK.
#define    Wild  0			// Indicates a wild-card in Foreign_Socket pair.
#if 0
    Empty_Queue = 3: UNSIGNED(8), // REMQUE: unsuccessful removal, queue empty.
    Queue_Empty_Now = 2: UNSIGNED(8), // REMQUE: OK removal & queue now empty.
    First_Queue_Elem = 1: UNSIGNED(8); // INSQUE return - inserted 1st element.

COMPILETIME
    tblsize = 0;

%SBTTL 'Define circular byte queue header'
/*
    Circular byte queues are used to keep track of data as it flows between
    the user and the network.
*/

MACRO
#endif
//	%NAME(N,'QUEUE')= [$Bytes(0)],	// First address of structure
#define    CQF$DEF(N) \
	void * N##base;	/* Base address of queue*/\
	void * N##end;	/* Address of end of queue*/\
	unsigned short int N##size;	/* Size of queue*/\
	unsigned short int N##count;	/* Count of items on queue*/\
	void * N##enqp;	/* Pointer to last byte on queue*/\
	void * N##deqp;	/* Pointer to first byte on queue*/
#if 0

$FIELD CQ_FIELDS =
    SET
    CQF$DEF('CQ$')		// Define the queue fields
    TES;
LITERAL
    CQ_SIZE = $Field_Set_Size;
MACRO
    CQ$BLOCK = BLOCK[CQ_SIZE] FIELD(CQ_FIELDS) %;


%SBTTL 'Queue Element Block Definitions.'
/*
 Queue blks are used as queue elements which in turn contain bliss fullwords
used for other purposes.  Case: Send Queue consists of a queue of send Queue
Blks.  Each queue block then contains information & pointers to the user's
send data eg.

		//===============================================//
		+		Queue Forward Link		+
		//-----------------------------------------------//
		+		Queue Backwards link		+
		//-----------------------------------------------//
		+		User data size in bytes		+
		//-----------------------------------------------//
		+		User data pointer		+
		//-----------------------------------------------//
		+		Data Time Out in Secs.		+
		//===============================================//

*/

// Queue Block (Send queue) Fields

$FIELD	QB_Send_Fields=
    SET
    SN$Next		= [$Address],		// Forward Queue Link (Flink).
    SN$Last		= [$Address],		// Backwards queue link (Blink).
    SN$Size		= [$Long_Integer],	// # of bytes to send.
    SN$Data 		= [$Address],		// Start of data buffer.
    SN$Uargs		= [$Address],		// Address of user arg blk.
    SN$TimeOut		= [$Short_Integer],	// connection timeout (time).
    SN$Flags		= [$Byte],
    $OverLay(SN$Flags)
	SN$EOL		= [$Bit],		// End Of Letter Boolean.
	SN$URG		= [$Bit],		// Urgent data boolean.
	SN$USER		= [$Bit]		// On if seg is in user space
    $CONTINUE
    TES;

LITERAL
    QB_SN_SIZE = $Field_set_size;

// Queue Block (ReTransmission Queue Fields).

$FIELD QB_RT_Fields=
    SET
    RT$Next		= [$Address],		// FLink.
    RT$Last		= [$Address],		// Blink.
    RT$Buf		= [$Address],		// segment buffer start adrs
    RT$BufSize		= [$short_integer],	// byte size of buffer.
    RT$SegSize		= [$short_Integer],	// Byte size of segment.
    RT$Seg		= [$Address],		// Start address of segment.
    RT$Dest_Adrs	= [$Bytes(4)],		// Destination network adrs.
    RT$TimeOut		= [$Bytes(4)],		// Retransmission timeout
    RT$Maxtimeout	= [$bytes(4)],		// expiration time on RX queue
    RT$Start_Xmit_Time	= [$bytes(4)],		// base for round trip calc.
    RT$Xmit_Count	= [$bytes(4)],		// # of transmissions.
    RT$Seq_End		= [$Bytes(4)]		// Last sequence # in segment
    TES;

LITERAL
    QB_RT_SIZE = $Field_set_size;

// Network Receive segment queue.

$FIELD QB_NR_Fields=
    SET
    NR$NEXT		= [$Address],		// FLINK.
    NR$LAST		= [$Address],		// BLink.
    NR$Buf_Size		= [$Long_Integer],	// byte size of read buffer.
    NR$Buf		= [$Address],		// Read buffer start.
    NR$Size		= [$Long_Integer],	// Byte size of TCP segment.
    NR$Seg		= [$Address],		// start of TCP segment.
    NR$Data_Size	= [$Long_Integer],	// # of data bytes available.
    NR$Uptr		= [$Address],		// pointer to new data
    NR$Ucount		= [$Long_Integer],	// count of new data
    NR$TimeOut		= [$Long_Integer],
    NR$SRC_ADRS		= [$Long_Integer],	// Internet address.
    NR$Dest_Adrs	= [$Long_Integer],	// Internet address.
    NR$Src_Port		= [$Bytes(2)],		// SYN wait list: Source Port
    NR$Dest_Port	= [$Bytes(2)],		// SYN wait list: Dest port.
    NR$SEQ_Start	= [$Bytes(4)],		// First usable seqence #
    NR$SEQ_End		= [$Bytes(4)],		// Segment's last sequence #.
    NR$SEQ_Count	= [$Bytes(4)],		// Count of usable sequence #s
    $OVERLAY(NR$SEQ_Start)			// Reuse for ICMP messages
	NR$ICM_TYPE	= [$Byte],		// ICMP msg type
	NR$ICM_CODE	= [$Byte],		// ICMP code field
	NR$ICM_EX	= [$Bytes(2)],		// ICMP extra data
    $CONTINUE
    $OVERLAY(NR$SEQ_Start)
	NR$FragPtr	= [$Address],		// IP fragment flag/pointer
    $CONTINUE
    NR$Flags		= [$Byte],
    $OVERLAY(NR$Flags)
	NR$EOL		= [$Bit],		// End Of Letter boolean
	NR$URG		= [$Bit],		// Urgent data boolean.
	NR$ICMP		= [$Bit]		// Really an ICMP message
    $CONTINUE
    TES;

LITERAL
    QB_NR_SIZE = $Field_set_size;

// User Receive Data Request queue

$FIELD QB_UR_Fields=
    SET
    UR$NEXT		= [$Address],		// FLINK.
    UR$LAST		= [$Address],		// Blink
    UR$Size		= [$Long_Integer],	// # of bytes requested by user.
    UR$Data		= [$Address],		// Start of data buffer.
    UR$IRP_Adrs		= [$Address],		// IO request adrs.
    UR$UCB_Adrs		= [$Address],		// Unit Control Blk adrs.
    UR$Uargs		= [$Address],		// Address of user arg blk.
    $OVERLAY(UR$IRP_Adrs)
	UR$ASTADR	= [$Address],		// AST address for internal conn
	UR$ASTPRM	= [$Address]		// AST param for internal conn
    $CONTINUE
    TES;

LITERAL
    QB_UR_SIZE = $Field_set_size;

LITERAL QB_Size = 0;
$MAXLIT(QB_Size,QB_SN_SIZE,QB_RT_SIZE,QB_NR_SIZE,QB_UR_SIZE);

Macro	Queue_Blk_Structure(F) = Block[QB_Size] Field(F)%;
%MESSAGE(%NUMBER(qb_size),' Longwords allocated per Queue Block')

%SBTTL 'Network Segment Received Queue Definition.'
/*

Segment receive queue consists of Queue Blks which are structured according
to the QB_NR_Field description.  IP places segments on this queue after IP
has verified the datagram.
*/

$FIELD SI_Fields=
    SET
    SI_QHead	= [$Address],
    SI_QTail	= [$Address]
    TES;

LITERAL
    SI_Qheader_Size = $Field_Set_Size;


%SBTTL '"SYN" Wait Queue Definition.'

// "SYN" wait queue. List of "SYN" segments received waiting for a server
// process to do a "PASSIVE" OPEN.  Queue elements (blocks) are structured
// according to the QB_NR_Fields.

$FIELD SW_Fields=
    SET
    SW_QHead	= [$Address],
    SW_QTail	= [$Address]
    TES;


%SBTTL 'User (TCP) I/O request Argument Block definitions.'
/*

User Network I/O requests are fed to TCP via an ACP queue.  The queue
elements are VMS I/O request packets (IRP's) which point to a TCP argument
blocks.  These argument block are formatted according to the following field
definitions.


User default argument block structure.
user argument blocks have at least these arguments.

//===============================================================//
+			Data Start Address			+
//---------------------------------------------------------------//
+			Users Buffer Address			+
//---------------------------------------------------------------//
+		     VMS Dynamic Memory Block ID		+
//---------------------------------------------------------------//
+		     I/O Request Packet Address			+
//---------------------------------------------------------------//
+		     Unit Control Block Address			+
//---------------------------------------------------------------//
+			Process ID				+
//---------------------------------------------------------------//
+						| TCP Function	+
//===============================================================//

*/

MACRO Static_Fields(NM) =
    %NAME(NM,$Data_Start)= [$Address],	// Start of data within this block.
    %NAME(NM,$UBuf_Adrs)= [$Address],	// User buffer address.
    %NAME(NM,$VMS_BLK_ID)= [$Bytes(4)],	// VMS system dynamic block ID.
    %NAME(NM,$IRP_Adrs)	= [$Address],	// I/O Request Packet Address.
    %NAME(NM,$UCB_Adrs)	= [$Address],	// Unit Control Block Address.
    %NAME(NM,$PID)	= [$Address],	// Owning process ID.
    %NAME(NM,$UARGSIZE)	= [$UWord],	// Size of this UARG block
    %NAME(NM,$Funct)	= [$Bytes(1)],	// ACP Function code.
    %NAME(NM,$Protocol)	= [$Bytes(1)]	// ACP Protocol code.
    %;

$FIELD User_Def_Fields=
    SET
    Static_Fields(UD)			// Only the standard fields
    TES;

LITERAL
    UD_Size = $Field_Set_Size;
MACRO
    User_Default_Args = BLOCK[UD_Size] Field(User_Def_Fields)%;


// User Call: OPEN

$FIELD U$OPEN_Fields=
    SET
    Static_Fields(OP),			// Define the standard fields
    OP$ProtoHdrBlk	= [$Bytes(0)],
    OP$Src_Host		= [$ULong],
    OP$Dst_Host		= [$ULong],
    OP$Ext1		= [$ULong],	// Protocol header extension info
    OP$Ext2		= [$ULong],	// Protocol header extension info
    OP$FLAGS		= [$UWord],	// Open flags
    $OVERLAY(OP$FLAGS)
	OP$Mode		= [$Bit],	// Open mode, 1=Active, 0=Passive
					// UDP: 1=ADDRMODE 
	OP$NoWait	= [$Bit],	// 1=No wait, 0=wait
	OP$Addr_Flag	= [$Bit],	// 1=Address, 0=String (Foreign_Host)
    $CONTINUE	
    $OVERLAY(OP$MODE)
	OP$Active_Open	= [$Bit],	// Alias for the above
    $CONTINUE
    OP$TimeOut		= [$SWord],	// Inactivity timeout in seconds.
    OP$PIOchan		= [$SWord],	// user's IO channel
    OP$Foreign_Hlen	= [$Uword], 	// Length of foreign host name
    OP$Foreign_Host	= [$String(MAX_HNAME)], // ASCIZ FH name
    $OVERLAY(OP$Foreign_Host)
      OP$Foreign_Address= [$Bytes(4)]	// FH address if OP$ADDR_FLAG set
    $CONTINUE
    TES;

LITERAL
    Open_Arg_Length = $Field_Set_Size,
    Max_User_ArgBlk_Size = Open_Arg_Length;	// used by Uarg rtns (MEMGR.BLI)
MACRO
    User_Open_Args = Block[Open_Arg_Length] Field(U$OPEN_Fields)%;

LITERAL
    OP$MODE_PASSIVE = 0,	// OP$MODE value for TCP passive open
    OP$MODE_ACTIVE = 1,		// OP$MODE value for TCP active open (default)
    OP$MODE_UDPDATA = 1,	// OP$MODE value for UDP data open (default)
    OP$MODE_UDPADDR = 0;	// OP$MODE value for UDP address open


// User Call: SEND

$FIELD U$Send_Fields=
    SET
    Static_Fields(SE),			// Define the standard fields
    SE$ProtoHdrBlk	= [$Bytes(0)],
    SE$Src_Host		= [$ULong],
    SE$Dst_Host		= [$ULong],
    SE$Ext1		= [$ULong],	// Protocol header extension info
    SE$Ext2		= [$ULong],	// Protocol header extension info
    SE$Flags		= [$UWord],
    $OVERLAY(SE$Flags)
	SE$OPM		= [$Bit],	// Used in "OPEN" processing.
	SE$EOL		= [$Bit],	// End Of Letter flag.
	SE$URG		= [$Bit],	// Urgent data sent.
    $CONTINUE
    SE$TimeOut		= [$SWord],	// Send timeout (obsolete)
    SE$Local_Conn_ID	= [$Address],	// Connection ID
    SE$Buf_Size		= [$SWord],	// Send buffer size
    SE$Data		= [$Byte]	// User data start
    TES;

LITERAL
    Send_Arg_Length = $Field_Set_Size;
MACRO
    User_Send_Args = Block[Send_Arg_Length] Field(U$Send_Fields)%;


// User Call: RECIEVE

$FIELD U$RECV_Fields=
    SET
    Static_Fields(RE),			// Define the standard fields
    RE$ProtoHdrBlk	= [$Bytes(0)],
    RE$Src_Host		= [$ULong],	// Source Host
    RE$Dst_Host		= [$ULong],	// Destination Host
    RE$Ext1		= [$ULong],	// Protocol header extension info
    RE$Ext2		= [$ULong],	// Protocol header extension info
    RE$Flags		= [$UWord],	// No flags defined
    RE$TimeOut		= [$SWord],	// Obsolete, unused
    RE$Local_Conn_ID	= [$Address],
    RE$Alt_IO		= [$Address],	// Alternate IO entry point
    RE$PH_Buff		= [$Address],	// P0 Proto hdr buff pntr
    RE$Buf_Size		= [$SWord],
    RE$Data		= [$Byte]
    TES;

LITERAL
    RECV_Arg_Length = $Field_Set_Size;
MACRO
    User_RECV_Args = Block[Recv_Arg_Length] Field(U$RECV_Fields)%;


// User Call: CLOSE

$FIELD U$Close_Fields=
    SET
    Static_Fields(CL),			// Define the standard fields
    CL$Flags		= [$UWord],
    $OVERLAY(CL$FLAGS)
	CL$Abort	= [$Bit],	// 1=Abort
	CL$NoWait	= [$Bit],	// 1=No wait, 0=wait
    $CONTINUE	
    CL$TimeOut		= [$SWord],	// Not used.
    CL$Local_Conn_ID	= [$Address]	// Connection ID
    TES;

LITERAL
    Close_Arg_Length = $Field_Set_Size;
MACRO
    User_Close_Args = Block[Close_Arg_Length] Field(U$Close_Fields)%;


// User Call: STATUS

$FIELD U$STATUS_Fields=
    SET
    Static_Fields(ST),			// Define the standard fields
    ST$Flags		= [$UWord],	// Not used.
    ST$TimeOut		= [$SWord],	// Not used.
    ST$Local_Conn_ID	= [$Address],	// Connection ID
    ST$Buf_Size		= [$SWord]
    TES;

LITERAL
    Status_Arg_Length = $Field_Set_Size;
MACRO
    User_Status_Args = Block[Status_Arg_Length] Field(U$Status_Fields)%;

// User Status call status-return blk

$FIELD Status_Return_Fields=
    SET
    SR$Bytes_Avail	= [$Bytes(2)],
    SR$State		= [$Byte],
    SR$Last_State	= [$Byte],
    SR$Send_Window	= [$Long_Integer],
    SR$Recv_Window	= [$Long_Integer],
    SR$User_ID		= [$long_Integer]
    TES;

LITERAL
    SR_BLK_Size = $Field_Set_Size;
MACRO
    Status_return_Arg_Blk = BLOCK [SR_BLK_SIZE] Field(Status_Return_Fields)%;


// User Call: Net$INFO

$FIELD U$INFO_Fields=
    SET
    Static_Fields(IF),			// Define the standard fields
    IF$Flags		= [$UWord],	// Not used.
    IF$TimeOut		= [$SWord],	// Not used.
    IF$Local_Conn_ID	= [$Address],	// Connection ID
    IF$Buf_Size		= [$SWord]	// Size of returned data buffer
    TES;

LITERAL
    Info_Arg_Length = $Field_Set_Size;
MACRO
    User_Info_Args = Block[Info_Arg_Length] Field(U$Info_Fields)%;

// User Call: ABORT

$FIELD	U$ABORT_Fields=
    SET
    Static_Fields(AB),			// Define the standard fields
    AB$Flags		= [$UWord],	// None defined
    AB$TimeOut		= [$SWord],	// Not used.
    AB$Local_Conn_ID	= [$address]
    TES;

LITERAL
    Abort_Arg_Length = $Field_Set_Size;
MACRO
    User_Abort_Args = Block[Abort_Arg_Length] Field(U$Abort_Fields)%;


// User call: GTHST

MACRO
    GTHST_Static(NM) =
	%NAME(NM,$FLAGS)	= [$UWord],	// Function flags
	%NAME(NM,$SUBFUNCT)	= [$SWord],	// GTHST subfunction
	%NAME(NM,$BUFSIZE)	= [$SWord],	// User buffer size
	%NAME(NM,$ARG1)		= [$Uword],	// Size of argument
	%NAME(NM,$ARG2SIZE)	= [$Uword]	// Size of argument
    %;


$FIELD U$GTHST_FIELDS =
    SET
    Static_Fields(GH),			// Define the standard fields
    GTHST_Static(GH)			// Define the standard fields
    TES;

LITERAL
    GTHST_ARG_LENGTH = $FIELD_SET_SIZE;
MACRO
    GTHST_ARGS = BLOCK[GTHST_ARG_LENGTH] FIELD(U$GTHST_FIELDS) %;


// Overlays for the various GTHST subfunctions

// Name to address function

$FIELD GTHST_NMLOOK_Fields =
    SET
    Static_Fields(GHN),			// Define the standard fields
    GTHST_Static(GHN),			// and the standard GTHST fields
    $OVERLAY(GHN$ARG2SIZE)
	GHN$HSTLEN	= [$Uword],
    $CONTINUE
    GHN$HSTNAM		= [$STRING(HOST_NAME_MAX_SIZE)]
    TES;

LITERAL
    GTHST_NMLOOK_ARGS_LENGTH = $FIELD_SET_SIZE;
MACRO
    GTHST_NMLOOK_ARGS =	BLOCK[GTHST_NMLOOK_ARGS_LENGTH]
			FIELD(GTHST_NMLOOK_FIELDS)%;

// Address to name subfunction

$FIELD GTHST_ADLOOK_Fields =
    SET
    Static_Fields(GHA),			// Define the standard fields
    GTHST_Static(GHA),			// and the standard GTHST fields
    GHA$IPADDR		= [$Bytes(4)]
    TES;

LITERAL
    GTHST_ADLOOK_ARGS_LENGTH = $FIELD_SET_SIZE;
MACRO
    GTHST_ADLOOK_ARGS = BLOCK[GTHST_ADLOOK_ARGS_LENGTH]
			FIELD(GTHST_ADLOOK_FIELDS)%;

// Name to RR function

$FIELD GTHST_RRLOOK_Fields =
    SET
    Static_Fields(GRR),			// Define the standard fields
    GTHST_Static(GRR),			// and the standard GTHST fields
    $OVERLAY(GRR$ARG1)
	GRR$RRTYPE	= [$Uword],
    $CONTINUE
    $OVERLAY(GRR$ARG2SIZE)
	GRR$HSTLEN	= [$Uword],
    $CONTINUE
    GRR$HSTNAM		= [$STRING(HOST_NAME_MAX_SIZE)]
    TES;

LITERAL
    GTHST_RRLOOK_ARGS_LENGTH = $FIELD_SET_SIZE;
MACRO
    GTHST_RRLOOK_ARGS =	BLOCK[GTHST_RRLOOK_ARGS_LENGTH]
			FIELD(GTHST_RRLOOK_FIELDS)%;

// Maintenance Call : DUMP

$FIELD M$DUMP_Fields=
    SET
    Static_Fields(DU),			// Define the standard fields
    DU$Dump_Directive	= [$UWord],	// Dump function code.
    DU$TimeOut		= [$SWord],	// Not used.
    DU$ARG0		= [$ULong],	// Argument #0
    $OVERLAY(DU$ARG0)
	DU$Local_Conn_ID	= [$Address],	// TCB/UDPCB index, DU$TCB_DUMP
    $CONTINUE
    $OVERLAY(DU$ARG0)
	DU$Device_idx	= [$Byte],	// device index, DU$DEVICE_DUMP
    $CONTINUE
    $OVERLAY(DU$ARG0)
	DU$Start_Index	= [$SWord],	// ARP cache index, DU$ARP_CACHE
    $CONTINUE
    DU$ARG1		= [$ULong],	// Argument #1
    DU$ARG2		= [$ULong],	// Argument #2
    DU$Buf_Size		= [$SWord],	// Size of returned data block
    DU$Data		= [$Byte]	// Start of return data buffer.
    TES;

LITERAL
    Dump_Arg_Length = $Field_Set_Size;
MACRO
    Debug_Dump_Args = Block[Dump_Arg_Length] Field(M$Dump_Fields)%;


	
// Maintenance Call : SNMP

$FIELD M$SNMP_Fields=
    SET
    Static_Fields(SNMP),			// Define the standard fields
    SNMP$Function	= [$UWord],	// SNMP sub-function code.
    SNMP$Misc		= [$SWord],	// Size of returned data block
    SNMP$WBuf_Size	= [$SWord],	// Size of VAR data block
    SNMP$RBuf_Size	= [$SWord],	// Size of returned data block
    SNMP$Data		= [$Byte]	// Start of UArg data buffer.
    TES;

LITERAL
    SNMP_Arg_Length = $Field_Set_Size;
MACRO
    SNMP_Args = Block[SNMP_Arg_Length] Field(M$SNMP_Fields)%;


	
// Maintenance Call: EXIT
 
$FIELD M$EXIT_Fields=
    SET
    Static_Fields(EX),			// Define the standard fields
    EX$Flags		= [$UWord],	// Not used.
    EX$TimeOut		= [$SWord],	// Not used.
    EX$Local_Conn_ID	= [$Address]	// Not used.
    TES;

LITERAL
    Exit_arg_Length = $Field_Set_Size;
MACRO
    Debug_Exit_Args = Block[Exit_Arg_Length] Field(M$Exit_Fields)%;


// Maintenance Call: PANIC

$FIELD M$PANIC_Fields=
    SET
    Static_Fields(PA),			// Define the standard fields
    PA$Flags		= [$UWord],	// Not Used.
    PA$TimeOut		= [$SWord],	// Not used.
    PA$Local_Conn_ID	= [$Address]	// Not used.
    TES;

LITERAL
    Panic_arg_Length = $Field_Set_Size;
MACRO
    Debug_Panic_Args = Block[Panic_Arg_Length] Field(M$Panic_Fields)%;


// Maintenance Call: DEBUG

$FIELD M$DEBUG_Fields=
    SET
    Static_Fields(DE),			// Define the standard fields
    DE$Level		= [$ULong],	// Debug level mask
    DE$Local_Conn_ID	= [$ULong],	// Connection-ID (NYI)
    DE$Group		= [$ULong]	// glag cluster (DEBUG or ACTIVITY)
    TES;

LITERAL
    Debug_Arg_Length = $Field_Set_Size;
MACRO
    Debug_Args = Block[Debug_Arg_Length] Field(M$Debug_Fields)%;


// Maintenance Call: EVENT

$FIELD M$EVENT_Fields=
    SET
    Static_Fields(EV),			// Define the standard fields
    EV$Buf_Size		= [$SWord],	// Event buffer size
    EV$Data		= [$Byte]	// User data start
    TES;

LITERAL
    Event_Arg_Length = $Field_Set_Size;
MACRO
    Event_Args = Block[Event_Arg_Length] Field(M$Event_Fields)%;


// VMS cancel IO call to TCPACP.  This argument block is generated by the
// User_requests_avail rtn(module:maclib.bli) in response to the virtual device
// driver "TCH" sending a fake IRP (IO request packet) to the TCPACP.  Only
// item that are of interest/valid are the user's PID & the Process IO cahnnel
// for which the IO is being canceled.  VMS cancels IO on both the $CANCEL
// system service & when the user deasssigns ($DEASGN) the IO channel.

$FIELD M$VMS_Cancel_Fields=
    SET
    Static_Fields(VC),			// Define the standard fields
    VC$PIOchan		= [$Bytes(2)],	// Process IO channel (from IRP)
    VC$Conn_ID		= [$Bytes(4)]	// Connection ID
    TES;

LITERAL
    Cancel_Arg_Length = $Field_Set_Size;
MACRO
    VMS$Cancel_Args = BLOCK[Cancel_Arg_Length]
		      FIELD(M$VMS_Cancel_Fields)%;

// End: Structure.Def
#endif
#endif
