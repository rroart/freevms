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
#include <descrip.h>

#undef TCP_DATA_OFFSET
#include <net/checksum.h>
#define Calc_Checksum(x,y) ip_compute_csum(y,x)

//*** Special literals from USER.BLI ***

#define    UCB$Q_DDP  ucb$q_devdepend //check
#define    UCB$L_CBID  ucb$q_devdepend
#define    UCB$L_EXTRA ucb$q_devdepend

// External data items

extern signed long
    ipttl,
    INTDF,
    AST_In_Progress,
    log_state,
    min_physical_bufsize,
    max_physical_bufsize;

// External routines

// MACLIB.MAR

extern  void    swapbytes();
extern  void    MOVBYT();
//extern     Calc_Checksum();

// MEMGR.BLI

extern  void    mm$uarg_free();
extern     mm$qblk_get();
extern  void    mm$qblk_free();
extern     mm$seg_get();
extern  void    mm$seg_free();

// USER.BLI

extern     user$check_access();
extern     USER$Err();
extern  void    IO$POST();
extern  void    user$post_io_status();

// IP.BLI

extern  void    IP$SET_HOSTS();
extern     ip$send_raw();
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


//SBTTL "IP data structures"

// Define the "IPCB" - IP analogue of TCB.

struct  IPCB_Structure
{
unsigned long     ipcb$foreign_host	;	// IP foreign host number
unsigned long     ipcb$host_filter	;	// Receive packets from this host
unsigned long     ipcb$proto_filter	;	// Receive packets to this protocol
  char    ipcb$foreign_hname[MAX_HNAME];
  short int    ipcb$foreign_hnlen;
void *     ipcb$usr_qhead	;	// User receive request queue
void *     ipcb$usr_qtail	;
void *     ipcb$nr_qhead	;	// Net receive queue
void *     ipcb$nr_qtail	;
unsigned short int    ipcb$nr_qcount;
union {
      unsigned short    ipcb$flags;
struct {
unsigned 	ipcb$wildcard	 : 1;	// IPCB opened with wild FH/LH
unsigned 	ipcb$addr_mode	 : 1;	// User wants IP addresses
unsigned 	ipcb$aborting	 : 1;	// IPCB is closing
unsigned 	ipcb$nmlook	 : 1;	// IPCB has an outstanding name lookup
	  };
};
void *     ipcb$ipcbid		;	// IPCB_Table index for this connection
void *     ipcb$ucb_adrs	;	// Connection UCB address
void *     ipcb$uargs	;	// Uarg block in pending open
unsigned long    ipcb$user_id;	// Process ID of owner
unsigned short    ipcb$piochan;	// Process IO channel
      };

#define IPCB_Size sizeof(struct IPCB_Structure)

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
    ipcb_table[MAX_IPCB+1];// Table of IPCBs



//SBTTL "IP packet logger"
/*
    Queue up a log entry to dump out a IP packet.
 */

void Log_IP_Packet(seg,SwapFlag,SendFlag)
	struct ip_structure * seg;
    {
    signed long
	Header_Size,
	segdata;
	struct ip_structure segcopy_ ,* segcopy=&segcopy_;
	struct ip_structure * seghdr;
	struct dsc$descriptor sptr;

//!!HACK!!// Make sure this works right.

    seghdr = seg;		// Point at segment header
    Header_Size = seg->iph$ihl * 4;	// Calculate header size
    segdata = (long)seg + Header_Size;
    if (SwapFlag)		// Need to byteswap header?
	{
	CH$MOVE(Header_Size,CH$PTR(seg,0),CH$PTR(segcopy,0)); // Make a copy
	seghdr = segcopy;	// Point at this version...
	swapbytes(Header_Size/2,seghdr); // Swap header bytes
	};

// Print first part of info

    if (SendFlag)
      sptr = ASCID("Sent");
    else
	sptr = ASCID("Received");

// Log the contents of the IP header

    QL$FAO(/*%STRING*/("!%T !AS IP packet, SEG=!XL, DATA=!XL, Header size !SL!/",
		   "!_CKsum:!_!SL!/"),
	    0,sptr,seg,segdata,Header_Size,
	   seghdr->iph$checksum);

    }

//SBTTL "IPCB_Find - look up IP control block"
IPCB_Find(Src$Adrs,Dst$Adrs,Protocol)
    {
    signed long
	ucount,
	IPCBIX;
	struct IPCB_Structure * IPCB;

    ucount = IPCB_Count;
    IPCBIX = 1;
    while ((ucount > 0) && (IPCBIX <= MAX_IPCB))
	{
	if ((IPCB = ipcb_table[IPCBIX]) != 0)
	    {
	    if ( ((IPCB->ipcb$host_filter == WILD) ||
		(IPCB->ipcb$host_filter == Src$Adrs)) &&
		((IPCB->ipcb$proto_filter == WILD) ||
		(IPCB->ipcb$proto_filter == Protocol)))
		return IPCB;
	    ucount = ucount-1;
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

void ipu$user_input ( Src$Adrs,Dst$Adrs,Protocol,
				bufsize,Buf,segsize,seg )
	struct ip_structure * seg;
{
    signed long
	Uptr,
	ucount,
	IPCBIX,
	sum,
      delete;
	struct IPCB_Structure * IPCB;

// Assume this packet should be deleted

    delete = TRUE;

// Log the IP packet if desired

    if ($$LOGF(LOG$IP))
	Log_IP_Packet(seg,TRUE,FALSE);

//!!HACK!!// I deleted this.  It should be done 

// Try to match the input packet up with a IPCB
//!!HACK!!// What if there's more than one IPCB for this address?
    IPCB = IPCB_Find ( Src$Adrs , Dst$Adrs , Protocol );
    if (IPCB == 0)
	{
//!!HACK!!// Don"t worry if there"e no IPCB.
	if ($$LOGF(LOG$IP))
	    QL$FAO("!%T No IPCB found for segment !XL!/",0,seg);
	}
    else
X:	{

// Log that it was found

	if ($$LOGF(LOG$IP))
	    QL$FAO("!%T IPCB !XL found for IP seg !XL!/",
		   0,IPCB,seg);

// Make sure the IPCB isn't aborted...

	if (IPCB->ipcb$aborting)
	    {
	    XQL$FAO(LOG$IP,"!%T IP input !XL for aborted IPCB !XL dropped!/",
		    0,seg,IPCB);
	    goto leave_x;
	    };

// Give the segment to the user now.

	delete = Queue_User_IP(IPCB,seg,segsize,Buf,bufsize,0);
    };
 leave_x:

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

Queue_User_IP(IPCB,Uptr,Usize,Buf,bufsize,QB)
	struct IPCB_Structure * IPCB;
	struct queue_blk_structure(qb_nr_fields) * QB;
    {
    signed long
	Buf2,
	QBR;
extern	mm$qblk_get();
#define	IPCB$NR_QMAX 5	// Max input packets permitted on input queue

// See if the input queue is full for this IPCB

    if (IPCB->ipcb$nr_qcount > IPCB$NR_QMAX)
	{
	if ($$LOGF(LOG$IP))
	    QL$FAO("!%T IP at !XL dropped - IPCB NR queue full!/",0,Uptr);
	return TRUE;		// Drop the packet - no room
	};

// We need to make a copy of this IP datagram.

    Buf2 = mm$seg_get(bufsize);	// Get a buffer
    Uptr = Buf2 + (Uptr - Buf);
//!!HACK!!// There's no need to copy the whole buffer, only Usize worth...
    MOVBYT(bufsize,Buf,Buf2);

// Allocate a queue block and insert onto user receive queue

    if (QB == 0)
	QB = mm$qblk_get();
    QB->nr$buf_size = bufsize;	// Total size of network buffer
    QB->nr$buf = Buf2;		// Pointer to network buffer
    QB->nr$ucount = Usize;	// Length of the data
    QB->nr$uptr = Uptr;	// Pointer to the data

// If there is a user read outstanding, deliver data, else queue for later

    if (REMQUE(IPCB->ipcb$usr_qhead,&QBR) != EMPTY_QUEUE) // check
      Deliver_IP_Data(IPCB,QB,QBR);
    else
	INSQUE(QB,IPCB->ipcb$nr_qtail);

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
	struct queue_blk_structure(qb_nr_fields) * QB;
 	struct queue_blk_structure(qb_ur_fields) * URQ;
    {
    signed long
	FLAGS,
	ICMTYPE,
	IRP,
	ucount;
    struct user_recv_args * Uargs;
    struct user_recv_args * Sargs;
  ipadr$address_block Aptr_, * Aptr = &Aptr_;
	  struct ip_structure * Uptr;

// Determine data start and data count

//!!HACK!!// come back here...

    ucount = QB->nr$ucount;
    Uptr = QB->nr$uptr;

// Truncate to user receive request size

    if (ucount > URQ->ur$size)
	ucount = URQ->ur$size;

    if ($$LOGF(LOG$IP))
	QL$FAO("!%T Posting IP receive,Size=!SL,IPCB=!XL,IRP=!XL,UCB_A=!XL!/",
	       0,ucount,IPCB,URQ->ur$irp_adrs,URQ->ur$ucb_adrs);

// Copy from our buffer to the user system buffer

    $$KCALL(MOVBYT,ucount,Uptr,URQ->ur$data);

    Uargs = URQ->ur$uargs;

// Copy IP Source and destination addresses to system space Diag Buff
// First, get the SysBlk address out of the IRP, then copy the Header
// block from our local copy of Uargs.

    if (Uargs->re$ph_buff != 0)
	{
	IRP = URQ->ur$irp_adrs;
	Aptr->ipadr$src_host = Uptr->iph$source;
	Aptr->ipadr$dst_host = Uptr->iph$dest;
	Aptr->ipadr$ext1 = ((long*)Uptr)[0];	// First long of IP header
	Aptr->ipadr$ext2 = ((long*)Uptr)[2];	// Third long of IP header
	$$KCALL(MOVBYT,IPADR$ADDRESS_BLEN,
		Aptr,Uargs->re$ph_buff);
	};

// Post the I/O and free up memory

    user$post_io_status(URQ->ur$uargs,SS$_NORMAL,
			ucount,0,0);
    mm$uarg_free(URQ->ur$uargs);

    mm$qblk_free(URQ);
    mm$seg_free(QB->nr$buf_size,QB->nr$buf);
    mm$qblk_free(QB);
    }

//SBTTL "IPCB_OK - Match connection ID to IPCB address"

IPCB_OK(long Conn_ID,long * RCaddr,struct user_default_args * Uargs)
    {
	struct IPCB_Structure * IPCB;

#define	IPCBERR(EC) { *RCaddr = EC; return 0;}

// Range check the connection id. This should never fail, since the user should
// not be fondling connection IDs.

    if ((Conn_ID <= 0) || (Conn_ID > MAX_IPCB))
	IPCBERR(NET$_CDE);	// Nonexistant connection ID
    IPCB = ipcb_table[Conn_ID];

// Make sure the table had something reasonable for this connection ID

    if (IPCB <= 0)
	IPCBERR(NET$_CDE);	// IPCB has been deleted (possible)

// Check consistancy of IPCB back-pointer into table

    if ((IPCB->ipcb$ipcbid != Conn_ID) ||
       (IPCB->ipcb$ucb_adrs != Uargs->ud$ucb_adrs))
	IPCBERR(NET$_CDE);	// Confusion (can this happen?)

// Everything is good - return the IPCB address

    return IPCB;
    }

//SBTTL "IPCB_Get - Allocate and initialize one IPCB"

IPCB_Get(IDX)
     long * IDX;
    {
extern	LIB$GET_VM();
extern	LIB$GET_VM_PAGE();
	struct IPCB_Structure * IPCB;
	signed long I,
	IPCBIDX,
	RC ;

// Find a free slot in the IPCB table

X:  {			// ** Block X **
    IPCBIDX = 0;
    for (I=1;I<=MAX_IPCB;I++)
	if (ipcb_table[I] == 0)
	  { IPCBIDX = I; goto leave_x; }
    return 0;			// Failed to allocate a IPCB
    }			// ** Block X **
    leave_x:

// Allocate some space for the IPCB

//    LIB$GET_VM(%REF(IPCB_Size*4),IPCB);
    RC = LIB$GET_VM_PAGE(/*%REF*/(((IPCB_Size * 4) / 512) + 1),&IPCB);
    if (! RC)
	FATAL$FAO("IPCB_GET - LIB$GET_VM failure, RC=!XL",RC);

// Clear it out and set it in the table

    ipcb_table[IPCBIDX] = IPCB;
    CH$FILL(/*%CHAR*/(0),IPCB_Size*4,IPCB);
    IPCB_Count = IPCB_Count+1;

// Initialize queue headers for the IPCB

    IPCB->ipcb$nr_qhead = IPCB->ipcb$nr_qtail = IPCB->ipcb$nr_qhead;
    IPCB->ipcb$usr_qhead = IPCB->ipcb$usr_qtail = IPCB->ipcb$usr_qhead;

// Set the connection ID

    IPCB->ipcb$ipcbid = IPCBIDX;

// Return the pointer

    *IDX = IPCBIDX;
    return IPCB;
    }

//SBTTL "IPCB_Free - Deallocate a IPCB"

void ipcb_free(long IPCBIX,struct IPCB_Structure * IPCB)
    {
extern	LIB$FREE_VM();
extern	LIB$FREE_VM_PAGE();

    signed long
	RC ;

// Clear the table entry

    ipcb_table[IPCBIX] = 0;

// Free the memory and decrement our counter.

//    LIB$FREE_VM(/*%REF*/(IPCB_Size*4),IPCB);
    RC = LIB$FREE_VM_PAGE(/*%REF*/(((IPCB_Size * 4) / 512) + 1),IPCB);
    if (! RC)
	FATAL$FAO("IPCB_FREE - LIB$FREE_VM failure, RC=!XL",RC);
    IPCB_Count = IPCB_Count-1;
    }

//SBTTL "Kill_IP_Requests - purge all I/O requests for a connection"

void Kill_IP_Requests(struct IPCB_Structure * IPCB,long RC)
    {
	struct queue_blk_structure(qb_ur_fields) * URQ;
	struct queue_blk_structure(qb_nr_fields) * QB;

// Make sure we aren't doing this more than once
//
//   if (IPCB->ipcb$aborting)
//	return;

// Say that this connection is aborting (prevent future requests)

    IPCB->ipcb$aborting = TRUE;

// Cancel any name lookup in progess

    if (IPCB->ipcb$nmlook)
	{
	NML$CANCEL(IPCB, 0, 0);
	IPCB->ipcb$nmlook = FALSE;
	};

// Kill any pending open

    NOINT;
    if (IPCB->ipcb$uargs != 0)
	{
	USER$Err(IPCB->ipcb$uargs,RC);
	IPCB->ipcb$uargs = 0;
	};
    OKINT;

// Purge the user request queue, posting all requests

    while (REMQUE(IPCB->ipcb$usr_qhead,&URQ) != EMPTY_QUEUE) // check
	{
	user$post_io_status(URQ->ur$uargs,RC,0,0,0);
	mm$uarg_free(URQ->ur$uargs);
	mm$qblk_free(URQ);	
	};

// Purge any received qblocks as well

    while (REMQUE(IPCB->ipcb$nr_qhead,&QB) != EMPTY_QUEUE) // check
	{
	mm$seg_free(QB->nr$buf_size,QB->nr$buf);
	mm$qblk_free(QB);
	};
    }

//SBTTL "IPCB_Close - Close/deallocate a IPCB"

void ipcb_close(long UIDX,struct IPCB_Structure * IPCB, long RC)
    {
    Kill_IP_Requests(IPCB,RC);
    ipcb_free(UIDX,IPCB);
    }

void IPCB_Abort(struct IPCB_Structure * IPCB, long RC)
//
// Abort a IPCB - called by IP code.
//
    {
    ipcb_close(IPCB->ipcb$ipcbid,IPCB,RC);
    }


//SBTTL "IPU$Purge_All_IO - delete IP database before network exits"

void ipu$purge_all_io (void)
    {
    signed long
	IPCBIDX;
	struct IPCB_Structure * IPCB;

// Loop for all connections, purge them, and delete them.

    for (IPCBIDX=1;IPCBIDX<=MAX_IPCB;IPCBIDX++)
	if ((IPCB = ipcb_table[IPCBIDX]) != 0)
	    ipcb_close(IPCBIDX,IPCB,NET$_TE);
    }


//SBTTL "IPU$OPEN - open a user IP "connection""
/*
    Open an IP "connection". Create a IP Control Block, which serves as a
    place to hang incoming packets and user receive requests.
 */

 void    IP_NMLOOK_DONE();
 void    IP_ADLOOK_DONE();

void ipu$open(struct user_open_args * Uargs)
    {
	struct IPCB_Structure * IPCB;
    signed long
	IPADDR,
	NAMLEN,
	NAMPTR,
	UIDX,
	ipcbptr,
	Args[4];

    XLOG$FAO(LOG$USER,"!%T IPU$OPEN: PID=!XL,CHAN=!XW,FLAGS=!XL X1=!XL!/",
	     0,Uargs->op$pid,Uargs->op$piochan,Uargs->op$flags,
	     Uargs->op$ext1);

// First create a IPCB for this connection.

    if ((IPCB = IPCB_Get(&UIDX)) <= 0)
	{
	USER$Err(Uargs,NET$_UCT);
	return;
	};

// Initialize user mode values

    IPCB->ipcb$ucb_adrs = Uargs->op$ucb_adrs;
    IPCB->ipcb$user_id = Uargs->op$pid;
    IPCB->ipcb$piochan = Uargs->op$piochan;

// At this point, the connection exists. Write the connection ID
// back into the Unit Control Block for this connection.

    ipcbptr = Uargs->op$ucb_adrs; // check + UCB$L_CBID;
    $$KCALL(MOVBYT,4,UIDX,ipcbptr);

// Initialize queue headers for the IPCB

    IPCB->ipcb$nr_qhead = IPCB->ipcb$nr_qtail = IPCB->ipcb$nr_qhead;
    IPCB->ipcb$usr_qhead = IPCB->ipcb$usr_qtail = IPCB->ipcb$usr_qhead;

// Copy user arguments to IPCB

    IPCB->ipcb$uargs = Uargs;
    IPCB->ipcb$host_filter = Uargs->op$src_host;
    IPCB->ipcb$proto_filter = Uargs->op$ext1;

// Handle wildcard host

    NAMPTR = CH$PTR(Uargs->op$foreign_host,0);
    NAMLEN = Uargs->op$foreign_hlen;
    if ((! Uargs->op$addr_flag) && (NAMLEN == 0))
	{
	IPCB->ipcb$wildcard = TRUE;
	IPCB->ipcb$foreign_host = WILD;
	IPCB->ipcb$foreign_hnlen = 0;
	IP_NMLOOK_DONE(IPCB,SS$_NORMAL,0,0,0,0);
	return;
	};

// Check for supplied IP address instead of name

X:  {			// *** Block X ***
    if (Uargs->op$addr_flag)
      IPADDR = Uargs->op$foreign_address;
    else
	if (GET_IP_ADDR(&NAMPTR,&IPADDR) < 0)
	    goto leave_x;
    IPCB->ipcb$foreign_hnlen = 0;
    IPCB->ipcb$foreign_host = IPADDR;
    IP_NMLOOK_DONE(IPCB,SS$_NORMAL,1,IPADDR,0,0);
    IPCB->ipcb$nmlook = TRUE;
    NML$GETNAME(IPADDR,IP_ADLOOK_DONE,IPCB);
    return;
    }			// *** Block X ***
    leave_x:

// "standard" case, host name is supplied - start name lookup for it

    IPCB->ipcb$nmlook = TRUE;
    NML$GETALST(NAMPTR,NAMLEN,IP_NMLOOK_DONE,IPCB);
    }



//SBTTL "IP_NMLOOK_DONE - Second phase of IPU$OPEN when namelookup done"
/*
    Come here when the foreign host name has been resolved.
    At this point, we set the local & foreign hosts in the IPCB
    and post the users open request.
*/

void IP_NMLOOK_DONE(IPCB,STATUS,ADRCNT,ADRLST,NAMLEN,NAMPTR)
	struct IPCB_Structure * IPCB;
    {
    signed long
    RC;
    struct user_open_args * Uargs;
	 netio_status_block IOSB_, * IOSB = &IOSB_ ;
#define	UOP_ERROR(EC) \
	    { \
	    USER$Err(Uargs,EC); \
	    ipcb_free(IPCB->ipcb$ipcbid,IPCB); \
	    return; \
	    }

// Clear name lookup flag and get uargs

    NOINT;
    IPCB->ipcb$nmlook = FALSE;
    Uargs = IPCB->ipcb$uargs;
    IPCB->ipcb$uargs = 0;
    OKINT;

// Check status of the name lookup

    if (! STATUS)
	UOP_ERROR(STATUS);

    // Finish up the open

//    if (ADRCNT > 0)
//	IP$SET_HOSTS(ADRCNT,ADRLST,IPCB->ipcb$local_host,
//		     IPCB->ipcb$foreign_host);

    // Done at last - log success

    XLOG$FAO(LOG$USER,"!%T UDB_OPEN: Conn idx = !XL, IPCB = !XL!/",
	     0,IPCB->ipcb$ipcbid,IPCB);

// Verify that we have access to the host set

//!!HACK!!// Should we do this or not??
//    RC = USER$CHECK_ACCESS(IPCB->ipcb$user_id,IPCB->ipcb$local_host,
//		      0,IPCB->ipcb$foreign_host,0);
//    if (! RC)
//	UOP_ERROR(RC);

// Set the foreign host name in the IPCB

    IPCB->ipcb$foreign_hnlen = NAMLEN;
    if (NAMLEN != 0)
	CH$MOVE(NAMLEN,NAMPTR,CH$PTR(IPCB->ipcb$foreign_hname,0));

// Finally, post the status

    IOSB->nsb$status = SS$_NORMAL; // Success return
    IOSB->nsb$byte_count = 0;
    IOSB->net_status.nsb$xstatus = 0;
    IO$POST(IOSB,Uargs);
    mm$uarg_free(Uargs);
    }



//SBTTL "IP_ADLOOK_DONE - Finish IP address to name lookup"

void IP_ADLOOK_DONE(IPCB,STATUS,NAMLEN,NAMPTR)
	struct IPCB_Structure * IPCB;
    {

// Clear pending name lookup flag

    IPCB->ipcb$nmlook = FALSE;

// Check status

    if (! STATUS)
	return;

// Copy the hostname into the IPCB

    IPCB->ipcb$foreign_hnlen = NAMLEN;
    CH$MOVE(NAMLEN,NAMPTR,CH$PTR(IPCB->ipcb$foreign_hname,0));
    }



//SBTTL "IPU$CLOSE - close IP "connection""
/*
    Close an IP "connection". Kills any receive requests that haven't
    finished yet and deallocates the IPCB and any other data structures
    associated with a connection.
*/

void ipu$close(struct user_close_args * Uargs)
    {
struct IPCB_Structure * IPCB;
    signed long
	RC;

// Check for valid IPCB

    if ((IPCB = IPCB_OK(Uargs->cl$local_conn_id,&RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);
	return;
	};

// Use common routine for closing

    ipcb_close(Uargs->cl$local_conn_id,IPCB,NET$_CC);

// Close done - post user request and free argblk

    user$post_io_status(Uargs,SS$_NORMAL,0,0,0);
    mm$uarg_free(Uargs);
    }

//SBTTL "IPU$ABORT - abort IP "connection""
/*
    Abort a IP "connection". Identical in functionality to IPU$CLOSE.
 */

void ipu$abort(struct user_abort_args * Uargs)
    {
	struct IPCB_Structure * IPCB;
    signed long
	RC;

// Check for valid IPCB

    if ((IPCB = IPCB_OK(Uargs->ab$local_conn_id,&RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);
	return;
	};

// Use common routine for closing

    ipcb_close(Uargs->ab$local_conn_id,IPCB,NET$_CC);

// Done. Clean up.

    user$post_io_status(Uargs,SS$_NORMAL,0,0,0);
    mm$uarg_free(Uargs);
    }

//SBTTL "IPU$SEND - send IP packet"
/*
    Handle user send request for IP connection. Form a IP packet from the
    user's data buffer and hand it to IP layer for transmission.
 */

void ipu$send(struct user_send_args * Uargs)
    {
    signed long
	RC,
	Flags,
	bufsize,
	Buf,
	LocalAddr, ForeignAddr, Protocol,
	segsize,
	USize;
	struct ip_structure * seg;
	struct IPCB_Structure * IPCB;


// Validate connection ID and get IPCB pointer

    if ((IPCB = IPCB_OK(Uargs->se$local_conn_id,&RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// No such connection
	return;
	};

//!!HACK!!// Does this size arg mean  anything?
    XLOG$FAO(LOG$USER,"!%T IP$SEND: Conn=!XL, IPCB=!XL, Size=!SL!/",
	     0,Uargs->se$local_conn_id,IPCB,Uargs->se$buf_size);

// Check for aborted connection

    if (IPCB->ipcb$aborting)
	{
	XLOG$FAO(LOG$USER,"!%T IPU$SEND for aborted IPCB !XL!/",0,IPCB);
	USER$Err(Uargs,NET$_CC);
	return;
	};

// Check for invalid buffer size

    if (Uargs->se$buf_size < 0)
	{
	USER$Err(Uargs,NET$_BTS);
	return;
	};


//!!HACK!!// Where's the comment?
    Flags = Uargs->se$flags;

// Allocate an output buffer and build an IP packet
//!!HACK!!// This is silly, why not just use the uarg block.
//!!HACK!!// Not possible now, but maybe with a little work...

    // Calc total size of IP packet.  Note: Uargs->se$ext2 is header size.
    USize = Uargs->se$buf_size;
    segsize = Uargs->se$buf_size + Uargs->se$ext2;
//    if (segsize > Max_IP_Data_Size)
//	segsize = Max_IP_Data_Size;

// Use preallocated buffer sizes to reduce dynamic memory load

    bufsize = segsize + DEVICE_HEADER;
    if (bufsize <= min_physical_bufsize)
      bufsize = min_physical_bufsize;
    else
	if (bufsize <= max_physical_bufsize)
	    bufsize = max_physical_bufsize;
    Buf = mm$seg_get(bufsize);	// Get a buffer
//!!HACK!!// Next line is a hack, but it really speeds things up...
    seg = Buf + DEVICE_HEADER; // Point at IP segment

// Copy the user data into the data area

    $$KCALL(MOVBYT,segsize,Uargs->se$data_start,seg);

    ForeignAddr = seg->iph$dest;
    LocalAddr = seg->iph$source;
    Protocol = seg->iph$protocol;

    // Use ip$send_raw if this is an exact packet
    if (Flags&2) // check
	{
	// Send packet exactly as the client passed it.

	// Re-arrange bytes and words in IP header
	swapbytesiphdr ( IP_HDR_SWAP_SIZE , seg );

	// Compute checksum for IP header
	if (Flags&4)
	    {
	    seg->iph$checksum = 0;
	    seg->iph$checksum = Calc_Checksum ( Uargs->se$ext2 , seg );
	    };

	if ($$LOGF(LOG$IP))
	    Log_IP_Packet(seg,FALSE,TRUE);

	RC = SS$_NORMAL;
	if ((ip$send_raw(seg->iph$dest,seg,segsize,1,
			Buf,bufsize) == 0)) RC = NET$_NRT;
	// Post the I/O request back to the user

	user$post_io_status(Uargs,RC,0,0,0);
	mm$uarg_free(Uargs);
	return;
	};

    // Compute Foreign address, source address, and protocol.
    if (ForeignAddr == WILD)
	ForeignAddr = IPCB->ipcb$foreign_host;
    if ((ForeignAddr == WILD))
	{
	mm$seg_free(bufsize,Buf);	// Give back buffer
	USER$Err(Uargs,NET$_NOPN);
	return 0;
	};


    if (LocalAddr == WILD)
	IP$SET_HOSTS(1,&ForeignAddr,&LocalAddr,&ForeignAddr);

    if (Protocol == WILD)
	Protocol = IPCB->ipcb$proto_filter;

    if ($$LOGF(LOG$IP))
	Log_IP_Packet(seg,FALSE,TRUE);

// Send the segment to IP (it will deallocate it)

    IPIPID = IPIPID+1;	// Increment packet ID
    RC = SS$_NORMAL;
    if ((ip$send(LocalAddr,ForeignAddr,IPTOS,ipttl,
		   (long)seg + Uargs->se$ext2,USize,
		   IPIPID,IPDF,TRUE,Protocol,
		   Buf,bufsize) == 0)) RC = NET$_NRT;

// Post the I/O request back to the user

    user$post_io_status(Uargs,RC,0,0,0);
    mm$uarg_free(Uargs);
    }



//SBTTL "ipu$receive - receive a IP packet"
/*
    Handle user receive request for IP connection. If there is a packet
    available on the IP receive queue, then deliver it to the user
    immediately. Otherwise, queue up the user receive for later.
 */

void ipu$receive(struct user_recv_args * Uargs) 
    {
	struct IPCB_Structure * IPCB;
	struct queue_blk_structure(qb_nr_fields) * QB;
	struct queue_blk_structure(qb_ur_fields) * URQ;
    signed long
	RC;

// Validate connection ID and get IPCB pointer

    if ((IPCB = IPCB_OK(Uargs->re$local_conn_id,&RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// No such connection
	return;
	};
    XLOG$FAO(LOG$USER,"!%T IPU$RECEIVE: Conn=!XL, IPCB=!XL, Size=!SL!/",
	     0,Uargs->re$local_conn_id,IPCB,Uargs->re$buf_size);

// Check for aborted connection

    if (IPCB->ipcb$aborting)
	{
	XLOG$FAO(LOG$USER,"!%T IPU$RECEIVE for aborted IPCB !XL!/",0,IPCB);
	USER$Err(Uargs,NET$_CC);
	return;
	};

// Check for invalid buffer size

    if (Uargs->re$buf_size <= 0)
	{
	USER$Err(Uargs,NET$_BTS);
	return;
	};

// Make a request block for the receive

    URQ = mm$qblk_get();		// Get a queue block
    URQ->ur$size = Uargs->re$buf_size; // # of bytes this rq can take
    URQ->ur$data = Uargs->re$data_start; // Address of system buffer
    URQ->ur$irp_adrs = Uargs->re$irp_adrs; // IO request packet address
    URQ->ur$ucb_adrs = Uargs->re$ucb_adrs; // Unit Control Block address
    URQ->ur$uargs = Uargs;	// User argument block address

// If anything is available on the queue, deliver it now, else queue for later

    NOINT;
    if (REMQUE(IPCB->ipcb$nr_qhead,&QB) != EMPTY_QUEUE) // check
      Deliver_IP_Data(IPCB,QB,URQ);
    else
	INSQUE(URQ,IPCB->ipcb$usr_qtail);
    OKINT;
    }



//SBTTL "IPU$INFO - get info about IP "connection""
/*
    Read the host names/numbers for a IP connection.
 */

void ipu$info(struct user_info_args * Uargs)
    {
extern	user$net_connection_info ();
	struct IPCB_Structure * IPCB;
    signed long
	RC;

// Validate the connection ID

    if ((IPCB = IPCB_OK(Uargs->if$local_conn_id,&RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Bad connection ID
	return;
	};

// Give the information back (common TCP/IP routine in USER.BLI)

    user$net_connection_info(Uargs,IPCB->ipcb$host_filter,
			IPCB->ipcb$foreign_host,
			0,0,
			IPCB->ipcb$foreign_hname,
			IPCB->ipcb$foreign_hnlen);
    }


//SBTTL "IPU$STATUS - get status of IP "connection""
/*
    This routine is a placeholder for the network STATUS command, which is
    currently implemented for the TCP protocol.
 */

void ipu$status(struct user_status_args * Uargs)
    {
    USER$Err(Uargs,NET$_NYI);
    }

//SBTTL "IPU$CANCEL - Handle VMS cancel for IP connection"
/*
    Handle process abort/$CANCEL request for a IP connection. Identical
    in functionality to IPU$CLOSE/IPU$ABORT except for calling procedure.
 */

ipu$cancel(struct vms$cancel_args * Uargs)
    {
      struct IPCB_Structure * IPCB;
      signed long I,
	Fcount;

    Fcount = 0;

// Check all valid IPCB's looking for a match on pid and channel #.

    for (I=1;I<=MAX_IPCB;I++)
	if ((IPCB = ipcb_table[I]) != 0)
	    {

// If the process doing the cancel owns this connection, then delete it.

	    if ((IPCB->ipcb$user_id == Uargs->vc$pid) &&
	       (IPCB->ipcb$piochan == Uargs->vc$piochan))
		{
		XLOG$FAO(LOG$USER,"!%T IPU$Cancel: IPCB=!XL!/",0,IPCB);
		ipcb_close(I,IPCB,NET$_CCAN);
		Fcount = Fcount + 1;
		};
	    };
    return Fcount;
    }

//SBTTL "IP dump routines"

void ipu$connection_list(RB)
//
// Dump out the list of IP connections.
//
	 D$IP_List_Return_Blk RB;
    {
      signed long I,
	RBIX;
    RBIX = 1;
    for (I=1;I<=MAX_IPCB-1;I++)
	if (ipcb_table[I] != 0)
	    {
	    RB[RBIX] = I;
	    RBIX = RBIX + 1;
	    };
    RB[0] = RBIX - 1;
    }

ipu$ipcb_dump(IPCBIX,RB)
//
// Dump out a single IP connection
//
 d$ipcb_dump_return_blk * RB;
    {
	struct IPCB_Structure * IPCB;
	signed long I,
	Qcount;
	struct queue_blk_structure(qb_nr_fields) * QB;

// Validate that there is a real IPCB there

    if ((IPCBIX < 1) || (IPCBIX > MAX_IPCB) ||
       ((IPCB = ipcb_table[IPCBIX]) == 0))
	return FALSE;

// Copy the IPCB contents

    RB->du$ipcb_address = IPCB;
    RB->du$ipcb_foreign_host = IPCB->ipcb$foreign_host;
    RB->du$ipcb_local_host = IPCB->ipcb$host_filter;
    RB->du$ipcb_flags = IPCB->ipcb$flags;
    RB->du$ipcb_user_id = IPCB->ipcb$user_id;

// Get length of network receive queue

    QB = IPCB->ipcb$nr_qhead;
    Qcount = 0;
    while ((QB != IPCB->ipcb$nr_qhead))
	{
	Qcount = Qcount + 1;
	QB = QB->nr$next;
	};
    RB->du$ipcb_nr_qcount = Qcount;

// Get length of user receive queue

    QB = IPCB->ipcb$usr_qhead;
    Qcount = 0;
    while ((QB != IPCB->ipcb$usr_qhead))
	{
	Qcount = Qcount + 1;
	QB = QB->nr$next; // was: ur$next, but same
	};
    RB->du$ipcb_ur_qcount = Qcount;

// Done.

    return TRUE;
    }
