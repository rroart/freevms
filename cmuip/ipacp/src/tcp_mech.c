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

*/


//SBTTL "Module definition"

#if 0
MODULE TCP_MECH(IDENT="1.0c",LANGUAGE(BLISS32),
		ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
				NONEXTERNAL=LONG_RELATIVE),
		LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
		OPTIMIZE,OPTLEVEL=3,ZIP)
#endif

#include <starlet.h>	// VMS system definitions
     // not yet #include <cmuip/central/include/netxport.h>	// XPORT data structure definitions
#include <cmuip/central/include/neterror.h>	// Network error message definitions
#include "netvms.h"	// Special VMS definitions
#include <cmuip/central/include/netcommon.h> // Network common defs
#include <cmuip/central/include/nettcpip.h>	// TCP/IP protocols
#include "structure.h"		// TCP system data structure definitions
#include "tcpmacros.h"		// System macro definitions
#include "tcp.h"			// TCP related definitions
#include "snmp.h"			// Simple Network Management Protocol

#include <ssdef.h>

#ifndef NOKERNEL
#define sys$numtim exe$numtim
#define sys$gettim exe$gettim
#endif

extern
    mm$get_mem(), mm$free_mem();


//SBTTL "System variable allocation and declaration"

extern
    window_default ;

unsigned long long    Start_Time;	// Quadword time IPACP started.
unsigned long long    TEK$sys_uptime;	// Quadword delta time since IPACP started.
struct TCP_MIB_struct tcp_mib_, * tcp_mib=&tcp_mib_;	// TCP management Information Block



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

static ConectSize;
static struct connection_table_structure * ConectPtr;

signed long
    max_local_ports  = 30;	// Settable in CONFIG

// Valid TCB table, contains pointers to valid TCB's. Used to verify 
// user supplied Local_connection_Id.  As one sees: A local_Connection_ID is
// actually the address of a TCB.

long * vtcb_ptr;
signed long    max_tcb  = 0,
    max_conn  = 60,	// Settable in CONFIG
    vtcb_size  = 0,
    tcb_count  = 0;	// # of valid TCB's in VTCB table.


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

//!!HACK!!// we're breaking the ADT... (gasp)

void tcp$init (void)
    {
    signed long
	cidx;

    // Allocate the connection list
    ConectSize = max_local_ports;
    mm$get_mem( &ConectPtr , ConectSize * CN$BLK_SIZE );
    for (cidx=0;cidx<=ConectSize-1;cidx++)
	{				// Initialize connection table
	ConectPtr[cidx].CN$TCB_List = &ConectPtr[cidx].CN$TCB_List;
	ConectPtr[cidx].CN$TCB_Tail = &ConectPtr[cidx].CN$TCB_List;
	ConectPtr[cidx].CN$Local_Port = -1;
	};

    // Allocate the valid TCB table
    vtcb_size = max_conn;
    mm$get_mem ( &vtcb_ptr , (vtcb_size+1) * sizeof(long) );
    CH$FILL ( 0 , (vtcb_size+1) * sizeof(long) , vtcb_ptr );

    tcp_mib->MIB$tcpRtoAlgorithm= 0;
    tcp_mib->MIB$tcpRtoMin	= 0;
    tcp_mib->MIB$tcpRtoMax	= 0;
    tcp_mib->MIB$tcpMaxConn	= 0;
    tcp_mib->MIB$tcpActiveOpens	= 0;
    tcp_mib->MIB$tcpPassiveOpens= 0;
    tcp_mib->MIB$tcpAttemptFails= 0;
    tcp_mib->MIB$tcpEstabResets	= 0;
    tcp_mib->MIB$tcpCurrEstab	= 0;
    tcp_mib->MIB$tcpInSegs	= 0;
    tcp_mib->MIB$tcpOutSegs	= 0;
    tcp_mib->MIB$tcpRetransSegs	= 0;

    };



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// VTCB ADT routines
//---------------------------------------------------------------------



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


VTCB_Insert ( struct tcb_structure * TCB )
    {
extern	MOVBYT();
 signed long J,
	Indx,
	Old;

    NOINT;

    Indx = 0;
    for (J=1;J<=vtcb_size;J++)
	if (vtcb_ptr[J] == 0) 
	  { Indx = J; break; }
    if (Indx == 0)
	{
	OPR$FAO("MMgr: Growing Valid TCB table to !SW entries...!/",
		 vtcb_size*2);
	// This should be called at AST level to assure that we aren't
	// pulling the rug out from anyone

	Indx = vtcb_size;
	vtcb_size = vtcb_size * 2;

	Old = vtcb_ptr;
	mm$get_mem( &vtcb_ptr , (vtcb_size+1) * sizeof(long) );
	MOVBYT ( (Indx+1) * sizeof(long) , Old , vtcb_ptr );
	mm$free_mem( Old , (Indx+1) * sizeof(long) );

        Indx = Indx + 1;
	};

    // Maintain pointer to last TCB in table
    if (Indx > max_tcb) max_tcb = Indx;
//OPR$FAO("!%T max_tcb = !UW",0,max_tcb);

    // Link the new TCB into the Valid TCB table.
    vtcb_ptr[Indx] = TCB; // set TCB's address
    tcb_count = tcb_count + 1; // Keep track of active TCB's.
    TCB->vtcb_index = Indx; // Remember index into Valid TCB Table

    OKINT;	// Carry on...

    return SS$_NORMAL;
    }



VTCB_Remove ( struct tcb_structure * TCB )
    {
      signed long J,
	IDX;

    // Get index into Valid TCB table
    IDX = TCB->vtcb_index;
    if (vtcb_ptr[IDX] == TCB)
	{
        vtcb_ptr[IDX] = 0;	// Clean out entry
	tcb_count = tcb_count-1; // Account for this TCB going away.
	if (max_tcb <= IDX)
	   X : {
	   for (J=IDX;J>=1;J--)
	     if (vtcb_ptr[J] != 0) { max_tcb = J; goto leave; }
	   // No valid TCB's left?  Do a sanity check.
	   for (J=vtcb_size;J>=1;J--)
	     if (vtcb_ptr[J] != 0) { max_tcb = J; goto leave; }
	   max_tcb = 1;
	   };  // end of block X
	leave:
	{
	}
	}
    else return 0;

    return TCB;
    }



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
     signed long (*ASTRTN)();
    {
      signed long J,
	sum  = 0,
	count;

    count = tcb_count;

    NOINT;

    for (J=1;J<=vtcb_size;J++)
	if (vtcb_ptr[J] != 0)
	    {
	    if (J > max_tcb)
		OPR$FAO("%T TCB (!UL) above max_tcb (!UL)",J,max_tcb);

	    sum = sum + (ASTRTN)(vtcb_ptr[J],J,ASTP1,ASTP2);

	    if ((count = count-1) <= 0) //only process what we have.
		{
		OKINT;
		return sum;	// all done.
		}
	    };
    OKINT;

    return sum;
    }



void tcp$connection_list(RB)
//
// Dump the list of TCP connections.
//
D$LC_ID_Return_Blk RB;
    {
      signed long J;

    RB[0] = 0;
    for (J=1;J<=vtcb_size;J++)
	{
	if (vtcb_ptr[J] != 0)
	    {
	    RB[0] = RB[0] + 1; // bump return vector index/count elements.
	    RB[RB[0]] = J;    // Return index into table
	    };
	};
    }



VTCB_Indx_OK ( LCID )
    {
    if ((LCID >= 1) && (LCID <= vtcb_size))
	if ((vtcb_ptr[LCID] != 0)) return 1;

    return 0;
    }



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Conect_Table ADT routines
//---------------------------------------------------------------------



//SBTTL "FIND Local Port in Connection Table"
/*
Function:
	Scan connection table for specified local port.

Outputs:
	CONECT index or -1 for local_port NOT found.

*/


Find_Local_Port(Port)
    {
      signed long J,
	P,
	LP;

    LP = NOT_FOUND;
    P = Port & 0xFFFF ;
    NOINT;
    for (J=0;J<=ConectSize - 1 ;J++)
	if (ConectPtr[J].CN$Local_Port == P)
	  { LP = J; break; }
    OKINT;
    return(LP);
    }



// Remove the TCB from local port list.
// Check if the Local_port queue of TCB's is now empty.
// If TRUE Then remove the local-port name from the Connection table (CONECT).
// REMQUE must not be interrupted...

Conect_Remove ( struct tcb_structure * TCB )
    {
    signed long
	RC,
	tcb_ptr;

    if ((REMQUE(TCB,&tcb_ptr)) == QUEUE_EMPTY_NOW)  // check
	{
	if ((RC=Find_Local_Port(TCB->local_port)) != ERROR)
	    ConectPtr[RC].CN$Local_Port = -1; // Make CONECT entry avail.
	return 1;
	}
    else return 0;
    }


//SBTTL "TCP mechanism overview"
//SBTTL "Init Local Port Entry"
/*
Function:
	Initialize an entry in the local port table.

Inputs:
	Index,		// which table entry to initialize.
	LPort		// Local port which this entry represents.

*/

void Init_LP_Entry (Index, LPort)
    {
    NOINT;
    ConectPtr[Index].CN$Local_Port = (LPort & 0xFFFF) ;
    ConectPtr[Index].CN$TCB_List = ConectPtr[Index].CN$TCB_List;
    ConectPtr[Index].CN$TCB_Tail = ConectPtr[Index].CN$TCB_List;
    OKINT;
    }



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
extern	MOVBYT(),
	mm$get_mem(), mm$free_mem();
    signed long
	J,
	Old,	
      idx;

    NOINT;
    idx = NOT_FOUND;
    for (J=0;J<=ConectSize - 1 ;J++)
	if (ConectPtr[J].CN$Local_Port == -1)
	    {
	    OKINT;
	    return J;
	    };

    // Bypass the conect_table expansion code (for now...)
    OKINT;
    return NOT_FOUND;

/*
//    OPR$FAO("TCP: Growing conect table to !SW entries...!/",ConectSize*2);
    // This should be called at AST level to assure that we aren't
    // pulling the rug out from anyone

//    Idx = ConectSize;
//    ConectSize = ConectSize * 2;

    Old = ConectPtr;
//    mm$get_mem( &ConectPtr , ConectSize * CN$Blk_Size * 4 );

    // Initialize the new table.
//    INCR cidx FROM (Idx) TO (ConectSize-1) DO
	{				// Initialize connection table
	ConectPtr[cidx].CN$TCB_List = ConectPtr[cidx].CN$TCB_List;
	ConectPtr[cidx].CN$TCB_Tail = ConectPtr[cidx].CN$TCB_List;
	ConectPtr[cidx].CN$Local_Port = -1;
	};
    MOVBYT ( idx * CN$BLK_SIZE, Old , ConectPtr );

    OKINT;
    mm$free_mem( Old , idx * CN$BLK_SIZE);
//    XLOG$FAO(LOG$TCP,"TCP: Grew conect table to !SW entries...!/",ConectSize);

return idx;
*/
    }


//SBTTL "Conect_Insert - Insert TCB into CONECT table"
/*
// Insert the TCB into the Local port list according to the following order:
// Fully-Specified Foreign(Host & Port) first, Partially-specified Foreign(H&P)
// Last comes fully-UNspecified Foreign(H&P).
*/

void Conect_Insert(struct tcb_structure * TCB,signed long CN_Index)
    {
      struct tcb_structure * OTCB;
      signed long IN;

// Point at 1st TCB in local port list.

    NOINT;
    OTCB = ConectPtr[CN_Index].CN$TCB_List;
    if (OTCB == &ConectPtr[CN_Index].CN$TCB_List)
	// Empty Local Port List
	IN = ConectPtr[CN_Index].CN$TCB_List;  // Inset into Empty list.
    else
	{
// Fully Unspecified?

	  if (TCB->foreign_host == WILD && TCB->foreign_port == WILD)
	    IN = ConectPtr[CN_Index].CN$TCB_Tail;  // Yes - insert at tail.
	  else {
// Fully Specified?


	    if (TCB->foreign_host != WILD && TCB->foreign_port != WILD)
	      IN = ConectPtr[CN_Index].CN$TCB_List; // yes - insert at Head.
	    else {
// Partially unspecified, insert before fully-unspecified or at end of list,
// which ever is approprate.

	    {
	    signed long
		ok;
	    ok = FALSE;
	    IN = ConectPtr[CN_Index].CN$TCB_Tail; // Default: add to tail.
	    while ((OTCB != &ConectPtr[CN_Index].CN$TCB_List) &&
		   (! ok))
		{
		if ((OTCB->foreign_host == WILD) ||
		    (OTCB->foreign_port == WILD))
		    {
		    ok = TRUE;
		    IN = OTCB->lp_back; // point at previous TCB.
		    }
		else
		    OTCB = OTCB->lp_next;
		}
	    };
	    }
	  }
	}

// Insert TCB in the Local Port list.

//!!HACK!!// should be conditional
//    LOG$FAO("Conect_Insert TCB=!XL IN=!XL!/",TCB,IN);
    INSQUE(TCB,IN);
    OKINT;
    }



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
		F(host or port) maybe unspecified(WILD=0)
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


check_unique_conn(LPort,Fhost,FPort,IDX)
     signed long * IDX;
    {
    register
	struct tcb_structure * TCB;
    signed long
	Index,
	LP,
	FP,
	Unique;

    NOINT;
    FP = (FPort & 0xFFFF) ;
    LP = (LPort & 0xFFFF) ;
    if ((Index = Find_Local_Port (LP) ) == NOT_FOUND)
	{			// Insert local port into connection table
	Index = Find_Free_LP_Entry(LP);
	if (Index == ERROR)
	    Unique = ERROR;  // CONECT tbl is full.
	else
	    {

// index = connection table index for specified local port.
// fill in connection table(CONECT) entry.

 // Init local port queue header.

	    Init_LP_Entry (Index, LP);
	    *IDX = Index;	// return conect index.
	    Unique = TRUE;	// Connection is unique.
	    };
	}
    else			// Local port is in conect table
	{

// Search this Local Port's TCB list checking for non-unique connection.
// Compare rtn call arg connection(Fhost,Fport) with TCB(Foreign_Host&Port).

	*IDX = Index;		// return conect index.
	Unique = TRUE;		// assume a good attitude!
	TCB = ConectPtr[Index].CN$TCB_List; //point at 1st TCB is list.
	while ((TCB != &ConectPtr[Index].CN$TCB_List) && (Unique == TRUE))
	    {
	    if ((TCB->foreign_host == Fhost) &&
	       ((TCB->foreign_port & 0xFFFF) == FP) &&
		(TCB->state != CS$INACTIVE))
	      Unique = FALSE;
	    else
		TCB = TCB->lp_next;
	    }
	} 
    OKINT;
    return(Unique);
    }



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


TCB_Find(lclport,frnaddr,frnport)
    {
    register
    struct tcb_structure * TCB;
    signed long result;
    signed long
	LP,
	FP,
	cn_idx;

// Find the connection this segment was destined for.

    NOINT;
    LP = lclport & 0xFFFF ;
    FP = frnport & 0xFFFF ;
    result=0;
    if ((cn_idx = Find_Local_Port(LP)) != ERROR)
	{
	TCB = ConectPtr[cn_idx].CN$TCB_List; // point at TCB.
//    LOG$FAO ( "cn_idx = !XL (!XW !XL !XW) TCB=!XL !/", cn_idx ,lclport,frnaddr,frnport,TCB);
	while ((TCB != &ConectPtr[cn_idx].CN$TCB_List))
	    {

// Check seg source-address aginst TCB foreign-host or a WILD foreign-host.
//	LOG$FAO ( "TCB = !XL !/", TCB);

	    if ((frnaddr == TCB->foreign_host) || 
		(TCB->foreign_host == WILD))
		{

// Check seg source-port aginst TCB foreign-port or a WILD foreign-port
// If a match make sure TCB has NOT been inactivated.

		if ((FP == (TCB->foreign_port & 0xFFFF)) ||
		    ((TCB->foreign_port & 0xFFFF) == WILD))
		    if (TCB->state != CS$INACTIVE)
		      { result = TCB; break; }
		};
	    TCB = TCB->lp_next;// advance to next TCB in list.
	    };
	};

// Checked all TCB's and not found - return failure
	
//    LOG$FAO ( "Find_TCB returns !XL!/", result );
    OKINT;
    return result;
    }



void TCB_Promote ( struct tcb_structure * TCB )
    {
    signed long
	TCBptr;

    NOINT;
    REMQUE(TCB,&TCBptr);
    INSQUE(TCBptr,ConectPtr[TCB->con_index].CN$TCB_List);
    OKINT;
    }



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// TCB ADT routines
//---------------------------------------------------------------------



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

tcb$create (void)
    {
      struct tcb_structure * TCB;	// New TCB
    signed long
	Indx,	// location of new TCB in Valid TCB table.
	Old,	// remember the old value of VTCB_ptr when resizing...
	SENDQ,	// Send Queue
	RECVQ,	// Receive Queue
	RC;	// return code

    NOINT;			// Hold AST's please...
    mm$get_mem ( &TCB   , TCB_SIZE );
    mm$get_mem ( &SENDQ , window_default );
    mm$get_mem ( &RECVQ , window_default );
    OKINT;

//    CH$FILL(%CHAR(0),tcb_size*4,TCB);	// clean house.....zero fill.

// Set pointers and sizes of send and receive queues

    TCB->snd_q_base = TCB->srx_q_base = SENDQ;
    TCB->snd_q_size = TCB->srx_q_size = window_default;
    TCB->rcv_q_base = RECVQ;
    TCB->rcv_q_size = window_default;

// Find an empty Valid_TCB_table entry for the newly created TCB address.
    VTCB_Insert ( TCB );

return TCB;
    }



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

long tcbdel[16384];
long tcbi=0;

void tcb$delete ( TCB_Ptr )
    {
extern	TELNET_CLOSE();
    signed long
	RC,
      IDX;
	struct tcb_structure * TCB;

    TCB = TCB_Ptr;
    long *l=&TCB_Ptr;
    memcpy(&tcbdel[tcbi], TCB, 20*sizeof(long));
    tcbdel[tcbi]=l[-1];
    tcbdel[tcbi+1]=TCB;
    tcbdel[tcbi+8]=TCB->tvtdata;
    tcbi+=20;
    if (tcbi>16000)
      tcbi=0;

// Flush any TVT data block.

    if (TCB->tvtdata != 0)
	TELNET_CLOSE(TCB);

    // Remove the TCB from the Valid TCB table.
    NOINT;			// Hold AST's
    if (VTCB_Remove(TCB) != 0)
	{

	// Remove the TCB from local port list.
	Conect_Remove ( TCB );

// First, deallocate the queues for this TCB
	mm$free_mem ( TCB->snd_q_base, TCB->snd_q_size ); 
	mm$free_mem ( TCB->rcv_q_base, TCB->rcv_q_size ); 
// Then, deallocate the TCB itself
	mm$free_mem ( TCB_Ptr, TCB_SIZE );

	}
    else
	Warn_Error("Attempt to Delete unknown TCB,");

    OKINT;
    }



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

tcb_ok(signed long TCBIDX,signed long * ERR,struct user_default_args * uargs)
    {
extern	tcp$kill_pending_requests();
#define	TCBERR(EC) {*ERR = EC; return 0;}
    register
	struct tcb_structure * TCB;

// The following two checks are to verify the validity of the connection ID.
// Since the user never touches connection IDs (except for priviliged
// functions), neither of these checks should ever fail.

    if ((TCBIDX <= 0) || (TCBIDX > vtcb_size))
	TCBERR(NET$_CDE);	// Bad connection-ID
    TCB = vtcb_ptr[TCBIDX];
    if (TCB <= 0)
	TCBERR(NET$_CDE);	// TCB has been deleted
    if ((TCB->vtcb_index != TCBIDX) ||
	(TCB->ucb_adrs != uargs->ud$ucb_adrs))
	TCBERR(NET$_CDE);	// Confusion...

// Check to see if the connection is still active
// (ie, TCB->State != CS$Inactive).

    if (TCB->state == CS$INACTIVE)
	{
	signed long
	    tmp;
	tmp = TCB->inactive_code; // pickup reason we are inactive.
	tcp$kill_pending_requests(TCB,tmp); // clean up & post user IO.
	tcb$delete(TCB); // Delete the inactive connection.
	*ERR = tmp;
	return 0;
	}
    else
	return TCB;		// Good connection - return TCB
    }



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
     long * TCBret;
    {

extern	tcp$kill_pending_requests();
    register
	struct tcb_structure * TCB;

// The following two checks are to verify the validity of the connection ID.
// Since the user never touches connection IDs (except for priviliged
// functions), neither of these checks should ever fail.

    if ((TCBIDX <= 0) || (TCBIDX > vtcb_size))
	return NET$_CDE;	// Bad connection-ID
    TCB = vtcb_ptr[TCBIDX];
    if (TCB <= 0)
	return NET$_CDE;	// TCB has been deleted
    if ((TCB->vtcb_index != TCBIDX))
	return NET$_CDE;	// Confusion...

    *TCBret = TCB;		// Good connection - return TCB
	return    SS$_NORMAL;
    }


//SBTTL "Calculate system uptime"

CALCULATE_UPTIME (void)
// Calculates the system uptime and stores it in delta system time format
// in the global variable TEK$sys_uptime, a quadword.  Also returns the
    {
    signed long
    uptime;
unsigned short	time_buffer[8];
unsigned long long	One=1,	// QuadWord of val 1.
	Now;			// time as in now.

    sys$gettim(&Now);	// current time
    Subm(2,Start_Time,Now,Now); // compute uptime.

// Convert to delta system time (ie, multiply by -1)  Problem is that we have
// time in quadwords.

    Now = ! Now;	// Compute Delta time (quadword).
#if 0
    Now[1] = ! Now[1];
#endif
    Addm(2,Now,One,TEK$sys_uptime); // compute Delta TCP uptime.

    sys$numtim(time_buffer,TEK$sys_uptime);

    // time in hundredth of seconds
    uptime = time_buffer[6] +
		time_buffer[5] * 100 +
		time_buffer[4] * 100 * 60 +
		time_buffer[3] * 100 * 60 * 60 +
		time_buffer[2] * 100 * 60 * 60 * 24 +
      time_buffer[1] * 100 * 60 * 60 * 24 * 30;
    }

