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
//TITLE "MEMGR - Dynamic Memory allocaion / deallocation rtns."
/*

Module:

	MEMGR

Facility:

	Provide dynamic memory allocation & deallocation for various
	TCP/IP data structures.

Abstract:

	Many of the data structures used by TCP/IP are dynamically allocated
	& deallocated (TCB's,Queue block, etc.).
	Some of the dynamically allocated data structures are preallocated and
	placed on free-lists.  The globals "xxx_xxx_count_base" are initialized
	during startup routine: "configure_acp" in module: config.bli.  These
	byte-size integers represent the number of elements on their respective
	free list.

Author:

	Stan C. Smith	Fall 1981
	This version by Vince Fuller, CMU-CSD, Summer, 1986

Modification History:

*** Begin CMU change log ***

2.8	14-Nov-1991	Henry W. Miller		USBR
	In MM$QBLK_Free(), fixed uninitialized variable PAGES.

2.7b	23-Jul-1991	Henry W. Miller		USBR
	Fix typo in printout in MM$FREE_MEM.
	Fix memory logging in various places.

2.7a	18-Jul-1991	Henry W. Miller		USBR
	Use LIB$GET_VM_PAGE and LIB$FREE_VM_PAGE rather then LIB$GET_VM
	and LIB$FREE_VM.

2.7  18-Aug-89, Edit by BRM (CMU NetDev)
	Added MM$Get_Mem and MM$Free_Mem generic memory allocation routines.
	Changed Conect reference to a pointer.

2.6-1 03-APR-1989, M. Madison, RPI/ECS
        Turn off AST's when doing interruptible REMQUE in routine MM$QBLK_FREE.

2.6  23-Jul-87, Edit by VAF
	Know to flush any TVT data when deleting TCB's.

2.5  20-Jul-87, Edit by VAF
	Minor cleanup of circular queue stuff.

2.4  23-Mar-87, Edit by VAF
	Replace the three standard-sized packet buffers with two - minimum
	size and maximum sized.

2.3   5-Aug-86, Edit by VAF
	Modify TCB$Create/TCB_Delete to also allocate/deallocate the
	the send/receive circular queues for the TCB.

2.2  22-Jul-86, Edit by VAF
	Add code to track allocation, deallocation and queue movement of
	QBlks. Queue movement is under the QDEBUG conditional.

2.1  11-Jul-86, Edit by VAF
	Make all counters global.

2.0  25-Jun-86, Edit by VAF
	Make TCB$Create set VTCB_INDEX field in TCB.
	TCB_DELETE doesn't search VALID_TCB anymore - just uses VTCB_INDEX.
	Don't use index 0 of VALID_TCB table.

1.9  22-May-86, Edit by VAF
	In TCB$Delete - remove TCB from UCB hash queue.

1.8  20-May-86, Edit by VAF
	Use new AST locking scheme - respect previous AST state.

*** End CMU change log ***

1.1 [9-10-81]

	Original version.

1.2 [6-5-82]

	Free-lists implemented for frequently accessed data structures.

1.3 [4-28-83]

	Number of elements on the free-lists is configurable from the config.txt
	file.  This file is processed during acp initialization phase.

1.4 [5-16-83] stan

	removed "max_seg" references (free_list, count, init) as they are no
	longer different from max_network_size_mesg.

1.5 [5-23-83] stan

	replaced xport memory manager calls with direct calls to vms runtime
	library.  Xport routines eventually used the RTL calls anyway,
	attempt was to increase memory mgmt speed.  Effect was dramatic.

1.6 [7-15-83] stan

	force byte-size for some external literals, explicit var's in register
	declarations.

1.7 [5-30-85] noelan olson
	Track current overflow and maximum size of preallocated queues.

*/

//SBTTL "Module & Environment Definition"

MODULE MEMGR(Ident="2.8",LANGUAGE(BLISS32),
	     ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			     NONEXTERNAL=LONG_RELATIVE),
	     LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	     OPTIMIZE,OPTLEVEL=3,ZIP)=
{

#include "SYS$LIBRARY:STARLET";	// Include VMS systems definitions
#include "CMUIP_SRC:[CENTRAL]NETXPORT";	// Include transportablity package
#include "CMUIP_SRC:[CENTRAL]NETVMS";			// VMS specifics
#include "TCP";			// TCP related definitions
#include "TCPMACROS";		// Local Macros
#include "STRUCTURE";		// Structure & Field definitions

//SBTTL "External: Routines, Literals and data"

extern
    LIB$GET_VM : ADDRESSING_MODE(GENERAL),
    LIB$FREE_VM : ADDRESSING_MODE(GENERAL),
    LIB$GET_VM_PAGE : ADDRESSING_MODE(GENERAL),
    LIB$FREE_VM_PAGE : ADDRESSING_MODE(GENERAL),
//    TCP$Find_Local_Port,	// USER.BLI
 VOID    TELNET_CLOSE,	// TCP_TELNET.BLI
    MovByt;			// MACLIB.MAR

// External Data Segements

extern signed long
//    struct Connection_Table_Structure * ConectPtr,
//    struct VECTOR * VTCB_ptr->0,	// Vector of Valid TCB pointers.
//    VTCB_Size,
//    Max_TCB,
    ast_in_progress,		// Flag if running at AST level
    INTDF,			// interrupt deferral count
    MIN_PHYSICAL_BUFSIZE,
    MAX_PHYSICAL_BUFSIZE;

//SBTTL "Memory manager header structure"

$FIELD MEM$HDR_FIELDS (void)
SET
  MEM$QNEXT	= [$Address],	// Next item on queue
  MEM$QPREV	= [$Address],	// Previous item on queue
//IF QDEBUG %THEN
  MEM$CURQUEUES	= [$Ulong],	// Queues this block is on
  MEM$ALLQUEUES = [$Ulong],	// Queues this block has been on
  MEM$ALLOCRTN	= [$Address],	// Routine which allocated most recently
  MEM$FREERTN	= [$Address],	// Routine which freed most recently
  MEM$INSQUERTN	= [$Address],	// Routine which INSQUE'd most recently
  MEM$INSQUEHDR	= [$Address],	// Queue header where most recently INSQUE'd
  MEM$INSQUEVAL	= [$Address],	// Additional value when INSQUE'd
  MEM$REMQUERTN	= [$Address],	// Routine which REMQUE'd most recently
  MEM$REMQUEHDR	= [$Address],	// Queue header where most recently REMQUE'd
  MEM$REMQUEVAL	= [$Address],	// Additional value when REMQUE'd
//FI
  MEM$FLAGS	= [$Ulong],	// Flags defining free, prealloc, etc.
  $OVERLAY(MEM$FLAGS)
      MEM$ISFREE = [$BIT],	// This block is free
      MEM$ISPERM = [$BIT]	// This block is permanent (preallocated)
  $CONTINUE
TES;

LITERAL
    MEM$HDR_SIZE = $FIELD_SET_SIZE;
MACRO
    MEM$HDR_STRUCT = BLOCK->MEM$HDR_SIZE FIELD(MEM$HDR_FIELDS) %;

//Sbttl "Preallocated dynamic data structures."
/*

Here we define memory management preallocated data structure queues.
Idea is to keep specified number of dynamic data structures around so we
don't have to dynamically allocate the structure each time we need one.
A little faster execution is what we are after.
*/

// Define and initialize free lists to "empty".

static signed long
    FREE_Qblks : Queue_Header_Structure(Queue_Header_Fields)
		 PRESET(
			[Qhead] = Free_Qblks,
			[Qtail]	= Free_Qblks
			),
    USED_Qblks : Queue_Header_Structure(Queue_Header_Fields)
		 PRESET(
			[Qhead] = Used_Qblks,
			[Qtail] = Used_Qblks
		       ),

    FREE_Uargs : Queue_Header_Structure(Queue_Header_Fields)
		 PRESET(
			[Qhead]	= Free_Uargs,
			[Qtail]	= Free_Uargs
			),

    Free_Minsize_Segs : Queue_Header_Structure(Queue_Header_Fields)
			PRESET (
				[Qhead]	= Free_Minsize_Segs,
				[Qtail]	= Free_Minsize_Segs
				),

    Free_Maxsize_Segs : Queue_Header_Structure(Queue_Header_Fields)
			    PRESET (
				[Qhead] = Free_Maxsize_Segs,
				[Qtail] = Free_Maxsize_Segs
				);

// "count_base" items are initialized in conifg_acp routine during startup.
// they provide the base number of items on a free list.

signed long
    QBLK_Count_base : UNSIGNED BYTE INITIAL(0),	// #Qblks on free list.
    Uarg_Count_base : UNSIGNED BYTE INITIAL(0),	// #Uarg blks on free list.
    Min_Seg_Count_base: UNSIGNED BYTE INITIAL(0), // #Min-size segs
    Max_Seg_Count_base: UNSIGNED BYTE INITIAL(0), // #Max-size segs

// Counters for the number of items on a specific queue.

    QBLK_Count: UNSIGNED BYTE INITIAL(0),	// #Qblks on free list.
    Uarg_Count: UNSIGNED BYTE INITIAL(0),	// #Uarg blks on free list.
    Min_Seg_Count: UNSIGNED BYTE INITIAL(0),	// #Min-size segs
    Max_Seg_Count: UNSIGNED BYTE INITIAL(0);	// #Max-size segs

/*
Here we keep track of what kind of data structures are being dynamically 
allocated & which should have more reserved during initialization.
*/

signed long
    QB_gets  = 0,	// Queue Blks dynamically allocated
    UA_gets  = 0,	// User arg blks.
    MIN_gets  = 0,	// Minimum size buffers
    MAX_gets  = 0,	// Maximum size buffers
    QB_max  = 0,	// Queue Blk max queue size
    UA_max  = 0,	// User arg max queue size
    MIN_max  = 0,	// Minimum size buffers max queue size
    MAX_max  = 0;	// Maximum seg max queue size

//Sbttl "Memory Management Fault Handler"
/*

Function:

	Die gracefully on memory allocation/deallocation errors.  Idea is to
	NOT hang user processes by making sure all user IO is posted to VMS
	to users don't end up in MWAIT.

Inputs:

	None

Outputs:

	None

Side Effects:

	Post all remaining user IO back to VMS & die gracefully here.
	leave some tracks by sending the network operator a mesage with the
	hex error code in it.
*/


Memgr_Fault_Handler(Caller,Primary_CC,Sec_CC) : NOVALUE (void)
    {
    Signal(Primary_cc);
    Fatal_Error("Memory Mgmt. Fault detected.",Primary_cc);
    };



//SBTTL "Get_Mem: Allocate memory."
/*
Function:
	Allocate a new block of memory.

Calling Convention:
	CALLS, standard BLISS linkage.

Inputs:
	Addr: pointer to longword to receive allocated memory's address
	Size: Amount of memory desired, in bytes.

Outputs:
	Address of newly allocated zeroed memory block.
	Error(0) for no memory available.

Side Effects:
	None

*/

MM$Get_Mem (Addr, Size)
    {
    signed long
	Pages,
	RC;

    Pages = Size / 512 ;
    Pages = Pages + 1 ;
    NOINT;			// Hold AST's please...
//    if (NOT (RC = LIB$GET_VM(Size,Addr)))
    if (NOT (RC = LIB$GET_VM_PAGE(Pages,Addr)))
	Memgr_Fault_Handler(0,RC,0);
    OKINT;

	XLOG$FAO(LOG$MEM,"!%T MM$Get_Mem !XL size !SL!/",0,..ADDR, Size);

    CH$FILL(0,Size,..Addr);	// clean house.....zero fill.

    RC
    };



//SBTTL "Free_Mem:  Release allocated memory."
/*

Function:
	Free the memory allocated by MM$Get_Mem.

Inputs:
	Mem = address of memory block.

Outputs:
	None.

Side Effects:
	None.

*/

MM$Free_Mem(Mem,Size) : NOVALUE (void)
    {
    signed long
	Pages,
	RC;

    Pages = Size / 512 ;
    Pages = Pages + 1 ;

    XLOG$FAO(LOG$MEM,"!%T MM$Free_Mem !XL size !SL!/",0,..Mem, Size);

    NOINT;			// Hold AST's
//    if (NOT ( RC = LIB$FREE_VM ( Size , Mem ) ))
    if (NOT ( RC = LIB$FREE_VM_PAGE ( Pages , Mem ) ))
	    Memgr_Fault_Handler(0,RC,0);
    OKINT;
    };



//SBTTL "Queue Block Handlers"
/*

Function:

	Acquire one queue block.  See TCP Segment definition for Queue block
	structure definition (SEG.DEF).

Calling Conventions:

	CALLS, standard BLISS linkage.

Inputs:

	none

Outputs:

	Pointer to Queue block is returned.

Side Effects:

	Get Queue Block from queue-block free list or dynamically allocate
	one.  If no memory is available then we die here.

*/

MM$QBLK_GET=
    {
    BUILTIN
	R0;	// standard vax/vms routine return value register.
    signed long
	Ptr,
	struct MEM$HDR_STRUCT * Hptr,
	Pages 	:	initial(0);

    if (REMQUE(Free_QBlks->QHead,Hptr) != Empty_Queue)
	QBlk_Count = QBlk_Count - 1 // Say there is 1 less avail.
    else			// allocate a new qb.
	{

// Disallow ast during memory allocation.

	NOINT;

!	if (NOT (LIB$GET_VM(%REF((qb_size+MEM$HDR_SIZE)*4),Hptr)))
	Pages = ((((qb_size + MEM$HDR_SIZE) * 4) / 512) + 1 ) ;
	if (NOT (LIB$GET_VM_PAGE(Pages, Hptr)))
	    Memgr_Fault_Handler(0,R0,0);

	OKINT;
	CH$FILL(%CHAR(0),MEM$HDR_SIZE*4,Hptr);
	Hptr->MEM$ISPERM = FALSE; // Not a permanent qblk
	QB_Gets = QB_gets + 1;	// track this event.
	if (QB_gets > QB_max)
	    QB_max = QB_gets;
	};

    ptr = Hptr + MEM$HDR_SIZE*4; // Point at data area
    CH$FILL(%CHAR(0),qb_size*4,ptr);	// fresh qb.
!!!HACK!!!~~~ Should record allocator here ~~~
    XLOG$FAO(LOG$MEM,"!%T MM$Qblk_Get !XL size !SL!/",0,Hptr, Pages);
    Hptr->MEM$ISFREE = FALSE;	// QB is no longer free
    INSQUE(Hptr,USED_Qblks->QTail); // Insert on used queue
    RETURN(Ptr);
    };

/*

Function:

	Deallocate one queue block.

Calling Conventions:

	CALLS, standard BLISS-32 linkage.

Inputs:

	Address of Queue block structure.

Outputs:

	None.

Side Effects:

	If count of queue-blocks on the free list is < max allowed on list
	then queue the QB otherwise delete the memory.

*/

MM$QBLK_Free(Ptr): NOVALUE (void)
    {
    BUILTIN
	R0;	// standard vax/vms routine return value register.
    signed long
	struct MEM$HDR_STRUCT * Hptr,
	Pages	 = 0;

    Hptr = Ptr - MEM$HDR_SIZE*4; // Point at header
    XLOG$FAO(LOG$MEM,"!%T MM$Qblk_Free !XL size !SL!/",0,Hptr, Pages);
    NOINT;
    REMQUE(Hptr,Hptr);		// Remove from the used queue
    OKINT;
    if (Hptr->MEM$ISPERM)
	{			// Free a permanent block - just put on free Q
!~~~ Record deallocator here ~~~
	Hptr->MEM$ISFREE = TRUE;
	INSQUE(Hptr,Free_QBlks->QTail);
	QBlk_Count = QBlk_Count + 1;
	}
    else
	{
	NOINT;			// Disable AST's
!!!HACK!!// Why?
!	if (NOT (LIB$FREE_VM(%REF((qb_size+MEM$HDR_SIZE)*4),Hptr)))
	Pages = ((((qb_size + MEM$HDR_SIZE) * 4) / 512) + 1) ;
	if (NOT (LIB$FREE_VM_PAGE(Pages, Hptr)))
	    Memgr_Fault_Handler(0,R0,0);
	OKINT;
	QB_Gets = QB_gets - 1;		// track this event.
	};
    };
/*

Function:

	Preallocate blocks & queue them	on the free list "Free_QBlks".

Inputs:

	None.

Outputs:

	None.

Side Effects:

	Could die here if no avail memory.

*/

QBLK_Init: NOVALUE (void)
    {
    signed long
	struct MEM$HDR_STRUCT * Hptr,
	Pages,
	RC ;

    QBLK_Count = Qblk_count_base;
    INCR J FROM 1 TO QBlk_count DO
	{
!	LIB$GET_VM(%REF((qb_size+MEM$HDR_SIZE)*4),Hptr);
	Pages = ((((qb_size + MEM$HDR_SIZE) * 4) / 512) + 1) ;
	if (NOT (RC = (LIB$GET_VM_PAGE(Pages, Hptr))))
	    Memgr_Fault_Handler(0,RC,0);
	XLOG$FAO(LOG$MEM,"!%T MM$Qblk_Init !XL size !SL!/",0,Hptr, Pages);
	CH$FILL(%CHAR(0),MEM$HDR_SIZE*4,Hptr);
	Hptr->MEM$ISFREE = TRUE;
	Hptr->MEM$ISPERM = TRUE;
	INSQUE(Hptr,Free_QBlks->QTail);
	};
    };



//Sbttl "TCP User IO argument block (uarg) memory mangler"
/*

Function:

	Allocate one user argument block from process virtual space.
	User-requests-avail (maclib.mar) calls this routine to allocate
	a user arg block when copying TCP user request arguments from the
	system buffer to this buffer.  Called from user_requests_avail routine
	in MACLIB.mar in "KERNEL" mode.

Calling Sequence:

	CALLS  #0,Uarg_Get

Inputs:

	None.

Outputs:

	Address of process-local space user argument block.

Side Effects:

	Routine executes in Kernel mode.
	Uarg block could be removed from free list "FREE_UARGS".

*/


MM$UARG_GET=
    {
    BUILTIN
	R0;	// standard vax/vms routine return value register.
    signed long
	Ptr,
	Pages ;

    if (REMQUE(FREE_Uargs->QHead,Ptr) != Empty_Queue)
	Uarg_Count = Uarg_Count - 1
    else
	{
	NOINT;			// Disable interrupts, do allocation
!	if (NOT (LIB$GET_VM(%REF(Max_User_ArgBlk_Size*4),ptr)))
	Pages = (((Max_User_ArgBlk_Size * 4) / 512) + 1) ;
	if (NOT (LIB$GET_VM_PAGE(Pages, ptr)))
	    Memgr_Fault_Handler(0,R0,0);
	OKINT;
	XLOG$FAO(LOG$MEM,"!%T MM$Uarg_Get !XL size !SL!/",0,Ptr, Pages);
	UA_Gets = UA_gets + 1;
	if (UA_gets > UA_max)
	    UA_max = UA_gets;
	};
    CH$FILL(%CHAR(0),Max_User_ArgBlk_Size*4,Ptr);
    RETURN(Ptr);
    };

/*

Function:

	Deallocate one process-local user arg block.

Inputs:

	Ptr = address of block to deallocate.

Outputs:

	None.

Side Effects:

	None.

*/


MM$UARG_Free(Ptr): NOVALUE (void)
    {
    signed long
	queptr,
	Pages ;
    BUILTIN
	R0;	// standard vax/vms routine return value register.

    if (Uarg_Count LSS Uarg_Count_base)
	{
!!!HACK!!// can an exception right here cause 
!!!HACK!!// CPU 00 -- DOUBLDEALO, Double deallocation of memory block????
	INSQUE(Ptr,Free_Uargs->QTail);
	Uarg_Count = Uarg_Count + 1;
	}
    else
	{
	NOINT;
!	if (NOT (LIB$FREE_VM(%REF(Max_User_ArgBlk_Size*4),ptr)))
	Pages = (((Max_User_ArgBlk_Size * 4) / 512) + 1) ;
	if (NOT (LIB$FREE_VM_PAGE(Pages, ptr)))
	    Memgr_Fault_Handler(0,R0,0);
	OKINT;
	XLOG$FAO(LOG$MEM,"!%T MM$Uarg_Free !XL size !SL!/",0,Ptr, Pages);
	UA_Gets = UA_gets - 1;
	};
    };

/*

Function:

	Preallocate Uarg blocks & queue them on the free list "Free_Uargs".

Inputs:

	None.

Outputs:

	None.

Side Effects:

	Could die here if no avail memory.

*/

Uarg_Init: NOVALUE (void)
    {
    signed long
	Ptr,
	Pages,
	RC ;

    Uarg_count = Uarg_count_base;
    INCR J FROM 1 TO Uarg_count DO
	{
!	LIB$GET_VM(%REF(Max_User_ArgBlk_Size*4),ptr);
	Pages = (((Max_User_ArgBlk_Size * 4) / 512) + 1) ;
	if (NOT (RC = (LIB$GET_VM_PAGE(Pages, ptr))))
	    Memgr_Fault_Handler(0,RC,0);
	XLOG$FAO(LOG$MEM,"!%T MM$Uarg_Init !XL size !SL!/",0,Ptr, Pages);
	INSQUE(Ptr,Free_Uargs->QTail);
	};
    };

//SBTTL "Segment Handlers"
/*

Function:

	Allocate one segment of specified size.  Used to construct a TCP
	network segment.
	Allocation is always in BLISS Fullwords, size argument is rounded to
e	next bliss fullword.
	Routine is called from both user & AST modes of operation.  Because of
	this fact we cannot allow AST's to be delivered when in this routine.

Calling Conventions:

	CALLS, Default BLISS-32 linkage.

Inputs:

	Size = Segment size in bytes

Outputs:

	Address of segment

Side Effects:

	AST delivery is disabled for the duration of this routine.  This
	prevents the routine from being called & then called again from an
	AST service routine.
*/


MM$SEG_GET(Size)
    {
    BUILTIN
	R0;	// standard vax/vms routine return value register.
    signed long
	Alloc = False,	// no seg allocation yet.
	Ptr,
	Pages ;

// Respond to various size segments (ie, some may be preallocated.)

    SELECTONE Size OF
    SET

// Minimum-sized segment buffer (control segs & small segs)

    [MIN_PHYSICAL_BUFSIZE]:
	{
	if (REMQUE(Free_Minsize_Segs->QHead,Ptr) != Empty_Queue)
	    {
	    MIN_Seg_Count = MIN_Seg_Count - 1;
	    Alloc = True;
	    }
	else
	    {
	    MIN_gets = MIN_gets + 1;
	    if (MIN_gets gtr MIN_max)
		MIN_max = MIN_gets;
	    };
	};

// Max size segment - big segments to transmit & all receive buffers

    [MAX_PHYSICAL_BUFSIZE]:
	{
	if (REMQUE(Free_Maxsize_Segs->QHead,Ptr) != Empty_Queue)
	    {
	    Max_Seg_Count = Max_Seg_Count - 1;
	    Alloc = True;
	    }
	else
	    {
	    MAX_gets = MAX_gets + 1;
	    if (MAX_gets > MAX_max)
		MAX_max = MAX_gets;
	    };
	};
    TES;

// Did we Allocate a segment yet?  If not then better get one before we leave.

    if (NOT Alloc)
	{
	NOINT;
!	if (NOT (LIB$GET_VM(size,ptr)))
	Pages = ((Size / 512) + 1) ;
	if (NOT (LIB$GET_VM_PAGE(Pages, ptr)))
	    {
	    OKINT ;
	    Memgr_Fault_Handler(0,R0,0);
	    } ;
	OKINT;
	};

    XLOG$FAO(LOG$MEM,"!%T MM$Seg_Get !XL size !SL!/", 0,PTR, Size);
    RETURN(Ptr);
    };

/*

Function:

	Deallocate one Segment.
	Size is specified in bytes BUT it is assumed that the actual allocation
	was done in bliss fullwords (ala MM$Seg_Get rtn).

Inputs:

	Size = size of segment in bytes
	Ptr = address of segement.

Outputs:

	None.

Side Effects:

	AST delivery is disabled for the duration of this routine.
	Prevents user mode & AST service routines from stepping on each
	others toes.

*/


MM$SEG_FREE(Size,Ptr) : NOVALUE (void)
    {
    BUILTIN
	R0;	// standard vax/vms routine return value register.
    signed long
	Released = False,
	Pages ;

// Check if segment is of a preallocated size.

    XLOG$FAO(LOG$MEM,"!%T MM$Seg_Free !XL size !SL!/", 0,PTR, Size);
    SELECTONE Size OF
    SET

    [MIN_PHYSICAL_BUFSIZE]:
	{
	if (MIN_Seg_Count LSS MIN_Seg_Count_base)
	    {
	    INSQue(Ptr,Free_Minsize_Segs->QTail);
	    MIN_Seg_Count = MIN_Seg_Count + 1;
	    Released = True;
	    }
	else
	    MIN_gets = MIN_gets - 1;
	};

    [MAX_PHYSICAL_BUFSIZE]:
	{
	if (Max_Seg_Count LSS Max_Seg_Count_base)
	    {
	    INSQUE(Ptr,Free_Maxsize_Segs->QTail);
	    Max_Seg_Count = Max_Seg_Count + 1;
	    Released = True;
	    }
	else
	    MAX_gets = MAX_gets - 1;
	};
    TES;

// Did we actually release the segment?  If not then do so.

    if (NOT Released)
	{
	NOINT;
!	if (NOT (LIB$FREE_VM(size,ptr)))
	Pages = ((Size / 512) + 1) ;
	if (NOT (LIB$FREE_VM_PAGE(Pages, ptr)))
	    {
	    OKINT ;
	    Memgr_Fault_Handler(0,R0,0);
	    } ;
	OKINT;
	};
    };


/*

Function:

	Initialize the free control segment list.

Inputs:

	None.

Outputs:

	None.

Side Effects:

	Can die if no memory.

*/

SEG_INIT : NOVALUE (void)
    {
    BUILTIN
	R0;	// standard vax/vms routine return value register.
    signed long
	Ptr,
	RC,
	Pages ;

// Allocate minimum (default) size segments.

    MIN_seg_count = MIN_seg_count_base;
    INCR J FROM 0 TO MIN_Seg_count-1 DO
	{
!	LIB$GET_VM(MIN_PHYSICAL_BUFSIZE,ptr);
	Pages = ((MIN_PHYSICAL_BUFSIZE / 512) + 1) ;
	if (NOT (RC = LIB$GET_VM_PAGE(Pages, ptr)))
	    Memgr_Fault_Handler(0,RC,0);
	XLOG$FAO(LOG$MEM,"!%T MM$Seg_Init !XL size !SL!/",0,Ptr, Pages);
	INSQUE(Ptr,Free_Minsize_Segs->QTail);
	};

// Allocate maximum size segments

    MAX_seg_count = MAX_seg_count_base;
    DECR J FROM MAX_Seg_count TO 1 DO
	{
!	LIB$GET_VM(MAX_PHYSICAL_BUFSIZE,ptr);
	Pages = ((MAX_PHYSICAL_BUFSIZE / 512) + 1) ;
	if (NOT (RC = LIB$GET_VM_PAGE(Pages, ptr)))
	    Memgr_Fault_Handler(0,RC,0);
	XLOG$FAO(LOG$MEM,"!%T MM$Seg_Init !XL size !SL!/",0,Ptr, Pages);
	INSQUE(Ptr,Free_Maxsize_Segs->QTail);
	};
    };

//Sbttl "Memory Management Initialization."
/*

Function:

	Initialize various aspects of dynamic memory preallocation.

Inputs:

	None.

Outputs:

	None.

Side Effects:

	Specific dynamically allocated data structures are preallocated.

*/


MM$INIT : NOVALUE (void)
    {
    QBLK_Init();
    Uarg_Init();
    Seg_Init();
    };


//SBTTL "Debugging routines to simulate INSQUE and REMQUE"
/*

The routines MEM_INSQUE and MEM_REMQUE are called by the XINSQUE and XREMQUE
macros when the QDEBUG flag has been turned on.

**N.B.** This routines will currently only work for QBLKS since they are the
only dynamic memory objects that have our special debugging header attached to
them. Use of XINSQUE or XREMQUE with other dynamic memory objects will cause
memory to be trashed.
*/

//IF QDEBUG %THEN

MM$INSQUE(QBLK,QHDR,QRTN,QID,QVAL)
    {
    signed long
	struct MEM$HDR_STRUCT * HPTR;
    HPTR = QBLK - MEM$HDR_SIZE*4;
    HPTR->MEM$INSQUERTN = QRTN;
    HPTR->MEM$INSQUEHDR = QHDR;
    HPTR->MEM$INSQUEVAL = QVAL;
    HPTR->MEM$CURQUEUES = HPTR->MEM$CURQUEUES || QID;
    HPTR->MEM$ALLQUEUES = HPTR->MEM$ALLQUEUES || QID;
    RETURN INSQUE(QBLK,QHDR);
    };

MM$REMQUE(QHDR,QBLK,QRTN,QID,QVAL)
    {
    signed long
	struct MEM$HDR_STRUCT * HPTR,
	RVAL;
    if ((RVAL = REMQUE(QHDR,QBLK)) != Empty_Queue)
	{
	HPTR = ..QBLK - MEM$HDR_SIZE*4;
	HPTR->MEM$REMQUERTN = QRTN;
	HPTR->MEM$REMQUEHDR = QHDR;
	HPTR->MEM$REMQUEVAL = QVAL;
	HPTR->MEM$CURQUEUES = HPTR->MEM$CURQUEUES && (NOT QID);
	};
    RETURN RVAL;
    };
//FI

}
ELUDOM
