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
!++
// Module:
!
!	NMLOOK - Handle IPACP name lookup requests.
!
// Facility:
!
!	Provides host name and address translation by talking to the system
!	name resolver through mailboxes.
!
// Abstract:
!
!	Exports the following routines for host name processing:
!
VOID !	    NML$CONFIG(IMNAME,PRIOR,STATS,PRIVS,QUOTAS)
!		Get the name resolver process configuration info from the config
!		file. Called from CONFIG module when this information is read.
VOID !	    NML$INIT
!		Initialize name lookup state. Create IPACP mailbox and start
!		name resolver process if none exists. Must be called before any
!		other functions may be performed.
VOID !	    NML$GETALST(NAMPTR,NAMLEN,ASTADR,ASTPRM)
!		Enqueue name translation. AST routine will be called when the
!		request completes. AST routine calling sequence is:
!		(ASTADR)(ASTPRM,<RC>,<ADRCNT>,<ADRLST>,<NAMLEN>,<NAMPTR>)
VOID !	    NML$GETNAME(ADDR,ASTADR,ASTPRM)
!		Enqueue address translation. AST routine will be called when
!		the request completes. AST routine calling sequence is:
!		(ASTADR)(ASTPRM,<RC>,<NAMLEN>,<NAMPTR>)
VOID !	    NML$GETRR(RRTYPE,NAMPTR,NAMLEN,ASTADR,ASTPRM)
!		Enqueue name translation. AST routine will be called when the
!		request completes. AST routine calling sequence is:
!		(ASTADR)(ASTPRM,<RC>,<RDLEN>,<RDATA>,<NAMLEN>,<NAMPTR>)
!	    NML$CANCEL(ASTPRM,ASTFLG,STATUS)
!		Delete an entry from the queue. Returns count of requests
!		found.
VOID !	    NML$STEP(COADDR) (void)
!		Step through the queue, calling the user coroutine for each
!		queue entry.
VOID !	    NML$PURGE(STATUS) (void)
!		Purge the name lookup queue and shutdown the name resolver.
!
// Author:
!
!	Vince Fuller, CMU-CSD, February, 1987
!	Copyright (c) 1987, Vince Fuller and Carnegie-Mellon University
!
// Edit history:
!
// 1.2b	18-Jul-1991	Henry W. Miller		USBR
!	Use LIB$GET_VM_PAGE and LIB$FREE_VM_PAGE rather then LIB$GET_VM
!	and LIB$FREE_VM.
!
// 1.2a	17-Jul-1991	Henry W. Miller		USBR
!	Fix comments from "server" to "resolver".
!	Updated IDENT.
!	Comment out LOG$MSG; now defined in TCPMACROS.
!	Corrected some spelling errors.
!
// 1.2  15-Jun-89, Edit by BRM (Bruce R. Miller)	CMU Network Development
!	Added NML$GETRR routine.
!
!	31-OCT-1988	Dale Moore	CMU-CS/RI
!	Store the name string in dynamic descriptors.  They
!	are easier to work with than descriptors created with
!	LIB$GET_VM.
!
// 1.1  10-Sep-87, Edit by VAF
!	Copy but don't count the null for name lookup requests.
!
// 1.0  24-Feb-87, Edit by VAF
!	Original version.
!--

MODULE NMLOOK(IDENT="1.2b",LANGUAGE(BLISS32),
	      ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			      NONEXTERNAL=LONG_RELATIVE),
	      LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	      OPTIMIZE,OPTLEVEL=3,ZIP)
{

#include "SYS$LIBRARY:STARLET";
#include "CMUIP_SRC:[CENTRAL]NETXPORT";
#include "CMUIP_SRC:[CENTRAL]NETCOMMON";
#include "CMUIP_SRC:[CENTRAL]NETVMS";
#include "CMUIP_SRC:[CENTRAL]NETERROR";
#include "CMUIP_SRC:[CENTRAL]NAMSYM";
#include "TCPMACROS";

!LITERAL
!!!HACK!!// Why is this temporary
//    LOG$MSG = %X"800";		// Temporary

extern
    LIB$GET_VM : BLISS ADDRESSING_MODE(GENERAL),
    LIB$GET_VM_PAGE : BLISS ADDRESSING_MODE(GENERAL),
    LIB$FREE_VM : BLISS ADDRESSING_MODE(GENERAL),
    LIB$FREE_VM_PAGE : BLISS ADDRESSING_MODE(GENERAL),
 VOID    QL_FAO,
 VOID    LOG_FAO,
 VOID    OPR_FAO,
    TIME_STAMP,
    USER$Clock_Base;

extern signed long
    MYUIC,
    LOG_STATE,
    AST_IN_PROGRESS,
    INTDF;


//SBTTL "Define name lookup queue entry"

// The name lookup queue contains an entry for each pending request. The request
// consists of a header describing it, followed by the mailbox message for it.

$FIELD NQENTRY_FIELDS (void)
    SET
    NQE$NEXT	= [$Address],	// Pointer to next item on queue
    NQE$PREV	= [$Address],	// Pointer to previous item on queue
    NQE$ID	= [$UShort],	// Request ID
    NQE$TYPE	= [$UShort],	// Request type
    NQE$LENGTH	= [$UShort],	// Request buffer length
    NQE$FLAGS	= [$Ushort],	// Request flags
    $OVERLAY(NQE$FLAGS)
      NQE$F_XMIT= [$Bit],	// Request has been successfully transmitted
    $CONTINUE
    NQE$TIME	= [$Long],	// Time request was enqueued
    NQE$ASTADR	= [$Address],	// Address of routine to call when done
    NQE$ASTPRM	= [$Address],	// Parameter to AST routine
    NQE$IOSB	= [$Bytes(8)],	// IOSB for mailbox send
    NQE$DATA	= [$Bytes(0)]	// Mailbox message buffer
    TES;
LITERAL
    NQENTRY_SIZE = $FIELD_SET_SIZE,
    NQENTRY_BLEN = NQENTRY_SIZE*4;
MACRO
    NQENTRY(SIZ) = 
	%IF %NULL(SIZ) %THEN
	    BLOCK->NQENTRY_SIZE
	%else
	    BLOCK[NQENTRY_SIZE + SIZ]
	%FI
	FIELD(NQENTRY_FIELDS) %;


// Definition of a mail message buffer

$FIELD MAIL$BUF_FIELDS (void)
    SET
    MB$IOSB	= [$BYTES(8)],	// IOSB for receive
    MB$DATA	= [$BYTES(0)]	// Data area
    TES;
LITERAL
    MAIL$BUF_SIZE = $FIELD_SET_SIZE;
MACRO
    MAIL$BUF = BLOCK->MAIL$BUF_SIZE FIELD(MAIL$BUF_FIELDS) %,
    MAX_MAIL$BUF = BLOCK[MAIL$BUF_SIZE + (MSGMAX/4)] FIELD(MAIL$BUF_FIELDS) %;

//SBTTL "Constants and OWN data"

BIND
    SRVPRCNAM = %ASCID"NAMRES",	// Name of the resolver process
    SYSTABNAM = %ASCID"LNM$SYSTEM_TABLE", // Logical name of system table
    SRVMBXNAM = %ASCID NAMRES_MBX, // System-wide logical name of resolver mailbox
    ACPMBXNAM = %ASCID IPACP_MBX, // System-wide logical name of ACP mailbox
    MYMBXNAM = %ASCID"IPACP_MBX", // Temporary name of ACP mailbox
    ACPMBXPRO = %X"FF00";	// Protection: (W:<no>,G:<no>,O:RWLP,S:RWLP)

LITERAL
    MBAMAX = MBNMAX*2,		// Max mailbox string length
    MSGCNT = 10,		// Number of receive messages to buffer
    NQE_QLIMIT = 20,		// Max length of request queue
    SRV$DOWN = -1,		// Resolver is unavailable
    SRV$INIT = 0,		// Resolver is initializing
    SRV$UP = 1;			// Resolver is up

static signed long
    SRVSTATE,			// Resolver state
    SRVMBXCHN,			// I/O channel on nameresolver mailbox
    SRVPID,			// PID of the nameresolver
    ACPMBXCHN,			// I/O channel on ACP mailbox
    RCVBUF : MAX_MAIL$BUF,	// Receive buffer
    MYMBA_BUF : BLOCK[CH$ALLOCATION(MBAMAX)],
    MYMBA_LEN,
    MYMBA_ATTR,
    NQE_QUEUE : QUEUE_HEADER	// Request queue
	PRESET([QHEAD] = NQE_QUEUE,
	       [QTAIL] = NQE_QUEUE),
    NQE_COUNT,			// Count of blocks currently allocated
    CURQID,			// Current request ID
    SRVIMGNAME : DESC$STR PRESET (	// Name resolver process name
			[DSC$W_LENGTH]	= 0,
			[DSC$B_DTYPE]	= DSC$K_DTYPE_T,
			[DSC$B_CLASS]	= DSC$K_CLASS_D,
			[DSC$A_POINTER]	= 0),
    SRVPRIOR,			// Name resolver priority
    SRVSTATUS,			// Name resolver status flags
    SRVPRIVS : VECTOR[2],	// Name resolver privileges
    struct VECTOR * SRVQUOTAS;	// Name resolver quotas

//SBTTL "NML$CONFIG - Configure name resolver process"

VOID NML$CONFIG(IMNAME_A,PRIOR,STATUS,PRIVS, QUOTAS) (void)
!
// Copy the configuration information about the name resolver into this module.
// This information is used by NML$INIT when creating the name resolver process.
!
    {
    BIND
	IMNAME	= IMNAME_A	: $BBLOCK;
    EXTERNAL ROUTINE
	STR$COPY_DX	: BLISS ADDRESSING_MODE (GENERAL);
    MAP
	PRIVS	: REF VECTOR[2],
	QUOTAS	: REF $BBLOCK;
    signed long
	RC,
	STRPTR,
	QUOPTR;

// Setup the descriptor of for the image name and allocate the string

    RC = STR$COPY_DX (SRVIMGNAME, IMNAME);
    if (NOT RC)
	FATAL$FAO("NML$CONFIG - failed to allocate string, RC = !XL",RC);

// Allocate the block for the process quota list

    RC = LIB$GET_VM(%REF(Quotas->DSC$W_LENGTH),QUOPTR);
    if (NOT RC)
	FATAL$FAO("NML$CONFIG - failed to allocate quolst, RC = !XL",RC);
    CH$MOVE(QUOTAS [DSC$W_LENGTH],QUOTAS [DSC$A_POINTER],QUOPTR);
    SRVQUOTAS = QUOPTR;

// Copy the remaining values into their appropriate places

    SRVPRIOR = PRIOR;		// Resolver priority
    SRVSTATUS = STATUS;	// Resolver process status flags
    SRVPRIVS[0] = PRIVS[0];	// Resolver privileges
    SRVPRIVS[1] = PRIVS[1];
    }

//SBTTL "NML$INIT - Initialize module state"

FORWARD ROUTINE
 VOID    MBX_RECV_AST,
    CHECK_SERVER,
 VOID    S}_CONTROL;

NML$INIT : NOVALUE (void)
!
// Create a mailbox for this process and start the name resolver.
// The name resolver will let us know when it is ready to start accepting
// requests.
!
    {
    signed long
	RC,
	ITMLIST : $ITMLST_DECL(ITEMS=3);

// Initialize state of service to unavailable

    SRVSTATE = SRV$DOWN;
    SRVMBXCHN = 0;
    SRVPID = -1;

// Initialize the request queue and initial request number

    CURQID = USER$Clock_Base();
    NQE_QUEUE->QHEAD = NQE_QUEUE->QTAIL = NQE_QUEUE;
    NQE_COUNT = 0;

// Create a mailbox for us

    RC = $CREMBX(CHAN	= ACPMBXCHN,
		 MAXMSG	= MSGMAX,
		 BUFQUO	= MSGMAX*MSGCNT,
		 PROMSK	= ACPMBXPRO,
		 LOGNAM	= MYMBXNAM);
    if (NOT RC)
	{
	ERROR$FAO("Failed to create ACP mailbox, RC = !XL",RC);
	RETURN;
	};

// Get the real name of the mailbox and create a system-wide logical to point
// to it. We want the name to be accessable to all, but for the mailbox to go
// away if this process aborts.

    $ITMLST_INIT(ITMLST=ITMLIST,
	(ITMCOD=LNM$_ATTRIBUTES,BUFADR=MYMBA_ATTR),
	(ITMCOD=LNM$_STRING,BUFADR=MYMBA_BUF,BUFSIZ=MBAMAX,RETLEN=MYMBA_LEN),
	(ITMCOD=0,BUFSIZ=0,BUFADR=0)
    );
    RC = $TRNLNM(ATTR = %REF(LNM$M_CASE_BLIND),
		 TABNAM = %ASCID"LNM$TEMPORARY_MAILBOX",
		 LOGNAM = MYMBXNAM,
		 ITMLST = ITMLIST);
    if (NOT RC)
	{
	ERROR$FAO("$TRNLNM failed for ACP mailbox, RC = !XL",RC);
	RETURN;
	};
    if ((MYMBA_ATTR && LNM$M_EXISTS) == 0)
	{
	ERROR$FAO("$TRNLNM claims mailbox name nonexistant!!");
	RETURN;
	};

// Now, create the system-wide logical to point at it

    $ITMLST_INIT(ITMLST=ITMLIST,
	(ITMCOD=LNM$_STRING,BUFADR=MYMBA_BUF,BUFSIZ=.MYMBA_LEN),
	(ITMCOD=0,BUFSIZ=0,BUFADR=0)
    );
    RC = $CRELNM(TABNAM = SYSTABNAM,
		 LOGNAM = ACPMBXNAM,
		 ITMLST = ITMLIST);
    if (NOT RC)
	{
	ERROR$FAO("$CRELNM failed for ACP mailbox, RC = !XL",RC);
	RETURN;
	};

// Start an initial read on the mailbox

    RC = $QIO(	FUNC	= IO$_READVBLK,
		CHAN	= ACPMBXCHN,
		IOSB	= RCVBUF->MB$IOSB,
		P1	= RCVBUF->MB$DATA,
		P2	= MSGMAX,
		ASTADR	= MBX_RECV_AST,
		ASTPRM	= RCVBUF);
    if (NOT RC)
	{
	ERROR$FAO("Queued read failed for mailbox, RC = !XL",RC);
	RETURN;
	};

// Check to see if NAMRES mailbox exists yet. If not, we'll start the resolver.

    if (CHECK_SERVER())
	{
	S}_CONTROL(CNRQ$START,0); // Tell it that the network is here
	SRVSTATE = SRV$UP	// Resolver should be ready now
	}
    else
	{

// Make sure the initialization was done properly.

	if (SRVIMGNAME->DSC$W_LENGTH == 0)
	    {
	    ERROR$FAO("Failed to create NAMRES - configuration info missing");
	    SRVSTATE = SRV$DOWN;
	    RETURN;
	    };

// Start the name resolver process - it will notify us when it is ready.

	RC = $CREPRC(IMAGE	= SRVIMGNAME,
		     PRCNAM	= SRVPRCNAM,
		     UIC	= MYUIC,
		     PIDADR	= SRVPID,
		     BASPRI	= SRVPRIOR,
		     STSFLG	= SRVSTATUS,
		     PRVADR	= SRVPRIVS,
		     QUOTA	= SRVQUOTAS
		     );
	if (NOT RC)
	    {
	    ERROR$FAO("$CREPRC for NAMRES failed, RC = !XL",RC);
	    RETURN;
	    };
	SRVSTATE = SRV$INIT;	// Resolver is initializing
	};
    }

//SBTTL "NML$GETALST - Translate name to address list"

FORWARD ROUTINE
    NQE_ALLOC,
 VOID    NQE_DEALLOC,
 VOID    NQE_ENQUEUE,
    NQE_XMIT;

NML$GETALST(NAMPTR,NAMLEN,ASTADR,ASTPRM) : NOVALUE (void)
!
// Request a name to address translation. Build the request and transmit it
// to the name resolver.  Receive decode routine will finish the request when
// a reply has been received for it.
!
    {
    signed long
	RC,
	CPTR,
	MSLEN,
	struct NQENTRY * NQE(),
	struct MAIL$MSG * MSBUF,
	struct RQ$NMLOOK * RQBUF;

// Log the request

    XLOG$FAO(LOG$MSG,"!%T NML$GETALST: name is !AD!/",0,NAMLEN,NAMPTR);

// Allocate and initialize a request block for us

    RC = NQE_ALLOC(NQE);
    if (NOT RC)
	{
	(ASTADR)(ASTPRM,RC);
	RETURN;
	};

// Build the request in it.

    MSBUF = NQE->NQE$DATA;
    RQBUF = MSBUF->MSG$DATA;
    RQBUF->RQNM$TYPE = NLRQ$NMLOOK;
    RQBUF->RQNM$ID = (CURQID = CURQID + 1);

// Note: the -1 is because the count included the null. We copy the null but
// don't count it in the buffer for NAMRES.

    RQBUF->RQNM$NAMLEN = NAMLEN-1;
    CPTR = CH$MOVE(NAMLEN,NAMPTR,CH$PTR(RQBUF->RQNM$NAMSTR));
    MSLEN = CH$DIFF(CPTR,CH$PTR(MSBUF));

// Enqueue the request and transmit it.

    NQE->NQE$TYPE = NLRQ$NMLOOK;
    NQE->NQE$ASTADR = ASTADR;
    NQE->NQE$ASTPRM = ASTPRM;
    NQE_ENQUEUE(NQE,CURQID,MSLEN,MSBUF);
    }

//SBTTL "NML$GETNAME - Translate address to name"

NML$GETNAME(ADDR,ASTADR,ASTPRM) : NOVALUE (void)
!
// Request an address to name translation. Build the request and send it to
// the name resolver.  Receive decode routine will finish the request when the
// reply comes in.
!
    {
    signed long
	RC,
	MSLEN,
	struct NQENTRY * NQE(),
	struct MAIL$MSG * MSBUF,
	struct RQ$ADLOOK * RQBUF;

// Log the request

    if ($$LOGF(LOG$MSG))
	{
	signed long
	    INA : VECTOR[4,BYTE];
	INA = ADDR;
	QL$FAO("!%T NML$GETNAME: address is !UB.!UB.!UB.!UB!/",
	       0,INA[0],INA[1],INA[2],INA[3]);
	};

// Allocate and initialize a request block for us

    RC = NQE_ALLOC(NQE);
    if (NOT RC)
	{
	(ASTADR)(ASTPRM,RC);
	RETURN;
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

NML$GETRR(RRTYPE,NAMPTR,NAMLEN,ASTADR,ASTPRM) : NOVALUE (void)
!
// Request a name to RR translation. Build the request and transmit it
// to the name resolver.  Receive decode routine will finish the request when
// a reply has been received for it.
!
    {
    signed long
	RC,
	CPTR,
	MSLEN,
	struct NQENTRY * NQE(),
	struct MAIL$MSG * MSBUF,
	struct RQ$RRLOOK * RQBUF;

// Log the request

    XLOG$FAO(LOG$MSG,"!%T NML$GETRR: type is !XL, name is !AD!/",0,
		.RRTYPE, NAMLEN, NAMPTR);

// Allocate and initialize a request block for us

    RC = NQE_ALLOC(NQE);
    if (NOT RC)
	{
	(ASTADR)(ASTPRM,RC);
	RETURN;
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
    CPTR = CH$MOVE(NAMLEN,NAMPTR,CH$PTR(RQBUF->RQRR$NAMSTR));
    MSLEN = CH$DIFF(CPTR,CH$PTR(MSBUF));

// Enqueue the request and transmit it.

    NQE->NQE$TYPE = NLRQ$RRLOOK;
    NQE->NQE$ASTADR = ASTADR;
    NQE->NQE$ASTPRM = ASTPRM;
    NQE_ENQUEUE(NQE,CURQID,MSLEN,MSBUF);
    }

//SBTTL "NML$CANCEL - Cancel name lookup request"

NML$CANCEL(ASTPRM,ASTFLG,STATUS)
!
// Search the name lookup queue for the specified request and delete it,
// optionally calling the "done" routine with the specified status.
!
    {
    signed long
	struct NQENTRY * NQE(),
	struct NQENTRY * NXNQE(),
	RC;

// Search the queue for the request

    RC = 0;
    NQE = NQE_QUEUE->QHEAD;
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

NML$STEP(COADDR,COVALUE) : NOVALUE (void)
!
// Examine the name lookup queue, calling the user coroutine with the AST
// address and AST parameter of the entry. This makes it easy for the CANCEL
// handler in the ACP to find any requests belonging to a process.
!
    {
    signed long
	struct NQENTRY * NQE(),
	struct NQENTRY * NXNQE();

// Walk the queue

    NQE = NQE_QUEUE->QHEAD;
    while (NQE != NQE_QUEUE)
	{
	NXNQE = NQE->NQE$NEXT;
	(COADDR)(COVALUE,NQE->NQE$ASTADR,NQE->NQE$ASTPRM);
	NQE = NXNQE;
	};
    }

NML$PURGE(STATUS) : NOVALUE (void)
!
// Purge the name lookup queue and tell the name resolver to shutdown.
// Called just before the ACP exits.
!
    {
    signed long
	RC,
	struct NQENTRY * NQE(),
	struct NQENTRY * NXNQE();

// Tell the name resolver that the network is exiting.

    if (SRVMBXCHN != 0)
	S}_CONTROL(CNRQ$STOP,STATUS);

// Flush our mailbox logical name

    RC = $DELLNM(TABNAM = SYSTABNAM,
		 LOGNAM = ACPMBXNAM);
    if (NOT RC)
	ERROR$FAO("$DELLNM failed for !AS, RC = !XL",ACPMBXNAM,RC);

// Delete our mailbox

    RC = $DELMBX(CHAN	= ACPMBXCHN);
    if (NOT RC)
	ERROR$FAO("$DELMBX failed for ACP mailbox, RC = !XL",RC);

// Walk the queue, purging all requests

    NQE = NQE_QUEUE->QHEAD;
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

FORWARD ROUTINE
 VOID    NQE_DUMP;

NML$DUMP : NOVALUE (void)
!
// Walk the queue, dumping out each entry. For debugging purposes only.
!
    {
    signed long
	struct NQENTRY * NQE(),
	struct NQENTRY * PNQE(),
	NOW;

    NOW = TIME_STAMP();
    LOG$FAO("NQE count is !SL, TIME is !XL, QHEAD=!XL, QTAIL=!XL!/",
	    NQE_COUNT,NOW,NQE_QUEUE->QHEAD,NQE_QUEUE->QTAIL);
    NQE = NQE_QUEUE->QHEAD;
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


VOID NQE_DUMP(struct NQENTRY * NQE()) (void)
!
// Dump out a single queue entry.
!
    {
    LOG$FAO(%STRING("NQE at !XL, NEXT=!XL, PREV=!XL!/",
		    "  TYPE=!UL, LEN=!UL, FLAGS=!XW, TIME=!XL!/",
		    "  ASTADR=!XL, ASTPRM=!XL!/"),
	    NQE,NQE->NQE$NEXT,NQE->NQE$PREV,
	    NQE->NQE$TYPE,NQE->NQE$LENGTH,NQE->NQE$FLAGS,NQE->NQE$TIME,
	    NQE->NQE$ASTADR,NQE->NQE$ASTPRM);
    }

S}_CONTROL(CCODE,CVALUE) : NOVALUE (void)
!
// Build and send a control message to the name resolver.
!
    {
    LITERAL
	CONTROL_MSGSIZE = RQ$CONTROL_BLEN + MAIL$MSG_BLEN;
    OWN
	MSBUF : MAX_MAIL$MSG;
    signed long
	RC,
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

    RC = $QIO(	FUNC	= IO$_WRITEVBLK || IO$M_NOW,
		CHAN	= SRVMBXCHN,
		P1	= MSBUF,
		P2	= CONTROL_MSGSIZE);
    if (NOT RC)
	ERROR$FAO("Failed to send NAMRES control message, RC = !XL",RC);
    }

//SBTTL "Request queue management routines"

LITERAL
    NQE_MAXSIZE = NQENTRY_BLEN + MSGMAX;

NQE_ALLOC(NQE)
!
// Allocate a full-size queue block from dynamic memory. Sets the NQE parameter
// to the address of the block on success. On failure, returns network error
// code indicating the failure reason.
!
    {
    signed long
	RC;

// Verify that we should do this.

    if (SRVSTATE == SRV$DOWN)
	return NET$_NONS;
    if (NQE_COUNT GEQ NQE_QLIMIT)
	return NET$_NSQFULL;

// Allocate the block

    NQE_COUNT = NQE_COUNT + 1;
//    RC = LIB$GET_VM(%REF(NQE_MAXSIZE),NQE);
    RC = LIB$GET_VM_PAGE(%REF((NQE_MAXSIZE / 512) + 1),NQE);
    XQL$FAO(LOG$MSG,"!%T NQE_ALLOC, NQE=!XL, RC=!XL!/",0,..NQE,RC);
    return RC;
    }



NQE_DEALLOC(NQE) : NOVALUE (void)
!
// Deallocate a queue block, decrementing count.
!
    {
    signed long
	RC;
    MAP
	struct NQENTRY * NQE();

// Deallocate the memory, using LIB$FREE_VM

//    RC = LIB$FREE_VM(%REF(NQE_MAXSIZE),NQE);
    RC = LIB$FREE_VM_PAGE(%REF((NQE_MAXSIZE / 512) + 1),NQE);
    if (NOT RC)
	FATAL$FAO("NQE_DEALLOC - LIB$FREE_VM failure, RC = !XL",RC);
    XQL$FAO(LOG$MSG,"!%T NQE_DEALLOC, NQE = !XL!/",0,NQE);

// Decrement count of allocated blocks and return success

    NQE_COUNT = NQE_COUNT - 1;
    return SS$_NORMAL;
    }

NQE_ENQUEUE(NQE,QRYID,MSLEN,MSBUF) : NOVALUE (void)
!
// Insert a new request onto the request queue and transmit it.
// Fills in the message header, puts it on NQE_QUEUE, and calls NQE_XMIT to
// transmit the query.
!
    {
    MAP
	struct NQENTRY * NQE(),
	struct MAIL$MSG * MSBUF;

// First, fill in our return mailbox in the message header.

    MSBUF->MSG$MBXNLN = MYMBA_LEN;
    CH$MOVE(MYMBA_LEN,CH$PTR(MYMBA_BUF),CH$PTR(MSBUF->MSG$MBXNAM));

// Next, finish setting up the request header

    NQE->NQE$TIME = TIME_STAMP();
    NQE->NQE$LENGTH = MSLEN;
    NQE->NQE$ID = QRYID;
    NQE->NQE$F_XMIT = 0;

// Insert it into the queue

    INSQUE(NQE,NQE_QUEUE->QTAIL);

// Do logging, if necessary

    XQL$FAO(LOG$MSG,"!%T NQE_ENQUEUE, NQE=!XL, ID=!XL, LEN=!SL, TIME=!UL!/",
	    0,NQE,NQE->NQE$ID,MSLEN,NQE->NQE$TIME);

// Transmit the request if the resolver is ready to take it.

    if (SRVSTATE GEQ SRV$UP)
	NQE_XMIT(NQE);
    }

NQE_DELETE(NQE,ASTFLG,STATUS) : NOVALUE (void)
!
// Delete an entry from the queue, optionally calling the AST routine with the
// specified status code.
!
    {
    MAP
	struct NQENTRY * NQE();

    XQL$FAO(LOG$MSG,"!%T NQE_DELETE of NQE !XL, ID !XL!/",0,NQE,NQE->NQE$ID);

// First, unlink the request from the queue

    REMQUE(NQE,NQE);

// Next, call the AST routine if that is requested

    if (ASTFLG != 0)
	(NQE->NQE$ASTADR)(NQE->NQE$ASTPRM,STATUS);

// Finally, deallocate the queue entry

    NQE_DEALLOC(NQE);
    }

FORWARD ROUTINE
 VOID    NQE_XMIT_DONE;

NQE_XMIT(NQE)
!
// Transmit a request to the name resolver.  Returns SS$_NORMAL on success, or
// $QIO failure code on failure.
!
    {
    MAP
	struct NQENTRY * NQE();
    signed long
	RC;

    XQL$FAO(LOG$MSG,"!%T NQE_XMIT of NQE !XL, ID !XL!/",0,NQE,NQE->NQE$ID);
    RC = $QIO(	CHAN	= SRVMBXCHN,
		FUNC	= IO$_WRITEVBLK,
		IOSB	= NQE->NQE$IOSB,
		P1	= NQE->NQE$DATA,
		P2	= NQE->NQE$LENGTH,
		ASTADR	= NQE_XMIT_DONE,
		ASTPRM	= NQE);

// Check the state of the send. We should probably do something useful here if
// it fails (like shutdown the name service and report the error to the opr).

    if (NOT RC)
	{
	XQL$FAO(LOG$MSG,"!%T NQE_XMIT failed, RC=!XL, NQE=!XL, ID=!XL!/",
		0,RC,NQE,NQE->NQE$ID);
	return RC;
	};
    return SS$_NORMAL;
    }

NQE_XMIT_DONE(NQE) : NOVALUE (void)
!
// AST routine for mailbox send done.
!
    {
    MAP
	struct NQENTRY * NQE();
    signed long
	struct MBX$IOSB * IOSB,
	RC;

// Check the status & set transmit done flag. We should probably do something
// useful if an error occurs here.

    IOSB = NQE->NQE$IOSB;
    RC = IOSB->MI$STATUS;
    XQL$FAO(LOG$MSG,"!%T NQE_XMIT_DONE, NQE=!XL, RC=!XL!/",0,NQE,RC);
    NQE->NQE$F_XMIT = 1;
    }

XMIT_REQUESTS : NOVALUE (void)
!
// Transmit all of the pending requests when the name resolver has come online.
!
    {
    signed long
	struct NQENTRY * NQE(),
	struct NQENTRY * NXNQE();

// Loop for the entire queue transmitting any requests which have not been
// tranmitted successfully yet.

    NQE = NQE_QUEUE->QHEAD;
    while (NQE != NQE_QUEUE)
	{
	NXNQE = NQE->NQE$NEXT;
	if (NOT (NQE->NQE$F_XMIT))
	    NQE_XMIT(NQE);
	NQE = NXNQE;
	};
    }

//SBTTL "Mailbox message receiving routines"

FORWARD ROUTINE
 VOID    DECODE_REPLY;

MBX_RECV_AST(MBUF) : NOVALUE (void)
!
// Come here when mailbox read completes. Read and decode the message.
// Note that currently, we use a static buffer for reading the incoming message,
// !!!HACK!!!
// though this code can easily handle dynamic buffers.
!
    {
    MAP
	struct MAIL$BUF * MBUF;
    signed long
	RC,
	RCVLEN,
	RCVPID,
	struct MAIL$MSG * RCVMSG,
	struct MBX$IOSB * RCVIOSB;

// Get the status of the message

    RCVMSG = MBUF->MB$DATA;
    RCVIOSB = MBUF->MB$IOSB;
    RC = RCVIOSB->MI$STATUS;

// Check error

    if (NOT RC)
	{
	FATAL$FAO("Mailbox read failure, RC = !XL",RC);
	RETURN;
	};

// Success. Retrieve message length and PID; call decoding routine.

    RCVLEN = RCVIOSB->MI$COUNT-MAIL$MSG_BLEN;
    RCVPID = RCVIOSB->MI$PID;
    XQL$FAO(LOG$MSG,"!%T NS msg received, LEN=!SL, PID=!XL!/",
	    0,RCVIOSB->MI$COUNT,RCVPID);
    if (RCVLEN > 0)
	DECODE_REPLY(RCVPID,RCVLEN,RCVMSG->MSG$DATA);

// Queue up another read

    RC = $QIO(	FUNC	= IO$_READVBLK,
		CHAN	= ACPMBXCHN,
		IOSB	= RCVIOSB,
		P1	= RCVMSG,
		P2	= MSGMAX,
		ASTADR	= MBX_RECV_AST,
		ASTPRM	= MBUF);
    if (NOT RC)
	{
	ERROR$FAO("Queued read failed for mailbox, RC = !XL",RC);
	RETURN;
	};
    }

FORWARD ROUTINE
 VOID    CONTROL_MSG;

DECODE_REPLY(PID,RLEN,RBUF) : NOVALUE (void)
    {
    MAP
	struct RPLY$DEFAULT * RBUF;
    signed long
	RTYPE,
	RID,
	struct NQENTRY * NQE(),
	struct NQENTRY * NXNQE(),
	FOUND,
	NAMLEN,
	NAMPTR,
	ASTADR,
	ASTPRM;

// Handle control message specially

    RTYPE = RBUF->RPLY$TYPE;
    if (RTYPE == NLRP$CONTROL)
	{
	CONTROL_MSG(PID,RLEN,RBUF);
	RETURN;
	};
    if ((RTYPE LSS NLRP$MIN) || (RTYPE > NLRP$MAX))
	{
	XQL$FAO(LOG$MSG,"!%T Unknown NS msg type !SL received, PID=!XL!/",
		0,RTYPE,PID);
	RETURN;
	};

// For all others, find the request that this reply is for and handle it.

    FOUND = 0;
    RID = RBUF->RPLY$ID;
    NQE = NQE_QUEUE->QHEAD;
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
	    CASE RTYPE FROM NLRP$MIN TO NLRP$MAX OF
	    SET
	    [NLRP$ERROR]:	// Error reply - abort request
		{
		MAP
		    struct RPLY$ERROR * RBUF;

		XQL$FAO(LOG$MSG,"!%T NS error reply, ID=!XL, ERR=!XL!/",
			0,RBUF->RPER$ID,RBUF->RPER$ECODE);
		(ASTADR)(ASTPRM,RBUF->RPER$ECODE);
		};

	    [NLRP$CONTROL]:	// Not possible at this level
		0;

	    [NLRP$NMLOOK]:	// Name lookup reply
		{
		MAP
		    struct RPLY$NMLOOK * RBUF;
		signed long
		    ADRCNT,
		    ADRLST;

		ADRCNT = RBUF->RPNM$ADRCNT;
		ADRLST = RBUF->RPNM$ADRLST;
		NAMLEN = RBUF->RPNM$NAMLEN;
		NAMPTR = RBUF->RPNM$NAMSTR;
		XQL$FAO(LOG$MSG,
			"!%T NS NMLOOK reply, ID=!XL, #ADDR=!SL, NAME=!AD!/",
			0,RBUF->RPNM$ID,ADRCNT,NAMLEN,NAMPTR);
		(ASTADR)(ASTPRM,SS$_NORMAL,ADRCNT,ADRLST,NAMLEN,NAMPTR);
		};

	    [NLRP$ADLOOK]:	// Address lookup reply
		{
		MAP
		    struct RPLY$ADLOOK * RBUF;

		NAMLEN = RBUF->RPAD$NAMLEN;
		NAMPTR = RBUF->RPAD$NAMSTR;
		XQL$FAO(LOG$MSG,"!%T NS ADLOOK reply, ID=!XL, name=!AD!/",
			0,RBUF->RPAD$ID,NAMLEN,NAMPTR);
		(ASTADR)(ASTPRM,SS$_NORMAL,NAMLEN,NAMPTR);
		};
	    [NLRP$RRLOOK]:	// RR lookup reply
		{
		MAP
		    struct RPLY$RRLOOK * RBUF;
		signed long
		    RDLEN,
		    RDATA;

		RDLEN = RBUF->RPRR$RDLEN;
		RDATA = RBUF->RPRR$RDATA;
!		NAMLEN = RBUF->RPRR$NAMLEN;
!		NAMPTR = RBUF->RPRR$DATA + RDLEN;
		XQL$FAO(LOG$MSG,
			"!%T NS RRLOOK reply, ID=!XL, SIZE=!SL, RDATA=!AD /",
			0,RBUF->RPRR$ID,RDLEN,
			.RDLEN,RDATA);
		(ASTADR)(ASTPRM,SS$_NORMAL,RDLEN,RDATA,NAMLEN,NAMPTR);
		};
	    [INRANGE]:	// Not possible at this level
		0
	    TES;

// Deallocate the queue entry

	    NQE_DEALLOC(NQE);
	    };

// Check next queue entry...

	NQE = NXNQE;
	};

// If the request wasn't found in the queue, report the error

    if (FOUND LEQ 0)
	XQL$FAO(LOG$MSG,"!%T DECODE_REPLY failed to find RQ !XL, TYPE !SL!/",
		0,RID,RTYPE);
    }

CONTROL_MSG(PID,RLEN,RBUF) : NOVALUE (void)
!
// Handle a resolver control message. We are interested in being told when
// either the name resolver goes away or when it has become ready. Note that
// control messages are generally one-way, so we don't care about control
// replies.
!
    {
    MAP
	struct RQ$CONTROL * RBUF;
    signed long
	CTYPE;

// Dispatch the control types we handle.

    CTYPE = RBUF->RQCN$CCODE;
    SELECTONE CTYPE OF
    SET
    [CNRQ$START]:		// Resolver has finished starting
	{
	XQL$FAO(LOG$MSG,"!%T NS Control: NS is up, PID=!XL!/",0,PID);
	if (CHECK_SERVER())
	    {
	    SRVSTATE = SRV$UP;	// Indicate that everything is ready
	    XMIT_REQUESTS();	// And send all queued requests to the resolver
	    };
	};

    [CNRQ$STOP]:		// Resolver is shutting down
	{
	XQL$FAO(LOG$MSG,"!%T NS Control: NS shutting down, PID=!XL!/",0,PID);
	$DASSGN(CHAN = SRVMBXCHN);
	SRVSTATE = SRV$DOWN;	// Service is unavailable
	SRVMBXCHN = 0;		// No channel defined
	SRVPID = 0;		// And no PID defined
	};

    [OTHERWISE]:
	{
	XQL$FAO(LOG$MSG,"!%T Unknown NS control msg, CCODE=!SL, PID=!XL!/",
		0,CTYPE,PID);
	};
    TES;
    }

CHECK_SERVER (void)
!
// Check for the existance of the NAMRES process and the name resolver system
// logical name. If both are found, set SRVMBXCHN and SRVPID accordingly and
// return TRUE. Else, return FALSE.
!
    {
    signed long
	RC,
	DEVCHN,
	PID,
	ITMLIST : $ITMLST_DECL(ITEMS=3);

// First, try to assign a channel on the name resolver mailbox.

    RC = $ASSIGN(DEVNAM = SRVMBXNAM,
		 CHAN	= DEVCHN);
    if (NOT RC)
	return FALSE;

// Now, look for a process with the name "NAMRES"
// N.B. NAMRES must run with the same group as IPACP for this to work. This is
// desirable in any case, since we don't want imposters claiming to be the
// name resolver process.

    $ITMLST_INIT(ITMLST=ITMLIST,
	(ITMCOD=JPI$_PID,BUFADR=PID),
	(ITMCOD=0,BUFSIZ=0,BUFADR=0)
    );
    RC = $GETJPIW(PRCNAM = SRVPRCNAM,
		  ITMLST = ITMLIST);
    if (NOT RC)
	{
	$DASSGN(CHAN	= DEVCHN);
	return FALSE;
	};

// Got what we want. Set up the info & return.

    XQL$FAO(LOG$MSG,"!%T NS is UP, PID is !XL!/",0,PID);
    SRVMBXCHN = DEVCHN;
    SRVPID = PID;
    return TRUE;
    }

}
ELUDOM
