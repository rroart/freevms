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
	Know about S}_SYN (S}_CTL) returning routing failure.

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
	calling sequence of User$Post_IO_Status.

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
	TCP$KILL_P}ING_REQUESTS off into a new routine, PURGE_USER_REQUESTS,
	which does only this.

1.9  12-Sep-86, Edit by VAF
	In second part of open (TCP_NMLOOK_DONE), don't set host numbers if
	the foreign host was unspecified.
	Also, call Check_Syn_Wait_List at end of passive open.

1.8  14-Aug-86, Edit by VAF
	Fix problem with passive open.
	Make things that check TCB->NMLook_Flag and TCB->ARGBLK be NOINT.
	Make TCP$CANCEL use TCP$KILL_P}ING_REQUESTS.

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
 )%


//SBTTL "Module definition"

MODULE TCP_USER(IDENT="3.7",LANGUAGE(BLISS32),
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

XQDEFINE			// (maybe) define queue debugging externals

!*** Special literals from USER.BLI ***

extern signed long LITERAL
    UCB$Q_DDP,
    UCB$L_CBID,
    UCB$L_EXTRA;

extern signed long LITERAL
// User function codes

    M$CANCEL : UNSIGNED(8),
    U$OPEN : UNSIGNED(8),
    U$CLOSE : UNSIGNED(8);

extern signed long
    Default_MSS,
    ACK_THRESHOLD,
    WINDOW_DEFAULT,
    AST_IN_PROGRESS,
    INTDF,
    CONN_TIMEVAL	:	UNSIGNED,
    TCP_User_LP,
    Log_State,
    struct VECTOR * VTCB_Ptr->0,
    ts$dbr,
    ts$aco,
    ts$pco;

extern

// Rtns from TCP_MECH.BLI

    TCB_OK,
    VTCB_Indx_OK,
    VTCB_Scan,
    CONECT_INSERT,
    Check_Unique_Conn,
    TCB$Create,
    TCB$Delete: NOVALUE,

// Rtns from MEMGR.BLI

    MM$Seg_Get,
    MM$Seg_Free: NOVALUE,
    MM$QBLK_Get,
    MM$QBLK_Free: NOVALUE,
    MM$UArg_Free: NOVALUE,

// IOUTIL.BLI

    GET_IP_ADDR,
    LOG_CHANGE,
 VOID    LOG_OUTPUT,
 VOID    LOG_TIME_STAMP,
 VOID    LOG_FAO,

// SEGIN.BLI

    SEG$Check_SYN_Wait_List: NOVALUE,
    SEG$Purge_RT_Queue: NOVALUE,

// IP.BLI

 VOID    IP$SET_HOSTS,

// Routines From:  MACLIB.MAR

    Set_IP_device_OFFline: NOVALUE,
    User_Requests_Avail,
    MovByt: NOVALUE,
    SwapBytes: NOVALUE,
    Time_Stamp,

// TCP.BLI

    TCP$S}_CTL,
 void    TCP$Enqueue_ACK,
 VOID    TCP$S}_DATA,
 VOID    TCP$SET_TCB_STATE,
 VOID    TCP$INACTIVATE_TCB,
    TCP$TCB_CLOSE,
 void    CQ_Dequeue,

// USER.BLI

 VOID    IO$POST,
 void    User$Post_IO_Status,
 void    USER$Post_Function_OK,
    USER$Err,
    USER$CHECK_ACCESS,
    USER$GET_LOCAL_PORT;

FORWARD ROUTINE
    TCP$KILL_P}ING_REQUESTS: NOVALUE;

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

USER_OK(struct User_default_Args * Args,struct tcb_structure * tcb)
    {
    if (tcb->user_id == Args->ud$pid)
	RETURN(true)
    else
	RETURN(false);
    };

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

TCP$Purge_Send_Queue(struct tcb_structure * tcb,RC): NOVALUE (void)
    {
    REGISTER
	struct Queue_Blk_Structure * QB(QB_Send_Fields),
	struct User_Send_Args * Uargs;

    WHILE REMQUE(TCB->Snd_QHead,QB) != Empty_Queue DO
	{
	Uargs = QB->SN$Uargs;	// point at user argblk.
	User$Post_IO_Status(Uargs,RC,0,0,0);
	MM$UArg_Free(Uargs);	// release user arg blk.
	MM$QBlk_Free(QB);		// release queue block
	};
    };

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

TCP$Purge_Receive_Queue(struct tcb_structure * tcb,RC): NOVALUE (void)
    {
    REGISTER
	struct Queue_Blk_Structure * QB(QB_UR_Fields);

    WHILE REMQUE(TCB->UR_Qhead,QB) != Empty_Queue DO
	{
	User$Post_IO_Status(QB->UR$Uargs,RC,0,0,0);
	MM$UArg_Free(QB->UR$Uargs); // release user arg block.
	MM$QBlk_Free(QB);
	};
    };

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

!!!HACK!!// ADT violation
    VTCB_ptr[IDX] = 0; // invalidate tcb.
    TCP$KILL_P}ING_REQUESTS(TCB,NET$_TE);
    1
    };

//SBTTL "Purge/Post all I/O requests for all TCBs."
/*

Function:

	Get rid of all TCB queue items for all TCBs. Called just before the
	ACP exits by USER$Purge_All_IO in the USER module.

Side Effects:

	All TCB's are scanned searching for user IO requests to POST.  When
	a particular TCB has been examined then we delete it.
*/

TCP$Purge_All_IO : NOVALUE (void)
    {
    signed long
	TCB;

// Scan all TCB's, posting any IO requests for them.

    VTCB_Scan ( Purge_TCB , 0 , 0 );
    };

//SBTTL "Purge User Requests for specified TCB"
/*
    Purge all user requests as well as any pending I/O and any received data
    that has not yet been delivered. This routine is called from the CANCEL
    processing to insure that all user I/O has been posted. It is also called
    as part of TCP$KILL_P}ING_REQUESTS when deleting or inactivating a TCB.
 )%

void Purge_User_Requests(struct tcb_structure * tcb,ERcode) (void)
    {

// Kill pending user send requests.

    TCP$Purge_Send_Queue(TCB,ERcode);

// Kill pending user receive requsts.

    TCP$Purge_Receive_Queue(TCB,ERcode);

// Clean out any pending IO.

    NOINT;
    if (TCB->Pending_IO)
	if (TCB->ARGBLK != 0)
	    {
	    USER$Err(TCB->ARGBLK,ERcode);
	    TCB->ARGBLK = 0;
	    };
    TCB->Pending_IO = FALSE;
    OKINT;

// Flush any received queued data & return back to the window
!~~~ This is probably OK, but make sure - may need to be in next routine.

    TCB->RCV_Q_ENQP = TCB->RCV_Q_DEQP;
    TCB->RCV_WND = TCB->RCV_WND + TCB->RCV_Q_Count;
    TCB->RCV_Q_Count = 0;
    };

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

TCP$KILL_P}ING_REQUESTS(struct tcb_structure * tcb,ERCODE): NOVALUE (void)
    {
    signed long
	struct Queue_Blk_Structure * QBR(QB_NR_Fields);

// Purge all user requests

    Purge_User_Requests(TCB,ERcode);

// Kill anything on the future queue

!~~~WHILE XREMQUE(TCB->RF_Qhead,QBR,TCP$KILL_P}ING_REQUESTS,
!~~~		  Q$TCBFQ,TCB->RF_Qhead) != Empty_Queue DO
    WHILE REMQUE(TCB->RF_Qhead,QBR) != Empty_Queue DO
	{
	MM$Seg_Free(QBR->NR$Buf_size,QBR->NR$Buf);
	MM$QBlk_Free(QBR);
	};

// Purge ReTransmission queue

    SEG$Purge_RT_Queue(TCB);
    };

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

void Cancel_TCB( TCB , Idx , Uargs , P2) (void)
    {
    MAP
	struct tcb_structure * tcb,
	struct VMS$Cancel_Args * Uargs;

// Check valid TCB looking for a match on User_ID VS specified PID &
// then user's IO channel # VS Process IO channel # in the TCB.
// On match, RESET/Delete the connection.

// Does process which has requested the IO Cancel own this Connection?
// If so check if this is the right (matching IO channel numbers) connection
// to cancel.

    IF (TCB->User_ID == Uargs->VC$PID) AND
       (TCB->Process_IO_Chan == Uargs->VC$PIOchan) THEN
	{
	XLOG$FAO(LOG$USER,"!%T TCP$Cancel: TCB=!XL!/",0,TCB);

// Return all IO requests with Error: Connection Cancelled so the user doesn't
// hang waiting for IO completion.

	Purge_User_Requests(TCB,NET$_CCAN);

// Make sure we're not trying this again...

	if (NOT TCB->IS_Aborted)
	    {
	    signed long
		XTCB;
	    XTCB = TCB;

// Indicate TCB is aborted, and initiate a close on it

	    TCB->IS_Aborted = TRUE;
	    TCP$TCB_CLOSE(XTCB);
	    if (XTCB != 0)
		{
		TCB->Curr_user_function = M$CANCEL;
		TCB->Pending_IO = TRUE;
		TCB->Function_Timer = Time_Stamp() + Close_Timeout;
		TCB->Argblk = 0;
		};
	    };

	return 1
	};

    return 0
    };


//SBTTL "TCP$Cancel - Cancel connection for TCP."
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

void TCP$Cancel(struct VMS$Cancel_Args * Uargs) (void)
    {
    signed long
	Fcount;

// Check All valid TCB's looking for a match on User_ID VS specified PID &
// then user's IO channel # VS Process IO channel # in the TCB.
// On match, RESET/Delete the connection.

    Fcount = VTCB_Scan ( Cancel_TCB , Uargs , 0 );

    Fcount
    };

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

TCP$STATUS(struct User_Status_args * Uargs): NOVALUE (void)
    {
    REGISTER
	struct Queue_Blk_Structure * QB(QB_NR_Fields),
	struct tcb_structure * tcb;
    signed long
	RC,
	CS: Status_Return_Arg_Blk;

    if ((TCB=TCB_OK(Uargs->ST$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Connection Doesn't Exist
	RETURN;
	};

    if (Uargs->ST$Buf_Size LSS SR_Blk_Size*4)
	{
	USER$Err(Uargs,NET$_BTS);// Buffer Too Small error.
	RETURN;
	};

  
// Fill in Connection Status return argument block.
// Connection State & previous state.
  
    CS->SR$State = TCB->State;
    CS->SR$Last_state = TCB->Last_State;
  
// Send & Receive Windows
  
    CS->SR$Send_Window = TCB->snd_Wnd;
    CS->SR$Recv_Window = TCB->Rcv_Wnd;
  
// Get receive data available

    CS->SR$Bytes_Avail = TCB->RCV_Q_Count;

// User identification
  
    CS->SR$User_ID = TCB->User_ID;
  
// Copy TCP local status return arg blk to system IO buffer for IO completion.
// Fill in MOVBYT arguments.

    $$KCALL(MOVBYT,SR_BLK_SIZE*4,CS,Uargs->ST$Data_Start);
  
// Return the Connection Status to the user by posting the IO request.
  
    User$Post_IO_Status(Uargs,SS$_NORMAL,SR_Blk_Size*4,0,0);
    MM$UArg_Free(Uargs);		// release user arg block.
    };

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

TCP$INFO(struct User_Info_Args * Uargs): NOVALUE (void)
    {
    EXTERNAL ROUTINE
	USER$Net_Connection_Info : NOVALUE;
    signed long
	RC  = SS$_NORMAL;
    REGISTER
	struct tcb_structure * tcb;

    if ((TCB=TCB_OK(Uargs->IF$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Connection Doesn't Exist
	RETURN;
	};

// Use common UDP/TCP routine to return the info

    USER$Net_Connection_Info(Uargs,TCB->Local_Host,TCB->Foreign_Host,
			TCB->Local_Port,TCB->Foreign_Port,
			TCB->Foreign_Hname,TCB->Foreign_Hnlen);
    };

TCP$TCB_Dump(LCID,RB)
!
// Dump a single TCB.
!
    {
    MAP
	struct D$TCB_Dump_Return_Blk * RB;
    signed long
	struct tcb_structure * tcb,
	Now	:	UNSIGNED,
	QB;

    if (NOT VTCB_Indx_OK (LCID))
	return FALSE;		// Give failure (error) return

    TCB = VTCB_ptr[LCID];
    RB->dm$tcb_addr = TCB;
    RB->dm$user_id = TCB->user_id;
    RB->dm$state = TCB->state;
    RB->dm$last_state = TCB->last_state;
    RB->dm$Foreign_Host = TCB->Foreign_Host;
    RB->dm$Foreign_Port = TCB->Foreign_Port;
    RB->dm$Local_Host = TCB->Local_Host;
    RB->dm$Local_Port = TCB->Local_port;
    RB->dm$Round_Trip_Time = TCB->Round_Trip_Time;
    Now = Time_Stamp() ;
//    RB->dm$Conn_TimeOut = TCB->Connection_TimeOut - Now ;
//    if (RB->dm$Conn_TimeOut LSS 0)
!	RB->dm$Conn_TimeOut = 0;
    if ((TCB->Connection_TimeOut > Now))
	RB->dm$Conn_TimeOut = TCB->Connection_TimeOut - Now
    else
	RB->dm$Conn_TimeOut = 0;

//    RB->dm$RT_TimeOut = TCB->RX_TimeOut - Now ;
//    if (RB->dm$RT_TimeOut LSS 0)
!	RB->dm$RT_TimeOut = 0;
    if ((TCB->RX_TimeOut > Now))
	RB->dm$RT_TimeOut = TCB->RX_TimeOut - Now
    else
	RB->dm$RT_TimeOut = 0;

// count # of elements on the user receive request queue.

    qb = TCB->UR_qhead;
    RB->dm$user_recv_qe = 0;
    WHILE qb NEQA TCB->UR_qhead DO
	{
	MAP
	    struct Queue_Blk_Structure * qb(QB_UR_Fields);
	RB->dm$user_recv_qe = RB->dm$user_recv_qe + 1;
	qb = qb->ur$next;
	};

// Elements on send queue

    RB->dm$user_send_qe = 0;
    QB = TCB->SND_qhead;
    WHILE qb NEQA TCB->SND_qhead DO
	{
	MAP
	    struct Queue_Blk_Structure * qb(QB_Send_Fields);
	RB->dm$user_send_qe = RB->dm$user_send_qe + 1;
	qb = qb->sn$next;
	};

// Elements on network segments received. (obsolete)

    RB->dm$net_recv_qe = 0;

// Elements on receive future queue

    RB->dm$future_qe = 0;
    QB = TCB->RF_qhead;
    WHILE qb NEQA TCB->RF_qhead DO
	{
	MAP
	    struct Queue_Blk_Structure * qb(QB_NR_Fields);
	RB->dm$future_qe = RB->dm$future_qe + 1;
	qb = qb->nr$next;
	};

// Number of bytes on retransmission queue

    RB->dm$rexmit_qe = TCB->SRX_Q_Count;

// user request disassembly pointers.

    RB->dm$Dasm_ptr = TCB->RCV_Q_DEQP;
    RB->dm$Dasm_bc = TCB->RCV_Q_Count;

// network segment ==> user data assembly pointers.

    RB->dm$Asm_ptr = TCB->SND_Q_DEQP;
    RB->dm$asm_bc = TCB->SND_Q_Count;

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
    };

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

TCP$KILL(TCBidx)
    {
    EXTERNAL ROUTINE
	GET_TCB;
    signed long
	struct tcb_structure * tcb INITIAL(0);
    signed long
	RC;

    if ((RC=GET_TCB(TCBidx,TCB)) != SS$_NORMAL)
	return RC;

    SELECTONE TCB->State OF
    SET
    [CS$Listen,CS$SYN_Sent,CS$Closing,CS$Time_Wait,CS$Last_Ack,CS$Inactive]:
	{
	TCP$KILL_P}ING_REQUESTS(TCB,NET$_KILL); // Error: connection Reset
 	TCB$Delete(TCB);	// Connection RESET.
	}; // 1

    [CS$SYN_RECV,CS$Established,CS$Fin_Wait_1,CS$Fin_Wait_2,CS$Close_Wait]:
	{
	Send_RST(TCB);
	TCP$KILL_P}ING_REQUESTS(TCB,NET$_KILL);
	TCB$Delete(TCB);
	};// 2
    TES;

    SS$_NORMAL
    };// 3

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

TCP$ABORT(struct User_Abort_Args * Uargs): NOVALUE (void)
    {
    signed long
	RC  = SS$_NORMAL;
    REGISTER
	struct tcb_structure * tcb;

    if ((TCB=TCB_OK(Uargs->AB$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Connection Doesn't Exist (probably...)
	RETURN;
	};

    SELECTONE TCB->State OF
    SET
    [CS$Listen,CS$SYN_Sent,CS$Closing,CS$Time_Wait,CS$Last_Ack]:
	{
	TCP$KILL_P}ING_REQUESTS(TCB,NET$_CR); // Error: connection Reset
 	TCB$Delete(TCB);	// Connection RESET.
	};

    [CS$SYN_RECV,CS$Established,CS$Fin_Wait_1,CS$Fin_Wait_2,CS$Close_Wait]:
	{
	Send_RST(TCB);
	TCP$KILL_P}ING_REQUESTS(TCB,NET$_CR);
	TCB$Delete(TCB);
	};
    TES;

    USER$Post_Function_OK(Uargs);	// okay
    };


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

TCP$Deliver_User_Data(struct tcb_structure * tcb): NOVALUE (void)
    {
    REGISTER
	struct Queue_BLK_Structure * UQB(QB_UR_Fields), // User rcv request queue.
	DataSize,		// Size of data written to users VAS.
	Usize,			// Size requested by user.
	Uadrs;			// Address of user's system buffer.
    signed long
	Uflags,			// Flag bits to return to user
	Args: VECTOR->4;	// CMKRNL arg blk.

    UQB = TCB->UR_Qhead;	// Point at User receive request list.

// Process User receive requests (UR_Qhead) until all have been processed
// or all the user-data bearing network segments have been exhausted.

    WHILE (UQB NEQA TCB->UR_Qhead) && (TCB->RCV_Q_Count > 0) DO
	{
	Uflags = 0;
	if (TCB->RCV_Push_Flag)
	    Uflags = Uflags || NSB$PUSHBIT;
	Usize = UQB->UR$Size;	// Size of data requested.
	Uadrs = UQB->UR$Data;	// User's System buffer address.
	Datasize = MIN(Usize,TCB->RCV_Q_Count);

// Indicate that we are doing this

	if ($$LOGF(LOG$USER))
	    {
	    EXTERNAL ROUTINE
		ASCII_HEX_BYTES : NOVALUE;
	    LITERAL
		maxhex = 20,
		maxasc = 50;
	    signed long
		DESC$STR_ALLOC(hexstr,maxhex*3),
		nhex,nasc;
	    nhex = MIN(datasize,maxhex);
	    nasc = MIN(datasize,maxasc);
	    ASCII_Hex_Bytes(hexstr,nhex,TCB->RCV_Q_DEQP,
			    hexstr->DSC$W_LENGTH);
	    LOG$FAO(%STRING("!%T Deliver user data: DQ=!XL,EQ=!XL,RCQ=!XL/!XL,Size=!SL!/",
			    "!_HEX:   !AS!/",
			    "!_ASCII: !AF!/"),
		    0,TCB->RCV_Q_DEQP,TCB->RCV_Q_ENQP,TCB->RCV_Q_BASE,
		    TCB[RCV_Q_}],Datasize,hexstr,nasc,TCB->RCV_Q_DEQP);
	    };

// Dequeue from TCB queue into user buffer

	$$KCALL(CQ_Dequeue,TCB->RCV_Q_Queue,Uadrs,Datasize);
  
// Update user/TCB data pointers

	TCB->RCV_DUptr = TCB->RCV_DUptr + Datasize;
	if (TCB->RCV_Push_Flag)
	    if ((TCB->RCV_DUptr - TCB->RCV_Pptr) > 0)
		TCB->RCV_Push_Flag = FALSE;

// Post user IO request as complete & successful. Return # of bytes
// delivered to user.  Update the receive window as we now have more resources
// available for receiving segments.
// Remove user's IO request from the user_recv_request_q, release queueBlk and
// user_arg_blk.

	XLOG$FAO(LOG$USER,
		 "!%T Posting TCP$RECEIVE: Size=!SL,Flags=!XB,Uargs=!XL!/",
		 0,datasize,Uflags,UQB->UR$Uargs);

	// we want to REMQUE the QBlock here since we don't want it on
	// the rec queue after we post it (since it might be posted twice.
	REMQUE(TCB->UR_Qhead,UQB); // remove the entry

	// post the IRP and release the Uargs
	User$Post_IO_Status(UQB->UR$Uargs,SS$_Normal,DataSize,Uflags,0);
	MM$UArg_Free(UQB->UR$Uargs); // release user arg blk.

	ts$dbr = ts$dbr + DataSize; // total data bytes delivered to users.

// attempt to beat the silly window syndrome, should we give a window update?

	tcb->ack_size = tcb->ack_size + datasize;
	if (tcb->ack_size GEQU TCB->SND_ack_threshold)
	    {

 // Return window resources.

	    tcb->rcv_wnd = tcb->rcv_wnd + tcb->ack_size;
	    tcb->ack_size = 0;
	    };

// Only flag an ACK if something has really changed.

	IF (tcb->old_rcv_nxt NEQU tcb->rcv_nxt) OR
	   (tcb->old_rcv_wnd NEQU tcb->rcv_wnd) THEN
!	    tcb->pending_ack = True; // Indicate we need an ack sent.
	    TCP$Enqueue_Ack(TCB) ;	// Indicate we need an ack sent.
	MM$QBlk_Free(UQB);	// Release Queue blk.
	UQB = TCB->UR_Qhead;	// point at next user request blk.
	};	// End of "OUTER" While.
    };

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

Queue_Receive_Request(TCB,Uargs): NOVALUE (void)
    {
    MAP
	struct tcb_structure * tcb,
	struct User_RECV_Args * Uargs;
    REGISTER
	struct Queue_Blk_Structure * QB(QB_UR_Fields);

// Fill in Queue Blk from user argument blk.

    QB = MM$QBLK_Get();		// Get a Queue block structure.
    QB->UR$Size = Uargs->RE$Buf_Size; // # of data bytes requested.
    QB->UR$Data = Uargs->RE$Data_Start; // Adrs of system buffer data start.
    QB->UR$IRP_Adrs = Uargs->RE$IRP_Adrs; // IO request Packet address.
    QB->UR$UCB_Adrs = Uargs->RE$UCB_Adrs; // Unit Control Blk address.
    QB->UR$Uargs = Uargs;	// point at process-local user argblk.

// Add to end of user receive request list

    INSQUE(QB,TCB->UR_QTail);
    };

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

FORWARD ROUTINE
 void    TCP$Post_User_Close_IO_Status;

TCP$RECEIVE(struct User_RECV_Args * Uargs): NOVALUE (void)
    {
    signed long
	RC  = SS$_NORMAL;
    REGISTER
	struct tcb_structure * tcb;

    if ((TCB=tcb_ok(Uargs->RE$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Connection doesn't exist.
	RETURN;
	};

    XLOG$FAO(LOG$USER,"!%T TCP$RECEIVE: Conn=!XL, TCB=!XL, Size=!SL!/",
	     0,Uargs->RE$Local_Conn_ID,TCB,Uargs->RE$Buf_size);

    if (Uargs->RE$Buf_size <= 0)
	{
	USER$Err(Uargs,NET$_BTS);
	RETURN;
	};

    if (TCB->user_timeout != 0)
	TCB->User_Timeout = TCB->User_timeval + Time_stamp();

    SELECTONE TCB->State OF
    SET
    [CS$Listen,CS$SYN_Sent,CS$SYN_Recv]:
	Queue_Receive_request(TCB,Uargs);

    [CS$Established,CS$FIN_wait_1,CS$Fin_Wait_2]:
	{

// If network data available then deliver it to the user.

	Queue_Receive_Request(TCB,Uargs); // Must maintain FIFO queue.
	if (TCB->RCV_Q_Count > 0)
	    TCP$Deliver_User_Data(TCB);

// If FIN_RCVD set, we are in FIN-WAIT-2 waiting for pending data to be
// delivered. If the receive queue has become empty, finish closing the TCB now.

    	if (TCB->FIN_RCVD && (TCB->RCV_Q_COUNT == 0))
	    {
	    TCP$SET_TCB_STATE(TCB,CS$Time_Wait);
	    if (NOT TCB->Close_NoWait)
		{
		TCP$Post_User_Close_IO_Status(TCB,SS$_NORMAL);
		TCB->Time_Wait_Timer = Time_Stamp() + Max_Seg_LifeTime;
		}
	    else
		{
		TCP$KILL_P}ING_REQUESTS(TCB,NET$_TWT);
		XLOG$FAO(LOG$TCBSTATE,"!%T Time-Wait forced, conn=!XL!/",
			 0,TCB);
		TCB$Delete(TCB);
		}
	    };
	};

// Allow receives in Close_Wait state only if data available. The exception to
// this rule is that the first receive after all data has been delivered is
// allowed to succeed, with 0 bytes xferred, as an EOF marker.

    [CS$Close_Wait]:
	{
	if (TCB->RCV_Q_Count > 0)
	    {
	    Queue_Receive_Request(TCB,Uargs);
	    TCP$Deliver_User_Data(TCB);
	    }
	else
	    if (NOT TCB->EOF)
		{
		TCB->EOF = TRUE;
		User$Post_IO_Status(Uargs,SS$_NORMAL,0,
				    NSB$PUSHBIT || NSB$EOFBIT,0);
		MM$UArg_Free(Uargs); // cleanup.
		}
	    else
		USER$Err(Uargs,NET$_CC);
	};

    [CS$Time_Wait]:
	USER$Err(Uargs,NET$_CC);

    [CS$Closing,CS$Last_ACK]:
	USER$Err(Uargs,NET$_CC);	// Error: Connection Closing.
    TES;
    };

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



ISN_GEN=
    {
    OWN
	ISN: UNSIGNED;
    signed long
	RVAL;
    EXTERNAL ROUTINE
	USER$Clock_Base;

    ISN = ISN+1;
    RVAL = (USER$Clock_Base()+.ISN)^16;
    return RVAL;
    };

//SBTTL "Queue_Send_Data: Queue a data bearing send request."
/*

Function:

	Place a user's send request on the send queue.  Queue Block contains
	pointers to users buffer & send call arguments.  See field definition
	"QB_Send_Fields" for Queue Block structure.

Inputs:

	Uargs = user S} call argument block
	TCB = Current TCB
Outputs:

	none.

Side Effects:

	Queue block is placed at the end of the send queue.
*/

Queue_Send_Data(Uargs,TCB): NOVALUE (void)
    {
    EXTERNAL ROUTINE
	TCP$Send_Enqueue : NOVALUE;
    MAP
	struct User_Send_Args * Uargs,
	struct tcb_structure * tcb;
    signed long
	args : VECTOR->4,
	Uaddr,
	Ucount;


// Append as much user data as we can to the queue.

    Uaddr = Uargs->SE$Data_Start;
    Ucount = Uargs->SE$Buf_Size;
    if ((TCB->SND_Q_Count+TCB->SRX_Q_Count) LSS TCB->SND_Q_Size)
	{

// Enqueue as much as we can.

	TCP$Send_Enqueue(TCB,Ucount,Uaddr,Uargs->SE$EOL);
	if (Ucount == 0)
	    {

// If we got all of it, post request now, else remember remainder for later

	    User$Post_IO_Status(Uargs,SS$_NORMAL,0,0,0);
	    MM$UArg_Free(Uargs);
	    };
	};

// If there is still data left in this buffer, get & queue a send queue block

    if (Ucount > 0)
	{
	REGISTER
	    struct Queue_BLK_Structure * QB(QB_Send_Fields);
	QB = MM$QBLK_Get();
	QB->SN$Size = Ucount;	// user's buffer length
	QB->SN$Data = Uaddr;	// Point at start of data
	QB->SN$EOL = Uargs->SE$EOL;
	QB->SN$URG = Uargs->SE$URG;
	QB->SN$Uargs = Uargs;	// point at TCP user argument blk.

!~~~ This may not be used any more - check

	if (TCB->User_timeval != 0)
	    QB->SN$TimeOut = TCB->User_TimeVal + Time_Stamp()
	else
	    QB->SN$TimeOut = CONN_TIMEVAL + Time_Stamp();

	INSQUE(QB,TCB->SND_Qtail);
	};

// Flag we have user data which needs to be sent over the network.

    TCB->Data_2_Send = True;
    };

//SBTTL "TCP$S}: User call to send data over network"
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

	Uargs = User "S}" call argument block

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

TCP$S}(struct User_Send_Args * Uargs): NOVALUE (void)
    {
    signed long
	RC  = SS$_NORMAL;
    REGISTER
	struct tcb_structure * tcb;

    if ((TCB=TCB_OK(Uargs->SE$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Connection does not exist.
	RETURN;
	};

    XLOG$FAO(LOG$USER,"!%T TCP$S}: Conn=!XL, TCB=!XL, Size=!SL!/",
		0,Uargs->SE$Local_Conn_ID,TCB,Uargs->SE$Buf_size);

    if (TCB->User_timeout != 0)
	TCB->User_TimeOut = TCB->User_Timeval + Time_Stamp();

    SELECTONE TCB->State OF
    SET
    [CS$NAMELOOK]:
	USER$Err(Uargs,NET$_NOADR); // Connection not yet open

    [CS$Listen]:
	{
	if ((TCB->Foreign_Host == Wild) || (TCB->Foreign_Port == Wild))
	    USER$Err(Uargs,NET$_FSU) // Foreign Socket unspecified.
	else
	    {
	    TCB->Active_Open = True; // change connection  passive --> active
	    if (NOT Send_SYN(TCB)) // Try to initiate open...
		USER$Err(Uargs,NET$_NRT) // No route to host
	    else
		{
		Queue_Send_Data(Uargs,TCB); // Always have data.
		TCP$SET_TCB_STATE(TCB,CS$SYN_Sent);
		};
	    };
	};

    [CS$SYN_Sent,CS$SYN_RECV,CS$Close_Wait,CS$Established]:
	Queue_Send_Data(Uargs,TCB);

/*~~~~~ Don't do S}_DATA here - main processing loop can handle it
    [CS$Close_Wait,CS$Established]:
	{

// Queue the S} request to maintain the FIFO ordering.

	Queue_Send_Data(Uargs,TCB);
	Send_Data(TCB);
	};
~~~~~)%

    [CS$Fin_Wait_1,CS$Fin_Wait_2,CS$Time_Wait,CS$Closing,CS$Last_Ack]:
	USER$Err(Uargs,NET$_CC);	// Connection Closed.
    TES;
    };

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


TCP$CLOSE(struct User_Close_Args * Uargs): NOVALUE (void)
    {
    signed long
	RC  = SS$_NORMAL,
	XTCB;
    REGISTER
	struct tcb_structure * tcb;

    if ((TCB=TCB_OK(Uargs->CL$Local_Conn_ID,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Connection Does NOT exist.
	RETURN;
	};

    XLOG$FAO(LOG$USER,"!%T TCP$CLOSE: conn=!XL, TCB=!XL!/",
	     0,Uargs->CL$Local_Conn_ID,TCB);

// Set close mode

    TCB->Close_NoWait = Uargs->CL$NoWait;

// Initiate close at TCP level.

    XTCB = TCB;
    if ((RC=TCP$TCB_CLOSE(XTCB)) != 0)
	{
	USER$Err(Uargs,RC);	// Connection Closing.
	RETURN;
	};

// If this close can complete now, post user else remember Uargs for later.

    IF (XTCB == 0) || (TCB->Close_NoWait) || (TCB->IS_Aborted) OR
       (TCB->State == CS$Time_Wait) THEN
	USER$Post_Function_OK(Uargs)
    else
	{
	TCB->Pending_IO = True;
	TCB->Curr_User_Function = U$CLOSE;
	TCB->Function_Timer = Time_Stamp() + Close_Timeout;
	TCB->Argblk = Uargs;
	};
    };

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
	TCB->Pending_IO - indicates IO needs to be posted.
	TCB->Function_Timer - handler function timeout.
	TCB->ArgBlk - points at user's TCP argument block.

Outputs:

	None.

Side Effects:

	User's IO is posted to VMS IO posting routines.  Pending_IO bit is
	cleared.  User argument block is deallocated.
*/


TCP$Post_User_Close_IO_Status(struct tcb_structure * tcb,
					 VMS_RC): NOVALUE (void)
    {
    REGISTER
	struct User_Close_Args * Uargs;
    signed long
	IOSB: NetIO_status_Block;

// Check for pending_io flagged...

    if (TCB->Pending_IO)
	{
	TCB->Pending_IO = False;
	if (TCB->Argblk == 0)
	    RETURN;
	IOSB->NSB$BYTE_COUNT = 0;
	IOSB->NSB$STATUS = VMS_RC;
	IOSB->NSB$XSTATUS = 0;
	Uargs = TCB->ArgBlk;	// point at user's TCP arg block.
	IO$POST(IOSB,Uargs);
	MM$UArg_Free(Uargs);	// Free user arg blk memory.
	};
    };


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

TCP$TCB_Init(TCB) : NOVALUE (void)
    {
    MAP
	struct tcb_structure * tcb;

// Initialize segment space counters & user data assembly/disassembly
// ptrs & counters.

    TCB->Snd_Nxt = TCB->SND_UNA = TCB->ISS = ISN_GEN();
    TCB->Snd_Wnd = TCB->RCV_WND = Window_Default;
    TCB->SND_ACK_Threshold = ACK_Threshold ;
    TCB->TCB$Flags = False;	// clear all flags
//    TCB->max_seg_data_size = Default_Data_Size; // default: max data per seg
//   TCB->max_eff_data_size = Default_Data_Size; // default: max data per seg
    TCB->max_seg_data_size = Default_MSS ; // default: max data per seg
    TCB->max_eff_data_size = Default_MSS ; // default: max data per seg

// Init queue headers, Empty queue forward & back ptrs point at QHead element of
// queue header as defined by VAX-11 absolute queue instructions.

    TCB->LP_Next = TCB->LP_Back = TCB->LP_Next; // Local port list.
    TCB->RF_Qhead = TCB->RF_Qtail = TCB->RF_Qhead; // Received future segments.
    TCB->SND_QHead = TCB->SND_QTail = TCB->SND_QHead; // Segment send.
    TCB->UR_QHead = TCB->UR_QTail = TCB->UR_QHead; // user receive request.
    TCB->State = TCB->Last_State = CS$Closed; // Set a known Connection state.

// Initialize circular queue pointers. TCB$Create already set queue addresses.
// Note that the "retransmission queue" is a clone of the send queue, and only
// really has a DEQ pointer.

    TCB[SND_Q_}] = TCB->SND_Q_BASE + TCB->SND_Q_SIZE;
    TCB->SND_Q_DEQP = TCB->SND_Q_ENQP = TCB->SND_Q_BASE;
    TCB->SND_Q_Count = 0;
    TCB[SRX_Q_}] = TCB->SRX_Q_BASE + TCB->SRX_Q_SIZE;
    TCB->SRX_Q_DEQP = TCB->SRX_Q_ENQP = TCB->SRX_Q_BASE;
    TCB->SRX_Q_Count = 0;
    TCB[RCV_Q_}] = TCB->RCV_Q_BASE + TCB->RCV_Q_SIZE;
    TCB->RCV_Q_DEQP = TCB->RCV_Q_ENQP = TCB->RCV_Q_BASE;
    TCB->RCV_Q_Count = 0;
    TCB->RX_CTL = 0;
    TCB->RX_SEQ = 0;
    TCB->ACK_SIZE = 0;

// Set RX timer, initial RTT info, and the connection timeout.
// TCB->Connection_timeout is the actual active TCP connection timeout.
// It is updated when segments are seen arriving over the connection. If this
// timeout expires, the connection is assumed dead and is aborted.

    TCB->Connection_TimeOut = CONN_TIMEVAL + Time_Stamp();
    TCB->Round_Trip_Time = Base_RT_TimeOut; // base for round trip timer.
    TCB->Calculated_RTO = Base_RT_TimeOut; // Initial retransmission timer
    TCB->SND_Delay_Timer	=	Base_RT_TimeOut + Time_Stamp() ;
    TCB->Delayed_ACK_Timer	=	Base_RT_TimeOut + Time_Stamp() ;
    XLOG$FAO(LOG$TCP,"!%T TCB_INIT TCB !XL!/",0,TCB);
    };

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

FORWARD ROUTINE
 VOID    TCP_NMLOOK_DONE,
 void    TCP$Adlook_Done;

extern
 VOID    NML$GETALST,
 VOID    NML$GETNAME;

TCP$OPEN(struct User_OPEN_Args * Uargs): NOVALUE (void)
    {
    REGISTER
	struct IPADR$Address_Block * ProtoHdr,
	struct tcb_structure * tcb;
    signed long
	Args : VECTOR->4,
	HostWild,
	Ucbptr,
	LP,
	FP,
	IPADDR;
    LABEL
	X;

    XLOG$FAO(LOG$USER,"!%T TCP$OPEN: PID=!XL,CHAN=!XL,FLAGS=!XL!/",
	     0,Uargs->OP$PID,Uargs->OP$PIOChan,Uargs->OP$FLAGS);

    ProtoHdr = Uargs->OP$ProtoHdrBLK;

// Handle easy errors before creating TCB, etc...

    HostWild = (NOT Uargs->OP$ADDR_FLAG) && 
	       (CH$RCHAR(CH$PTR(Uargs->OP$Foreign_Host)) == WILD);
    if (Uargs->OP$Active_Open)
	{			// Active case

// For active open, wildcard foreign host or foreign port makes no sense.

	if (HostWild || (ProtoHdr->IPADR$Dst_Port == Wild))
	    {
	    USER$Err(Uargs,NET$_FSU);
	    RETURN;
	    };
	}
    else
	{			// Passive case

// For passive open, wildcard local port makes no sense...

	if (ProtoHdr->IPADR$Src_Port == WILD)
	    {
	    USER$Err(Uargs,NET$_ILP);
	    RETURN;
	    };
	};

// Create a TCB (Transmission Control Blk) for this connection

    if ((TCB = TCB$Create()) == Error)
	{
	USER$Err(Uargs,NET$_UCT);// Error: Unable to create TCB.
	RETURN;			// punt
	};

// Initialize the TCB

    TCP$TCB_Init(TCB);

// Copy user args to TCB
// If we have a wild(0) Foreign_Host then don't bother to try and decode
// the asciz Foreign-Host-Name string. Likewise, we defer specification of
// the Local_Host field until a packet is received. Note that this means
// that wildcarding cannot work on active connections. Note that we allow
// the upper-level routine to determine this, so it can give a reasonable
// error code to the user.

    TCB->Active_OPEN = Uargs->OP$Active_Open;
    TCB->Open_NoWait = Uargs->OP$NoWait;
    LP = ProtoHdr->IPADR$Src_Port ;
    FP = ProtoHdr->IPADR$Dst_Port ;
    TCB->Local_Port = (LP && %X"FFFF") ;
    TCB->Foreign_Port = (FP && %X"FFFF") ;
    TCB->UCB_ADRS = Uargs->OP$UCB_Adrs;

// Set user specified connection time-out in seconds.
// TCB->User_timeout is a timeout for idle connections at the user level.
!	It is updated whenever the user touches the connection or whenever
!	user data is received. If op$timeout is unspecified, no user timeout
!	handling is performed, otherwise this value is saved in the cell
!	TCB->user_timeval and is used to compute this idle timeout.

    TCB->User_Timeval = Uargs->op$timeout*csec;
    if (TCB->User_Timeval != 0)
	TCB->User_timeout = TCB->User_timeval + Time_Stamp()
    else
	TCB->User_timeout = 0;
// Reset the TCP connection timeout for passive open case
    if (NOT TCB->Active_OPEN)
        TCB->Connection_TimeOut = Time_Stamp() + Passive_OPEN_TimeOut;
    TCB->User_ID = Uargs->OP$PID;
    TCB->Process_IO_Chan = Uargs->OP$PIOchan; // user's IO channel.

// Setup TCB ID in UCB - move 4-byte TCB index into system UCB

    ucbptr = Uargs->OP$UCB_Adrs + UCB$L_CBID;
    $$KCALL(MOVBYT,4,TCB->VTCB_INDEX,ucbptr);

// Remember Uargs for TCP_NMLOOK_DONE

    TCB->ArgBlk = Uargs;

// If wildcard foreign name, then done...

    if (HostWild)
	{
	TCB->Foreign_Host = Wild;
!	TCB->Local_Host = Wild;
        TCB->Pending_IO = TRUE;                 ![VU] Satisfy new validity test
    	TCP_NMLOOK_DONE(TCB,SS$_NORMAL,0,0,0,0);
	RETURN;
	};

// Check for user open with IP address.

X:  {			// *** Block X ***
    if (Uargs->OP$Addr_Flag)
	IPADDR = Uargs->OP$Foreign_Address
    else
	{
	signed long
	    NAMPTR;
	NAMPTR = CH$PTR(Uargs->OP$Foreign_Host);
	if (GET_IP_ADDR(NAMPTR,IPADDR) LSS 0)
	    LEAVE X;
        };

// Have an IP address - finish the open now & start address translation

    TCB->Foreign_Hnlen = 0;
    TCB->Pending_IO = TRUE;                 ![VU] Satisfy new validity test
    TCP_NMLOOK_DONE(TCB,SS$_NORMAL,1,IPADDR,0,0);
    TCB->NMlook_Flag = TRUE;
    NML$GETNAME(IPADDR,TCP$Adlook_Done,TCB);
    RETURN;
    };			// *** Block X ***

// Have a host name. Start name lookup which will finish the open when done.

    TCB->State = CS$NAMELOOK;
    TCB->Pending_IO = TRUE;
    TCB->Curr_User_Function = U$OPEN;
    TCB->Function_Timer = Time_Stamp() + NameLook_Timeout;
    TCB->NMLook_Flag = TRUE;
    NML$GETALST(CH$PTR(Uargs->OP$Foreign_Host),Uargs->OP$Foreign_Hlen,
		TCP_NMLOOK_DONE,TCB);
    };

//SBTTL "TCP_NMLOOK_DONE - Come here when name lookup for TCB finishes"
/*
    Here is where we finish up the open.
*/


TCP_NMLOOK_DONE(TCB,STATUS,ADRCNT,ADRLST,NAMLEN,NAMPTR) : NOVALUE (void)
    {
    MAP
	STRUCT TCB_STRUCTURE * TCB;
    signed long
	ISWILD,
	struct User_Open_Args * Uargs,
	OpenErr,
	LP,
        Count,
	CN_Index,
	IOSB: NetIO_Status_Block,
	OK;

   ![VU] Insure TCB valid before modifying.  This protects against case
   ![VU] where Service_Connections first times-out pending I/O functions
   ![VU] and later times-out and deletes the resulting inactive connection.

    if (NOT VTCB_Indx_OK(TCB->VTCB_Index))
	{
	WARN$FAO ("!%T VTCB Index out of range (!XL)",0,TCB->VTCB_Index);
        RETURN
	};

// Clear name lookup flag and pending name lookup I/O

    NOINT;
    TCB->NMLook_Flag = FALSE;
    OK = TCB [Pending_IO];     ![VU] Save for validity check.
    TCB->Pending_IO = FALSE;
    Uargs = TCB->ARGBLK;
    TCB->ARGBLK = 0;
    OKINT;

// If the name lookup failed, give the error now and delete the TCB

    if (NOT STATUS)
	{
	USER$Err(Uargs,STATUS);
	TCB$Delete(TCB);
	RETURN;
	};

    ![VU] Test validity of ARGBLK and Pending_IO to avoid system crashes, etc.
    ![VU] In each case, we assume that Service_Connections will clean up anythin
    ![VU] that it has not already.
    ![VU] All callers of TCP_NMLOOK_DONE now set Pending_IO as well as ARGBLK
    ![VU] to support this validity test.  This should be safe since new sites
    ![VU] which set Pending_IO call TCP_NMLOOK_DONE immediately, which clears
    ![VU] the flag.
    if (OK == 0) RETURN;           ![VU] No pending I/O
    if (Uargs == 0) RETURN;        ![VU] No ARGBLK

// Name lookup succeded. Set the host numbers and host name.

    if (ADRLST != 0)
	{
	IP$SET_HOSTS(ADRCNT,ADRLST,TCB->Local_Host,TCB->Foreign_host);
	TCB->Foreign_Hnlen = NAMLEN;
	if (NAMLEN != 0)
	    CH$MOVE(NAMLEN,NAMPTR,CH$PTR(TCB->Foreign_Hname));
	};

// Finish opening the connection

    OK = OpenErr = False;

// Check access to the specified hosts/ports

    OK = USER$CHECK_ACCESS(TCB->USER_ID,TCB->Local_Host,TCB->Local_Port,
		      TCB->Foreign_Host,TCB->Foreign_Port);
    if (NOT OK)
	{
	TCB$Delete(TCB);
	USER$Err(Uargs,OK);
	RETURN;
	};

// Set current function for timeout

    TCB->Curr_User_Function = U$OPEN;
    NOINT;
    if (TCB->Active_Open)
	{

// Process an "ACTIVE" open.

	if ((TCB->Local_Port != Wild))
	    {		// user specified local port?

// make sure it's a unique connection request.
// Is connection table full? || a Non-unique connection?
// Either case report the error.

	    XLOG$FAO(LOG$USER,
		     "!%T ACTIVE Open, LP = !XL (!UL), FP = !XL (!UL)!/",
		     0,tcb->local_port,tcb->local_port,
		     tcb->foreign_port,tcb->foreign_port);

	    OK =Check_Unique_Conn(TCB->Local_Port, TCB->Foreign_Host,
				   TCB->Foreign_Port, CN_Index);
	    if (OK == Error)
		{		// Error: Connection Tbl space Exhausted.
		TCB$Delete(TCB);
		USER$Err(Uargs,NET$_CSE);
		OKINT;
		RETURN;
		}
	    else
		if (NOT OK)
		    {	// Error: Non-Unique Connection
		    TCB$Delete(TCB);
		    USER$Err(Uargs,NET$_NUC);
		    OKINT;
		    RETURN;
		    }
	    }
	else
	    {		// Active OPEN Unspecified LP, allocate one.

// Allocate a user local port & check if the connection will be unique.
// Loop until an allocated Local-port & rest of connection is unique.

	    XLOG$FAO(LOG$USER,
		     "!%T ACTIVE Open, wild LP, FP = !XL (!UL)!/",
		     0,tcb->foreign_port,tcb->foreign_port);
	    REPEAT
		{
		LP = USER$GET_LOCAL_PORT(TCP_User_LP);
		OK =Check_Unique_Conn(LP,TCB->Foreign_Host,
				       TCB->Foreign_Port,CN_Index);
 		if (OK == Error)
		    {	// Connection table Space Exhausted
		    TCB$Delete(TCB);
		    USER$Err(Uargs,NET$_CSE);
		    OKINT;
		    RETURN;
		    }
		}
	    UNTIL (ok);
	    TCB->Local_Port = (LP && %X"FFFF") ;
	    };

// Set initial state for active open

	if (NOT Send_SYN(TCB)) // Try to start 3-way handshake
	    {
	    TCB$Delete(TCB);
	    USER$Err(Uargs,NET$_NRT); // No route to host
	    OKINT;
	    RETURN;
	    };
	TCP$SET_TCB_STATE(TCB,CS$SYN_Sent);

// Set function timer for active open timeout

	TCB->Function_Timer = Time_Stamp() + Active_OPEN_TimeOut;
	ts$aco = ts$aco + 1;
	}
    else			// Passive Open
	{
	XLOG$FAO(LOG$USER,
		 "!%T Passive open on LP=!XL (!UL), FP=!XL (!UL)!/",
		 0,TCB->Local_Port,TCB->Local_Port,
		 TCB->Foreign_Port,TCB->Foreign_Port);

// Check for uniqueness of host/port set. This has the side effect of obtaining
// a slot for us in the connection table.

	ISWILD=(TCB->Foreign_Port == WILD) || (TCB->Foreign_Host == WILD);

	OK =Check_Unique_Conn(TCB->Local_Port,TCB->Foreign_Host,
			       TCB->Foreign_Port,CN_Index);

// OK = Error means that the connection table was full - punt.

	if (OK == Error)
	    {
	    TCB$Delete(TCB);
	    USER$Err(Uargs,NET$_CSE);
	    OKINT;
	    RETURN;
	    };

// OK = FALSE means the connection was non-unique. This is NOT an error if the
// foreign host or port is wildcarded.

	if ((NOT OK) && (NOT ISWILD))
	    {
	    TCB$Delete(TCB);
	    USER$Err(Uargs,NET$_NUC);
	    OKINT;
	    RETURN;
	    };

// Set initial state for Passive Open

	TCP$SET_TCB_STATE(TCB,CS$Listen);

// Indicate which function we're timing.

	TCB->Function_Timer = Time_Stamp() + Passive_OPEN_TimeOut;
	ts$pco = ts$pco + 1; // count passive opens.
	};

// Save connection table index and insert into the connection table

    TCB->Con_Index = CN_Index;
    Conect_Insert(TCB,CN_Index);
    OKINT;
	
// Log successful open

    XLOG$FAO(LOG$USER,"!%T TCP$OPEN: Conn idx = !XL, TCB = !XL!/",
	     0,TCB->VTCB_INDEX,TCB);

// Check for immediate-return mode

    if (tcb->Open_NoWait)
	{

// Return Local-Connection-ID (Actually the TCB's address).
// 2nd fullword of the user's Net IO status block is the Local conn ID.

	IOSB->NSB$STATUS = SS$_Normal; // indicate success.
	IOSB->NSB$Byte_Count = 0;
	IOSB->NSB$XSTATUS = 0;
	IO$POST(IOSB,Uargs); // Tell em.
	MM$UArg_Free(Uargs);	// Release user arg blk.
	}
    else
	{

// He wants to wait. Set up for I/O posting in SEGIN when connection becomes
// established.

	tcb->Pending_IO = True;	// indicate we have IO to post later.
	tcb->ArgBlk = Uargs;	// save pointer to user argument blk.
	};

// Check SYN wait list for passive connection.
// It is important that this step be done last, since the pending I/O
// information must be set up for the callback to Post_User_Active_Open.

    if (NOT TCB->Active_Open)
	SEG$Check_SYN_Wait_List(TCB);
    };

//SBTTL "TCP$Adlook_Done - Handle completion of address to name lookup"

TCP$ADLOOK_DONE(TCB,STATUS,NAMLEN,NAMPTR) : NOVALUE (void)
    {
    MAP
	struct tcb_structure * tcb;
    LABEL
	X;

// Check status

    if (NOT STATUS)
	RETURN;

// Make sure the TCB is still valid
    if ( NOT VTCB_Indx_OK (TCB->VTCB_Index))
	{
	XLOG$FAO(LOG$TCPERR,"!%T ADLOOK-DONE for nonexist. TCB !XL!/",0,TCB);
	RETURN
	};

// Make sure it is really waiting for this to happen...

    if (NOT TCB->NMLook_Flag)
	{
	XLOG$FAO(LOG$TCPERR,"!%T ADLOOK-DONE unexpected for TCB !XL!/",0,TCB);
	RETURN;
	};

// Clear name lookup flag

    TCB->NMLook_Flag = FALSE;

// Copy the host name into the TCB

    TCB->Foreign_Hnlen = NAMLEN;
    CH$MOVE(NAMLEN,NAMPTR,CH$PTR(TCB->Foreign_Hname));
    };

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
	TCP_RC = TCP Error code if NOT (VMS_RC = SS$_Normal).

Implicit Inputs:

	Assumed this was an ACTIVE open (ie, TCB->Active_OPEN = True).
	TCB->Pending_IO - indicates IO needs to be posted.
	TCB->Function_Timer - handler function timeout.
	TCB->ArgBlk - points at user's TCP argument block.

Outputs:

	None.

Side Effects:

	User's IO is posted to VMS IO posting routines.  Pending_IO bit is
	cleared.  User argument block is deallocated.
*/


TCP$Post_Active_Open_IO_Status(struct tcb_structure * tcb,
					  VMS_RC): NOVALUE (void)
    {
    REGISTER
	struct User_OPEN_Args * Uargs;
    signed long
	IOSB: NetIO_status_Block;

    if (TCB->Pending_IO)
	{
	TCB->Pending_IO = False;
	if (TCB->ArgBlk != 0)
	    {
	    IOSB->NSB$STATUS = VMS_RC;
	    IOSB->NSB$Byte_Count = 0;
	    IOSB->NSB$XSTATUS = 0;
	    Uargs = TCB->ArgBlk; // point at user's TCP arg block.
	    IO$POST(IOSB,Uargs);
	    MM$UArg_Free(Uargs);	// Free user arg blk memory.
	    };
	};
    };
}
ELUDOM
