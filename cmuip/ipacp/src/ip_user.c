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
//TITLE "Internet Protocol User Interface"
//SBTTL "Internet Protocol User Interface Overview"
/*

Module:

	IP_User

Facility:

	Internet Protocol (IP) User Interface

Abstract:

	IP provides the user with a access to IP service.
	This module handles the IP interface between the user
	and the IP layer.

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

#if 0
MODULE IP_User (IDENT="1.0c",LANGUAGE(BLISS32),
	    ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			    NONEXTERNAL=LONG_RELATIVE),
	    LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	    OPTIMIZE,OPTLEVEL=3,ZIP)=

#endif

// Include standard definition files

//LIBRARY "SYS$LIBRARY:STARLET";
// not yet #include "SYS$LIBRARY:LIB";
#include <cmuip/central/include/neterror.h>
// not yet #include "CMUIP_SRC:[CENTRAL]NETXPORT
#include "netvms.h"
#include <cmuip/central/include/netcommon.h>
#include <cmuip/central/include/nettcpip.h>			// IP & ICMP definitions
#include "structure.h"
#include "tcpmacros.h"

#include <ssdef.h>

//*** Special literals from USER.BLI ***

extern signed long LITERAL
    UCB$Q_DDP,
    UCB$L_CBID,
    UCB$L_EXTRA;

// External data items

extern signed long
    IPTTL,
    INTDF,
    AST_In_Progress,
    LOG_STATE,
    MIN_PHYSICAL_BUFSIZE,
    MAX_PHYSICAL_BUFSIZE;

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
    IP$S}_RAW,
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


//SBTTL "IP data structures"

// Define the "IPCB" - IP analogue of TCB.

struct  IPCB_Structure
{
unsigned long     IPCB$Foreign_Host	;	// IP foreign host number
unsigned long     IPCB$Host_Filter	;	// Receive packets from this host
unsigned long     IPCB$Proto_Filter	;	// Receive packets to this protocol
    IPCB$Foreign_Hname	= [$Bytes(MAX_HNAME)],
    IPCB$Foreign_Hnlen	= [$Short_Integer],
void *     IPCB$USR_Qhead	;	// User receive request queue
void *     IPCB$USR_Qtail	;
void *     IPCB$NR_Qhead	;	// Net receive queue
void *     IPCB$NR_Qtail	;
unsigned short int    IPCB$NR_Qcount;
union {
      unsigned short    IPCB$Flags;
struct {
unsigned 	IPCB$Wildcard	 : 1;	// IPCB opened with wild FH/LH
unsigned 	IPCB$Addr_Mode	 : 1;	// User wants IP addresses
unsigned 	IPCB$Aborting	 : 1;	// IPCB is closing
unsigned 	IPCB$NMLook	 : 1;	// IPCB has an outstanding name lookup
	  };
};
void *     IPCB$IPCBID		;	// IPCB_Table index for this connection
void *     IPCB$UCB_Adrs	;	// Connection UCB address
void *     IPCB$UARGS	;	// Uarg block in pending open
unsigned long    IPCB$User_ID;	// Process ID of owner
unsigned short    IPCB$PIOchan;	// Process IO channel
      };

#if 0
LITERAL
    IPCB_Size = $Field_Set_Size;
MACRO
    IPCB_Structure = BLOCK->IPCB_Size FIELD(IPCB_Fields) %;
//MESSAGE(%NUMBER(IPCB_Size)," longwords per IPCB")
#endif



//SBTTL "IP data storage"

signed long
    IPIPID  = 1,	// Current IP packet ID
    IPCB_Count  = 0,	// Count of active IPCBs
    IPCB_TABLE : VECTOR[MAX_IPCB+1];// Table of IPCBs



//SBTTL "IP packet logger"
/*
    Queue up a log entry to dump out a IP packet.
 */

void Log_IP_Packet(Seg,SwapFlag,SendFlag)
	struct IP_Structure * Seg;
    {
    signed long
	Header_Size,
	sptr,
	segdata;
	struct ip_structure * segcopy;
	struct ip_structure * seghdr;

//!!HACK!!// Make sure this works right.

    seghdr = seg;		// Point at segment header
    Header_Size = Seg->IPH$IHL * 4;	// Calculate header size
    segdata = seg + Header_Size;
    if (SwapFlag)		// Need to byteswap header?
	{
	CH$MOVE(Header_Size,CH$PTR(seg),CH$PTR(segcopy)); // Make a copy
	seghdr = segcopy;	// Point at this version...
	SwapBytes(Header_Size/2,seghdr); // Swap header bytes
	};

// Print first part of info

    if (SendFlag)
	sptr = %ASCID"Sent"
    else
	sptr = %ASCID"Received";

// Log the contents of the IP header

    QL$FAO(%STRING("!%T !AS IP packet, SEG=!XL, DATA=!XL, Header size !SL!/",
		   "!_CKsum:!_!SL!/"),
	    0,sptr,seg,segdata,Header_Size,
	   seghdr->IPH$CheckSum);

    }

//SBTTL "IPCB_Find - look up IP control block"
IPCB_Find(Src$Adrs,Dst$Adrs,Protocol)
    {
    signed long
	Ucount,
	IPCBIX;
	struct IPCB_Structure * IPCB;

    Ucount = IPCB_Count;
    IPCBIX = 1;
    while ((Ucount > 0) && (IPCBIX <= Max_IPCB))
	{
	if ((IPCB = IPCB_Table[IPCBIX]) != 0)
	    {
	    IF  ((IPCB->IPCB$Host_Filter == WILD) OR
		(IPCB->IPCB$Host_Filter == Src$Adrs)) AND
		((IPCB->IPCB$Proto_Filter == WILD) OR
		(IPCB->IPCB$Proto_Filter == Protocol)) THEN
		return IPCB;
	    Ucount = Ucount-1;
	    };
	IPCBIX = IPCBIX + 1;
	};
    return 0;
    }

//SBTTL "IP input handler"
/*
    Come here at AST level when input packet is determined to be IP packet.
    At present, all IP input handling is done at AST level, so we search
    the IPCB list and queue the IP packet for deliver here.
*/

    Queue_User_IP();

void IPU$User_Input ( Src$Adrs,Dst$Adrs,Protocol,
				BufSize,Buf,SegSize,Seg )
	struct IP_Structure * Seg;
{
    signed long
	Uptr,
	Ucount,
	IPCBIX,
	sum,
      delete;
	struct IPCB_Structure * IPCB;
    LABEL
	X;

// Assume this packet should be deleted

    delete = TRUE;

// Log the IP packet if desired

    if ($$LOGF(LOG$IP))
	Log_IP_Packet(Seg,TRUE,FALSE);

//!!HACK!!// I deleted this.  It should be done 

// Try to match the input packet up with a IPCB
//!!HACK!!// What if there's more than one IPCB for this address?
    IPCB = IPCB_Find ( Src$Adrs , Dst$Adrs , Protocol );
    if (IPCB == 0)
	{
//!!HACK!!// Don"t worry if there"e no IPCB.
	if ($$LOGF(LOG$IP))
	    QL$FAO("!%T No IPCB found for segment !XL!/",0,Seg);
	}
    else
X:	{

// Log that it was found

	if ($$LOGF(LOG$IP))
	    QL$FAO("!%T IPCB !XL found for IP Seg !XL!/",
		   0,IPCB,Seg);

// Make sure the IPCB isn't aborted...

	if (IPCB->IPCB$Aborting)
	    {
	    XQL$FAO(LOG$IP,"!%T IP input !XL for aborted IPCB !XL dropped!/",
		    0,Seg,IPCB);
	    LEAVE X;
	    };

// Give the segment to the user now.

	delete = Queue_User_IP(IPCB,Seg,SegSize,Buf,Bufsize,0);
	};

    }


//SBTTL "Queue_User_IP - Queue up IP packet for delivery to user"
/*
    Called by IP_Input at AST level when an input packet matches a
    user IP "connection". Function of this routine is to either deliver
    the IP packet to the user (if a user read request is available) or
    queue it for later deliver.
    Returns TRUE if the IP packet has been fully disposed of (i.e. the
    caller may deallocate the packet), FALSE otherwise (i.e. the packet
    has been placed on a queue and may not be deallocated yet).
 */

 void    DELIVER_IP_DATA();

Queue_User_IP(IPCB,Uptr,Usize,Buf,Bufsize,QB)
	struct IPCB_Structure * IPCB;
	struct queue_blk_structure(QB_NR_Fields) * QB;
    {
    signed long
	Buf2,
	QBR;
extern	MM$QBlk_Get();
    LITERAL
	IPCB$NR_Qmax = 5;	// Max input packets permitted on input queue

// See if the input queue is full for this IPCB

    if (IPCB->IPCB$NR_Qcount > IPCB$NR_Qmax)
	{
	if ($$LOGF(LOG$IP))
	    QL$FAO("!%T IP at !XL dropped - IPCB NR queue full!/",0,Uptr);
	return TRUE;		// Drop the packet - no room
	};

// We need to make a copy of this IP datagram.

    Buf2 = MM$Seg_Get(Bufsize);	// Get a buffer
    Uptr = Buf2 + (Uptr - Buf);
//!!HACK!!// There's no need to copy the whole buffer, only Usize worth...
    MOVBYT(Bufsize,Buf,Buf2);

// Allocate a queue block and insert onto user receive queue

    if (QB == 0)
	QB = MM$QBLK_Get();
    QB->NR$Buf_Size = Bufsize;	// Total size of network buffer
    QB->NR$Buf = Buf2;		// Pointer to network buffer
    QB->NR$Ucount = Usize;	// Length of the data
    QB->NR$Uptr = Uptr;	// Pointer to the data

// If there is a user read outstanding, deliver data, else queue for later

    if (REMQUE(IPCB->IPCB$USR_Qhead,QBR) != Empty_Queue)
	Deliver_IP_Data(IPCB,QB,QBR)
    else
	INSQUE(QB,IPCB->IPCB$NR_Qtail);

    return TRUE;		// Go ahead and deallocate this segment...
    }

//SBTTL "Deliver_IP_Data - Deliver IP data to user"
/*
    Perform actual delivery of IP packet to a user request.
    IP packet is copied into the user buffer and the user I/O request
    is posted.
 */

void Deliver_IP_Data(IPCB,QB,URQ)
	struct IPCB_Structure * IPCB;
	struct queue_blk_structure(QB_NR_Fields) * QB;
 	struct queue_blk_structure(QB_UR_Fields) * URQ;
    {
    signed long
	FLAGS,
	ICMTYPE,
	struct $BBLOCK * IRP[],
	struct User_RECV_Args * UArgs,
	struct User_RECV_Args * Sargs,
	Aptr : IPADR$ADDRESS_BLOCK,
	struct IP_Structure * Uptr,
	Ucount;

// Determine data start and data count

//!!HACK!!// come back here...

    Ucount = QB->NR$Ucount;
    Uptr = QB->NR$Uptr;

// Truncate to user receive request size

    if (Ucount > URQ->UR$Size)
	Ucount = URQ->UR$Size;

    if ($$LOGF(LOG$IP))
	QL$FAO("!%T Posting IP receive,Size=!SL,IPCB=!XL,IRP=!XL,UCB_A=!XL!/",
	       0,Ucount,IPCB,URQ->UR$IRP_Adrs,URQ->UR$UCB_Adrs);

// Copy from our buffer to the user system buffer

    $$KCALL(MOVBYT,Ucount,Uptr,URQ->UR$Data);

    UArgs = URQ->UR$UArgs;

// Copy IP Source and destination addresses to system space Diag Buff
// First, get the SysBlk address out of the IRP, then copy the Header
// block from our local copy of UArgs.

    if (Uargs->RE$PH_Buff != 0)
	{
	IRP = URQ->UR$IRP_Adrs;
	Aptr->IPADR$SRC_HOST = Uptr->IPH$Source;
	Aptr->IPADR$DST_HOST = Uptr->IPH$Dest;
	Aptr->IPADR$EXT1 = (Uptr)<0,32,0>;	// First long of IP header
	Aptr->IPADR$EXT2 = (Uptr+8)<0,32,0>;	// Third long of IP header
	$$KCALL(MOVBYT,IPADR$ADDRESS_BLEN,
		Aptr,Uargs->RE$PH_Buff);
	};

// Post the I/O and free up memory

    User$Post_IO_Status(URQ->UR$Uargs,SS$_NORMAL,
			.Ucount,0,0);
    MM$UArg_Free(URQ->UR$Uargs);

    MM$QBLK_Free(URQ);
    MM$Seg_Free(QB->NR$Buf_Size,QB->NR$Buf);
    MM$QBLK_Free(QB);
    }

//SBTTL "IPCB_OK - Match connection ID to IPCB address"

IPCB_OK(Conn_ID,RCaddr,struct User_Default_Args * Uargs)
    {
	struct IPCB_Structure * IPCB;
    MACRO
	IPCBERR(EC) = (RCaddr = EC; return 0) %;

// Range check the connection id. This should never fail, since the user should
// not be fondling connection IDs.

    if ((Conn_ID <= 0) || (Conn_ID > MAX_IPCB))
	IPCBERR(NET$_CDE);	// Nonexistant connection ID
    IPCB = IPCB_Table[Conn_ID];

// Make sure the table had something reasonable for this connection ID

    if (IPCB <= 0)
	IPCBERR(NET$_CDE);	// IPCB has been deleted (possible)

// Check consistancy of IPCB back-pointer into table

    IF (IPCB->IPCB$IPCBID != Conn_ID) OR
       (IPCB->IPCB$UCB_ADRS != Uargs->UD$UCB_Adrs) THEN
	IPCBERR(NET$_CDE);	// Confusion (can this happen?)

// Everything is good - return the IPCB address

    return IPCB;
    }

//SBTTL "IPCB_Get - Allocate and initialize one IPCB"

IPCB_Get(IDX)
    {
extern	LIB$GET_VM();
extern	LIB$GET_VM_PAGE();
	struct IPCB_Structure * IPCB;
    signed long
	IPCBIDX,
	RC ;
    LABEL
	X;

// Find a free slot in the IPCB table

X:  {			// ** Block X **
    IPCBIDX = 0;
    for (I=1;I<=MAX_IPCB;I++)
	if (IPCB_Table[I] == 0)
	    LEAVE X WITH (IPCBIDX = I);
    return 0;			// Failed to allocate a IPCB
    }			// ** Block X **

// Allocate some space for the IPCB

//    LIB$GET_VM(%REF(IPCB_Size*4),IPCB);
    RC = LIB$GET_VM_PAGE(%REF(((IPCB_Size * 4) / 512) + 1),IPCB);
    if (NOT RC)
	FATAL$FAO("IPCB_GET - LIB$GET_VM failure, RC=!XL",RC);

// Clear it out and set it in the table

    IPCB_Table[IPCBIDX] = IPCB;
    CH$FILL(%CHAR(0),IPCB_Size*4,IPCB);
    IPCB_Count = IPCB_Count+1;

// Initialize queue headers for the IPCB

    IPCB->IPCB$NR_Qhead = IPCB->IPCB$NR_Qtail = IPCB->IPCB$NR_Qhead;
    IPCB->IPCB$USR_Qhead = IPCB->IPCB$USR_Qtail = IPCB->IPCB$USR_Qhead;

// Set the connection ID

    IPCB->IPCB$IPCBID = IPCBIDX;

// Return the pointer

    IDX = IPCBIDX;
    return IPCB;
    }

//SBTTL "IPCB_Free - Deallocate a IPCB"

void IPCB_Free(IPCBIX,struct IPCB_Structure * IPCB)
    {
extern	LIB$FREE_VM();
extern	LIB$FREE_VM_PAGE();

    signed long
	RC ;

// Clear the table entry

    IPCB_Table[IPCBIX] = 0;

// Free the memory and decrement our counter.

//    LIB$FREE_VM(%REF(IPCB_Size*4),IPCB);
    RC = LIB$FREE_VM_PAGE(%REF(((IPCB_Size * 4) / 512) + 1),IPCB);
    if (NOT RC)
	FATAL$FAO("IPCB_FREE - LIB$FREE_VM failure, RC=!XL",RC);
    IPCB_Count = IPCB_Count-1;
    }

//SBTTL "Kill_IP_Requests - purge all I/O requests for a connection"

void Kill_IP_Requests(struct IPCB_Structure * IPCB,RC)
    {
	struct queue_blk_structure(QB_UR_Fields) * URQ;
	struct queue_blk_structure(QB_NR_Fields) * QB;

// Make sure we aren't doing this more than once
//
//   if (IPCB->IPCB$Aborting)
//	RETURN;

// Say that this connection is aborting (prevent future requests)

    IPCB->IPCB$Aborting = TRUE;

// Cancel any name lookup in progess

    if (IPCB->IPCB$NMLOOK)
	{
	NML$CANCEL(IPCB, 0, 0);
	IPCB->IPCB$NMLOOK = FALSE;
	};

// Kill any pending open

    NOINT;
    if (IPCB->IPCB$UARGS != 0)
	{
	USER$Err(IPCB->IPCB$UARGS,RC);
	IPCB->IPCB$UARGS = 0;
	};
    OKINT;

// Purge the user request queue, posting all requests

    while (REMQUE(IPCB->IPCB$USR_Qhead,URQ) != Empty_Queue)
	{
	User$Post_IO_Status(URQ->UR$Uargs,RC,0,0,0);
	MM$UArg_Free(URQ->UR$Uargs);
	MM$QBlk_Free(URQ);	
	};

// Purge any received qblocks as well

    while (REMQUE(IPCB->IPCB$NR_Qhead,QB) != Empty_Queue)
	{
	MM$Seg_Free(QB->NR$Buf_Size,QB->NR$Buf);
	MM$QBlk_Free(QB);
	};
    }

//SBTTL "IPCB_Close - Close/deallocate a IPCB"

void IPCB_Close(UIDX,struct IPCB_Structure * IPCB,RC)
    {
    Kill_IP_Requests(IPCB,RC);
    IPCB_FREE(UIDX,IPCB);
    }

void IPCB_Abort(struct IPCB_Structure * IPCB,RC)
//
// Abort a IPCB - called by IP code.
//
    {
    IPCB_CLOSE(IPCB->IPCB$IPCBID,IPCB,RC);
    }


//SBTTL "IPU$Purge_All_IO - delete IP database before network exits"

void IPU$Purge_All_IO (void)
    {
    signed long
	IPCBIDX;
	struct IPCB_Structure * IPCB;

// Loop for all connections, purge them, and delete them.

    for (IPCBIDX=1;IPCBIDX<=MAX_IPCB;IPCBIDX++)
	if ((IPCB = IPCB_Table[IPCBIDX]) != 0)
	    IPCB_Close(IPCBIDX,IPCB,NET$_TE);
    }


//SBTTL "IPU$OPEN - open a user IP "connection""
/*
    Open an IP "connection". Create a IP Control Block, which serves as a
    place to hang incoming packets and user receive requests.
 */

 void    IP_NMLOOK_DONE();
 void    IP_ADLOOK_DONE();

void IPU$OPEN(struct User_Open_Args * Uargs)
    {
	struct IPCB_Structure * IPCB;
    signed long
	IPADDR,
	NAMLEN,
	NAMPTR,
	UIDX,
	IPCBPTR,
	Args : VECTOR[4];
    LABEL
	X;

    XLOG$FAO(LOG$USER,"!%T IPU$OPEN: PID=!XL,CHAN=!XW,FLAGS=!XL X1=!XL!/",
	     0,Uargs->OP$PID,Uargs->OP$PIOchan,Uargs->OP$FLAGS,
	     UArgs->OP$Ext1);

// First create a IPCB for this connection.

    if ((IPCB = IPCB_Get(UIDX)) <= 0)
	{
	USER$Err(Uargs,NET$_UCT);
	RETURN;
	};

// Initialize user mode values

    IPCB->IPCB$UCB_ADRS = Uargs->OP$UCB_Adrs;
    IPCB->IPCB$User_ID = Uargs->OP$PID;
    IPCB->IPCB$PIOchan = Uargs->OP$PIOchan;

// At this point, the connection exists. Write the connection ID
// back into the Unit Control Block for this connection.

    IPCBptr = Uargs->OP$UCB_Adrs + UCB$L_CBID;
    $$KCALL(MOVBYT,4,UIDX,IPCBptr);

// Initialize queue headers for the IPCB

    IPCB->IPCB$NR_Qhead = IPCB->IPCB$NR_Qtail = IPCB->IPCB$NR_Qhead;
    IPCB->IPCB$USR_Qhead = IPCB->IPCB$USR_Qtail = IPCB->IPCB$USR_Qhead;

// Copy user arguments to IPCB

    IPCB->IPCB$Uargs = Uargs;
    IPCB->IPCB$Host_Filter = Uargs->OP$Src_Host;
    IPCB->IPCB$Proto_Filter = Uargs->OP$EXT1;

// Handle wildcard host

    NAMPTR = CH$PTR(Uargs->OP$Foreign_Host);
    NAMLEN = Uargs->OP$Foreign_Hlen;
    if ((NOT Uargs->OP$ADDR_FLAG) && (NAMLEN == 0))
	{
	IPCB->IPCB$Wildcard = TRUE;
	IPCB->IPCB$Foreign_Host = WILD;
	IPCB->IPCB$Foreign_Hnlen = 0;
	IP_NMLOOK_DONE(IPCB,SS$_NORMAL,0,0,0,0);
	RETURN;
	};

// Check for supplied IP address instead of name

X:  {			// *** Block X ***
    if (Uargs->OP$ADDR_FLAG)
	IPADDR = Uargs->OP$Foreign_Address
    else
	if (GET_IP_ADDR(NAMPTR,IPADDR) LSS 0)
	    LEAVE X;
    IPCB->IPCB$Foreign_Hnlen = 0;
    IPCB->IPCB$Foreign_Host = IPADDR;
    IP_NMLOOK_DONE(IPCB,SS$_NORMAL,1,IPADDR,0,0);
    IPCB->IPCB$NMLook = TRUE;
    NML$GETNAME(IPADDR,IP_ADLOOK_DONE,IPCB);
    RETURN;
    }			// *** Block X ***

// "standard" case, host name is supplied - start name lookup for it

    IPCB->IPCB$NMLook = TRUE;
    NML$GETALST(NAMPTR,NAMLEN,IP_NMLOOK_DONE,IPCB);
    }



//SBTTL "IP_NMLOOK_DONE - Second phase of IPU$OPEN when namelookup done"
/*
    Come here when the foreign host name has been resolved.
    At this point, we set the local & foreign hosts in the IPCB
    and post the users open request.
*/

void IP_NMLOOK_DONE(IPCB,STATUS,ADRCNT,ADRLST,NAMLEN,NAMPTR)
    {
    signed long
    RC;
	struct User_Open_Args * Uargs,
	struct NetIO_Status_Block * IOSB ;
    MACRO
	UOP_ERROR(EC) = 
	    {
	    USER$Err(Uargs,EC);
	    IPCB_FREE(IPCB->IPCB$IPCBID,IPCB);
	    RETURN;
	    } %;

// Clear name lookup flag and get uargs

    NOINT;
    IPCB->IPCB$NMLook = FALSE;
    Uargs = IPCB->IPCB$Uargs;
    IPCB->IPCB$Uargs = 0;
    OKINT;

// Check status of the name lookup

    if (NOT STATUS)
	UOP_ERROR(STATUS);

    // Finish up the open

//    if (ADRCNT > 0)
//	IP$SET_HOSTS(ADRCNT,ADRLST,IPCB->IPCB$Local_Host,
//		     IPCB->IPCB$Foreign_Host);

    // Done at last - log success

    XLOG$FAO(LOG$USER,"!%T UDB_OPEN: Conn idx = !XL, IPCB = !XL!/",
	     0,IPCB->IPCB$IPCBID,IPCB);

// Verify that we have access to the host set

//!!HACK!!// Should we do this or not??
//    RC = USER$CHECK_ACCESS(IPCB->IPCB$USER_ID,IPCB->IPCB$Local_Host,
//		      0,IPCB->IPCB$Foreign_Host,0);
//    if (NOT RC)
//	UOP_ERROR(RC);

// Set the foreign host name in the IPCB

    IPCB->IPCB$Foreign_Hnlen = NAMLEN;
    if (NAMLEN != 0)
	CH$MOVE(NAMLEN,NAMPTR,CH$PTR(IPCB->IPCB$Foreign_Hname));

// Finally, post the status

    IOSB->NSB$STATUS = SS$_NORMAL; // Success return
    IOSB->NSB$Byte_Count = 0;
    IOSB->NSB$XSTATUS = 0;
    IO$POST(IOSB,Uargs);
    MM$UArg_Free(Uargs);
    }



//SBTTL "IP_ADLOOK_DONE - Finish IP address to name lookup"

void IP_ADLOOK_DONE(IPCB,STATUS,NAMLEN,NAMPTR)
	struct IPCB_Structure * IPCB;
    {

// Clear pending name lookup flag

    IPCB->IPCB$NMLook = FALSE;

// Check status

    if (NOT STATUS)
	RETURN;

// Copy the hostname into the IPCB

    IPCB->IPCB$Foreign_Hnlen = NAMLEN;
    CH$MOVE(NAMLEN,NAMPTR,CH$PTR(IPCB->IPCB$Foreign_Hname));
    }



//SBTTL "IPU$CLOSE - close IP "connection""
/*
    Close an IP "connection". Kills any receive requests that haven't
    finished yet and deallocates the IPCB and any other data structures
    associated with a connection.
*/

void IPU$CLOSE(struct User_Close_Args * Uargs)
    {
struct IPCB_Structure * IPCB;
    signed long
	RC;

// Check for valid IPCB

    if ((IPCB = IPCB_OK(Uargs->CL$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);
	RETURN;
	};

// Use common routine for closing

    IPCB_Close(Uargs->CL$Local_Conn_ID,IPCB,NET$_CC);

// Close done - post user request and free argblk

    User$Post_IO_Status(Uargs,SS$_NORMAL,0,0,0);
    MM$UArg_Free(Uargs);
    }

//SBTTL "IPU$ABORT - abort IP "connection""
/*
    Abort a IP "connection". Identical in functionality to IPU$CLOSE.
 */

void IPU$ABORT(struct User_Abort_Args * Uargs)
    {
	struct IPCB_Structure * IPCB;
    signed long
	RC;

// Check for valid IPCB

    if ((IPCB = IPCB_OK(Uargs->AB$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);
	RETURN;
	};

// Use common routine for closing

    IPCB_Close(Uargs->AB$Local_Conn_ID,IPCB,NET$_CC);

// Done. Clean up.

    User$Post_IO_Status(Uargs,SS$_NORMAL,0,0,0);
    MM$UArg_Free(Uargs);
    }

//SBTTL "IPU$S} - send IP packet"
/*
    Handle user send request for IP connection. Form a IP packet from the
    user's data buffer and hand it to IP layer for transmission.
 */

void IPU$S}(struct User_Send_Args * Uargs)
    {
    signed long
	RC,
	Flags,
	Bufsize,
	Buf,
	LocalAddr, ForeignAddr, Protocol,
	Segsize,
	USize;
	struct IP_Structure * Seg;
	struct IPCB_Structure * IPCB;


// Validate connection ID and get IPCB pointer

    if ((IPCB = IPCB_OK(Uargs->SE$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// No such connection
	RETURN;
	};

//!!HACK!!// Does this size arg mean  anything?
    XLOG$FAO(LOG$USER,"!%T IP$S}: Conn=!XL, IPCB=!XL, Size=!SL!/",
	     0,Uargs->SE$Local_Conn_ID,IPCB,Uargs->SE$Buf_size);

// Check for aborted connection

    if (IPCB->IPCB$Aborting)
	{
	XLOG$FAO(LOG$USER,"!%T IPU$S} for aborted IPCB !XL!/",0,IPCB);
	USER$Err(Uargs,NET$_CC);
	RETURN;
	};

// Check for invalid buffer size

    if (Uargs->SE$Buf_Size LSS 0)
	{
	USER$Err(Uargs,NET$_BTS);
	RETURN;
	};


//!!HACK!!// Where's the comment?
    Flags = Uargs->SE$Flags;

// Allocate an output buffer and build an IP packet
//!!HACK!!// This is silly, why not just use the uarg block.
//!!HACK!!// Not possible now, but maybe with a little work...

    // Calc total size of IP packet.  Note: Uargs->SE$EXT2 is header size.
    USize = Uargs->SE$Buf_size;
    Segsize = Uargs->SE$Buf_size + Uargs->SE$EXT2;
//    if (SegSize > Max_IP_Data_Size)
//	SegSize = Max_IP_Data_Size;

// Use preallocated buffer sizes to reduce dynamic memory load

    bufsize = SegSize + Device_header;
    if (bufsize <= MIN_PHYSICAL_BUFSIZE)
	bufsize = MIN_PHYSICAL_BUFSIZE
    else
	if (bufsize <= MAX_PHYSICAL_BUFSIZE)
	    bufsize = MAX_PHYSICAL_BUFSIZE;
    Buf = MM$Seg_Get(Bufsize);	// Get a buffer
//!!HACK!!// Next line is a hack, but it really speeds things up...
    Seg = Buf + device_header; // Point at IP segment

// Copy the user data into the data area

    $$KCALL(MOVBYT,SegSize,Uargs->SE$Data_Start,Seg);

    ForeignAddr = Seg->IPH$Dest;
    LocalAddr = Seg->IPH$Source;
    Protocol = Seg->IPH$Protocol;

    // Use IP$S}_RAW if this is an exact packet
    if (Flags<0,1,0>)
	{
	// Send packet exactly as the client passed it.

	// Re-arrange bytes and words in IP header
	SwapBytes ( IP_hdr_swap_size , Seg );

	// Compute checksum for IP header
	if (Flags<0,2,0>)
	    {
	    Seg->IPH$Checksum = 0;
	    Seg->IPH$Checksum = Calc_Checksum ( Uargs->SE$EXT2 , Seg )
	    };

	if ($$LOGF(LOG$IP))
	    Log_IP_Packet(Seg,FALSE,TRUE);

	RC = SS$_NORMAL;
	IF (IP$S}_RAW(Seg->IPH$Dest,Seg,SegSize,1,
			.Buf,Bufsize) == 0) THEN RC = NET$_NRT;
	// Post the I/O request back to the user

	User$Post_IO_Status(Uargs,RC,0,0,0);
	MM$UArg_Free(Uargs);
	RETURN
	};

    // Compute Foreign address, source address, and protocol.
    if (ForeignAddr == WILD)
	ForeignAddr = IPCB->IPCB$Foreign_Host;
    if ((ForeignAddr == WILD))
	{
	MM$Seg_FREE(BufSize,Buf);	// Give back buffer
	USER$Err(Uargs,NET$_NOPN);
	return 0
	};


    if (LocalAddr == WILD)
	IP$SET_HOSTS(1,ForeignAddr,LocalAddr,ForeignAddr);

    if (Protocol == WILD)
	Protocol = IPCB->IPCB$Proto_Filter;

    if ($$LOGF(LOG$IP))
	Log_IP_Packet(Seg,FALSE,TRUE);

// Send the segment to IP (it will deallocate it)

    IPIPID = IPIPID+1;	// Increment packet ID
    RC = SS$_NORMAL;
    IF (IP$S}(LocalAddr,ForeignAddr,IPTOS,IPTTL,
		   Seg + UArgs->SE$EXT2,USize,
		   IPIPID,IPDF,TRUE,Protocol,
		   Buf,Bufsize) == 0) THEN RC = NET$_NRT;

// Post the I/O request back to the user

    User$Post_IO_Status(Uargs,RC,0,0,0);
    MM$UArg_Free(Uargs);
    }



//SBTTL "IPU$RECEIVE - receive a IP packet"
/*
    Handle user receive request for IP connection. If there is a packet
    available on the IP receive queue, then deliver it to the user
    immediately. Otherwise, queue up the user receive for later.
 */

void IPU$RECEIVE(struct User_Recv_Args * Uargs) 
    {
	struct IPCB_Structure * IPCB;
	struct queue_blk_structure(QB_NR_Fields) * QB;
	struct queue_blk_structure(QB_UR_Fields) * URQ;
    signed long
	RC;

// Validate connection ID and get IPCB pointer

    if ((IPCB = IPCB_OK(Uargs->RE$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// No such connection
	RETURN;
	};
    XLOG$FAO(LOG$USER,"!%T IPU$RECEIVE: Conn=!XL, IPCB=!XL, Size=!SL!/",
	     0,Uargs->RE$Local_Conn_ID,IPCB,Uargs->RE$Buf_size);

// Check for aborted connection

    if (IPCB->IPCB$Aborting)
	{
	XLOG$FAO(LOG$USER,"!%T IPU$RECEIVE for aborted IPCB !XL!/",0,IPCB);
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
    if (REMQUE(IPCB->IPCB$NR_Qhead,QB) != Empty_Queue)
	Deliver_IP_Data(IPCB,QB,URQ)
    else
	INSQUE(URQ,IPCB->IPCB$USR_Qtail);
    OKINT;
    }



//SBTTL "IPU$INFO - get info about IP "connection""
/*
    Read the host names/numbers for a IP connection.
 */

void IPU$INFO(struct User_Info_Args * Uargs)
    {
extern	USER$Net_Connection_Info ();
	struct IPCB_Structure * IPCB;
    signed long
	RC;

// Validate the connection ID

    if ((IPCB = IPCB_OK(Uargs->IF$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Bad connection ID
	RETURN;
	};

// Give the information back (common TCP/IP routine in USER.BLI)

    USER$Net_Connection_Info(Uargs,IPCB->IPCB$Host_Filter,
			IPCB->IPCB$Foreign_Host,
			0,0,
			IPCB->IPCB$Foreign_Hname,
			IPCB->IPCB$Foreign_Hnlen);
    }


//SBTTL "IPU$STATUS - get status of IP "connection""
/*
    This routine is a placeholder for the network STATUS command, which is
    currently implemented for the TCP protocol.
 */

void IPU$STATUS(struct User_Status_Args * Uargs)
    {
    USER$Err(Uargs,NET$_NYI);
    }

//SBTTL "IPU$CANCEL - Handle VMS cancel for IP connection"
/*
    Handle process abort/$CANCEL request for a IP connection. Identical
    in functionality to IPU$CLOSE/IPU$ABORT except for calling procedure.
 */

IPU$CANCEL(struct VMS$Cancel_Args * Uargs)
    {
	struct IPCB_Structure * IPCB,
    signed long
	Fcount;

    Fcount = 0;

// Check all valid IPCB's looking for a match on pid and channel #.

    for (I=1;I<=MAX_IPCB;I++)
	if ((IPCB = IPCB_Table[I]) != 0)
	    {

// If the process doing the cancel owns this connection, then delete it.

	    IF (IPCB->IPCB$User_ID == Uargs->VC$PID) AND
	       (IPCB->IPCB$PIOchan == Uargs->VC$PIOchan) THEN
		{
		XLOG$FAO(LOG$USER,"!%T IPU$Cancel: IPCB=!XL!/",0,IPCB);
		IPCB_Close(I,IPCB,NET$_ccan);
		Fcount = Fcount + 1;
		};
	    };
    return Fcount;
    }

//SBTTL "IP dump routines"

void IPU$Connection_List(RB)
//
// Dump out the list of IP connections.
//
	struct D$IP_List_Return_Blk * RB;
    {
    signed long
	RBIX;
    RBIX = 1;
    for (I=1;I<=MAX_IPCB-1;I++)
	if (IPCB_TABLE[I] != 0)
	    {
	    RB[RBIX] = I;
	    RBIX = RBIX + 1;
	    };
    RB[0] = RBIX - 1;
    }

IPU$IPCB_Dump(IPCBIX,RB)
//
// Dump out a single IP connection
//
	struct D$IPCB_Dump_Return_BLK * RB;
    {
	struct IPCB_Structure * IPCB;
    signed long
	QB,
	Qcount;

// Validate that there is a real IPCB there

    IF (IPCBIX LSS 1) || (IPCBIX > MAX_IPCB) OR
       ((IPCB = IPCB_TABLE[IPCBIX]) == 0) THEN
	return FALSE;

// Copy the IPCB contents

    RB->DU$IPCB_Address = IPCB;
    RB->DU$IPCB_Foreign_Host = IPCB->IPCB$Foreign_Host;
    RB->DU$IPCB_Local_Host = IPCB->IPCB$Host_Filter;
    RB->DU$IPCB_Flags = IPCB->IPCB$Flags;
    RB->DU$IPCB_User_ID = IPCB->IPCB$User_ID;

// Get length of network receive queue

    QB = IPCB->IPCB$NR_Qhead;
    Qcount = 0;
    while ((QB NEQA IPCB->IPCB$NR_Qhead))
	{
	MAP
	    struct queue_blk_structure(QB_NR_Fields) * QB;
	Qcount = Qcount + 1;
	QB = QB->NR$NEXT;
	};
    RB->DU$IPCB_NR_Qcount = Qcount;

// Get length of user receive queue

    QB = IPCB->IPCB$USR_Qhead;
    Qcount = 0;
    while ((QB NEQA IPCB->IPCB$USR_Qhead))
	{
	MAP
	    struct queue_blk_structure(QB_UR_Fields) * QB;
	Qcount = Qcount + 1;
	QB = QB->UR$NEXT;
	};
    RB->DU$IPCB_UR_Qcount = Qcount;

// Done.

    return TRUE;
    }
