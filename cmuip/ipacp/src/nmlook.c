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
//TITLE "NMLOOK - Mailbox name lookup service"
//++
// Module:
//
//	NMLOOK - Handle IPACP name lookup requests.
//
// Facility:
//
//	Provides host name and address translation by talking to the system
//	name resolver through mailboxes.
//
// Abstract:
//
//	Exports the following routines for host name processing:
//
//	    NML$CONFIG(IMNAME,PRIOR,STATS,PRIVS,QUOTAS)
//		Get the name resolver process configuration info from the config
//		file. Called from CONFIG module when this information is read.
//	    NML$INIT
//		Initialize name lookup state. Create IPACP mailbox and start
//		name resolver process if none exists. Must be called before any
//		other functions may be performed.
//	    NML$GETALST(NAMPTR,NAMLEN,ASTADR,ASTPRM)
//		Enqueue name translation. AST routine will be called when the
//		request completes. AST routine calling sequence is:
//		(ASTADR)(ASTPRM,<RC>,<ADRCNT>,<ADRLST>,<NAMLEN>,<NAMPTR>)
//	    NML$GETNAME(ADDR,ASTADR,ASTPRM)
//		Enqueue address translation. AST routine will be called when
//		the request completes. AST routine calling sequence is:
//		(ASTADR)(ASTPRM,<RC>,<NAMLEN>,<NAMPTR>)
//	    NML$GETRR(RRTYPE,NAMPTR,NAMLEN,ASTADR,ASTPRM)
//		Enqueue name translation. AST routine will be called when the
//		request completes. AST routine calling sequence is:
//		(ASTADR)(ASTPRM,<RC>,<RDLEN>,<RDATA>,<NAMLEN>,<NAMPTR>)
//	    NML$CANCEL(ASTPRM,ASTFLG,STATUS)
//		Delete an entry from the queue. Returns count of requests
//		found.
//	    NML$STEP(COADDR) (void)
//		Step through the queue, calling the user coroutine for each
//		queue entry.
//	    NML$PURGE(STATUS) (void)
//		Purge the name lookup queue and shutdown the name resolver.
//
// Author:
//
//	Vince Fuller, CMU-CSD, February, 1987
//	Copyright (c) 1987, Vince Fuller and Carnegie-Mellon University
//
// Edit history:
//
// 1.2b	18-Jul-1991	Henry W. Miller		USBR
//	Use LIB$GET_VM_PAGE and LIB$FREE_VM_PAGE rather then LIB$GET_VM
//	and LIB$FREE_VM.
//
// 1.2a	17-Jul-1991	Henry W. Miller		USBR
//	Fix comments from "server" to "resolver".
//	Updated IDENT.
//	Comment out LOG$MSG; now defined in TCPMACROS.
//	Corrected some spelling errors.
//
// 1.2  15-Jun-89, Edit by BRM (Bruce R. Miller)	CMU Network Development
//	Added NML$GETRR routine.
//
//	31-OCT-1988	Dale Moore	CMU-CS/RI
//	Store the name string in dynamic descriptors.  They
//	are easier to work with than descriptors created with
//	LIB$GET_VM.
//
// 1.1  10-Sep-87, Edit by VAF
//	Copy but don't count the null for name lookup requests.
//
// 1.0  24-Feb-87, Edit by VAF
//	Original version.
//--

#if 0
MODULE NMLOOK(IDENT="1.2b",LANGUAGE(BLISS32),
	      ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			      NONEXTERNAL=LONG_RELATIVE),
	      LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	      OPTIMIZE,OPTLEVEL=3,ZIP)
#endif

#include <starlet.h>
     // not yet #include "CMUIP_SRC:[CENTRAL]NETXPORT";
#include <cmuip/central/include/netcommon.h>
#include "netvms.h"
#include <cmuip/central/include/neterror.h>
#include <cmuip/central/include/namsym.h>
#include "tcpmacros.h"

#include <ssdef.h>
#include <descrip.h>
#include <lnmdef.h>
#include <iodef.h>
#include <jpidef.h>

//LITERAL
//!!HACK!!// Why is this temporary
//    LOG$MSG = %X"800";		// Temporary

extern     LIB$GET_VM ();
extern     LIB$GET_VM_PAGE ();
extern     LIB$FREE_VM ();
extern     LIB$FREE_VM_PAGE ();
extern  void    QL_FAO();
extern  void    LOG_FAO();
extern  void    OPR_FAO();
extern     Time_Stamp();
extern     user$clock_base();

extern signed long
    myuic,
    log_state,
    ast_in_progress,
    intdf;


//SBTTL "Define name lookup queue entry"

// The name lookup queue contains an entry for each pending request. The request
// consists of a header describing it, followed by the mailbox message for it.

struct NQENTRY
{
void *     NQE$NEXT	;	// Pointer to next item on queue
void *     NQE$PREV	;	// Pointer to previous item on queue
unsigned short     NQE$ID	;	// Request ID
unsigned short     NQE$TYPE	;	// Request type
unsigned short     NQE$LENGTH	;	// Request buffer length
  union {
unsigned short     NQE$FLAGS	;	// Request flags
    struct{
      unsigned NQE$F_XMIT:1;	// Request has been successfully transmitted
};
};
  long  NQE$TIME;	// Time request was enqueued
void (*     NQE$ASTADR)()	;	// Address of routine to call when done
void *     NQE$ASTPRM	;	// Parameter to AST routine
long long    NQE$IOSB;	// IOSB for mailbox send
char    NQE$DATA[0];	// Mailbox message buffer
    };

#define    NQENTRY_SIZE sizeof(struct NQENTRY)
#define    NQENTRY_BLEN NQENTRY_SIZE*4

#define    NQENTRY(SIZ) NQENTRY


// Definition of a mail message buffer

struct MAIL$BUF
{
long long    MB$IOSB;	// IOSB for receive
char    MB$DATA	[0];	// Data area
    };

#define    MAIL$BUF_SIZE sizeof(struct MAIL$BUF)
#define MAX_MAIL$BUF MAIL$BUF_SIZE + (MSGMAX/4)

#if 0
MACRO
    MAIL$BUF = BLOCK->MAIL$BUF_SIZE FIELD(MAIL$BUF_FIELDS) %,
    MAX_MAIL$BUF = BLOCK[MAIL$BUF_SIZE + (MSGMAX/4)] FIELD(MAIL$BUF_FIELDS) %;
#endif

//SBTTL "Constants and OWN data"

struct dsc$descriptor     SRVPRCNAM_ = ASCID2(6,"NAMRES"), *SRVPRCNAM=&SRVPRCNAM_;	// Name of the resolver process
struct dsc$descriptor     SYSTABNAM_ = ASCID2(15,"LNM$SYSTEM_TABLE"), *SYSTABNAM=&SYSTABNAM_; // Logical name of system table
struct dsc$descriptor     SRVMBXNAM_ = ASCID2(10,"NAMRES_MBX"), *SRVMBXNAM=&SRVMBXNAM_; // System-wide logical name of resolver mailbox
struct dsc$descriptor     ACPMBXNAM_ = ASCID2(9 ,"IPACP_MBX"), *ACPMBXNAM=&ACPMBXNAM_; // System-wide logical name of ACP mailbox
struct dsc$descriptor     MYMBXNAM_ = ASCID2(9,"IPACP_MBX"), *MYMBXNAM=&MYMBXNAM_; // Temporary name of ACP mailbox
long    ACPMBXPRO = 0xFF00;	// Protection: (W:<no>,G:<no>,O:RWLP,S:RWLP)

#define     MBAMAX   MBNMAX*2		// Max mailbox string length
#define     MSGCNT   10		// Number of receive messages to buffer
#define     NQE_QLIMIT   20		// Max length of request queue
#define     SRV$DOWN   -1		// Resolver is unavailable
#define     SRV$INIT   0		// Resolver is initializing
#define     SRV$UP   1			// Resolver is up

static struct MAIL$BUF    RCVBUF[MSGMAX];	// Receive buffer
static struct queue_header_fields    NQE_QUEUE_ = {	// Request queue
	qhead: &NQE_QUEUE_,
	qtail:&NQE_QUEUE_}, * NQE_QUEUE = &NQE_QUEUE;
static signed long
    SRVSTATE,			// Resolver state
    SRVMBXCHN,			// I/O channel on nameresolver mailbox
    SRVPID,			// PID of the nameresolver
    ACPMBXCHN,			// I/O channel on ACP mailbox
    MYMBA_BUF[MBAMAX],
    MYMBA_LEN,
    MYMBA_ATTR,
    NQE_COUNT,			// Count of blocks currently allocated
    CURQID,			// Current request ID
    SRVPRIOR,			// Name resolver priority
    SRVSTATUS,			// Name resolver status flags
    SRVPRIVS[2],	// Name resolver privileges
     * SRVQUOTAS;	// Name resolver quotas
 struct dsc$descriptor   SRVIMGNAME_ = {	// Name resolver process name
			dsc$w_length:0,
			dsc$b_dtype:DSC$K_DTYPE_T,
			dsc$b_class:DSC$K_CLASS_D,
			 dsc$a_pointer:0},*SRVIMGNAME=&SRVIMGNAME;

//SBTTL "NML$CONFIG - Configure name resolver process"

void NML$CONFIG(IMNAME_A,PRIOR,STATUS,PRIVS, QUOTAS)
//
// Copy the configuration information about the name resolver into this module.
// This information is used by NML$INIT when creating the name resolver process.
//
long     PRIVS	[2];
struct dsc$descriptor	* QUOTAS;
    {
	long IMNAME	= IMNAME_A;
    extern STR$COPY_DX();
    signed long
	RC,
	STRPTR,
	QUOPTR;

// Setup the descriptor of for the image name and allocate the string

    RC = STR$COPY_DX (SRVIMGNAME, IMNAME);
    if (! RC)
	FATAL$FAO("NML$CONFIG - failed to allocate string, RC = !XL",RC);

// Allocate the block for the process quota list

    RC = LIB$GET_VM(/*%REF*/(QUOTAS->dsc$w_length),&QUOPTR);
    if (! RC)
	FATAL$FAO("NML$CONFIG - failed to allocate quolst, RC = !XL",RC);
    CH$MOVE(QUOTAS->dsc$w_length,QUOTAS->dsc$a_pointer,QUOPTR);
    SRVQUOTAS = QUOPTR;

// Copy the remaining values into their appropriate places

    SRVPRIOR = PRIOR;		// Resolver priority
    SRVSTATUS = STATUS;	// Resolver process status flags
    SRVPRIVS[0] = PRIVS[0];	// Resolver privileges
    SRVPRIVS[1] = PRIVS[1];
    }

//SBTTL "NML$INIT - Initialize module state"

void    MBX_RECV_AST();
     CHECK_SERVER();
     void    SEND_CONTROL();

void NML$INIT (void)
//
// Create a mailbox for this process and start the name resolver.
// The name resolver will let us know when it is ready to start accepting
// requests.
//
    {
    signed long
      RC;
    struct item_list_3 ITMLIST[3];

// Initialize state of service to unavailable

    SRVSTATE = SRV$INIT; // was: SRV$DOWN; check
    SRVMBXCHN = 0;
    SRVPID = -1;

// Initialize the request queue and initial request number

    CURQID = user$clock_base();
    NQE_QUEUE->qhead = NQE_QUEUE->qtail = NQE_QUEUE;
    NQE_COUNT = 0;

// Create a mailbox for us

#if 0
    // not yet
    RC = exe$crembx(0, &ACPMBXCHN,
		 MSGMAX,
		 MSGMAX*MSGCNT,
		 ACPMBXPRO, 0,
		 MYMBXNAM, 0, 0);
#else
    RC = 1;
#endif
    if (! RC)
	{
	ERROR$FAO("Failed to create ACP mailbox, RC = !XL",RC);
	return;
	};

// Get the real name of the mailbox and create a system-wide logical to point
// to it. We want the name to be accessable to all, but for the mailbox to go
// away if this process aborts.

    ITMLIST[0].item_code=LNM$_ATTRIBUTES;
    ITMLIST[0].bufaddr=MYMBA_ATTR;
    ITMLIST[1].item_code=LNM$_STRING;
    ITMLIST[1].bufaddr=MYMBA_BUF;
    ITMLIST[1].buflen=MBAMAX;
    ITMLIST[1].retlenaddr=MYMBA_LEN;
    ITMLIST[2].item_code=0;
    ITMLIST[2].buflen=0;
    ITMLIST[2].bufaddr=0;

    RC = exe$trnlnm(LNM$M_CASE_BLIND,
		 ASCID("LNM$TEMPORARY_MAILBOX"),
		 MYMBXNAM, 0,
		 ITMLIST);
    if (! RC)
	{
	ERROR$FAO("$TRNLNM failed for ACP mailbox, RC = !XL",RC);
	return;
	};
    if ((MYMBA_ATTR && LNM$M_EXISTS) == 0)
	{
	ERROR$FAO("$TRNLNM claims mailbox name nonexistant!!");
	return;
	};

// Now, create the system-wide logical to point at it

ITMLIST[0].item_code=LNM$_STRING;
ITMLIST[0].bufaddr=MYMBA_BUF;
ITMLIST[0].buflen=MYMBA_LEN;
ITMLIST[1].item_code=0;
ITMLIST[1].buflen=0;
ITMLIST[1].bufaddr=0;

    RC = exe$crelnm(0 , SYSTABNAM,
		    ACPMBXNAM, 0,
		 ITMLIST);
    if (! RC)
	{
	ERROR$FAO("$CRELNM failed for ACP mailbox, RC = !XL",RC);
	return;
	};

// Start an initial read on the mailbox

    RC = exe$qio(0, ACPMBXCHN, IO$_READVBLK,
		RCVBUF->MB$IOSB,
		MBX_RECV_AST,
		 RCVBUF,
		 RCVBUF->MB$DATA,
		 MSGMAX, 0, 0, 0, 0);
    if (! RC)
	{
	ERROR$FAO("Queued read failed for mailbox, RC = !XL",RC);
	return;
	};

// Check to see if NAMRES mailbox exists yet. If not, we'll start the resolver.

    if (CHECK_SERVER())
	{
	SEND_CONTROL(CNRQ$START,0); // Tell it that the network is here
	SRVSTATE = SRV$UP;	// Resolver should be ready now
	}
    else
	{

// Make sure the initialization was done properly.

	if (SRVIMGNAME->dsc$w_length == 0)
	    {
	    ERROR$FAO("Failed to create NAMRES - configuration info missing");
	    SRVSTATE = SRV$DOWN;
	    return;
	    };

// Start the name resolver process - it will notify us when it is ready.

	RC = exe$creprc(SRVPID, SRVIMGNAME,0,0,0,
		     SRVPRIVS,
			SRVQUOTAS,
		     SRVPRCNAM,
		     SRVPRIOR,
		     myuic,
			0, SRVSTATUS, 0, 0, 0
		     );
	if (! RC)
	    {
	    ERROR$FAO("$CREPRC for NAMRES failed, RC = !XL",RC);
	    return;
	    };
	SRVSTATE = SRV$INIT;	// Resolver is initializing
	};
    }

//SBTTL "NML$GETALST - Translate name to address list"

NQE_ALLOC();
void    NQE_DEALLOC();
void    NQE_ENQUEUE();
NQE_XMIT();

void NML$GETALST(NAMPTR,NAMLEN,ASTADR,ASTPRM)
//
// Request a name to address translation. Build the request and transmit it
// to the name resolver.  Receive decode routine will finish the request when
// a reply has been received for it.
//
     int (*ASTADR)();
    {
    signed long
	RC,
	CPTR,
      MSLEN;
    struct NQENTRY * NQE;
    struct MAIL$MSG * MSBUF;
	struct RQ$NMLOOK * RQBUF;

// Log the request

    XLOG$FAO(LOG$MSG,"!%T NML$GETALST: name is !AD!/",0,NAMLEN,NAMPTR);

// Allocate and initialize a request block for us

    RC = NQE_ALLOC(&NQE);
    if (! RC)
	{
	(ASTADR)(ASTPRM,RC);
	return;
	};

// Build the request in it.

    MSBUF = NQE->NQE$DATA;
    RQBUF = MSBUF->MSG$DATA;
    RQBUF->RQNM$TYPE = NLRQ$NMLOOK;
    RQBUF->RQNM$ID = (CURQID = CURQID + 1);

// Note: the -1 is because the count included the null. We copy the null but
// don't count it in the buffer for NAMRES.

    RQBUF->RQNM$NAMLEN = NAMLEN-1;
    CPTR = CH$MOVE(NAMLEN,NAMPTR,CH$PTR(RQBUF->RQNM$NAMSTR,0));
    MSLEN = CH$DIFF(CPTR,CH$PTR(MSBUF,0));

// Enqueue the request and transmit it.

    NQE->NQE$TYPE = NLRQ$NMLOOK;
    NQE->NQE$ASTADR = ASTADR;
    NQE->NQE$ASTPRM = ASTPRM;
    NQE_ENQUEUE(NQE,CURQID,MSLEN,MSBUF);
    }

//SBTTL "NML$GETNAME - Translate address to name"

void NML$GETNAME(ADDR,ASTADR,ASTPRM)
//
// Request an address to name translation. Build the request and send it to
// the name resolver.  Receive decode routine will finish the request when the
// reply comes in.
//
     int (*ASTADR)();
    {
    signed long
	RC,
      MSLEN;
    struct NQENTRY * NQE;
    struct MAIL$MSG * MSBUF;
	struct RQ$ADLOOK * RQBUF;

// Log the request

    if ($$LOGF(LOG$MSG))
	{
	signed long
	    INA;
	INA = ADDR;
#if 0
	QL$FAO("!%T NML$GETNAME: address is !UB.!UB.!UB.!UB!/",
	       0,INA[0],INA[1],INA[2],INA[3]);
#endif
	};

// Allocate and initialize a request block for us

    RC = NQE_ALLOC(&NQE);
    if (! RC)
	{
	(ASTADR)(ASTPRM,RC);
	return;
	};

// Build the request in it.

    MSBUF = NQE->NQE$DATA;
    RQBUF = MSBUF->MSG$DATA;
    RQBUF->RQAD$TYPE = NLRQ$ADLOOK;
    RQBUF->RQAD$ID = (CURQID = CURQID + 1);
    RQBUF->RQAD$ADDR = ADDR;
    MSLEN = RQ$ADLOOK_BLEN + MAIL$MSG_BLEN;

// Enqueue the request and transmit it.

    NQE->NQE$TYPE = NLRQ$ADLOOK;
    NQE->NQE$ASTADR = ASTADR;
    NQE->NQE$ASTPRM = ASTPRM;
    NQE_ENQUEUE(NQE,CURQID,MSLEN,MSBUF);
    }

//SBTTL "NML$GETRR - Translate name to resource record"

void NML$GETRR(RRTYPE,NAMPTR,NAMLEN,ASTADR,ASTPRM)
//
// Request a name to RR translation. Build the request and transmit it
// to the name resolver.  Receive decode routine will finish the request when
// a reply has been received for it.
//
     int (*ASTADR)();
    {
    signed long
	RC,
	CPTR,
      MSLEN;
    struct NQENTRY * NQE;
    struct MAIL$MSG * MSBUF;
	struct RQ$RRLOOK * RQBUF;

// Log the request

    XLOG$FAO(LOG$MSG,"!%T NML$GETRR: type is !XL, name is !AD!/",0,
		RRTYPE, NAMLEN, NAMPTR);

// Allocate and initialize a request block for us

    RC = NQE_ALLOC(&NQE);
    if (! RC)
	{
	(ASTADR)(ASTPRM,RC);
	return;
	};

// Build the request in it.

    MSBUF = NQE->NQE$DATA;
    RQBUF = MSBUF->MSG$DATA;
    RQBUF->RQRR$TYPE = NLRQ$RRLOOK;
    RQBUF->RQRR$ID = (CURQID = CURQID + 1);

// Note: the -1 is because the count included the null. We copy the null but
// don't count it in the buffer for NAMRES.

    RQBUF->RQRR$RRTYPE = RRTYPE;
    RQBUF->RQRR$NAMLEN = NAMLEN-1;
    CPTR = CH$MOVE(NAMLEN,NAMPTR,CH$PTR(RQBUF->RQRR$NAMSTR,0));
    MSLEN = CH$DIFF(CPTR,CH$PTR(MSBUF,0));

// Enqueue the request and transmit it.

    NQE->NQE$TYPE = NLRQ$RRLOOK;
    NQE->NQE$ASTADR = ASTADR;
    NQE->NQE$ASTPRM = ASTPRM;
    NQE_ENQUEUE(NQE,CURQID,MSLEN,MSBUF);
    }

//SBTTL "NML$CANCEL - Cancel name lookup request"

NML$CANCEL(ASTPRM,ASTFLG,STATUS)
//
// Search the name lookup queue for the specified request and delete it,
// optionally calling the "done" routine with the specified status.
//
    {
      struct NQENTRY * NQE;
      struct NQENTRY * NXNQE;
    signed long
	RC;

// Search the queue for the request

    RC = 0;
    NQE = NQE_QUEUE->qhead;
    while (NQE != NQE_QUEUE)
	{
	NXNQE = NQE->NQE$NEXT;

// On match, optionally call the AST routine then delete the request.

	if (NQE->NQE$ASTPRM == ASTPRM)
	    {
	    XQL$FAO(LOG$MSG,"!%T NML$CANCEL - Deleting NQE !XL!/",0,NQE);
	    RC = RC + 1;
	    REMQUE(NQE,NQE);
	    if (ASTFLG != 0)
		(NQE->NQE$ASTADR)(NQE->NQE$ASTPRM,STATUS);
	    NQE_DEALLOC(NQE);
	    };

// Advance to next request

	NQE = NXNQE;
	};

// Return the count of requests found and deleted

    return RC;
    }

//SBTTL "NML$STEP - Examine the name lookup queue, calling coroutine"

void NML$STEP(COADDR,COVALUE)
//
// Examine the name lookup queue, calling the user coroutine with the AST
// address and AST parameter of the entry. This makes it easy for the CANCEL
// handler in the ACP to find any requests belonging to a process.
//
     int (*COADDR)();
    {
      struct NQENTRY * NQE;
	struct NQENTRY * NXNQE;

// Walk the queue

    NQE = NQE_QUEUE->qhead;
    while (NQE != NQE_QUEUE)
	{
	NXNQE = NQE->NQE$NEXT;
	(COADDR)(COVALUE,NQE->NQE$ASTADR,NQE->NQE$ASTPRM);
	NQE = NXNQE;
	};
    }

void NML$PURGE(STATUS)
//
// Purge the name lookup queue and tell the name resolver to shutdown.
// Called just before the ACP exits.
//
    {
    signed long
      RC;
    struct NQENTRY * NQE;
	struct NQENTRY * NXNQE;

// Tell the name resolver that the network is exiting.

    if (SRVMBXCHN != 0)
	SEND_CONTROL(CNRQ$STOP,STATUS);

// Flush our mailbox logical name

    RC = exe$dellnm(SYSTABNAM,
		 ACPMBXNAM, 0);
    if (! RC)
	ERROR$FAO("$DELLNM failed for !AS, RC = !XL",ACPMBXNAM,RC);

// Delete our mailbox

    RC = exe$delmbx(ACPMBXCHN);
    if (! RC)
	ERROR$FAO("$DELMBX failed for ACP mailbox, RC = !XL",RC);

// Walk the queue, purging all requests

    NQE = NQE_QUEUE->qhead;
    while (NQE != NQE_QUEUE)
	{
	NXNQE = NQE->NQE$NEXT;
	REMQUE(NQE,NQE);
	(NQE->NQE$ASTADR)(NQE->NQE$ASTPRM,STATUS);
	NQE_DEALLOC(NQE);
	NQE = NXNQE;
	};
    }

//SBTTL "Debugging routine to dump out the queue contents"

 void    NQE_DUMP();

void NML$DUMP (void)
//
// Walk the queue, dumping out each entry. For debugging purposes only.
//
    {
      struct NQENTRY * NQE;
      struct NQENTRY * PNQE;
    signed long
	NOW;

    NOW = Time_Stamp();
    LOG$FAO("NQE count is !SL, TIME is !XL, QHEAD=!XL, QTAIL=!XL!/",
	    NQE_COUNT,NOW,NQE_QUEUE->qhead,NQE_QUEUE->qtail);
    NQE = NQE_QUEUE->qhead;
    PNQE = NQE_QUEUE;
    while (NQE != NQE_QUEUE)
	{
	NQE_DUMP(NQE);
	if (NQE->NQE$PREV != PNQE)
	    {
	    LOG$FAO("** List link error, PREV should be !XL **!/",PNQE);
	    break;
	    };
	PNQE = NQE;
	NQE = NQE->NQE$NEXT;
	};
    }


void NQE_DUMP(struct NQENTRY * NQE)
//
// Dump out a single queue entry.
//
    {
      LOG$FAO(/*%STRING*/("NQE at !XL, NEXT=!XL, PREV=!XL!/",
		    "  TYPE=!UL, LEN=!UL, FLAGS=!XW, TIME=!XL!/",
		    "  ASTADR=!XL, ASTPRM=!XL!/"),
	    NQE,NQE->NQE$NEXT,NQE->NQE$PREV,
	    NQE->NQE$TYPE,NQE->NQE$LENGTH,NQE->NQE$FLAGS,NQE->NQE$TIME,
	    NQE->NQE$ASTADR,NQE->NQE$ASTPRM);
    }

void SEND_CONTROL(CCODE,CVALUE)
//
// Build and send a control message to the name resolver.
//
    {
#define	CONTROL_MSGSIZE RQ$CONTROL_BLEN + MAIL$MSG_BLEN
    static
	struct MAIL$MSG MSBUF[MAIL$BUF_SIZE + (MSGMAX/4)] ;
    signed long
      RC;
	struct RQ$CONTROL * RQBUF;

// Set up the request buffer

    RQBUF = MSBUF->MSG$DATA;
    RQBUF->RQCN$TYPE = NLRQ$CONTROL;
    RQBUF->RQCN$ID = (CURQID = CURQID + 1);
    RQBUF->RQCN$CCODE = CCODE;
    RQBUF->RQCN$CVALUE = CVALUE;

// Indicate no reply wanted in message header

    MSBUF->MSG$MBXNLN = 0;

// Send the message off to the name resolver

    RC = exe$qio(0, SRVMBXCHN,	IO$_WRITEVBLK || IO$M_NOW, 0,
		MSBUF,
		CONTROL_MSGSIZE);
    if (! RC)
	ERROR$FAO("Failed to send NAMRES control message, RC = !XL",RC);
    }

//SBTTL "Request queue management routines"

#define    NQE_MAXSIZE NQENTRY_BLEN + MSGMAX

NQE_ALLOC(NQE)
//
// Allocate a full-size queue block from dynamic memory. Sets the NQE parameter
// to the address of the block on success. On failure, returns network error
// code indicating the failure reason.
//
     long * NQE;
    {
    signed long
	RC;

// Verify that we should do this.

    if (SRVSTATE == SRV$DOWN)
	return NET$_NONS;
    if (NQE_COUNT >= NQE_QLIMIT)
	return NET$_NSQFULL;

// Allocate the block

    NQE_COUNT = NQE_COUNT + 1;
//    RC = LIB$GET_VM(%REF(NQE_MAXSIZE),NQE);
    RC = LIB$GET_VM_PAGE(/*%REF*/((NQE_MAXSIZE / 512) + 1),NQE);
    XQL$FAO(LOG$MSG,"!%T NQE_ALLOC, NQE=!XL, RC=!XL!/",0,*NQE,RC);
    return RC;
    }



void NQE_DEALLOC(NQE)
//
// Deallocate a queue block, decrementing count.
//
	struct NQENTRY * NQE;
    {
    signed long
	RC;

// Deallocate the memory, using LIB$FREE_VM

//    RC = LIB$FREE_VM(%REF(NQE_MAXSIZE),NQE);
    RC = LIB$FREE_VM_PAGE(/*%REF*/((NQE_MAXSIZE / 512) + 1),NQE);
    if (! RC)
	FATAL$FAO("NQE_DEALLOC - LIB$FREE_VM failure, RC = !XL",RC);
    XQL$FAO(LOG$MSG,"!%T NQE_DEALLOC, NQE = !XL!/",0,NQE);

// Decrement count of allocated blocks and return success

    NQE_COUNT = NQE_COUNT - 1;
    return SS$_NORMAL;
    }

void NQE_ENQUEUE(NQE,QRYID,MSLEN,MSBUF)
//
// Insert a new request onto the request queue and transmit it.
// Fills in the message header, puts it on NQE_QUEUE, and calls NQE_XMIT to
// transmit the query.
//
     struct NQENTRY * NQE;
	struct MAIL$MSG * MSBUF;
    {

// First, fill in our return mailbox in the message header.

    MSBUF->MSG$MBXNLN = MYMBA_LEN;
    CH$MOVE(MYMBA_LEN,CH$PTR(MYMBA_BUF,0),CH$PTR(MSBUF->MSG$MBXNAM,0));

// Next, finish setting up the request header

    NQE->NQE$TIME = Time_Stamp();
    NQE->NQE$LENGTH = MSLEN;
    NQE->NQE$ID = QRYID;
    NQE->NQE$F_XMIT = 0;

// Insert it into the queue

    INSQUE(NQE,NQE_QUEUE->qtail);

// Do logging, if necessary

    XQL$FAO(LOG$MSG,"!%T NQE_ENQUEUE, NQE=!XL, ID=!XL, LEN=!SL, TIME=!UL!/",
	    0,NQE,NQE->NQE$ID,MSLEN,NQE->NQE$TIME);

// Transmit the request if the resolver is ready to take it.

    if (SRVSTATE >= SRV$UP)
	NQE_XMIT(NQE);
    }

void NQE_DELETE(NQE,ASTFLG,STATUS)
//
// Delete an entry from the queue, optionally calling the AST routine with the
// specified status code.
//
	struct NQENTRY * NQE;
    {

    XQL$FAO(LOG$MSG,"!%T NQE_DELETE of NQE !XL, ID !XL!/",0,NQE,NQE->NQE$ID);

// First, unlink the request from the queue

    REMQUE(NQE,NQE);

// Next, call the AST routine if that is requested

    if (ASTFLG != 0)
	(NQE->NQE$ASTADR)(NQE->NQE$ASTPRM,STATUS);

// Finally, deallocate the queue entry

    NQE_DEALLOC(NQE);
    }


 void    NQE_XMIT_DONE();

NQE_XMIT(NQE)
//
// Transmit a request to the name resolver.  Returns SS$_NORMAL on success, or
// $QIO failure code on failure.
//
	struct NQENTRY * NQE;
    {
    signed long
	RC;

    XQL$FAO(LOG$MSG,"!%T NQE_XMIT of NQE !XL, ID !XL!/",0,NQE,NQE->NQE$ID);
    RC = exe$qio(0, 	SRVMBXCHN,
		IO$_WRITEVBLK,
		NQE->NQE$IOSB,
		 NQE_XMIT_DONE,
		NQE,
		NQE->NQE$DATA,
		NQE->NQE$LENGTH, 0, 0, 0 ,0);

// Check the state of the send. We should probably do something useful here if
// it fails (like shutdown the name service and report the error to the opr).

    if (! RC)
	{
	XQL$FAO(LOG$MSG,"!%T NQE_XMIT failed, RC=!XL, NQE=!XL, ID=!XL!/",
		0,RC,NQE,NQE->NQE$ID);
	return RC;
	};
    return SS$_NORMAL;
    }

void NQE_XMIT_DONE(NQE)
//
// AST routine for mailbox send done.
//
	struct NQENTRY * NQE;
    {
    signed long
	RC;
    struct MBX$IOSB * IOSB;

// Check the status & set transmit done flag. We should probably do something
// useful if an error occurs here.

    IOSB = NQE->NQE$IOSB;
    RC = IOSB->MI$STATUS;
    XQL$FAO(LOG$MSG,"!%T NQE_XMIT_DONE, NQE=!XL, RC=!XL!/",0,NQE,RC);
    NQE->NQE$F_XMIT = 1;
    }

void XMIT_REQUESTS (void)
//
// Transmit all of the pending requests when the name resolver has come online.
//
    {
      struct NQENTRY * NQE;
	struct NQENTRY * NXNQE;

// Loop for the entire queue transmitting any requests which have not been
// tranmitted successfully yet.

    NQE = NQE_QUEUE->qhead;
    while (NQE != NQE_QUEUE)
	{
	NXNQE = NQE->NQE$NEXT;
	if (! (NQE->NQE$F_XMIT))
	    NQE_XMIT(NQE);
	NQE = NXNQE;
	};
    }

//SBTTL "Mailbox message receiving routines"

 void    DECODE_REPLY();

void MBX_RECV_AST(MBUF)
//
// Come here when mailbox read completes. Read and decode the message.
// Note that currently, we use a static buffer for reading the incoming message,
// !!!HACK!!!
// though this code can easily handle dynamic buffers.
//
	struct MAIL$BUF * MBUF;
    {
    signed long
	RC,
	RCVLEN,
      RCVPID;
    struct MAIL$MSG * RCVMSG;
	struct MBX$IOSB * RCVIOSB;

// Get the status of the message

    RCVMSG = MBUF->MB$DATA;
    RCVIOSB = MBUF->MB$IOSB;
    RC = RCVIOSB->MI$STATUS;

// Check error

    if (! RC)
	{
	FATAL$FAO("Mailbox read failure, RC = !XL",RC);
	return;
	};

// Success. Retrieve message length and PID; call decoding routine.

    RCVLEN = RCVIOSB->MI$COUNT-MAIL$MSG_BLEN;
    RCVPID = RCVIOSB->MI$PID;
    XQL$FAO(LOG$MSG,"!%T NS msg received, LEN=!SL, PID=!XL!/",
	    0,RCVIOSB->MI$COUNT,RCVPID);
    if (RCVLEN > 0)
	DECODE_REPLY(RCVPID,RCVLEN,RCVMSG->MSG$DATA);

// Queue up another read

    RC = exe$qio(0, ACPMBXCHN,	IO$_READVBLK,
		RCVIOSB,
		 MBX_RECV_AST,
		 MBUF,
		RCVMSG,
		 MSGMAX, 0, 0, 0, 0);
    if (! RC)
	{
	ERROR$FAO("Queued read failed for mailbox, RC = !XL",RC);
	return;
	};
    }


 void    CONTROL_MSG();

void DECODE_REPLY(PID,RLEN,RBUF)
	struct RPLY$DEFAULT * RBUF;
    {
     int (*ASTADR)();
    signed long
	RTYPE,
	RID,
	FOUND,
	NAMLEN,
	NAMPTR,
	ASTPRM;
    struct NQENTRY * NQE;
    struct NQENTRY * NXNQE;

// Handle control message specially

    RTYPE = RBUF->RPLY$TYPE;
    if (RTYPE == NLRP$CONTROL)
	{
	CONTROL_MSG(PID,RLEN,RBUF);
	return;
	};
    if ((RTYPE < NLRP$MIN) || (RTYPE > NLRP$MAX))
	{
	XQL$FAO(LOG$MSG,"!%T Unknown NS msg type !SL received, PID=!XL!/",
		0,RTYPE,PID);
	return;
	};

// For all others, find the request that this reply is for and handle it.

    FOUND = 0;
    RID = RBUF->RPLY$ID;
    NQE = NQE_QUEUE->qhead;
    while (NQE != NQE_QUEUE)
	{
	NXNQE = NQE->NQE$NEXT;

// Check the ID of the reply against the request ID...

	if (RID == NQE->NQE$ID)
	    {

// Have a match. Dequeue & finish request according to the reply type

	    FOUND = FOUND + 1;
	    REMQUE(NQE,NQE);
	    ASTADR = NQE->NQE$ASTADR;
	    ASTPRM = NQE->NQE$ASTPRM;
	    if ( RTYPE >= NLRP$MIN && RTYPE <= NLRP$MAX) 
	      switch (RTYPE)
		{
	    case NLRP$ERROR:	// Error reply - abort request
		{
		    struct RPLY$ERROR * RBUF2=RBUF;

		XQL$FAO(LOG$MSG,"!%T NS error reply, ID=!XL, ERR=!XL!/",
			0,RBUF2->RPER$ID,RBUF2->RPER$ECODE);
		(ASTADR)(ASTPRM,RBUF2->RPER$ECODE);
		};
		break;

	    case NLRP$CONTROL:	// Not possible at this level
		0;
		break;

	    case NLRP$NMLOOK:	// Name lookup reply
		{
		    struct RPLY$NMLOOK * RBUF2 = RBUF;
		signed long
		    ADRCNT,
		    ADRLST;

		ADRCNT = RBUF2->RPNM$ADRCNT;
		ADRLST = RBUF2->RPNM$ADRLST;
		NAMLEN = RBUF2->RPNM$NAMLEN;
		NAMPTR = RBUF2->RPNM$NAMSTR;
		XQL$FAO(LOG$MSG,
			"!%T NS NMLOOK reply, ID=!XL, #ADDR=!SL, NAME=!AD!/",
			0,RBUF2->RPNM$ID,ADRCNT,NAMLEN,NAMPTR);
		(ASTADR)(ASTPRM,SS$_NORMAL,ADRCNT,ADRLST,NAMLEN,NAMPTR);
		};
		break;

	    case NLRP$ADLOOK:	// Address lookup reply
		{
		    struct RPLY$ADLOOK * RBUF2 = RBUF;

		NAMLEN = RBUF2->RPAD$NAMLEN;
		NAMPTR = RBUF2->RPAD$NAMSTR;
		XQL$FAO(LOG$MSG,"!%T NS ADLOOK reply, ID=!XL, name=!AD!/",
			0,RBUF2->RPAD$ID,NAMLEN,NAMPTR);
		(ASTADR)(ASTPRM,SS$_NORMAL,NAMLEN,NAMPTR);
		};
		break;
	    case NLRP$RRLOOK:	// RR lookup reply
		{
		    struct RPLY$RRLOOK * RBUF2=RBUF;
		signed long
		    RDLEN,
		    RDATA;

		RDLEN = RBUF2->RPRR$RDLEN;
		RDATA = RBUF2->RPRR$RDATA;
//		NAMLEN = RBUF2->RPRR$NAMLEN;
//		NAMPTR = RBUF2->RPRR$DATA + RDLEN;
		XQL$FAO(LOG$MSG,
			"!%T NS RRLOOK reply, ID=!XL, SIZE=!SL, RDATA=!AD /",
			0,RBUF2->RPRR$ID,RDLEN,
			RDLEN,RDATA);
		(ASTADR)(ASTPRM,SS$_NORMAL,RDLEN,RDATA,NAMLEN,NAMPTR);
		};
		break;
	    default:	// Not possible at this level
	    0;
	    };

// Deallocate the queue entry

	    NQE_DEALLOC(NQE);
	    };

// Check next queue entry...

	NQE = NXNQE;
	};

// If the request wasn't found in the queue, report the error

    if (FOUND <= 0)
	XQL$FAO(LOG$MSG,"!%T DECODE_REPLY failed to find RQ !XL, TYPE !SL!/",
		0,RID,RTYPE);
    }

void CONTROL_MSG(PID,RLEN,RBUF)
//
// Handle a resolver control message. We are interested in being told when
// either the name resolver goes away or when it has become ready. Note that
// control messages are generally one-way, so we don't care about control
// replies.
//
	struct RQ$CONTROL * RBUF;
    {
    signed long
	CTYPE;

// Dispatch the control types we handle.

    CTYPE = RBUF->RQCN$CCODE;
    switch (CTYPE)
      {
    case CNRQ$START:		// Resolver has finished starting
	{
	XQL$FAO(LOG$MSG,"!%T NS Control: NS is up, PID=!XL!/",0,PID);
	if (CHECK_SERVER())
	    {
	    SRVSTATE = SRV$UP;	// Indicate that everything is ready
	    XMIT_REQUESTS();	// And send all queued requests to the resolver
	    };
	};
	break;

    case CNRQ$STOP:		// Resolver is shutting down
	{
	XQL$FAO(LOG$MSG,"!%T NS Control: NS shutting down, PID=!XL!/",0,PID);
	exe$dassgn(SRVMBXCHN);
	SRVSTATE = SRV$DOWN;	// Service is unavailable
	SRVMBXCHN = 0;		// No channel defined
	SRVPID = 0;		// And no PID defined
	};
	break;

    default:
	{
	XQL$FAO(LOG$MSG,"!%T Unknown NS control msg, CCODE=!SL, PID=!XL!/",
		0,CTYPE,PID);
	};
    };
    }

CHECK_SERVER (void)
//
// Check for the existance of the NAMRES process and the name resolver system
// logical name. If both are found, set SRVMBXCHN and SRVPID accordingly and
// return TRUE. Else, return FALSE.
//
    {
    signed long
	RC,
	DEVCHN,
      PID;
struct item_list_3 ITMLIST[3];

// First, try to assign a channel on the name resolver mailbox.

    RC = exe$assign( SRVMBXNAM,
		 DEVCHN, 0, 0, 0);
    if (! RC)
	return FALSE;

// Now, look for a process with the name "NAMRES"
// N.B. NAMRES must run with the same group as IPACP for this to work. This is
// desirable in any case, since we don't want imposters claiming to be the
// name resolver process.

    ITMLIST[0].item_code=JPI$_PID;
    ITMLIST[0].bufaddr=PID;
    ITMLIST[1].item_code=0;
    ITMLIST[1].buflen=0;
    ITMLIST[1].bufaddr=0;

    RC = exe$getjpiw(0, 0, SRVPRCNAM,
		  ITMLIST, 0, 0, 0);
    if (! RC)
	{
	exe$dassgn(DEVCHN);
	return FALSE;
	};

// Got what we want. Set up the info & return.

    XQL$FAO(LOG$MSG,"!%T NS is UP, PID is !XL!/",0,PID);
    SRVMBXCHN = DEVCHN;
    SRVPID = PID;
    return TRUE;
    }
