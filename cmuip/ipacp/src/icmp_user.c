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

#if 0
MODULE ICMP_User (IDENT="1.0c",LANGUAGE(BLISS32),
	    ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			    NONEXTERNAL=LONG_RELATIVE),
	    LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	    OPTIMIZE,OPTLEVEL=3,ZIP)=

#endif

// Include standard definition files

//LIBRARY "SYS$LIBRARY:STARLET";
// not yet#include "SYS$LIBRARY:LIB";
#include <cmuip/central/include/neterror.h>
//not yet#include "CMUIP_SRC:[CENTRAL]NETXPORT";
#include "netvms.h"
#include <cmuip/central/include/netcommon.h>
#include <cmuip/central/include/nettcpip.h>	// IP & ICMP definitions
#include "structure.h"
#include "tcpmacros.h"
#include "snmp.h"

#include <ssdef.h>
#include <descrip.h>

//*** Special literals from USER.BLI ***

#if 0
extern signed long 
    UCB$Q_DDP,
    UCB$L_CBID,
    UCB$L_EXTRA;
#endif

// External data items

extern signed long
    intdf,
    ast_in_progress,
    log_state,
    min_physical_bufsize,
    max_physical_bufsize;

 extern  struct  ICMP_MIB_struct * icmp_mib ;	// ICMP Management Information Block


// External routines

// MACLIB.MAR

extern  void    swapbytes();
extern  void    MOVBYT();
extern     Calc_Checksum();

// MEMGR.BLI

extern  void    mm$uarg_free();
extern     mm$qblk_get();
extern  void    mm$qblk_free();
extern     mm$seg_get();
extern  void    mm$seg_free();

// USER.BLI

extern     USER$CHECK_ACCESS();
extern     USER$Err();
extern  void    IO$POST();
extern  void    user$post_io_status();

// IP.BLI

extern  void    IP$SET_HOSTS();
extern     ip$send();

// NMLOOK.BLI

extern  void    NML$CANCEL();
extern  void    NML$GETALST();
extern  void    NML$GETNAME();

// IOUTIL.BLI

extern     GET_IP_ADDR();
extern  void    ASCII_DEC_BYTES();
extern  void    ASCII_HEX_BYTES();
extern  void    LOG_FAO();
extern  void    QL_FAO();


//SBTTL "ICMP data structures"

// Define the "ICMPCB" - ICMP analogue of TCB.

struct  ICMPCB_Structure
    {
unsigned long     ICMPCB$Foreign_Host	;	// ICMP foreign host number
unsigned long     ICMPCB$Local_Host	;	//     local host
char    ICMPCB$Foreign_Hname[MAX_HNAME];
short    ICMPCB$Foreign_Hnlen;
void *     ICMPCB$USR_Qhead	;	// User receive request queue
void *     ICMPCB$USR_Qtail	;
void *     ICMPCB$NR_Qhead	;	// Net receive queue
void *     ICMPCB$NR_Qtail	;
short    ICMPCB$NR_Qcount;
union {
short    ICMPCB$Flags;
struct {
unsigned 	ICMPCB$Wildcard	 : 1;	// ICMPCB opened with wild FH/FP/LH
  //unsigned 	ICMPCB$Addr_Mode	 : 1;	// User wants IP addresses
unsigned 	ICMPCB$Aborting	 : 1;	// ICMPCB is closing
unsigned 	icmpcb$nmlook	 : 1;	// ICMPCB has an outstanding name lookup
unsigned 	ICMPCB$Internal	 : 1;	// ICMPCB is open by ACP not user
};
};
void *     icmpcb$icmpcbid		;	// ICMPCB_Table index for this connection
void *     ICMPCB$UCB_ADRS	;	// Connection UCB address
void *     icmpcb$uargs	;	// Uarg block in pending open
long    ICMPCB$User_ID	;	// Process ID of owner
short    ICMPCB$PIOchan	;	// Process IO channel
    };


#define    ICMPCB_Size sizeof(struct  ICMPCB_Structure)
#if 0
MACRO
    ICMPCB_Structure = BLOCK->ICMPCB_Size FIELD(ICMPCB_Fields) %;
//MESSAGE(%NUMBER(ICMPCB_Size)," longwords per ICMPCB")
#endif


//SBTTL "ICMP data storage"

signed long
    icmpttl	 = 32;	// TTL for ICMP

static signed long
    ICMPIPID  = 1,	// Current IP packet ID
    ICMPCB_Count  = 0,	// Count of active ICMPCBs
    icmpcb_table[MAX_ICMPCB+1];// Table of ICMPCBs



//SBTTL "ICMP packet logger"
/*
    Queue up a log entry to dump out a ICMP packet.
 */

void Log_ICMP_Packet(Seg,SwapFlag,SendFlag)
	struct icmp_header * Seg;
    {
struct dsc$descriptor sptr;
    signed long
	segdata;
	struct icmp_header segcopy_ , * segcopy = &segcopy_;
	struct icmp_header * seghdr;

    seghdr = Seg;		// Point at segment header
    segdata = (long)Seg + ICMP_HEADER_SIZE;
    if (SwapFlag)		// Need to byteswap header?
	{
	CH$MOVE(ICMP_HEADER_SIZE,CH$PTR(Seg,0),CH$PTR(segcopy,0)); // Make a copy
	seghdr = segcopy;	// Point at this version...
	swapbytesicmphdr(ICMP_HEADER_SIZE/2,seghdr); // Swap header bytes
	};

// Print first part of info

    if (SendFlag)
	sptr = ASCID("Sent");
    else
	sptr = ASCID("Received");

// Log the contents of the ICMP header

    QL$FAO(/*%STRING*/("!%T !AS ICMP packet, SEG=!XL, DATA=!XL!/",
		   "!_CKsum:!_!SL!/"),
	    0,sptr,Seg,segdata,
	   seghdr->icm$cksum);

    }

//SBTTL "ICMPCB_Find - look up ICMP control block"

ICMPCB_Find(Src$Adrs)
    {
    signed long
	Ucount,
	ICMPCBIX;
	struct ICMPCB_Structure * ICMPCB;
    Ucount = ICMPCB_Count;
    ICMPCBIX = 1;
    while ((Ucount > 0) && (ICMPCBIX <= MAX_ICMPCB))
	{
	if ((ICMPCB = icmpcb_table[ICMPCBIX]) != 0)
	    {
	    if (((ICMPCB->ICMPCB$Foreign_Host == WILD) ||
		(ICMPCB->ICMPCB$Foreign_Host == Src$Adrs)))
		return ICMPCB;
	    Ucount = Ucount-1;
	    };
	ICMPCBIX = ICMPCBIX + 1;
	};
    return 0;
    }

//SBTTL "ICMP input handler"
/*
    Come here at AST level when input packet is determined to be ICMP packet.
    At present, all ICMP input handling is done at AST level, so we search
    the ICMPCB list and queue the ICMP packet for deliver here.
*/

    Queue_User_ICMP();

void icmp$user_input(Src$Adrs,Dest$Adrs,bufsize,Buf,SegSize,Seg)
	struct icmp_header * Seg;
    {
    signed long
	Buf2,
	Ucount,
	ICMPCBIX,
	sum,
	delete;
	struct ICMPCB_Structure * ICMPCB;
		 ipadr$address_block * Uptr;

// Assume this packet (Buf2) should not be deleted

    delete = FALSE;

// Log the ICMP packet if desired

    if ($$LOGF(LOG$ICMP))
	Log_ICMP_Packet(Seg,TRUE,FALSE);

//!!HACK!!// I deleted this.  It should be done 

// Try to match the input packet up with a ICMPCB
//!!HACK!!// What if there's more than one ICMPCB for this address?
    ICMPCB = ICMPCB_Find(Src$Adrs);
    if (ICMPCB == 0)
	{
//!!HACK!!// Don"t worry if there"e no ICMPCB.
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
	    goto leave_x;
	    };

	Buf2 = mm$seg_get(bufsize);	// Get a buffer
	Seg = Buf2 + ((long)Seg - Buf);
//!!HACK!!// There's no need to copy the whole buffer, only usize worth...
	MOVBYT(bufsize,Buf,Buf2);

// Setup pointer to ICMP data and ICMP data size

	Uptr = (long)Seg + ICMP_HEADER_SIZE;
	Ucount = SegSize - ICMP_HEADER_SIZE;

	    {

// Kluge. Overwrite the ICMP/IP header in the buffer, since we don't need it.

	    Uptr = Uptr - IPADR$ADDRESS_BLEN;
	    Ucount = Ucount + IPADR$ADDRESS_BLEN;
	    Uptr->ipadr$ext1 = Seg->icm$ext1;
	    Uptr->ipadr$ext2 = Seg->icm$var;
	    Uptr->ipadr$src_host = Src$Adrs;
	    Uptr->ipadr$dst_host = Dest$Adrs;
	    };

// Give the segment to the user now.

	delete = Queue_User_ICMP (ICMPCB,Uptr,Ucount,Buf2,bufsize,0);
	};
    leave_x:

// If the packet hasn't been given to the user, delete it now

    if (delete)
	mm$seg_free(bufsize,Buf2);
    }


//SBTTL "Queue_User_ICMP - Queue up ICMP packet for delivery to user"
/*
    Called by ICMP_Input at AST level when an input packet matches a
    user ICMP "connection". Function of this routine is to either deliver
    the ICMP packet to the user (if a user read request is available) or
    queue it for later deliver.
    Returns TRUE if the ICMP packet has been fully disposed of (i.e. the
    caller may deallocate the packet), FALSE otherwise (i.e. the packet
    has been placed on a queue and may not be deallocated yet).
 */

 void    DELIVER_ICMP_DATA();

Queue_User_ICMP(ICMPCB,Uptr,usize,Buf,bufsize,QB)
	struct ICMPCB_Structure * ICMPCB;
	struct queue_blk_structure(qb_nr_fields) * QB;
    {
    signed long
	QBR;
extern	mm$qblk_get();

#define	ICMPCB$NR_Qmax 5	// Max input packets permitted on input queue

// See if the input queue is full for this ICMPCB

    if (ICMPCB->ICMPCB$NR_Qcount > ICMPCB$NR_Qmax)
	{
	if ($$LOGF(LOG$ICMP))
	    QL$FAO("!%T ICMP at !XL dropped - ICMPCB NR queue full!/",0,Uptr);
	return TRUE;		// Drop the packet - no room
	};

// Allocate a queue block and insert onto user receive queue

    if (QB == 0)
	QB = mm$qblk_get();
    QB->nr$buf_size = bufsize;	// Total size of network buffer
    QB->nr$buf = Buf;		// Pointer to network buffer
    QB->nr$ucount = usize;	// Length of the data
    QB->nr$uptr = Uptr;	// Pointer to the data

// If there is a user read outstanding, deliver data, else queue for later

    if (REMQUE(ICMPCB->ICMPCB$USR_Qhead,&QBR) != EMPTY_QUEUE) // check
      Deliver_ICMP_Data(ICMPCB,QB,QBR);
    else
	INSQUE(QB,ICMPCB->ICMPCB$NR_Qtail);
    return FALSE;		// Don't deallocate this segment...
    }

//SBTTL "Deliver_ICMP_Data - Deliver ICMP data to user"
/*
    Perform actual delivery of ICMP packet to a user request.
    ICMP packet is copied into the user buffer and the user I/O request
    is posted.
 */

void Deliver_ICMP_Data(ICMPCB,QB,URQ)
	struct ICMPCB_Structure * ICMPCB;
	struct queue_blk_structure(qb_nr_fields) * QB;
	struct queue_blk_structure(qb_ur_fields) * URQ;
    {
    signed long
	FLAGS,
	ICMTYPE,
	Aptr,
	Uptr,
	Ucount;
	struct _irp * IRP;
	struct user_recv_args * uargs;
	struct user_recv_args * Sargs;

// Determine data start and data count

    Ucount = QB->nr$ucount - IPADR$ADDRESS_BLEN;
    Uptr = QB->nr$uptr + IPADR$ADDRESS_BLEN;
    Aptr = QB->nr$uptr;

// Truncate to user receive request size

    if (Ucount > URQ->ur$size)
	Ucount = URQ->ur$size;

    if ($$LOGF(LOG$ICMP))
	QL$FAO("!%T Posting ICMP receive,Size=!SL,ICMPCB=!XL,IRP=!XL,UCB_A=!XL!/",
	       0,Ucount,ICMPCB,URQ->ur$irp_adrs,URQ->ur$ucb_adrs);

// Copy from our buffer to the user system buffer

    $$KCALL(MOVBYT,Ucount,Uptr,URQ->ur$data);

// Copy ICMP Source and destination addresses to system space Diag Buff
// First, get the SysBlk address out of the IRP, then copy the Header
// block from our local copy of uargs.

    uargs = URQ->ur$uargs;
    IRP = URQ->ur$irp_adrs;
    if (uargs->re$ph_buff != 0)
	$$KCALL(MOVBYT,IPADR$ADDRESS_BLEN,
		Aptr,uargs->re$ph_buff);

// Post the I/O and free up memory

    user$post_io_status(URQ->ur$uargs,SS$_NORMAL,
			Ucount,0,0);
    mm$uarg_free(URQ->ur$uargs);

    mm$qblk_free(URQ);
    mm$seg_free(QB->nr$buf_size,QB->nr$buf);
    mm$qblk_free(QB);
    }

//SBTTL "ICMPCB_OK - Match connection ID to ICMPCB address"

ICMPCB_OK(long Conn_ID,long *RCaddr,struct user_default_args * uargs)
    {
	struct ICMPCB_Structure * ICMPCB;

#define	ICMPCBERR(EC) { *RCaddr = EC; return 0;}

// Range check the connection id. This should never fail, since the user should
// not be fondling connection IDs.

    if ((Conn_ID <= 0) || (Conn_ID > MAX_ICMPCB))
	ICMPCBERR(NET$_CDE);	// Nonexistant connection ID
    ICMPCB = icmpcb_table[Conn_ID];

// Make sure the table had something reasonable for this connection ID

    if (ICMPCB <= 0)
	ICMPCBERR(NET$_CDE);	// ICMPCB has been deleted (possible)

// Check consistancy of ICMPCB back-pointer into table

    if ((ICMPCB->icmpcb$icmpcbid != Conn_ID) ||
       (ICMPCB->ICMPCB$UCB_ADRS != uargs->ud$ucb_adrs))
	ICMPCBERR(NET$_CDE);	// Confusion (can this happen?)

// Everything is good - return the ICMPCB address

    return ICMPCB;
    }

//SBTTL "ICMPCB_Get - Allocate and initialize one ICMPCB"

ICMPCB_Get(IDX)
     long * IDX;
    {
extern	LIB$GET_VM();
extern	LIB$GET_VM_PAGE();
	struct ICMPCB_Structure * ICMPCB;
	signed long I,
	ICMPCBIDX,
	RC,
	Pages ;

// Find a free slot in the ICMPCB table

X:  {			// ** Block X **
    ICMPCBIDX = 0;
    for (I=1;I<=MAX_ICMPCB;I++)
	if (icmpcb_table[I] == 0)
	  { ICMPCBIDX = I; goto leave_x; }
    return 0;			// Failed to allocate a ICMPCB
    }			// ** Block X **
    leave_x:

// Allocate some space for the ICMPCB

//    LIB$GET_VM(%REF(ICMPCB_Size*4),ICMPCB);
    Pages = ((ICMPCB_Size * 4) / 512) + 1 ;
    RC = LIB$GET_VM_PAGE(Pages, &ICMPCB);
    if (BLISSIFNOT(RC))
	FATAL$FAO("ICMPCB_GET - LIB$GET_VM failure, RC=!XL",RC);

// Clear it out and set it in the table

    icmpcb_table[ICMPCBIDX] = ICMPCB;
    CH$FILL(/*%CHAR*/(0),ICMPCB_Size*4,ICMPCB);
    ICMPCB_Count = ICMPCB_Count+1;

// Initialize queue headers for the ICMPCB

    ICMPCB->ICMPCB$NR_Qhead = ICMPCB->ICMPCB$NR_Qtail = ICMPCB->ICMPCB$NR_Qhead;
    ICMPCB->ICMPCB$USR_Qhead = ICMPCB->ICMPCB$USR_Qtail = ICMPCB->ICMPCB$USR_Qhead;

// Set the connection ID

    ICMPCB->icmpcb$icmpcbid = ICMPCBIDX;

// Return the pointer

    *IDX = ICMPCBIDX;
    return ICMPCB;
    }

//SBTTL "ICMPCB_Free - Deallocate a ICMPCB"

void ICMPCB_Free(long ICMPCBIX,struct ICMPCB_Structure * ICMPCB)
    {
extern	LIB$FREE_VM();
extern	LIB$FREE_VM_PAGE();

    signed long
	RC,
	Pages ;

// Clear the table entry

    icmpcb_table[ICMPCBIX] = 0;

// Free the memory and decrement our counter.

//    LIB$FREE_VM(%REF(ICMPCB_Size*4),ICMPCB);
    Pages = ((ICMPCB_Size * 4) / 512) + 1 ;
    RC = LIB$FREE_VM_PAGE(Pages, ICMPCB);
    if (BLISSIFNOT(RC))
	FATAL$FAO("ICMPCB_FREE - LIB$FREE_VM failure, RC=!XL",RC);
    ICMPCB_Count = ICMPCB_Count-1;
    }

//SBTTL "Kill_ICMP_Requests - purge all I/O requests for a connection"

void Kill_ICMP_Requests(struct ICMPCB_Structure * ICMPCB,long RC)
    {
	struct queue_blk_structure(qb_ur_fields) * URQ;
	struct queue_blk_structure(qb_nr_fields) * QB;

// Make sure we aren't doing this more than once
//
//   if (ICMPCB->ICMPCB$Aborting)
//	return;

// Say that this connection is aborting (prevent future requests)

    ICMPCB->ICMPCB$Aborting = TRUE;

// Cancel any name lookup in progess

    if (ICMPCB->icmpcb$nmlook)
	{
	NML$CANCEL(ICMPCB, 0, 0);
	ICMPCB->icmpcb$nmlook = FALSE;
	};

// Kill any pending open

    NOINT;
    if (ICMPCB->icmpcb$uargs != 0)
	{
	USER$Err(ICMPCB->icmpcb$uargs,RC);
	ICMPCB->icmpcb$uargs = 0;
	};
    OKINT;

// Purge the user request queue, posting all requests

    while (REMQUE(ICMPCB->ICMPCB$USR_Qhead,&URQ) != EMPTY_QUEUE) // check
	{
	if (ICMPCB->ICMPCB$Internal)
	  (URQ->ur$astadr)(URQ->ur$astprm,RC,0);
	else
	    {
	    user$post_io_status(URQ->ur$uargs,RC,0,0,0);
	    mm$uarg_free(URQ->ur$uargs);
	    };
	mm$qblk_free(URQ);	
	};

// Purge any received qblocks as well

    while (REMQUE(ICMPCB->ICMPCB$NR_Qhead,&QB) != EMPTY_QUEUE) // check
	{
	mm$seg_free(QB->nr$buf_size,QB->nr$buf);
	mm$qblk_free(QB);
	};
    }

//SBTTL "ICMPCB_Close - Close/deallocate a ICMPCB"

void ICMPCB_Close(long UIDX,struct ICMPCB_Structure * ICMPCB, long RC)
    {
    Kill_ICMP_Requests(ICMPCB,RC);
    ICMPCB_Free(UIDX,ICMPCB);
    }

void ICMPCB_Abort(struct ICMPCB_Structure * ICMPCB,long RC)
//
// Abort a ICMPCB - called by ICMP code.
//
    {
    if (ICMPCB->ICMPCB$Internal)
      Kill_ICMP_Requests(ICMPCB,RC);
    else
	ICMPCB_Close(ICMPCB->icmpcb$icmpcbid,ICMPCB,RC);
    }


//SBTTL "ICMP$Purge_All_IO - delete ICMP database before network exits"

void icmp$purge_all_io (void)
    {
    signed long
	ICMPCBIDX;
	struct ICMPCB_Structure * ICMPCB;

// Loop for all connections, purge them, and delete them.

    for (ICMPCBIDX=1;ICMPCBIDX<=MAX_ICMPCB;ICMPCBIDX++)
	if ((ICMPCB = icmpcb_table[ICMPCBIDX]) != 0)
	    ICMPCB_Close(ICMPCBIDX,ICMPCB,NET$_TE);
    }


//SBTTL "ICMP$OPEN - open a ICMP "connection""
/*
    Open a ICMP "connection". Create a ICMP Control Block, which serves as a
    place to hang incoming packets and user receive requests.
 */

    ICMP_COPEN_DONE();
 void    ICMP_NMLOOK_DONE();
 void    ICMP_ADLOOK_DONE();

void icmp$open(struct user_open_args * uargs)
    {
    signed long
	IPADDR,
	NAMLEN,
	NAMPTR,
	UIDX,
	icmpcbptr,
	Args[4];
	struct ICMPCB_Structure * ICMPCB;

    XLOG$FAO(LOG$USER,"!%T ICMP$OPEN: PID=!XL,CHAN=!XW,FLAGS=!XL X1=!XL!/",
	     0,uargs->op$pid,uargs->op$piochan,uargs->op$flags,
	     uargs->op$ext1);

// First create a ICMPCB for this connection.

    if ((ICMPCB = ICMPCB_Get(&UIDX)) <= 0)
	{
	USER$Err(uargs,NET$_UCT);
	return;
	};

// Initialize user mode values

    ICMPCB->ICMPCB$UCB_ADRS = uargs->op$ucb_adrs;
    ICMPCB->ICMPCB$User_ID = uargs->op$pid;
    ICMPCB->ICMPCB$PIOchan = uargs->op$piochan;

// At this point, the connection exists. Write the connection ID
// back into the Unit Control Block for this connection.

    icmpcbptr = uargs->op$ucb_adrs; // not yet  + UCB$L_CBID;
    $$KCALL(MOVBYT,4,UIDX,icmpcbptr);

// Initialize queue headers for the ICMPCB

    ICMPCB->ICMPCB$NR_Qhead = ICMPCB->ICMPCB$NR_Qtail = ICMPCB->ICMPCB$NR_Qhead;
    ICMPCB->ICMPCB$USR_Qhead = ICMPCB->ICMPCB$USR_Qtail = ICMPCB->ICMPCB$USR_Qhead;

// Copy user arguments to ICMPCB

// Handle wildcard host

    NAMPTR = CH$PTR(uargs->op$foreign_host,0);
    NAMLEN = uargs->op$foreign_hlen;
    if ((! uargs->op$addr_flag) && (NAMLEN == 0))
	{
	ICMPCB->ICMPCB$Wildcard = TRUE;
	ICMPCB->ICMPCB$Foreign_Host = WILD;
	ICMPCB->ICMPCB$Foreign_Hnlen = 0;
	ICMPCB->ICMPCB$Local_Host = WILD;
	ICMPCB->icmpcb$uargs = uargs;
	ICMP_NMLOOK_DONE(ICMPCB,SS$_NORMAL,0,0,0,0);
	return;
	};

// Check for supplied IP address instead of name

X:  {			// *** Block X ***
    if (uargs->op$addr_flag)
	IPADDR = uargs->op$foreign_address;
    else
	if (GET_IP_ADDR(&NAMPTR,&IPADDR) < 0)
	    goto leave_x;
    ICMPCB->ICMPCB$Foreign_Hnlen = 0;
    ICMPCB->icmpcb$uargs = uargs;
    ICMP_NMLOOK_DONE(ICMPCB,SS$_NORMAL,1,IPADDR,0,0);
    ICMPCB->icmpcb$nmlook = TRUE;
    NML$GETNAME(IPADDR,ICMP_ADLOOK_DONE,ICMPCB);
    return;
    }			// *** Block X ***
    leave_x:

// "standard" case, host name is supplied - start name lookup for it

    ICMPCB->icmpcb$uargs = uargs;
    ICMPCB->icmpcb$nmlook = TRUE;
    NML$GETALST(NAMPTR,NAMLEN,ICMP_NMLOOK_DONE,ICMPCB);
    }



//SBTTL "ICMP_NMLOOK_DONE - Second phase of ICMP$OPEN when namelookup done"
/*
    Come here when the foreign host name has been resolved.
    At this point, we set the local & foreign hosts in the ICMPCB
    and post the users open request.
*/

void ICMP_NMLOOK_DONE(ICMPCB,STATUS,ADRCNT,ADRLST,NAMLEN,NAMPTR)
	struct ICMPCB_Structure * ICMPCB;
    {
    signed long
	RC;
	struct user_open_args * uargs;
	netio_status_block IOSB_, * IOSB = &IOSB_;

#define	UOP_ERROR(EC) \
	    { \
	    USER$Err(uargs,EC); \
	    ICMPCB_Free(ICMPCB->icmpcb$icmpcbid,ICMPCB); \
	    return; \
	    }

// Clear name lookup flag and get uargs

    NOINT;
    ICMPCB->icmpcb$nmlook = FALSE;
    uargs = ICMPCB->icmpcb$uargs;
    ICMPCB->icmpcb$uargs = 0;
    OKINT;

// Check status of the name lookup

    if (! STATUS)
	UOP_ERROR(STATUS);

// Finish up the common part of the open

    RC = ICMP_COPEN_DONE(ICMPCB,ADRCNT,ADRLST);
    if (BLISSIFNOT(RC))
	UOP_ERROR(RC);

// Verify that we have access to the host set

//!!HACK!!// Should we do this or not??
//    RC = USER$CHECK_ACCESS(ICMPCB->ICMPCB$USER_ID,ICMPCB->ICMPCB$Local_Host,
//		      0,ICMPCB->ICMPCB$Foreign_Host,0);
//    if (BLISSIFNOT(RC))
//	UOP_ERROR(RC);

// Set the foreign host name in the ICMPCB

    ICMPCB->ICMPCB$Foreign_Hnlen = NAMLEN;
    if (NAMLEN != 0)
	CH$MOVE(NAMLEN,NAMPTR,CH$PTR(ICMPCB->ICMPCB$Foreign_Hname,0));

// Finally, post the status

    IOSB->nsb$status = SS$_NORMAL; // Success return
    IOSB->nsb$byte_count = 0;
    IOSB->net_status.nsb$xstatus = 0;
    IO$POST(IOSB,uargs);
    mm$uarg_free(uargs);
    }

//SBTTL "ICMP_COPEN_DONE - Common user/internal ICMP open done routine"

ICMP_COPEN_DONE(ICMPCB,ADRCNT,ADRLST)
	struct ICMPCB_Structure * ICMPCB;
    {

// Set local and foreign host numbers according to our info

    if (ADRCNT > 0)
	IP$SET_HOSTS(ADRCNT,ADRLST,&ICMPCB->ICMPCB$Local_Host,
		     &ICMPCB->ICMPCB$Foreign_Host);

// Done at last - log success

    XLOG$FAO(LOG$USER,"!%T UDB_COPEN: Conn idx = !XL, ICMPCB = !XL!/",
	     0,ICMPCB->icmpcb$icmpcbid,ICMPCB);
    return SS$_NORMAL;
    }

//SBTTL "ICMP_ADLOOK_DONE - Finish ICMP address to name lookup"

void ICMP_ADLOOK_DONE(ICMPCB,STATUS,NAMLEN,NAMPTR)
	struct ICMPCB_Structure * ICMPCB;
    {

// Clear pending name lookup flag

    ICMPCB->icmpcb$nmlook = FALSE;

// Check status

    if (! STATUS)
	return;

// Copy the hostname into the ICMPCB

    ICMPCB->ICMPCB$Foreign_Hnlen = NAMLEN;
    CH$MOVE(NAMLEN,NAMPTR,CH$PTR(ICMPCB->ICMPCB$Foreign_Hname,0));
    }

//SBTTL "ICMP$CLOSE - close ICMP "connection""
/*
    Close a ICMP "connection". Kills any receive requests that haven't
    finished yet and deallocates the ICMPCB and any other data structures
    associated with a connection.
*/

void icmp$close(struct user_close_args * uargs)
    {
	struct ICMPCB_Structure * ICMPCB;
    signed long
	RC;

// Check for valid ICMPCB

    if ((ICMPCB = ICMPCB_OK(uargs->cl$local_conn_id,&RC,uargs)) == 0)
	{
	USER$Err(uargs,RC);
	return;
	};

// Use common routine for closing

    ICMPCB_Close(uargs->cl$local_conn_id,ICMPCB,NET$_CC);

// Close done - post user request and free argblk

    user$post_io_status(uargs,SS$_NORMAL,0,0,0);
    mm$uarg_free(uargs);
    }

//SBTTL "ICMP$ABORT - abort ICMP "connection""
/*
    Abort a ICMP "connection". Identical in functionality to ICMP$CLOSE.
 */

void icmp$abort(struct user_abort_args * uargs)
    {
	struct ICMPCB_Structure * ICMPCB;
    signed long
	RC;

// Check for valid ICMPCB

    if ((ICMPCB = ICMPCB_OK(uargs->ab$local_conn_id,&RC,uargs)) == 0)
	{
	USER$Err(uargs,RC);
	return;
	};

// Use common routine for closing

    ICMPCB_Close(uargs->ab$local_conn_id,ICMPCB,NET$_CC);

// Done. Clean up.

    user$post_io_status(uargs,SS$_NORMAL,0,0,0);
    mm$uarg_free(uargs);
    }

//SBTTL "ICMP$SEND - send ICMP packet"
/*
    Handle user send request for ICMP connection. Form a ICMP packet from the
    user's data buffer and hand it to IP layer for transmission.
 */

void icmp$send(struct user_send_args * uargs)
    {
	struct icmp_header * Seg;
    signed long
	RC,
	bufsize,
	Buf,
	LocalAddr, ForeignAddr,
	Segsize,
	usize;
	 ipadr$address_block * uhead;
	struct ICMPCB_Structure * ICMPCB;

// Validate connection ID and get ICMPCB pointer

    if ((ICMPCB = ICMPCB_OK(uargs->se$local_conn_id,&RC,uargs)) == 0)
	{
	USER$Err(uargs,RC);	// No such connection
	icmp_mib->MIB$icmpOutErrors =
		icmp_mib->MIB$icmpOutErrors + 1;
	return;
	};
    XLOG$FAO(LOG$USER,"!%T ICMP$SEND: Conn=!XL, ICMPCB=!XL, Size=!SL!/",
	     0,uargs->se$local_conn_id,ICMPCB,uargs->se$buf_size);

// Check for aborted connection

    if (ICMPCB->ICMPCB$Aborting)
	{
	XLOG$FAO(LOG$USER,"!%T ICMP$SEND for aborted ICMPCB !XL!/",0,ICMPCB);
	USER$Err(uargs,NET$_CC);
	icmp_mib->MIB$icmpOutErrors =
		icmp_mib->MIB$icmpOutErrors + 1;
	return;
	};

// Check for invalid buffer size

    if (uargs->se$buf_size < 0)
	{
	USER$Err(uargs,NET$_BTS);
	icmp_mib->MIB$icmpOutErrors =
		icmp_mib->MIB$icmpOutErrors + 1;
	return;
	};

// Check for "address mode" connection and set host addresses from user buffer
// in that case.

    ForeignAddr = ICMPCB->ICMPCB$Foreign_Host;
    if (ForeignAddr == WILD)
	ForeignAddr = ICMPCB->ICMPCB$Foreign_Host;

    LocalAddr = ICMPCB->ICMPCB$Local_Host;
    if (LocalAddr == WILD)
    IP$SET_HOSTS(1,&ForeignAddr,&LocalAddr,&ForeignAddr);

   if ((ForeignAddr == WILD))
	{
	USER$Err(uargs,NET$_NOPN);
	icmp_mib->MIB$icmpOutErrors =
		icmp_mib->MIB$icmpOutErrors + 1;
	return;
	};

// Allocate an output buffer and build an IP packet

    usize = uargs->se$buf_size;
    if (usize > MAX_ICMP_DATA_SIZE)
	usize = MAX_ICMP_DATA_SIZE;

// Use preallocated buffer sizes to reduce dynamic memory load

    bufsize = usize + IP_HDR_BYTE_SIZE + DEVICE_HEADER;
    if (bufsize <= min_physical_bufsize)
      bufsize = min_physical_bufsize;
    else
	if (bufsize <= max_physical_bufsize)
	    bufsize = max_physical_bufsize;
    Buf = mm$seg_get(bufsize);	// Get a buffer
    Seg = Buf + DEVICE_HEADER + IP_HDR_BYTE_SIZE; // Point at ICMP segment
    Segsize = usize+ICMP_HEADER_SIZE; // Length of segment + ICMP header

// Set up the ICMP header
    uhead = uargs->se$protohdrblk;
    Seg->icm$type = uhead->ipadr$type;
    Seg->icm$code = uhead->ipadr$code;
    Seg->icm$cksum = 0;
    Seg->icm$var = uhead->ipadr$special;

// Copy the user data into the data area

    $$KCALL(MOVBYT,usize,uargs->se$data_start,Seg->icm$data);

// Swap the header bytes and compute the checksum

    swapbytesicmphdr(ICMP_HEADER_SIZE/2,Seg);
//!!HACK!!// Hardwired in ICMP Header size of 8.
    Seg->icm$cksum=Calc_Checksum(usize+ICMP_HEADER_SIZE,Seg);

// Log the ICMP packet if desired

    if ($$LOGF(LOG$ICMP))
	Log_ICMP_Packet(Seg,FALSE,TRUE);

// Send the segment to IP (it will deallocate it)

    ICMPIPID = ICMPIPID+1;	// Increment packet ID
    RC = SS$_NORMAL;
    if ((ip$send(LocalAddr,ForeignAddr,ICMPTOS,icmpttl,
		   Seg,Segsize,ICMPIPID,ICMPDF,TRUE,ICMP_Protocol,
		   Buf,bufsize) == 0)) RC = NET$_NRT;

    // Keep count of outgoing packets and errors
    icmp_mib->MIB$icmpOutMsgs = icmp_mib->MIB$icmpOutMsgs + 1;
    if (RC != SS$_NORMAL) icmp_mib->MIB$icmpOutErrors =
				    icmp_mib->MIB$icmpOutErrors + 1;

    // Do SNMP accounting
    switch ( Seg->icm$type)
	{
	case ICM_ECHO:	icmp_mib->MIB$icmpOutEchos =
				icmp_mib->MIB$icmpOutEchos + 1;
	break;
	case ICM_TSTAMP:	icmp_mib->MIB$icmpOutTimeStamps =
				icmp_mib->MIB$icmpOutTimeStamps + 1;
	break;
	case ICM_AMREQUEST:icmp_mib->MIB$icmpOutAddrMasks =
				icmp_mib->MIB$icmpOutAddrMasks + 1;
	break;
	case ICM_DUNREACH:	icmp_mib->MIB$icmpOutDestUnreachs =
				icmp_mib->MIB$icmpOutDestUnreachs + 1;
	break;
	case ICM_SQUENCH:	icmp_mib->MIB$icmpOutSrcQuenchs =
				icmp_mib->MIB$icmpOutSrcQuenchs + 1;
	break;
	case ICM_REDIRECT:	icmp_mib->MIB$icmpOutRedirects =
				icmp_mib->MIB$icmpOutRedirects + 1;
	break;
	case ICM_TEXCEED:	icmp_mib->MIB$icmpOutTimeExcds =
				icmp_mib->MIB$icmpOutTimeExcds + 1;
	break;
	case ICM_PPROBLEM:	icmp_mib->MIB$icmpOutParamProbs =
				icmp_mib->MIB$icmpOutParamProbs + 1;
	break;
	case ICM_TSREPLY:	icmp_mib->MIB$icmpOutTimeStampReps =
				icmp_mib->MIB$icmpOutTimeStampReps + 1;
	break;
	case ICM_AMREPLY:	icmp_mib->MIB$icmpOutAddrMaskReps =
				icmp_mib->MIB$icmpOutAddrMaskReps + 1;
	break;
	case ICM_EREPLY:	icmp_mib->MIB$icmpOutEchoReps =
				icmp_mib->MIB$icmpOutEchoReps + 1;
	break;
	};

// Post the I/O request back to the user

    user$post_io_status(uargs,RC,0,0,0);
    mm$uarg_free(uargs);

    }



//SBTTL "ICMP$RECEIVE - receive a ICMP packet"
/*
    Handle user receive request for ICMP connection. If there is a packet
    available on the ICMP receive queue, then deliver it to the user
    immediately. Otherwise, queue up the user receive for later.
 */

void icmp$receive(struct user_recv_args * uargs)
    {
	struct ICMPCB_Structure * ICMPCB;
	struct queue_blk_structure(qb_nr_fields) * QB;
	struct queue_blk_structure(qb_ur_fields) * URQ;
    signed long
	RC;

// Validate connection ID and get ICMPCB pointer

    if ((ICMPCB = ICMPCB_OK(uargs->re$local_conn_id,&RC,uargs)) == 0)
	{
	USER$Err(uargs,RC);	// No such connection
	return;
	};
    XLOG$FAO(LOG$USER,"!%T ICMP$RECEIVE: Conn=!XL, ICMPCB=!XL, Size=!SL!/",
	     0,uargs->re$local_conn_id,ICMPCB,uargs->re$buf_size);

// Check for aborted connection

    if (ICMPCB->ICMPCB$Aborting)
	{
	XLOG$FAO(LOG$USER,"!%T ICMP$RECEIVE for aborted ICMPCB !XL!/",0,ICMPCB);
	USER$Err(uargs,NET$_CC);
	return;
	};

// Check for invalid buffer size

    if (uargs->re$buf_size <= 0)
	{
	USER$Err(uargs,NET$_BTS);
	return;
	};

// Make a request block for the receive

    URQ = mm$qblk_get();		// Get a queue block
    URQ->ur$size = uargs->re$buf_size; // # of bytes this rq can take
    URQ->ur$data = uargs->re$data_start; // Address of system buffer
    URQ->ur$irp_adrs = uargs->re$irp_adrs; // IO request packet address
    URQ->ur$ucb_adrs = uargs->re$ucb_adrs; // Unit Control Block address
    URQ->ur$uargs = uargs;	// User argument block address

// If anything is available on the queue, deliver it now, else queue for later

    NOINT;
    if (REMQUE(ICMPCB->ICMPCB$NR_Qhead,&QB) != EMPTY_QUEUE) // check
	Deliver_ICMP_Data(ICMPCB,QB,URQ);
    else
	INSQUE(URQ,ICMPCB->ICMPCB$USR_Qtail);
    OKINT;
    }



//SBTTL "ICMP$INFO - get info about ICMP "connection""
/*
    Read the host names/numbers for a ICMP connection.
 */

void icmp$info(struct user_info_args * uargs)
    {
extern	void user$net_connection_info();
	struct ICMPCB_Structure * ICMPCB;
    signed long
	RC;

// Validate the connection ID

    if ((ICMPCB = ICMPCB_OK(uargs->if$local_conn_id,&RC,uargs)) == 0)
	{
	USER$Err(uargs,RC);	// Bad connection ID
	return;
	};

// Give the information back (common TCP/ICMP routine in USER.BLI)

    user$net_connection_info(uargs,ICMPCB->ICMPCB$Local_Host,
			ICMPCB->ICMPCB$Foreign_Host,
			0,0,
			ICMPCB->ICMPCB$Foreign_Hname,
			ICMPCB->ICMPCB$Foreign_Hnlen);
    }


//SBTTL "ICMP$STATUS - get status of ICMP "connection""
/*
    This routine is a placeholder for the network STATUS command, which is
    currently implemented for the TCP protocol.
 */

void icmp$status(struct user_status_args * uargs)
    {
    USER$Err(uargs,NET$_NYI);
    }

//SBTTL "ICMP$CANCEL - Handle VMS cancel for ICMP connection"
/*
    Handle process abort/$CANCEL request for a ICMP connection. Identical
    in functionality to ICMP$CLOSE/ICMP$ABORT except for calling procedure.
 */

icmp$cancel(struct vms$cancel_args * uargs)
    {
	struct ICMPCB_Structure * ICMPCB;
    signed long I,
	Fcount;

    Fcount = 0;

// Check all valid ICMPCB's looking for a match on pid and channel #.

    for (I=1;I<=MAX_ICMPCB;I++)
	if ((ICMPCB = icmpcb_table[I]) != 0)
	    {

// If the process doing the cancel owns this connection, then delete it.

	    if ((ICMPCB->ICMPCB$User_ID == uargs->vc$pid) &&
	       (ICMPCB->ICMPCB$PIOchan == uargs->vc$piochan))
		{
		XLOG$FAO(LOG$USER,"!%T ICMP$Cancel: ICMPCB=!XL!/",0,ICMPCB);
		ICMPCB_Close(I,ICMPCB,NET$_CCAN);
		Fcount = Fcount + 1;
		};
	    };
    return Fcount;
    }

//SBTTL "ICMP dump routines"

void icmp$connection_list(RB)
//
// Dump out the list of ICMP connections.
//
	D$ICMP_LIST_RETURN_BLK RB;
    {
      signed long I,
	RBIX;
    RBIX = 1;
    for (I=1;I<=MAX_ICMPCB-1;I++)
	if (icmpcb_table[I] != 0)
	    {
	    RB[RBIX] = I;
	    RBIX = RBIX + 1;
	    };
    RB[0] = RBIX - 1;
    }

icmp$icmpcb_dump(ICMPCBIX,RB)
//
// Dump out a single ICMP connection
//
	D$ICMPCB_Dump_Return_Blk * RB;
    {
	struct ICMPCB_Structure * ICMPCB;
	    struct queue_blk_structure(qb_nr_fields) * QB;
    signed long
	Qcount;

// Validate that there is a real ICMPCB there

    if ((ICMPCBIX < 1) || (ICMPCBIX > MAX_ICMPCB) ||
       ((ICMPCB = icmpcb_table[ICMPCBIX]) == 0))
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
    while ((QB != &ICMPCB->ICMPCB$NR_Qhead))
	{
	Qcount = Qcount + 1;
	QB = QB->nr$next;
	};
    RB->DU$ICMPCB_NR_Qcount = Qcount;

// Get length of user receive queue

    QB = ICMPCB->ICMPCB$USR_Qhead;
    Qcount = 0;
    while ((QB != &ICMPCB->ICMPCB$USR_Qhead))
	{
	Qcount = Qcount + 1;
	QB = QB->nr$next; // was: ur$next, but the same
	};
    RB->DU$ICMPCB_UR_Qcount = Qcount;

// Done.

    return TRUE;
    }
