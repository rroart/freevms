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
//TITLE "TCP_MECH - TCP data structure routines"
//SBTTL "TCP mechanism overview"
/*

Module:

	TCP_USER

Facility:

	Process user TCP I/O requests.

Abstract:

	This module maintains and manipulates all of the data structures
	used to implement the TCP sub-system.  These data structures are
	the conection-list (ordered by local port number) and the Valid
	TCB list.

Author:

	(originally extracted from USER.BLI)
	This version by Vince Fuller, CMU-CSD, Summer/Fall, 1986
	Copyright (c) 1986,1987, Vince Fuller and Carnegie-Mellon University


Modification History:

1.0c	13-Mar-1991	Henry W. Miller		USBR
	Fix port comparison logic.

1.0b	11-Mar-1991	Henry W. Miller		USBR
	Fix port comparison logic.

1.0a	25-Jan-1991	Henry W. Miller		USBR
	Make WINDOW_DEFAULT a configurable variable.

 )%


//SBTTL "Module definition"

MODULE TCP_MECH(IDENT="1.0c",LANGUAGE(BLISS32),
		ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
				NONEXTERNAL=LONG_RELATIVE),
		LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
		OPTIMIZE,OPTLEVEL=3,ZIP)

{
#include "SYS$LIBRARY:STARLET";	// VMS system definitions
#include "CMUIP_SRC:[CENTRAL]NETXPORT";	// XPORT data structure definitions
#include "CMUIP_SRC:[CENTRAL]NETERROR";	// Network error message definitions
#include "CMUIP_SRC:[CENTRAL]NETVMS";	// Special VMS definitions
#include "CMUIP_SRC:[CENTRAL]NETCOMMON";// Network common defs
#include "CMUIP_SRC:[CENTRAL]NETTCPIP";	// TCP/IP protocols
#include "STRUCTURE";		// TCP system data structure definitions
#include "TCPMACROS";		// System macro definitions
#include "TCP";			// TCP related definitions
#include "SNMP";			// Simple Network Management Protocol

extern
    MM$Get_Mem, MM$Free_Mem;


//SBTTL "System variable allocation and declaration"

extern ^GLOBAL
    WINDOW_DEFAULT ;

signed long
    Start_Time: VECTOR->2,	// Quadword time IPACP started.
    TEK$sys_uptime : VECTOR->2,	// Quadword delta time since IPACP started.
    TCP_MIB : TCP_MIB_struct;	// TCP management Information Block



//SBTTL "Connection Table Allocation."
/*
Conect is the data structure used to associate TCB's with a specific local
port.  Each block of the BlockVector "CONECT" contains a local-Port field &
a TCB list header.  All TCBs which have this local port # are contained in
the list.  The reason behind this data structure is to facilate connection
"OPEN" processing by quick access to all TCBs for a given local connection.
All connections must be unique therefore one must verify this by scanning
all TCBs with the given local port.
TCB's are accessed thru the CONECT method or via the LOCAL-CONNECTION-ID which
is actually the TCB's address.
*/

static ^GLOBAL
    ConectSize,
    struct Connection_Table_Structure * ConectPtr;

signed long
    MAX_LOCAL_PORTS  = 30,	// Settable in CONFIG

// Valid TCB table, contains pointers to valid TCB's. Used to verify 
// user supplied Local_connection_Id.  As one sees: A local_Connection_ID is
// actually the address of a TCB.

    struct VECTOR * VTCB_ptr->0,
    Max_TCB  = 0,
    Max_CONN  = 60,	// Settable in CONFIG
    VTCB_size  = 0,
    TCB_Count  = 0;	// # of valid TCB's in VTCB table.


//SBTTL "Initialize TCP"
/*

Function:

	Initialize TCP.  We are at user access mode in case of errors
	in processing we don't clobber the entire system.

Inputs:

	None.

Outputs:

	None.

Side Effects:

	Allocate and initialize the TCP tables.
*/

!!!HACK!!// we're breaking the ADT... (gasp)

TCP$Init: NOVALUE (void)
    {
    signed long
	cidx;

    // Allocate the connection list
    ConectSize = MAX_LOCAL_PORTS;
    MM$Get_Mem( ConectPtr , ConectSize * CN$Blk_Size * 4 );
    INCR cidx FROM 0 TO (ConectSize-1) DO
	{				// Initialize connection table
	ConectPtr[cidx,CN$TCB_List] = ConectPtr[cidx,CN$TCB_List];
	ConectPtr[cidx,CN$TCB_Tail] = ConectPtr[cidx,CN$TCB_List];
	ConectPtr[cidx,CN$Local_Port] = -1;
	};

    // Allocate the valid TCB table
    VTCB_Size = MAX_CONN;
    MM$Get_Mem ( VTCB_Ptr , (VTCB_Size+1) * 4 );
    CH$FILL ( 0 , (VTCB_Size+1) * 4 , VTCB_Ptr );

    TCP_MIB->MIB$tcpRtoAlgorithm= 0;
    TCP_MIB->MIB$tcpRtoMin	= 0;
    TCP_MIB->MIB$tcpRtoMax	= 0;
    TCP_MIB->MIB$tcpMaxConn	= 0;
    TCP_MIB->MIB$tcpActiveOpens	= 0;
    TCP_MIB->MIB$tcpPassiveOpens= 0;
    TCP_MIB->MIB$tcpAttemptFails= 0;
    TCP_MIB->MIB$tcpEstabResets	= 0;
    TCP_MIB->MIB$tcpCurrEstab	= 0;
    TCP_MIB->MIB$tcpInSegs	= 0;
    TCP_MIB->MIB$tcpOutSegs	= 0;
    TCP_MIB->MIB$tcpRetransSegs	= 0;

    };



!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// VTCB ADT routines
!---------------------------------------------------------------------



//SBTTL "VTCB_Insert - Insert a TCB into the VTCB"
/*

Function:

	Given foreign address and port pair, find the TCB.

Inputs:

	lclport = Local port number
	frnaddr = Foreign address
	frnport = Foreign port

Outputs:

	False(0) = unknown connection
	Otherwise The address of the destination connection's TCB.

Side Effects:

	None.
*/


VTCB_Insert ( struct tcb_structure * tcb )
    {
    EXTERNAL ROUTINE
	MovByt;
    signed long
	Indx,
	Old;

    NOINT;

    Indx = 0;
    INCR J FROM 1 TO VTCB_Size DO
	if (VTCB_ptr[J] == 0)
	    EXITLOOP Indx = J;
    if (Indx == 0)
	{
	OPR$FAO("MMgr: Growing Valid TCB table to !SW entries...!/",
		 VTCB_Size*2);
	// This should be called at AST level to assure that we aren't
	// pulling the rug out from anyone

	Indx = VTCB_Size;
	VTCB_Size = VTCB_Size * 2;

	Old = VTCB_Ptr;
	MM$Get_Mem( VTCB_Ptr , (VTCB_Size+1) * 4 );
	MovByt ( (Indx+1) * 4 , Old , VTCB_Ptr );
	MM$Free_Mem( Old , (Indx+1) * 4 );

        Indx = Indx + 1;
	};

    // Maintain pointer to last TCB in table
    if (Indx > Max_TCB) Max_TCB = Indx;
!OPR$FAO("!%T Max_TCB = !UW",0,Max_TCB);

    // Link the new TCB into the Valid TCB table.
    VTCB_ptr[Indx] = TCB; // set TCB's address
    TCB_Count = TCB_Count + 1; // Keep track of active TCB's.
    TCB->VTCB_INDEX = Indx; // Remember index into Valid TCB Table

    OKINT;	// Carry on...

    SS$_NORMAL
    };



VTCB_Remove ( struct tcb_structure * tcb )
    {
    signed long
	Idx;

    // Get index into Valid TCB table
    IDX = TCB->VTCB_INDEX;
    if (VTCB_ptr[IDX] EQLA TCB)
	{
	LABEL
	    X;

        VTCB_ptr[IDX] = 0;	// Clean out entry
	TCB_Count = TCB_Count-1; // Account for this TCB going away.
	if (Max_TCB <= Idx)
	   X : {
	   DECR J FROM Idx TO 1 DO
		if (VTCB_ptr[J] != 0) LEAVE X WITH Max_TCB = J;
	   // No valid TCB's left?  Do a sanity check.
	   DECR J FROM VTCB_Size TO 1 DO
		if (VTCB_ptr[J] != 0) LEAVE X WITH Max_TCB = J;
	   Max_TCB = 1;
	   };  // end of block X
	}
    else RETURN 0;

    TCB
    };



//SBTTL "VTCB_Scan"
/*

Function:

	Given foreign address and port pair, find the TCB.

Inputs:

	lclport = Local port number
	frnaddr = Foreign address
	frnport = Foreign port

Outputs:

	False(0) = unknown connection
	Otherwise The address of the destination connection's TCB.

Side Effects:

	None.
*/


VTCB_Scan ( ASTRTN , ASTP1 , ASTP2 )
    {
    signed long
	sum  = 0,
	count;

    count = TCB_count;

    NOINT;

    INCR j FROM 1 TO VTCB_Size DO
	if (VTCB_ptr[J] NEQA 0)
	    {
	    if (j > Max_TCB)
		OPR$FAO("%T TCB (!UL) above Max_TCB (!UL)",j,Max_TCB);

	    sum = sum + (ASTRTN)(VTCB_ptr[J],J,ASTP1,ASTP2);

	    if ((count = count-1) <= 0) !only process what we have.
		{
		OKINT;
		RETURN Sum	// all done.
		}
	    };
    OKINT;

    sum
    };



TCP$Connection_List(RB) : NOVALUE (void)
!
// Dump the list of TCP connections.
!
    {
    MAP
	struct D$LC_ID_Return_Blk * RB;

    RB->0 = 0;
    INCR J FROM 1 TO VTCB_Size DO
	{
	if (VTCB_ptr[J] NEQA 0)
	    {
	    RB->0 = RB->0 + 1; // bump return vector index/count elements.
	    RB[RB->0] = J;    // Return index into table
	    };
	};
    };



VTCB_Indx_OK ( LCID )
    {
    if ((LCID GEQ 1) && (LCID <= VTCB_Size))
	if ((VTCB_ptr[LCID] != 0)) RETURN 1;

    0
    };



!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Conect_Table ADT routines
!---------------------------------------------------------------------



//SBTTL "FIND Local Port in Connection Table"
/*
Function:
	Scan connection table for specified local port.

Outputs:
	CONECT index or -1 for local_port NOT found.

*/


Find_Local_Port(Port)
    {
    signed long
	P,
	LP;

    LP = Not_Found;
    P = Port && %X"FFFF" ;
    NOINT;
    INCR J FROM 0 TO ConectSize - 1  DO
	if (ConectPtr[J,CN$Local_Port] EQLU P)
	    EXITLOOP LP = J;
    OKINT;
    RETURN(LP);
    };



// Remove the TCB from local port list.
// Check if the Local_port queue of TCB's is now empty.
// If TRUE Then remove the local-port name from the Connection table (CONECT).
// REMQUE must not be interrupted...

Conect_Remove ( struct tcb_structure * tcb )
    {
    signed long
	RC,
	tcb_ptr;

    if ((REMQUE(tcb,tcb_ptr)) == queue_empty_now)
	{
	if ((RC=Find_Local_Port(tcb->local_port)) != Error)
	    ConectPtr[rc,CN$Local_Port] = -1; // Make CONECT entry avail.
	1
	}
    else 0
    };


//SBTTL "TCP mechanism overview"
//SBTTL "Init Local Port Entry"
/*
Function:
	Initialize an entry in the local port table.

Inputs:
	Index,		// which table entry to initialize.
	LPort		// Local port which this entry represents.

*/

void Init_LP_Entry (Index, LPort) (void)
    {
    NOINT;
    ConectPtr[index,CN$Local_Port] = (Lport && %X"FFFF") ;
    ConectPtr[Index,CN$TCB_List] = ConectPtr[Index,CN$TCB_List];
    ConectPtr[Index,CN$TCB_Tail] = ConectPtr[Index,CN$TCB_List];
    OKINT;
    };



//SBTTL "Find Free Connection Table Entry"
/*
Function:
	Scan connection tbl "CONECT" for an entry which has the local_port
	field set to -1 for available.

Outputs:
	CONECT index of free entry.
	"Not_Found"(-1) for table full.
*/




Find_Free_LP_Entry (LPort)
    {
    EXTERNAL ROUTINE
	MovByt,
	MM$Get_Mem, MM$Free_Mem;
    signed long
	J,
	Old,	
	IDX : ^GLOBAL;

    NOINT;
    Idx = Not_Found;
    INCR J FROM 0 TO ConectSize - 1  DO
	if (CONECTPTR[J,CN$Local_Port] == -1)
	    {
	    OKINT;
	    RETURN J
	    };

    // Bypass the conect_table expansion code (for now...)
    OKINT;
    RETURN Not_Found;

/*
//    OPR$FAO("TCP: Growing conect table to !SW entries...!/",ConectSize*2);
    // This should be called at AST level to assure that we aren't
    // pulling the rug out from anyone

//    Idx = ConectSize;
//    ConectSize = ConectSize * 2;

    Old = ConectPtr;
//    MM$Get_Mem( ConectPtr , ConectSize * CN$Blk_Size * 4 );

    // Initialize the new table.
//    INCR cidx FROM (Idx) TO (ConectSize-1) DO
	{				// Initialize connection table
	ConectPtr[cidx,CN$TCB_List] = ConectPtr[cidx,CN$TCB_List];
	ConectPtr[cidx,CN$TCB_Tail] = ConectPtr[cidx,CN$TCB_List];
	ConectPtr[cidx,CN$Local_Port] = -1;
	};
    MovByt ( Idx * CN$Blk_Size * 4 , Old , ConectPtr );

    OKINT;
    MM$Free_Mem( Old , Idx * CN$Blk_Size * 4 );
//    XLOG$FAO(LOG$TCP,"TCP: Grew conect table to !SW entries...!/",ConectSize);

    IDX
*/
    };


//SBTTL "Conect_Insert - Insert TCB into CONECT table"
/*
// Insert the TCB into the Local port list according to the following order:
// Fully-Specified Foreign(Host & Port) first, Partially-specified Foreign(H&P)
// Last comes fully-UNspecified Foreign(H&P).
*/

void Conect_Insert(struct tcb_structure * tcb,CN_Index) (void)
    {
    signed long
	Ostruct tcb_structure * tcb,
	IN;

// Point at 1st TCB in local port list.

    NOINT;
    OTCB = ConectPtr[CN_Index,CN$tcb_list];
    if (OTCB EQLA ConectPtr[CN_Index,CN$TCB_List])
	// Empty Local Port List
	IN = ConectPtr[CN_Index,CN$TCB_List]  // Inset into Empty list.
    else
	{
	SELECTONE TRUE OF
	SET

// Fully Unspecified?

	[tcb->Foreign_Host == wild && tcb->Foreign_Port == Wild]:
	    IN = ConectPtr[CN_Index,CN$tcb_tail];  // Yes - insert at tail.

// Fully Specified?


	[tcb->Foreign_Host != Wild && tcb->Foreign_Port != Wild]:
	    IN = ConectPtr[CN_Index,CN$TCB_List]; // yes - insert at Head.

// Partially unspecified, insert before fully-unspecified or at end of list,
// which ever is approprate.

	[OTHERWISE]:
	    {
	    signed long
		OK;
	    OK = False;
	    IN = ConectPtr[CN_Index,CN$TCB_Tail]; // Default: add to tail.
	    WHILE (OTCB NEQA ConectPtr[CN_Index,CN$TCB_List]) AND
		  (NOT OK) DO
		{
		IF (OTCB->Foreign_Host == Wild) OR
		   (OTCB->Foreign_Port eql Wild) THEN
		    {
		    OK = True;
		    IN = OTCB->LP_Back; // point at previous TCB.
		    }
		else
		    OTCB = OTCB->LP_Next;
		}
	    };
	TES;
	};

// Insert TCB in the Local Port list.

!!!HACK!!// should be conditional
//    LOG$FAO("Conect_Insert TCB=!XL IN=!XL!/",TCB,IN);
    INSQUE(TCB,IN);
    OKINT;
    };



//SBTTL "Verify Requested Socket Pair represents a unique connection."
/*

Function:

	Verify specified connection is unique or the requestor has privileges
	to request a non-unique connection (case of: Well-Known-Port).  User
	can specify a local port on an active open if the port is in the range
	of [32768..65535].  Otherwise TCP will supply the local port.  To open
	a passive connection the connection must be unique or the user MUST
	have privileges.  To use a well-known port [1..255] the user must have
	privileges.

Inputs:

	Lport = Requested local port #
	Fhost = Foreign_Host (Internet adrs + Host adrs)
	Fport = Foreign_Port
		F(host or port) maybe unspecified(wild=0)
	IDX = Address of fullword which gets conect table index.

Outputs:

	TRUE = unique connection.
	False = non-unique connection
	-1 = Connection tbl (CONECT) is full

Side Effects:

	Fullword which IDX points at gets the index into the connection
	table (CONECT) for the specified local port.  If the specified
	Local_Port is not in connection table (CONECT) then it will be
	inserted if space is avail.
*/


CHECK_UNIQUE_CONN(Lport,Fhost,Fport,IDX)
    {
    REGISTER
	struct tcb_structure * tcb;
    signed long
	Index,
	LP,
	FP,
	unique;

    NOINT;
    FP = (FPort && %X"FFFF") ;
    LP = (LPort && %X"FFFF") ;
    if ((Index = Find_Local_Port (LP) ) == Not_Found)
	{			// Insert local port into connection table
	index = Find_Free_LP_Entry(LP);
	if (Index == Error)
	    Unique = Error  // CONECT tbl is full.
	else
	    {

// index = connection table index for specified local port.
// fill in connection table(CONECT) entry.

 // Init local port queue header.

	    Init_LP_Entry (Index, LP);
	    IDX = Index;	// return conect index.
	    Unique = True;	// Connection is unique.
	    };
	}
    else			// Local port is in conect table
	{

// Search this Local Port's TCB list checking for non-unique connection.
// Compare rtn call arg connection(Fhost,Fport) with TCB(Foreign_Host&Port).

	.IDX = Index;		// return conect index.
	Unique = True;		// assume a good attitude!
	tcb = ConectPtr[Index,CN$TCB_List]; !point at 1st TCB is list.
	WHILE (tcb NEQA ConectPtr[index,cn$tcb_list]) && (unique == true) DO
	    {
	    IF (TCB->Foreign_Host == FHOST) AND
	       ((TCB->Foreign_Port && %X"FFFF") == FP) AND
	       (TCB->State neq CS$Inactive) THEN
		Unique = False
	    else
		tcb = TCB->LP_Next;
	    }
	};
    OKINT;
    RETURN(Unique);
    };



//SBTTL "TCB_Find - Find TCB for specified frn addrs and ports"
/*

Function:

	Given foreign address and port pair, find the TCB.

Inputs:

	lclport = Local port number
	frnaddr = Foreign address
	frnport = Foreign port

Outputs:

	False(0) = unknown connection
	Otherwise The address of the destination connection's TCB.

Side Effects:

	None.
*/


TCB_Find(lclport,frnaddr,frnport)=
    {
    REGISTER
	struct tcb_structure * tcb,
	result;
    signed long
	LP,
	FP,
	Cn_Idx;

// Find the connection this segment was destined for.

    NOINT;
    LP = lclport && %X"FFFF" ;
    FP = frnport && %X"FFFF" ;
    result=0;
    if ((cn_idx = Find_Local_Port(LP)) != Error)
	{
	TCB = ConectPtr[CN_IDX,CN$TCB_list]; // point at TCB.
//    LOG$FAO ( "cn_idx = !XL (!XW !XL !XW) TCB=!XL !/", cn_idx ,lclport,frnaddr,frnport,TCB);
	WHILE (TCB NEQA ConectPtr[cn_idx,CN$TCB_list]) DO
	    {

// Check seg source-address aginst TCB foreign-host or a wild foreign-host.
!	LOG$FAO ( "TCB = !XL !/", TCB);

	    IF (frnaddr EQLU TCB->Foreign_Host) || 
	       (TCB->Foreign_Host EQLU Wild) THEN
		{

// Check seg source-port aginst TCB foreign-port or a wild foreign-port
// If a match make sure TCB has NOT been inactivated.

		IF (FP EQLU (TCB->Foreign_Port && %X"FFFF")) OR
		   ((TCB->Foreign_Port && %X"FFFF") EQLU Wild) THEN
		    if (TCB->state != CS$Inactive)
			EXITLOOP result = TCB;
		};
	    TCB = TCB->LP_Next;// advance to next TCB in list.
	    };
	};

// Checked all TCB's and not found - return failure
	
//    LOG$FAO ( "Find_TCB returns !XL!/", result );
    OKINT;
    result
    };



void TCB_Promote ( struct tcb_structure * tcb ) (void)
    {
    signed long
	TCBptr;

    NOINT;
    REMQUE(TCB,TCBptr);
    INSQUE(TCBptr,ConectPtr[TCB->Con_Index,CN$TCB_list]);
    OKINT;
    };



!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// TCB ADT routines
!---------------------------------------------------------------------



//SBTTL "TCB_Create: Create a Transmission Control Blk."
/*

Function:

	Create one TCB (transmission Control Block) & place it's address in
	the valid TCB table.

Calling Convention:

	CALLS, standard BLISS linkage.

Inputs:

	none

Outputs:

	Address of newly allocated uninitialized TCB.
	Error(-1) for Valid TCB table full

Side Effects:

	Address of newly allocated TCB is placed in first free entry
	in the Valid TCB table.

*/

TCB$Create (void)
    {
    signed long
	struct tcb_structure * tcb,	// New TCB
	Indx,	// location of new TCB in Valid TCB table.
	Old,	// remember the old value of VTCB_ptr when resizing...
	S}Q,	// Send Queue
	RECVQ,	// Receive Queue
	RC;	// return code

    NOINT;			// Hold AST's please...
    MM$Get_Mem ( TCB   , tcb_Size*4 );
    MM$Get_Mem ( S}Q , Window_Default );
    MM$Get_Mem ( RECVQ , Window_Default );
    OKINT;

//    CH$FILL(%CHAR(0),tcb_size*4,TCB);	// clean house.....zero fill.

// Set pointers and sizes of send and receive queues

    TCB->SND_Q_BASE = TCB->SRX_Q_BASE = S}Q;
    TCB->SND_Q_SIZE = TCB->SRX_Q_SIZE = Window_Default;
    TCB->RCV_Q_BASE = RECVQ;
    TCB->RCV_Q_SIZE = Window_Default;

// Find an empty Valid_TCB_table entry for the newly created TCB address.
    VTCB_Insert ( TCB );

    TCB
    };



//SBTTL "TCB-Delete:  Delete one TCB"
/*

Function:

	Free the memory associated with a specified TCB.  If this happens
	to be the last TCB in a Local_Port TCB list then free the Local_port
	slot from the Connection table (CONECT).

Inputs:

	TCB_Ptr = address of TCB data structure.

Outputs:

	None.

Side Effects:

	Connection table (CONECT) entry maybe cleared if last TCB for specified
	local_port.

*/

void TCB$Delete ( TCB_Ptr ) (void)
    {
    EXTERNAL ROUTINE
	TELNET_CLOSE;
    signed long
	RC,
	IDX,
	struct tcb_structure * tcb;

    TCB = TCB_Ptr;

// Flush any TVT data block.

    if (TCB->TVTDATA != 0)
	TELNET_CLOSE(TCB);

    // Remove the TCB from the Valid TCB table.
    NOINT;			// Hold AST's
    if (VTCB_Remove(TCB) NEQA 0)
	{

	// Remove the TCB from local port list.
	Conect_Remove ( TCB );

// First, deallocate the queues for this TCB
	MM$Free_Mem ( TCB->SND_Q_BASE, TCB->SND_Q_SIZE ); 
	MM$Free_Mem ( TCB->RCV_Q_BASE, TCB->RCV_Q_SIZE ); 
// Then, deallocate the TCB itself
	MM$Free_Mem ( tcb_ptr, tcb_size*4 );

	}
    else
	Warn_Error("Attempt to Delete unknown TCB,");

    OKINT;
    };



//SBTTL "TCB_OK - Verify that Local_Connection_ID is really a TCB address."
/*


Function:

	Scan the valid tcb table checking for a match with the user specified
	local_connection_id, which is actually the TCB's address.  If found
	then check to see if the TCB is still active (ie, state neq
	CS$inactive).
	Returns TCB address for valid TCB, or 0, with ERROR value returned.
	All this inactive business stems from the fact that under some
	pathological conditions a connection maybe deleted from under the
	user.  The inactive scheme allows the user a one time access to a
	connection that has gone away and gives the user an idea as to why
	instead of just the knowledge that the connectio has gone away.

Inputs:	

	TCBptr = user specified Local_Connection_ID.

Outputs:

	OK = (0) Valid connection ID, RTCB updated to TCB address.
	OTHERWISE the appropriate error code.

Side Effects:

	If the TCB was marked inactive (TCB->State == CS$Inactive) then
	the inactive code is returned as the routine value.  The TCB is
	completly deleted.
*/

TCB_OK(TCBIDX,ERR,struct User_Default_Args * UARGS)
    {
    EXTERNAL ROUTINE
	TCP$KILL_P}ING_REQUESTS;
    MACRO
	TCBERR(EC) = (ERR = EC; RETURN 0) %;
    REGISTER
	struct tcb_structure * tcb;

// The following two checks are to verify the validity of the connection ID.
// Since the user never touches connection IDs (except for priviliged
// functions), neither of these checks should ever fail.

    if ((TCBIDX <= 0) || (TCBIDX > VTCB_Size))
	TCBERR(NET$_CDE);	// Bad connection-ID
    TCB = VTCB_Ptr[TCBIDX];
    if (TCB <= 0)
	TCBERR(NET$_CDE);	// TCB has been deleted
    IF (TCB->VTCB_INDEX != TCBIDX) OR
       (TCB->UCB_ADRS != Uargs->UD$UCB_Adrs) THEN
	TCBERR(NET$_CDE);	// Confusion...

// Check to see if the connection is still active
// (ie, TCB->State != CS$Inactive).

    if (TCB->State == CS$Inactive)
	{
	signed long
	    TMP;
	TMP = TCB->Inactive_Code; // pickup reason we are inactive.
	TCP$KILL_P}ING_REQUESTS(TCB,Tmp); // clean up & post user IO.
	TCB$Delete(TCB); // Delete the inactive connection.
	.ERR = TMP;
	RETURN 0;
	}
    else
	RETURN TCB;		// Good connection - return TCB
    };



//SBTTL "GET_TCB - Translate index into a TCB."
/*


Function:

	Simple index to TCB translation with a few error checks.

Inputs:	

	TCBidx = user specified Local_Connection_ID.

Outputs:

	SS$_NORMAL if everything is ok,
	OTHERWISE the appropriate error code.

Side Effects:

*/

GET_TCB(TCBIDX,TCBret)
    {
    EXTERNAL ROUTINE
	TCP$KILL_P}ING_REQUESTS;
    REGISTER
	struct tcb_structure * tcb;

// The following two checks are to verify the validity of the connection ID.
// Since the user never touches connection IDs (except for priviliged
// functions), neither of these checks should ever fail.

    if ((TCBIDX <= 0) || (TCBIDX > VTCB_Size))
	RETURN NET$_CDE;	// Bad connection-ID
    TCB = VTCB_Ptr[TCBIDX];
    if (TCB <= 0)
	RETURN NET$_CDE;	// TCB has been deleted
    if ((TCB->VTCB_INDEX != TCBIDX))
	RETURN NET$_CDE;	// Confusion...

    TCBret = TCB;		// Good connection - return TCB
    SS$_NORMAL
    };


//SBTTL "Calculate system uptime"

CALCULATE_UPTIME (void)
// Calculates the system uptime and stores it in delta system time format
// in the global variable TEK$sys_uptime, a quadword.  Also returns the
    {
    signed long
	uptime,
	time_buffer : VECTOR [ 8, WORD ],
	One: VECTOR->2 Initial(1,0),	// QuadWord of val 1.
	Now: VECTOR->2;			// time as in now.

    $gettim(timadr=Now);	// current time
    Subm(2,Start_Time,Now,Now); // compute uptime.

// Convert to delta system time (ie, multiply by -1)  Problem is that we have
// time in quadwords.

    Now->0 = NOT Now->0;	// Compute Delta time (quadword).
    Now->1 = NOT Now->1;
    Addm(2,Now,One,TEK$sys_uptime); // compute Delta TCP uptime.

    $numtim( TIMBUF = time_buffer, TIMADR = TEK$sys_uptime);

    // time in hundredth of seconds
    uptime = time_buffer->6 +
		time_buffer->5 * 100 +
		time_buffer->4 * 100 * 60 +
		time_buffer->3 * 100 * 60 * 60 +
		time_buffer->2 * 100 * 60 * 60 * 24 +
		time_buffer->1 * 100 * 60 * 60 * 24 * 30
    };

} ELUDOM
