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

#if 0
MODULE MEMGR(Ident="2.8",LANGUAGE(BLISS32),
	     ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			     NONEXTERNAL=LONG_RELATIVE),
	     LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	     OPTIMIZE,OPTLEVEL=3,ZIP)=
#endif

#include <starlet.h>	// Include VMS systems definitions
  // not yet#include "CMUIP_SRC:[CENTRAL]NETXPORT";	// Include transportablity package
#include "netvms.h"			// VMS specifics
#include "tcp.h"			// TCP related definitions
#include "tcpmacros.h"		// Local Macros
#include "structure.h"		// Structure & Field definitions

#include <ssdef.h>
#include "nettcpip.h" // for some min max defines

//SBTTL "External: Routines, Literals and data"

extern    LIB$GET_VM();
extern    LIB$FREE_VM();
extern    LIB$GET_VM_PAGE();
extern    LIB$FREE_VM_PAGE();
//    TCP$Find_Local_Port,	// USER.BLI
extern void    TELNET_CLOSE();
extern    MovByt();

// External Data Segements

extern signed long
//    struct Connection_Table_Structure * ConectPtr,
//    struct VECTOR * VTCB_ptr[0],	// Vector of Valid TCB pointers.
//    VTCB_Size,
//    Max_TCB,
    ast_in_progress,		// Flag if running at AST level
    INTDF;			// interrupt deferral count

  /* making doubles */
#if 0
    MIN_PHYSICAL_BUFSIZE,
    MAX_PHYSICAL_BUFSIZE;
#endif
#define    MAX_PHYSICAL_BUFSIZE (DEVICE_HEADER+TCP_HEADER_SIZE+IP_HDR_BYTE_SIZE+OPT$MAX_RECV_DATASIZE)
#define    MIN_PHYSICAL_BUFSIZE (DEVICE_HEADER+TCP_HEADER_SIZE+IP_HDR_BYTE_SIZE+DEFAULT_DATA_SIZE)


//SBTTL "Memory manager header structure"

struct MEM$HDR_STRUCT
{
void *   MEM$QNEXT	;	// Next item on queue
void *   MEM$QPREV	;	// Previous item on queue
#ifdef QDEBUG
unsigned long   MEM$CURQUEUES	;	// Queues this block is on
unsigned long   MEM$ALLQUEUES ;	// Queues this block has been on
void *   MEM$ALLOCRTN	;	// Routine which allocated most recently
void *   MEM$FREERTN	;	// Routine which freed most recently
void *   MEM$INSQUERTN	;	// Routine which INSQUE'd most recently
void *   MEM$INSQUEHDR	;	// Queue header where most recently INSQUE'd
void *   MEM$INSQUEVAL	;	// Additional value when INSQUE'd
void *   MEM$REMQUERTN	;	// Routine which REMQUE'd most recently
void *   MEM$REMQUEHDR	;	// Queue header where most recently REMQUE'd
void *   MEM$REMQUEVAL	;	// Additional value when REMQUE'd
#endif
  union {
unsigned long   MEM$FLAGS	;	// Flags defining free, prealloc, etc.
    struct {

 unsigned     MEM$ISFREE :1 ;	// This block is free
unsigned      MEM$ISPERM :1 ; // This block is permanent (preallocated)
    };
  };
};

#define MEM$HDR_SIZE sizeof(struct  MEM$HDR_STRUCT)
#if 0
LITERAL
    MEM$HDR_SIZE = $FIELD_SET_SIZE;
MACRO
    MEM$HDR_STRUCT = BLOCK->MEM$HDR_SIZE FIELD(MEM$HDR_FIELDS) %;
#endif

//Sbttl "Preallocated dynamic data structures."
/*

Here we define memory management preallocated data structure queues.
Idea is to keep specified number of dynamic data structures around so we
don't have to dynamically allocate the structure each time we need one.
A little faster execution is what we are after.
*/

// Define and initialize free lists to "empty".

static struct queue_header_structure(queue_header_fields) FREE_Qblks = 
  {
			qhead : &FREE_Qblks,
			qtail : &FREE_Qblks
  };
static struct queue_header_structure(queue_header_fields) USED_Qblks =
  {
			qhead : &USED_Qblks,
			qtail : &USED_Qblks
  };

static struct queue_header_structure(queue_header_fields) FREE_Uargs =
  {
			qhead : &FREE_Uargs,
			qtail : &FREE_Uargs
  };

static struct queue_header_structure(queue_header_fields) Free_Minsize_Segs =
  {
				qhead : &Free_Minsize_Segs,
				qtail : &Free_Minsize_Segs
  };

static struct queue_header_structure(queue_header_fields) Free_Maxsize_Segs =
  {
				qhead : &Free_Maxsize_Segs,
				qtail : &Free_Maxsize_Segs
  };

// "count_base" items are initialized in conifg_acp routine during startup.
// they provide the base number of items on a free list.

unsigned char
    qblk_count_base  = 0,	// #Qblks on free list.
    uarg_count_base  = 0,	// #Uarg blks on free list.
    min_seg_count_base = 0, // #Min-size segs
    max_seg_count_base = 0, // #Max-size segs

// Counters for the number of items on a specific queue.

    qblk_count = 0,	// #Qblks on free list.
    uarg_count = 0,	// #Uarg blks on free list.
    min_seg_count = 0,	// #Min-size segs
    max_seg_count = 0;	// #Max-size segs

/*
Here we keep track of what kind of data structures are being dynamically 
allocated & which should have more reserved during initialization.
*/

signed long
    qb_gets  = 0,	// Queue Blks dynamically allocated
    ua_gets  = 0,	// User arg blks.
    min_gets  = 0,	// Minimum size buffers
    max_gets  = 0,	// Maximum size buffers
    qb_max  = 0,	// Queue Blk max queue size
    ua_max  = 0,	// User arg max queue size
    min_max  = 0,	// Minimum size buffers max queue size
    max_max  = 0;	// Maximum seg max queue size

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


void Memgr_Fault_Handler(Caller,Primary_CC,Sec_CC)
    {
    Signal(Primary_CC);
    Fatal_Error("Memory Mgmt. Fault detected.",Primary_CC);
    }



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

mm$get_mem (Addr, Size)
    {
    signed long
	Pages,
	RC;

    Pages = Size / 512 ;
    Pages = Pages + 1 ;
    NOINT;			// Hold AST's please...
//    if (! (RC = LIB$GET_VM(Size,Addr)))
    if (! (RC = LIB$GET_VM_PAGE(Pages,&Addr)))
	Memgr_Fault_Handler(0,RC,0);
    OKINT;

	XLOG$FAO(LOG$MEM,"!%T MM$Get_Mem !XL size !SL!/",0,Addr, Size);

    CH$FILL(0,Size,Addr);	// clean house.....zero fill.

    return RC;
    }



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

void mm$free_mem(Mem,Size)
    {
    signed long
	Pages,
	RC;

    Pages = Size / 512 ;
    Pages = Pages + 1 ;

    XLOG$FAO(LOG$MEM,"!%T MM$Free_Mem !XL size !SL!/",0,Mem, Size);

    NOINT;			// Hold AST's
//    if (! ( RC = LIB$FREE_VM ( Size , Mem ) ))
    if (! ( RC = LIB$FREE_VM_PAGE ( Pages , Mem ) ))
	    Memgr_Fault_Handler(0,RC,0);
    OKINT;
    }



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

void mm$qblk_get (void)
    {
      long R0=0;
#if 0
      // check
    BUILTIN
	R0;	// standard vax/vms routine return value register.
#endif
    struct MEM$HDR_STRUCT * Hptr;
    signed long
	ptr,
    Pages =0;

    if (REMQUE(&FREE_Qblks.qhead,Hptr) != EMPTY_QUEUE) // check
	qblk_count = qblk_count - 1; // Say there is 1 less avail.
    else			// allocate a new qb.
	{

// Disallow ast during memory allocation.

	NOINT;

//	if (! (LIB$GET_VM(%REF((qb_size+MEM$HDR_SIZE)*4),Hptr)))
	Pages = ((((qb_size + MEM$HDR_SIZE) * 4) / 512) + 1 ) ;
	if (! (LIB$GET_VM_PAGE(Pages, &Hptr)))
	    Memgr_Fault_Handler(0,R0,0);

	OKINT;
	CH$FILL(/*%CHAR*/(0),MEM$HDR_SIZE*4,Hptr);
	Hptr->MEM$ISPERM = FALSE; // Not a permanent qblk
	qb_gets = qb_gets + 1;	// track this event.
	if (qb_gets > qb_max)
	    qb_max = qb_gets;
	};

    ptr = Hptr + MEM$HDR_SIZE*4; // Point at data area
    CH$FILL(/*%CHAR*/(0),qb_size*4,ptr);	// fresh qb.
//!!HACK!!!~~~ Should record allocator here ~~~
    XLOG$FAO(LOG$MEM,"!%T MM$Qblk_Get !XL size !SL!/",0,Hptr, Pages);
    Hptr->MEM$ISFREE = FALSE;	// QB is no longer free
    INSQUE(Hptr,&USED_Qblks.qtail); // Insert on used queue
    return(ptr);
    }

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

void mm$qblk_free(Ptr)
    {
      long R0=0;
#if 0
      // check
    BUILTIN
	R0;	// standard vax/vms routine return value register.
#endif
    struct MEM$HDR_STRUCT * Hptr;
    signed long
	Pages	 = 0;

    Hptr = Ptr - MEM$HDR_SIZE*4; // Point at header
    XLOG$FAO(LOG$MEM,"!%T MM$Qblk_Free !XL size !SL!/",0,Hptr, Pages);
    NOINT;
    REMQUE(Hptr,Hptr);		// Remove from the used queue
    OKINT;
    if (Hptr->MEM$ISPERM)
	{			// Free a permanent block - just put on free Q
//~~~ Record deallocator here ~~~
	Hptr->MEM$ISFREE = TRUE;
	INSQUE(Hptr,&FREE_Qblks.qtail);
	qblk_count = qblk_count + 1;
	}
    else
	{
	NOINT;			// Disable AST's
//!!HACK!!// Why?
//	if (! (LIB$FREE_VM(%REF((qb_size+MEM$HDR_SIZE)*4),Hptr)))
	Pages = ((((qb_size + MEM$HDR_SIZE) * 4) / 512) + 1) ;
	if (! (LIB$FREE_VM_PAGE(Pages, Hptr)))
	    Memgr_Fault_Handler(0,R0,0);
	OKINT;
	qb_gets = qb_gets - 1;		// track this event.
	};
    }
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

void QBLK_Init (void)
    {
      struct MEM$HDR_STRUCT * Hptr;
      signed long J,
	Pages,
	RC ;

    qblk_count = qblk_count_base;
    for (J=1;J<=qblk_count;J++)
	{
//	LIB$GET_VM(%REF((qb_size+MEM$HDR_SIZE)*4),Hptr);
	Pages = ((((qb_size + MEM$HDR_SIZE) * 4) / 512) + 1) ;
	if (! (RC = (LIB$GET_VM_PAGE(Pages, &Hptr))))
	    Memgr_Fault_Handler(0,RC,0);
	XLOG$FAO(LOG$MEM,"!%T MM$Qblk_Init !XL size !SL!/",0,Hptr, Pages);
	CH$FILL(/*%CHAR*/(0),MEM$HDR_SIZE*4,Hptr);
	Hptr->MEM$ISFREE = TRUE;
	Hptr->MEM$ISPERM = TRUE;
	INSQUE(Hptr,&FREE_Qblks.qtail);
	};
    }



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


void mm$uarg_get (void)
    {
      long R0 = 0;
#if 0
      // check
    BUILTIN
	R0;	// standard vax/vms routine return value register.
#endif
    signed long
	Ptr,
	Pages ;

    if (REMQUE(&FREE_Uargs.qhead,Ptr) != EMPTY_QUEUE) // check
      uarg_count = uarg_count - 1;
    else
	{
	NOINT;			// Disable interrupts, do allocation
//	if (! (LIB$GET_VM(%REF(Max_User_ArgBlk_Size*4),Ptr)))
	Pages = (((Max_User_ArgBlk_Size * 4) / 512) + 1) ;
	if (! (LIB$GET_VM_PAGE(Pages, &Ptr)))
	    Memgr_Fault_Handler(0,R0,0);
	OKINT;
	XLOG$FAO(LOG$MEM,"!%T MM$Uarg_Get !XL size !SL!/",0,Ptr, Pages);
	ua_gets = ua_gets + 1;
	if (ua_gets > ua_max)
	    ua_max = ua_gets;
	};
    CH$FILL(/*%CHAR*/(0),Max_User_ArgBlk_Size*4,Ptr);
    return(Ptr);
    }

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


void mm$uarg_free(Ptr)
    {
      long R0=0;
    signed long
	queptr,
	Pages ;
#if 0
    BUILTIN
	R0;	// standard vax/vms routine return value register.
#endif

    if (uarg_count < uarg_count_base)
	{
//!!HACK!!// can an exception right here cause 
//!!HACK!!// CPU 00 -- DOUBLDEALO, Double deallocation of memory block????
	INSQUE(Ptr,&FREE_Uargs.qtail);
	uarg_count = uarg_count + 1;
	}
    else
	{
	NOINT;
//	if (! (LIB$FREE_VM(%REF(Max_User_ArgBlk_Size*4),ptr)))
	Pages = (((Max_User_ArgBlk_Size * 4) / 512) + 1) ;
	if (! (LIB$FREE_VM_PAGE(Pages, Ptr)))
	    Memgr_Fault_Handler(0,R0,0);
	OKINT;
	XLOG$FAO(LOG$MEM,"!%T MM$Uarg_Free !XL size !SL!/",0,Ptr, Pages);
	ua_gets = ua_gets - 1;
	};
    }

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

void uarg_init (void)
    {
      signed long J,
	Ptr,
	Pages,
	RC ;

    uarg_count = uarg_count_base;
    for (J=1;J<=uarg_count;J++)
	{
//	LIB$GET_VM(%REF(Max_User_ArgBlk_Size*4),Ptr);
	Pages = (((Max_User_ArgBlk_Size * 4) / 512) + 1) ;
	if (! (RC = (LIB$GET_VM_PAGE(Pages, &Ptr))))
	    Memgr_Fault_Handler(0,RC,0);
	XLOG$FAO(LOG$MEM,"!%T MM$Uarg_Init !XL size !SL!/",0,Ptr, Pages);
	INSQUE(Ptr,&FREE_Uargs.qtail);
	};
    }

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


mm$seg_get(Size)
    {
      long R0=0;
#if 0
      // check
    BUILTIN
	R0;	// standard vax/vms routine return value register.
#endif
    signed long
	Alloc = FALSE,	// no seg allocation yet.
	Ptr,
	Pages ;

// Respond to various size segments (ie, some may be preallocated.)

    switch (Size)
      {

// Minimum-sized segment buffer (control segs & small segs)

    case MIN_PHYSICAL_BUFSIZE:
	{
	  if (REMQUE(&Free_Minsize_Segs.qhead,Ptr) != EMPTY_QUEUE) // check
	    {
	    min_seg_count = min_seg_count - 1;
	    Alloc = TRUE;
	    }
	else
	    {
	    min_gets = min_gets + 1;
	    if (min_gets > min_max)
		min_max = min_gets;
	    };
	};
	break;

// Max size segment - big segments to transmit & all receive buffers

    case MAX_PHYSICAL_BUFSIZE:
	{
	  if (REMQUE(&Free_Maxsize_Segs.qhead,Ptr) != EMPTY_QUEUE) // check
	    {
	    max_seg_count = max_seg_count - 1;
	    Alloc = TRUE;
	    }
	else
	    {
	    max_gets = max_gets + 1;
	    if (max_gets > max_max)
		max_max = max_gets;
	    };
	};
    };

// Did we Allocate a segment yet?  If not then better get one before we leave.

    if (! Alloc)
	{
	NOINT;
//	if (! (LIB$GET_VM(size,Ptr)))
	Pages = ((Size / 512) + 1) ;
	if (! (LIB$GET_VM_PAGE(Pages, &Ptr)))
	    {
	    OKINT ;
	    Memgr_Fault_Handler(0,R0,0);
	    } ;
	OKINT;
	};

    XLOG$FAO(LOG$MEM,"!%T MM$Seg_Get !XL size !SL!/", 0,Ptr, Size);
    return(Ptr);
    }

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


void mm$seg_free(Size,Ptr)
    {
      long R0=0;
#if 0
    BUILTIN
	R0;	// standard vax/vms routine return value register.
#endif
    signed long
	Released = FALSE,
	Pages ;

// Check if segment is of a preallocated size.

    XLOG$FAO(LOG$MEM,"!%T MM$Seg_Free !XL size !SL!/", 0,Ptr, Size);
    switch (Size)
      {

    case MIN_PHYSICAL_BUFSIZE:
	{
	if (min_seg_count < min_seg_count_base)
	    {
	    INSQue(Ptr,&Free_Minsize_Segs.qtail);
	    min_seg_count = min_seg_count + 1;
	    Released = TRUE;
	    }
	else
	    min_gets = min_gets - 1;
	};
	break;

    case MAX_PHYSICAL_BUFSIZE:
	{
	if (max_seg_count < max_seg_count_base)
	    {
	    INSQUE(Ptr,&Free_Maxsize_Segs.qtail);
	    max_seg_count = max_seg_count + 1;
	    Released = TRUE;
	    }
	else
	    max_gets = max_gets - 1;
	};
    };

// Did we actually release the segment?  If not then do so.

    if (! Released)
	{
	NOINT;
//	if (! (LIB$FREE_VM(size,ptr)))
	Pages = ((Size / 512) + 1) ;
	if (! (LIB$FREE_VM_PAGE(Pages, Ptr)))
	    {
	    OKINT ;
	    Memgr_Fault_Handler(0,R0,0);
	    } ;
	OKINT;
	};
    }


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

void seg_init (void)
    {
#if 0
    BUILTIN
	R0;	// standard vax/vms routine return value register.
#endif
    signed long J,
	Ptr,
	RC,
	Pages ;

// Allocate minimum (default) size segments.

    min_seg_count = min_seg_count_base;
    for (J=0;J<=min_seg_count-1;J++)
	{
//	LIB$GET_VM(MIN_PHYSICAL_BUFSIZE,Ptr);
	Pages = ((MIN_PHYSICAL_BUFSIZE / 512) + 1) ;
	if (! (RC = LIB$GET_VM_PAGE(Pages, &Ptr)))
	    Memgr_Fault_Handler(0,RC,0);
	XLOG$FAO(LOG$MEM,"!%T MM$Seg_Init !XL size !SL!/",0,Ptr, Pages);
	INSQUE(Ptr,&Free_Minsize_Segs.qtail);
	};

// Allocate maximum size segments

    max_seg_count = max_seg_count_base;
    for (J=max_seg_count;J>=1;J--)
	{
//	LIB$GET_VM(MAX_PHYSICAL_BUFSIZE,ptr);
	Pages = ((MAX_PHYSICAL_BUFSIZE / 512) + 1) ;
	if (! (RC = LIB$GET_VM_PAGE(Pages, &Ptr)))
	    Memgr_Fault_Handler(0,RC,0);
	XLOG$FAO(LOG$MEM,"!%T MM$Seg_Init !XL size !SL!/",0,Ptr, Pages);
	INSQUE(Ptr,&Free_Maxsize_Segs.qtail);
	};
    }

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


void mm$init (void)
    {
    QBLK_Init();
    uarg_init();
    seg_init();
    }


//SBTTL "Debugging routines to simulate INSQUE and REMQUE"
/*

The routines MEM_INSQUE and MEM_REMQUE are called by the XINSQUE and XREMQUE
macros when the QDEBUG flag has been turned on.

**N.B.** This routines will currently only work for QBLKS since they are the
only dynamic memory objects that have our special debugging header attached to
them. Use of XINSQUE or XREMQUE with other dynamic memory objects will cause
memory to be trashed.
*/

#ifdef QDEBUG

mm$insque(QBLK,QHDR,QRTN,QID,QVAL)
    {
      struct MEM$HDR_STRUCT * HPTR;
    HPTR = QBLK - MEM$HDR_SIZE*4;
    HPTR->MEM$INSQUERTN = QRTN;
    HPTR->MEM$INSQUEHDR = QHDR;
    HPTR->MEM$INSQUEVAL = QVAL;
    HPTR->MEM$CURQUEUES = HPTR->MEM$CURQUEUES || QID;
    HPTR->MEM$ALLQUEUES = HPTR->MEM$ALLQUEUES || QID;
    return INSQUE(QBLK,QHDR);
    }

mm$remque(QHDR,QBLK,QRTN,QID,QVAL)
     signed long * QBLK;
    {
      struct MEM$HDR_STRUCT * HPTR;
    signed long
	RVAL;
    if ((RVAL = REMQUE(QHDR,QBLK)) != EMPTY_QUEUE) // check
	{
	HPTR = *QBLK - MEM$HDR_SIZE*4;
	HPTR->MEM$REMQUERTN = QRTN;
	HPTR->MEM$REMQUEHDR = QHDR;
	HPTR->MEM$REMQUEVAL = QVAL;
	HPTR->MEM$CURQUEUES = HPTR->MEM$CURQUEUES && (! QID);
	};
    return RVAL;
    }
#endif


