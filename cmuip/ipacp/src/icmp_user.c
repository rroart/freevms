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
//TITLE "Internet Control Message Protocol User Interface"
//SBTTL "Internet Control Message Protocol User Interface Overview"
/*

Module:

	ICMP_User

Facility:

	Internet Control Message Protocol (ICMP) User Interface

Abstract:

	ICMP provides the user with a access to ICMP service
	via the Internet Protocol (IP). This module handles the
	ICMP interface between the user and the IP layer.

Author:

	Bruce R. Miller, CMU Network Development, Nov. 1989
	Copyright (c) 1989, Carnegie-Mellon University

Modification History:

1.0c	18-Jul-1991	Henry W. Miller		USBR
	Use LIB$GET_VM_PAGE and LIB$FREE_VM_PAGE rather then LIB$GET_VM
	and LIB$FREE_VM, and check return status.

1.0b	09-Jul-1991	Henry W. Miller		USBR
	Added STARLET for VMS 5.4.

1.0a	13-Jan-1991	Henry W. Miller		USBR
	Make ICMPTTL a configurable variable.

*/

//SBTTL "Module definition"	

MODULE ICMP_User (IDENT="1.0c",LANGUAGE(BLISS32),
	    ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			    NONEXTERNAL=LONG_RELATIVE),
	    LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	    OPTIMIZE,OPTLEVEL=3,ZIP)=

{

// Include standard definition files

!LIBRARY "SYS$LIBRARY:STARLET";
#include "SYS$LIBRARY:LIB";
#include "CMUIP_SRC:[CENTRAL]NETERROR";
#include "CMUIP_SRC:[CENTRAL]NETXPORT";
#include "CMUIP_SRC:[CENTRAL]NETVMS";
#include "CMUIP_SRC:[CENTRAL]NETCOMMON";
#include "CMUIP_SRC:[CENTRAL]NetTCPIP";	// IP & ICMP definitions
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
    LOG_STATE,
    MIN_PHYSICAL_BUFSIZE,
    MAX_PHYSICAL_BUFSIZE,

    ICMP_MIB : ICMP_MIB_struct;	// ICMP Management Information Block


// External routines

extern

// MACLIB.MAR

 void    Swapbytes,
 void    Movbyt,
    Calc_Checksum,

// MEMGR.BLI

 void    MM$UArg_Free,
    MM$QBLK_Get,
 void    MM$QBLK_Free,
    MM$Seg_Get,
 void    MM$Seg_Free,

// USER.BLI

    USER$CHECK_ACCESS,
    USER$Err,
 VOID    IO$POST,
 void    User$Post_IO_Status,

// IP.BLI

 VOID    IP$SET_HOSTS,
    IP$S},

// NMLOOK.BLI

 VOID    NML$CANCEL,
 VOID    NML$GETALST,
 VOID    NML$GETNAME,

// IOUTIL.BLI

    GET_IP_ADDR,
 VOID    ASCII_DEC_BYTES,
 VOID    ASCII_HEX_BYTES,
 VOID    LOG_FAO,
 VOID    QL_FAO;


//SBTTL "ICMP data structures"

// Define the "ICMPCB" - ICMP analogue of TCB.

$FIELD  ICMPCB_Fields (void)
    SET
    ICMPCB$Foreign_Host	= [$Ulong],	// ICMP foreign host number
    ICMPCB$Local_Host	= [$Ulong],	//     local host
    ICMPCB$Foreign_Hname	= [$Bytes(MAX_HNAME)],
    ICMPCB$Foreign_Hnlen	= [$Short_Integer],
    ICMPCB$USR_Qhead	= [$Address],	// User receive request queue
    ICMPCB$USR_Qtail	= [$Address],
    ICMPCB$NR_Qhead	= [$Address],	// Net receive queue
    ICMPCB$NR_Qtail	= [$Address],
    ICMPCB$NR_Qcount	= [$Short_Integer],
    ICMPCB$Flags		= [$Bytes(2)],
    $OVERLAY(ICMPCB$Flags)
	ICMPCB$Wildcard	= [$Bit],	// ICMPCB opened with wild FH/FP/LH
!	ICMPCB$Addr_Mode	= [$Bit],	// User wants IP addresses
	ICMPCB$Aborting	= [$Bit],	// ICMPCB is closing
	ICMPCB$NMLook	= [$Bit],	// ICMPCB has an outstanding name lookup
	ICMPCB$Internal	= [$Bit],	// ICMPCB is open by ACP not user
    $CONTINUE
    ICMPCB$ICMPCBID		= [$Address],	// ICMPCB_Table index for this connection
    ICMPCB$UCB_Adrs	= [$Address],	// Connection UCB address
    ICMPCB$UARGS	= [$Address],	// Uarg block in pending open
    ICMPCB$User_ID	= [$Bytes(4)],	// Process ID of owner
    ICMPCB$PIOchan	= [$Bytes(2)]	// Process IO channel
    TES;

LITERAL
    ICMPCB_Size = $Field_Set_Size;
MACRO
    ICMPCB_Structure = BLOCK->ICMPCB_Size FIELD(ICMPCB_Fields) %;
//MESSAGE(%NUMBER(ICMPCB_Size)," longwords per ICMPCB")



//SBTTL "ICMP data storage"

signed long
    ICMPTTL	 = 32;	// TTL for ICMP

static signed long
    ICMPIPID  = 1,	// Current IP packet ID
    ICMPCB_Count  = 0,	// Count of active ICMPCBs
    ICMPCB_TABLE : VECTOR[MAX_ICMPCB+1];// Table of ICMPCBs



//SBTTL "ICMP packet logger"
/*
    Queue up a log entry to dump out a ICMP packet.
 )%

Log_ICMP_Packet(Seg,SwapFlag,SendFlag) : NOVALUE (void)
    {
    MAP
	struct ICMP_Structure * Seg;
    signed long
	sptr,
	segdata,
	segcopy : ICMP_Structure,
	struct ICMP_Structure * seghdr;

    seghdr = seg;		// Point at segment header
    segdata = seg + ICMP_Header_Size;
    if (SwapFlag)		// Need to byteswap header?
	{
	CH$MOVE(ICMP_Header_Size,CH$PTR(seg),CH$PTR(segcopy)); // Make a copy
	seghdr = segcopy;	// Point at this version...
	SwapBytes(ICMP_Header_Size/2,seghdr); // Swap header bytes
	};

// Print first part of info

    if (SendFlag)
	sptr = %ASCID"Sent"
    else
	sptr = %ASCID"Received";

// Log the contents of the ICMP header

    QL$FAO(%STRING("!%T !AS ICMP packet, SEG=!XL, DATA=!XL!/",
		   "!_CKsum:!_!SL!/"),
	    0,sptr,seg,segdata,
	   seghdr->ICM$CkSum);

    };

//SBTTL "ICMPCB_Find - look up ICMP control block"

ICMPCB_Find(Src$Adrs)
    {
    signed long
	Ucount,
	ICMPCBIX,
	struct ICMPCB_Structure * ICMPCB;
    Ucount = ICMPCB_Count;
    ICMPCBIX = 1;
    WHILE (Ucount > 0) && (ICMPCBIX <= Max_ICMPCB) DO
	{
	if ((ICMPCB = ICMPCB_Table[ICMPCBIX]) != 0)
	    {
	    IF ((ICMPCB->ICMPCB$Foreign_Host == WILD) OR
		(ICMPCB->ICMPCB$Foreign_Host == Src$Adrs)) THEN
		return ICMPCB;
	    Ucount = Ucount-1;
	    };
	ICMPCBIX = ICMPCBIX + 1;
	};
    return 0;
    };

//SBTTL "ICMP input handler"
/*
    Come here at AST level when input packet is determined to be ICMP packet.
    At present, all ICMP input handling is done at AST level, so we search
    the ICMPCB list and queue the ICMP packet for deliver here.
*/

FORWARD ROUTINE
    Queue_User_ICMP;

ICMP$User_Input(Src$Adrs,Dest$Adrs,BufSize,Buf,SegSize,Seg): NOvalue=
    {
    MAP
	struct ICMP_Structure * Seg;
    signed long
	Buf2,
	Uptr,
	Ucount,
	ICMPCBIX,
	sum,
	delete,
	struct ICMPCB_Structure * ICMPCB;
    LABEL
	X;

// Assume this packet (Buf2) should not be deleted

    delete = FALSE;

// Log the ICMP packet if desired

    if ($$LOGF(LOG$ICMP))
	Log_ICMP_Packet(Seg,TRUE,FALSE);

!!!HACK!!// I deleted this.  It should be done 

// Try to match the input packet up with a ICMPCB
!!!HACK!!// What if there's more than one ICMPCB for this address?
    ICMPCB = ICMPCB_Find(Src$Adrs);
    if (ICMPCB == 0)
	{
!!!HACK!!// Don"t worry if there"e no ICMPCB.
	if ($$LOGF(LOG$ICMP))
	    QL$FAO("!%T No ICMPCB found for segment !XL!/",0,Seg);
	}
    else
X:	{

// Log that it was found

	if ($$LOGF(LOG$ICMP))
	    QL$FAO("!%T ICMPCB !XL found for ICMP Seg !XL!/",
		   0,ICMPCB,Seg);

// Make sure the ICMPCB isn't aborted...

	if (ICMPCB->ICMPCB$Aborting)
	    {
	    XQL$FAO(LOG$ICMP,"!%T ICMP input !XL for aborted ICMPCB !XL dropped!/",
		    0,Seg,ICMPCB);
	    LEAVE X;
	    };

	Buf2 = MM$Seg_Get(Bufsize);	// Get a buffer
	Seg = Buf2 + (Seg - Buf);
!!!HACK!!// There's no need to copy the whole buffer, only Usize worth...
	MOVBYT(Bufsize,Buf,Buf2);

// Setup pointer to ICMP data and ICMP data size

	Uptr = Seg + ICMP_Header_Size;
	Ucount = SegSize - ICMP_Header_Size;

	    {
	    MAP
		struct IPADR$ADDRESS_BLOCK * Uptr;

// Kluge. Overwrite the ICMP/IP header in the buffer, since we don't need it.

	    Uptr = Uptr - IPADR$ADDRESS_BLEN;
	    Ucount = Ucount + IPADR$ADDRESS_BLEN;
	    UPTR->IPADR$EXT1 = Seg->ICM$EXT1;
	    UPTR->IPADR$EXT2 = Seg->ICM$VAR;
	    UPTR->IPADR$SRC_HOST = Src$Adrs;
	    UPTR->IPADR$DST_HOST = Dest$Adrs;
	    };

// Give the segment to the user now.

	delete = Queue_User_ICMP (ICMPCB,Uptr,Ucount,Buf2,Bufsize,0);
	};

// If the packet hasn't been given to the user, delete it now

    if (delete)
	MM$Seg_Free(Bufsize,Buf2);
    };


//SBTTL "Queue_User_ICMP - Queue up ICMP packet for delivery to user"
/*
    Called by ICMP_Input at AST level when an input packet matches a
    user ICMP "connection". Function of this routine is to either deliver
    the ICMP packet to the user (if a user read request is available) or
    queue it for later deliver.
    Returns TRUE if the ICMP packet has been fully disposed of (i.e. the
    caller may deallocate the packet), FALSE otherwise (i.e. the packet
    has been placed on a queue and may not be deallocated yet).
 )%

FORWARD ROUTINE
 VOID    DELIVER_ICMP_DATA;

Queue_User_ICMP(ICMPCB,Uptr,Usize,Buf,Bufsize,QB)
    {
    MAP
	struct ICMPCB_Structure * ICMPCB,
	struct Queue_BLK_Structure * QB(QB_NR_Fields);
    signed long
	QBR;
    EXTERNAL ROUTINE
	MM$QBlk_Get;
    LITERAL
	ICMPCB$NR_Qmax = 5;	// Max input packets permitted on input queue

// See if the input queue is full for this ICMPCB

    if (ICMPCB->ICMPCB$NR_Qcount > ICMPCB$NR_Qmax)
	{
	if ($$LOGF(LOG$ICMP))
	    QL$FAO("!%T ICMP at !XL dropped - ICMPCB NR queue full!/",0,Uptr);
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

    if (REMQUE(ICMPCB->ICMPCB$USR_Qhead,QBR) != Empty_Queue)
	Deliver_ICMP_Data(ICMPCB,QB,QBR)
    else
	INSQUE(QB,ICMPCB->ICMPCB$NR_Qtail);
    return FALSE;		// Don't deallocate this segment...
    };

//SBTTL "Deliver_ICMP_Data - Deliver ICMP data to user"
/*
    Perform actual delivery of ICMP packet to a user request.
    ICMP packet is copied into the user buffer and the user I/O request
    is posted.
 )%

Deliver_ICMP_Data(ICMPCB,QB,URQ) : NOVALUE (void)
    {
    MAP
	struct ICMPCB_Structure * ICMPCB,
	struct Queue_Blk_Structure * QB(QB_NR_Fields),
	struct Queue_Blk_Structure * URQ(QB_UR_Fields);
    signed long
	FLAGS,
	ICMTYPE,
	struct $BBLOCK * IRP[],
	struct User_RECV_Args * UArgs,
	struct User_RECV_Args * Sargs,
	Aptr,
	Uptr,
	Ucount;

// Determine data start and data count

    Ucount = QB->NR$Ucount - IPADR$ADDRESS_BLEN;
    Uptr = QB->NR$Uptr + IPADR$ADDRESS_BLEN;
    Aptr = QB->NR$Uptr;

// Truncate to user receive request size

    if (Ucount > URQ->UR$Size)
	Ucount = URQ->UR$Size;

    if ($$LOGF(LOG$ICMP))
	QL$FAO("!%T Posting ICMP receive,Size=!SL,ICMPCB=!XL,IRP=!XL,UCB_A=!XL!/",
	       0,Ucount,ICMPCB,URQ->UR$IRP_Adrs,URQ->UR$UCB_Adrs);

// Copy from our buffer to the user system buffer

    $$KCALL(MOVBYT,Ucount,Uptr,URQ->UR$Data);

// Copy ICMP Source and destination addresses to system space Diag Buff
// First, get the SysBlk address out of the IRP, then copy the Header
// block from our local copy of UArgs.

    UArgs = URQ->UR$UArgs;
    IRP = URQ->UR$IRP_Adrs;
    if (Uargs->RE$PH_Buff != 0)
	$$KCALL(MOVBYT,IPADR$ADDRESS_BLEN,
		.Aptr,Uargs->RE$PH_Buff);

// Post the I/O and free up memory

    User$Post_IO_Status(URQ->UR$Uargs,SS$_NORMAL,
			.Ucount,0,0);
    MM$UArg_Free(URQ->UR$Uargs);

    MM$QBLK_Free(URQ);
    MM$Seg_Free(QB->NR$Buf_Size,QB->NR$Buf);
    MM$QBLK_Free(QB);
    };

//SBTTL "ICMPCB_OK - Match connection ID to ICMPCB address"

ICMPCB_OK(Conn_ID,RCaddr,struct User_Default_Args * Uargs)
    {
    signed long
	struct ICMPCB_Structure * ICMPCB;
    MACRO
	ICMPCBERR(EC) = (RCaddr = EC; return 0) %;

// Range check the connection id. This should never fail, since the user should
// not be fondling connection IDs.

    if ((Conn_ID <= 0) || (Conn_ID > MAX_ICMPCB))
	ICMPCBERR(NET$_CDE);	// Nonexistant connection ID
    ICMPCB = ICMPCB_Table[Conn_ID];

// Make sure the table had something reasonable for this connection ID

    if (ICMPCB <= 0)
	ICMPCBERR(NET$_CDE);	// ICMPCB has been deleted (possible)

// Check consistancy of ICMPCB back-pointer into table

    IF (ICMPCB->ICMPCB$ICMPCBID != Conn_ID) OR
       (ICMPCB->ICMPCB$UCB_ADRS != Uargs->UD$UCB_Adrs) THEN
	ICMPCBERR(NET$_CDE);	// Confusion (can this happen?)

// Everything is good - return the ICMPCB address

    return ICMPCB;
    };

//SBTTL "ICMPCB_Get - Allocate and initialize one ICMPCB"

ICMPCB_Get(IDX)
    {
    EXTERNAL ROUTINE
	LIB$GET_VM	: ADDRESSING_MODE(GENERAL),
	LIB$GET_VM_PAGE	: ADDRESSING_MODE(GENERAL);
    signed long
	struct ICMPCB_Structure * ICMPCB,
	ICMPCBIDX,
	RC,
	Pages ;
    LABEL
	X;

// Find a free slot in the ICMPCB table

X:  {			// ** Block X **
    ICMPCBIDX = 0;
    INCR I FROM 1 TO MAX_ICMPCB DO
	if (ICMPCB_Table[I] == 0)
	    LEAVE X WITH (ICMPCBIDX = I);
    return 0;			// Failed to allocate a ICMPCB
    };			// ** Block X **

// Allocate some space for the ICMPCB

//    LIB$GET_VM(%REF(ICMPCB_Size*4),ICMPCB);
    Pages = ((ICMPCB_Size * 4) / 512) + 1 ;
    RC = LIB$GET_VM_PAGE(Pages, ICMPCB);
    if (NOT RC)
	FATAL$FAO("ICMPCB_GET - LIB$GET_VM failure, RC=!XL",RC);

// Clear it out and set it in the table

    ICMPCB_Table[ICMPCBIDX] = ICMPCB;
    CH$FILL(%CHAR(0),ICMPCB_Size*4,ICMPCB);
    ICMPCB_Count = ICMPCB_Count+1;

// Initialize queue headers for the ICMPCB

    ICMPCB->ICMPCB$NR_Qhead = ICMPCB->ICMPCB$NR_Qtail = ICMPCB->ICMPCB$NR_Qhead;
    ICMPCB->ICMPCB$USR_Qhead = ICMPCB->ICMPCB$USR_Qtail = ICMPCB->ICMPCB$USR_Qhead;

// Set the connection ID

    ICMPCB->ICMPCB$ICMPCBID = ICMPCBIDX;

// Return the pointer

    IDX = ICMPCBIDX;
    return ICMPCB;
    };

//SBTTL "ICMPCB_Free - Deallocate a ICMPCB"

void ICMPCB_Free(ICMPCBIX,struct ICMPCB_Structure * ICMPCB) (void)
    {
    EXTERNAL ROUTINE
	LIB$FREE_VM 		: ADDRESSING_MODE(GENERAL),
	LIB$FREE_VM_PAGE	: ADDRESSING_MODE(GENERAL);

    signed long
	RC,
	Pages ;

// Clear the table entry

    ICMPCB_Table[ICMPCBIX] = 0;

// Free the memory and decrement our counter.

//    LIB$FREE_VM(%REF(ICMPCB_Size*4),ICMPCB);
    Pages = ((ICMPCB_Size * 4) / 512) + 1 ;
    RC = LIB$FREE_VM_PAGE(Pages, ICMPCB);
    if (NOT RC)
	FATAL$FAO("ICMPCB_FREE - LIB$FREE_VM failure, RC=!XL",RC);
    ICMPCB_Count = ICMPCB_Count-1;
    };

//SBTTL "Kill_ICMP_Requests - purge all I/O requests for a connection"

void Kill_ICMP_Requests(struct ICMPCB_Structure * ICMPCB,RC) (void)
    {
    signed long
	struct Queue_Blk_Structure * URQ(QB_UR_Fields),
	struct Queue_Blk_Structure * QB(QB_NR_Fields);

// Make sure we aren't doing this more than once
!
//   if (ICMPCB->ICMPCB$Aborting)
!	RETURN;

// Say that this connection is aborting (prevent future requests)

    ICMPCB->ICMPCB$Aborting = TRUE;

// Cancel any name lookup in progess

    if (ICMPCB->ICMPCB$NMLOOK)
	{
	NML$CANCEL(ICMPCB, 0, 0);
	ICMPCB->ICMPCB$NMLOOK = FALSE;
	};

// Kill any pending open

    NOINT;
    if (ICMPCB->ICMPCB$UARGS != 0)
	{
	USER$Err(ICMPCB->ICMPCB$UARGS,RC);
	ICMPCB->ICMPCB$UARGS = 0;
	};
    OKINT;

// Purge the user request queue, posting all requests

    WHILE REMQUE(ICMPCB->ICMPCB$USR_Qhead,URQ) != Empty_Queue DO
	{
	if (ICMPCB->ICMPCB$Internal)
	    (URQ->UR$ASTADR)(URQ->UR$ASTPRM,RC,0)
	else
	    {
	    User$Post_IO_Status(URQ->UR$Uargs,RC,0,0,0);
	    MM$UArg_Free(URQ->UR$Uargs);
	    };
	MM$QBlk_Free(URQ);	
	};

// Purge any received qblocks as well

    WHILE REMQUE(ICMPCB->ICMPCB$NR_Qhead,QB) != Empty_Queue DO
	{
	MM$Seg_Free(QB->NR$Buf_Size,QB->NR$Buf);
	MM$QBlk_Free(QB);
	};
    };

//SBTTL "ICMPCB_Close - Close/deallocate a ICMPCB"

void ICMPCB_Close(UIDX,struct ICMPCB_Structure * ICMPCB,RC) (void)
    {
    Kill_ICMP_Requests(ICMPCB,RC);
    ICMPCB_FREE(UIDX,ICMPCB);
    };

void ICMPCB_Abort(struct ICMPCB_Structure * ICMPCB,RC) (void)
!
// Abort a ICMPCB - called by ICMP code.
!
    {
    if (ICMPCB->ICMPCB$Internal)
	Kill_ICMP_Requests(ICMPCB,RC)
    else
	ICMPCB_CLOSE(ICMPCB->ICMPCB$ICMPCBID,ICMPCB,RC);
    };


//SBTTL "ICMP$Purge_All_IO - delete ICMP database before network exits"

ICMP$Purge_All_IO : NOVALUE (void)
    {
    signed long
	ICMPCBIDX,
	struct ICMPCB_Structure * ICMPCB;

// Loop for all connections, purge them, and delete them.

    INCR ICMPCBIDX FROM 1 TO MAX_ICMPCB DO
	if ((ICMPCB = ICMPCB_Table[ICMPCBIDX]) != 0)
	    ICMPCB_Close(ICMPCBIDX,ICMPCB,NET$_TE);
    };


//SBTTL "ICMP$OPEN - open a ICMP "connection""
/*
    Open a ICMP "connection". Create a ICMP Control Block, which serves as a
    place to hang incoming packets and user receive requests.
 )%

FORWARD ROUTINE
    ICMP_COPEN_DONE,
 VOID    ICMP_NMLOOK_DONE,
 VOID    ICMP_ADLOOK_DONE;

void ICMP$OPEN(struct User_Open_Args * Uargs) (void)
    {
    signed long
	IPADDR,
	NAMLEN,
	NAMPTR,
	UIDX,
	struct ICMPCB_Structure * ICMPCB,
	ICMPCBPTR,
	Args : VECTOR->4;
    LABEL
	X;

    XLOG$FAO(LOG$USER,"!%T ICMP$OPEN: PID=!XL,CHAN=!XW,FLAGS=!XL X1=!XL!/",
	     0,Uargs->OP$PID,Uargs->OP$PIOchan,Uargs->OP$FLAGS,
	     UArgs->OP$Ext1);

// First create a ICMPCB for this connection.

    if ((ICMPCB = ICMPCB_Get(UIDX)) <= 0)
	{
	USER$Err(Uargs,NET$_UCT);
	RETURN;
	};

// Initialize user mode values

    ICMPCB->ICMPCB$UCB_ADRS = Uargs->OP$UCB_Adrs;
    ICMPCB->ICMPCB$User_ID = Uargs->OP$PID;
    ICMPCB->ICMPCB$PIOchan = Uargs->OP$PIOchan;

// At this point, the connection exists. Write the connection ID
// back into the Unit Control Block for this connection.

    ICMPCBptr = Uargs->OP$UCB_Adrs + UCB$L_CBID;
    $$KCALL(MOVBYT,4,UIDX,ICMPCBptr);

// Initialize queue headers for the ICMPCB

    ICMPCB->ICMPCB$NR_Qhead = ICMPCB->ICMPCB$NR_Qtail = ICMPCB->ICMPCB$NR_Qhead;
    ICMPCB->ICMPCB$USR_Qhead = ICMPCB->ICMPCB$USR_Qtail = ICMPCB->ICMPCB$USR_Qhead;

// Copy user arguments to ICMPCB

// Handle wildcard host

    NAMPTR = CH$PTR(Uargs->OP$Foreign_Host);
    NAMLEN = Uargs->OP$Foreign_Hlen;
    if ((NOT Uargs->OP$ADDR_FLAG) && (NAMLEN == 0))
	{
	ICMPCB->ICMPCB$Wildcard = TRUE;
	ICMPCB->ICMPCB$Foreign_Host = WILD;
	ICMPCB->ICMPCB$Foreign_Hnlen = 0;
	ICMPCB->ICMPCB$Local_Host = WILD;
	ICMPCB->ICMPCB$Uargs = Uargs;
	ICMP_NMLOOK_DONE(ICMPCB,SS$_NORMAL,0,0,0,0);
	RETURN;
	};

// Check for supplied IP address instead of name

X:  {			// *** Block X ***
    if (Uargs->OP$ADDR_FLAG)
	IPADDR = Uargs->OP$Foreign_Address
    else
	if (GET_IP_ADDR(NAMPTR,IPADDR) LSS 0)
	    LEAVE X;
    ICMPCB->ICMPCB$Foreign_Hnlen = 0;
    ICMPCB->ICMPCB$Uargs = Uargs;
    ICMP_NMLOOK_DONE(ICMPCB,SS$_NORMAL,1,IPADDR,0,0);
    ICMPCB->ICMPCB$NMLook = TRUE;
    NML$GETNAME(IPADDR,ICMP_ADLOOK_DONE,ICMPCB);
    RETURN;
    };			// *** Block X ***

// "standard" case, host name is supplied - start name lookup for it

    ICMPCB->ICMPCB$Uargs = Uargs;
    ICMPCB->ICMPCB$NMLook = TRUE;
    NML$GETALST(NAMPTR,NAMLEN,ICMP_NMLOOK_DONE,ICMPCB);
    };



//SBTTL "ICMP_NMLOOK_DONE - Second phase of ICMP$OPEN when namelookup done"
/*
    Come here when the foreign host name has been resolved.
    At this point, we set the local & foreign hosts in the ICMPCB
    and post the users open request.
*/

ICMP_NMLOOK_DONE(ICMPCB,STATUS,ADRCNT,ADRLST,NAMLEN,NAMPTR) : NOVALUE (void)
    {
    MAP
	struct ICMPCB_Structure * ICMPCB;
    signed long
	RC,
	struct User_Open_Args * Uargs,
	IOSB : NetIO_Status_Block;
    MACRO
	UOP_ERROR(EC) = 
	    {
	    USER$Err(Uargs,EC);
	    ICMPCB_FREE(ICMPCB->ICMPCB$ICMPCBID,ICMPCB);
	    RETURN;
	    } %;

// Clear name lookup flag and get uargs

    NOINT;
    ICMPCB->ICMPCB$NMLook = FALSE;
    Uargs = ICMPCB->ICMPCB$Uargs;
    ICMPCB->ICMPCB$Uargs = 0;
    OKINT;

// Check status of the name lookup

    if (NOT STATUS)
	UOP_ERROR(STATUS);

// Finish up the common part of the open

    RC = ICMP_COPEN_DONE(ICMPCB,ADRCNT,ADRLST);
    if (NOT RC)
	UOP_ERROR(RC);

// Verify that we have access to the host set

!!!HACK!!// Should we do this or not??
//    RC = USER$CHECK_ACCESS(ICMPCB->ICMPCB$USER_ID,ICMPCB->ICMPCB$Local_Host,
!		      0,ICMPCB->ICMPCB$Foreign_Host,0);
//    if (NOT RC)
!	UOP_ERROR(RC);

// Set the foreign host name in the ICMPCB

    ICMPCB->ICMPCB$Foreign_Hnlen = NAMLEN;
    if (NAMLEN != 0)
	CH$MOVE(NAMLEN,NAMPTR,CH$PTR(ICMPCB->ICMPCB$Foreign_Hname));

// Finally, post the status

    IOSB->NSB$STATUS = SS$_NORMAL; // Success return
    IOSB->NSB$Byte_Count = 0;
    IOSB->NSB$XSTATUS = 0;
    IO$POST(IOSB,Uargs);
    MM$UArg_Free(Uargs);
    };

//SBTTL "ICMP_COPEN_DONE - Common user/internal ICMP open done routine"

ICMP_COPEN_DONE(ICMPCB,ADRCNT,ADRLST)
    {
    MAP
	struct ICMPCB_Structure * ICMPCB;

// Set local and foreign host numbers according to our info

    if (ADRCNT > 0)
	IP$SET_HOSTS(ADRCNT,ADRLST,ICMPCB->ICMPCB$Local_Host,
		     ICMPCB->ICMPCB$Foreign_Host);

// Done at last - log success

    XLOG$FAO(LOG$USER,"!%T UDB_COPEN: Conn idx = !XL, ICMPCB = !XL!/",
	     0,ICMPCB->ICMPCB$ICMPCBID,ICMPCB);
    return SS$_NORMAL;
    };

//SBTTL "ICMP_ADLOOK_DONE - Finish ICMP address to name lookup"

ICMP_ADLOOK_DONE(ICMPCB,STATUS,NAMLEN,NAMPTR) : NOVALUE (void)
    {
    MAP
	struct ICMPCB_Structure * ICMPCB;

// Clear pending name lookup flag

    ICMPCB->ICMPCB$NMLook = FALSE;

// Check status

    if (NOT STATUS)
	RETURN;

// Copy the hostname into the ICMPCB

    ICMPCB->ICMPCB$Foreign_Hnlen = NAMLEN;
    CH$MOVE(NAMLEN,NAMPTR,CH$PTR(ICMPCB->ICMPCB$Foreign_Hname));
    };

//SBTTL "ICMP$CLOSE - close ICMP "connection""
/*
    Close a ICMP "connection". Kills any receive requests that haven't
    finished yet and deallocates the ICMPCB and any other data structures
    associated with a connection.
*/

void ICMP$CLOSE(struct User_Close_Args * Uargs) (void)
    {
    signed long
	struct ICMPCB_Structure * ICMPCB,
	RC;

// Check for valid ICMPCB

    if ((ICMPCB = ICMPCB_OK(Uargs->CL$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);
	RETURN;
	};

// Use common routine for closing

    ICMPCB_Close(Uargs->CL$Local_Conn_ID,ICMPCB,NET$_CC);

// Close done - post user request and free argblk

    User$Post_IO_Status(Uargs,SS$_NORMAL,0,0,0);
    MM$UArg_Free(Uargs);
    };

//SBTTL "ICMP$ABORT - abort ICMP "connection""
/*
    Abort a ICMP "connection". Identical in functionality to ICMP$CLOSE.
 )%

void ICMP$ABORT(struct User_Abort_Args * Uargs) (void)
    {
    signed long
	struct ICMPCB_Structure * ICMPCB,
	RC;

// Check for valid ICMPCB

    if ((ICMPCB = ICMPCB_OK(Uargs->AB$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);
	RETURN;
	};

// Use common routine for closing

    ICMPCB_Close(Uargs->AB$Local_Conn_ID,ICMPCB,NET$_CC);

// Done. Clean up.

    User$Post_IO_Status(Uargs,SS$_NORMAL,0,0,0);
    MM$UArg_Free(Uargs);
    };

//SBTTL "ICMP$S} - send ICMP packet"
/*
    Handle user send request for ICMP connection. Form a ICMP packet from the
    user's data buffer and hand it to IP layer for transmission.
 )%

void ICMP$S}(struct User_Send_Args * Uargs) (void)
    {
    signed long
	RC,
	Bufsize,
	Buf,
	LocalAddr, ForeignAddr,
	struct ICMP_Structure * Seg,
	Segsize,
	struct IPADR$ADDRESS_BLOCK * Uhead,
	USize,
	struct ICMPCB_Structure * ICMPCB;

// Validate connection ID and get ICMPCB pointer

    if ((ICMPCB = ICMPCB_OK(Uargs->SE$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// No such connection
	ICMP_MIB->MIB$icmpOutErrors =
		ICMP_MIB->mib$icmpOutErrors + 1;
	RETURN;
	};
    XLOG$FAO(LOG$USER,"!%T ICMP$S}: Conn=!XL, ICMPCB=!XL, Size=!SL!/",
	     0,Uargs->SE$Local_Conn_ID,ICMPCB,Uargs->SE$Buf_size);

// Check for aborted connection

    if (ICMPCB->ICMPCB$Aborting)
	{
	XLOG$FAO(LOG$USER,"!%T ICMP$S} for aborted ICMPCB !XL!/",0,ICMPCB);
	USER$Err(Uargs,NET$_CC);
	ICMP_MIB->MIB$icmpOutErrors =
		ICMP_MIB->mib$icmpOutErrors + 1;
	RETURN;
	};

// Check for invalid buffer size

    if (Uargs->SE$Buf_Size LSS 0)
	{
	USER$Err(Uargs,NET$_BTS);
	ICMP_MIB->MIB$icmpOutErrors =
		ICMP_MIB->mib$icmpOutErrors + 1;
	RETURN;
	};

// Check for "address mode" connection and set host addresses from user buffer
// in that case.

    ForeignAddr = ICMPCB->ICMPCB$Foreign_Host;
    if (ForeignAddr == WILD)
	ForeignAddr = ICMPCB->ICMPCB$Foreign_Host;

    LocalAddr = ICMPCB->ICMPCB$Local_Host;
    if (LocalAddr == WILD)
    IP$SET_HOSTS(1,ForeignAddr,LocalAddr,ForeignAddr);

   if ((ForeignAddr == WILD))
	{
	USER$Err(Uargs,NET$_NOPN);
	ICMP_MIB->MIB$icmpOutErrors =
		ICMP_MIB->mib$icmpOutErrors + 1;
	RETURN;
	};

// Allocate an output buffer and build an IP packet

    USize = Uargs->SE$Buf_size;
    if (Usize > Max_ICMP_Data_Size)
	Usize = Max_ICMP_Data_Size;

// Use preallocated buffer sizes to reduce dynamic memory load

    bufsize = Usize + IP_hdr_byte_size + Device_header;
    if (bufsize <= MIN_PHYSICAL_BUFSIZE)
	bufsize = MIN_PHYSICAL_BUFSIZE
    else
	if (bufsize <= MAX_PHYSICAL_BUFSIZE)
	    bufsize = MAX_PHYSICAL_BUFSIZE;
    Buf = MM$Seg_Get(Bufsize);	// Get a buffer
    Seg = Buf + device_header + IP_hdr_byte_size; // Point at ICMP segment
    Segsize = Usize+ICMP_Header_Size; // Length of segment + ICMP header

// Set up the ICMP header
    UHead = Uargs->SE$ProtoHdrBlk;
    Seg->ICM$Type = UHead->IPADR$TYPE;
    Seg->ICM$Code = UHead->IPADR$CODE;
    Seg->ICM$CkSum = 0;
    Seg->ICM$VAR = UHead->IPADR$SPECIAL;

// Copy the user data into the data area

    $$KCALL(MOVBYT,Usize,Uargs->SE$Data_Start,Seg->ICM$Data);

// Swap the header bytes and compute the checksum

    SwapBytes(ICMP_Header_Size/2,Seg);
!!!HACK!!// Hardwired in ICMP Header size of 8.
    Seg->ICM$CkSum=Calc_Checksum(USize+ICMP_Header_Size,Seg);

// Log the ICMP packet if desired

    if ($$LOGF(LOG$ICMP))
	Log_ICMP_Packet(Seg,FALSE,TRUE);

// Send the segment to IP (it will deallocate it)

    ICMPIPID = ICMPIPID+1;	// Increment packet ID
    RC = SS$_NORMAL;
    IF (IP$S}(LocalAddr,ForeignAddr,ICMPTOS,ICMPTTL,
		   Seg,Segsize,ICMPIPID,ICMPDF,TRUE,ICMP_Protocol,
		   Buf,Bufsize) == 0) THEN RC = NET$_NRT;

    // Keep count of outgoing packets and errors
    ICMP_MIB->MIB$icmpOutMsgs = ICMP_MIB->mib$icmpOutMsgs + 1;
    if (RC != SS$_NORMAL) ICMP_MIB->MIB$icmpOutErrors =
				    ICMP_MIB->mib$icmpOutErrors + 1;

    // Do SNMP accounting
    SELECTONE Seg->ICM$Type OF
	SET
	[ICM_ECHO]:	ICMP_MIB->MIB$icmpOutEchos =
				ICMP_MIB->mib$icmpOutEchos + 1;
	[ICM_TSTAMP]:	ICMP_MIB->MIB$icmpOutTimeStamps =
				ICMP_MIB->mib$icmpOutTimeStamps + 1;
	[ICM_AMREQUEST]:ICMP_MIB->MIB$icmpOutAddrMasks =
				ICMP_MIB->mib$icmpOutAddrMasks + 1;
	[ICM_DUNREACH]:	ICMP_MIB->MIB$icmpOutDestUnreachs =
				ICMP_MIB->mib$icmpOutDestUnreachs + 1;
	[ICM_SQUENCH]:	ICMP_MIB->MIB$icmpOutSrcQuenchs =
				ICMP_MIB->mib$icmpOutSrcQuenchs + 1;
	[ICM_REDIRECT]:	ICMP_MIB->MIB$icmpOutRedirects =
				ICMP_MIB->mib$icmpOutRedirects + 1;
	[ICM_TEXCEED]:	ICMP_MIB->MIB$icmpOutTimeExcds =
				ICMP_MIB->mib$icmpOutTimeExcds + 1;
	[ICM_PPROBLEM]:	ICMP_MIB->MIB$icmpOutParamProbs =
				ICMP_MIB->mib$icmpOutParamProbs + 1;
	[ICM_TSREPLY]:	ICMP_MIB->MIB$icmpOutTimestampReps =
				ICMP_MIB->mib$icmpOutTimestampReps + 1;
	[ICM_AMREPLY]:	ICMP_MIB->MIB$icmpOutAddrMaskReps =
				ICMP_MIB->mib$icmpOutAddrMaskReps + 1;
	[ICM_EREPLY]:	ICMP_MIB->MIB$icmpOutEchoReps =
				ICMP_MIB->mib$icmpOutEchoReps + 1;
	TES;

// Post the I/O request back to the user

    User$Post_IO_Status(Uargs,RC,0,0,0);
    MM$UArg_Free(Uargs);

    };



//SBTTL "ICMP$RECEIVE - receive a ICMP packet"
/*
    Handle user receive request for ICMP connection. If there is a packet
    available on the ICMP receive queue, then deliver it to the user
    immediately. Otherwise, queue up the user receive for later.
 )%

void ICMP$RECEIVE(struct User_Recv_Args * Uargs) (void)
    {
    signed long
	struct ICMPCB_Structure * ICMPCB,
	struct Queue_Blk_Structure * QB(QB_NR_Fields),
	struct Queue_Blk_Structure * URQ(QB_UR_Fields),
	RC;

// Validate connection ID and get ICMPCB pointer

    if ((ICMPCB = ICMPCB_OK(Uargs->RE$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// No such connection
	RETURN;
	};
    XLOG$FAO(LOG$USER,"!%T ICMP$RECEIVE: Conn=!XL, ICMPCB=!XL, Size=!SL!/",
	     0,Uargs->RE$Local_Conn_ID,ICMPCB,Uargs->RE$Buf_size);

// Check for aborted connection

    if (ICMPCB->ICMPCB$Aborting)
	{
	XLOG$FAO(LOG$USER,"!%T ICMP$RECEIVE for aborted ICMPCB !XL!/",0,ICMPCB);
	USER$Err(Uargs,NET$_CC);
	RETURN;
	};

// Check for invalid buffer size

    if (Uargs->RE$Buf_Size <= 0)
	{
	USER$Err(Uargs,NET$_BTS);
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
    if (REMQUE(ICMPCB->ICMPCB$NR_Qhead,QB) != Empty_Queue)
	Deliver_ICMP_Data(ICMPCB,QB,URQ)
    else
	INSQUE(URQ,ICMPCB->ICMPCB$USR_Qtail);
    OKINT;
    };



//SBTTL "ICMP$INFO - get info about ICMP "connection""
/*
    Read the host names/numbers for a ICMP connection.
 )%

void ICMP$INFO(struct User_Info_Args * Uargs) (void)
    {
    EXTERNAL ROUTINE
	USER$Net_Connection_Info : NOVALUE;
    signed long
	struct ICMPCB_Structure * ICMPCB,
	RC;

// Validate the connection ID

    if ((ICMPCB = ICMPCB_OK(Uargs->IF$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Bad connection ID
	RETURN;
	};

// Give the information back (common TCP/ICMP routine in USER.BLI)

    USER$Net_Connection_Info(Uargs,ICMPCB->ICMPCB$Local_Host,
			ICMPCB->ICMPCB$Foreign_Host,
			0,0,
			ICMPCB->ICMPCB$Foreign_Hname,
			ICMPCB->ICMPCB$Foreign_Hnlen);
    };


//SBTTL "ICMP$STATUS - get status of ICMP "connection""
/*
    This routine is a placeholder for the network STATUS command, which is
    currently implemented for the TCP protocol.
 )%

void ICMP$STATUS(struct User_Status_Args * Uargs) (void)
    {
    USER$Err(Uargs,NET$_NYI);
    };

//SBTTL "ICMP$CANCEL - Handle VMS cancel for ICMP connection"
/*
    Handle process abort/$CANCEL request for a ICMP connection. Identical
    in functionality to ICMP$CLOSE/ICMP$ABORT except for calling procedure.
 )%

ICMP$CANCEL(struct VMS$Cancel_Args * Uargs)
    {
    signed long
	struct ICMPCB_Structure * ICMPCB,
	Fcount;

    Fcount = 0;

// Check all valid ICMPCB's looking for a match on pid and channel #.

    INCR I FROM 1 TO MAX_ICMPCB DO
	if ((ICMPCB = ICMPCB_Table[I]) != 0)
	    {

// If the process doing the cancel owns this connection, then delete it.

	    IF (ICMPCB->ICMPCB$User_ID == Uargs->VC$PID) AND
	       (ICMPCB->ICMPCB$PIOchan == Uargs->VC$PIOchan) THEN
		{
		XLOG$FAO(LOG$USER,"!%T ICMP$Cancel: ICMPCB=!XL!/",0,ICMPCB);
		ICMPCB_Close(I,ICMPCB,NET$_ccan);
		Fcount = Fcount + 1;
		};
	    };
    return Fcount;
    };

//SBTTL "ICMP dump routines"

ICMP$Connection_List(RB) : NOVALUE (void)
!
// Dump out the list of ICMP connections.
!
    {
    MAP
	struct D$ICMP_List_Return_Blk * RB;
    signed long
	RBIX;
    RBIX = 1;
    INCR I FROM 1 TO MAX_ICMPCB-1 DO
	if (ICMPCB_TABLE[I] != 0)
	    {
	    RB[RBIX] = I;
	    RBIX = RBIX + 1;
	    };
    RB->0 = RBIX - 1;
    };

ICMP$ICMPCB_Dump(ICMPCBIX,RB)
!
// Dump out a single ICMP connection
!
    {
    MAP
	struct D$ICMPCB_Dump_Return_BLK * RB;
    signed long
	struct ICMPCB_Structure * ICMPCB,
	QB,
	Qcount;

// Validate that there is a real ICMPCB there

    IF (ICMPCBIX LSS 1) || (ICMPCBIX > MAX_ICMPCB) OR
       ((ICMPCB = ICMPCB_TABLE[ICMPCBIX]) == 0) THEN
	return FALSE;

// Copy the ICMPCB contents

    RB->DU$ICMPCB_Address = ICMPCB;
    RB->DU$ICMPCB_Foreign_Host = ICMPCB->ICMPCB$Foreign_Host;
    RB->DU$ICMPCB_Local_Host = ICMPCB->ICMPCB$Local_Host;
    RB->DU$ICMPCB_Flags = ICMPCB->ICMPCB$Flags;
    RB->DU$ICMPCB_User_ID = ICMPCB->ICMPCB$User_ID;

// Get length of network receive queue

    QB = ICMPCB->ICMPCB$NR_Qhead;
    Qcount = 0;
    WHILE (QB NEQA ICMPCB->ICMPCB$NR_Qhead) DO
	{
	MAP
	    struct Queue_Blk_Structure * QB(QB_NR_Fields);
	Qcount = Qcount + 1;
	QB = QB->NR$NEXT;
	};
    RB->DU$ICMPCB_NR_Qcount = Qcount;

// Get length of user receive queue

    QB = ICMPCB->ICMPCB$USR_Qhead;
    Qcount = 0;
    WHILE (QB NEQA ICMPCB->ICMPCB$USR_Qhead) DO
	{
	MAP
	    struct Queue_Blk_Structure * QB(QB_UR_Fields);
	Qcount = Qcount + 1;
	QB = QB->UR$NEXT;
	};
    RB->DU$ICMPCB_UR_Qcount = Qcount;

// Done.

    return TRUE;
    };
}
ELUDOM
