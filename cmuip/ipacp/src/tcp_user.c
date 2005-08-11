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
//TITLE "TCP_USER - TCP user interface routines"
//SBTTL "TCP user request overview"
/*

Module:

	TCP_USER

Facility:

	Process user TCP I/O requests.

Abstract:

	This module is called by the main user IRP processing loop in the
	USER.BLI module to handle all TCP requests.

Author:

	(originally extracted from USER.BLI)
	This version by Vince Fuller, CMU-CSD, Summer/Fall, 1986
	Copyright (c) 1986,1987, Vince Fuller and Carnegie-Mellon University


Modification History:

3.7	14-Nov-1991	Henry W. Miller		USBR
	Use TCB->SND_ACK_THRESHOLD instead of ACK_THRESHOLD.
	In TCP$TCB_Init(), init TCB->SND_ACK_THRESHOLD from ACK_THRESHOLD.

3.6	14-Nov-1991	Henry W. Miller		USBR
	Change timer arithmetic from signed to unsigned.
	Call TCP$Enqueue_ACK() rather than TCP$Send_ACK().

3.5e	7-Aug-1991	Henry W. Miller		USBR
	In TCB$TCB_DUMP(), set up retransmit timeout variable properly.

3.5d	13-Mar-1991	Henry W. Miller		USBR
	Fix port comparison logic.

3.5c	25-Jan-1991	Henry W. Miller		USBR
	Make WINDOW_DEFAULT and ACK_THRESHOLD configurable variables.

3.5b	24-Jan-1991	Henry W. Miller		USBR
	In TCP$TCB_Init(), initialize ACK_SIZE to zero.

3.5a	15-Jan-1991	Henry W. Miller		USBR
	Changed SND_WND, RCV_WND, Old_RCV_WND and RCV_PPTR to unsigned long
	values as recommended by RFC1122.
	Updated IDENT.

3.5  18-Aug-89, Edit by BRM (CMU NetDev)
	Changed all occurances of the hardwired Conect table and the literal
	Conect_Table_Size to use instead a reference to the table
	(ConectPtr) and a variable table size (ConectSize).  I did this
	in order to facilitate the move to completely dynamic IPACP
	structures.
	
3.4  18-Nov-87, Edit by VAF
	Know about SEND_SYN (SEND_CTL) returning routing failure.

3.3   8-Sep-87, Edit by VAF
	Be sure to set current user function in TCB when doing name lookup in
	TCP$OPEN.

3.2  30-Jul-87, Edit by VAF
	Start using $$KCALL macro to call $CMKRNL routines.
	Set "no update" flag for retransmit queue.

3.1  24-Jul-87, Edit by VAF
	Modify TCP$TCB_Init to not deal with user arguments, only initialize the
	"common" parts of the TCB (so TVT TCB's can use it).
	Make TCP$Conect_Insert and FIND_FREE_CONECT_ENTRY global for TVT code's
	use (they should be moved into TCP_MAIN module).

3.0  20-Jul-87, Edit by VAF
	Clean up handling of circular queues a bit.

2.9  10-Jun-87, Edit by VAF
	In NET$OPEN, call USER$CHECK_ACCESS earlier, particularly, before doing
	anything to the CONECT table.

2.8   3-Mar-87, Edit by VAF
	Flush the I/O request tag crud. Change format of IOSB and change
	calling sequence of user$post_io_status.

2.7  27-Feb-87, Edit by VAF
    	Know about FIN_RCVD flag (FIN-WAIT-2 and FIN received) flag when
	delivering pending user data.

2.6  19-Feb-87, Edit by VAF
	Use new name lookup routines.

2.5  18-Feb-87, Edit by VAF
	Move UCB extension definitions to USER.BLI.
	Add support for user open with IP address instead of host name.

2.4  12-Feb-87, Edit by VAF
	Modifications to support domain service for name/address lookups.

2.3   5-Feb-87, Edit by VAF
	Rewrite TCP$OPEN to add general network access check facility and to
	clean the code up a bit.

2.2  14-Nov-86, Edit by VAF
	Set calculated_RTO field in TCB.

2.1   1-Oct-86, Edit by VAF
	In TCP$Adlook_Done, make sure the TCB hasn't gone away.

2.0  30-Sep-86, Edit by VAF
	In CANCEL processing, only purge user request queue - don't clean out
	send and retransmit internal queues. This involves splitting part of
	TCP$KILL_PENDING_REQUESTS off into a new routine, PURGE_USER_REQUESTS,
	which does only this.

1.9  12-Sep-86, Edit by VAF
	In second part of open (TCP_NMLOOK_DONE), don't set host numbers if
	the foreign host was unspecified.
	Also, call Check_Syn_Wait_List at end of passive open.

1.8  14-Aug-86, Edit by VAF
	Fix problem with passive open.
	Make things that check TCB->NMLook_Flag and TCB->ARGBLK be NOINT.
	Make TCP$CANCEL use TCP$KILL_PENDING_REQUESTS.

1.7  13-Aug-86, Edit by VAF
	Move TCB dumping routines in here.

1.6  11-Aug-86, Edit by VAF
	Make TCP$OPEN use green protocol name-lookup function.

1.5   6-Aug-86, Edit by VAF
	Add circular queue buffering for send side.

1.4   6-Aug-86, Edit by VAF
	Allow TCP$RECEIVE to succede in CLOSE_WAIT state if data available.

1.3   5-Aug-86, Edit by VAF
	Modify receive side processing to use circular byte queue.

1.2  31-Jul-86, Edit by VAF
	Remove meat of TCP$INFO routine into USER$Net_Connection_Info routine,
	used by both TCP and UDP.

1.1  29-Jul-86, Edit by VAF
	Move some routines that UDP wants to use back into USER.BLI (in
	particular, SET_HOSTS and USER$GET_LOCAL_PORT)

1.0  23-Jul-86, Edit by VAF
	Split this module off from USER.BLI.
	Change routine names from NET$... to TCP$...
*/


//SBTTL "Module definition"

#if 0
MODULE TCP_USER(IDENT="3.7",LANGUAGE(BLISS32),
		ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
				NONEXTERNAL=LONG_RELATIVE),
		LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
		OPTIMIZE,OPTLEVEL=3,ZIP)
#endif

#include <starlet.h>	// VMS system definitions
// not yet #include <cmuip/central/include/netxport.h>	// XPORT data structure definitions
#include <cmuip/central/include/neterror.h>	// Network error message definitions
#include "netvms.h"	// Special VMS definitions
#include <cmuip/central/include/netcommon.h>// Network common defs
#include <cmuip/central/include/nettcpip.h>	// TCP/IP protocols
#include "structure.h"		// TCP system data structure definitions
#include "tcpmacros.h"		// System macro definitions
#include "tcp.h"			// TCP related definitions

#include <ssdef.h>
#include <descrip.h>
#include <ucbdef.h>

#if 0
XQDEFINE			// (maybe) define queue debugging externals
#endif

//*** Special literals from USER.BLI ***

#define    UCB$L_CBID	ucb$l_devdepend	// Control Block associated with UCB

extern signed long /* LITERAL*/
    UCB$Q_DDP,
    UCB$L_EXTRA;

// User function codes

#define     U$OPEN	  1
#define     U$CLOSE	  4
#define     M$CANCEL	  14

extern signed long
    default_mss,
    ack_threshold,
    window_default,
    AST_IN_PROGRESS,
    INTDF,
    TCP_User_LP,
    log_state,
    ts$dbr,
    ts$aco,
    ts$pco;
extern unsigned long   CONN_TIMEVAL;
extern    unsigned long * vtcb_ptr;

// Rtns from TCP_MECH.BLI

extern     tcb_ok();
extern     VTCB_Indx_OK();
extern     VTCB_Scan();
extern     CONECT_INSERT();
extern     check_unique_conn();
extern     tcb$create();
extern     void tcb$delete();

// Rtns from MEMGR.BLI

extern     mm$seg_get();
extern     void mm$seg_free();
extern     mm$qblk_get();
extern     void mm$qblk_free();
extern     void mm$uarg_free();

// IOUTIL.BLI

extern     GET_IP_ADDR();
extern     LOG_CHANGE();
extern  void    LOG_OUTPUT();
extern  void    LOG_TIME_STAMP();
extern  void    LOG_FAO();

// SEGIN.BLI

extern     void SEG$Check_SYN_Wait_List();
extern     void SEG$Purge_RT_Queue();

// IP.BLI

extern  void    IP$SET_HOSTS();

// Routines From:  MACLIB.MAR

extern     void set_ip_device_offline();
extern     User_Requests_Avail();
extern     void MOVBYT();
extern     void swapbytes();
extern     Time_Stamp();

// TCP.BLI

extern     tcp$send_ctl();
extern  void    tcp$enqueue_ack();
extern  void    tcp$send_data();
extern  void    tcp$set_tcb_state();
extern  void    tcp$inactivate_tcb();
extern     tcp$tcb_close();
extern  void    cq_dequeue();

// USER.BLI

extern  void    IO$POST();
extern  void    user$post_io_status();
extern  void    user$post_function_ok();
extern     USER$Err();
extern     user$check_access();
extern     user$get_local_port();

    void tcp$kill_pending_requests();

//SBTTL "User_OK - Verify a user has Privileges to use Connection"

/*
Verify a user has privs to use the specified connection.  Check is made
aginst the User_ID in the TCB & the user_id specified in the call args.

Entry:	Args = Default user args structure (function & user_ID).
	TCB = points at TCB for this connection

Returns:
	True = User has priv's to use connection.
	False - No priv's.

Deficiencies:

	Need to design a more secure/comprehensive check
*/

USER_OK(struct user_default_args * args,struct tcb_structure * tcb)
    {
    if (tcb->user_id == args->ud$pid)
      return(TRUE);
    else
	return(FALSE);
    }

//SBTTL "Purge User Send Request Queue"
/*

Function:

	Kill any pending user sends, Return the user's IO request status & 
	delete the send queue element.

Inputs:

	TCB = address of TCB
	ERR = User error code.

Outputs:

	None.

Side Effects:

	User send request queue is purged.  Send requests are returned back
	to the user with the appro. error code.

*/

void tcp$purge_send_queue(struct tcb_structure * TCB,signed long RC)
    {
    register
      struct queue_blk_structure(qb_send_fields) * QB;
register	struct user_send_args * Uargs;

 while (REMQUE(TCB->snd_qhead,&QB) != EMPTY_QUEUE) // check
	{
	Uargs = QB->sn$uargs;	// point at user argblk.
	user$post_io_status(Uargs,RC,0,0,0);
	mm$uarg_free(Uargs);	// release user arg blk.
	mm$qblk_free(QB);		// release queue block
	};
    }

//Sbttl "Purge-User-Receive-Queue: Dump user network IO requests."
/*

Function:

	Purge the user network receive request queue.  Something has happened
	which forces ALL receive requests to be aborted.  VMS return code
	is SS$_ABORT.  TCP Error code varies.

Inputs:

	TCB = TCB pointer.
	Err = TCP Error code which is returned to the user.

Implicit Inputs:

	TCB->UR_Qhead - User receive requests for this connection.
	TCB->UR_Qtail - Both fields comprize the queue header.

Outputs:

	None.

Side Effects:

	This TCB's user receive request queue is emptied.
	User argument blocks are released.

*/

void tcp$purge_receive_queue(struct tcb_structure * TCB, signed long RC)
    {
    register
	struct queue_blk_structure(qb_ur_fields) * QB;

    while (REMQUE(TCB->ur_qhead,&QB) != EMPTY_QUEUE) // check
	{
	user$post_io_status(QB->ur$uargs,RC,0,0,0);
	mm$uarg_free(QB->ur$uargs); // release user arg block.
	mm$qblk_free(QB);
	};
    }

//SBTTL "Purge/Post all I/O requests for all TCBs."
/*

Function:

	Get rid of all TCB queue items for all TCBs. Called just before the
	ACP exits by USER$Purge_All_IO in the USER module.

Side Effects:

	All TCB's are scanned searching for user IO requests to POST.  When
	a particular TCB has been examined then we delete it.
*/

Purge_TCB ( TCB , IDX , P1 , P2 )
    {

//!!HACK!!// ADT violation
    vtcb_ptr[IDX] = 0; // invalidate tcb.
    tcp$kill_pending_requests(TCB,NET$_TE);
    return 1;
    }

//SBTTL "Purge/Post all I/O requests for all TCBs."
/*

Function:

	Get rid of all TCB queue items for all TCBs. Called just before the
	ACP exits by USER$Purge_All_IO in the USER module.

Side Effects:

	All TCB's are scanned searching for user IO requests to POST.  When
	a particular TCB has been examined then we delete it.
*/

void tcp$purge_all_io  (void)
    {
    signed long
	TCB;

// Scan all TCB's, posting any IO requests for them.

    VTCB_Scan ( Purge_TCB , 0 , 0 );
    }

//SBTTL "Purge User Requests for specified TCB"
/*
    Purge all user requests as well as any pending I/O and any received data
    that has not yet been delivered. This routine is called from the CANCEL
    processing to insure that all user I/O has been posted. It is also called
    as part of TCP$KILL_PENDING_REQUESTS when deleting or inactivating a TCB.
*/

void Purge_User_Requests(struct tcb_structure * TCB, signed long ERcode)
    {

// Kill pending user send requests.

    tcp$purge_send_queue(TCB,ERcode);

// Kill pending user receive requsts.

    tcp$purge_receive_queue(TCB,ERcode);

// Clean out any pending IO.

    NOINT;
    if (TCB->pending_io)
	if (TCB->argblk != 0)
	    {
	    USER$Err(TCB->argblk,ERcode);
	    TCB->argblk = 0;
	    };
    TCB->pending_io = FALSE;
    OKINT;

// Flush any received queued data & return back to the window
//~~~ This is probably OK, but make sure - may need to be in next routine.

    TCB->rcv_q_enqp = TCB->rcv_q_deqp;
    TCB->rcv_wnd = TCB->rcv_wnd + TCB->rcv_q_count;
    TCB->rcv_q_count = 0;
    }

//SBTTL "Kill Pending send/receive requests from specified TCB."
/*

Function:

	Purge all user requests (call Purge_User_Requests above) and flush
	all queues prior to deleting a TCB.

Inputs:

	TCB = pointer to currect TCB.
	ErCode = TCP Error Code to be returned to user.

Outputs:

	none

Side Effects:

	ALL TCB queues are flushed.  All queue elements which represent
	a user IO request are returned with the VMS error code "ABORT"
	& the TCP Error Code Specified in the rtn call.

*/

void tcp$kill_pending_requests(struct tcb_structure * TCB,signed long ERcode)
    {
	struct queue_blk_structure(qb_nr_fields) * QBR;

// Purge all user requests

    Purge_User_Requests(TCB,ERcode);

// Kill anything on the future queue

//~~~WHILE XREMQUE(TCB->RF_Qhead,QBR,tcp$kill_pending_requests,
//~~~		  Q$TCBFQ,TCB->RF_Qhead) != Empty_Queue DO
    while (REMQUE(TCB->rf_qhead,&QBR) != EMPTY_QUEUE) // check
	{
	mm$seg_free(QBR->nr$buf_size,QBR->nr$buf);
	mm$qblk_free(QBR);
	};

// Purge ReTransmission queue

    SEG$Purge_RT_Queue(TCB);
    }

//SBTTL "TCP$Cancel - Cancel connection for TCP."
/*

Function:

	Called by main VMS$Cancel routine in TCP_USER module when a $CANCEL
	request is initiated by VMS.  Called through VTCB_Scan.

Inputs:

	VMS cancel request arg block (VMS$Cancel_Args)

Outputs:

	None.

Side Effects:

	All I/O for the TCB is aborted, close is initiated on the connection.
*/

void Cancel_TCB( TCB , Idx , Uargs , P2)
     struct tcb_structure * TCB;
     struct vms$cancel_args * Uargs;
    {

// Check valid TCB looking for a match on User_ID VS specified PID &
// then user's IO channel # VS Process IO channel # in the TCB.
// On match, RESET/Delete the connection.

// Does process which has requested the IO Cancel own this Connection?
// If so check if this is the right (matching IO channel numbers) connection
// to cancel.

    if ((TCB->user_id == Uargs->vc$pid) &&
	(TCB->process_io_chan == Uargs->vc$piochan))
	{
	XLOG$FAO(LOG$USER,"!%T TCP$Cancel: TCB=!XL!/",0,TCB);

// Return all IO requests with Error: Connection Cancelled so the user doesn't
// hang waiting for IO completion.

	Purge_User_Requests(TCB,NET$_CCAN);

// Make sure we're not trying this again...

	if (! TCB->is_aborted)
	    {
	    signed long
		XTCB;
	    XTCB = TCB;

// Indicate TCB is aborted, and initiate a close on it

	    TCB->is_aborted = TRUE;
	    tcp$tcb_close(XTCB);
	    if (XTCB != 0)
		{
		TCB->curr_user_function = M$CANCEL;
		TCB->pending_io = TRUE;
		TCB->function_timer = Time_Stamp() + CLOSE_TIMEOUT;
		TCB->argblk = 0;
		};
	    };

	return 1;
	};

    return 0;
    }


//SBTTL "tcp$cancel - Cancel connection for TCP."
/*

Function:

	Called by main VMS$Cancel routine in USER module when a $CANCEL
	request is initiated by VMS.

Inputs:

	VMS cancel request arg block (VMS$Cancel_Args)

Outputs:

	None.

Side Effects:

	All I/O for the TCB is aborted, close is initiated on the connection.
*/

void tcp$cancel(struct vms$cancel_args * Uargs)
    {
    signed long
	Fcount;

// Check All valid TCB's looking for a match on User_ID VS specified PID &
// then user's IO channel # VS Process IO channel # in the TCB.
// On match, RESET/Delete the connection.

    Fcount = VTCB_Scan ( Cancel_TCB , Uargs , 0 );

    return Fcount;
    }

//SBTTL "User call: TCP$STATUS - Get the Status of Connection"
/*

Function:

	Return to the user the status of a specified connection

Inputs:

	Uargs = TCP user argument block.

Outputs:
	none

Side Effects:

	Status of specified connection is returned to the user in the
	user specified buffer.  IRP is queued to VMS IO post-processing
	rtns.

*/

void tcp$status(struct user_status_args * Uargs)
    {
    register
      struct queue_blk_structure(qb_nr_fields) * QB;
	struct tcb_structure * TCB;
    signed long
      RC;
	struct status_return_arg_blk strct,*CS=&strct;

    if ((TCB=tcb_ok(Uargs->st$local_conn_id,&RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Connection Doesn't Exist
	return;
	};

    if (Uargs->st$buf_size < SR_BLK_SIZE*4)
	{
	USER$Err(Uargs,NET$_BTS);// Buffer Too Small error.
	return;
	};

  
// Fill in Connection Status return argument block.
// Connection State & previous state.
  
    CS->sr$state = TCB->state;
    CS->sr$last_state = TCB->last_state;
  
// Send & Receive Windows
  
    CS->sr$send_window = TCB->snd_wnd;
    CS->sr$recv_window = TCB->rcv_wnd;
  
// Get receive data available

    CS->sr$bytes_avail = TCB->rcv_q_count;

// User identification
  
    CS->sr$user_id = TCB->user_id;
  
// Copy TCP local status return arg blk to system IO buffer for IO completion.
// Fill in MOVBYT arguments.

    // check
    $$KCALL(MOVBYT,SR_BLK_SIZE*4,CS,Uargs->st$data_start);
  
// Return the Connection Status to the user by posting the IO request.
  
    user$post_io_status(Uargs,SS$_NORMAL,SR_BLK_SIZE*4,0,0);
    mm$uarg_free(Uargs);		// release user arg block.
    }

//SBTTL "User call: TCP$INFO - Return Connection Information."
/*

Function:

	Return to the requesting user information about the connection.
	Foreign-Host,Local-Host & Node-Description strings are returned.

Inputs:

	Uargs = TCP user argument block.

Outputs:
	none

Side Effects:

	Information about a specified connection is returned to the user in the
	user specified buffer.  IRP is queued to VMS IO post-processing
	rtns.

*/

void tcp$info(struct user_info_args * Uargs)
    {
	extern void user$net_connection_info();
    signed long
	RC  = SS$_NORMAL;
    register
	struct tcb_structure * TCB;

    if ((TCB=tcb_ok(Uargs->if$local_conn_id,&RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Connection Doesn't Exist
	return;
	};

// Use common UDP/TCP routine to return the info

    user$net_connection_info(Uargs,TCB->local_host,TCB->foreign_host,
			TCB->local_port,TCB->foreign_port,
			TCB->foreign_hname,TCB->foreign_hnlen);
    }

tcp$tcb_dump(LCID,RB)
//
// Dump a single TCB.
//
 d$tcb_dump_return_blk * RB;
    {
      struct queue_blk_structure(qb_ur_fields) * QB;
      struct tcb_structure * TCB;
      unsigned long Now;

    if (! VTCB_Indx_OK (LCID))
	return FALSE;		// Give failure (error) return

    TCB = vtcb_ptr[LCID];
    RB->dm$tcb_addr = TCB;
    RB->dm$user_id = TCB->user_id;
    RB->dm$state = TCB->state;
    RB->dm$last_state = TCB->last_state;
    RB->dm$foreign_host = TCB->foreign_host;
    RB->dm$foreign_port = TCB->foreign_port;
    RB->dm$local_host = TCB->local_host;
    RB->dm$local_port = TCB->local_port;
    RB->dm$round_trip_time = TCB->round_trip_time;
    Now = Time_Stamp() ;
//    RB->dm$Conn_TimeOut = TCB->connection_timeout - Now ;
//    if (RB->dm$Conn_TimeOut LSS 0)
//	RB->dm$Conn_TimeOut = 0;
    if ((TCB->connection_timeout > Now))
      RB->dm$conn_timeout = TCB->connection_timeout - Now;
    else
	RB->dm$conn_timeout = 0;

//    RB->dm$RT_TimeOut = TCB->rx_timeout - Now ;
//    if (RB->dm$RT_TimeOut LSS 0)
//	RB->dm$RT_TimeOut = 0;
    if ((TCB->rx_timeout > Now))
      RB->dm$rt_timeout = TCB->rx_timeout - Now;
    else
	RB->dm$rt_timeout = 0;

// count # of elements on the user receive request queue.

    QB = TCB->ur_qhead;
    RB->dm$user_recv_qe = 0;
    while (QB != &TCB->ur_qhead)
	{
	RB->dm$user_recv_qe = RB->dm$user_recv_qe + 1;
	QB = QB->ur$next;
	};

// Elements on send queue

    RB->dm$user_send_qe = 0;
    QB = TCB->snd_qhead;
    while (QB != &TCB->snd_qhead)
	{
	RB->dm$user_send_qe = RB->dm$user_send_qe + 1;
	QB = QB->ur$next; // was: sn$next, but the same place
	};

// Elements on network segments received. (obsolete)

    RB->dm$net_recv_qe = 0;

// Elements on receive future queue

    RB->dm$future_qe = 0;
    QB = TCB->rf_qhead;
    while (QB != &TCB->rf_qhead)
	{
	RB->dm$future_qe = RB->dm$future_qe + 1;
	QB = QB->ur$next; // was: nr$next, but at same place 
	};

// Number of bytes on retransmission queue

    RB->dm$rexmit_qe = TCB->srx_q_count;

// user request disassembly pointers.

    RB->dm$dasm_ptr = TCB->rcv_q_deqp;
    RB->dm$dasm_bc = TCB->rcv_q_count;

// network segment ==> user data assembly pointers.

    RB->dm$asm_ptr = TCB->snd_q_deqp;
    RB->dm$asm_bc = TCB->snd_q_count;

// Sequence pointers

    RB->dm$iss = TCB->iss;
    RB->dm$snd_una = TCB->snd_una;
    RB->dm$snd_nxt = TCB->snd_nxt;
    RB->dm$snd_wnd = TCB->snd_wnd;
    RB->dm$snd_wl = TCB->snd_wl;
    RB->dm$snd_bs = TCB->snd_bs;
    RB->dm$irs = TCB->irs;
    RB->dm$rcv_nxt = TCB->rcv_nxt;
    RB->dm$rcv_wnd = TCB->rcv_wnd;

    return TRUE;
    }

//SBTTL "TCP$ABORT - User call to abort connection."
/*

Function:

	User request to "ABORT" a connection.  Clear ALL TCB queues &
	finially delete the TCB thus making the connection non-existant.

Inputs:

	Uargs = Address of TCP argument block, "ABORT" format.

Outputs:

	None.

Side Effects:

	All TCB queues are flushed.  All IO requests are returned to the
	user with VMS code of SS$_ABORT & the TCP Error code "Connection
	RESET".  TCB is finially deleted.  Connection no longer exists.

Notes:
	We make this a seperate function from TCP$ABORT because we
	sometimes want to kill a connection without posting IO.
*/

tcp$kill(TCBidx)
    {
extern	GET_TCB();
	struct tcb_structure * TCB=0;
    signed long
	RC;

    if ((RC=GET_TCB(TCBidx,TCB)) != SS$_NORMAL)
	return RC;

    switch (TCB->state)
      {
    case CS$LISTEN: case CS$SYN_SENT: case CS$CLOSING: case CS$TIME_WAIT: case CS$LAST_ACK: case CS$INACTIVE:
	{
	tcp$kill_pending_requests(TCB,NET$_KILL); // Error: connection Reset
 	tcb$delete(TCB);	// Connection RESET.
	}; // 1
	break;

    case CS$SYN_RECV: case CS$ESTABLISHED: case CS$FIN_WAIT_1: case CS$FIN_WAIT_2: case CS$CLOSE_WAIT:
	{
	send_rst(TCB);
	tcp$kill_pending_requests(TCB,NET$_KILL);
	tcb$delete(TCB);
	};// 2
      }

    return SS$_NORMAL;
    }// 3

//SBTTL "TCP$ABORT - User call to abort connection."
/*

Function:

	User request to "ABORT" a connection.  Call TCP$Kill the post IO.

Inputs:

	Uargs = Address of TCP argument block, "ABORT" format.

Outputs:

	None.

Side Effects:

	All TCB queues are flushed.  All IO requests are returned to the
	user with VMS code of SS$_ABORT & the TCP Error code "Connection
	RESET".  TCB is finially deleted.  Connection no longer exists.

*/

void tcp$abort(struct user_abort_args * Uargs)
    {
    signed long
	RC  = SS$_NORMAL;
    register
	struct tcb_structure * TCB;

    if ((TCB=tcb_ok(Uargs->ab$local_conn_id,&RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Connection Doesn't Exist (probably...)
	return;
	};

    switch (TCB->state)
      {
    case CS$LISTEN: case CS$SYN_SENT: case CS$CLOSING: case CS$TIME_WAIT: case CS$LAST_ACK:
	{
	tcp$kill_pending_requests(TCB,NET$_CR); // Error: connection Reset
 	tcb$delete(TCB);	// Connection RESET.
	};
	break;

    case CS$SYN_RECV: case CS$ESTABLISHED: case CS$FIN_WAIT_1: case CS$FIN_WAIT_2: case CS$CLOSE_WAIT:
	{
	send_rst(TCB);
	tcp$kill_pending_requests(TCB,NET$_CR);
	tcb$delete(TCB);
	};
      }

    user$post_function_ok(Uargs);	// okay
    }


//SBTTL "Deliver User Data"
/*

Function:

	Return data received by this TCB to the user specified buffer.
	Return as much data as possible within the user specified buffer size.

Inputs:

	TCB - points at current TCB.

Implicit Inputs:

	UR_Qhead - Points at list of User receive request Qblks.
	NR_Qhead - points at list of segments received from the network.
	TCB->Dasm_ptr = Points @ data in 1st queue element.
	TCB->Dasm_BC = # of data bytes available from 1st segment in NR_Qhead.

Outputs:

	None.

Side Effects:

	User's Network receive request is satisfied with as much data as
	available or exactly what the user asked for.  The Network I/O
	status block is returned with a VMS code of SS$_NORMAL & a count
	of bytes transfered.  If any errors the VMS code = "ABORT" & the
	TCP error field is valid.  The IO request tag field is always
	valid.  May request a window update "ACK" be sent if user data was
	delivered (TCB->Pending_ACK = true).


*/

void tcp$deliver_user_data(struct tcb_structure * TCB)
    {
    register
	struct queue_blk_structure(qb_ur_fields) * UQB; // User rcv request queue.
    register
	datasize,		// Size of data written to users VAS.
	Usize,			// Size requested by user.
	Uadrs;			// Address of user's system buffer.
    signed long
	Uflags,			// Flag bits to return to user
	Args[4];	// CMKRNL arg blk.

    UQB = TCB->ur_qhead;	// Point at User receive request list.

// Process User receive requests (UR_Qhead) until all have been processed
// or all the user-data bearing network segments have been exhausted.

    while ((UQB != &TCB->ur_qhead) && (TCB->rcv_q_count > 0))
	{
	Uflags = 0;
	if (TCB->rcv_push_flag)
	    Uflags = Uflags | NSB$PUSHBIT;
	Usize = UQB->ur$size;	// Size of data requested.
	Uadrs = UQB->ur$data;	// User's System buffer address.
	datasize = MIN(Usize,TCB->rcv_q_count);

// Indicate that we are doing this

	if ($$LOGF(LOG$USER))
	    {
		extern void ASCII_HEX_BYTES();
#define		maxhex 20
#define		maxasc 50
	    signed long
		nhex,nasc;
	    nhex = MIN(datasize,maxhex);
	    nasc = MIN(datasize,maxasc);
	    char hexstr_str[maxhex*3];
	    struct dsc$descriptor hexstr={dsc$w_length:maxhex*3,dsc$a_pointer:hexstr_str};
#if 0
	    DESC$STR_ALLOC(hexstr,maxhex*3),
#endif
	    ASCII_HEX_BYTES(&hexstr,nhex,TCB->rcv_q_deqp,
			    &hexstr.dsc$w_length);
	    LOG$FAO("!%T Deliver user data: DQ=!XL,EQ=!XL,RCQ=!XL/!XL,Size=!SL!/!_HEX:   !AS!/!_ASCII: !AF!/",
		    0,TCB->rcv_q_deqp,TCB->rcv_q_enqp,TCB->rcv_q_base,
		    TCB->rcv_q_end,datasize,&hexstr,nasc,TCB->rcv_q_deqp);
	    };

// Dequeue from TCB queue into user buffer

	// check
	$$KCALL(cq_dequeue,&TCB->rcv_q_queue,Uadrs,datasize);
  
// Update user/TCB data pointers

	TCB->rcv_duptr = TCB->rcv_duptr + datasize;
	if (TCB->rcv_push_flag)
	    if ((TCB->rcv_duptr - TCB->rcv_pptr) > 0)
		TCB->rcv_push_flag = FALSE;

// Post user IO request as complete & successful. Return # of bytes
// delivered to user.  Update the receive window as we now have more resources
// available for receiving segments.
// Remove user's IO request from the user_recv_request_q, release queueBlk and
// user_arg_blk.

	XLOG$FAO(LOG$USER,
		 "!%T Posting TCP$RECEIVE: Size=!SL,Flags=!XB,Uargs=!XL!/",
		 0,datasize,Uflags,UQB->ur$uargs);

	// we want to REMQUE the QBlock here since we don't want it on
	// the rec queue after we post it (since it might be posted twice.
	REMQUE(TCB->ur_qhead,&UQB); // remove the entry

	// post the IRP and release the Uargs
	user$post_io_status(UQB->ur$uargs,SS$_NORMAL,datasize,Uflags,0);
	mm$uarg_free(UQB->ur$uargs); // release user arg blk.

	ts$dbr = ts$dbr + datasize; // total data bytes delivered to users.

// attempt to beat the silly window syndrome, should we give a window update?

	TCB->ack_size = TCB->ack_size + datasize;
	if (TCB->ack_size >= TCB->snd_ack_threshold)
	    {

 // Return window resources.

	    TCB->rcv_wnd = TCB->rcv_wnd + TCB->ack_size;
	    TCB->ack_size = 0;
	    };

// Only flag an ACK if something has really changed.

	if ((TCB->old_rcv_nxt != TCB->rcv_nxt) ||
	    (TCB->old_rcv_wnd != TCB->rcv_wnd))
//	    tcb->pending_ack = True; // Indicate we need an ack sent.
	    tcp$enqueue_ack(TCB) ;	// Indicate we need an ack sent.
	mm$qblk_free(UQB);	// Release Queue blk.
	UQB = TCB->ur_qhead;	// point at next user request blk.
	};	// End of "OUTER" While.
    }

//SBTTL "Queue User Receive Request"
/*

Function:

	User has requested a receive function but there is no data available
	 or connection "STATE" is not correct so we must queue the receive
	request & wait for data to arrive.  Request is placed on this TCB's
	User Receive request list (UR_Qhead).

Inputs:

	TCB = TCB pointer.
	Uargs = TCP user receive request argument blk pointer.

Implicit Inputs:

	No data available from the network for this user.

Outputs:

	None.

Side Effects:

	Request is placed on User receive request list (UR_Qhead).
	User IO request has not been satisfied (ie, if waiting for Event
	flag, then ther're still waiting!).
*/

void Queue_Receive_Request(TCB,Uargs)
     struct tcb_structure * TCB;
     struct user_recv_args * Uargs;
    {
    register
	struct queue_blk_structure(qb_ur_fields) * QB;

// Fill in Queue Blk from user argument blk.

    QB = mm$qblk_get();		// Get a Queue block structure.
    QB->ur$size = Uargs->re$buf_size; // # of data bytes requested.
    QB->ur$data = Uargs->re$data_start; // Adrs of system buffer data start.
    QB->ur$irp_adrs = Uargs->re$irp_adrs; // IO request Packet address.
    QB->ur$ucb_adrs = Uargs->re$ucb_adrs; // Unit Control Blk address.
    QB->ur$uargs = Uargs;	// point at process-local user argblk.

// Add to end of user receive request list

    INSQUE(QB,TCB->ur_qtail);
    }

//SBTTL "TCP$RECEIVE - User call to receive data."
/*

Function:

	User has requested some data.  If data is available then fill the
	specified buffer OTHERWISE queue the receive request & process it
	when data becomes available.  Multiple TCP segments maybe returned
	to the requesting user in one receive request.

Inputs:

	Uargs = User receive argument block pointer.

Outputs:

	None.

Side Effects:

	IF User data available
	THEN
	User has data placed in specified buffer with NetIO Status block
	filled out to reflect the status of the receive (ie, which receive
	request, # of bytes, EOL, Urgent flags, & status code).
	else The USER request is queue & processed when data becomes available.
*/

 void    tcp$post_user_close_io_status();

void tcp$receive(struct user_recv_args * Uargs)
    {
    signed long
	RC  = SS$_NORMAL;
    register
	struct tcb_structure * TCB;

    if ((TCB=tcb_ok(Uargs->re$local_conn_id,&RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Connection doesn't exist.
	return;
	};

    XLOG$FAO(LOG$USER,"!%T TCP$RECEIVE: Conn=!XL, TCB=!XL, Size=!SL!/",
	     0,Uargs->re$local_conn_id,TCB,Uargs->re$buf_size);

    if (Uargs->re$buf_size <= 0)
	{
	USER$Err(Uargs,NET$_BTS);
	return;
	};

    if (TCB->user_timeout != 0)
	TCB->user_timeout = TCB->user_timeval + Time_Stamp();

	switch (TCB->state)
{
    case CS$LISTEN: case CS$SYN_SENT: case CS$SYN_RECV:
	Queue_Receive_Request(TCB,Uargs);
break;

    case CS$ESTABLISHED: case CS$FIN_WAIT_1: case CS$FIN_WAIT_2:
	{

// If network data available then deliver it to the user.

	Queue_Receive_Request(TCB,Uargs); // Must maintain FIFO queue.
	if (TCB->rcv_q_count > 0)
	    tcp$deliver_user_data(TCB);

// If FIN_RCVD set, we are in FIN-WAIT-2 waiting for pending data to be
// delivered. If the receive queue has become empty, finish closing the TCB now.

    	if (TCB->fin_rcvd && (TCB->rcv_q_count == 0))
	    {
	    tcp$set_tcb_state(TCB,CS$TIME_WAIT);
	    if (! TCB->close_nowait)
		{
		tcp$post_user_close_io_status(TCB,SS$_NORMAL);
		TCB->time_wait_timer = Time_Stamp() + MAX_SEG_LIFETIME;
		}
	    else
		{
		tcp$kill_pending_requests(TCB,NET$_TWT);
		XLOG$FAO(LOG$TCBSTATE,"!%T Time-Wait forced, conn=!XL!/",
			 0,TCB);
		tcb$delete(TCB);
		}
	    };
	};
break;

// Allow receives in Close_Wait state only if data available. The exception to
// this rule is that the first receive after all data has been delivered is
// allowed to succeed, with 0 bytes xferred, as an EOF marker.

    case CS$CLOSE_WAIT:
	{
	if (TCB->rcv_q_count > 0)
	    {
	    Queue_Receive_Request(TCB,Uargs);
	    tcp$deliver_user_data(TCB);
	    }
	else
	    if (! TCB->eof)
		{
		TCB->eof = TRUE;
		user$post_io_status(Uargs,SS$_NORMAL,0,
				    NSB$PUSHBIT | NSB$EOFBIT,0);
		mm$uarg_free(Uargs); // cleanup.
		}
	    else
		USER$Err(Uargs,NET$_CC);
	};
break;

    case CS$TIME_WAIT:
	USER$Err(Uargs,NET$_CC);
break;

    case CS$CLOSING: case CS$LAST_ACK:
	USER$Err(Uargs,NET$_CC);	// Error: Connection Closing.
}
    }

//SBTTL "Generate an Initial Sequence space number."
/*

Function:

	Generate an initial sequence number which is tied to today's clock
	to prevent duplicate segment numbers from being generated for a
	given connection.  covers the case of a connection being open'ed
	& closed in rapid succession with segments still floating around
	the network.  Possible pathological case.

Inputs:

	None.

Outputs:

	Unsigned integer which represents a sequence number.  Up to a full
	32-bits worth can be provided.

Side Effects:

	Sequence numbers are incremental in that a clock base is added to
	the last generated sequence number.
*/



ISN_GEN(void)
    {
static unsigned long isn;
signed long
	RVAL;
extern	user$clock_base();

    isn = isn+1;
RVAL = (user$clock_base()+isn)<<16; // check
    return RVAL;
    }

//SBTTL "Queue_Send_Data: Queue a data bearing send request."
/*

Function:

	Place a user's send request on the send queue.  Queue Block contains
	pointers to users buffer & send call arguments.  See field definition
	"QB_Send_Fields" for Queue Block structure.

Inputs:

	Uargs = user SEND call argument block
	TCB = Current TCB
Outputs:

	none.

Side Effects:

	Queue block is placed at the end of the send queue.
*/

void Queue_Send_Data(Uargs,TCB)
     struct user_send_args * Uargs;
	struct tcb_structure * TCB;
    {
	extern void tcp$send_enqueue();
    signed long
	args[4],
	Uaddr,
	Ucount;


// Append as much user data as we can to the queue.

    Uaddr = Uargs->se$data_start;
    Ucount = Uargs->se$buf_size;
    if ((TCB->snd_q_count+TCB->srx_q_count) < TCB->snd_q_size)
	{

// Enqueue as much as we can.

	tcp$send_enqueue(TCB,&Ucount,&Uaddr,Uargs->se$eol);
	if (Ucount == 0)
	    {

// If we got all of it, post request now, else remember remainder for later

	    user$post_io_status(Uargs,SS$_NORMAL,0,0,0);
	    mm$uarg_free(Uargs);
	    };
	};

// If there is still data left in this buffer, get & queue a send queue block

    if (Ucount > 0)
	{
	register
	    struct queue_blk_structure(qb_send_fields) * QB;
	QB = mm$qblk_get();
	QB->sn$size = Ucount;	// user's buffer length
	QB->sn$data = Uaddr;	// Point at start of data
	QB->sn$eol = Uargs->se$eol;
	QB->sn$urg = Uargs->se$urg;
	QB->sn$uargs = Uargs;	// point at TCP user argument blk.

//~~~ This may not be used any more - check

	if (TCB->user_timeval != 0)
	  QB->sn$timeout = TCB->user_timeval + Time_Stamp();
	else
	    QB->sn$timeout = CONN_TIMEVAL + Time_Stamp();

	INSQUE(QB,TCB->snd_qtail);
	};

// Flag we have user data which needs to be sent over the network.

    TCB->data_2_send = TRUE;
    }

//SBTTL "TCP$SEND: User call to send data over network"
/*

Function:

	User has requested that the data be transmitted over the network via
	the connection represented by the local-connection-id.  Verify
	user has privileges to use connection.  
	Queue the send request to the TCB & then if correct connection
	"STATE" then segmentize the data & send it over the network.
	Amount of data sent is controlled by the Send window (Snd_wnd).
	Multiple send requests will be collected in to the fewest #
	of TCP segments needed.  If the connection is "CLOSED" while there
	is still user data to be sent the TCB flag "Pending_Close" is set.
	The send_Data routine is aware of this flag & when the send queue
	is empty the fin will be sent.  If there is an error, the user's IO
	request is posted to VMS IO post-processing rtns thus completing
	the users IO request.  Otherwise the IO request is queued & maybe
	completed/posted by the Send_Data rtn.

Inputs:

	Uargs = User "SEND" call argument block

Implicit Inputs:

	Send request are ALWAYS for at least one data byte.  Virtual device
	"IP:" does not pass zero length send or receive requests.

Outputs:

	None

Side Effects:

	User TCP argument block & data are queued to the TCB send queue.
	If connection state & send window permits then data will be
	segmentized & send over the network (via: IP).
*/

void tcp$send(struct user_send_args * Uargs)
    {
    signed long
	RC  = SS$_NORMAL;
    register
	struct tcb_structure * TCB;

    if ((TCB=tcb_ok(Uargs->se$local_conn_id,&RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Connection does not exist.
	return;
	};

    XLOG$FAO(LOG$USER,"!%T TCP$SEND: Conn=!XL, TCB=!XL, Size=!SL!/",
		0,Uargs->se$local_conn_id,TCB,Uargs->se$buf_size);

    if (TCB->user_timeout != 0)
	TCB->user_timeout = TCB->user_timeval + Time_Stamp();

    switch (TCB->state)
      {
    case CS$NAMELOOK:
	USER$Err(Uargs,NET$_NOADR); // Connection not yet open
	break;

    case CS$LISTEN:
	{
	if ((TCB->foreign_host == WILD) || (TCB->foreign_port == WILD))
	    USER$Err(Uargs,NET$_FSU); // Foreign Socket unspecified.
	else
	    {
	    TCB->active_open = TRUE; // change connection  passive --> active
	    if (! send_syn(TCB)) // Try to initiate open...
		USER$Err(Uargs,NET$_NRT); // No route to host
	    else
		{
		Queue_Send_Data(Uargs,TCB); // Always have data.
		tcp$set_tcb_state(TCB,CS$SYN_SENT);
		};
	    };
	};
	break;

    case CS$SYN_SENT: case CS$SYN_RECV: case CS$CLOSE_WAIT: case CS$ESTABLISHED:
	Queue_Send_Data(Uargs,TCB);
	break;
/*~~~~~ Don't do SEND_DATA here - main processing loop can handle it
    case CS$CLOSE_WAIT: case CS$ESTABLISHED:
	{

// Queue the SEND request to maintain the FIFO ordering.

	Queue_Send_Data(Uargs,TCB);
	Send_Data(TCB);
	};
	break;
~~~~~*/

    case CS$FIN_WAIT_1: case CS$FIN_WAIT_2: case CS$TIME_WAIT: case CS$CLOSING: case CS$LAST_ACK:
	USER$Err(Uargs,NET$_CC);	// Connection Closed.
	break;
      }
    }

//SBTTL "User Call: (TCP$CLOSE) CLOSE a network connection"
/*

Function:

	User has requested TCP to close this connection.

Inputs:

	UARGS - points at the user's close arg blk

Outputs:

	None.

Side Effects:

	Connection maybe closed if no errors.  Closed implies that a "FIN"
	has been sent to the foreign_host or that it will be as soon as
	all the data has been segmentized & sent.
*/


void tcp$close(struct user_close_args * Uargs)
    {
    signed long
	RC  = SS$_NORMAL,
	XTCB;
    register
	struct tcb_structure * TCB;

    if ((TCB=tcb_ok(Uargs->cl$local_conn_id,&RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Connection Does NOT exist.
	return;
	};

    XLOG$FAO(LOG$USER,"!%T TCP$CLOSE: conn=!XL, TCB=!XL!/",
	     0,Uargs->cl$local_conn_id,TCB);

// Set close mode

    TCB->close_nowait = Uargs->cl$nowait;

// Initiate close at TCP level.

    XTCB = TCB;
    if ((RC=tcp$tcb_close(XTCB)) != 0)
	{
	USER$Err(Uargs,RC);	// Connection Closing.
	return;
	};

// If this close can complete now, post user else remember Uargs for later.

    if ((XTCB == 0) || (TCB->close_nowait) || (TCB->is_aborted) ||
	(TCB->state == CS$TIME_WAIT))
      user$post_function_ok(Uargs);
    else
	{
	TCB->pending_io = TRUE;
	TCB->curr_user_function = U$CLOSE;
	TCB->function_timer = Time_Stamp() + CLOSE_TIMEOUT;
	TCB->argblk = Uargs;
	};
    }

//Sbttl "Post User Close IO Status"
/*

Function:

	Some user network functions (OPEN, Close) do not post user IO
	completion until a network event has happened (eg, open: connection
	established). Here we take care of that posting. The TCB field ArgBlk
	points to the TCP argument block used in the TCP$OPEN/TCP$CLOSE
	routines (ie, used to be refernced by UARGS).  A function
	timer is running (TCB->Function_Timer) & if it times out then the
	IO will be posted as failure & we won't see it here. Magic TCB flag bit
	Pending_IO says that IO still needs to be posted.

Inputs:

	TCB = TCB pointer.
	VMS_RC = VMS return code (SS$_Normal etc.)
	TCP_RC = TCP Error code if NOT (VMS_RC = SS$_Normal).

Implicit Inputs:

	Assumed this was an wait-mode CLOSE
	TCB->pending_io - indicates IO needs to be posted.
	TCB->Function_Timer - handler function timeout.
	TCB->ArgBlk - points at user's TCP argument block.

Outputs:

	None.

Side Effects:

	User's IO is posted to VMS IO posting routines.  Pending_IO bit is
	cleared.  User argument block is deallocated.
*/


void tcp$post_user_close_io_status(struct tcb_structure * TCB,
					 signed long VMS_RC)
    {
    register
	struct User_close_args * Uargs;
	 netio_status_block iosb, *IOSB=&iosb;

// Check for pending_io flagged...

    if (TCB->pending_io)
	{
	TCB->pending_io = FALSE;
	if (TCB->argblk == 0)
	    return;
	IOSB->nsb$byte_count = 0;
	IOSB->nsb$status = VMS_RC;
	IOSB->net_status.nsb$xstatus = 0;
	Uargs = TCB->argblk;	// point at user's TCP arg block.
	IO$POST(IOSB,Uargs);
	mm$uarg_free(Uargs);	// Free user arg blk memory.
	};
    }


//SBTTL "Initialize specified TCB"
/*

Function:

	Fill in the required TCB slots as the specified TCB is newly created
	& unintialized although it has all longwords set to zero.

Inputs:

	TCB - points at a TCB structure

Outputs:

	True for success.
	False for error in user OPEN arg(network-node-name) is invalid.

Side Effects:

	TCB fields are initialized.  Queue headers point at themselves (Empty
	condition, as defined by VMS queue instructions). Sequence #'s are
	generated, Foreign_Host & port are setup.
*/

void tcp$tcb_init(TCB)
	struct tcb_structure * TCB;
    {

// Initialize segment space counters & user data assembly/disassembly
// ptrs & counters.

    TCB->snd_nxt = TCB->snd_una = TCB->iss = ISN_GEN();
    TCB->snd_wnd = TCB->rcv_wnd = window_default;
    TCB->snd_ack_threshold = ack_threshold ;
    TCB->tcb$flags = FALSE;	// clear all flags
//    TCB->max_seg_data_size = Default_Data_Size; // default: max data per seg
//   TCB->max_eff_data_size = Default_Data_Size; // default: max data per seg
    TCB->max_seg_data_size = default_mss ; // default: max data per seg
    TCB->max_eff_data_size = default_mss ; // default: max data per seg

// Init queue headers, Empty queue forward & back ptrs point at QHead element of
// queue header as defined by VAX-11 absolute queue instructions.

    TCB->lp_next = TCB->lp_back = &TCB->lp_next; // Local port list.
    TCB->rf_qhead = TCB->rf_qtail = &TCB->rf_qhead; // Received future segments.
    TCB->snd_qhead = TCB->snd_qtail = &TCB->snd_qhead; // Segment send.
    TCB->ur_qhead = TCB->ur_qtail = &TCB->ur_qhead; // user receive request.
    TCB->state = TCB->last_state = CS$CLOSED; // Set a known Connection state.

// Initialize circular queue pointers. TCB$Create already set queue addresses.
// Note that the "retransmission queue" is a clone of the send queue, and only
// really has a DEQ pointer.

    TCB->snd_q_end = TCB->snd_q_base + TCB->snd_q_size;
    TCB->snd_q_deqp = TCB->snd_q_enqp = TCB->snd_q_base;
    TCB->snd_q_count = 0;
    TCB->srx_q_end = TCB->srx_q_base + TCB->srx_q_size;
    TCB->srx_q_deqp = TCB->srx_q_enqp = TCB->srx_q_base;
    TCB->srx_q_count = 0;
    TCB->rcv_q_end = TCB->rcv_q_base + TCB->rcv_q_size;
    TCB->rcv_q_deqp = TCB->rcv_q_enqp = TCB->rcv_q_base;
    TCB->rcv_q_count = 0;
    TCB->rx_ctl = 0;
    TCB->rx_seq = 0;
    TCB->ack_size = 0;

// Set RX timer, initial RTT info, and the connection timeout.
// TCB->Connection_timeout is the actual active TCP connection timeout.
// It is updated when segments are seen arriving over the connection. If this
// timeout expires, the connection is assumed dead and is aborted.

    TCB->connection_timeout = CONN_TIMEVAL + Time_Stamp();
    TCB->round_trip_time = BASE_RT_TIMEOUT; // base for round trip timer.
    TCB->calculated_rto = BASE_RT_TIMEOUT; // Initial retransmission timer
    TCB->snd_delay_timer	=	BASE_RT_TIMEOUT + Time_Stamp() ;
    TCB->delayed_ack_timer	=	BASE_RT_TIMEOUT + Time_Stamp() ;
    XLOG$FAO(LOG$TCP,"!%T TCB_INIT TCB !XL!/",0,TCB);
    }

//SBTTL "TCP$OPEN - OPEN a Network connection"
/*

Function:

	"ABBRA KaDabbra Kadiddle Kazam......OPEN a connection while we can."
	User call to open a network connection.  Verify user has privileges to use
	local port if one was specified.  AOUS (Active OPEN User Specified)
	Local-Port situation is special cased so users can not screw-up TCP/FTP.

Inputs:

	UARGS - points at user open arg blk

Outputs:

	None.

Side Effects:

	User's OPEN request is posted to IO post-processing rtns.
	If No error then a TCB has been created & initialized.  If the
	user specified an ACTIVE open then a "SYN" control-segment has been
	sent to the specified foreign-host.
*/

void    TCP_NMLOOK_DONE();
void    tcp$adlook_done();

extern  void    NML$GETALST();
extern void    NML$GETNAME();

void tcp$open(struct user_open_args * Uargs)
    {
    register
      ipadr$address_block * ProtoHdr;
    register
	struct tcb_structure * TCB;
    signed long
	Args[4],
	HostWild,
	ucbptr,
	LP,
	FP,
	IPADDR;

    XLOG$FAO(LOG$USER,"!%T TCP$OPEN: PID=!XL,CHAN=!XL,FLAGS=!XL!/",
	     0,Uargs->op$pid,Uargs->op$piochan,Uargs->op$flags);

    ProtoHdr = &Uargs->op$protohdrblk;

// Handle easy errors before creating TCB, etc...

    HostWild = (! Uargs->op$addr_flag) && 
	       (CH$RCHAR(CH$PTR(Uargs->op$foreign_host,0)) == WILD);
    if (Uargs->op$active_open)
	{			// Active case

// For active open, wildcard foreign host or foreign port makes no sense.

	if (HostWild || (ProtoHdr->ipadr$dst_port == WILD))
	    {
	    USER$Err(Uargs,NET$_FSU);
	    return;
	    };
	}
    else
	{			// Passive case

// For passive open, wildcard local port makes no sense...

	if (ProtoHdr->ipadr$src_port == WILD)
	    {
	    USER$Err(Uargs,NET$_ILP);
	    return;
	    };
	};

// Create a TCB (Transmission Control Blk) for this connection

    if ((TCB = tcb$create()) == ERROR)
	{
	USER$Err(Uargs,NET$_UCT);// Error: Unable to create TCB.
	return;			// punt
	};

// Initialize the TCB

    tcp$tcb_init(TCB);

// Copy user args to TCB
// If we have a wild(0) Foreign_Host then don't bother to try and decode
// the asciz Foreign-Host-Name string. Likewise, we defer specification of
// the Local_Host field until a packet is received. Note that this means
// that wildcarding cannot work on active connections. Note that we allow
// the upper-level routine to determine this, so it can give a reasonable
// error code to the user.

    TCB->active_open = Uargs->op$active_open;
    TCB->open_nowait = Uargs->op$nowait;
    LP = ProtoHdr->ipadr$src_port ;
    FP = ProtoHdr->ipadr$dst_port ;
    TCB->local_port = (LP & 0xFFFF) ;
    TCB->foreign_port = (FP & 0xFFFF) ;
    TCB->ucb_adrs = Uargs->op$ucb_adrs;

// Set user specified connection time-out in seconds.
// TCB->User_timeout is a timeout for idle connections at the user level.
//	It is updated whenever the user touches the connection or whenever
//	user data is received. If op$timeout is unspecified, no user timeout
//	handling is performed, otherwise this value is saved in the cell
//	TCB->user_timeval and is used to compute this idle timeout.

    TCB->user_timeval = Uargs->op$timeout*CSEC;
    if (TCB->user_timeval != 0)
      TCB->user_timeout = TCB->user_timeval + Time_Stamp();
    else
	TCB->user_timeout = 0;
// Reset the TCP connection timeout for passive open case
    if (! TCB->active_open)
        TCB->connection_timeout = Time_Stamp() + PASSIVE_OPEN_TIMEOUT;
    TCB->user_id = Uargs->op$pid;
    TCB->process_io_chan = Uargs->op$piochan; // user's IO channel.

// Setup TCB ID in UCB - move 4-byte TCB index into system UCB

    ucbptr = &((struct _ucb *)Uargs->op$ucb_adrs)->UCB$L_CBID ; // check

	// check
    $$KCALL(MOVBYT,4,&TCB->vtcb_index,ucbptr);

// Remember Uargs for TCP_NMLOOK_DONE

    TCB->argblk = Uargs;

// If wildcard foreign name, then done...

    if (HostWild)
	{
	TCB->foreign_host = WILD;
//	TCB->local_host = WILD;
        TCB->pending_io = TRUE;                 //[VU] Satisfy new validity test
    	TCP_NMLOOK_DONE(TCB,SS$_NORMAL,0,0,0,0);
	return;
	};

// Check for user open with IP address.

X:  {			// *** Block X ***
    if (Uargs->op$addr_flag)
      IPADDR = Uargs->op$foreign_address;
    else
	{
	signed long
	    NAMPTR;
	NAMPTR = CH$PTR(Uargs->op$foreign_host,0);
	if (GET_IP_ADDR(&NAMPTR,&IPADDR) < 0)
	goto leave;
        };

// Have an IP address - finish the open now & start address translation

    TCB->foreign_hnlen = 0;
    TCB->pending_io = TRUE;                 //[VU] Satisfy new validity test
    TCP_NMLOOK_DONE(TCB,SS$_NORMAL,1,&IPADDR,0,0);
    TCB->nmlook_flag = TRUE;
    NML$GETNAME(IPADDR,tcp$adlook_done,TCB);
    return;
    }			// *** Block X ***
    leave:

// Have a host name. Start name lookup which will finish the open when done.

    TCB->state = CS$NAMELOOK;
    TCB->pending_io = TRUE;
    TCB->curr_user_function = U$OPEN;
    TCB->function_timer = Time_Stamp() + NAMELOOK_TIMEOUT;
    TCB->nmlook_flag = TRUE;
    NML$GETALST(CH$PTR(Uargs->op$foreign_host,0),Uargs->op$foreign_hlen,
		TCP_NMLOOK_DONE,TCB);
    }

//SBTTL "TCP_NMLOOK_DONE - Come here when name lookup for TCB finishes"
/*
    Here is where we finish up the open.
*/


void TCP_NMLOOK_DONE(TCB,STATUS,ADRCNT,ADRLST,NAMLEN,NAMPTR)
	struct tcb_structure * TCB;
    {
      struct user_open_args * Uargs;
    signed long
	ISWILD,
	OpenErr,
	LP,
        Count,
	CN_Index,
	ok;
       netio_status_block iosb, *IOSB=&iosb;

   //[VU] Insure TCB valid before modifying.  This protects against case
   //[VU] where Service_Connections first times-out pending I/O functions
   //[VU] and later times-out and deletes the resulting inactive connection.

    if (! VTCB_Indx_OK(TCB->vtcb_index))
	{
	WARN$FAO ("!%T VTCB Index out of range (!XL)",0,TCB->vtcb_index);
        return;
	};

// Clear name lookup flag and pending name lookup I/O

    NOINT;
    TCB->nmlook_flag = FALSE;
    ok = TCB->pending_io;     //[VU] Save for validity check.
    TCB->pending_io = FALSE;
    Uargs = TCB->argblk;
    TCB->argblk = 0;
    OKINT;

// If the name lookup failed, give the error now and delete the TCB

    if (! STATUS)
	{
	USER$Err(Uargs,STATUS);
	tcb$delete(TCB);
	return;
	};

    //[VU] Test validity of ARGBLK and Pending_IO to avoid system crashes, etc.
    //[VU] In each case, we assume that Service_Connections will clean up anythin
    //[VU] that it has not already.
    //[VU] All callers of TCP_NMLOOK_DONE now set Pending_IO as well as ARGBLK
    //[VU] to support this validity test.  This should be safe since new sites
    //[VU] which set Pending_IO call TCP_NMLOOK_DONE immediately, which clears
    //[VU] the flag.
    if (ok == 0) return;           //[VU] No pending I/O
    if (Uargs == 0) return;        //[VU] No ARGBLK

// Name lookup succeded. Set the host numbers and host name.

    if (ADRLST != 0)
	{
	IP$SET_HOSTS(ADRCNT,ADRLST,&TCB->local_host,&TCB->foreign_host);
	TCB->foreign_hnlen = NAMLEN;
	if (NAMLEN != 0)
	    CH$MOVE(NAMLEN,NAMPTR,CH$PTR(TCB->foreign_hname,0));
	};

// Finish opening the connection

    ok = OpenErr = FALSE;

// Check access to the specified hosts/ports

    ok = user$check_access(TCB->user_id,TCB->local_host,TCB->local_port,
		      TCB->foreign_host,TCB->foreign_port);
    if (! ok)
	{
	tcb$delete(TCB);
	USER$Err(Uargs,ok);
	return;
	};

// Set current function for timeout

    TCB->curr_user_function = U$OPEN;
    NOINT;
    if (TCB->active_open)
	{

// Process an "ACTIVE" open.

	if ((TCB->local_port != WILD))
	    {		// user specified local port?

// make sure it's a unique connection request.
// Is connection table full? || a Non-unique connection?
// Either case report the error.

	    XLOG$FAO(LOG$USER,
		     "!%T ACTIVE Open, LP = !XL (!UL), FP = !XL (!UL)!/",
		     0,TCB->local_port,TCB->local_port,
		     TCB->foreign_port,TCB->foreign_port);

	    ok =check_unique_conn(TCB->local_port, TCB->foreign_host,
				   TCB->foreign_port, &CN_Index);
	    if (ok == ERROR)
		{		// Error: Connection Tbl space Exhausted.
		tcb$delete(TCB);
		USER$Err(Uargs,NET$_CSE);
		OKINT;
		return;
		}
	    else
		if (! ok)
		    {	// Error: Non-Unique Connection
		    tcb$delete(TCB);
		    USER$Err(Uargs,NET$_NUC);
		    OKINT;
		    return;
		    }
	    }
	else
	    {		// Active OPEN Unspecified LP, allocate one.

// Allocate a user local port & check if the connection will be unique.
// Loop until an allocated Local-port & rest of connection is unique.

	    XLOG$FAO(LOG$USER,
		     "!%T ACTIVE Open, wild LP, FP = !XL (!UL)!/",
		     0,TCB->foreign_port,TCB->foreign_port);
	    do
		{
		LP = user$get_local_port(&TCP_User_LP);
		ok =check_unique_conn(LP,TCB->foreign_host,
				       TCB->foreign_port,&CN_Index);
 		if (ok == ERROR)
		    {	// Connection table Space Exhausted
		    tcb$delete(TCB);
		    USER$Err(Uargs,NET$_CSE);
		    OKINT;
		    return;
		    }
		}
	    while (!ok);
	    TCB->local_port = (LP & 0xFFFF) ;
	    };

// Set initial state for active open

	if (! send_syn(TCB)) // Try to start 3-way handshake
	    {
	    tcb$delete(TCB);
	    USER$Err(Uargs,NET$_NRT); // No route to host
	    OKINT;
	    return;
	    };
	tcp$set_tcb_state(TCB,CS$SYN_SENT);

// Set function timer for active open timeout

	TCB->function_timer = Time_Stamp() + ACTIVE_OPEN_TIMEOUT;
	ts$aco = ts$aco + 1;
	}
    else			// Passive Open
	{
	XLOG$FAO(LOG$USER,
		 "!%T Passive open on LP=!XL (!UL), FP=!XL (!UL)!/",
		 0,TCB->local_port,TCB->local_port,
		 TCB->foreign_port,TCB->foreign_port);

// Check for uniqueness of host/port set. This has the side effect of obtaining
// a slot for us in the connection table.

	ISWILD=(TCB->foreign_port == WILD) || (TCB->foreign_host == WILD);

	ok =check_unique_conn(TCB->local_port,TCB->foreign_host,
			       TCB->foreign_port,&CN_Index);

// OK = Error means that the connection table was full - punt.

	if (ok == ERROR)
	    {
	    tcb$delete(TCB);
	    USER$Err(Uargs,NET$_CSE);
	    OKINT;
	    return;
	    };

// OK = FALSE means the connection was non-unique. This is NOT an error if the
// foreign host or port is wildcarded.

	if ((! ok) && (! ISWILD))
	    {
	    tcb$delete(TCB);
	    USER$Err(Uargs,NET$_NUC);
	    OKINT;
	    return;
	    };

// Set initial state for Passive Open

	tcp$set_tcb_state(TCB,CS$LISTEN);

// Indicate which function we're timing.

	TCB->function_timer = Time_Stamp() + PASSIVE_OPEN_TIMEOUT;
	ts$pco = ts$pco + 1; // count passive opens.
	};

// Save connection table index and insert into the connection table

    TCB->con_index = CN_Index;
    Conect_Insert(TCB,CN_Index);
    OKINT;
	
// Log successful open

    XLOG$FAO(LOG$USER,"!%T TCP$OPEN: Conn idx = !XL, TCB = !XL!/",
	     0,TCB->vtcb_index,TCB);

// Check for immediate-return mode

    if (TCB->open_nowait)
	{

// Return Local-Connection-ID (Actually the TCB's address).
// 2nd fullword of the user's Net IO status block is the Local conn ID.

	IOSB->nsb$status = SS$_NORMAL; // indicate success.
	IOSB->nsb$byte_count = 0;
	IOSB->net_status.nsb$xstatus = 0;
	IO$POST(IOSB,Uargs); // Tell em.
	mm$uarg_free(Uargs);	// Release user arg blk.
	}
    else
	{

// He wants to wait. Set up for I/O posting in SEGIN when connection becomes
// established.

	TCB->pending_io = TRUE;	// indicate we have IO to post later.
	TCB->argblk = Uargs;	// save pointer to user argument blk.
	};

// Check SYN wait list for passive connection.
// It is important that this step be done last, since the pending I/O
// information must be set up for the callback to Post_User_Active_Open.

    if (! TCB->active_open)
	SEG$Check_SYN_Wait_List(TCB);
    }

//SBTTL "TCP$Adlook_Done - Handle completion of address to name lookup"

void tcp$adlook_done(TCB,STATUS,NAMLEN,NAMPTR)
     struct tcb_structure * TCB;
    {

// Check status

    if (! STATUS)
	return;

// Make sure the TCB is still valid
    if ( ! VTCB_Indx_OK (TCB->vtcb_index))
	{
	XLOG$FAO(LOG$TCPERR,"!%T ADLOOK-DONE for nonexist. TCB !XL!/",0,TCB);
	return;
	};

// Make sure it is really waiting for this to happen...

    if (! TCB->nmlook_flag)
	{
	XLOG$FAO(LOG$TCPERR,"!%T ADLOOK-DONE unexpected for TCB !XL!/",0,TCB);
	return;
	};

// Clear name lookup flag

    TCB->nmlook_flag = FALSE;

// Copy the host name into the TCB

    TCB->foreign_hnlen = NAMLEN;
    CH$MOVE(NAMLEN,NAMPTR,CH$PTR(TCB->foreign_hname,0));
    }

//Sbttl "Post Users Active Open User IO Status"
/*

Function:

	Some user network functions (OPEN, Close) do not post user IO
	completion until a network event has happened (eg, open: connection
	established). Here we take care of that posting. The TCB field ArgBlk
	points to the TCP argument block used in the TCP$OPEN/TCP$CLOSE
	routines (ie, used to be refernced by UARGS).  A function
	timer is running (TCB->Function_Timer) & if it times out then the
	IO will be posted as failure & we won't see it here. Magic TCB flag bit
	Pending_IO says that IO still needs to be posted.

Inputs:

	TCB = TCB pointer.
	VMS_RC = VMS return code (SS$_Normal etc.)
	TCP_RC = TCP Error code if ! (VMS_RC = SS$_Normal).

Implicit Inputs:

	Assumed this was an ACTIVE open (ie, TCB->Active_OPEN = True).
	TCB->pending_io - indicates IO needs to be posted.
	TCB->function_timer - handler function timeout.
	TCB->argblk - points at user's TCP argument block.

Outputs:

	None.

Side Effects:

	User's IO is posted to VMS IO posting routines.  Pending_IO bit is
	cleared.  User argument block is deallocated.
*/


void tcp$post_active_open_io_status(struct tcb_structure * TCB,
					  signed long VMS_RC)
    {
    register
	struct user_open_args * Uargs;
	netio_status_block iosb, *IOSB=&iosb;

    if (TCB->pending_io)
	{
	TCB->pending_io = FALSE;
	if (TCB->argblk != 0)
	    {
	    IOSB->nsb$status = VMS_RC;
	    IOSB->nsb$byte_count = 0;
	    IOSB->net_status.nsb$xstatus = 0;
	    Uargs = TCB->argblk; // point at user's TCP arg block.
	    IO$POST(IOSB,Uargs);
	    mm$uarg_free(Uargs);	// Free user arg blk memory.
	    };
	};
    }

